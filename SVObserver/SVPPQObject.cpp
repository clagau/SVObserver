//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPPQObject
//* .File Name       : $Workfile:   SVPPQObject.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.36  $
//* .Check In Date   : $Date:   19 Jan 2015 11:42:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "MonitorListAttributeStruct.h"
#include "RootObject.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVGlobal.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVToolSet.h"
#include "SVVisionProcessorHelper.h"
#include "Definitions/Color.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "Definitions/SVGigeEnums.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "InspectionEngine/SVDigitizerProcessingClass.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMessage/SVMessage.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "Triggering/ResultData.h"
#include "Triggering/SVTriggerClass.h"
#include "Triggering/SVTriggerInfoStruct.h"
#include "Triggering/SVTriggerObject.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "SVLogLibrary/Logging.h"


#pragma endregion Includes
//#define TRACE_PPQ
#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr double TwentyPercent = .20;
constexpr DWORD cDefaultObjectIdCount {1};
bool SVPPQObject::m_timerResolution {false};
#pragma endregion Declarations

void SVPPQObject::ProcessOutputs(SVProductInfoStruct& rProduct)
{
	if (false == rProduct.m_dataComplete)
	{
		SetProductIncomplete(rProduct);
		if (cDefaultObjectIdCount < m_objectIdCount)
		{
			for (DWORD i = 0; i < m_objectIdCount; ++i)
			{
				WriteOutputs(&rProduct, i);
			}
		}
	}
	bool done {true};
	if (cDefaultObjectIdCount == m_objectIdCount)
	{
		constexpr int cDefaultObjectIdIndex {0};
		done = WriteOutputs(&rProduct, cDefaultObjectIdIndex);
	}
	if (done)
	{
		rProduct.m_outputsInfo.m_EndProcess = SvUl::GetTimeStamp();

		if (0 < m_resetDelay)
		{
			rProduct.m_outputsInfo.m_EndResetDelay = rProduct.m_outputsInfo.m_EndProcess + m_resetDelay;

			m_oOutputsResetQueue.AddTail(rProduct.triggerCount());
			m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::ResetOutputDelay]));
		}
		SetProductComplete(rProduct);
	}
}

#pragma region Constructor
SVPPQObject::SVPPQObject(LPCSTR ObjectName) : SVObjectClass(ObjectName)
{
	init();
}

SVPPQObject::SVPPQObject(SVObjectClass* POwner, int StringResourceID) : SVObjectClass(POwner, StringResourceID)
{
	init();
}
#pragma endregion Constructor

SVPPQObject::~SVPPQObject()
{
	SVObjectManagerClass::Instance().ClearShortPPQIndicator();
	m_TrcResetSubscriptionRAII.reset();

	if (m_isCreated)
	{
		Destroy();
	}

	m_pTriggerToggle.reset();
	m_pOutputToggle.reset();
	m_pDataValid.reset();
}

void SVPPQObject::init()
{
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVPPQObjectType;
	m_DataValidDelay = TheSVObserverApp().getDataValidDelay();

	for (auto& rOutput : m_PpqOutputs)
	{
		rOutput.SetObjectAttributesAllowed(SvPb::useable, SvOi::SetAttributeType::OverwriteAttribute);
		m_childObjects.push_back(&rOutput);
	}

	auto InitPPQCounter = [&](SvVol::BasicValueObjectPtr& counter, LPCSTR CounterName)
	{
		counter = m_PpqValues.setValueObject(CounterName, 0L, this);
		if (nullptr != counter)
		{
			//PPQ Trigger Count should never be used as it is not synchronous to the inspection rather use the Tool Set.Trigger Count
			counter->SetObjectAttributesAllowed(SvPb::useable, SvOi::SetAttributeType::RemoveAttribute);
			m_childObjects.push_back(counter.get());
		}
	};
	InitPPQCounter(m_spNotCompleteCount, SvDef::FqnPpqNotCompleteCount);
	InitPPQCounter(m_spMissingImageCount, SvDef::FqnPpqMissingImageCount);
	InitPPQCounter(m_spTiggercount, SvDef::FqnPpqTriggerCount);

	m_spPpqLength = m_PpqValues.setValueObject(SvDef::FqnPpqLength, cStandardPpqLength, this);
	SVObjectManagerClass::Instance().IncrementShortPPQIndicator();
	if (nullptr != m_spPpqLength)
	{
		m_childObjects.push_back(m_spPpqLength.get());
	}
	m_cameraCallback = [this](ULONG_PTR pCaller, const CameraInfo& rCameraInfo) { cameraCallback(pCaller, rCameraInfo); };
	m_triggerCallback = [this](SvTrig::SVTriggerInfoStruct&& triggerInfo) { triggerCallback(std::move(triggerInfo)); };

	auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
	if (nullptr != pTrc)
	{
		m_TrcResetSubscriptionRAII = pTrc->registerResetCallback([this]()
		{
			for (auto* pIP : m_arInspections)
			{
				if (nullptr != pIP)
				{
					pIP->resetLastProduct();
				}
			}
			m_storeForInterestMap.clear();
		});
	}
}

