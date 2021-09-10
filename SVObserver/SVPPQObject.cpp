//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVGlobal.h"
#include "SVInfoStructs.h"
#include "SVObserver.h"
#include "SVPPQConstants.h"
#include "SVToolSet.h"
#include "SVVisionProcessorHelper.h"
#include "TextDefinesSvO.h"
#include "Definitions/Color.h"
#include "Definitions/GlobalConst.h"
#include "Definitions/StringTypeDef.h"
#include "InspectionEngine/SVAcquisitionClass.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVIOLibrary/SVInputObjectList.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVIOLibrary/SVOutputObjectList.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVMessage/SVMessage.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVProtoBuf/TriggerRecordController.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVOLibrary/SVHardwareManifest.h"
#include "Triggering/SVTriggerObject.h"
#include "Triggering/SVTriggerClass.h"
#include "ObjectInterfaces/ITriggerRecordControllerRW.h"
#include "SVLogLibrary/Logging.h"

#pragma endregion Includes
//#define TRACE_PPQ2
#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr double TwentyPercent = .20;
const long MinReducedPPQPosition = 2;
#pragma endregion Declarations

HRESULT SVPPQObject::ProcessDelayOutputs(bool& rProcessed)
/// Used in data completion mode, but not in next trigger mode.
/// Retrieves the process (i.e. trigger) count from the m_oOutputsDelayQueue and uses it to get a "product pointer"
/// from m_ppPPQPositions via GetProductInfoStruct()
/// If the delay time is already over, calls ProcessTimeDelay/AndDataComplete/Outputs().
/// from where WriteOutputs() will be called.
/// Otherwise, puts the trigger count back into the outputs delay queue, 
{
	HRESULT l_Status = S_OK;

	SVProductInfoStruct* l_pProduct = nullptr;

	double l_CurrentTime = SvUl::GetTimeStamp();

	while (0 < m_oOutputsDelayQueue.GetCount() && nullptr == l_pProduct)
	{
		long l_ProcessCount = 0;

		if (m_oOutputsDelayQueue.RemoveHead(&l_ProcessCount))
		{
			l_pProduct = GetProductInfoStruct(l_ProcessCount);
		}
		else
		{
			l_Status = E_FAIL;

			break;
		}

		if (nullptr != l_pProduct)
		{
			if (0 < m_oOutputsDelayQueue.GetCount())
			{
				SVProductInfoStruct* l_pNextProduct = nullptr;

				if (m_oOutputsDelayQueue.GetHead(&l_ProcessCount))
				{
					l_pNextProduct = GetProductInfoStruct(l_ProcessCount);
				}

				if (0 < m_outputDelay && nullptr != l_pNextProduct && l_pNextProduct->m_outputsInfo.m_EndOutputDelay <= l_CurrentTime)
				{
					l_pProduct = nullptr;
				}
			}
		}

		if (nullptr != l_pProduct)
		{
			// if time delay has not expired yet, then put back on the 
			// m_oOutputsDelayQueue.  Then why did we remove it to begin with?
			if (l_CurrentTime < l_pProduct->m_outputsInfo.m_EndOutputDelay)
			{
				m_NextOutputDelayTimestamp = l_pProduct->m_outputsInfo.m_EndOutputDelay;

				m_oOutputsDelayQueue.AddHead(l_pProduct->ProcessCount());

				l_pProduct = nullptr;

				break;
			}
			else
			{
				m_NextOutputDelayTimestamp = 0.0;
			}
		}
	}

	rProcessed = (nullptr != l_pProduct);

	if (rProcessed)
	{
		switch (getPPQOutputMode())
		{
			case SvDef::SVPPQTimeDelayMode:
			case SvDef::SVPPQExtendedTimeDelayMode:
			{
				l_Status = ProcessOutputs(*l_pProduct);
				break;
			}
			case SvDef::SVPPQTimeDelayAndDataCompleteMode:
			case SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode:
			{
				l_Status = ProcessTimeDelayAndDataCompleteOutputs(*l_pProduct, rProcessed);

				break;
			}
			default:
			{
				l_Status = E_FAIL;

				break;
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessOutputs(SVProductInfoStruct& rProduct)
{
	HRESULT l_Status = S_OK;

	if (WriteOutputs(&rProduct))
	{
		rProduct.m_outputsInfo.m_EndProcess = SvUl::GetTimeStamp();

		if (0 < m_resetDelay)
		{
			rProduct.m_outputsInfo.m_EndResetDelay = rProduct.m_outputsInfo.m_EndProcess + m_resetDelay;

			m_oOutputsResetQueue.AddTail(rProduct.ProcessCount());

		}
		SetProductComplete(rProduct);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessTimeDelayAndDataCompleteOutputs(SVProductInfoStruct& p_rProduct, bool& rProcessed)
{
	HRESULT l_Status = S_OK;

	if (p_rProduct.m_dataComplete)
	{
		l_Status = ProcessOutputs(p_rProduct);
		m_NextOutputDelayTimestamp = 0.0;
	}
	else
	{
		m_NextOutputDelayTimestamp = p_rProduct.m_outputsInfo.m_EndOutputDelay;

		m_oOutputsDelayQueue.AddHead(p_rProduct.ProcessCount());

		rProcessed = false;
	}

	return l_Status;
}

#pragma region Constructor
SVPPQObject::SVPPQObject(LPCSTR ObjectName)
	: SVObjectClass(ObjectName)
	, m_oTriggerQueue(10)
	, m_NAKParameter(SvDef::DefaultNakParameter)
	, m_NAKCount(0)
	, m_NAKMode(SvDef::Bursts)
	, m_FirstNAKProcessCount(0)
	, m_NewNAKCount(0)
	, m_ReducedPPQPosition(MinReducedPPQPosition)
	, m_conditionalOutputName(PPQ_CONDITIONAL_OUTPUT_ALWAYS)
	, m_bActiveMonitorList(false)
{
	init();
}

SVPPQObject::SVPPQObject(SVObjectClass* POwner, int StringResourceID)
	: SVObjectClass(POwner, StringResourceID)
	, m_oTriggerQueue(10)
	, m_NAKParameter(SvDef::DefaultNakParameter)
	, m_NAKCount(0)
	, m_NAKMode(SvDef::Bursts)
	, m_FirstNAKProcessCount(0)
	, m_NewNAKCount(0)
	, m_ReducedPPQPosition(MinReducedPPQPosition)
	, m_conditionalOutputName(PPQ_CONDITIONAL_OUTPUT_ALWAYS)
	, m_bActiveMonitorList(false)
{
	init();
}
#pragma endregion Constructor

SVPPQObject::~SVPPQObject()
{
	SVObjectManagerClass::Instance().ClearShortPPQIndicator();

	// Stop the multimedia timer thread for the output and reset time delays
	if (0 != m_uOutputTimer)
	{
		::timeKillEvent(m_uOutputTimer);

		::timeEndPeriod(1);

		m_uOutputTimer = 0;
	}

	if (m_isCreated)
	{
		Destroy();
	}

	m_pTriggerToggle.reset();
	m_pOutputToggle.reset();
	m_pDataValid.reset();
	m_SlotManager.reset();
}

void SVPPQObject::init()
{
	m_outputMode = SvDef::SVPPQNextTriggerMode;
	m_isCreated = false;
	m_bOnline = false;
	m_maintainSourceImages = false;
	m_inspectionTimeoutMillisec = 0;
	m_outputDelay = 100;
	m_resetDelay = 0;
	m_DataValidDelay = TheSVObserverApp.getDataValidDelay();
	m_NextOutputDelayTimestamp = 0.0;
	m_NextOutputResetTimestamp = 0.0;
	m_uOutputTimer = 0;
	m_pTrigger = nullptr;
	m_pMasterProductInfos = nullptr;
	m_pInputList = nullptr;
	m_pOutputList = nullptr;
	m_NotCompleteCount = 0;
	m_MissingImageCount = 0;

	for (auto& rOutput : m_PpqOutputs)
	{
		rOutput.SetObjectAttributesAllowed(SvPb::embedable, SvOi::SetAttributeType::OverwriteAttribute);
		m_childObjects.push_back(&rOutput);
	}

	//PPQ Trigger Count should never be used as it is not synchronous to the inspection rather use the Tool Set.Trigger Count
	UINT Attribute2Remove = SvPb::selectableForEquation | SvPb::selectableForStatistics | SvPb::archivable;

	auto InitPPQCounter = [&](SvVol::BasicValueObjectPtr& counter, LPCSTR CounterName)
	{
		counter = m_PpqValues.setValueObject(CounterName, 0L, this);
		if (nullptr != counter)
		{
			counter->SetObjectAttributesAllowed(Attribute2Remove, SvOi::SetAttributeType::RemoveAttribute);
			m_childObjects.push_back(counter.get());
		}
	};
	InitPPQCounter(m_spNotCompleteCount, SvDef::FqnPpqNotCompleteCount);
	InitPPQCounter(m_spMissingImageCount, SvDef::FqnPpqMissingImageCount);
	InitPPQCounter(m_spTiggercount, SvDef::FqnPpqTriggerCount);

	m_spPpqLength = m_PpqValues.setValueObject(SvDef::FqnPpqLength, StandardPpqLength, this);
	SVObjectManagerClass::Instance().IncrementShortPPQIndicator();
	if (nullptr != m_spPpqLength)
	{
		m_childObjects.push_back(m_spPpqLength.get());
	}
	m_cameraCallback = [this](ULONG_PTR pCaller, const CameraInfo& rCameraInfo) { cameraCallback(pCaller, rCameraInfo); };
	m_triggerCallback = [this](SvTrig::SVTriggerInfoStruct&& triggerInfo) { triggerCallback(std::move(triggerInfo)); };
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
	::OutputDebugString(SvUl::Format(_T("%s Inspection %s completed TRI=%d State=%x\n"), GetName(), rData.second.m_pInspection->GetName(), rData.first, rData.second.m_InspectedState).c_str());
#endif
	m_lastPPQPosition = m_PPQPositions.GetIndexByTriggerCount(rData.first);

	m_oInspectionQueue.AddTail(rData);
	m_AsyncProcedure.Signal(nullptr);

	return S_OK;
}

bool SVPPQObject::Create()
{
	// Return if already created
	if (m_isCreated) { return false; }

	for (int i = 0; i < static_cast<long>(m_PPQPositions.size()); ++i)
	{
		RecycleProductInfo(m_PPQPositions.GetProductAt(i));

		m_PPQPositions.SetProductAt(i, nullptr);
	}

	// Create buckets for the PPQ positions
	m_PPQPositions.resize(getPPQLength());

	// Create a set of ProductInfoStructs to use
	if (!m_qAvailableProductInfos.Create()) { return false; }

	SetupProductInfoStructs();

	m_isCreated = true;

	// Create the Queues that run the threads
	m_isCreated &= m_CameraResponseQueue.Create();
	m_isCreated &= m_oInspectionQueue.Create();
	m_isCreated &= m_oOutputsDelayQueue.Create();
	m_isCreated &= m_oOutputsResetQueue.Create();
	m_isCreated &= m_DataValidDelayQueue.Create();
	m_isCreated &= m_ProductRequests.Create();

	m_oNotifyInspectionsSet.clear();
	m_ProcessInspectionsSet.clear();
	calcUseProcessingOffset4InterestFlag();

	if (m_isCreated)
	{
		// Force the Inspections to rebuild
		for (auto pInspection : m_arInspections)
		{
			pInspection->RebuildInspection();
		}// end for
	}

	m_TriggerToggle = false;
	m_OutputToggle = false;

	return m_isCreated;
}

bool SVPPQObject::Rebuild()
{
	// Return if not created
	if (!m_isCreated) { return false; }

	// Delete buckets for the PPQ positions
	m_PPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete[] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	// Create buckets for the PPQ positions
	m_PPQPositions.resize(getPPQLength());

	bool result = SetupProductInfoStructs();

	calcUseProcessingOffset4InterestFlag();

	// Force the Inspections to rebuild as well
	for (auto pInspection : m_arInspections)
	{
		pInspection->RebuildInspection(false);
	}// end for

	return result;
}// end Rebuild

void SVPPQObject::Destroy()
{
	// Return if not created
	if (!m_isCreated) { return; }

	// Stop the multimedia timer thread for the output and reset time delays
	if (0 != m_uOutputTimer)
	{
		::timeKillEvent(m_uOutputTimer);

		::timeEndPeriod(1);

		m_uOutputTimer = 0;
	}

	// Destroy the Queues that run the threads
	bool l_Processed = true;

	m_oTriggerQueue.clear();

	while (l_Processed)
	{
		ProcessProductRequests(l_Processed);
	}

	m_oNotifyInspectionsSet.clear();
	m_ProcessInspectionsSet.clear();

	if (!(m_PendingCameraResponses.empty()))
	{
		SVObjectManagerClass::Instance().AdjustPendingImageIndicator(-(static_cast<long>(m_PendingCameraResponses.size())));

		m_PendingCameraResponses.clear();
	}

	m_ProductRequests.Destroy();
	m_DataValidDelayQueue.Destroy();
	m_oOutputsDelayQueue.Destroy();
	m_oOutputsResetQueue.Destroy();
	m_oInspectionQueue.Destroy();
	m_CameraResponseQueue.Destroy();

	// Delete buckets for the PPQ positions
	m_PPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete[] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	m_qAvailableProductInfos.Destroy();

	m_AllInputs.clear();
	m_UsedInputs.clear();
	m_AllOutputs.clear();
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

void SVPPQObject::SetMaintainSourceImages(bool bMaintainImages)
{
	m_maintainSourceImages = bMaintainImages;
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

void SVPPQObject::GetInspectionCount(long& lSize) const
{
	lSize = static_cast<long> (m_arInspections.size());
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
	assert(nullptr != pCamera);

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
	assert(nullptr != pCamera);

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

	for (int j = 0; nullptr != m_pMasterProductInfos && j < getPPQLength() + g_lPPQExtraBufferSize; j++)
	{
		m_pMasterProductInfos[j].m_svCameraInfos = cameraInfos;
	}// end for
}

void SVPPQObject::PrepareGoOnline(bool isTestMode)
{
	SvSml::SharedMemWriter::Instance().clearInspectionIdsVector(GetName());

	calcUseProcessingOffset4InterestFlag();

	if (!m_pTrigger->CanGoOnline(isTestMode))
	{
		SvDef::StringVector msgList;
		msgList.push_back(m_pTrigger->GetCompleteName());

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}

	for (SVCameraInfoMap::iterator l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
	{
		if (!(l_svIter->second.m_CameraPPQIndex >= 0 && l_svIter->first->CanGoOnline()))
		{
			SvDef::StringVector msgList;
			msgList.push_back(l_svIter->first->GetCompleteName());

			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
			throw Msg;
		}
	}

	// Reset the PPQ
	for (size_t i = 0; i < m_PPQPositions.size(); ++i)
	{
		SVProductInfoStruct* l_pProduct = m_PPQPositions.GetProductAt(i);
		if (nullptr != l_pProduct)
		{
			if (!RecycleProductInfo(l_pProduct))
			{
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
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
				msgList.push_back(SvUl::Format(_T("%X"), hRTemp));
				SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
				throw Msg;
			}
		}
		else
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
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
			SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, messageId, msgList, sourceFileParam, SvStl::Err_10185);
			throw Msg;
		}
	}// end for

	if (!ResolveConditionalOutput())
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_CanGoOnlineFailure_ConditionalOutput, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}
}


void SVPPQObject::GoOnline()
{
	std::string FailureObjectName;

	m_lastPPQPosition = 0L;
#ifdef EnableTracking
	m_PPQTracking.clear();
	m_PPQTracking.m_QueueLength = m_PPQPositions.size();
	m_PPQTracking.m_TimeLength = m_outputDelay + 50;
#endif
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

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Inspection, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}// end if

	bool bCameraGoOnline = true;
	for (auto& rCameraPair : m_Cameras)
	{
		if (!rCameraPair.first->GoOnline())
		{
			FailureObjectName = rCameraPair.first->GetCompleteName();
			bCameraGoOnline = false;
			break;
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

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}// end if

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

		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}// end if

	// Create a multimedia timer thread for the output and reset time delays
	if (TIMERR_NOERROR != ::timeBeginPeriod(1))
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_CreateTimerThread, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}

	//The timer should start when not "Next Trigger Mode" or when reset or data valid delay are not 0
	bool StartTimer(SvDef::SVPPQNextTriggerMode != m_outputMode);
	StartTimer |= 0 < m_resetDelay;
	StartTimer |= 0 < m_DataValidDelay;
	if (StartTimer)
	{
		m_uOutputTimer = ::timeSetEvent(1, 0, SVPPQObject::OutputTimerCallback, reinterpret_cast<DWORD_PTR>(this),
			TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	}// end if

	// Create the PPQ's threads
	auto threadProcess = [this](bool& rProcessed) {ThreadProcess(rProcessed); };
	if (S_OK != m_AsyncProcedure.Create(&SVPPQObject::APCThreadProcess, threadProcess, GetName(), SVAffinityPPQ))
	{
		SvStl::MessageContainer Msg(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_GoOnlineFailure_CreatePPQThread, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10185);
		throw Msg;
	}

	// Kick the threads up a notch, for real!
	m_AsyncProcedure.SetPriority(THREAD_PRIORITY_HIGHEST);

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
	m_ReducedPPQPosition = MinReducedPPQPosition;
	const int HundredPercent = 100;
	if ((SvDef::FixedMaximum == m_NAKMode || SvDef::RepairedLegacy == m_NAKMode) && m_NAKParameter > 0 && m_NAKParameter <= HundredPercent)
	{
		m_ReducedPPQPosition = (long(m_PPQPositions.size()) * m_NAKParameter) / HundredPercent;
		if (m_ReducedPPQPosition < MinReducedPPQPosition)
		{
			m_ReducedPPQPosition = MinReducedPPQPosition;
		}
		m_ReducedPPQPosition = std::min< long >(m_ReducedPPQPosition, static_cast<long>(m_PPQPositions.size()));
	}
}// end GoOnline



bool SVPPQObject::GoOffline()
{
	if (!m_bOnline) { return false; }

#ifdef EnableTracking
	if (TheSVObserverApp.UpdateAndGetLogDataManager())
	{
		std::string l_FileName;

		l_FileName = SvUl::Format(_T("C:\\SVObserver\\ProductIndexes_%ld-%s.log"),
			SVObjectManagerClass::Instance().GetFileSequenceNumber(), GetName());

		std::fstream l_Stream(l_FileName.c_str(), std::ios_base::trunc | std::ios_base::out);

		if (l_Stream.is_open())
		{
			for (size_t i = 0; i < m_PPQPositions.size(); ++i)
			{
				std::string l_Info;

				SVProductInfoStruct* l_pProduct = m_PPQPositions.GetProductAt(i);

				if (nullptr != l_pProduct)
				{
					l_pProduct->DumpIndexInfo(l_Info);
				}

				l_Stream << _T("Position ") << i << _T(" : ");
				l_Stream << l_Info.c_str();
				l_Stream << std::endl;
			}

			l_Stream.close();
		}

		std::string l_Name = SvUl::Format(_T("C:\\SVObserver\\%s-Counts-%ld.csv"), GetName(), SVObjectManagerClass::Instance().GetFileSequenceNumber());

		std::fstream l_TrackingStream(l_Name.c_str(), std::ios_base::trunc | std::ios_base::out);

		if (l_TrackingStream.is_open())
		{
			l_TrackingStream << _T("Name,Count") << std::endl;

			SVPPQTracking::SVPPQTrackingMap::iterator l_PPQIter = m_PPQTracking.m_Counts.begin();

			while (l_PPQIter != m_PPQTracking.m_Counts.end())
			{
				l_TrackingStream << l_PPQIter->first.c_str() << _T(",");
				l_TrackingStream << l_PPQIter->second;
				l_TrackingStream << std::endl;

				++l_PPQIter;
			}

			l_TrackingStream << std::endl;

			l_TrackingStream << _T("Name");

			for (size_t i = 0; i < m_PPQPositions.size(); ++i)
			{
				l_TrackingStream << _T(",") << i;
			}

			l_TrackingStream << std::endl;

			SVPPQTracking::SVQueueTrackingMap::iterator l_QueueIter = m_PPQTracking.m_QueueCounts.begin();

			while (l_QueueIter != m_PPQTracking.m_QueueCounts.end())
			{
				l_TrackingStream << l_QueueIter->first.c_str();

				SVPPQTrackingElement& l_rElement = l_QueueIter->second;

				for (size_t i = 0; i < l_rElement.m_TrackedCounts.size(); ++i)
				{
					l_TrackingStream << _T(",") << l_rElement.m_TrackedCounts[i];
				}

				l_TrackingStream << std::endl;

				++l_QueueIter;
			}

			l_TrackingStream << std::endl;

			// Write Outputs Time...
			l_TrackingStream << _T("Time ms");

			for (size_t i = 0; i < m_PPQTracking.m_TimeLength; ++i)
			{
				l_TrackingStream << _T(",") << i;
			}

			l_TrackingStream << std::endl;

			l_QueueIter = m_PPQTracking.m_QueueWriteTimeCounts.begin();  // m_QueueCounts.begin();

			while (l_QueueIter != m_PPQTracking.m_QueueWriteTimeCounts.end())
			{
				l_TrackingStream << l_QueueIter->first.c_str();

				SVPPQTrackingElement& l_rElement = l_QueueIter->second;

				for (size_t i = 0; i < l_rElement.m_TrackedCounts.size(); ++i)
				{
					l_TrackingStream << _T(",") << l_rElement.m_TrackedCounts[i];
				}

				l_TrackingStream << std::endl;

				++l_QueueIter;
			}
			l_TrackingStream << std::endl;

			l_TrackingStream.close();
		}
	}
#endif EnableTracking

	m_pTrigger->GoOffline();

	SVCameraInfoMap::iterator l_svIter;

	for (l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter)
	{
		l_svIter->first->GoOffline();
	}// end for



	for (auto pInspection : m_arInspections)
	{
		pInspection->GoOffline();
	}// end for

	// Stop the multimedia timer thread for the output and reset time delays
	if (0 != m_uOutputTimer)
	{
		::timeKillEvent(m_uOutputTimer);

		::timeEndPeriod(1);

		m_uOutputTimer = 0;
	}

	// Bring the threads back down to earth
	m_AsyncProcedure.SetPriority(THREAD_PRIORITY_NORMAL);

	m_AsyncProcedure.Destroy();

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
	m_AllInputs.push_back(pInput);
}// end AddInput

bool SVPPQObject::RemoveInput(SVIOEntryHostStructPtr pInput)
{
	bool l_Status = false;

	std::string strName;

	if (nullptr != pInput->getObject())
	{
		strName = pInput->getObject()->GetName();
	}
	else if (SvDef::InvalidObjectId != pInput->m_IOId)
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pInput->m_IOId);

		strName = l_pObject->GetName();
	}

	SVIOEntryHostStructPtrVector::iterator l_Iter = m_AllInputs.begin();

	while (l_Iter != m_AllInputs.end())
	{
		l_Status = (strName == (*l_Iter)->getObject()->GetName());

		if (l_Status)
		{
			m_AllInputs.erase(l_Iter);

			break;
		}

		++l_Iter;
	}

	return l_Status;
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
	SVIOEntryHostStructPtr pOldInput;
	SVIOEntryHostStructPtr pNewInput;

	// Make sure all the defaults are here for old configurations
	AddDefaultInputs();

	if (nullptr != m_pInputList)
	{
		SVIOEntryHostStructPtrVector inputEntryVector = m_pInputList->getInputList();
		size_t lNewSize = inputEntryVector.size();

		for (size_t iOld = 0; iOld < m_AllInputs.size(); iOld++)
		{
			pOldInput = m_AllInputs[iOld];

			for (size_t iNew = 0; iNew < lNewSize; iNew++)
			{
				pNewInput = inputEntryVector[iNew];

				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pNewInput->m_IOId);

				if ((0 == strcmp(l_pObject->GetName(), pOldInput->getObject()->GetName())) &&
					(pNewInput->m_ObjectType == pOldInput->m_ObjectType))
				{
					pNewInput->setLinkedObject(pOldInput->getObject());
					pNewInput->m_PPQIndex = pOldInput->m_PPQIndex;
					pNewInput->m_Enabled = pOldInput->m_Enabled;
					pOldInput->m_ObjectType = pNewInput->m_ObjectType;
					pOldInput->m_IOId = pNewInput->m_IOId;
					pOldInput->getObject()->ResetObject();
					break;
				}// end if
			}// end for
		}// end for

		m_UsedInputs = inputEntryVector;

		std::sort(m_UsedInputs.begin(), m_UsedInputs.end(), &SVIOEntryHostStruct::PtrGreater);

		return true;
	}// end if

	return false;
}// end RebuildInputList


// AddToAvailableInputs searches the m_AllInputs by name. If it does not exist,
// then the new input is added. Two types are supported: 
// IO_REMOTE_INPUT and IO_DIGITAL_INPUT.
// A variant value object is created for the remote input.
// A Boolean value object is created for the digital input.
bool SVPPQObject::AddToAvailableInputs(SVIOObjectType eType, const std::string& rName)
{
	bool bFound = false;

	// search for the input. 
	for (size_t k = 0; k < m_AllInputs.size(); k++)
	{
		SVIOEntryHostStructPtr pOldInput = m_AllInputs[k];
		if (rName == pOldInput->getObject()->GetName())
		{
			bFound = true;
			break;
		}// end if
	}// end for

	if (!bFound)
	{
		SVObjectClass* pObject {nullptr};
		std::shared_ptr<SvOi::IValueObject> pInputValueObject;
		if (eType == IO_REMOTE_INPUT)
		{
			// new variant value object for Remote Inputs.
			pInputValueObject = std::make_shared<SvVol::SVVariantValueObjectClass>();
			_variant_t defaultValue;
			defaultValue.ChangeType(VT_R8);
			if (nullptr != pInputValueObject)
			{
				pInputValueObject->setDefaultValue(defaultValue);
				pObject = dynamic_cast<SVObjectClass*> (pInputValueObject.get());
			}
		}
		else
		{
			// new Bool Value Object for Digital Inputs.
			pInputValueObject = std::make_shared<SvVol::SVBoolValueObjectClass>();
			if (nullptr != pInputValueObject)
			{
				pObject = dynamic_cast<SVObjectClass*> (pInputValueObject.get());
			}
		}

		if (nullptr != pObject)
		{
			pObject->SetName(rName.c_str());
			pObject->SetObjectOwner(this);
			pObject->SetObjectAttributesAllowed(SvDef::selectableAttributes, SvOi::SetAttributeType::RemoveAttribute);
			pObject->ResetObject();
		}

		if (nullptr != pInputValueObject)
		{
			SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();
			pIOEntry->setValueObject(pInputValueObject);
			pIOEntry->m_ObjectType = eType;
			pIOEntry->m_PPQIndex = -1;
			pIOEntry->m_Enabled = false;

			// Add input to PPQ.
			AddInput(pIOEntry);
			return true;
		}
	}// end if
	return false;
}

// Add Default Inputs adds inputs to the m_AllInputs 
// m_AllInputs appears to be the available input list.
void SVPPQObject::AddDefaultInputs()
{
	unsigned long ulCount = 0;
	unsigned long l;

	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount(ulCount);

	// Create all the default Digital Inputs
	for (l = 0; l < ulCount; l++)
	{
		std::string Name = SvUl::Format(_T("DIO.Input%d"), l + 1);
		AddToAvailableInputs(IO_DIGITAL_INPUT, Name);
	}// end for

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	SVInputObjectList* pInputObjectList(nullptr);
	if (nullptr != pConfig) { pInputObjectList = pConfig->GetInputObjectList(); }

	long count = (nullptr != pInputObjectList) ? pInputObjectList->getRemoteInputCount() : 0;

	// Create all the default Remote Inputs
	for (l = 0; l < static_cast<unsigned long>(count); l++)
	{
		std::string Name = SvUl::Format(SvO::cRemoteInputNumberLabel, l + 1);
		AddToAvailableInputs(IO_REMOTE_INPUT, Name);
	}// end for
}

SVIOEntryHostStructPtr SVPPQObject::GetInput(const std::string& name) const
{
	for (SVIOEntryHostStructPtrVector::const_iterator it = m_AllInputs.begin(); it != m_AllInputs.end(); ++it)
	{
		SVIOEntryHostStructPtr pIoEntry = (*it);
		if (nullptr != pIoEntry)
		{
			if (name == pIoEntry->getObject()->GetName())
			{
				return pIoEntry;
			}
		}
	}
	return SVIOEntryHostStructPtr();
}

void SVPPQObject::AddOutput(SVIOEntryHostStructPtr pOutput)
{
	m_AllOutputs.push_back(pOutput);
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

	SVIOEntryHostStructPtrVector::iterator l_Iter = m_AllOutputs.begin();

	while (l_Iter != m_AllOutputs.end())
	{
		std::string PPQName;
		PPQName = (*l_Iter)->getObject()->GetCompleteName();

		l_Status = (Name == PPQName);

		if (l_Status)
		{
			m_AllOutputs.erase(l_Iter);

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
				return (pIoEntry->getObject()->GetName() == m_conditionalOutputName);
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
	return (m_conditionalOutputName == PPQ_CONDITIONAL_OUTPUT_ALWAYS);
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

bool SVPPQObject::WriteOutputs(SVProductInfoStruct* pProduct)
{
	bool result {true};
	bool bNak {true};
	bool dataValidResult {false};
	bool writeOutputData {false};
	DWORD inspectedObjectID {0};
	_variant_t objectType;

#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
	std::string l_ProductState;
#endif
#endif

	m_OutputToggle = !m_OutputToggle;

	if (nullptr != pProduct)
	{
		if (true == pProduct->m_dataComplete)
		{
			//As all inspections have been tested to have the same object ID we will set it to the first inspection
			ObjectIdSVInspectionInfoStructMap::const_iterator iter {pProduct->m_svInspectionInfos.begin()};
			if (pProduct->m_svInspectionInfos.end() != iter)
			{
				inspectedObjectID = iter->second.m_ObjectID;
			}
		}
		else
		{
			SetProductIncomplete(*pProduct);
			//Data index with -1 will return the default output values which is required in this case
			pProduct->m_outputsInfo.m_Outputs = m_pOutputList->getOutputValues(m_UsedOutputs, true, false, true);
			//Set the default inspected object ID which would be the input object ID
			SvTrig::IntVariantMap::const_iterator iterData = pProduct->m_triggerInfo.m_Data.find(SvTrig::TriggerDataEnum::ObjectID);
			if (pProduct->m_triggerInfo.m_Data.end() != iterData)
			{
				inspectedObjectID = static_cast<DWORD> (iterData->second);
			}
		}

		DWORD triggerIndex {0};
		DWORD triggerPerObjectID {0};
		SvTrig::IntVariantMap::const_iterator  iterData = pProduct->m_triggerInfo.m_Data.find(SvTrig::TriggerDataEnum::ObjectType);
		if (pProduct->m_triggerInfo.m_Data.end() != iterData)
		{
			objectType = iterData->second;
		}
		iterData = pProduct->m_triggerInfo.m_Data.find(SvTrig::TriggerDataEnum::TriggerIndex);
		if (pProduct->m_triggerInfo.m_Data.end() != iterData)
		{
			triggerIndex = static_cast<DWORD> (iterData->second);
		}
		iterData = pProduct->m_triggerInfo.m_Data.find(SvTrig::TriggerDataEnum::TriggerPerObjectID);
		if (pProduct->m_triggerInfo.m_Data.end() != iterData)
		{
			triggerPerObjectID = static_cast<DWORD> (iterData->second);
		}
		//Only when both triggerIndex and triggerPerObjectID are valid and the same write data
		writeOutputData = (0 != triggerIndex && 0 != triggerPerObjectID) ? (triggerIndex == triggerPerObjectID) : false;

		pProduct->m_outputsInfo.m_OutputToggleResult = m_OutputToggle;
		bNak = pProduct->m_outputsInfo.m_NakResult;
		dataValidResult = pProduct->m_outputsInfo.m_DataValidResult;
#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
		l_ProductState = pProduct->m_ProductState;
#endif
#endif
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
			//Trigger channel needs to  be one based
			long triggerChannel = (nullptr != m_pTrigger->getDevice()) ? m_pTrigger->getDevice()->getDigitizerNumber() + 1 : -1;

			if (triggerChannel >= 0 && rOutputValues.size() > 0)
			{
				SvTrig::IntVariantMap outputData;
				outputData[SvTrig::TriggerDataEnum::ObjectID] = _variant_t(inspectedObjectID);
				if (VT_EMPTY != objectType.vt)
				{
					outputData[SvTrig::TriggerDataEnum::ObjectType] = objectType;
				}
				outputData[SvTrig::TriggerDataEnum::OutputData] = rOutputValues[0].second;
				m_pOutputList->WriteOutputData(triggerChannel, outputData);
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
			m_DataValidDelayQueue.AddTail(pProduct->ProcessCount());
		}
	}


#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
	if (!(l_ProductState.empty()))
	{
		l_ProductState += _T("\n");
		OutputDebugString(l_ProductState.c_str());
	}
#endif
#endif

	if (bNak)
	{
		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//m_PPQTracking.IncrementCount( _T( "Product NAK" ) );
#endif
		::InterlockedIncrement(&m_NAKCount);
		::InterlockedIncrement(&m_NewNAKCount);
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
	// Make sure all the defaults are here for old configurations
	AddDefaultOutputs();

	if (nullptr != m_pOutputList)
	{
		SVIOEntryHostStructPtrVector newOutputs = m_pOutputList->getOutputList();
		size_t lNewSize = newOutputs.size();

		m_pTriggerToggle.reset();
		m_pOutputToggle.reset();
		m_pDataValid.reset();

		for (size_t iOld = 0; iOld < m_AllOutputs.size(); iOld++)
		{
			SVIOEntryHostStructPtr pOldOutput = m_AllOutputs[iOld];
			pOldOutput->m_IOId = SvDef::InvalidObjectId;

			for (size_t iNew = 0; iNew < lNewSize; iNew++)
			{
				SVIOEntryHostStructPtr pNewOutput = newOutputs[iNew];

				SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(pNewOutput->m_IOId);

				std::string NewName = pObject->GetCompleteName();
				std::string OldName = pOldOutput->getObject()->GetCompleteName();

				if (pNewOutput->m_ObjectType == pOldOutput->m_ObjectType && NewName == OldName)
				{
					if (SvPb::SVIoObjectType == pObject->GetObjectType())
					{
						//IO object types require the complete name
						pObject->SetName(pOldOutput->getObject()->GetCompleteName().c_str());
						pNewOutput->setLinkedObject(pOldOutput->getObject());
					}
					else
					{
						//Normal object types require the name and set the owner
						pObject->SetName(pOldOutput->getObject()->GetName());
						pNewOutput->setLinkedObject(pOldOutput->getObject());
						pNewOutput->getObject()->SetObjectOwner(pOldOutput->getObject()->GetParent());
					}

					pNewOutput->m_Enabled = pOldOutput->m_Enabled;
					pOldOutput->m_ObjectType = pNewOutput->m_ObjectType;
					pOldOutput->m_IOId = pNewOutput->m_IOId;
					pOldOutput->getObject()->ResetObject();

					// Check for prefix of PPQ for these special signals...
					if (0 == NewName.compare(0, 4, _T("PPQ_")))
					{
						// Disable Trigger Toggle since it is not written with the outputs
						if (std::string::npos != NewName.find(_T("Trigger Toggle")))
						{
							m_pTriggerToggle = pNewOutput;
							pNewOutput->m_Enabled = false;
						}// end if

						// Find Output Toggle now to make it quicker later
						if (std::string::npos != NewName.find(_T("Output Toggle")))
						{
							m_pOutputToggle = pNewOutput;
							pNewOutput->m_Enabled = false;
						}// end if

						// Find Data Valid now to make it quicker later
						if (std::string::npos != NewName.find(_T("Data Valid")))
						{
							m_pDataValid = pNewOutput;
							pNewOutput->m_Enabled = false;
						}// end if
					}
					break;
				}// end if
			}// end for
		}// end for

		m_UsedOutputs = newOutputs;

		return true;
	}// end if

	return false;
}// end RebuildOutputList

void SVPPQObject::AddDefaultOutputs()
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
	assert(nullptr != pConfig);
	if (nullptr == pConfig)
	{
		return;
	}

	//This checks if the default outputs are already in the list by checking if the first default output is found
	bool	bFound {false};
	std::string Name = cPpqOutputNames[PpqOutputEnums::TriggerToggle];
	for (auto pOutput : m_AllOutputs)
	{
		if (Name == pOutput->getObject()->GetName())
		{
			bFound = true;
			break;
		}
	}

	for (int i = 0; i < PpqOutputEnums::OutputNr; ++i)
	{
		auto& rOutput = m_PpqOutputs[i];
		rOutput.SetName(cPpqOutputNames[i]);
		rOutput.SetObjectOwner(this);
		rOutput.SetDefaultValue(BOOL(cPpqOutputDefaults[i]), true);
		rOutput.SetValue(BOOL(cPpqOutputDefaults[i]));
		rOutput.ResetObject();

		if (!bFound)
		{
			SVIOEntryHostStructPtr pIOEntry = std::make_shared<SVIOEntryHostStruct>();
			pIOEntry->setLinkedObject(dynamic_cast<SVObjectClass*> (&rOutput));
			pIOEntry->m_ObjectType = SVHardwareManifest::isPlcSystem(pConfig->GetProductType()) ? IO_PLC_OUTPUT : IO_DIGITAL_OUTPUT;
			pIOEntry->m_Enabled = true;

			AddOutput(pIOEntry);
		}
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
}// end AddDefaultOutputs

void SVPPQObject::GetAllOutputs(SVIOEntryHostStructPtrVector& ppIOEntries) const
{
	ppIOEntries = m_AllOutputs;
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
	if (TheSVObserverApp.GetLogDataManager())
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


// p_Offset represents PPQ position of product.
HRESULT SVPPQObject::NotifyInspections(long offset)
{
	HRESULT l_Status = S_OK;

	// ************************************************************************
	// Now we need to check if any inspections were waiting on inputs read after
	// the first position. Only check the products at positions that have a input set

	// Begin checking inspections to start processing
	if (0 <= offset && static_cast<size_t>(offset) < m_PPQPositions.size())
	{
		SVProductInfoStruct* pTempProduct = m_PPQPositions.GetProductAt(offset);
		pTempProduct->m_CantProcessReason = CantProcessEnum::NoReason;
		// See if the Inspection Processes can inspect this product
		int iSize = static_cast<int> (m_arInspections.size());

		for (int i = 0; i < iSize; i++)
		{
			SVInspectionInfoStruct& rInfo = pTempProduct->m_svInspectionInfos[m_arInspections[i]->getObjectId()];

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
			::OutputDebugString(SvUl::Format(_T("%s Notify Inspection TRI=%d CanProcess=%d, InProcess=%d, ProdActive=%d\n"), GetName(), pTempProduct->ProcessCount(), rInfo.m_CanProcess, rInfo.m_InProcess, pTempProduct->IsProductActive()).c_str());
#endif
			// if product has not been previously determined that it "can 
			// process".  That would generally mean that it's been here 
			// before, and should not be able to happen.
			if (false == rInfo.m_CanProcess && false == rInfo.m_InProcess && false == rInfo.m_HasBeenQueued && pTempProduct->IsProductActive())
			{
				// Tests if we have all required inputs.  Includes assigned 
				// discrete inputs and camera images.
				// If images or inputs are missing, CanProcess will return 
				// FALSE(0).  m_CanProcess will also equal false.
				rInfo.m_CanProcess = m_arInspections[i]->CanProcess(pTempProduct, pTempProduct->m_CantProcessReason);

				if (rInfo.m_CanProcess)
				{
					if (pTempProduct->IsProductActive())
					{
						m_ProcessInspectionsSet.insert(m_arInspections[i]->getObjectId());

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
						::OutputDebugString(SvUl::Format(_T("%s Add Process Inspection Set TRI=%d\n"), GetName(), pTempProduct->ProcessCount()).c_str());
#endif
#ifdef EnableTracking
						std::string l_Title = m_arInspections[i]->GetName();

						l_Title += _T(" CP");

						m_PPQTracking.IncrementCount(l_Title, offset);
#endif
					}
				}
				else
				{
					// If it can not process, this still does not change the 
					// l_Status, so the returned status will still be S_OK
					// which indicates NOT "Processed".
					l_Status = S_FALSE; // 1
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
					::OutputDebugString(SvUl::Format(_T("%s Cannot Process Inspection TRI=%d\n"), GetName(), pTempProduct->ProcessCount()).c_str());
#endif
				}
			} // if( !( l_rInfo.m_CanProcess ) && !( l_rInfo.m_InProcess ) && pTempProduct->IsProductActive() )
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	// Now we need to check if any inspections were waiting on inputs read after
	// the first position. Only check the products at positions that have a input set
	// End checking inspections to start processing
	// ************************************************************************

	return l_Status;
}

HRESULT SVPPQObject::StartInspection(uint32_t inspectionID)
{
	HRESULT result {S_OK};

	SVProductInfoStruct* pProduct {nullptr};
	size_t count = m_PPQPositions.size();

	switch (m_NAKMode)
	{
		case SvDef::Legacy: ///old Behavior 
			if (0 < m_NAKCount)
			{
				long l_NAKCount = m_NAKCount;
				if ((l_NAKCount + MinReducedPPQPosition) < static_cast<long>(count))
				{
					count -= l_NAKCount;
				}
				else
				{
					count = MinReducedPPQPosition;
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

#ifdef EnableTracking
	size_t l_ProductIndex = m_PPQPositions.size();
#endif
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
#ifdef EnableTracking
				l_ProductIndex = m_PPQPositions.GetIndexByTriggerCount(pProduct->ProcessCount());
#endif
				if (SvDef::Bursts == m_NAKMode)
				{
					if (0 == m_FirstNAKProcessCount || m_FirstNAKProcessCount < pTempProduct->ProcessCount())
					{
						pProduct = pTempProduct;
					}
					if (m_NewNAKCount > 2 && 0L == m_FirstNAKProcessCount)
					{
						m_FirstNAKProcessCount = pTempProduct->ProcessCount();
						continue;
					}
				}
				else
				{
					pProduct = pTempProduct;
				}
			}
			if (SvDef::Bursts == m_NAKMode && m_FirstNAKProcessCount > pTempProduct->ProcessCount())
			{
				SetProductIncomplete(*pTempProduct);
				pTempProduct = nullptr;
			}
		}
	}

	bool canStartInspection = nullptr != pProduct && nullptr != pProduct->m_svInspectionInfos[inspectionID].m_pInspection && false == pProduct->m_svInspectionInfos[inspectionID].m_pInspection->isProcessActive();
	if (canStartInspection)
	{
		pProduct->m_lastPPQPosition = m_lastPPQPosition;
		pProduct->m_NotCompleteCount = m_NotCompleteCount;
		pProduct->m_MissingImageCount = m_MissingImageCount;
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		long ppqPos = m_PPQPositions.GetIndexByTriggerCount(pProduct->ProcessCount());
		::OutputDebugString(SvUl::Format(_T("%s Start Inspection TRI=%d, PPQPos=%d\n"), GetName(), pProduct->ProcessCount(), ppqPos).c_str());
#endif

		if (pProduct->m_triggered == 0)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, _T("SVPPQObject::StartInspection product not triggerd"), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16223);
		}
		result = pProduct->m_svInspectionInfos[inspectionID].m_pInspection->StartProcess(pProduct);

#ifdef EnableTracking
		std::string l_Title = pProduct->m_svInspectionInfos[inspectionID].m_pInspection->GetName();
		l_Title += _T(" Start");
		m_PPQTracking.IncrementCount(l_Title, l_ProductIndex);
#endif

	}

	return result;
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
				if (0 == m_outputDelay)
				{
					m_oOutputsDelayQueue.AddTail(pProduct->ProcessCount());
				}
				else if (0 < m_outputDelay)
				{
					pProduct->m_outputsInfo.m_EndOutputDelay = pProduct->m_triggerInfo.m_ToggleTimeStamp + m_outputDelay;
					if (pProduct->m_outputsInfo.m_BeginProcess < pProduct->m_outputsInfo.m_EndOutputDelay)
					{
						m_oOutputsDelayQueue.AddTail(pProduct->ProcessCount());
					}
				}
				break;
			}
			default:
				break;
		}// end switch
	}
}

void SVPPQObject::AddResultsToPPQ(SVProductInfoStruct& rProduct)
{
	bool bValid {rProduct.m_svInspectionInfos.size() > 0};
	long oState {0};
	DWORD previousObjectID {0};
	for (const auto& rInspectionInfo : rProduct.m_svInspectionInfos)
	{
		oState |= rInspectionInfo.second.m_InspectedState;
		bValid &= (rInspectionInfo.second.m_EndInspection > 0.0);
		//Make sure that the inspected object ID is the same for all inspections if not then failed
		if (0 == previousObjectID)
		{
			previousObjectID = rInspectionInfo.second.m_ObjectID;
		}
		else if (rInspectionInfo.second.m_ObjectID != previousObjectID)
		{
			oState |= PRODUCT_INSPECTION_FAILED;
		}
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
	BOOL bACK(false);
	BOOL bNAK(true);
	m_PpqOutputs[PpqOutputEnums::ACK].GetValue(bACK);
	m_PpqOutputs[PpqOutputEnums::NAK].GetValue(bNAK);
	rProduct.m_outputsInfo.m_Outputs = m_pOutputList->getOutputValues(m_UsedOutputs, false, bACK ? true : false, bNAK ? true : false);
	rProduct.m_outputsInfo.m_NakResult = bNAK ? true : false;
	rProduct.m_outputsInfo.m_DataValidResult = bValid && !bNAK;
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

	if (bValid)
	{
		// Currently only used for Remote Outputs and Fail Status Stream.
		// returns E_FAIL when there are no listeners/observers.  Not having 
		// Remote Outputs or Fail Status is not an error in this case.
		SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cPPQObjectTag), getObjectId(), rProduct);
		AddResultsToPPQ(rProduct);
	}

	//Only place that could set bDataComplete to true
	rProduct.m_dataComplete = bValid;
	if (rProduct.m_dataComplete)
	{
		if ((SvDef::SVPPQTimeDelayAndDataCompleteMode == m_outputMode) ||
			(SvDef::SVPPQExtendedTimeDelayAndDataCompleteMode == m_outputMode))
		{
			NotifyProcessTimerOutputs();
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

bool SVPPQObject::SetProductComplete(long p_PPQIndex)
{
	SVProductInfoStruct* pProduct = m_PPQPositions.GetProductAt(p_PPQIndex);

	bool l_Status = false;

	if (nullptr != pProduct)
	{
		CommitSharedMemory(*pProduct);
		l_Status = SetProductComplete(*pProduct);

#ifdef EnableTracking
		m_PPQTracking.IncrementCount(_T("Product Complete"), p_PPQIndex);
#endif
	}

	return l_Status;
}

bool SVPPQObject::SetProductComplete(SVProductInfoStruct& rProduct)
{

	bool l_Status = true;
	bool isNAK = rProduct.m_outputsInfo.m_NakResult;

	//@TODO[MEC][10.20][29.07.2021] 
	//This function is also called under certain circumstances when NAKs occur. 
	//Check if this is correct and if so, if it makes sense to set the NAK counter to zero. 
	if (m_PPQPositions.IsProductAlive(rProduct.ProcessCount()))
	{
		::InterlockedExchange(&m_NAKCount, 0);
	}

	// if bInspecting is false, this loop will fall through
	// if we are maintaining source images it will also fall through
	// we don't want to release all the source images unless all the IPs have started

	if (false == getMaintainSourceImages())
	{
		for (auto& rValue : rProduct.m_svCameraInfos)
		{
			rValue.second.ClearCameraInfo();
		}
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
		::OutputDebugString(std::format("{}: isNak: {},  Set Product Complete \n", rProduct.m_triggerInfo.lTriggerCount, isNAK).c_str());
#endif

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		long ppqPos = m_PPQPositions.GetIndexByTriggerCount(rProduct.ProcessCount());
		::OutputDebugString(SvUl::Format(_T("%s Product complete TRI=%d, PPQPos=%d\n"), GetName(), rProduct.ProcessCount(), ppqPos).c_str());
#endif
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

#ifdef EnableTracking
		m_PPQTracking.IncrementCount(_T("Product Incomplete"), p_PPQIndex);
#endif
	}

	return l_Status;
}

bool SVPPQObject::SetProductIncomplete(SVProductInfoStruct& p_rProduct)
{
	bool isNak = p_rProduct.m_outputsInfo.m_NakResult;
	
	bool l_Status = true;
	// Release from Shared Memory
	ReleaseSharedMemory(p_rProduct);

	for (auto& rValue : p_rProduct.m_svCameraInfos)
	{
		rValue.second.ClearCameraInfo();
	}
	p_rProduct.setInspectionTriggerRecordComplete(SvDef::InvalidObjectId);
	if (p_rProduct.IsProductActive())
	{
		p_rProduct.SetProductComplete();
		SV_LOG_GLOBAL(trace) << "Product Incomplete: " << GetName() << " : " << p_rProduct.ProcessCount() << " : " << m_PPQPositions.GetIndexByTriggerCount(p_rProduct.ProcessCount());
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		long ppqPos = m_PPQPositions.GetIndexByTriggerCount(p_rProduct.ProcessCount());
		::OutputDebugString(SvUl::Format(_T("%s Product incomplete TRI=%d, PPQPos=%d\n"), GetName(), p_rProduct.ProcessCount(), ppqPos).c_str());
#endif
		if (isNak)
		{
			checkNakReason(p_rProduct.m_CantProcessReason);
		}
#if defined (TRACE_PPQ2)
		::OutputDebugString(std::format("{}: isNak: {},  Set Product Incomplete \n", p_rProduct.m_triggerInfo.lTriggerCount,isNak).c_str());
#endif
	}

	return l_Status;
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
		size_t l_ProductIndex = static_cast<size_t> (-1);
		SVProductInfoStruct* pProduct = nullptr;
		SVCameraInfoMap::iterator l_svIter;
		long	ppqSize = static_cast<long> (m_PPQPositions.size());

		l_svIter = m_Cameras.find(rElement.m_pCamera);

		if (l_svIter != m_Cameras.end())
		{
			cameraPpqPos = l_svIter->second.m_CameraPPQIndex;
		}

		double startTime = rElement.m_Data.m_startFrameTime;

		if (cameraPpqPos < ppqSize)
		{
			// Attempts for find the trigger (which may not have happened 
			// yet) that correlates to this image, based on image time stamp.
			int cameraID = rElement.m_pCamera->getCameraID();

			bool notPending = (getPPQLength() <= 2);

			long position = m_PPQPositions.GetIndexByTriggerTimeStamp(startTime, cameraID);

			// If trigger has not occurred yet, l_Position will equal -1.
			if (0 <= position && position < ppqSize)
			{
				l_ProductIndex = static_cast<size_t> (position + cameraPpqPos);
				if (l_ProductIndex < ppqSize)
				{
					pProduct = m_PPQPositions.GetProductAt(l_ProductIndex);
					if (nullptr != pProduct && pProduct->IsProductActive() && !pProduct->m_dataComplete)
					{
						if (cameraID >= 0 && cameraID < SvDef::cMaximumCameras)
						{
							pProduct->m_hasCameraImage[cameraID] = true;
						}
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
						::OutputDebugString(SvUl::Format(_T("%s Camera %s | TRI=%d, PPQPos=%d\n"), GetName(), rElement.m_pCamera->GetName(), pProduct->ProcessCount(), l_ProductIndex).c_str());
#endif
					}
					else
					{
						pProduct = nullptr;
						position = -1;
					}
				}
			}
			if (position < 0 && !notPending)
			{
				m_PendingCameraResponses[rElement.m_pCamera] = rElement;

				SVObjectManagerClass::Instance().IncrementPendingImageIndicator();
				l_Status = E_FAIL;
#ifdef EnableTracking

				std::string l_Title = _T("Pending ");

				l_Title += rElement.m_pCamera->GetName();

				m_PPQTracking.IncrementCount(l_Title);
#endif
			}
			else// if l_Position > than PPQ size.
			{
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
				double	priorCameraSF = IterCamera->second.m_StartFrameTimeStamp;

				// Attempting to make sure we don't have the previous trigger 
				// count where the image has already been assigned (and 
				// possibly inspected).  This scenario occurs when the Camera
				// Response Queue notification arrives before the Trigger 
				// Queue notification.
				if (priorCameraSF == 0.0)
				{
					double endTime = rElement.m_Data.m_endFrameTime;

					IterCamera->second.Assign(startTime, endTime, rElement.m_Data.m_pImage);



					if (nullptr == IterCamera->second.getImage())
					{
#ifdef EnableTracking
						std::string l_Title = rElement.m_pCamera->GetName();

						l_Title += _T(" Index NAK");

						m_PPQTracking.IncrementCount(l_Title);
						m_PPQTracking.IncrementCount(l_Title, l_ProductIndex);
#endif
					}
					else
					{
#ifdef EnableTracking
						m_PPQTracking.IncrementCount(rElement.m_pCamera->GetName(), l_ProductIndex);
#endif
					}

					for (size_t i = (l_ProductIndex + 1); i < m_PPQPositions.size(); ++i)
					{
						SVProductInfoStruct* l_pAcqProduct = m_PPQPositions.GetProductAt(i);

						if (nullptr != l_pAcqProduct)
						{
							SvIe::SVObjectIdSVCameraInfoStructMap::iterator IterCamera2(l_pAcqProduct->m_svCameraInfos.find(rElement.m_pCamera->getObjectId()));

							if (IterCamera2 != l_pAcqProduct->m_svCameraInfos.end())
							{
								if (IterCamera2->second.m_CallbackTimeStamp != 0)
								{
									break;
								}
								IterCamera2->second.m_CallbackTimeStamp = SvUl::GetTimeStamp();
							}
#ifdef EnableTracking
							std::string l_Title = rElement.m_pCamera->GetName();

							l_Title += _T(" NAK");

							m_PPQTracking.IncrementCount(l_Title);
							m_PPQTracking.IncrementCount(l_Title, l_ProductIndex);
#endif

							MarkProductInspectionsMissingAcquisiton(*l_pAcqProduct, rElement.m_pCamera);
						}
					}

					IterCamera->second.m_CallbackTimeStamp = SvUl::GetTimeStamp();

					m_oNotifyInspectionsSet.insert(pProduct->ProcessCount());
					m_AsyncProcedure.Signal(nullptr);
				} // if (( priorCameraSF == 0 ) && (1 != productComplete))
				else
				{

					// trying to use the wrong Trigger Count.  
					// Trigger even for this camera image has not 
					// occurred yet.
					m_PendingCameraResponses[rElement.m_pCamera] = rElement;

					SVObjectManagerClass::Instance().IncrementPendingImageIndicator();
					l_Status = E_FAIL;

#ifdef EnableTracking
					std::string l_Title = _T("Pending ");
					l_Title += rElement.m_pCamera->GetName();
					m_PPQTracking.IncrementCount(l_Title);
#endif
				}
			} // if( l_svIter1 != l_pProduct->m_svCameraInfos.end() )
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
				pTrigger->Fire(rCameraInfo.m_startFrameTime);
			}
		}

		m_CameraResponseQueue.AddTail(SVCameraQueueElement(pCamera, rCameraInfo));
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
		::OutputDebugString(SvUl::Format(_T("%s Finished Camera Acquisition %s\n"), GetName(), pCamera->GetName()).c_str());
#endif
		m_AsyncProcedure.Signal(nullptr);
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
		::OutputDebugString(SvUl::Format(_T("%s Finished Trigger TRI=%d\n"), GetName(), triggerInfo.lTriggerCount).c_str());
#endif

		m_AsyncProcedure.Signal(nullptr);
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
	product.m_pPPQ = this;

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

void CALLBACK SVPPQObject::OutputTimerCallback(UINT, UINT, DWORD_PTR dwUser, DWORD_PTR, DWORD_PTR)
{
	SVPPQObject* pPPQ = reinterpret_cast<SVPPQObject*>(dwUser);

	if (nullptr != pPPQ)
	{
		if (pPPQ->m_AsyncProcedure.IsActive())
		{
			pPPQ->NotifyProcessTimerOutputs();
		}
	}
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
				SVInspectionInfoPair l_Info(rProduct.ProcessCount(), rProduct.m_svInspectionInfos[pInspection->getObjectId()]);

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

void CALLBACK SVPPQObject::APCThreadProcess(ULONG_PTR)
{
}

void SVPPQObject::ThreadProcess(bool& p_WaitForEvents)
{
	//PPQ thread will only start when in run or test mode
	bool processed {!m_bOnline};

	if (!processed)
	{
		ProcessTrigger(processed);
	}

	if (!processed)
	{
		ProcessDelayOutputs(processed);
	}

	if (!processed)
	{
		ProcessResetOutputs(processed);
	}

	if (!processed)
	{
		ProcessDataValidDelay(processed);
	}

	if (!processed)
	{
		// Inserts items onto the m_oCamerasQueue.
		ProcessCameraResponses(processed);
	}

	if (!processed)
	{
		ProcessNotifyInspections(processed);
	}

	if (!processed)
	{
		// will only execute if 0 == m_oTriggerQueue.size().
		processed = processInspections();
	}

	if (!processed)
	{
		ProcessCompleteInspections(processed);
	}

	if (!processed)
	{
		ProcessProductRequests(processed);
	}

	p_WaitForEvents = !processed;
}

HRESULT SVPPQObject::NotifyProcessTimerOutputs()
{
	HRESULT l_Status = S_OK;

	bool signalThread = false;

	double currentTime = SvUl::GetTimeStamp();
	if (0 < m_outputDelay)
	{
		if (0.0 < m_NextOutputDelayTimestamp && m_NextOutputDelayTimestamp <= currentTime)
		{
			signalThread = true;
		}
	}

	if (false == signalThread && 0 < m_resetDelay)
	{
		if (0.0 < m_NextOutputResetTimestamp && m_NextOutputResetTimestamp <= currentTime)
		{
			signalThread = true;
		}
	}

	if (false == signalThread && 0 < m_DataValidDelay)
	{
		if (0.0 < m_NextDataValidDelayTimestamp && m_NextDataValidDelayTimestamp <= currentTime)
		{
			signalThread = true;
		}
	}

	if (signalThread)
	{
		m_AsyncProcedure.Signal(nullptr);
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessTrigger(bool& rProcessed)
{
	HRESULT l_Status = S_OK;

	/// Works through the queued triggers (e.g., pushed by FinishTrigger()).
	/// This is done by taking a SVTriggerQueueElement from the head of m_oTriggerQueue 
	/// and using the information in the SVTriggerInfoStruct therein to "create" a new product
	/// (the ProcessCount of which will be added to m_oNotifyInspectionsSet).
	/// In NextTriggerMode the results of the oldest product will be output through IndexPPQ().
	/// m_CameraInputData will contain trigger-data including the data index 
	rProcessed = (0 < m_oTriggerQueue.size());

	if (rProcessed)
	{
		SvTrig::SVTriggerInfoStruct poppedFromQueue;

		if (S_OK == m_oTriggerQueue.PopHead(poppedFromQueue))
		{
			if (m_bOnline)
			{
				SVProductInfoStruct* pProduct = IndexPPQ(std::move(poppedFromQueue));

				if (nullptr != pProduct)
				{
					InitializeProduct(pProduct);
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
					::OutputDebugString(SvUl::Format(_T("%s Process Trigger TRI=%d, ProdActive=%d\n"), GetName(), pProduct->ProcessCount(), pProduct->IsProductActive()).c_str());
#endif


					if (nullptr != m_spTiggercount)
					{
						m_spTiggercount->setValue(pProduct->m_triggerInfo.lTriggerCount);
					}

					m_oNotifyInspectionsSet.insert(pProduct->ProcessCount());

					// Get Shared Memory Slot
					if (HasActiveMonitorList() && GetSlotmanager().get())
					{
						try
						{

							long writerslot = GetSlotmanager()->GetNextWriteSlot();
							//TRACE("Get Writeslot %i\n", writerslot);
							pProduct->m_monitorListSMSlot = writerslot;
							for (auto& mapElement : pProduct->m_svInspectionInfos)
							{
								mapElement.second.m_lastInspectedSlot = writerslot;
							}
						}
						catch (const std::exception& e)
						{
							SvDef::StringVector msgList;
							msgList.push_back(e.what());
							SvStl::MessageManager Exception(SvStl::MsgType::Log);
							Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ProcessTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15026);
						}
						catch (...)
						{
							SvDef::StringVector msgList;
							msgList.push_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_Unknown));
							SvStl::MessageManager Exception(SvStl::MsgType::Log);
							Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ProcessTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15027);
						}
					} // if (HasActiveMonitorList())
				}
			} // if( m_bOnline )
		} // if( S_OK == m_oTriggerQueue.PopHead( poppedFromQueue )  )
		else
		{
			l_Status = E_FAIL;
		}
	} // if( p_rProcessed )

	return l_Status;
}

HRESULT SVPPQObject::ProcessNotifyInspections(bool& rProcessed)
{
	/// Does nothing if the trigger queue is nonempty or the m_oNotifyInspectionsSet is empty.
	/// Otherwise works through m_oNotifyInspectionsSet trying to notify one (and only one) inspection.
	HRESULT l_Status = S_OK;

	size_t	triggerQueueSize = m_oTriggerQueue.size();

	rProcessed = (0 < triggerQueueSize);

	if (!rProcessed) // if there are no triggers remaining on the queue.
	{

		// Currently, 
		// SVPPQOjbect::AssignInputs () - called from ProcessTrigger (),
		// SVPPQOjbect::ProcessTrigger ()
		// can add items to m_oNotifyInspectionSet.
		rProcessed = (0 < m_oNotifyInspectionsSet.size());

		if (rProcessed) // if there is a request to notify.
		{
			rProcessed = false;

			SVProcessCountSet::iterator l_Iter(m_oNotifyInspectionsSet.begin());

			// if the offset is outside the range of the PPQ, then execute 
			// this while statement.  The offset is initially forced outside 
			// the range in order to force the first execution.  
			do
			{
				long l_ProcessCount = *l_Iter;

				long l_Offset = m_PPQPositions.GetIndexByTriggerCount(l_ProcessCount);

				rProcessed = (0 <= l_Offset && l_Offset < static_cast<long> (m_PPQPositions.size()));

				if (true == rProcessed) // if offset for element is within PPQ.
				{
					l_Status = NotifyInspections(l_Offset);

					if (S_OK == l_Status)
					{
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
						::OutputDebugString(SvUl::Format(_T("%s Removed Notify Inspection TRI=%d\n"), GetName(), *l_Iter).c_str());
#endif
						// event has been placed on the inspection queue and
						// should be removed from the NotifyInspectionQueue.
						// Processed is already set to true.
						l_Iter = m_oNotifyInspectionsSet.erase(l_Iter);
					}
					else if (S_FALSE == l_Status)
					{
						// this means that inspection was not notified 
						// because inputs or image were not ready.  
						// proceed to next inspection.
						++l_Iter;
						rProcessed = false;
						l_Status = S_OK;
					}
				}
				else // of offset for element is outside PPQ (fell off/dead).
				{
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
					::OutputDebugString(SvUl::Format(_T("%s Removed Notify Inspection TRI=%d\n"), GetName(), *l_Iter).c_str());
#endif
					// product is dead and process should be killed here 
					// so as to now waste inspection time.
					// processed is already false.
					l_Iter = m_oNotifyInspectionsSet.erase(l_Iter);
				}

			} while ((true != rProcessed) &&
				(l_Iter != m_oNotifyInspectionsSet.end()) &&
				SUCCEEDED(l_Status));


		} // if there is an item on the notify inspection queue.

	} // if there are no triggers on trigger event queue.

	return l_Status;
}

bool SVPPQObject::processInspections()
/// Does nothing if there is at least one trigger in the trigger queue.
/// Otherwise, extracts all the IDs from m_ProcessInspectionsSet and starts the 
/// corresponding inspections via StartInspection().
/// They will be completed asynchronously in the class SVInspectionProcess.
{
	bool processed = (0 < m_oTriggerQueue.size());

	if (!processed)
	{
		processed = (0 < m_ProcessInspectionsSet.size());

		if (processed)
		{
			for (const auto& rId : m_ProcessInspectionsSet)
			{
				StartInspection(rId);
			}
			m_ProcessInspectionsSet.clear();
		}
	}

	return processed;
}

HRESULT SVPPQObject::ProcessResetOutputs(bool& rProcessed)
/// Removes the "head process count" from the outputs reset queue
/// and determines from it the corresponding product pointer.
/// If there is a new "head process count" in the outputs reset queue:
/// Determines if the corresponding product's m_EndResetDelay has already been reached.
/// If so, calls ResetOutputs().
/// If not, puts the original "head process count" back onto the outputs reset queue
{
	HRESULT l_Status = S_OK;

	rProcessed = (0 < m_oTriggerQueue.size());

	if (!rProcessed)
	{
		rProcessed = (0 < m_oOutputsResetQueue.GetCount());

		if (rProcessed)
		{
			rProcessed = false;
			SVProductInfoStruct* pProduct {nullptr};

			double currentTime = SvUl::GetTimeStamp();

			while (0 < m_oOutputsResetQueue.GetCount())
			{
				long processCount {0L};

				if (m_oOutputsResetQueue.GetHead(&processCount))
				{
					pProduct = GetProductInfoStruct(processCount);
				}
				else
				{
					l_Status = E_FAIL;

					break;
				}

				if (nullptr != pProduct)
				{
					if (currentTime >= pProduct->m_outputsInfo.m_EndResetDelay)
					{
						m_oOutputsResetQueue.RemoveHead(&processCount);
						rProcessed = true;
						m_NextOutputResetTimestamp = 0.0;
					}
					else
					{
						m_NextOutputResetTimestamp = pProduct->m_outputsInfo.m_EndResetDelay;
						break;
					}
				}
				else
				{
					m_oOutputsResetQueue.RemoveHead(&processCount);
				}
			}

			if (rProcessed)
			{
				ResetOutputs();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessDataValidDelay(bool& rProcessed)
{
	HRESULT Status = S_OK;

	rProcessed = (0 < m_oTriggerQueue.size());

	if (!rProcessed)
	{
		rProcessed = (0 < m_DataValidDelayQueue.GetCount());

		if (rProcessed)
		{
			SVProductInfoStruct* pProduct(nullptr);

			double CurrentTime = SvUl::GetTimeStamp();

			while (0 < m_DataValidDelayQueue.GetCount() && nullptr == pProduct)
			{
				long ProcessCount = 0;

				if (m_DataValidDelayQueue.RemoveHead(&ProcessCount))
				{
					pProduct = GetProductInfoStruct(ProcessCount);
				}
				else
				{
					Status = E_FAIL;
					break;
				}

				if (nullptr != pProduct && 0 < m_DataValidDelayQueue.GetCount())
				{
					SVProductInfoStruct* pNextProduct(nullptr);

					if (m_DataValidDelayQueue.GetHead(&ProcessCount))
					{
						pNextProduct = GetProductInfoStruct(ProcessCount);
					}
					if (nullptr != pNextProduct && pNextProduct->m_outputsInfo.m_EndDataValidDelay <= CurrentTime)
					{
						pProduct = nullptr;
					}
				}

				if (nullptr != pProduct)
				{
					if (CurrentTime < pProduct->m_outputsInfo.m_EndDataValidDelay)
					{
						m_NextDataValidDelayTimestamp = pProduct->m_outputsInfo.m_EndDataValidDelay;

						m_DataValidDelayQueue.AddHead(pProduct->ProcessCount());

						pProduct = nullptr;
						break;
					}
				}
			}

			rProcessed = (nullptr != pProduct);

			if (rProcessed)
			{
				m_NextDataValidDelayTimestamp = 0.0;
				if (nullptr != m_pDataValid)
				{
					m_pOutputList->WriteOutputValue(m_pDataValid, pProduct->m_outputsInfo.m_DataValidResult);
				}
				if (nullptr != m_pOutputToggle)
				{
					m_pOutputList->WriteOutputValue(m_pOutputToggle, pProduct->m_outputsInfo.m_OutputToggleResult);
				}
			}
			else
			{
				Status = E_FAIL;
			}
		}
	}

	return Status;
}

HRESULT SVPPQObject::ProcessCameraResponses(bool& rProcessed)
/// If camera responses pending AND trigger queue empty:
/// Removes camera responses from m_PendingCameraResponses and tries to process them
/// until the first processing succeeds.
/// If no responses processed: Removes the head of m_CameraResponseQueue and processes that
/// until one processing attempt is successful or m_CameraResponseQueue is empty.
{
	HRESULT l_Status = S_OK;
	rProcessed = false;

	if (0 < m_PendingCameraResponses.size())
	{
		SVCameraInfoMap::const_iterator l_CameraIter = m_Cameras.begin();

		while (S_OK == l_Status && l_CameraIter != m_Cameras.end() && !rProcessed)
		{
			// stop processing camera responses if there is an element on the trigger queue.
			if (0 >= m_oTriggerQueue.size())
			{
				SVPendingCameraResponseMap::iterator l_PendingIter = m_PendingCameraResponses.find(l_CameraIter->first);

				if (l_PendingIter != m_PendingCameraResponses.end())
				{
					SVCameraQueueElement l_Data = l_PendingIter->second;

					m_PendingCameraResponses.erase(l_PendingIter);

					SVObjectManagerClass::Instance().DecrementPendingImageIndicator();

					rProcessed = (S_OK == ProcessCameraResponse(l_Data));

#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
					::OutputDebugString(SvUl::Format(_T("%s Processed Pending Camera Response %s\n"), GetName(), l_Data.m_pCamera->GetName()).c_str());
#endif
				}
			}

			++l_CameraIter;
		}
	} // if( 0 < m_PendingCameraResponses.size() )

	while (S_OK == l_Status && 0 < m_CameraResponseQueue.GetCount() && !rProcessed)
	{
		rProcessed = 0 < m_oTriggerQueue.size();
		if (0 >= m_oTriggerQueue.size())
		{
			SVCameraQueueElement l_Data;
			if (m_CameraResponseQueue.RemoveHead(&l_Data))
			{
				rProcessed = (S_OK == ProcessCameraResponse(l_Data));
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
				::OutputDebugString(SvUl::Format(_T("%s Processed Camera Response %s\n"), GetName(), l_Data.m_pCamera->GetName()).c_str());
#endif
			}
			else
			{
				l_Status = E_FAIL;

				break;
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessCompleteInspections(bool& rProcessed)
/// Does nothing unless m_oTriggerQueue is empty and m_oInspectionQueue is non-empty.
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
	HRESULT l_Status = S_OK;

	rProcessed = (0 < m_oTriggerQueue.size());

	if (!rProcessed)
	{
		rProcessed = (0 < m_oInspectionQueue.GetCount());

		if (rProcessed)
		{
			SVInspectionInfoPair l_Info;
			SVProductInfoStruct* l_pPPQProduct = nullptr;
			long l_PPQIndex = static_cast<long>(m_PPQPositions.size());

			for (int i = 0; i < m_oInspectionQueue.GetCount(); ++i)
			{
				if (m_oInspectionQueue.GetAt(i, &l_Info))
				{
					if (nullptr != l_Info.second.m_pInspection)
					{
						m_ProcessInspectionsSet.insert(l_Info.second.m_pInspection->getObjectId());
					}
				}
			}

			if (!(m_ProcessInspectionsSet.empty()))
			{
				processInspections();
			}

			while (0 < m_oInspectionQueue.GetCount() && nullptr == l_pPPQProduct)
			{
				if (m_oInspectionQueue.RemoveHead(&l_Info))
				{
					l_PPQIndex = m_PPQPositions.GetIndexByTriggerCount(l_Info.first);

					if (0 <= l_PPQIndex && l_PPQIndex < static_cast<long>(m_PPQPositions.size()))
					{
						l_pPPQProduct = m_PPQPositions.GetProductAt(l_PPQIndex);
					}
				}
				else
				{
					l_Status = E_FAIL;

					break;
				}
			}

			rProcessed = (nullptr != l_pPPQProduct);

			if (rProcessed)
			{
				if (l_pPPQProduct->m_outputsInfo.m_EndProcess == 0.0)
				{
					SVInspectionInfoStruct& l_rInspectInfo = l_Info.second;
					SVInspectionInfoStruct& l_rPPQInspectInfo = l_pPPQProduct->m_svInspectionInfos[l_rInspectInfo.m_pInspection->getObjectId()];

					l_rPPQInspectInfo = l_rInspectInfo;

					l_rPPQInspectInfo.ClearIndexes();

#ifdef EnableTracking
					std::string l_Title = l_rInspectInfo.m_pInspection->GetName();

					l_Title += _T(" Complete");


					m_PPQTracking.IncrementCount(l_Title, l_PPQIndex);
#endif

					// Inspection Process is done, let everyone know.
					if (!SetInspectionComplete(*l_pPPQProduct, l_rInspectInfo.m_pInspection->getObjectId()))
					{
						l_Status = E_FAIL;
					}
				}

				if (l_pPPQProduct->m_dataComplete)
				{
					SetProductComplete(l_PPQIndex);

					for (size_t i = l_PPQIndex + 1; i < m_PPQPositions.size(); ++i)
					{
						l_pPPQProduct = m_PPQPositions.GetProductAt(l_PPQIndex);

						if (nullptr != l_pPPQProduct && l_pPPQProduct->IsProductActive())
						{
							SetProductIncomplete(static_cast<long>(i));
#if defined (TRACE_THEM_ALL) || defined (TRACE_PPQ)
							::OutputDebugString(SvUl::Format(_T("%s Product incomplete TRI=%d\n"), GetName(), l_pPPQProduct->ProcessCount()).c_str());
#endif
						}
						else
						{
							break;
						}
					}
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessProductRequests(bool& rProcessed)
/// Does nothing unless both m_oTriggerQueue and m_ProductRequests are nonempty.
/// Otherwise extracts the first SVProductRequestPair from m_ProductRequests,
/// gets the SVProductInfoStruct specified in the request.
/// Then copies the SVProductInfoStruct into the m_pProduct member of the SVProductInfoRequestStruct
/// contained in the request pair.
{
	HRESULT l_Status = S_OK;

	rProcessed = (0 < m_oTriggerQueue.size());

	if (!rProcessed)
	{
		rProcessed = (0 < m_ProductRequests.GetCount());

		if (rProcessed)
		{
			SVProductRequestPair l_Request;

			if (m_ProductRequests.RemoveHead(&l_Request))
			{
				if (nullptr != l_Request.second.m_pProduct)
				{
					SVProductInfoStruct* pProduct = GetProductInfoStruct(l_Request.first);

					if (nullptr != pProduct)
					{
						*(l_Request.second.m_pProduct) = *(pProduct);
					}
					else
					{
						l_Request.second.m_pProduct->InitProductInfo();
					}
				}

				l_Status = l_Request.second.NotifyRequestComplete();
			}
			else
			{
				l_Status = E_FAIL;
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::GetProduct(SVProductInfoStruct& p_rProduct, long lProcessCount) const
{
	HRESULT l_Status = S_OK;

	p_rProduct.InitProductInfo();

	if (m_AsyncProcedure.IsActive() && m_ProductRequests.IsCreated())
	{
		SVProductInfoStruct* l_pProduct = new SVProductInfoStruct;

		if (nullptr != l_pProduct)
		{
			SVProductRequestPair l_Request(lProcessCount, SVProductInfoRequestStruct(l_pProduct));

			if (m_ProductRequests.AddTail(l_Request))
			{
				m_AsyncProcedure.Signal(nullptr);

				l_Status = l_Request.second.WaitForRequest();

				if (S_OK == l_Status)
				{
					p_rProduct = *l_pProduct;

					if (p_rProduct.ProcessCount() < 1)
					{
						l_Status = E_FAIL;
					}
				}
			}
			else
			{
				l_Status = E_FAIL;
			}

			delete l_pProduct;
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
	for (const auto& pEntry : GetAllInputs())
	{
		if (nullptr != pEntry)
		{
			l_svName = SvUl::Format(SvXml::CTAGF_INPUT_X, lInputNr);
			rWriter.StartElement(l_svName.c_str());

			switch (pEntry->m_ObjectType)
			{
				case IO_DIGITAL_INPUT:
				{
					l_svValue.SetString(SvXml::cDigitalType);
					rWriter.WriteAttribute(SvXml::CTAG_IO_TYPE, l_svValue);
					l_svValue.Clear();

					l_svValue.SetString(pEntry->getObject()->GetName());
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

					l_svValue.SetString(pEntry->getObject()->GetName());
					rWriter.WriteAttribute(SvXml::CTAG_ITEM_NAME, l_svValue);
					l_svValue.Clear();

					SVInputObjectPtr pInput = m_pInputList->GetInput(pEntry->m_IOId);
					if (nullptr != pInput)
					{
						l_svValue = pInput->GetChannel();
						rWriter.WriteAttribute(SvXml::CTAG_REMOTE_INDEX, l_svValue);
						l_svValue.Clear();
					}

					l_svValue = pEntry->m_PPQIndex;
					rWriter.WriteAttribute(SvXml::CTAG_PPQ_POSITION, l_svValue);
					l_svValue.Clear();

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

void SVPPQObject::SetSlotmanager(const SvSml::RingBufferPointer& Slotmanager)
{
	if (m_SlotManager.get())
	{
		m_SlotManager.reset();
	}
	m_SlotManager = Slotmanager;
}
SvSml::RingBufferPointer SVPPQObject::GetSlotmanager()
{
	return m_SlotManager;
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


#ifdef EnableTracking
#pragma region SVPPQTrackingElement Constructor
SVPPQObject::SVPPQTrackingElement::~SVPPQTrackingElement()
{
	clear();
}
#pragma endregion SVPPQTrackingElement Constructor

void SVPPQObject::SVPPQTrackingElement::clear()
{
	if (!(m_TrackedCounts.empty()))
	{
		m_TrackedCounts.clear();
	}
}

void SVPPQObject::SVPPQTrackingElement::IncrementCount(size_t p_Index, size_t p_VectorSize)
{
	if (p_Index < p_VectorSize)
	{
		if (m_TrackedCounts.size() != p_VectorSize)
		{
			m_TrackedCounts.resize(p_VectorSize, 0);
		}

		++(m_TrackedCounts[p_Index]);
	}
}

#pragma region SVPPQTracking Constructor
SVPPQObject::SVPPQTracking::~SVPPQTracking()
{
	clear();
}
#pragma endregion SVPPQTracking Constructor

void SVPPQObject::SVPPQTracking::clear()
{
	m_QueueLength = 0;

	if (!(m_Counts.empty()))
	{
		m_Counts.clear();
	}

	if (!(m_QueueCounts.empty()))
	{
		m_QueueCounts.clear();
	}
	if (!(m_QueueWriteTimeCounts.empty()))
	{
		m_QueueWriteTimeCounts.clear();
	}
}

void SVPPQObject::SVPPQTracking::IncrementCount(const std::string& p_rName)
{
	++(m_Counts[p_rName]);
}

void SVPPQObject::SVPPQTracking::IncrementCount(const std::string& p_rName, size_t p_Index)
{
	m_QueueCounts[p_rName].IncrementCount(p_Index, m_QueueLength);
}

#endif //EnableTracking

void SVPPQObject::ResetOutputValueObjects()
{
	m_PpqOutputs[PpqOutputEnums::ACK].SetValue(BOOL(false));
	m_PpqOutputs[PpqOutputEnums::NAK].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::MasterFault].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::MasterWarning].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::NotInspected].SetValue(BOOL(true));
	m_PpqOutputs[PpqOutputEnums::DataValid].SetValue(BOOL(false));
}

// This method is for releasing the writer lock for Inspections when the product is incomplete
void SVPPQObject::ReleaseSharedMemory(SVProductInfoStruct& rProduct)
{
	if (HasActiveMonitorList())
	{
		try
		{
			long shareSlotIndex = rProduct.m_monitorListSMSlot;
			if (shareSlotIndex >= 0 && GetSlotmanager().get())
			{
				GetSlotmanager()->ReleaseWriteSlot(shareSlotIndex, rProduct.ProcessCount(), true);
				rProduct.m_monitorListSMSlot = -1;
			}

		}
		catch (const std::exception& e)
		{
			SvDef::StringVector msgList;
			msgList.push_back(e.what());
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ReleaseProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15029);
		}
		catch (...)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_Unknown));
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ReleaseProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15030);
		}
	}
}

// This method writes the inspection info to the PPQ shared memory when the product is Complete
void SVPPQObject::CommitSharedMemory(SVProductInfoStruct& rProduct)
{
	if (HasActiveMonitorList())
	{
		try
		{
			long shareSlotIndex = rProduct.m_monitorListSMSlot;
			if (shareSlotIndex >= 0 && GetSlotmanager().get())
			{
				GetSlotmanager()->ReleaseWriteSlot(shareSlotIndex, rProduct.ProcessCount(), true);
				rProduct.m_monitorListSMSlot = -1;
			}

		}
		catch (const std::exception& e)
		{
			SvDef::StringVector msgList;
			msgList.push_back(e.what());
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_CommitSharedMemory, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15032);
		}
		catch (...)
		{
			SvDef::StringVector msgList;
			msgList.push_back(SvStl::MessageData::convertId2AdditionalText(SvStl::Tid_Unknown));
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_CommitSharedMemory, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15033);
		}
	}
}

DWORD SVPPQObject::GetObjectColor() const
{
	return SvDef::DefaultWhiteColor;
}

bool SVPPQObject::setRejectDepth(long depth, SvStl::MessageContainerVector* pErrorMessages/*=nullptr */)
{
	assert(depth >= 0);
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
	m_pMasterProductInfos = new SVProductInfoStruct[getPPQLength() + g_lPPQExtraBufferSize];

	for (int j = 0; j < getPPQLength() + g_lPPQExtraBufferSize; j++)
	{
		m_pMasterProductInfos[j].m_pPPQ = this;
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

		assert(pInspList->end() != pInspPB);
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
		bool isReject {false};
		auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
		if (nullptr != pTrc)
		{
			isReject = pTrc->setTrsOfInterest(trVec, isInterest);
		}
		if (HasActiveMonitorList())
		{
			long slotindex = rProduct.m_monitorListSMSlot;

			if (rProduct.ProcessCount() > 0 && slotindex >= 0)
			{
				if (isReject)
				{
					GetSlotmanager()->SetToReject(slotindex);
				}
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(std::format("Error in getting next Slot  {}, Processcount: {}", slotindex, rProduct.ProcessCount()));
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams));
			}
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