HRESULT SVPPQObject::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	HRESULT l_Status = SVObjectClass::GetChildObject(rpObject, rNameInfo, Index);

	if (S_OK != l_Status)
	{
		if (static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName())
		{
			if (static_cast<const size_t> (Index + 1) == (rNameInfo.m_NameArray.size() - 1))
			{
				SVObjectPtrVector::const_iterator iter = m_childObjects.cbegin();
				while (iter != m_childObjects.cend() && nullptr == rpObject)
				{
					const SVObjectClass* pObject = *iter;
					l_Status = pObject->GetChildObject(rpObject, rNameInfo, Index + 1);
					++iter;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ObserverUpdate(const SVInspectionInfoPair& rData)
{
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
	::OutputDebugString(std::format(_T("{} Inspection {} completed TRI={:d} State={:x}\n"), GetName(), rData.second.m_pInspection->GetName(), rData.first, static_cast<int> (rData.second.m_InspectedState)).c_str());
#endif
	m_lastPPQPosition = m_PPQPositions.GetIndexByTriggerCount(rData.first);

	m_oInspectionQueue.AddTail(rData);
	m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::CompleteInspections]));

	return S_OK;
}

bool SVPPQObject::Create()
{
	// Return if already created
	if (m_isCreated) { return false; }

	m_isCreated = true;
	Rebuild();

	m_TriggerToggle = false;
	m_OutputToggle = false;

	m_processFunctions[PpqFunction::Trigger] = [this]() {ProcessTrigger(); };
	m_processFunctions[PpqFunction::DataValidDelay] = [this]() {ProcessDataValidDelay(); };
	m_processFunctions[PpqFunction::ResetOutputDelay] = [this]() {ProcessResetOutputs(); };
	m_processFunctions[PpqFunction::DelayOutputs] = [this]() {ProcessDelayOutputs(); };
	m_processFunctions[PpqFunction::CameraResponses] = [this]() {ProcessCameraResponses(); };
	m_processFunctions[PpqFunction::NotifyInspection] = [this]() {ProcessNotifyInspections(); };
	m_processFunctions[PpqFunction::CompleteInspections] = [this]() {ProcessCompleteInspections(); };
	m_processFunctions[PpqFunction::ProductRequests] = [this]() {ProcessProductRequests(); };

	return m_isCreated;
}

bool SVPPQObject::Rebuild()
{
	// Return if not created
	if (!m_isCreated) { return false; }

	m_PPQPositions.clear();
	m_notifyInspectionList.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete[] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	// Create buckets for the PPQ positions
	m_PPQPositions.resize(getPPQLength());

	bool result = SetupProductInfoStructs();

	calcUseProcessingOffset4InterestFlag();

	std::set<DWORD> objectIDNumber;
	// Force the Inspections to rebuild as well
	for (auto pInspection : m_arInspections)
	{
		pInspection->RebuildInspection(false);
		objectIDNumber.insert(pInspection->GetObjectIdIndex());
	}
	m_objectIdCount = static_cast<DWORD> (objectIDNumber.size());

	return result;
}// end Rebuild

void SVPPQObject::Destroy()
{
	// Return if not created
	if (false == m_isCreated) { return; }

	m_oTriggerQueue.clear();

	while (0 != m_ProductRequests.GetCount())
	{
		ProcessProductRequests();
	}

	m_notifyInspectionList.clear();

	if (!(m_PendingCameraResponses.empty()))
	{
		SVObjectManagerClass::Instance().AdjustPendingImageIndicator(-(static_cast<long>(m_PendingCameraResponses.size())));

		m_PendingCameraResponses.clear();
	}

	// Delete buckets for the PPQ positions
	m_PPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete[] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	m_UsedInputs.clear();
	m_UsedOutputs.clear();

	m_isCreated = false;
}// end Destroy

void SVPPQObject::DetachAll()
{
	DetachTrigger(m_pTrigger);

	SvIe::SVVirtualCameraPtrVector cameraVector = GetVirtualCameras();

	for (auto* pCamera : cameraVector)
	{
		DetachCamera(pCamera);
	}

	int iSize = static_cast<int> (m_arInspections.size());
	for (int i = iSize - 1; i >= 0; i--)
	{
		DetachInspection(m_arInspections[i]);
	}
}

void SVPPQObject::SetPPQOutputMode(SvDef::SVPPQOutputModeEnum oPPQOutputMode)
{
	m_outputMode = oPPQOutputMode;
}

void SVPPQObject::SetOutputDelay(long lDelayTime)
{
	m_outputDelay = lDelayTime;
}

void SVPPQObject::SetResetDelay(long lResetTime)
{
	m_resetDelay = lResetTime;
}

void SVPPQObject::SetPPQLength(long lPPQLength)
{
	long oldPPQLength = getPPQLength();
	if (oldPPQLength != lPPQLength)
	{
		if (2 < oldPPQLength && lPPQLength < 3)
		{
			SVObjectManagerClass::Instance().IncrementShortPPQIndicator();
		}
		else if (2 < lPPQLength && oldPPQLength < 3)
		{
			SVObjectManagerClass::Instance().DecrementShortPPQIndicator();
		}
	}

	m_spPpqLength->setValue(lPPQLength);
	if (getPPQLength() != m_PPQPositions.size())
	{
		Rebuild();
	}

	int ppqLength = getPPQLength();
	for (auto& cameraPair : m_Cameras)
	{
		if (nullptr != cameraPair.first)
		{
			cameraPair.first->addNeededBuffer(getObjectId(), ppqLength);
		}
	}
}

void SVPPQObject::SetInspectionTimeout(long lTimeoutMillisec)
{
	m_inspectionTimeoutMillisec = lTimeoutMillisec;
}

long SVPPQObject::getPPQLength() const
{
	long length = 0;
	if (m_spPpqLength == nullptr)
	{
		m_spPpqLength = m_PpqValues.getValueObject(SvDef::FqnPpqLength);
	}
	if (nullptr != m_spPpqLength)
	{
		m_spPpqLength->getValue(length);
	}
	return length;
}

bool SVPPQObject::AttachTrigger(SvTrig::SVTriggerObject* pTrigger)
{
	if (nullptr == pTrigger)
	{
		return false;
	}
	m_pTrigger = pTrigger;

	m_pTrigger->SetObjectOwner(this);

	return m_pTrigger->RegisterCallback(m_triggerCallback);
}

bool SVPPQObject::AttachCamera(SvIe::SVVirtualCamera* pCamera, long lPosition, bool p_AllowMinusOne)
{
	bool l_bOk = true;

	if (nullptr == pCamera) { return false; }

	if (!p_AllowMinusOne && lPosition < 0) { lPosition = 0; }

	SVCameraInfoMap::iterator l_Iter = m_Cameras.find(pCamera);

	if (l_Iter != m_Cameras.end())
	{
		l_Iter->second.m_CameraPPQIndex = lPosition;
	}
	else
	{
		m_Cameras[pCamera].m_CameraPPQIndex = lPosition;
		m_Cameras[pCamera].m_ToggleState = true;

		RebuildProductCameraInfoStructs();

		l_bOk &= pCamera->RegisterCallback(reinterpret_cast<ULONG_PTR> (this), m_cameraCallback);
		pCamera->addNeededBuffer(getObjectId(), getPPQLength());
	}

	return l_bOk;
}

bool SVPPQObject::AttachInspection(SVInspectionProcess* pInspection)
{
	if (nullptr == pInspection) { return false; }

	m_arInspections.push_back(pInspection);

	pInspection->SetPPQIdentifier(getObjectId());

	return true;
}

bool SVPPQObject::DetachTrigger(SvTrig::SVTriggerObject* pTrigger)
{
	bool bOk = nullptr != m_pTrigger && m_pTrigger == pTrigger;

	if (bOk)
	{
		m_pTrigger->SetObjectOwner(nullptr);
		bOk = m_pTrigger->UnregisterCallback();

		m_pTrigger = nullptr;
	}

	return bOk;
}// end DetachTrigger

bool SVPPQObject::DetachCamera(SvIe::SVVirtualCamera* pCamera, bool bRemoveDepends/*=false*/)
{
	if (nullptr == pCamera) { return false; }

	bool l_Status = true;

	l_Status &= pCamera->UnregisterCallback(reinterpret_cast<ULONG_PTR> (this));
	pCamera->removeNeededBufferEntry(getObjectId());

	SVCameraInfoMap::iterator l_svIter = m_Cameras.find(pCamera);

	if (l_svIter != m_Cameras.end())
	{
		m_Cameras.erase(l_svIter);
	}

	//check inspections and remove camera from them also...
	if (bRemoveDepends)
	{
		for (auto pInspection : m_arInspections)
		{
			if (nullptr != pInspection) { pInspection->RemoveCamera(pCamera->GetName()); }
		}
	}

	RebuildProductCameraInfoStructs();

	return l_Status;
}// end DetachCamera

bool SVPPQObject::DetachInspection(SVInspectionProcess* pInspection)
{
	if (nullptr == pInspection) { return false; }

	int i;
	bool bFound = false;
	int iSize = static_cast<int> (m_arInspections.size());
	for (i = 0; i < iSize; i++)
	{
		if (pInspection == m_arInspections[i])
		{
			bFound = true;
			break;
		}// end if
	}// end for

	if (!bFound) { return false; }

	pInspection->SetPPQIdentifier(SvDef::InvalidObjectId);

	m_arInspections[i]->resetLastProduct();

	m_arInspections.erase(m_arInspections.begin() + i);

	return true;
}// end DetachInspection

bool SVPPQObject::AddSharedCamera(SvIe::SVVirtualCamera* pCamera)
{
	bool l_Status = (nullptr != pCamera);

	if (l_Status)
	{
		SVCameraInfoMap::iterator l_svIter = m_Cameras.find(pCamera);

		l_Status = (l_svIter != m_Cameras.end());

		if (l_Status && l_svIter->second.m_CameraPPQIndex == -1)
		{
			l_svIter->second.m_CameraPPQIndex = 0;
		}
	}

	if (!l_Status)
	{
		l_Status = AttachCamera(pCamera, 0);

		Rebuild();
	}

	return l_Status;
}

long SVPPQObject::GetInspectionCount() const
{
	return static_cast<long> (m_arInspections.size());
}

size_t SVPPQObject::GetCameraCount() const
{
	return m_Cameras.size();
}

SvIe::SVVirtualCameraPtrVector SVPPQObject::GetVirtualCameras(bool sortAndMakeUnique /*= false*/) const
{
	SvIe::SVVirtualCameraPtrVector cameraVector;

	for (const auto& rCamera : m_Cameras)
	{
		if (nullptr != rCamera.first)
		{
			cameraVector.emplace_back(rCamera.first);
		}
	}

	if (sortAndMakeUnique)
	{
		std::sort(cameraVector.begin(), cameraVector.end(), SvIe::isLessByName);
		//Remove duplicates
		cameraVector.erase(std::unique(cameraVector.begin(), cameraVector.end()), cameraVector.end());
	}

	return cameraVector;
}

bool SVPPQObject::GetInspection(long lIndex, SVInspectionProcess*& ppInspection) const
{
	if (lIndex < 0 || lIndex >= static_cast<long> (m_arInspections.size())) { return false; }

	ppInspection = m_arInspections[lIndex];

	return true;
}// end GetInspection

bool SVPPQObject::SetCameraPPQPosition(long lPosition, SvIe::SVVirtualCamera* pCamera)
{
	bool l_Status = true;

	//Only do an assert check so that in release mode no check is made
	Log_Assert(nullptr != pCamera);

	if (m_isCreated && -1 <= lPosition && lPosition < static_cast<long>(m_PPQPositions.size()) - 1)
	{
		l_Status = AttachCamera(pCamera, lPosition, true);
	}
	else
	{
		l_Status = false;
	}

	return l_Status;
}// end SetCameraPPQPosition

bool SVPPQObject::GetCameraPPQPosition(long& lPosition, const SvIe::SVVirtualCamera* const pCamera) const
{
	bool bFound;

	//Only do an assert check so that in release mode no check is made
	Log_Assert(nullptr != pCamera);

	lPosition = -1;

	if (!m_isCreated)
	{
		return false;
	}// end if

	//The std::map find function seems to have a problem when the pointer is const
	SVCameraInfoMap::const_iterator l_svIter = m_Cameras.find(const_cast<SvIe::SVVirtualCamera* const> (pCamera));

	bFound = l_svIter != m_Cameras.end();

	if (!bFound) { return false; }

	lPosition = l_svIter->second.m_CameraPPQIndex;

	return true;
}

void SVPPQObject::RebuildProductCameraInfoStructs()
{
	SvIe::SVObjectIdSVCameraInfoStructMap cameraInfos;

	BuildCameraInfos(cameraInfos);

	for (int j = 0; nullptr != m_pMasterProductInfos && j < getPPQLength() + cPPQExtraBufferSize; j++)
	{
		m_pMasterProductInfos[j].m_svCameraInfos = cameraInfos;
	}// end for
}

void SVPPQObject::PrepareGoOnline(bool setSoftwareTrigger)
{
	SvSml::SharedMemWriter::Instance().clearInspectionIdsVector(GetName());

	calcUseProcessingOffset4InterestFlag();

	if (false == m_pTrigger->CanGoOnline(setSoftwareTrigger))
	{
		SvDef::StringVector msgList;
		msgList.push_back(m_pTrigger->GetCompleteName());

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw Msg;
	}
	
	bool cameraSoftwareTrigger {false};
	RootObject::getRootChildValue(SvDef::FqnEnvironmentCameraSoftwareTrigger, cameraSoftwareTrigger);
	bool cameraFileAcquisition {false};
	RootObject::getRootChildValue(SvDef::FqnEnvironmentFileAcquisition, cameraFileAcquisition);
	setSoftwareTrigger |= cameraSoftwareTrigger;
	setSoftwareTrigger |= SvDef::TriggerType::SoftwareTrigger == m_pTrigger->getType();
	for (SVCameraInfoMap::iterator l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
	{
		l_svIter->first->setAcquisitionDevice(cameraFileAcquisition);
		if (setSoftwareTrigger)
		{
			_variant_t value {static_cast<long> (SvDef::TriggerType::SoftwareTrigger)};
			l_svIter->first->setParameterValue(SvDef::SVTriggerType, value);
		}
		if (false == (l_svIter->second.m_CameraPPQIndex >= 0 && l_svIter->first->CanGoOnline()))
		{
			SvDef::StringVector msgList;
			msgList.push_back(l_svIter->first->GetCompleteName());

			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams));
			throw Msg;
		}
	}
	AttachAcqToTriggers(setSoftwareTrigger);

	// Reset the PPQ
	for (size_t i = 0; i < m_PPQPositions.size(); ++i)
	{
		SVProductInfoStruct* l_pProduct = m_PPQPositions.GetProductAt(i);
		if (nullptr != l_pProduct)
		{
			if (!RecycleProductInfo(l_pProduct))
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams));
				throw Msg;
			}

			m_PPQPositions.SetProductAt(i, nullptr);
		}

		if (m_qAvailableProductInfos.RemoveHead(&l_pProduct) && nullptr != l_pProduct)
		{
			HRESULT hRTemp = m_PPQPositions.SetProductAt(i, l_pProduct);
			if (S_OK != hRTemp)
			{
				SvDef::StringVector msgList;
				msgList.push_back(std::format(_T("{:X}"), hRTemp));
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, msgList, SvStl::SourceFileParams(StdMessageParams));
				throw Msg;
			}
		}
		else
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams));
			throw Msg;
		}
	}// end for

	size_t iSize = m_arInspections.size();
	for (size_t i = 0; i < iSize; i++)
	{
		if (!m_arInspections[i]->CanGoOnline())
		{
			SvStl::MessageTextEnum messageId = SvStl::Tid_CanGoOnlineFailure_Inspection;
			SvDef::StringVector msgList;

			//@TODO[gra][7.40][25.05.2016]: This should at a later stage show all the tool errors not only the first error
			SvStl::MessageContainerVector AllToolMessages;
			auto inserter = std::back_inserter(AllToolMessages);
			m_arInspections[i]->getToolMessages(inserter);
			SvStl::MessageContainerVector::const_iterator Iter(AllToolMessages.begin());
			SvStl::SourceFileParams sourceFileParam(StdMessageParams);
			if (AllToolMessages.end() != Iter)
			{
				std::string sToolName = SVObjectManagerClass::Instance().GetCompleteObjectName(Iter->getObjectId());
				msgList.push_back(sToolName);
				msgList.push_back(std::string(Iter->what()));
				messageId = SvStl::Tid_CanGoOnlineFailure_InspectionTool;
				sourceFileParam = Iter->getMessage().m_SourceFile;
			}
			else
			{
				msgList.push_back(m_arInspections[i]->GetCompleteName());
			}
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, messageId, msgList, sourceFileParam);
			throw Msg;
		}
	}// end for

	if (!ResolveConditionalOutput())
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_ConditionalOutput, SvStl::SourceFileParams(StdMessageParams));
		throw Msg;
	}
}


void SVPPQObject::GoOnline()
{
	std::string FailureObjectName;

	m_lastPPQPosition = 0L;
	RebuildOutputList();

	// First, make sure the trigger toggle output is set to the right default
	SvOi::IValueObject* pValueObject(nullptr);
	if (nullptr != m_pTriggerToggle)
	{
		pValueObject = m_pTriggerToggle->getValueObject();
		m_pTriggerToggle->m_Enabled = true;
	}
	if (nullptr != pValueObject)
	{
		_variant_t Value(false);
		pValueObject->setValue(Value);
		m_TriggerToggle = static_cast<bool> (Value);
	}// end if

	pValueObject = nullptr;
	if (nullptr != m_pOutputToggle)
	{
		pValueObject = m_pOutputToggle->getValueObject();
		m_pOutputToggle->m_Enabled = true;
	}
	// Also, make sure the output toggle output is set to the right default
	if (nullptr != pValueObject)
	{
		_variant_t Value(false);
		pValueObject->setValue(Value);
		m_OutputToggle = static_cast<bool> (Value);
	}// end if

	ResetOutputs();

	if (nullptr != m_pTriggerToggle)
	{
		m_pTriggerToggle->m_Enabled = false;
	}

	if (nullptr != m_pOutputToggle)
	{
		m_pOutputToggle->m_Enabled = false;
	}

	m_storeForInterestMap.clear();

	if (m_objectIdCount > cDefaultObjectIdCount && m_outputMode != SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode)
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ErrorMultiObjectID, {}, SvStl::SourceFileParams(StdMessageParams));
		throw Msg;
	}

	bool bInspGoOnline = true;
	for (auto* pInspection : m_arInspections)
	{
		if (!pInspection->GoOnline())
		{
			FailureObjectName = pInspection->GetCompleteName();
			bInspGoOnline = false;
			break;
		}
	}// end for

	if (!bInspGoOnline)
	{
		for (auto* pInspection : m_arInspections)
		{
			pInspection->GoOffline();
		}// end for

		SvDef::StringVector msgList;
		msgList.push_back(FailureObjectName);

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Inspection, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw Msg;
	}// end if

	bool bCameraGoOnline = true;
	double maxShutterTime {0.0};
	for (auto& rCameraPair : m_Cameras)
	{
		if (!rCameraPair.first->GoOnline())
		{
			FailureObjectName = rCameraPair.first->GetCompleteName();
			bCameraGoOnline = false;
			break;
		}
		SvVol::BasicValueObjectPtr pCameraShutter = rCameraPair.first->getCameraValue(SvDef::FqnCameraShutter);
		if (nullptr != pCameraShutter)
		{
			double shutterTime {0.0};
			pCameraShutter->getValue(shutterTime);
			shutterTime /= 1000.0;
			if (shutterTime > maxShutterTime)
			{
				maxShutterTime = shutterTime;
			}
		}
	}// end for

	if (!bCameraGoOnline)
	{
		for (auto& rCameraPair : m_Cameras)
		{
			rCameraPair.first->GoOffline();
		}// end for

		for (auto* pInspection : m_arInspections)
		{
			pInspection->GoOffline();
		}// end for

		SvDef::StringVector msgList;
		msgList.push_back(FailureObjectName);

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw Msg;
	}// end if

	if (maxShutterTime > 0.0)
	{
		m_PPQPositions.setMaxCameraShutterTime(maxShutterTime);
	}

	if (!m_pTrigger->GoOnline())
	{
		FailureObjectName = m_pTrigger->GetCompleteName();
		m_pTrigger->GoOffline();

		for (auto& rCameraPair : m_Cameras)
		{
			rCameraPair.first->GoOffline();
		}// end for

		for (auto* pInspection : m_arInspections)
		{
			pInspection->GoOffline();
		}// end for

		SvDef::StringVector msgList;
		msgList.push_back(FailureObjectName);

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams));
		throw Msg;
	}// end if

	if (m_timerResolution)
	{
		::timeBeginPeriod(1);
	}
	// Create the PPQ's threads
	if (S_OK != m_processThread.Create(&SVPPQObject::ProcessCallback, GetName()))
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_CreatePPQThread, SvStl::SourceFileParams(StdMessageParams));
		throw Msg;
	}

	// Kick the threads up a notch, for real!
	m_processThread.SetPriority(THREAD_PRIORITY_HIGHEST);

	if (SvDef::SVPPQNextTriggerMode == m_outputMode)
	{
		m_NAKCount = -static_cast<long>(m_PPQPositions.size());
	}
	else
	{
		m_NAKCount = 0;
	}
	m_NewNAKCount = m_NAKCount;
	m_FirstNAKProcessCount = 0;
	m_ReducedPPQPosition = cMinReducedPpqPos;
	const int HundredPercent = 100;
	if ((SvDef::FixedMaximum == m_NAKMode || SvDef::RepairedLegacy == m_NAKMode) && m_NAKParameter > 0 && m_NAKParameter <= HundredPercent)
	{
		m_ReducedPPQPosition = (long(m_PPQPositions.size()) * m_NAKParameter) / HundredPercent;
		if (m_ReducedPPQPosition < cMinReducedPpqPos)
		{
			m_ReducedPPQPosition = cMinReducedPpqPos;
		}
		m_ReducedPPQPosition = std::min< long >(m_ReducedPPQPosition, static_cast<long>(m_PPQPositions.size()));
	}
}// end GoOnline



bool SVPPQObject::GoOffline()
{
	if (!m_bOnline) { return false; }

	m_pTrigger->GoOffline();

	SVCameraInfoMap::iterator l_svIter;

	for (l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
	{
		l_svIter->first->GoOffline();
	}// end for

	bool cameraFileAcquisition {false};
	RootObject::setRootChildValue(SvDef::FqnEnvironmentFileAcquisition, cameraFileAcquisition);

	for (auto pInspection : m_arInspections)
	{
		pInspection->GoOffline();
	}// end for

	if (m_timerResolution)
	{
		::timeEndPeriod(1);
	}
	m_PPQPositions.setMaxCameraShutterTime(0.0);
	// Bring the threads back down to earth
	m_processThread.SetPriority(THREAD_PRIORITY_NORMAL);

	m_processThread.Destroy();

	m_bOnline = false;

	for (long i = static_cast<long>(m_PPQPositions.size()) - 1; /*l_ClearCount < 5 && */0 < i; --i)
	{
		SVProductInfoStruct* l_pProduct = m_PPQPositions.GetProductAt(i);

		if (nullptr != l_pProduct)
		{
			l_pProduct->SetProductComplete();
		}
	}
	if (HasActiveMonitorList())
	{
		SetMonitorList({});
	}

	return true;
}// end GoOffline

bool SVPPQObject::IsOnline() const
{
	return m_bOnline;
}// end IsOnline

void SVPPQObject::AddInput(SVIOEntryHostStructPtr pInput)
{
	m_UsedInputs.push_back(pInput);
}// end AddInput

bool SVPPQObject::RemoveInput(SVIOEntryHostStructPtr pInput)
{
	bool result {false};
	std::string strName;

	if (nullptr != pInput)
	{
		strName = pInput->m_name;
	}

	auto iter = std::find_if(m_UsedInputs.begin(), m_UsedInputs.end(), [&strName](const auto& rEntry) {return rEntry->m_name == strName; });
	if (m_UsedInputs.end() != iter)
	{
		m_UsedInputs.erase(iter);
		result = true;
	}

	return result;
}

HRESULT SVPPQObject::GetInputIOValues(std::vector<_variant_t>& rInputValues) const
{
	HRESULT l_Status(S_OK);

	if (!m_pInputList->ReadInputs(m_UsedInputs, rInputValues))
	{
		l_Status = E_FAIL;
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_INPUT_VALUES)
	SVVariantBoolVector::const_iterator Iter = rInputValues.begin();
	std::string DebugString = _T("Inputs");
	for (; rInputValues.end() != Iter; ++Iter)
	{
		//Only valid if second is true
		if (Iter->second)
		{
			DebugString += Iter->first ? _T(", 1") : _T(", 0");
		}
	}
	DebugString += _T("\r\n");
	::OutputDebugString(DebugString.c_str());
#endif

	return l_Status;
}

bool SVPPQObject::RebuildInputList()
{
	if (nullptr != m_pInputList)
	{
		SVIOEntryHostStructPtrVector previousInputs;
		previousInputs.swap(m_UsedInputs);
		m_UsedInputs = m_pInputList->getInputList();
		for (auto& pEntry : m_UsedInputs)
		{
			//Not this is required to ensure inputs once inserted in the input list remain correctly in the list
			uint32_t ioID {pEntry->m_IOId};
			auto iter = std::find_if(previousInputs.begin(), previousInputs.end(), [&ioID](const auto& rpEntry) {return nullptr != rpEntry && ioID == rpEntry->m_IOId; });
			if (previousInputs.end() != iter)
			{
				*pEntry = *(*iter);
			}
		}
		std::sort(m_UsedInputs.begin(), m_UsedInputs.end(), &SVIOEntryHostStruct::PtrGreater);

		for (auto& pInspection : m_arInspections)
		{
			if (nullptr != pInspection)
			{
				pInspection->RebuildInspectionInputList(m_UsedInputs);
			}
		}
		return true;
	}

	return false;
}

bool SVPPQObject::RemoveOutput(SVIOEntryHostStructPtr pOutput)
{
	bool l_Status = false;

	std::string Name;

	if (nullptr != pOutput->getObject())
	{
		Name = pOutput->getObject()->GetCompleteName();
	}
	else if (SvDef::InvalidObjectId != pOutput->m_IOId)
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pOutput->m_IOId);

		Name = l_pObject->GetCompleteName();
	}

	SVIOEntryHostStructPtrVector::iterator l_Iter = m_UsedOutputs.begin();

	while (l_Iter != m_UsedOutputs.end())
	{
		std::string PPQName;
		PPQName = (*l_Iter)->getObject()->GetCompleteName();

		l_Status = (Name == PPQName);

		if (l_Status)
		{
			m_UsedOutputs.erase(l_Iter);

			break;
		}

		++l_Iter;
	}

	return l_Status;
}

bool SVPPQObject::ResolveConditionalOutput()
{
	bool bRetVal = true;

	m_conditionalOutputValueID = SvDef::InvalidObjectId;

	if (!AlwaysWriteOutputs())
	{
		// Get Input with this name and assign id
		auto compareNameWithIOEntry = [this](SVIOEntryHostStructPtr pIoEntry)
		{
			if (nullptr != pIoEntry && nullptr != pIoEntry->getObject())
			{
				return (pIoEntry->m_name == m_conditionalOutputName);
			}
			return false;
		};
		SVIOEntryHostStructPtrVector::const_iterator it = std::find_if(m_UsedInputs.begin(), m_UsedInputs.end(), compareNameWithIOEntry);
		if (it != m_UsedInputs.end())
		{
			SVIOEntryHostStructPtr pIoEntry = (*it);
			if (nullptr != pIoEntry && nullptr != pIoEntry->getObject())
			{
				m_conditionalOutputValueID = pIoEntry->getObject()->getObjectId();
				bRetVal = true;
			}
		}
		else
		{
			bRetVal = false;
		}
	}
	return bRetVal;
}

bool SVPPQObject::AlwaysWriteOutputs() const
{
	return (m_conditionalOutputName == SvDef::cPpqConditionalOutputAlways);
}

bool SVPPQObject::EvaluateConditionalOutput() const
{
	bool bRetVal = AlwaysWriteOutputs();
	if (!bRetVal)
	{
		SVObjectClass* pObject = nullptr;
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByIdentifier(m_conditionalOutputValueID, pObject);
		if (S_OK == hr && nullptr != pObject)
		{
			double Value;
			hr = pObject->getValue(Value, -1);
			if (S_OK == hr)
			{
				bRetVal = (std::numeric_limits<double>::epsilon() < Value) ? true : false;
			}
		}
	}
	return bRetVal;
}

bool SVPPQObject::WriteOutputs(SVProductInfoStruct* pProduct, int ObjectIdIndex)
{
	bool result {true};
	bool bNak {true};
	bool dataValidResult {false};
	bool writeOutputData {false};
	uint32_t inspectedObjectID {0UL};
	uint8_t objectType {0U};

	m_OutputToggle = !m_OutputToggle;

	if (nullptr != pProduct)
	{
		inspectedObjectID = pProduct->m_triggerInfo.m_Data.m_objectData[ObjectIdIndex].m_objectID;
		bNak = pProduct->m_outputsInfo.m_NakResult[ObjectIdIndex];

		objectType = pProduct->m_triggerInfo.m_Data.m_objectType;
		uint8_t triggerIndex {pProduct->m_triggerInfo.m_Data.m_triggerIndex};
		uint8_t triggerPerObjectID {pProduct->m_triggerInfo.m_Data.m_triggerPerObjectID};
		//Only when both triggerIndex and triggerPerObjectID are valid and the same write data
		writeOutputData = (0 != triggerIndex && 0 != triggerPerObjectID) ? (triggerIndex == triggerPerObjectID) : false;

		pProduct->m_outputsInfo.m_OutputToggleResult = m_OutputToggle;
		dataValidResult = pProduct->m_outputsInfo.m_DataValidResult;
	}

	bool bWriteOutputs = true;
	if (!AlwaysWriteOutputs())
	{
		// get value that represents condition
		bWriteOutputs = EvaluateConditionalOutput();
	}

	const ObjectIdVariantPairVector& rOutputValues = (nullptr != pProduct) ? pProduct->m_outputsInfo.m_Outputs : ObjectIdVariantPairVector();

	if (bWriteOutputs)
	{
		if (nullptr != m_pTrigger && writeOutputData)
		{
			int triggerChannel = (nullptr != m_pTrigger->getDevice()) ? m_pTrigger->getDevice()->getDigitizerNumber() : -1;

			if (triggerChannel >= 0 && rOutputValues.size() > 0)
			{
				SvTrig::ResultData resultData;
				resultData.m_channel = static_cast<uint8_t> (triggerChannel);
				resultData.m_objectID = inspectedObjectID;
				resultData.m_objectType = objectType;
				if ((VT_UI1 | VT_ARRAY) == rOutputValues[0].second.vt && rOutputValues[0].second.parray->rgsabound[0].cElements == sizeof(resultData.m_results))
				{
					memcpy(&resultData.m_results[0], rOutputValues[0].second.parray->pvData, sizeof(resultData.m_results));
				}
				m_pOutputList->WriteOutputData(resultData);
			}
		}
		result = m_pOutputList->WriteOutputs(rOutputValues);
		if (0 == m_DataValidDelay)
		{
			if (nullptr != m_pDataValid)
			{
				m_pOutputList->WriteOutputValue(m_pDataValid, _variant_t(dataValidResult));
			}
			if (nullptr != m_pOutputToggle)
			{
				m_pOutputList->WriteOutputValue(m_pOutputToggle, _variant_t(m_OutputToggle));
			}
		}
		else if (nullptr != pProduct)
		{
			// Set output data valid expire time
			pProduct->m_outputsInfo.m_EndDataValidDelay = SvUl::GetTimeStamp() + m_DataValidDelay;
			m_DataValidDelayQueue.AddTail(pProduct->triggerCount());
			m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::DataValidDelay]));
		}
	}

	if (bNak)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		::OutputDebugString(std::format(_T("{} NAK TRI={:d}\n"), GetName(), pProduct->triggerCount()).c_str());
#endif
		::InterlockedIncrement(&m_NAKCount);
		if (pProduct->m_triggered)
		{
			::InterlockedIncrement(&m_NewNAKCount);
		}
	}
	else
	{
		::InterlockedExchange(&m_NewNAKCount, 0);
	}

	return result;
}// end WriteOutputs

bool SVPPQObject::ResetOutputs()
{
	bool l_bRet = false;

	if (nullptr != m_pDataValid)
	{
		m_pDataValid->m_Enabled = true;
	}

	if (m_pOutputList)
	{
		l_bRet = m_pOutputList->ResetOutputs(m_UsedOutputs);
	}

	if (nullptr != m_pDataValid)
	{
		m_pDataValid->m_Enabled = false;
	}

	return l_bRet;
}// end ResetOutputs

bool SVPPQObject::RebuildOutputList()
{
	AddPpqResults();

	if (nullptr != m_pOutputList)
	{
		m_UsedOutputs.clear();
		SVIOEntryHostStructPtrVector allOutputs = m_pOutputList->getOutputList();
		for (auto& pEntry : allOutputs)
		{
			bool isPpqOutput {false};
			for (DWORD i = 0; i < SvDef::cObjectIndexMaxNr; ++i)
			{
				if (i >= m_objectIdCount)
				{
					//Reset value object setting
					pEntry->setValueObject(nullptr, i);
					continue;
				}
				SVObjectClass* pObject {pEntry->getObject(i)};
				if (nullptr == pObject && cDefaultObjectIdCount == m_objectIdCount)
				{
					//Only search for the object by name for older configurations with ObjectID index 0
					std::string fqName {SvDef::FqnConfiguration};
					fqName += '.' + pEntry->m_name;
					SVObjectManagerClass::Instance().GetObjectByDottedName(fqName, pObject);
					pEntry->setValueObject(pObject, i);
				}
				if(nullptr != pObject)
				{
					uint32_t inspectionID {pEntry->getInspectionID(i)};
					auto iterIpd = std::find_if(m_arInspections.begin(), m_arInspections.end(), [&inspectionID](const auto* pEntry) { return pEntry->getObjectId() == inspectionID; });
					isPpqOutput |=  m_arInspections.end() != iterIpd;
					SVObjectClass* pPpq = pObject->GetAncestor(SvPb::SVObjectTypeEnum::SVPPQObjectType);
					isPpqOutput |= (this == pPpq);
					if (isPpqOutput)
					{
						pEntry->m_Enabled = true;
						//These outputs have special functionality and need to be disabled
						if (pObject == &m_PpqOutputs[PpqOutputEnums::DataValid])
						{
							m_pDataValid = pEntry;
							pEntry->m_Enabled = false;
						}
						else if (pObject == &m_PpqOutputs[PpqOutputEnums::TriggerToggle])
						{
							m_pTriggerToggle = pEntry;
							pEntry->m_Enabled = false;
						}
						else if (pObject == &m_PpqOutputs[PpqOutputEnums::OutputToggle])
						{
							m_pOutputToggle = pEntry;
							pEntry->m_Enabled = false;
						}
					}
				}
			}
			if (isPpqOutput)
			{
				m_UsedOutputs.push_back(pEntry);
			}
		}
		std::sort(m_UsedOutputs.begin(), m_UsedOutputs.end(), &SVIOEntryHostStruct::PtrGreater);
		return true;
	}
	return false;
}

void SVPPQObject::AddPpqResults()
{
	static_assert(PpqOutputEnums::OutputNr != 0, "PPQ output number enum should not be 0");
	constexpr LPCTSTR cPpqOutputNames[PpqOutputEnums::OutputNr] =
	{
		_T("Trigger Toggle"),
		_T("Output Toggle"),
		_T("ACK"),
		_T("NAK"),
		_T("Master Fault"),
		_T("Master Warning"),
		_T("Not Inspected"),
		_T("Data Valid")
	};

	constexpr bool cPpqOutputDefaults[PpqOutputEnums::OutputNr] =
	{
		true,		//Trigger Toggle
		true,		//Output Toggle
		false,		//ACK
		true,		//NAK
		true,		//Master Fault
		true,		//Master Warning
		true,		//Not Inspected
		false		//Data Valid
	};

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	Log_Assert(nullptr != pConfig);
	if (nullptr == pConfig)
	{
		return;
	}

	for (int i = 0; i < PpqOutputEnums::OutputNr; ++i)
	{
		auto& rOutput = m_PpqOutputs[i];
		rOutput.SetName(cPpqOutputNames[i]);
		rOutput.SetObjectOwner(this);
		rOutput.SetDefaultValue(BOOL(cPpqOutputDefaults[i]), true);
		rOutput.SetValue(BOOL(cPpqOutputDefaults[i]));
		rOutput.ResetObject();
	}

	std::string PpqName = GetName();
	long PpqID(0);
	const size_t PpqFixedNameLength = strlen(SvDef::cPpqFixedName);
	if (PpqFixedNameLength < PpqName.size())
	{
		PpqID = atol(PpqName.c_str() + PpqFixedNameLength);
		//Zero based PPQ ID, note PPQ name is one based!
		PpqID--;
	}

	//Make sure it is above 0
	if (0 <= PpqID && nullptr != m_spPpqLength)
	{
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(m_spTiggercount.get(), ObjectIdEnum::PpqBaseTriggerCountId + PpqID);
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(m_spPpqLength.get(), ObjectIdEnum::PpqBaseLengthId + PpqID);
		m_spTiggercount->SetObjectOwner(this);
		m_spPpqLength->SetObjectOwner(this);
	}
}

long SVPPQObject::getOutputCount()
{
	return SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputCount();
}

SVProductInfoStruct* SVPPQObject::IndexPPQ(SvTrig::SVTriggerInfoStruct&& rTriggerInfo)
{
	SVProductInfoStruct* pProduct {nullptr};
	SVProductInfoStruct* pNewProduct {nullptr};
	SVProductInfoStruct* pPrevProduct = m_PPQPositions.GetProductAt(0);
	SVProductInfoStruct* pLastProduct = m_PPQPositions.IndexRegister();

	if (m_qAvailableProductInfos.RemoveHead(&pNewProduct) && nullptr != pNewProduct)
	{
		pNewProduct->m_triggerInfo = std::move(rTriggerInfo);

		if (nullptr != pPrevProduct)
		{
			pNewProduct->m_triggerInfo.m_PreviousTrigger = pPrevProduct->m_triggerInfo.m_triggerTimeStamp;
			auto& rNAKList = pPrevProduct->m_outputsInfo.m_NakResult;
			bool isNAK = std::any_of(rNAKList.begin(), rNAKList.end(), [](const bool& rNAK) { return rNAK; });
			auto& rPrevNAKList = pPrevProduct->m_prevTriggerNAK;
			isNAK |= std::any_of(rPrevNAKList.begin(), rPrevNAKList.end(), [](const bool& rNAK) { return rNAK; });
			if (pPrevProduct->m_dataComplete && isNAK)
			{
				setPreviousNAK(*pPrevProduct, pNewProduct);
			}
			checkTriggerIndex(*pPrevProduct, pNewProduct);
		}

		pNewProduct->m_triggered = true;

		pNewProduct->SetProductActive();

		m_PPQPositions.SetProductAt(0, pNewProduct);
	}

	switch (m_outputMode)
	{
		case SvDef::SVPPQNextTriggerMode:
		{
			pProduct = m_PPQPositions.GetProductAt(m_PPQPositions.size() - 1);
			break;
		}
		case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
		{
			pProduct = m_PPQPositions.GetProductAt(m_PPQPositions.size() - 1);
			bool isProductIncomplete = nullptr != pProduct && 0 == m_outputDelay && true == pProduct->m_triggered && false == pProduct->m_dataComplete;
			if (isProductIncomplete)
			{
				ProcessOutputs(*pProduct);
			}
			pProduct = pNewProduct;
			break;
		}
		default:
		{
			pProduct = pNewProduct;
			break;
		}
	}
	StartOutputs(pProduct);

	if (nullptr != pLastProduct)
	{
		// Recycle the exiting SVProductInfoStruct
		if (!RecycleProductInfo(pLastProduct))
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_69_PPQ_INDEX_NOT_RELEASED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}
	}

#ifdef _DEBUG
	if (TheSVObserverApp().GetLogDataManager())
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_IP)
		std::string l_ProductStates;

		l_ProductStates += GetName();
		l_ProductStates += _T("|SVPPQObject::IndexPPQ\n");

		m_PPQPositions.GetProductStates(l_ProductStates);

		::OutputDebugString(l_ProductStates.c_str());
#endif
	}
#endif

	return pNewProduct;
}

void SVPPQObject::InitializeProduct(SVProductInfoStruct* pNewProduct)
{
	///The inputs need to be sorted to their PPQ position
	std::vector<_variant_t> inputValues;
	inputValues.resize(pNewProduct->m_triggerInfo.m_Inputs.size());
	inputValues.swap(pNewProduct->m_triggerInfo.m_Inputs);

	///Set the inputs to the required PPQ position
	for (size_t i = 0; i < m_UsedInputs.size(); i++)
	{
		auto& rpInputEntry = m_UsedInputs[i];

		if (nullptr != rpInputEntry && rpInputEntry->m_Enabled)
		{
			///When input is connected to the standard PPQ position 1 (index 0)
			if (0 == rpInputEntry->m_PPQIndex)
			{
				pNewProduct->m_triggerInfo.m_Inputs[i] = inputValues[i];
			}
			else if (0 < rpInputEntry->m_PPQIndex && rpInputEntry->m_PPQIndex < static_cast<long> (m_PPQPositions.size()))
			{
				SVProductInfoStruct* pProduct = m_PPQPositions.GetProductAt(rpInputEntry->m_PPQIndex);
				if (nullptr != pProduct && pProduct->m_triggered && i < pProduct->m_triggerInfo.m_Inputs.size())
				{
					pProduct->m_triggerInfo.m_Inputs[i] = inputValues[i];
				}
			}
		}
	}


	// Reset all IO output objects for this product to use the new output data index
	ResetOutputValueObjects();

}// end IndexPPQ

bool SVPPQObject::StartInspection(uint32_t inspectionID)
{
	SVProductInfoStruct* pProduct {nullptr};
	size_t count = m_PPQPositions.size();

	switch (m_NAKMode)
	{
		case SvDef::Legacy: ///old Behavior 
			if (0 < m_NAKCount)
			{
				long l_NAKCount = m_NAKCount;
				if ((l_NAKCount + cMinReducedPpqPos) < static_cast<long>(count))
				{
					count -= l_NAKCount;
				}
				else
				{
					count = cMinReducedPpqPos;
				}
			}
			break;
		case SvDef::Bursts:
			if (m_FirstNAKProcessCount && !IsProductAlive(m_FirstNAKProcessCount))
			{
				m_FirstNAKProcessCount = 0;
			}
			break;

		case SvDef::RepairedLegacy:
			if (0 < m_NewNAKCount)
			{
				long l_NAKCount = m_NewNAKCount;
				if ((l_NAKCount + m_ReducedPPQPosition) < static_cast<long>(count))
				{
					count -= l_NAKCount;
				}
				else
				{
					count = std::min< size_t >(m_ReducedPPQPosition, m_PPQPositions.size());
				}
			}
			break;

		case SvDef::FixedMaximum:
			count = m_ReducedPPQPosition;
			break;
	}

	// Begin checking inspections to start processing
	for (size_t i = 0; i < count; ++i)
	{
		SVProductInfoStruct* pTempProduct = m_PPQPositions.GetProductAt(i);

		if (nullptr != pTempProduct && pTempProduct->IsProductActive())
		{
			const SVInspectionInfoStruct& rInfo = pTempProduct->m_svInspectionInfos[inspectionID];

			/// All inputs are available and inspection can start
			/// Inspection is currently not running
			/// Inspection has not already been queued
			if (true == rInfo.m_CanProcess && false == rInfo.m_InProcess && false == rInfo.m_HasBeenQueued)
			{
				if (SvDef::Bursts == m_NAKMode)
				{
					if (0 == m_FirstNAKProcessCount || m_FirstNAKProcessCount < pTempProduct->triggerCount())
					{
						pProduct = pTempProduct;
					}
					if (m_NewNAKCount > 2 && 0L == m_FirstNAKProcessCount)
					{
						m_FirstNAKProcessCount = pTempProduct->triggerCount();
					}
				}
				else
				{
					pProduct = pTempProduct;
				}
			}
			if (SvDef::Bursts == m_NAKMode && m_FirstNAKProcessCount >= pTempProduct->triggerCount() && false == pTempProduct->m_dataComplete)
			{
				SetProductIncomplete(*pTempProduct);
				//Here we need to reset the NAK flag otherwise the m_NewNAKCounter would be incremented and cause the PPQ to be reset again
				pTempProduct->m_outputsInfo.m_NakResult = {false, false, false, false};
				pTempProduct->m_prevTriggerNAK = {true, true, true, true};
				pTempProduct = nullptr;
				if (0 != m_NewNAKCount)
				{
					::InterlockedExchange(&m_NewNAKCount, 0);
				}
			}
		}
	}

	bool canStartInspection = nullptr != pProduct && nullptr != pProduct->m_svInspectionInfos[inspectionID].m_pInspection && false == pProduct->m_svInspectionInfos[inspectionID].m_pInspection->isProcessActive();
	if (canStartInspection)
	{
		pProduct->m_lastPPQPosition = m_lastPPQPosition;
		pProduct->m_NotCompleteCount = m_NotCompleteCount;
		pProduct->m_MissingImageCount = m_MissingImageCount;
		//inspectionObjectIdIndex can only be set to a valid index so no extra check!
		DWORD inspectionObjectIdIndex {pProduct->m_svInspectionInfos[inspectionID].m_pInspection->GetObjectIdIndex()};
		pProduct->m_svInspectionInfos[inspectionID].m_ObjectID = pProduct->m_triggerInfo.m_Data.m_objectData[inspectionObjectIdIndex].m_objectID;
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		long ppqPos = m_PPQPositions.GetIndexByTriggerCount(pProduct->triggerCount());
		std::string inspectionName = pProduct->m_svInspectionInfos[inspectionID].m_pInspection->GetName();
		::OutputDebugString(std::format(_T("{} Start Inspection {} TRI={:d}, PPQPos={:d}\n"), GetName(), inspectionName, pProduct->triggerCount(), ppqPos).c_str());
#endif

		if (pProduct->m_triggered == 0)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ProductNotTriggered, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		}
		if (S_OK != pProduct->m_svInspectionInfos[inspectionID].m_pInspection->StartProcess(pProduct))
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ErrorStartInspection, SvStl::SourceFileParams(StdMessageParams), 0, getObjectId());
		}
	}

	return canStartInspection;
}

void SVPPQObject::StartOutputs(SVProductInfoStruct* pProduct)
{
	if (nullptr != pProduct)
	{
		pProduct->m_outputsInfo.m_BeginProcess = SvUl::GetTimeStamp();
		pProduct->m_outputsInfo.m_EndOutputDelay = SvUl::GetMinTimeStamp();
		pProduct->m_outputsInfo.m_EndResetDelay = SvUl::GetMinTimeStamp();
		pProduct->m_outputsInfo.m_EndDataValidDelay = SvUl::GetMinTimeStamp();

		switch (m_outputMode)
		{
			case SvDef::SVPPQNextTriggerMode:
			{
				ProcessOutputs(*pProduct);
				break;
			}
			case SvDef::SVPPQTimeDelayMode:
			case SvDef::SVPPQTimeDelayAndDataCompleteMode:
			case SvDef::SVPPQExtendedTimeDelayMode:
			case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
			{
				if (0 < m_outputDelay)
				{
					pProduct->m_outputsInfo.m_EndOutputDelay = pProduct->m_triggerInfo.m_ToggleTimeStamp + m_outputDelay;
					if (pProduct->m_outputsInfo.m_BeginProcess < pProduct->m_outputsInfo.m_EndOutputDelay)
					{
						m_oOutputsDelayQueue.AddTail(pProduct->triggerCount());
						m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::DelayOutputs]));
					}
				}
				break;
			}
			default:
				break;
		}// end switch
	}
}

void SVPPQObject::AddResultsToPPQ(SVProductInfoStruct& rProduct, uint32_t inspId)
{
	bool bValid {true};
	long oState {0};
	if (cDefaultObjectIdCount == m_objectIdCount)
	{
	for (const auto& rInspectionInfo : rProduct.m_svInspectionInfos)
	{
		oState |= rInspectionInfo.second.m_InspectedState;
		bValid &= (rInspectionInfo.second.m_EndInspection > 0.0);
		}
	}
	else
		{
		oState = rProduct.m_svInspectionInfos[inspId].m_InspectedState;
		bValid = rProduct.m_svInspectionInfos[inspId].m_EndInspection > 0.0;
	}
	m_PpqOutputs[PpqOutputEnums::DataValid].SetValue(BOOL(bValid));

	// Set the value objects depending on the inspection state result
	if (PRODUCT_INSPECTION_NOT_RUN & oState)
	{
		if (PRODUCT_NOT_INSPECTED & oState)
		{
			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(true));
		}// end if
		else if (PRODUCT_NOT_ACKNOWLEDGED & oState)
		{
			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(true));
		}// end else if
		else if (PRODUCT_INSPECTION_DISABLED & oState)
		{
			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(true));
		}// end else if
		else
		{

			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(true));
		}// end else
	}// end if
	else
	{
		if (PRODUCT_INSPECTION_FAILED & oState)
		{
			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(false));
		}// end if
		else if (PRODUCT_INSPECTION_WARNING & oState)
		{
			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(false));
		}// end else if
		else if (PRODUCT_INSPECTION_PASSED & oState)
		{
			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(false));
		}// end else if
		else
		{
			m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(false));
			m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
			m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(true));
		}// end else
	}
	// First, write the trigger toggle output if it is okay
	if (nullptr != m_pTriggerToggle && nullptr != m_pTriggerToggle->getValueObject())
	{
		m_pTriggerToggle->getValueObject()->setValue(_variant_t(rProduct.m_triggerInfo.m_ToggleState));
	}
	// Toggle the Output Toggle if it is okay
	if (nullptr != m_pOutputToggle && nullptr != m_pOutputToggle->getValueObject())
	{
		m_pOutputToggle->getValueObject()->setValue(_variant_t(m_OutputToggle));
	}
	DWORD objectIdIndex = rProduct.m_svInspectionInfos[inspId].m_pInspection->GetObjectIdIndex();
	BOOL bACK(false);
	BOOL bNAK(true);
	m_PpqOutputs[PpqOutputEnums::ACK].GetValue(bACK);
	m_PpqOutputs[PpqOutputEnums::NAK].GetValue(bNAK);
	//Previous trigger NAK is true when Trigger per Object > 1 and a NAK occured during a previous trigger with the same objectID
	bACK &= (false == rProduct.m_prevTriggerNAK[objectIdIndex]) ? TRUE : FALSE;

	bool objectNak = (bNAK || rProduct.m_prevTriggerNAK[objectIdIndex]) ? true : false;

	SVIOEntryHostStructPtrVector outputList;
	std::copy_if(m_UsedOutputs.begin(), m_UsedOutputs.end(), std::back_inserter(outputList), [bValid, inspId](const SVIOEntryHostStructPtr& rEntry)
	{
		if (bValid)
		{
			//InspectionID 0 are PPQ variables
			return (inspId == rEntry->getInspectionID()) || (0 == rEntry->getInspectionID());
		}
		else
		{
			return inspId == rEntry->getInspectionID();
		}
	});
	auto insertOutput = std::back_inserter(rProduct.m_outputsInfo.m_Outputs);
	m_pOutputList->getOutputValues(insertOutput, outputList, objectIdIndex, false, bACK ? true : false, objectNak);
	rProduct.m_outputsInfo.m_NakResult[objectIdIndex]= bNAK ? true : false;
	rProduct.m_outputsInfo.m_DataValidResult = bValid && !bNAK;

	if (m_objectIdCount > cDefaultObjectIdCount)
	{
		WriteOutputs(&rProduct, objectIdIndex);
	}
}

bool SVPPQObject::SetInspectionComplete(SVProductInfoStruct& rProduct, uint32_t inspId)
{
	bool bValid = true;
	bool isReject = false;
	for (const auto& rInsp : rProduct.m_svInspectionInfos)
	{
		bValid &= (rInsp.second.m_EndInspection > 0);
		isReject |= rInsp.second.m_bReject;
	}

	AddResultsToPPQ(rProduct, inspId);

	//Only place that could set bDataComplete to true
	rProduct.m_dataComplete = bValid;
	if (rProduct.m_dataComplete)
	{
		//@TODO[MZA][10.40][14.04.2023] check ob das gebraucht wird, war bisher so implementiert das es nicht funktioniert hat.
		//SVObjectManagerClass::Instance().UpdateObservers(SVObjectManagerClass::ObserverIdEnum::PPQ, getObjectId(), rProduct);

		if ((SvDef::SVPPQTimeDelayAndDataCompleteMode == m_outputMode) ||
			(SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode == m_outputMode))
		{
			m_NextOutputDelayTimestamp = 0.0;
			m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::DelayOutputs]));
		}

		setTRofInterest(rProduct, isReject);
		rProduct.clearTRCs();
	}
	else
	{
		if (m_useProcessingOffset4Interest && nullptr != rProduct.m_svInspectionInfos[inspId].m_triggerRecordComplete)
		{
			setTR2StoreForInterestMap(inspId, rProduct);
			rProduct.m_svInspectionInfos[inspId].clearTRCs();
		}
	}

	return bValid;
}

bool SVPPQObject::SetProductComplete(SVProductInfoStruct& rProduct)
{
	bool l_Status = true;
	auto& rNAKList = rProduct.m_outputsInfo.m_NakResult;
	bool isNAK = std::any_of(rNAKList.begin(), rNAKList.begin() + m_objectIdCount, [](const bool& rNAK) { return rNAK; });

	//@TODO[MEC][10.20][29.07.2021] 
	//This function is also called under certain circumstances when NAKs occur. 
	//Check if this is correct and if so, if it makes sense to set the NAK counter to zero. 
	if (m_PPQPositions.IsProductAlive(rProduct.triggerCount()))
	{
		::InterlockedExchange(&m_NAKCount, 0);
	}

	rProduct.setInspectionTriggerRecordComplete(SvDef::InvalidObjectId);
	if (rProduct.IsProductActive())
	{
		rProduct.SetProductComplete();
		if (isNAK)
		{
			checkNakReason(rProduct.m_CantProcessReason);
		}

#if defined(TRACE_PPQ2)
		::OutputDebugString(std::format("{:d}: isNak: {:d},  Set Product Complete \n", rProduct.m_triggerInfo.lTriggerCount, isNAK).c_str());
#endif

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		long ppqPos = m_PPQPositions.GetIndexByTriggerCount(rProduct.triggerCount());
		::OutputDebugString(std::format(_T("{} Product complete TRI={:d}, PPQPos={:d}\n"), GetName(), rProduct.triggerCount(), ppqPos).c_str());
#endif
	}

	for (DWORD i = 0; i < m_objectIdCount; ++i)
	{
		bool isPreviousNAK = rProduct.m_outputsInfo.m_NakResult[i] || rProduct.m_prevTriggerNAK[i];
	long ppqIndex = m_PPQPositions.GetIndexByTriggerCount(rProduct.triggerCount());
	if (isPreviousNAK && rProduct.m_dataComplete && ppqIndex > 0)
	{
		SVProductInfoStruct* pNextProduct = m_PPQPositions.GetProductAt(ppqIndex - 1);
		if (nullptr != pNextProduct)
		{
			setPreviousNAK(rProduct, pNextProduct);
		}
	}
	}

	for (auto& rValue : rProduct.m_svCameraInfos)
	{
		rValue.second.ClearInfo(true);
	}

	return l_Status;
}

bool SVPPQObject::SetProductIncomplete(long p_PPQIndex)
{
	SVProductInfoStruct* pProduct = m_PPQPositions.GetProductAt(p_PPQIndex);
	bool l_Status = (nullptr != pProduct);

	if (l_Status)
	{
		l_Status = SetProductIncomplete(*pProduct);
	}

	return l_Status;
}

bool SVPPQObject::SetProductIncomplete(SVProductInfoStruct& rProduct)
{
	bool result {true};
	bool isNAK {false};
	for (DWORD i = 0; i < m_objectIdCount; ++i)
	{
		isNAK |= rProduct.m_outputsInfo.m_NakResult[i];
	}

	rProduct.setInspectionTriggerRecordComplete(SvDef::InvalidObjectId);
	if (rProduct.IsProductActive())
	{
		rProduct.SetProductComplete();
		SV_LOG_GLOBAL(trace) << "Product Incomplete: " << GetName() << " : " << rProduct.triggerCount() << " : " << m_PPQPositions.GetIndexByTriggerCount(rProduct.triggerCount());
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		long ppqPos = m_PPQPositions.GetIndexByTriggerCount(rProduct.triggerCount());
		::OutputDebugString(std::format(_T("{} Product incomplete TRI={:d}, PPQPos={:d}\n"), GetName(), rProduct.triggerCount(), ppqPos).c_str());
#endif
		if (isNAK)
		{
			checkNakReason(rProduct.m_CantProcessReason);
		}
#if defined (TRACE_PPQ2)
		::OutputDebugString(std::format("{:d}: isNak: {:d},  Set Product Incomplete \n", rProduct.m_triggerInfo.lTriggerCount,isNak).c_str());
#endif
	}
	rProduct.m_dataComplete = true;
	m_pOutputList->getOutputValues(std::back_inserter(rProduct.m_outputsInfo.m_Outputs), m_UsedOutputs, 0, true, false, true);

	for (DWORD i = 0; i < m_objectIdCount; ++i)
	{
		isNAK = rProduct.m_outputsInfo.m_NakResult[i] || rProduct.m_prevTriggerNAK[i];
	long ppqIndex = m_PPQPositions.GetIndexByTriggerCount(rProduct.triggerCount());
	if (isNAK && ppqIndex > 0)
	{
		SVProductInfoStruct* pNextProduct = m_PPQPositions.GetProductAt(ppqIndex - 1);
		if (nullptr != pNextProduct)
		{
			setPreviousNAK(rProduct, pNextProduct);
		}
	}
	}

	for (auto& rCamera : rProduct.m_svCameraInfos)
	{
		rCamera.second.ClearInfo(true);
	}
	return result;
}

bool SVPPQObject::RecycleProductInfo(SVProductInfoStruct* pProduct)
{
	bool l_Status = false;

	if (nullptr != pProduct)
	{
		ResetOutputValueObjects();
		pProduct->InitProductInfo();
	}

	l_Status = m_qAvailableProductInfos.AddTail(pProduct);

	return l_Status;
}// end RecycleProductInfo

HRESULT SVPPQObject::ProcessCameraResponse(const SVCameraQueueElement& rElement)
/// Either gets the appropriate product information (if available) or increments the pending image indicator.
/// If product information is available: 
/// Adds the process count to the cameras queue and signals the asynchronous procedure
{
	HRESULT l_Status = S_OK;

	if ((nullptr != rElement.m_pCamera) && (nullptr != rElement.m_Data.m_pImage))
	{
		long cameraPpqPos = 0;
		size_t productIndex = static_cast<size_t> (-1);
		SVProductInfoStruct* pProduct = nullptr;
		SVCameraInfoMap::iterator l_svIter;
		long	ppqSize = static_cast<long> (m_PPQPositions.size());

		l_svIter = m_Cameras.find(rElement.m_pCamera);

		if (l_svIter != m_Cameras.end())
		{
			cameraPpqPos = l_svIter->second.m_CameraPPQIndex;
		}

		double startTime = rElement.m_Data.m_startFrameTimestamp;

		if (cameraPpqPos < ppqSize)
		{
			// Attempts for find the trigger (which may not have happened 
			// yet) that correlates to this image, based on image time stamp.
			int cameraID = rElement.m_pCamera->getCameraID();

			long position = m_PPQPositions.GetIndexByTriggerTimeStamp(startTime, cameraID);

			// If trigger has not occurred yet, l_Position will equal -1.
			if (0 <= position && position < ppqSize)
			{
				productIndex = static_cast<size_t> (position + cameraPpqPos);
				if (productIndex < ppqSize)
				{
					pProduct = m_PPQPositions.GetProductAt(productIndex);
					if (nullptr != pProduct && pProduct->IsProductActive() && !pProduct->m_dataComplete)
					{
						if (cameraID >= 0 && cameraID < SvDef::cMaximumCameras)
						{
							pProduct->m_hasCameraImage[cameraID] = true;
						}

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
						::OutputDebugString(std::format(_T("{} Camera {} | TRI={:d}, PPQPos={:d}\n"), GetName(), rElement.m_pCamera->GetName(), pProduct->triggerCount(), productIndex).c_str());
#endif
						for (size_t i = (productIndex + 1); i < m_PPQPositions.size(); ++i)
						{
							SVProductInfoStruct* pCheckProduct = m_PPQPositions.GetProductAt(i);
							if (true == pCheckProduct->m_hasCameraImage[cameraID])
							{
								break;
							}
							else
							{
								SetProductIncomplete(*pCheckProduct);
							}
						}
					}
					else
					{
						pProduct = nullptr;
						position = -1;
					}
				}
			}
			if (position < 0)
			{
				m_PendingCameraResponses[rElement.m_pCamera] = rElement;

				SVObjectManagerClass::Instance().IncrementPendingImageIndicator();
				l_Status = E_FAIL;
			}
		}
		else
		{
			l_Status = E_FAIL;
		}

		if (nullptr != pProduct)
		{
			SvIe::SVObjectIdSVCameraInfoStructMap::iterator IterCamera;

			IterCamera = pProduct->m_svCameraInfos.find(rElement.m_pCamera->getObjectId());

			if (IterCamera != pProduct->m_svCameraInfos.end())
			{
				double	priorCameraSF = IterCamera->second.getCameraInfo().m_startFrameTimestamp;

				// Attempting to make sure we don't have the previous trigger 
				// count where the image has already been assigned (and 
				// possibly inspected).  This scenario occurs when the Camera
				// Response Queue notification arrives before the Trigger 
				// Queue notification.
				if (priorCameraSF == 0.0)
				{
					IterCamera->second.Assign(rElement.m_Data);

					for (size_t i = (productIndex + 1); i < m_PPQPositions.size(); ++i)
					{
						SVProductInfoStruct* l_pAcqProduct = m_PPQPositions.GetProductAt(i);

						if (nullptr != l_pAcqProduct)
						{
							SvIe::SVObjectIdSVCameraInfoStructMap::iterator IterCamera2(l_pAcqProduct->m_svCameraInfos.find(rElement.m_pCamera->getObjectId()));

							if (IterCamera2 != l_pAcqProduct->m_svCameraInfos.end())
							{
								if (IterCamera2->second.getCameraInfo().m_endFrameTimestamp != 0.0)
								{
									break;
								}
							}

							MarkProductInspectionsMissingAcquisiton(*l_pAcqProduct, rElement.m_pCamera);
						}
					}

					m_notifyInspectionList.push_back(pProduct->triggerCount());
					m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::NotifyInspection]));
				}
				else
				{
					// trying to use the wrong Trigger Count.  
					// Trigger even for this camera image has not 
					// occurred yet.
					m_PendingCameraResponses[rElement.m_pCamera] = rElement;

					SVObjectManagerClass::Instance().IncrementPendingImageIndicator();
					l_Status = E_FAIL;
				}
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVPPQObject::BuildCameraInfos(SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos) const
{
	HRESULT l_Status = S_OK;

	rCameraInfos.clear();

	for (const auto& rCamera : m_Cameras)
	{
		if (-1 != rCamera.second.m_CameraPPQIndex)
		{
			uint32_t rCameraID(rCamera.first->getObjectId());
			auto nextImageHandleFunctor = [&rCamera]() { return rCamera.first->ReserveNextImageHandle(); };
			rCameraInfos[rCameraID].setCamera(rCameraID, nextImageHandleFunctor);
		}
	}

	return l_Status;
}

void SVPPQObject::cameraCallback(ULONG_PTR pCaller, const CameraInfo& rCameraInfo)
{
	SvIe::SVVirtualCamera* pCamera = reinterpret_cast<SvIe::SVVirtualCamera*> (pCaller);
	bool valid = (m_bOnline && (nullptr != pCamera) && (nullptr != rCameraInfo.m_pImage));

	if (valid)
	{
		SvTrig::SVTriggerObject* pTrigger = GetTrigger();
		if (nullptr != pTrigger)
		{
			bool isCameraTriggerAndHwCameraWithImage = SvDef::TriggerType::CameraTrigger == pTrigger->getType() && false == pCamera->IsFileAcquisition() && nullptr != rCameraInfo.m_pImage;
			if (isCameraTriggerAndHwCameraWithImage)
			{
				pTrigger->Fire(rCameraInfo.m_startFrameTimestamp);
			}
		}

		m_CameraResponseQueue.AddTail(SVCameraQueueElement(pCamera, rCameraInfo));
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		::OutputDebugString(std::format(_T("{} Finished Camera Acquisition {}\n"), GetName(), pCamera->GetName()).c_str());
#endif
		m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::CameraResponses]));
	}
}

void __stdcall SVPPQObject::triggerCallback(SvTrig::SVTriggerInfoStruct&& triggerInfo)
{
	if (m_bOnline)
	{
		GetInputIOValues(triggerInfo.m_Inputs);

		switch (m_outputMode)
		{
			case SvDef::SVPPQTimeDelayMode:
			case SvDef::SVPPQTimeDelayAndDataCompleteMode:
			{
				ResetOutputs();
				break;
			}
		}

		m_TriggerToggle = !m_TriggerToggle;

		if (nullptr != m_pTriggerToggle && nullptr != m_pTriggerToggle->getObject())
		{
			m_pOutputList->WriteOutputValue(m_pTriggerToggle, m_TriggerToggle);
		}

		triggerInfo.m_ToggleState = m_TriggerToggle;
		triggerInfo.m_ToggleTimeStamp = SvUl::GetTimeStamp();

		m_oTriggerQueue.PushTail(triggerInfo);

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		::OutputDebugString(std::format(_T("{} Finished Trigger TRI={:d}\n"), GetName(), triggerInfo.lTriggerCount).c_str());
#endif

		m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::Trigger]));
	}
}

bool SVPPQObject::IsProductAlive(long p_ProductCount) const
{
	bool l_Status = (S_OK == m_PPQPositions.IsProductAlive(p_ProductCount));

	return l_Status;
}

SVProductInfoStruct* SVPPQObject::GetProductInfoStruct(long processCount) const
{
	return m_PPQPositions.GetProductByTriggerCount(processCount);
}

SVProductInfoStruct SVPPQObject::getProductReadyForRunOnce(uint32_t ipId)
{
	SVProductInfoStruct product;

	BuildCameraInfos(product.m_svCameraInfos);

	bool bOk = product.setNextAvailableCameraImage();

	if (bOk)
	{
		auto pInspection = find_if(m_arInspections.begin(), m_arInspections.end(), [&ipId](SVInspectionProcess* pIP) { return nullptr != pIP ? ipId == pIP->getObjectId() : false; });

		if (m_arInspections.end() != pInspection)
		{
			SVInspectionInfoStruct& rInspectionStruct = product.m_svInspectionInfos[(*pInspection)->getObjectId()];
			rInspectionStruct.m_pInspection = *pInspection;
			rInspectionStruct.m_inspectionPosInTrc = (*pInspection)->getTrcPos();
			bOk = rInspectionStruct.setNextAvailableTR();
		}
		else
		{
			bOk = false;
		}
	}

	if (!bOk)
	{
		product.Reset();
	}

	return product;
}

bool SVPPQObject::IsObjectInPPQ(const SVObjectClass& object) const
{
	bool retValue = false;
	const SVObjectClass* inspectObject = object.GetAncestor(SvPb::SVInspectionObjectType);

	if (nullptr != inspectObject)
	{
		for (SVInspectionProcessVector::const_iterator it = m_arInspections.begin(); it != m_arInspections.end(); ++it)
		{
			if (inspectObject == (*it))
			{
				return true;
			}
		}
	}

	return retValue;
}

HRESULT SVPPQObject::MarkProductInspectionsMissingAcquisiton(SVProductInfoStruct& rProduct, SvIe::SVVirtualCamera* pCamera)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pCamera)
	{
		for (auto pInspection : m_arInspections)
		{
			if (nullptr != pInspection && pInspection->IsCameraInInspection(pCamera->GetName()))
			{
				SVInspectionInfoPair l_Info(rProduct.triggerCount(), rProduct.m_svInspectionInfos[pInspection->getObjectId()]);

				l_Info.second.m_InspectedState = PRODUCT_NOT_INSPECTED;
				l_Info.second.m_CanProcess = false;
				l_Info.second.m_InProcess = true;
				l_Info.second.m_EndInspection = SvUl::GetTimeStamp();

				m_oInspectionQueue.AddTail(l_Info);
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void CALLBACK SVPPQObject::ProcessCallback(ULONG_PTR pCaller)
{
	PpqProcessFunction* pProcessFunction = reinterpret_cast<PpqProcessFunction*> (pCaller);
	if (nullptr != pProcessFunction && nullptr != *pProcessFunction)
	{
		(*pProcessFunction)();
	}
}

void SVPPQObject::NotifyProcessTimerOutputs(double timestamp)
{
	const PpqProcessFunction* pProcessFunction {nullptr};

	if (0 < m_DataValidDelay)
	{
		if (0.0 < m_NextDataValidDelayTimestamp && m_NextDataValidDelayTimestamp <= timestamp)
		{
			m_NextDataValidDelayTimestamp = 0.0;
			pProcessFunction = &m_processFunctions[PpqFunction::DataValidDelay];
		}
	}

	if (nullptr == pProcessFunction && 0 < m_resetDelay)
	{
		if (0.0 < m_NextOutputResetTimestamp && m_NextOutputResetTimestamp <= timestamp)
		{
			m_NextOutputResetTimestamp = 0.0;
			pProcessFunction = &m_processFunctions[PpqFunction::ResetOutputDelay];
		}
	}

	if (nullptr == pProcessFunction && 0 < m_outputDelay)
	{
		if (0.0 < m_NextOutputDelayTimestamp && m_NextOutputDelayTimestamp <= timestamp)
		{
			m_NextOutputDelayTimestamp = 0.0;
			pProcessFunction = &m_processFunctions[PpqFunction::DelayOutputs];
		}
	}

	if (nullptr != pProcessFunction)
	{
		m_processThread.Signal(reinterpret_cast<ULONG_PTR> (pProcessFunction));
	}
}

void SVPPQObject::ProcessTrigger()
{
	/// Works through the queued triggers (e.g., pushed by FinishTrigger()).
	/// This is done by taking a SVTriggerQueueElement from the head of m_oTriggerQueue 
	/// and using the information in the SVTriggerInfoStruct therein to "create" a new product
	/// (the ProcessCount of which will be added to m_oNotifyInspectionsSet).
	/// In NextTriggerMode the results of the oldest product will be output through IndexPPQ().
	/// m_CameraInputData will contain trigger-data including the data index 

	while (0 < m_oTriggerQueue.size())
	{
		SvTrig::SVTriggerInfoStruct triggerInfo;
		if (m_bOnline && S_OK == m_oTriggerQueue.PopHead(triggerInfo))
		{
			SVProductInfoStruct* pProduct = IndexPPQ(std::move(triggerInfo));

			if (nullptr != pProduct)
			{
				InitializeProduct(pProduct);
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
				::OutputDebugString(std::format(_T("{} Process Trigger TRI={:d}, ProdActive={:d}\n"), GetName(), pProduct->triggerCount(), pProduct->IsProductActive()).c_str());
#endif

				if (nullptr != m_spTiggercount)
				{
					m_spTiggercount->setValue(pProduct->m_triggerInfo.lTriggerCount);
				}

				m_notifyInspectionList.push_back(pProduct->triggerCount());
			}
		}
	}
	if (0 < m_PendingCameraResponses.size())
	{
		m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::CameraResponses]));
	}
}

void SVPPQObject::ProcessNotifyInspections()
{
	std::vector<long> notifyRestInspectionList;
	for(const auto triggerCount : m_notifyInspectionList)
	{
		SVProductInfoStruct* pProduct = m_PPQPositions.GetProductByTriggerCount(triggerCount);
		if (nullptr != pProduct)
		{
			pProduct->m_CantProcessReason = CantProcessEnum::NoReason;
			// See if the Inspection Processes can inspect this product
			int iSize = static_cast<int> (m_arInspections.size());

			for (int i = 0; i < iSize; i++)
			{
				SVInspectionInfoStruct& rInfo = pProduct->m_svInspectionInfos[m_arInspections[i]->getObjectId()];

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
				::OutputDebugString(std::format(_T("{} Notify Inspection TRI={:d} CanProcess={:d}, InProcess={:d}, ProdActive={:d}\n"), GetName(), triggerCount, rInfo.m_CanProcess, rInfo.m_InProcess, pProduct->IsProductActive()).c_str());
#endif
				if (false == rInfo.m_CanProcess && false == rInfo.m_InProcess && false == rInfo.m_HasBeenQueued && pProduct->IsProductActive())
				{
					rInfo.m_CanProcess = m_arInspections[i]->CanProcess(pProduct, pProduct->m_CantProcessReason);

					if (rInfo.m_CanProcess)
					{
						if (false == StartInspection(m_arInspections[i]->getObjectId()))
						{
							notifyRestInspectionList.push_back(triggerCount);
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
							::OutputDebugString(std::format(_T("{} Start Inspection {} failed TRI={:d}\n"), GetName(), m_arInspections[i]->GetName(), triggerCount).c_str());
#endif
					}
					}
					else
					{
						notifyRestInspectionList.push_back(triggerCount);
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
						::OutputDebugString(std::format(_T("{} Cannot Process Inspection TRI={:d}\n"), GetName(), triggerCount).c_str());
#endif
					}
				}
			}
		}
		else
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
			::OutputDebugString(std::format(_T("{} Removed Notify Inspection TRI={:d}\n"), GetName(), triggerCount).c_str());
#endif
		}
	}

	std::swap(m_notifyInspectionList, notifyRestInspectionList);
}

void SVPPQObject::ProcessDelayOutputs()
/// Used in data completion mode, but not in next trigger mode.
/// Retrieves the process (i.e. trigger) count from the m_oOutputsDelayQueue and uses it to get a "product pointer"
/// from m_ppPPQPositions via GetProductInfoStruct()
/// If the delay time is already over, calls ProcessTimeDelay/AndDataComplete/Outputs().
/// from where WriteOutputs() will be called.
/// Otherwise, puts the trigger count back into the outputs delay queue, 
{
	double currentTime {SvUl::GetTimeStamp()};

	while (0 < m_oOutputsDelayQueue.GetCount() && 0.0 == m_NextOutputDelayTimestamp)
	{
		SVProductInfoStruct* pProduct {nullptr};
		long processCount = 0;

		if (m_oOutputsDelayQueue.GetHead(&processCount))
		{
			pProduct = GetProductInfoStruct(processCount);
		}

		if (nullptr != pProduct)
		{
			if (currentTime >= pProduct->m_outputsInfo.m_EndOutputDelay)
			{
				ProcessOutputs(*pProduct);
				m_oOutputsDelayQueue.RemoveHead(&processCount);
			}
			else
			{
				switch (getPPQOutputMode())
				{
					case SvDef::SVPPQTimeDelayMode:
					case SvDef::SVPPQExtendedTimeDelayMode:
					{
						m_NextOutputDelayTimestamp = pProduct->m_outputsInfo.m_EndOutputDelay;
						break;
					}
					case SvDef::SVPPQTimeDelayAndDataCompleteMode:
					case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
					{
						if (pProduct->m_dataComplete)
						{
							m_oOutputsDelayQueue.RemoveHead(&processCount);
							ProcessOutputs(*pProduct);
						}
						else
						{
							m_NextOutputDelayTimestamp = pProduct->m_outputsInfo.m_EndOutputDelay;
						}
						break;
					}
					default:
					{
						break;
					}
				}
			}
		}
		else
		{
			m_oOutputsDelayQueue.RemoveHead(&processCount);
		}
	}
}

void SVPPQObject::ProcessResetOutputs()
{
/// Removes the "head process count" from the outputs reset queue
/// and determines from it the corresponding product pointer.
/// If there is a new "head process count" in the outputs reset queue:
/// Determines if the corresponding product's m_EndResetDelay has already been reached.
/// If so, calls ResetOutputs().
/// If not, puts the original "head process count" back onto the outputs reset queue
	double currentTime {SvUl::GetTimeStamp()};

	while (0 < m_oOutputsResetQueue.GetCount() && 0.0 == m_NextOutputResetTimestamp)
	{
		SVProductInfoStruct* pProduct {nullptr};
		long processCount {0L};

		if (m_oOutputsResetQueue.GetHead(&processCount))
		{
			pProduct = GetProductInfoStruct(processCount);
		}

		if (nullptr != pProduct)
		{
			if (currentTime >= pProduct->m_outputsInfo.m_EndResetDelay)
			{
				ResetOutputs();
				m_oOutputsResetQueue.RemoveHead(&processCount);
			}
			else
			{
				m_NextOutputResetTimestamp = pProduct->m_outputsInfo.m_EndResetDelay;
			}
		}
		else
		{
			m_oOutputsResetQueue.RemoveHead(&processCount);
		}
	}
}

void SVPPQObject::ProcessDataValidDelay()
{
	double currentTime {SvUl::GetTimeStamp()};

	while (0 < m_DataValidDelayQueue.GetCount() && 0.0 == m_NextDataValidDelayTimestamp)
	{
		SVProductInfoStruct* pProduct {nullptr};
		long processCount {0L};

		if (m_DataValidDelayQueue.GetHead(&processCount))
		{
			pProduct = GetProductInfoStruct(processCount);
		}

		if (nullptr != pProduct)
		{
			if (currentTime >= pProduct->m_outputsInfo.m_EndDataValidDelay)
			{
				if (nullptr != m_pDataValid)
				{
					m_pOutputList->WriteOutputValue(m_pDataValid, pProduct->m_outputsInfo.m_DataValidResult);
				}
				if (nullptr != m_pOutputToggle)
				{
					m_pOutputList->WriteOutputValue(m_pOutputToggle, pProduct->m_outputsInfo.m_OutputToggleResult);
				}
				m_DataValidDelayQueue.RemoveHead(&processCount);
			}
			else
			{
				m_NextDataValidDelayTimestamp = pProduct->m_outputsInfo.m_EndDataValidDelay;
			}
		}
		else
		{
			m_DataValidDelayQueue.RemoveHead(&processCount);
		}
	}
}

void SVPPQObject::ProcessCameraResponses()
/// If camera responses pending AND trigger queue empty:
/// Removes camera responses from m_PendingCameraResponses and tries to process them
/// until the first processing succeeds.
/// If no responses processed: Removes the head of m_CameraResponseQueue and processes that
/// until one processing attempt is successful or m_CameraResponseQueue is empty.
{
	bool processed {false};
	if (0 < m_PendingCameraResponses.size())
	{
		SVCameraInfoMap::const_iterator l_CameraIter = m_Cameras.begin();

		while (l_CameraIter != m_Cameras.end() && false == processed)
		{
			SVPendingCameraResponseMap::iterator l_PendingIter = m_PendingCameraResponses.find(l_CameraIter->first);

			if (l_PendingIter != m_PendingCameraResponses.end())
			{
				SVCameraQueueElement l_Data = l_PendingIter->second;

				m_PendingCameraResponses.erase(l_PendingIter);

				SVObjectManagerClass::Instance().DecrementPendingImageIndicator();

				processed = (S_OK == ProcessCameraResponse(l_Data));

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
				::OutputDebugString(std::format(_T("{} Processed Pending Camera Response {}\n"), GetName(), l_Data.m_pCamera->GetName()).c_str());
#endif
			}
			++l_CameraIter;
		}
	}

	while (0 < m_CameraResponseQueue.GetCount())
	{
		SVCameraQueueElement cameraData;
		if (m_CameraResponseQueue.RemoveHead(&cameraData))
		{
			ProcessCameraResponse(cameraData);
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
			::OutputDebugString(std::format(_T("{} Processed Camera Response {}\n"), GetName(), cameraData.m_pCamera->GetName()).c_str());
#endif
		}
		else
		{
			break;
		}
	}
}

void SVPPQObject::ProcessCompleteInspections()
/// Does nothing unless m_oInspectionQueue is non-empty.
/// If so, gets all SVInspectionInfoPairs from m_oInspectionQueue and inserts all
/// inspection IDs into m_ProcessInspectionsSet.
/// If m_ProcessInspectionsSet then is non-empty, calls ProcessInspection(false),
/// so the next product (if any) can be inspected,
/// then extracts the first valid SVInspectionInfoPair form m_oInspectionQueue
/// and gets a pointer to the corresponding SVProductInfoStruct.
/// If m_EndProcess == 0.0 SVInspectionInfoStruct::ClearIndexes() and then 
/// SetInspectionComplete() are called.
/// The product is then set to complete and all products further back in m_ppPPQPositions
/// are set to incomplete.
{
	while (0 < m_oInspectionQueue.GetCount())
	{
		SVInspectionInfoPair inspectionInfoPair;
		if (m_oInspectionQueue.RemoveHead(&inspectionInfoPair))
		{
			SVProductInfoStruct* pProduct = m_PPQPositions.GetProductByTriggerCount(inspectionInfoPair.first);
			if (nullptr != pProduct)
			{
				if (pProduct->m_outputsInfo.m_EndProcess == 0.0)
				{
					SVInspectionInfoStruct& inspectionInfo = inspectionInfoPair.second;
					uint32_t inspectedID {inspectionInfo.m_pInspection->getObjectId()};
					SVInspectionInfoStruct& rPpqInspectionInfo = pProduct->m_svInspectionInfos[inspectedID];

					rPpqInspectionInfo = inspectionInfo;
					rPpqInspectionInfo.ClearIndexes();

					// Inspection Process is done, let everyone know.
					SetInspectionComplete(*pProduct, inspectionInfo.m_pInspection->getObjectId());
				}

				if (pProduct->m_dataComplete)
				{
					SetProductComplete(*pProduct);
					switch (getPPQOutputMode())
					{
						case SvDef::SVPPQTimeDelayAndDataCompleteMode:
						case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
						{
							long triggerCount {pProduct->triggerCount()};
							m_oOutputsDelayQueue.RemoveHead(&triggerCount);
							ProcessOutputs(*pProduct);
							break;
						}
						default:
						{
							break;
						}
					}

					size_t ppqIndex = m_PPQPositions.GetIndexByTriggerCount(inspectionInfoPair.first);
					for (size_t i = ppqIndex + 1; i < m_PPQPositions.size(); ++i)
					{
						pProduct = m_PPQPositions.GetProductAt(ppqIndex);

						if (nullptr != pProduct && pProduct->IsProductActive())
						{
							SetProductIncomplete(static_cast<long>(i));
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
							::OutputDebugString(std::format(_T("{} Product incomplete TRI={:d}\n"), GetName(), pProduct->triggerCount()).c_str());
#endif
						}
						else
						{
							break;
						}
					}
				}
				//Start inspection after results have been saved with SetProductComplete
				if (nullptr != inspectionInfoPair.second.m_pInspection)
				{
					StartInspection(inspectionInfoPair.second.m_pInspection->getObjectId());
				}
		}
		}
		else
		{
			break;
		}
	}
}

void SVPPQObject::ProcessProductRequests()
/// Does nothing unless both m_oTriggerQueue and m_ProductRequests are nonempty.
/// Otherwise extracts the first SVProductRequestPair from m_ProductRequests,
/// gets the SVProductInfoStruct specified in the request.
/// Then copies the SVProductInfoStruct into the m_pProduct member of the SVProductInfoRequestStruct
/// contained in the request pair.
{
	while (0 < m_ProductRequests.GetCount())
	{
		SVProductRequestPair request;
		if (m_ProductRequests.RemoveHead(&request))
		{
			if (nullptr != request.second.m_pProduct)
			{
				SVProductInfoStruct* pProduct = GetProductInfoStruct(request.first);

				if (nullptr != pProduct)
				{
					*(request.second.m_pProduct) = *(pProduct);
				}
				else
				{
					request.second.m_pProduct->InitProductInfo();
				}
			}
			request.second.NotifyRequestComplete();
		}
	}
}

HRESULT SVPPQObject::GetProduct(SVProductInfoStruct& rProduct, long lProcessCount) const
{
	HRESULT l_Status = S_OK;
	rProduct.InitProductInfo();
	if (m_processThread.IsActive())
	{
		SVProductRequestPair l_Request(lProcessCount, SVProductInfoRequestStruct(&rProduct));
		if (m_ProductRequests.AddTail(l_Request))
		{
			m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[PpqFunction::ProductRequests]));
			l_Status = l_Request.second.WaitForRequest();
			if (S_OK == l_Status)
			{
				if (rProduct.triggerCount() < 1)
				{
					l_Status = E_FAIL;
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVPPQObject::PersistInputs(SvOi::IObjectWriter& rWriter)
{
	_variant_t l_svValue;
	std::string l_svName;

	rWriter.StartElement(SvXml::CTAG_INPUT);

	long lInputNr {0};
	for (const auto& pEntry : GetUsedInputs())
	{
		if (nullptr != pEntry)
		{
			l_svName = std::format(SvXml::CTAGF_INPUT_X, lInputNr);
			rWriter.StartElement(l_svName.c_str());

			switch (pEntry->m_ObjectType)
			{
				case IO_DIGITAL_INPUT:
				{
					l_svValue.SetString(SvXml::cDigitalType);
					rWriter.WriteAttribute(SvXml::CTAG_IO_TYPE, l_svValue);
					l_svValue.Clear();

					l_svValue.SetString(pEntry->m_name.c_str());
					rWriter.WriteAttribute(SvXml::CTAG_ITEM_NAME, l_svValue);
					l_svValue.Clear();

					l_svValue = -1;
					rWriter.WriteAttribute(SvXml::CTAG_ATTRIBUTE, l_svValue);
					l_svValue.Clear();

					l_svValue = pEntry->m_PPQIndex;
					rWriter.WriteAttribute(SvXml::CTAG_PPQ_POSITION, l_svValue);
					l_svValue.Clear();
				}
				break;

				case IO_REMOTE_INPUT:
				{
					l_svValue.SetString(SvXml::cRemoteType);
					rWriter.WriteAttribute(SvXml::CTAG_IO_TYPE, l_svValue);
					l_svValue.Clear();

					l_svValue.SetString(pEntry->m_name.c_str());
					rWriter.WriteAttribute(SvXml::CTAG_ITEM_NAME, l_svValue);
					l_svValue.Clear();

					l_svValue = pEntry->m_PPQIndex;
					rWriter.WriteAttribute(SvXml::CTAG_PPQ_POSITION, l_svValue);
					l_svValue.Clear();

					SVInputObjectPtr pInput = m_pInputList->GetInput(pEntry->m_IOId);
					if (nullptr != pInput)
					{
						pInput->Read(l_svValue);

						if (l_svValue.vt != VT_EMPTY)
						{
							rWriter.WriteAttribute(SvXml::CTAG_REMOTE_INITIAL_VALUE, l_svValue);
						}
					}
				}
				break;

				default:
				{
					break; // Do nothing.
				}
			}
			rWriter.EndElement();
		}

		lInputNr++;
	}
	rWriter.EndElement();
}

void SVPPQObject::SetMonitorList(const MonitorListAttributeStruct& rActiveList)
{
	m_bActiveMonitorList = rActiveList.m_isActive;
	if (m_bActiveMonitorList)
	{
		setRejectDepth(rActiveList.m_rejectDepth);
	}
}

bool SVPPQObject::HasActiveMonitorList() const
{
	return m_bActiveMonitorList;
}

const std::string& SVPPQObject::GetConditionalOutputName() const
{
	return m_conditionalOutputName;
}

void SVPPQObject::SetConditionalOutputName(const std::string& conditionalOutputName)
{
	m_conditionalOutputName = conditionalOutputName;
}

#pragma region SVCameraQueueElement Constructor
SVPPQObject::SVCameraQueueElement::SVCameraQueueElement(SvIe::SVVirtualCamera* pCamera, const CameraInfo& rData)
	: m_pCamera(pCamera)
	, m_Data(rData)
{
}
#pragma endregion SVCameraQueueElement Constructor

void SVPPQObject::ResetOutputValueObjects()
{
	m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(false));
	m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::DataValid].SetValue(BOOL(false));
}

DWORD SVPPQObject::GetObjectColor() const
{
	return SvDef::DefaultWhiteColor;
}

bool SVPPQObject::setRejectDepth(long depth, SvStl::MessageContainerVector* pErrorMessages/*=nullptr */)
{
	Log_Assert(depth >= 0);
	if (m_rejectCount != depth)
	{
		m_rejectCount = depth;
		std::vector<int> inspectionPosVec;
		for (auto pInspection : m_arInspections)
		{
			if (nullptr != pInspection)
			{
				inspectionPosVec.push_back(pInspection->getTrcPos());
			}
		}
		try
		{
			SvOi::getTriggerRecordControllerRWInstanceThrow().resizeIPNumberOfRecords(inspectionPosVec, getNeededRecords(), m_rejectCount);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(rSvE.getMessage());
			if (nullptr != pErrorMessages)
			{
				pErrorMessages->push_back(rSvE);
			}
			return false;
		}
	}
	return true;
}

bool SVPPQObject::requiresTimer() const
{
	bool result {SvDef::SVPPQNextTriggerMode != m_outputMode && 0 != m_outputDelay};
	result |= 0 < m_resetDelay;
	result |= 0 < m_DataValidDelay;
	return result;
}

HRESULT SVPPQObject::setModuleReady(bool set)
{
	HRESULT result {E_FAIL};
	if (nullptr != m_pTrigger && nullptr != m_pTrigger->getDevice() && nullptr != m_pTrigger->getDevice()->getDLLTrigger())
	{
		_variant_t moduleReadyState {set};
		unsigned long triggerIndex = m_pTrigger->getDevice()->getDLLTrigger()->GetHandle(m_pTrigger->getDevice()->getDigitizerNumber());
		result = m_pTrigger->getDevice()->getDLLTrigger()->SetParameterValue(triggerIndex, SVIOParameterEnum::SVModuleReady, moduleReadyState);
	}
	return result;
}

void SVPPQObject::SetNAKMode(SvDef::NakGeneration nakMode, int NAKPar)
{
	m_NAKMode = nakMode;
	m_NAKParameter = NAKPar;
}

bool SVPPQObject::SetupProductInfoStructs()
{
	bool result = setInspections2TRC();

	// Set up all the ProductInfo Structs
	SvIe::SVObjectIdSVCameraInfoStructMap cameraInfos;
	BuildCameraInfos(cameraInfos);
	m_pMasterProductInfos = new SVProductInfoStruct[getPPQLength() + cPPQExtraBufferSize];

	for (int j = 0; j < getPPQLength() + cPPQExtraBufferSize; j++)
	{
		m_pMasterProductInfos[j].m_svCameraInfos = cameraInfos;
		m_pMasterProductInfos[j].m_svInspectionInfos.clear();

		for (auto* pInsp : m_arInspections)
		{
			SVInspectionInfoStruct& rInspectionStruct = m_pMasterProductInfos[j].m_svInspectionInfos[pInsp->getObjectId()];
			rInspectionStruct.m_pInspection = pInsp;
			rInspectionStruct.m_inspectionPosInTrc = pInsp->getTrcPos();
		}// end for

		m_qAvailableProductInfos.AddTail(&m_pMasterProductInfos[j]);
	}// end for

	return result;
}

bool SVPPQObject::setInspections2TRC()
{
	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	SvPb::InspectionList inspListMessage = pConfig->GetInspectionList4TRC();
	auto* pInspList = inspListMessage.mutable_list();
	for (auto pInspection : m_arInspections)
	{
		uint32_t tmpId = pInspection->getObjectId();
		auto pInspPB = std::find_if(pInspList->begin(), pInspList->end(), [tmpId](const auto& rData)->bool
		{
			return rData.id() == tmpId;
		});

		Log_Assert(pInspList->end() != pInspPB);
		if (pInspList->end() != pInspPB)
		{
			pInspPB->set_numberofrecords(getNeededRecords());
			pInspPB->set_numberrecordsofinterest(m_rejectCount);
			pInspection->setTrcPos(static_cast<int>(std::distance(pInspList->begin(), pInspPB)));
		}
		else
		{
			pInspection->setTrcPos(-1);
		}
	}
	bool result {false};
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTrcRW)
	{
		result = pTrcRW->setInspections(std::move(inspListMessage));
		if (result)
		{
			pConfig->UpdateInspectionList4TRC();
		}
	}
	return result;
}

void SVPPQObject::setTRofInterest(const SVProductInfoStruct& rProduct, bool isInterest)
{
	std::vector<SvOi::ITriggerRecordRPtr> trVec;
	for (const auto& rIpInfo : rProduct.m_svInspectionInfos)
	{
		if (!m_useProcessingOffset4Interest)
		{
			if (nullptr != rIpInfo.second.m_triggerRecordComplete)
			{
				trVec.push_back(rIpInfo.second.m_triggerRecordComplete);
			}
		}
		else
		{
			if (nullptr != rIpInfo.second.m_triggerRecordComplete)
			{
				trVec.push_back(rIpInfo.second.m_triggerRecordComplete);
			}
			else
			{
				auto& rIpQueue = m_storeForInterestMap[rIpInfo.first];
				long triggerCount = rProduct.m_triggerInfo.lTriggerCount;
				auto iter = find_if(rIpQueue.begin(), rIpQueue.end(), [triggerCount](auto& rEntry)
				{
					return nullptr != rEntry.m_triggerRecordComplete && triggerCount == rEntry.m_triggerRecordComplete->getTriggerData().m_TriggerCount;
				});
				if (rIpQueue.end() != iter)
				{
					if (nullptr != iter->m_triggerRecordComplete)
					{
						trVec.push_back(iter->m_triggerRecordComplete);
					}
					rIpQueue.erase(iter);
				}
			}
		}
	}

	try
	{
		auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
		if (nullptr != pTrc)
		{
			pTrc->setTrsOfInterest(trVec, isInterest);
		}
	}
	catch (const std::exception& e)
	{
		SvDef::StringVector msgList;
		msgList.push_back(e.what());
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	catch (...)
	{
		SvDef::StringVector msgList;
		msgList.push_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_Unknown));
		SvStl::MessageManager Exception(SvStl::MsgType::Log);
		Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
}

void SVPPQObject::setTR2StoreForInterestMap(uint32_t ipId, SVProductInfoStruct& rProduct)
{
	auto& rIpQueue = m_storeForInterestMap[ipId];
	if (rIpQueue.size() >= m_maxProcessingOffset4Interest)
	{
		auto& rIter = rIpQueue.front();
		if (!rIter.m_bReject)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_RejectTRBeforeInterestDecision, SvStl::SourceFileParams(StdMessageParams));
		}

		auto trHandle = rIter.m_triggerRecordComplete;
		if (nullptr != trHandle)
		{
			try
			{
				auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
				if (nullptr != pTrc)
				{
					pTrc->setTrsOfInterest({trHandle}, rIter.m_bReject);
				}
			}
			catch (...)
			{
				SvDef::StringVector msgList;
				msgList.push_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_Unknown));
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		rIpQueue.pop_front();
	}
	rIpQueue.emplace_back(rProduct.m_svInspectionInfos[ipId]);
}

void SVPPQObject::calcUseProcessingOffset4InterestFlag()
{
	m_useProcessingOffset4Interest = 1 < m_arInspections.size() && 1 < m_maxProcessingOffset4Interest && m_maxProcessingOffset4Interest < getPPQLength() && SvDef::SVPPQNextTriggerMode == m_outputMode;
}

long SVPPQObject::getNeededRecords() const
{
	if (1 == m_arInspections.size() || SvDef::SVPPQNextTriggerMode != m_outputMode)
	{
		return 2l;
	}
	else if (2 > m_maxProcessingOffset4Interest || getPPQLength() < m_maxProcessingOffset4Interest)
	{
		return getPPQLength();
	}
	else
	{
		return m_maxProcessingOffset4Interest;
	}
}

void SVPPQObject::checkNakReason(CantProcessEnum cantProcessReason)
{
	switch (cantProcessReason)
	{
		case CantProcessEnum::NoReason:
		{
			m_spNotCompleteCount->setValue(++m_NotCompleteCount);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ProductIncompleteNak, SvStl::SourceFileParams(StdMessageParams));
			break;
		}
		case CantProcessEnum::MissingProduct:
		{
			m_spNotCompleteCount->setValue(++m_NotCompleteCount);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ProductMissingNak, SvStl::SourceFileParams(StdMessageParams));
			break;
		}
		case CantProcessEnum::MissingImage:
		{
			m_spMissingImageCount->setValue(++m_MissingImageCount);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_ImageMissingNak, SvStl::SourceFileParams(StdMessageParams));
			break;
		}
		case CantProcessEnum::MissingInput:
		{
			m_spNotCompleteCount->setValue(++m_NotCompleteCount);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InputMissingNak, SvStl::SourceFileParams(StdMessageParams));
			break;
		}
		default:
		{
			break;
		}
	}
}

void SVPPQObject::setPreviousNAK(const SVProductInfoStruct& rCurrentProduct, SVProductInfoStruct* pNextProduct) const
{
	for (DWORD i = 0; i < m_objectIdCount; ++i)
	{
		uint32_t currentObjectID {rCurrentProduct.m_triggerInfo.m_Data.m_objectData[i].m_objectID};
		uint32_t nextObjectID {pNextProduct->m_triggerInfo.m_Data.m_objectData[i].m_objectID};

	if (nextObjectID != 0 && nextObjectID == currentObjectID)
	{
		//Object has more than 1 trigger per Object then relay NAK result to new product
		if (1u < pNextProduct->m_triggerInfo.m_Data.m_triggerIndex)
		{
				pNextProduct->m_prevTriggerNAK[i] = true;
			}
		}
	}
}

void SVPPQObject::checkTriggerIndex(const SVProductInfoStruct& rCurrentProduct, SVProductInfoStruct* pNextProduct) const
{
	for (DWORD i = 0; i < m_objectIdCount; ++i)
	{
		uint32_t currentObjectID {rCurrentProduct.m_triggerInfo.m_Data.m_objectData[i].m_objectID};
		uint32_t nextObjectID {pNextProduct->m_triggerInfo.m_Data.m_objectData[i].m_objectID};
	uint8_t nextTriggerIndex {pNextProduct->m_triggerInfo.m_Data.m_triggerIndex};

	if (nextObjectID != 0 && nextObjectID == currentObjectID)
	{
		uint8_t currentTriggerIndex {rCurrentProduct.m_triggerInfo.m_Data.m_triggerIndex};
		//Check that trigger index has been incremented
		if (currentTriggerIndex + 1 != nextTriggerIndex)
		{
				pNextProduct->m_prevTriggerNAK[i] = true;
		}
	}
	else
	{
		//Check when next objectID that trigger index is 1
		if (1 != nextTriggerIndex)
		{
				pNextProduct->m_prevTriggerNAK[i] = true;
			}
		}
	}
}

void SVPPQObject::AttachAcqToTriggers(bool setSoftwareTrigger)
{
	if (nullptr != m_pTrigger)
	{
		SvTrig::SVTriggerClass* pTriggerDevice = m_pTrigger->getDevice();
		if (nullptr != pTriggerDevice)
		{
			pTriggerDevice->clearAcquisitionTriggers();
			int iDigNum = pTriggerDevice->getDigitizerNumber();
			auto* pTriggerDll = pTriggerDevice->getDLLTrigger();
			if (nullptr != pTriggerDll)
			{
				if (setSoftwareTrigger)
				{
					unsigned long triggerIndex = pTriggerDll->GetHandle(iDigNum);
					_variant_t value = m_pTrigger->GetSoftwareTriggerPeriod();
					pTriggerDll->SetParameterValue(triggerIndex, SVIOParameterEnum::TriggerPeriod, value);
				}

				bool cameraSoftwareTrigger {false};
				RootObject::getRootChildValue(SvDef::FqnEnvironmentCameraSoftwareTrigger, cameraSoftwareTrigger);
				SvIe::SVVirtualCameraPtrVector cameraVector = GetVirtualCameras();
				for (auto* pCamera : cameraVector)
				{
					bool isFileAcquisition = pCamera->IsFileAcquisition();
					SvIe::SVAcquisitionClassPtr pAcq = pCamera->GetAcquisitionDevice();
					if (nullptr != pAcq)
					{
						SvTrig::SVDigitizerLoadLibraryClass* pAcqDLL = SvIe::SVDigitizerProcessingClass::Instance().GetDigitizerSubsystem(pAcq->DigName().c_str());
						if (pAcqDLL)
						{
							SvTrig::AcquisitionParameter acquisitionParameter;
							acquisitionParameter.m_pDllDigitizer = pAcqDLL;
							acquisitionParameter.m_triggerChannel = pAcq->m_hDigitizer;
							acquisitionParameter.m_active = setSoftwareTrigger || isFileAcquisition;
							pTriggerDevice->addAcquisitionTrigger(std::move(acquisitionParameter));
						}
					}
				}
			}
		}
	}
}
