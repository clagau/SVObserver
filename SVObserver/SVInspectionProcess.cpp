//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInspectionProcess
//* .File Name       : $Workfile:   SVInspectionProcess.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.26  $
//* .Check In Date   : $Date:   19 Jan 2015 11:41:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <algorithm>
#include "SVInspectionProcess.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVIOLibrary/SVInputObject.h"
#include "SVObjectLibrary/SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"

#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVUtilityLibrary/NaturalStringCompare.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVObserver.h"
#include "SVToolSet.h"
#include "SVPPQObject.h"
#include "Tools/SVTool.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVConditional.h"
#include "SVStatusLibrary/SvimState.h"
#include "SVStatusLibrary/MessageManager.h"
#include "TextDefinesSvO.h"
#include "Definitions/Color.h"
#include "SVSharedMemoryLibrary\SharedMemWriter.h"
#include "SVOGuiUtility/FormulaController.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVLogLibrary/Logging.h"
#include "SVOGuiUtility/SVGlobal.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVValueObjectLibrary/SVFileNameValueObjectClass.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVUtilityLibrary/VectorHelper.h"
#include "SVStatusLibrary/MessageManagerHelper.h"
#pragma endregion Includes

//#define TRACE_TRC
#ifdef TRACE_RESETALL	

std::unordered_map<int, int> ResetImageIds;
std::unordered_map<int, int> ResetIds;
std::unordered_map<int, std::string> IdsName;
#endif 

SV_IMPLEMENT_CLASS(SVInspectionProcess, SvPb::InspectionProcessClassId);

void fillList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> result, const SVObjectReferenceVector& rObjectVector, SvPb::ObjectSelectorType type)
{
	for (auto ObjectRef : rObjectVector)
	{
		if (nullptr != ObjectRef.getObject() && ObjectRef.getObject()->isCorrectType(type))
		{
			SvPb::TreeItem insertItem;
			insertItem.set_name(ObjectRef.GetName());
			if (nullptr != ObjectRef.getValueObject())
			{
				insertItem.set_type(ObjectRef.getValueObject()->getTypeName());
			}
			if (ObjectRef.isArray())
			{
				// add array elements
				int iArraySize = ObjectRef.getValueObject()->getArraySize();
				for (int i = 0; i < iArraySize; i++)
				{
					ObjectRef.SetArrayIndex(i);
					insertItem.set_location(ObjectRef.GetCompleteName(true));
					insertItem.set_objectidindex(ObjectRef.GetObjectIdAndIndexOneBased());
					result = insertItem;
				}
			}
			else
			{
				insertItem.set_location(ObjectRef.GetCompleteName(true));
				insertItem.set_objectidindex(ObjectRef.GetObjectIdAndIndexOneBased());
				result = insertItem;
			}
		}
	}
}

bool isProductAlive(SVPPQObject* pPPQ, long triggerCount)
{
	bool l_Status = !SvimState::CheckState(SV_STATE_RUNNING);

	if (!l_Status && nullptr != pPPQ)
	{
		l_Status = pPPQ->IsProductAlive(triggerCount);
	}

	return l_Status;
}

//////////////////////////////////////////////////////////////////////////
//asums  input is a string or Safearray variant 
std::string GetStringFromInputRequest(const _variant_t& rValue)
{
	std::string retValue;

	if (VT_ARRAY == (rValue.vt & VT_ARRAY))
	{
		SvUl::SVSAFEARRAY::SVBounds Bounds;
		SvUl::SVSAFEARRAY SafeArray {rValue};

		SafeArray.GetBounds(Bounds);

		if (1 == SafeArray.size())
		{
			_variant_t TempVariant;

			SafeArray.GetElement(Bounds[0].lLbound, TempVariant);

			retValue = SvUl::createStdString(TempVariant);
		}
		else if (1 < SafeArray.size())
		{
			for (size_t i = 0; i < Bounds[0].cElements; ++i)
			{
				_variant_t TempVariant;

				SafeArray.GetElement((Bounds[0].lLbound + i), TempVariant);

				if (i > 0)
				{
					retValue += _T(",");
				}
				retValue += std::format(_T("`{}`"), SvUl::createStdString(TempVariant).c_str());
			}
		}
	}
	else
	{
		retValue = SvUl::createStdString(rValue);
	}
	return retValue;
}
//////////////////////////////////////////////////////////////////////////
HRESULT SVInspectionProcess::ProcessInspection()
{
	HRESULT l_Status = S_OK;

	SVProductInfoStruct product;

	{
		std::lock_guard<std::mutex>  Autolock(m_inspectionMutex);
		product = std::move(m_product);
		m_product.Reset();
	}
	if (false == product.m_triggered)
	{
		m_processActive = false;
	}

	if (m_processActive)
	{
		// Get the info struct for this inspection
		const auto iter = product.m_svInspectionInfos.find(getObjectId());
		bool validProduct = product.m_svInspectionInfos.end() != iter || false == product.m_triggered;
		validProduct = (false == validProduct) ? false : iter->second.m_InProcess && iter->second.m_HasBeenQueued;
		if (false == validProduct)
		{
			return E_FAIL;
		}
		SVInspectionInfoStruct& rIPInfo = iter->second;
		rIPInfo.m_BeginInspection = SvUl::GetTimeStamp();

		/// Note this is the PPQ position of the previous trigger
		m_pCurrentToolset->setPpqPosition(product.m_lastPPQPosition);
		double triggerTimeStamp = product.m_triggerInfo.m_triggerTimeStamp;
		double time {0.0};
		if (product.m_triggerInfo.m_PreviousTrigger > 0.0)
		{
			time = (triggerTimeStamp - product.m_triggerInfo.m_PreviousTrigger) * SvUl::c_MicrosecondsPerMillisecond;
		}
		m_pCurrentToolset->setTime(time, ToolSetTimes::TriggerDelta);

		time = (rIPInfo.m_BeginInspection - triggerTimeStamp) * SvUl::c_MicrosecondsPerMillisecond;
		m_pCurrentToolset->setTime(time, ToolSetTimes::TriggerToStart);

		double triggerToAcqTime {0.0};
		double acqTime {0.0};

		m_pCurrentToolset->setMissingImageCount(product.m_MissingImageCount);
		m_pCurrentToolset->setNotCompleteCount(product.m_NotCompleteCount);

		if (nullptr != m_pToolSetCamera)
		{
			SvIe::SVObjectIdSVCameraInfoStructMap::const_iterator iterCamera(product.m_svCameraInfos.find(m_pToolSetCamera->getObjectId()));
			if (product.m_svCameraInfos.cend() != iterCamera)
			{
				const CameraInfo& rCameraInfo = iterCamera->second.getCameraInfo();
				triggerToAcqTime = (rCameraInfo.m_startFrameTimestamp - triggerTimeStamp) * SvUl::c_MicrosecondsPerMillisecond;
				acqTime = (rCameraInfo.m_endFrameTimestamp - rCameraInfo.m_startFrameTimestamp) * SvUl::c_MicrosecondsPerMillisecond;

				if (VT_EMPTY != rCameraInfo.m_cameraData[CameraDataEnum::ChunkFrameID].vt)
				{
					SvVol::BasicValueObjectPtr pFrameID = m_pToolSetCamera->getCameraValue(SvDef::FqnCameraFrameID);
					if (nullptr != pFrameID)
					{
						pFrameID->setValue(rCameraInfo.m_cameraData[CameraDataEnum::ChunkFrameID].lVal);
					}
				}
				if (VT_EMPTY != rCameraInfo.m_cameraData[CameraDataEnum::ChunkTimeStamp].vt)
				{
					SvVol::BasicValueObjectPtr pTimestamp = m_pToolSetCamera->getCameraValue(SvDef::FqnCameraTimestamp);
					if (nullptr != pTimestamp)
					{
						pTimestamp->setValue(rCameraInfo.m_cameraData[CameraDataEnum::ChunkTimeStamp].ulVal);
					}
				}
				if (VT_EMPTY != rCameraInfo.m_cameraData[CameraDataEnum::ChunkLineStatusAll].vt)
				{
					SvVol::BasicValueObjectPtr pLineStatusAll = m_pToolSetCamera->getCameraValue(SvDef::FqnCameraLineStatusAll);
					if (nullptr != pLineStatusAll)
					{
						pLineStatusAll->setValue(rCameraInfo.m_cameraData[CameraDataEnum::ChunkLineStatusAll].lVal);
					}
				}
			}
		}

		m_pCurrentToolset->setTime(triggerTimeStamp * SvUl::c_MicrosecondsPerMillisecond, ToolSetTimes::TriggerTimeStamp);
		//If no tool set camera was found TriggerToAcquisitionStart and AcquisitionTime will be set to 0
		m_pCurrentToolset->setTime(triggerToAcqTime, ToolSetTimes::TriggerToAcquisitionStart);
		m_pCurrentToolset->setTime(acqTime, ToolSetTimes::AcquisitionTime);
		m_pCurrentToolset->setTriggerData(product.m_triggerInfo.m_Data);

		Log_Assert(m_InputObjects.size() == product.m_triggerInfo.m_Inputs.size());
		// Copy inputs to Inspection Process's Value objects
		size_t inputXferCount {0};
		for (size_t i = 0; i < m_InputObjects.size(); i++)
		{
			auto& rInputEntry = m_InputObjects[i];
			const auto& rInputValue = product.m_triggerInfo.m_Inputs.at(i);

			//Is the input enabled
			if (nullptr != rInputEntry)
			{
				_variant_t value;
				rInputEntry->getValue(value);
				//Only use  the input value types when they match
				if (value.vt == rInputValue.vt)
				{
					rInputEntry->setValue(rInputValue);
					++inputXferCount;
				}
			}
			else
			{
				++inputXferCount;
			}
		}

		bool processed = (inputXferCount == m_InputObjects.size()) && (isProductAlive(GetPPQ(), product.m_triggerInfo.lTriggerCount));

		// Check if tool set is alive and enabled...
		if (false == processed)
		{
			// Product has left the PPQ ... don't inspect
			rIPInfo.m_InspectedState = PRODUCT_NOT_INSPECTED;
		}
		else
		{
			// The Toolset will return false if it did not run due to a Conditional
			// That means: true  - The Tool Set were running.
			//			   false - The Tool Set Condition failed and the Tool Set were
			//					   NOT running!
			RunInspection(rIPInfo, product.m_svCameraInfos, product.m_triggerInfo.lTriggerCount);

			if (PRODUCT_INSPECTION_NOT_RUN != (rIPInfo.m_InspectedState & PRODUCT_INSPECTION_NOT_RUN))
			{
				rIPInfo.m_InspectedState = GetToolSet()->GetResultList()->GetInspectionState();

				rIPInfo.m_EndInspection = SvUl::GetTimeStamp();
				rIPInfo.setTriggerRecordCompleted();

				time = (rIPInfo.m_EndInspection - triggerTimeStamp) * SvUl::c_MicrosecondsPerMillisecond;
				m_pCurrentToolset->setTime(time, ToolSetTimes::TriggerToCompletion);

#ifdef _DEBUG
				//					std::string l_TempStateString = std::format( _T( "SVInspectionProcess::ProcessInspection|{}|TRI={:d}\n" ),
				//						GetName(), p_rProduct.ProcessCount() );
				//					::OutputDebugString( l_TempStateString.c_str() );
#endif

				// Mark last run product
				rIPInfo.ClearIndexes();
				LastProductUpdate(product);
			}
		}
		SVObjectManagerClass::Instance().DecrementInspectionIndicator();

		if (processed)
		{
			rIPInfo.ClearIndexes();
			LastProductNotify();
		}
		m_processActive = false;
	}
	return l_Status;
}


void SVInspectionProcess::BuildWatchlist()
{
	m_WatchListDatas.clear();

	if (m_StoreIndex < 0 || GetPPQ() == nullptr)
	{
		return;
	}
	const SvSml::MonitorListCpy* pMonitorlist(nullptr);
	pMonitorlist = SvSml::SharedMemWriter::Instance().GetMonitorListCpyPointerForPPQ(GetPPQ()->GetName());
	if (!pMonitorlist)
	{
		return;
	}

	DWORD RejectConditionFlag = SvSml::ListFlags[SvSml::ListType::rejectCondition];
	for (auto& it : pMonitorlist->m_EntriesMap)
	{
		if (it.second->data.m_inspectionStoreId == m_StoreIndex)
		{
			if (it.second->data.ObjectType != SvPb::SVImageObjectType && (it.second->data.m_MonitorListFlag & RejectConditionFlag))
			{
				SVObjectReference ObjectRef;
				if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(it.first, ObjectRef))
				{

					if (false == it.second->data.wholeArray)
					{
						m_WatchListDatas.push_back(it.second);
					}
					else
					{
						SV_LOG_GLOBAL(error) << "whole array not implemented";
					}
				}
			}
		}
	}
}

bool SVInspectionProcess::isReject(SvOi::ITriggerRecordRPtr pTriggerRecord)
{
	if (nullptr == pTriggerRecord)
	{
		assert(false);
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RejectClarifyFailedNoTR, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	bool result(false);
	for (auto& element : m_WatchListDatas)
	{
		if (nullptr != element)
		{
			if (0 <= element->data.m_triggerRecordPos)
			{
				double val = pTriggerRecord->getDataValue(element->data.m_triggerRecordPos);
				if (val != 0)
				{
					result = true;
				}
			}
			else
			{
				assert(false);
				SvStl::MessageManager Msg(SvStl::MsgType::Log);
				Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RejectClarifyFailedUnknowData, {element->name}, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	return result;
}

HRESULT SVInspectionProcess::ProcessNotifyWithLastInspected()
{
	if (m_NotifyWithLastInspected)
	{
		m_NotifyWithLastInspected = false;

		SVPPQObject* pPPQ {GetPPQ()};
		if (nullptr != pPPQ && pPPQ->HasActiveMonitorList() && m_lastRunProduct.m_triggered)
		{
			m_lastRunProduct.m_svInspectionInfos[getObjectId()].m_bReject = isReject(m_lastRunProduct.m_svInspectionInfos[getObjectId()].m_triggerRecordComplete);
		}
		std::pair<long, SVInspectionInfoStruct> data {m_lastRunProduct.triggerCount(), m_lastRunProduct.m_svInspectionInfos[getObjectId()]};
		updateObserver(data);

		if (m_lastRunProduct.m_triggered && SvDef::InvalidObjectId != m_PPQId)
		{
			pPPQ->ObserverUpdate(data);
		}
		m_lastRunProduct.m_triggered = false;
	}
	return S_OK;
}

HRESULT SVInspectionProcess::ProcessCommandQueue()
{
	HRESULT result {S_OK};

	if (!(m_CommandQueue.IsEmpty()))
	{
		SvOi::ICommandPtr pCommand;

		if (m_CommandQueue.RemoveHead(&pCommand))
		{
			if (nullptr != pCommand)
			{
				result = pCommand->Execute();
			}
		}
	}

	return result;
}

SVInspectionProcess::SVInspectionProcess(LPCSTR ObjectName)
	: SVObjectClass(ObjectName)
{
	Init();
}

SVInspectionProcess::SVInspectionProcess(SVObjectClass* POwner, int StringResourceID)
	: SVObjectClass(POwner, StringResourceID)
{
	Init();
}

void SVInspectionProcess::Init()
{
	// Set up your type...
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVInspectionObjectType;
	m_svReset.RemoveState(SvDef::SVResetStateAll);
	SetObjectAttributesAllowed(SvPb::viewable, SvOi::SetAttributeType::AddAttribute);
}

SVInspectionProcess::~SVInspectionProcess()
{
	DestroyInspection();
	m_PPQId = SvDef::InvalidObjectId;
	m_WatchListDatas.clear();
	m_RegressionTestPlayEquation.CloseObject();
}

bool SVInspectionProcess::IsCameraInInspection(const std::string& rCameraName) const
{
	bool Result(false);
	SvIe::SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();
	while (!Result && l_Iter != m_CameraImages.end())
	{
		if (nullptr != (*l_Iter))
		{
			Result = (rCameraName == (*l_Iter)->GetCameraName());
		}
		++l_Iter;
	}
	return Result;
}

bool SVInspectionProcess::IsResetStateSet(unsigned long p_State) const
{
	return m_svReset.IsStateSet(p_State);
}

void SVInspectionProcess::AddResetState(unsigned long p_State)
{
	m_svReset.AddState(p_State);
}

void SVInspectionProcess::RemoveResetState(unsigned long p_State)
{
	m_svReset.RemoveState(p_State);
}

bool SVInspectionProcess::CreateInspection(LPCTSTR szDocName)
{
	SetName(szDocName);

	m_NotifyWithLastInspected = false;

	if (S_OK != m_processThread.Create(&SVInspectionProcess::ProcessCallback, GetName()))
	{
		return false;
	}

	m_pCurrentToolset = new SVToolSet(this);

	if (!CreateChildObject(m_pCurrentToolset))
	{
		return false;
	}

	return true;
}// end Create

void __stdcall SVInspectionProcess::ProcessCallback(ULONG_PTR pCaller)
{
	auto* pProcessFunction = reinterpret_cast<InspectionProcessFunction*> (pCaller);
	if (nullptr != pProcessFunction && nullptr != *pProcessFunction)
	{
		(*pProcessFunction)();
	}
}

void SVInspectionProcess::DestroyInspection()
{
	auto* pPPQ = GetPPQ();
	if (nullptr != pPPQ)
	{
		pPPQ->DetachInspection(this);
	}
	updateObserver(SVRemoveSubjectStruct());
	m_NotifyWithLastInspected = false;
	::Sleep(0);
	m_processThread.Destroy();
	resetLastProduct();
	if (nullptr != m_pCurrentToolset)
	{
		DestroyChildObject(m_pCurrentToolset);
	}
	if (m_processActive)
	{
		SVObjectManagerClass::Instance().DecrementInspectionIndicator();
		m_processActive = false;
	}
	for (auto& pInputValueObj : m_InputObjects)
	{
		SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (pInputValueObj.get());
		if (nullptr != pObject)
		{
			pObject->CloseObject();
		}
	}
	m_InputObjects.clear();
	m_PPQId = SvDef::InvalidObjectId;
	SVResultList* pResultlist = GetResultList();
	if (pResultlist)
	{
		pResultlist->RebuildReferenceVector(this);
	}
}// end Destroy

bool SVInspectionProcess::CanGoOnline()
{
	bool l_bOk(true);

	CWaitCursor l_cwcMouse;


	resetCounterSynchronous();
	requestResetAllObjects();

	m_svReset.AddState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	l_bOk = (S_OK == InitializeRunOnce());

	m_svReset.RemoveState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	m_StoreIndex = SvSml::SharedMemWriter::Instance().GetInspectionStoreId(GetName());

	if (0 <= m_StoreIndex)
	{
		if (0 > m_trcPos)
		{
			m_trcPos = SvOi::getInspectionPos(getObjectId());
		}
		SvSml::SharedMemWriter::Instance().addInspectionIdEntry(GetPPQ()->GetName(), m_StoreIndex, m_trcPos);
		auto* pTrc = SvOi::getTriggerRecordControllerRInstance();
		if (nullptr != pTrc)
		{
			const auto& rDataDefMap = pTrc->getDataDefMap(m_trcPos);
			const auto& rImageMap = pTrc->getImageDefMap(m_trcPos);
			const auto& rChildImageMap = pTrc->getChildImageDefMap(m_trcPos);
			const auto& rLinkedImageMap = pTrc->getLinkedImageDefMap(m_trcPos);
			SvSml::SharedMemWriter::Instance().setDataTrcPos(GetPPQ()->GetName(), m_StoreIndex, m_trcPos, rDataDefMap, rImageMap, rChildImageMap, rLinkedImageMap);
		}
	}

	return l_bOk;
}// end CanGoOnline

bool SVInspectionProcess::CanRegressionGoOnline()
{
	bool l_bOk(true);

	CWaitCursor l_cwcMouse;

	resetCounterSynchronous();
	requestResetAllObjects();
	m_svReset.AddState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	l_bOk = resetAllObjects();

	m_svReset.RemoveState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	return l_bOk;
}

bool SVInspectionProcess::GoOnline()
{
	resetLastProduct();
	m_offlineRequest = false;

	m_processThread.SetPriority(THREAD_PRIORITY_NORMAL);

	try
	{
		BuildWatchlist();
	}

	catch (const SvStl::MessageContainer& rSvE)
	{
		//Now that we have caught the exception we would like to display it
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(rSvE.getMessage());
		return false;
	}
	catch (std::exception& e)
	{

		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, e.what(), SvStl::SourceFileParams(StdMessageParams));
		return false;
	}

	catch (...)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, _T("Unknown error Handler"), SvStl::SourceFileParams(StdMessageParams));
		return false;
	}
	return true;
}

bool SVInspectionProcess::GoOffline()
{
	m_offlineRequest = true;
	//Wait a while to make sure that m_processActive and m_offlineRequest not set simultaneously
	::Sleep(10);
	///Wait a maximum of 10 seconds that inspection is done to avoid infinite loop
	constexpr int cMaxWaitLoop = 1000;
	for (int i = 0; i < cMaxWaitLoop; ++i)
	{
		if (false == m_processActive)
		{
			break;
		}
		
		::Sleep(10);
	}
	//This should usually be false if inspection process has terminated normally
	if (true == m_processActive)
	{
		m_processActive = false;
	}
	m_processThread.SetPriority(THREAD_PRIORITY_NORMAL);

	//save the last image to have it in the edit-mode
	if (0 < m_pCurrentToolset->getTriggerCount())
	{
		for (SvIe::SVCameraImageTemplate* pCameraImage : m_CameraImages)
		{
			if (nullptr != pCameraImage)
			{
				SvIe::SVVirtualCamera* pCamera = pCameraImage->GetCamera();
				auto pImageData = pCameraImage->getLastImage();
				if (nullptr != pCamera && nullptr != pImageData && !pImageData->empty())
				{
					pCamera->setTempImage(pImageData->GetBuffer());
				}
			}
		}
	}

	if (nullptr != m_pCurrentToolset)
	{
		m_pCurrentToolset->goingOffline();
	}

	return true;
}// end GoOffline

bool SVInspectionProcess::CanProcess(SVProductInfoStruct* pProduct, CantProcessEnum& rReason)
{
	bool bReady {true};
	rReason = CantProcessEnum::NoReason;
	if (nullptr == pProduct)
	{
		rReason = CantProcessEnum::MissingProduct;
		return false;
	}

	// See if we have discrete inputs.
	Log_Assert(m_InputObjects.size() == pProduct->m_triggerInfo.m_Inputs.size());
	for (size_t i = 0; bReady && i < m_InputObjects.size(); i++)
	{
		if (nullptr != m_InputObjects[i])
		{
			//Is input value valid
			bReady &= (VT_EMPTY != pProduct->m_triggerInfo.m_Inputs.at(i).vt);
		}
	}
	if (!bReady)
	{
		rReason = CantProcessEnum::MissingInput;
		return false;
	}
	// See if we have Camera images.
	SvIe::SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

	while (bReady && l_ImageIter != m_CameraImages.end())
	{
		SvIe::SVCameraImageTemplate* l_pImage = (*l_ImageIter);

		if (nullptr != l_pImage)
		{
			SvIe::SVVirtualCamera* pCamera = l_pImage->GetCamera();

			if (nullptr != pCamera)
			{
				auto l_Iter = pProduct->m_svCameraInfos.find(pCamera->getObjectId());

				if (l_Iter != pProduct->m_svCameraInfos.end())
				{
					bReady &= (nullptr != l_Iter->second.getImage());
				}
			}
		}

		++l_ImageIter;
	}
	if (!bReady)
	{
		rReason = CantProcessEnum::MissingImage;
		return false;
	}
	return bReady;
}

HRESULT SVInspectionProcess::StartProcess(SVProductInfoStruct* pProduct)
{
	HRESULT result {S_OK};

	// Make sure that the lists are the same size
	if (nullptr == pProduct || m_InputObjects.size() != pProduct->m_triggerInfo.m_Inputs.size() || m_processActive)
	{
		return E_FAIL;
	}

	for (const auto* pImage : m_CameraImages)
	{
		if (nullptr != pImage && nullptr != pImage->GetCamera())
		{
			const auto iter = pProduct->m_svCameraInfos.find(pImage->GetCamera()->getObjectId());
			if (pProduct->m_svCameraInfos.end() != iter)
			{
				if (nullptr == iter->second.getImage())
				{
					SvStl::MessageManager Msg(SvStl::MsgType::Log);
					Msg.setMessage(SVMSG_SVO_71_INVALID_SOURCE_IMAGE_INDEX, _T(""), SvStl::SourceFileParams(StdMessageParams));
					return SVMSG_SVO_71_INVALID_SOURCE_IMAGE_INDEX;
				}
			}
		}
	}

	auto ipIter = pProduct->m_svInspectionInfos.find(getObjectId());
	if (pProduct->m_svInspectionInfos.end() == ipIter)
	{
		return E_FAIL;
	}

	SVInspectionInfoStruct& rIPInfo = ipIter->second;

	if (rIPInfo.m_InProcess)
	{
		return E_FAIL;
	}

	rIPInfo.m_CanProcess = false;
	rIPInfo.m_InProcess = true;
	rIPInfo.m_HasBeenQueued = true;

	if (false == m_offlineRequest)
	{
		{
			std::lock_guard<std::mutex>  Autolock(m_inspectionMutex);
			m_product = pProduct->copyInspectionToNewProduct(getObjectId());
		}
		SVObjectManagerClass::Instance().IncrementInspectionIndicator();
		m_processActive = true;
		m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[InspectionFunction::Inspection]));
	}
	return result;
}

void SVInspectionProcess::RebuildInspectionInputList(const SVIOEntryHostStructPtrVector& rUsedInputs)
{
	auto listSize = static_cast<long>(rUsedInputs.size());

	std::vector<std::shared_ptr<SvOi::IValueObject>> previousInputObjects;
	previousInputObjects.resize(listSize);

	previousInputObjects.swap(m_InputObjects);

	for (long i = 0; i < listSize; ++i)
	{
		const auto& rEntry = rUsedInputs[i];
		auto* pInput = dynamic_cast<SVInputObject*> (SVObjectManagerClass::Instance().GetObject(rEntry->m_IOId));
		if (nullptr == rEntry || false == rEntry->m_Enabled || nullptr == pInput)
		{
			continue;
		}

		auto InputIter = std::find_if(previousInputObjects.begin(), previousInputObjects.end(), [&rEntry](const auto& pInput)->bool
		{
			SVObjectClass* pObject {dynamic_cast<SVObjectClass*> (pInput.get())};
			return nullptr != pObject && 0 == strcmp(rEntry->m_name.c_str(), pObject->GetName());
		});
		if (InputIter != previousInputObjects.end())
		{
			SVObjectClass* pInputObject {dynamic_cast<SVObjectClass*> ((*InputIter).get())};
			if (nullptr != pInputObject)
			{
				pInputObject->ResetObject();
			}
			if (nullptr != rEntry->getValueObject())
			{
				variant_t inputValue;
				pInput->Read(inputValue);
				rEntry->getValueObject()->setValue(inputValue);
			}
			m_InputObjects[i] = *InputIter;
			previousInputObjects.erase(InputIter);
		}
		else
		{
			SVObjectClass* pNewObject {nullptr};
			std::shared_ptr<SvOi::IValueObject> pInputValueObject;
			switch (rEntry->m_ObjectType)
			{
				case IO_DIGITAL_INPUT:
				{
					pInputValueObject = std::make_shared<SvVol::SVBoolValueObjectClass>();
					pInputValueObject->setResetOptions(false, SvOi::SVResetItemNone);
					pNewObject = dynamic_cast<SVObjectClass*> (pInputValueObject.get());
					pNewObject->SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
					break;
				}
				case IO_REMOTE_INPUT:
				{
					pInputValueObject = std::make_shared<SvVol::SVVariantValueObjectClass>();
					pInputValueObject->setResetOptions(false, SvOi::SVResetItemNone);
					variant_t inputValue;
					inputValue.ChangeType(VT_R8);
					pInputValueObject->setDefaultValue(inputValue);
					inputValue.Clear();
					pInput->Read(inputValue);
					pInputValueObject->setValue(inputValue);
					pNewObject = dynamic_cast<SVObjectClass*> (pInputValueObject.get());
					pNewObject->SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
					break;
				}
				default:
					break;
			}

			if (nullptr != pNewObject)
			{
				pNewObject->SetName(pInput->GetName());
				auto findIter = m_ioObjectMap.find(pInput->GetName());
				if (m_ioObjectMap.end() != findIter && SvDef::InvalidObjectId != findIter->second && pNewObject->getObjectId() != findIter->second)
				{
					SVObjectManagerClass::Instance().ChangeUniqueObjectID(pNewObject, findIter->second);
				}
				pNewObject->SetObjectOwner(this);
				pNewObject->ResetObject();
				CreateChildObject(pNewObject);
				pNewObject->SetObjectAttributesAllowed(SvDef::viewableAndUseable, SvOi::SetAttributeType::AddAttribute);
			}

			if (nullptr != pInputValueObject)
			{
				m_InputObjects[i] = pInputValueObject;
			}
		}
	}// end for
	for (auto& pInputValueObj : previousInputObjects)
	{
		if (nullptr != pInputValueObj)
		{
			SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (pInputValueObj.get());
			if (nullptr != pObject)
			{
				pObject->CloseObject();
			}
		}
	}

	SVResultList* pResultlist = GetResultList();
	if (pResultlist)
	{
		pResultlist->RebuildReferenceVector(this);
	}
}

bool SVInspectionProcess::AddInputRequest(const SVObjectReference& rObjectRef, const _variant_t& rValue)
{

	//setting Reset Counters to true reset the counter
	if (rObjectRef.getObject() == &(GetToolSet()->m_ResetCountsObject))
	{
		std::string Value = GetStringFromInputRequest(rValue);
		BOOL bValue(FALSE);
		// Convert to the appropriate type of value
		if (0 == SvUl::CompareNoCase(Value, _T("TRUE")))
		{
			bValue = TRUE;
		}
		else
		{
			bValue = static_cast<BOOL> (atof(Value.c_str()));
		}

		if (TRUE == bValue)
		{
			SvCmd::ResetCountsSynchronous(m_pCurrentToolset->GetInspection()->getObjectId(), false, false);
		}
		return true;
	}

	bool Result(false);
	try
	{

		SVInputRequestInfoStructPtr pInRequest {new SVInputRequestInfoStruct(rObjectRef, rValue)};

		//add request to inspection process
		Result = AddInputRequest(pInRequest);
	}
	catch (...) // @WARNING:  bad practice catching ...
	{
	}

	return Result;
}

bool SVInspectionProcess::AddInputRequest(SVInputRequestInfoStructPtr pInRequest)
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING | SV_STATE_CLOSING;

	if (SvimState::CheckState(notAllowedStates))
	{
		return false;
	}

	if (!m_InputRequests.AddTail(pInRequest))
	{
		return false;
	}


	std::string l_StringValue;

	if (VT_BSTR == pInRequest->m_Value.vt)
	{
		l_StringValue = SvUl::createStdString(pInRequest->m_Value);
	}

	if (l_StringValue == SvO::SVTOOLPARAMETERLIST_MARKER)
	{
		m_lInputRequestMarkerCount++;

	}// end if

	return true;
}

bool SVInspectionProcess::AddInputRequestMarker()
{
	return AddInputRequest(SVObjectReference {nullptr}, SvO::SVTOOLPARAMETERLIST_MARKER);
}

bool SVInspectionProcess::AddInputRequestResetAllObject()
{
	return AddInputRequest(SVObjectReference {nullptr}, SvO::RESETALLOBJECTS_MARKER);
}

HRESULT SVInspectionProcess::AddInputImageRequest(SvIe::SVImageClass* p_psvImage, BSTR& p_rbstrValue)
{
	HRESULT l_Status = S_OK;

	if (nullptr != p_psvImage)
	{
		SVImageInfoClass l_ImageInfo = p_psvImage->GetImageInfo();

		try
		{
			SVInputImageRequestInfoStructPtr l_pInRequest {new SVInputImageRequestInfoStruct};

			try
			{
				auto* l_psvMainImage = dynamic_cast<SvIe::SVCameraImageTemplate*>(p_psvImage);

				l_Status = SvIe::SVImageProcessingClass::LoadImageBuffer((void*)p_rbstrValue,
					l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr, l_ImageInfo);

				l_pInRequest->m_bUsingCameraName = nullptr != l_psvMainImage;

				if (l_pInRequest->m_bUsingCameraName)
				{
					l_pInRequest->m_ObjectName = l_psvMainImage->GetCameraName();
				}
				else
				{
					l_pInRequest->m_ObjectName = p_psvImage->GetCompleteName();
				}

				//add request to inspection process
				HRESULT l_Temp = AddInputImageRequest(l_pInRequest);

				if (S_OK == l_Status)
				{
					l_Status = l_Temp;
				}
			}
			catch (...)
			{
				l_Status = E_FAIL;
			}
		}
		catch (...)
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

HRESULT SVInspectionProcess::AddInputImageFileNameRequest(SvIe::SVImageClass* pImage, const std::string& rImageFileName)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pImage)
	{
		SVImageInfoClass l_ImageInfo = pImage->GetImageInfo();

		try
		{
			SVInputImageRequestInfoStructPtr l_pInRequest {new SVInputImageRequestInfoStruct};

			auto* pMainImage = dynamic_cast<SvIe::SVCameraImageTemplate*> (pImage);

			l_Status = SvIe::SVImageProcessingClass::LoadImageBuffer(rImageFileName.c_str(), l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr);
			l_pInRequest->m_bUsingCameraName = nullptr != pMainImage;

			if (l_pInRequest->m_bUsingCameraName)
			{
				l_pInRequest->m_ObjectName = pMainImage->GetCameraName();
			}
			else
			{
				l_pInRequest->m_ObjectName = pImage->GetCompleteName();
			}

			//add request to inspection process
			HRESULT l_Temp = AddInputImageRequest(l_pInRequest);

			if (S_OK == l_Status)
			{
				l_Status = l_Temp;
			}
		}
		catch (...)
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

HRESULT SVInspectionProcess::AddInputImageRequest(SVInputImageRequestInfoStructPtr p_pInRequest)
{
	DWORD notAllowedStates = SV_STATE_START_PENDING | SV_STATE_STARTING | SV_STATE_STOP_PENDING | SV_STATE_STOPING | SV_STATE_CLOSING;

	if (SvimState::CheckState(notAllowedStates))
	{
		return E_FAIL;
	}

	if (!m_InputImageRequests.AddTail(p_pInRequest))
	{
		return E_FAIL;
	}// end if

	return S_OK;
}// end AddInputImageRequest

//******************************************************************************
// Message Operation(s):
//******************************************************************************

HRESULT SVInspectionProcess::RebuildInspection(bool shouldCreateAllObject)
{
	HRESULT l_Status = S_OK;

	SetDefaultInputs();

	SVProductInfoStruct l_Product = LastProductGet();
	if (l_Product.empty())
	{
		l_Product.setInspectionTriggerRecordComplete(getObjectId());
		LastProductUpdate(l_Product);
	}

	////////////////////////
	// Color SVIM
	if (IsColorCamera())
	{
		SvIe::SVVirtualCameraPtrVector cameraVector;
		SVPPQObject* pPPQ = GetPPQ();
		if (nullptr != pPPQ)
		{
			cameraVector = pPPQ->GetVirtualCameras();
		}

		bool bColorSourceImage = false;
		bool bCameraSupportsColor = false;
		for (const auto* pCamera : cameraVector)
		{
			SvIe::SVAcquisitionClassPtr pCamDevice = nullptr;

			if (nullptr != pCamera)
			{
				pCamDevice = pCamera->GetAcquisitionDevice();
			}

			if (pCamDevice && 3L == pCamDevice->BandSize())
			{
				bColorSourceImage = true;
				break;
			}

			SVDeviceParamCollection params;
			SVDeviceParamCollection CFParams;
			pCamDevice->GetDeviceParameters(params);
			pCamDevice->GetCameraFileParameters(CFParams);
			const SVCameraFormatsDeviceParam* pParam = params.Parameter(DeviceParamCameraFormats).DerivedValue(pParam);
			const SVCameraFormatsDeviceParam* pCFParam = CFParams.Parameter(DeviceParamCameraFormats).DerivedValue(pCFParam);

			if (pParam && pCFParam)
			{
				SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
				for (iter = pCFParam->options.begin(); iter != pCFParam->options.end(); ++iter)
				{
					if (iter->second.m_bColor)
					{
						bCameraSupportsColor = true;
					}
				}

				if (pParam->options.find(pParam->strValue)->second.m_bColor)
				{
					bColorSourceImage = true;
					break;
				}
			}
		}

		if (!bColorSourceImage)
		{
			if (!bCameraSupportsColor)
			{
				l_Status = E_FAIL;

				SvStl::MessageManager Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_IPDoc_NoCameraColorAttached, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	////////////////////////
	if (shouldCreateAllObject || !IsCreated())
	{
		if (createAllObjects())
		{
			l_Status = E_FAIL;
		}
	}

	SetDefaultInputs();

	return l_Status;
}

void SVInspectionProcess::resetCounterSynchronous()
{
	if (nullptr != m_pCurrentToolset)
	{
		SvCmd::ResetCountsSynchronous(m_pCurrentToolset->GetInspection()->getObjectId(), false, false);
	}
}

void SVInspectionProcess::resetCounterDirectly()
{
	m_pCurrentToolset->resetCounterDirectly();
}

void SVInspectionProcess::requestResetAllObjects()
{
	AddInputRequestResetAllObject();
	AddInputRequestMarker();
}




SvOi::IObjectClass* SVInspectionProcess::GetIObjectClassPtr()
{
	return this;
}

void SVInspectionProcess::ValidateAndInitialize(bool p_Validate)
{
	// Call ToolSet Validate 
	// so the SVEquation can rebuild its symbol table
	if (p_Validate)
	{
		resetCounterSynchronous();
		requestResetAllObjects();

		m_svReset.AddState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

		SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemIP;

		ProcessInputRequests(eResetItem);

		m_svReset.RemoveState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);
	}
}

void SVInspectionProcess::SingleRunModeLoop(bool p_Refresh)
{
	SVProductInfoStruct l_svProduct;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		l_svProduct = pPPQ->getProductReadyForRunOnce(getObjectId());
	}

	LastProductCopySourceImagesTo(&l_svProduct);

	RunInspection(l_svProduct.m_svInspectionInfos[getObjectId()], l_svProduct.m_svCameraInfos, l_svProduct.m_triggerInfo.lTriggerCount);
	l_svProduct.setInspectionTriggerRecordComplete(getObjectId());

	LastProductUpdate(l_svProduct);

	if (p_Refresh)
	{
		// Result View, Display Image and Overlay Update
		if (!SvimState::CheckState(SV_STATE_RUNNING))
		{
			std::pair<long, SVInspectionInfoStruct> data {l_svProduct.triggerCount(), l_svProduct.m_svInspectionInfos[getObjectId()]};

			updateObserver(data);
		}
	}
}

bool SVInspectionProcess::resetAllObjects(SvStl::MessageContainerVector* pErrorMessages/*=nullptr */, int /*nResetDepth= 0*/)
{
#ifdef TRACE_RESETALL	
	ResetImageIds.clear();
	ResetIds.clear();
#endif 
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr == pTrcRW)
	{
		return false;
	}

	SvStl::MessageContainerVector ErrorMessages;
	bool Result = true;
	try
	{
		SvOi::TRC_RAIIPtr pResetRaii;
		if (false == pTrcRW->isResetLocked())
		{
			pResetRaii = pTrcRW->startResetTriggerRecordStructure(m_trcPos);
		}

		if (nullptr != m_pCurrentToolset)
		{
			Result = m_pCurrentToolset->resetAllObjects(&ErrorMessages) && Result;
		}
		else
		{
			Result = false;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ToolsetNotCreated, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			ErrorMessages.push_back(Msg);
		}

		Result = __super::resetAllObjects(&ErrorMessages) && Result;

		if (!SvimState::CheckState(SV_STATE_RUNNING | SV_STATE_TEST | SV_STATE_REGRESSION))
		{
			buildValueObjectData();
		}

		if (nullptr != pResetRaii)
		{
			pResetRaii->free();
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		if (SVMSG_TRC_GENERAL_ERROR == rExp.getMessage().m_MessageCode && SvStl::Tid_TRC_Error_CreateBuffer == rExp.getMessage().m_AdditionalTextId)
		{
			SvStl::MessageManager oldException(SvStl::MsgType::Log);
			oldException.setMessage(rExp.getMessage());
			SvStl::MessageManager newException(SvStl::MsgType::Display);
			newException.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetInspectionFailed, SvStl::SourceFileParams(StdMessageParams));
			ErrorMessages.insert(ErrorMessages.begin(), newException.getMessageContainer());
		}
		else if (SVMSG_TRC_GENERAL_ERROR == rExp.getMessage().m_MessageCode && SvStl::Tid_TRC_Error_ResetBuffer_TooMany == rExp.getMessage().m_AdditionalTextId)
		{
			SvStl::MessageManager oldException(SvStl::MsgType::Display | SvStl::MsgType::Log);
			oldException.setMessage(rExp.getMessage());
			ErrorMessages.insert(ErrorMessages.begin(), oldException.getMessageContainer());
		}
		else
		{
			ErrorMessages.push_back(rExp);
		}
		Result = false;
	}

	//log all error messages to event log
	for (SvStl::MessageContainerVector::iterator iter = ErrorMessages.begin(); ErrorMessages.end() != iter; ++iter)
	{
		SvStl::MessageManager message(SvStl::MsgType::Log);
		message.setMessage(iter->getMessage());
	}
	if (nullptr != pErrorMessages)
	{
		*pErrorMessages = ErrorMessages;
	}
#ifdef TRACE_RESETALL	

	std::string msg = std::format("Total ResetedImage Ids: {}\n", ResetImageIds.size());
	OutputDebugString(msg.c_str());
	bool first {true};
	for (auto& it : ResetImageIds)
	{
		if (it.second > 1)
		{
			if (first)
			{
				OutputDebugString("RESET IMAGE IDS MORE THEN ONCE!!!!!!!!!!:\n");
				first = false;
			}
			msg = std::format("{} : ({}): {} \n", IdsName[it.first], it.first, it.second);
			OutputDebugString(msg.c_str());
		}

	}
	first = true;
	msg = std::format("Total ResetedIds: {}\n", ResetIds.size());
	OutputDebugString(msg.c_str());
	for (auto& it : ResetIds)
	{
		if (it.second > 1)
		{
			if (first)
			{
				OutputDebugString("RESET IDS MORE THEN ONCE!!!!!!!!!!:\n");
				first = false;
			}

			msg = std::format("{} : ({}): {} \n", IdsName[it.first], it.first, it.second);
			OutputDebugString(msg.c_str());
		}
	}

#endif 
	return Result;
}

HRESULT SVInspectionProcess::FindPPQInputObjectByName(SVObjectClass*& rpObject, LPCTSTR p_FullName) const
{
	HRESULT l_Status = S_OK;

	for (size_t l = 0; nullptr == rpObject && l < m_InputObjects.size(); ++l)
	{
		if (nullptr != m_InputObjects[l])
		{
			SVObjectClass* pInputObject = dynamic_cast<SVObjectClass*> (m_InputObjects[l].get());
			if (nullptr != pInputObject)
			{
				std::string l_LocalName = GetName();
				l_LocalName += _T(".");
				l_LocalName += pInputObject->GetName();

				if (l_LocalName == p_FullName)
				{
					rpObject = pInputObject;
				}
			}
		}
	}

	if (nullptr == rpObject)
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetChildObject(SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index) const
{
	HRESULT l_Status = SVObjectClass::GetChildObject(rpObject, rNameInfo, Index);

	if (l_Status != S_OK)
	{
		if (static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName())
		{
			std::string l_ObjectName = rNameInfo.GetObjectName(Index);

			l_Status = FindPPQInputObjectByName(rpObject, l_ObjectName.c_str());

			if (l_Status != S_OK)
			{
				if (GetToolSet())
				{
					l_Status = GetToolSet()->GetChildObject(rpObject, rNameInfo, Index + 1);
				}
			}
		}
	}

	return l_Status;
}

void SVInspectionProcess::SetPPQIdentifier(uint32_t PPQId)
{
	m_PPQId = PPQId;
}

uint32_t SVInspectionProcess::GetPPQIdentifier() const
{
	return m_PPQId;
}

SVPPQObject* SVInspectionProcess::GetPPQ() const
{
	SVPPQObject* pPPQ(nullptr);

	if (SvDef::InvalidObjectId != m_PPQId)
	{
		pPPQ = dynamic_cast<SVPPQObject*>(SVObjectManagerClass::Instance().GetObject(m_PPQId));
	}

	return pPPQ;
}

SvOi::IObjectClass* SVInspectionProcess::GetPPQInterface() const
{
	return GetPPQ();
}

HRESULT SVInspectionProcess::InitializeRunOnce()
{
	HRESULT l_Status = S_OK;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		SVProductInfoStruct product = pPPQ->getProductReadyForRunOnce(getObjectId());

		if (0 >= product.m_svInspectionInfos.size())
		{
			l_Status = E_FAIL;
		}

		if (!(RunInspection(product.m_svInspectionInfos[getObjectId()], product.m_svCameraInfos, product.m_triggerInfo.lTriggerCount, false)) && S_OK == l_Status)
		{
			l_Status = E_FAIL;
		}
		//SvDef::InvalidObjectId reset all inspection-TR and do not mark the TR as new TR. 
		//And this is important, because else an event is sent and leads to wrong image updates in webApp.
		product.setInspectionTriggerRecordComplete(SvDef::InvalidObjectId);

		LastProductUpdate(product);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVInspectionProcess::ProcessInputRequests(bool& rForceOffsetUpdate)
{
	SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemNone;

	bool l_bOk = ProcessInputRequests(eResetItem);

	rForceOffsetUpdate |= (eResetItem < SvOi::SVResetItemNone);

	return l_bOk;
}

bool SVInspectionProcess::ProcessInputRequests(SvOi::SVResetItemEnum& rResetItem)
{
	bool bRet = true;

	///If doing reset do not process input requests
	if (m_resetting)
	{
		return false;
	}

	SVInputRequestInfoStructPtr pInputRequest;
	std::vector<uint32_t> IdsToReset;


	while (m_lInputRequestMarkerCount > 0L)
	{
		long l_lSize = 0;

		// Only loop through the InputRequests that are here now
		// Preprocessing phase
		m_InputRequests.GetSize(l_lSize);

		for (long l = 0; l < l_lSize; l++)
		{
			if (!m_InputRequests.RemoveHead(&pInputRequest))
			{
				bRet = false;

				break;
			}// end if

			std::string Value = GetStringFromInputRequest(pInputRequest->m_Value);


			// New delimiter added after each SVSetToolParameterList call
			// This breaks the list into pieces and we are only processing
			// 1 piece of the list per inspection iteration
			if (Value == SvO::SVTOOLPARAMETERLIST_MARKER)
			{
				m_lInputRequestMarkerCount--;
				break;
			}

			if (Value == SvO::RESETALLOBJECTS_MARKER)
			{
				rResetItem = SvOi::SVResetItemIP;
			}


			// Get the ValueObject that they are trying to set
			SVObjectReference ObjectRef = pInputRequest->m_ValueObjectRef;
			HRESULT hrSet = S_OK;
			if (nullptr != ObjectRef.getValueObject())
			{
				bool bResetObject = ObjectRef.getValueObject()->ResetAlways();

				// Value objects don't accept each different type for sets
				// so convert to the appropriate type
				if (auto* pFileNameObj = dynamic_cast<SvVol::SVFileNameValueObjectClass*> (ObjectRef.getObject()))
				{
					if (ObjectRef.isArray() && ObjectRef.isEntireArray())
					{
						hrSet = SetObjectArrayValues<CFile>(ObjectRef, Value, bResetObject);
					}
					else
					{
						CFileStatus rStatus;

						bool bOk = (TRUE == CFile::GetStatus(Value.c_str(), rStatus));
						if (bOk)
						{
							bOk = 0L <= rStatus.m_size;
						}// end if

						if (bOk)
						{
							if (!bResetObject)
							{
								std::string PrevValue;

								/*hrSet =*/ pFileNameObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

								bResetObject = (PrevValue != Value);
							}

							hrSet = pFileNameObj->SetValue(Value, ObjectRef.ArrayIndex());
						}// end if
					}
				}
				else if (auto* pStringValueObj = dynamic_cast<SvVol::SVStringValueObjectClass*> (ObjectRef.getObject()))
				{
					if (ObjectRef.isArray() && ObjectRef.isEntireArray())
					{
						hrSet = SetObjectArrayValues<std::string>(ObjectRef, Value, bResetObject);
					}
					else
					{
						if (!bResetObject)
						{
							std::string PrevValue;

							/*hrSet =*/ pStringValueObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

							bResetObject = (PrevValue != Value);
						}

						hrSet = pStringValueObj->SetValue(Value, ObjectRef.ArrayIndex());
					}
				}
				else if (auto* pBoolValueObj = dynamic_cast<SvVol::SVBoolValueObjectClass*> (ObjectRef.getObject()))
				{
					BOOL NewValue(false);

					// Convert to the appropriate type of value
					if (0 == SvUl::CompareNoCase(Value, _T("TRUE")))
					{
						NewValue = true;
					}
					else
					{
						NewValue = static_cast<BOOL> (atof(Value.c_str()));
					}

					if (!bResetObject)
					{
						BOOL PrevValue(false);

						/*hrSet =*/ pBoolValueObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

						// eventually use CompareWithCurrentValueImpl
						bResetObject = NewValue != PrevValue;
					}

					hrSet = pBoolValueObj->SetValue(NewValue, ObjectRef.ArrayIndex());
				}// end else if SVBoolValueObjectClass
				else if (auto* pvValueObject = dynamic_cast <SvVol::SVVariantValueObjectClass*> (ObjectRef.getObject()))
				{
					// Convert to the appropriate type of value

					// eventually use CompareWithCurrentValueImpl
					bResetObject = true;

					if (ObjectRef.isArray())
					{
						hrSet = pvValueObject->SetValueKeepType(Value.c_str(), ObjectRef.ArrayIndex());
					}
					else
					{
						hrSet = pvValueObject->SetValueKeepType(Value.c_str());
					}
				}// end else if SVVariantValueObjectClass
				else if (auto* pEnumerateValueObj = dynamic_cast <SvVol::SVEnumerateValueObjectClass*> (ObjectRef.getObject()))
				{
					// Convert to the appropriate type of value
					bool l_bTempReset = false;

					if (!bResetObject)
					{
						//Reset if values not the same
						l_bTempReset = !pEnumerateValueObj->CompareWithCurrentValue(Value);
					}

					if (S_OK != (hrSet = pEnumerateValueObj->setValue(Value, ObjectRef.ArrayIndex())))
					{
						if (false == Value.empty())
						{
							if (_ismbcdigit(Value.c_str()[0]))
							{
								long EnumValue = atol(Value.c_str());

								if (!bResetObject)
								{
									l_bTempReset = !pEnumerateValueObj->CompareWithCurrentValue(Value);
								}

								hrSet = pEnumerateValueObj->SetValue(EnumValue, ObjectRef.ArrayIndex());
							}
						}
					}

					bResetObject = l_bTempReset;
				}// end else if SVEnumerateValueObjectClass
				else if (auto* pDoubleValueObj = dynamic_cast <SvVol::SVDoubleValueObjectClass*> (ObjectRef.getObject()))
				{
					if (ObjectRef.isArray() && ObjectRef.isEntireArray())
					{
						hrSet = SetObjectArrayValues<double>(ObjectRef, Value, bResetObject);
					}
					else
					{
						double NewValue = atof(Value.c_str());
						if (!bResetObject)
						{
							double PrevValue;

							/*hrSet = */pDoubleValueObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

							// eventually use CompareWithCurrentValueImpl
							bResetObject = PrevValue != NewValue;
						}

						hrSet = pDoubleValueObj->SetValue(NewValue, ObjectRef.ArrayIndex());
					}
				}// end else if
				else // Long, DWord, Byte
				{
					if (!bResetObject)
					{
						_variant_t PrevValue;

						/*hrSet = */ObjectRef.getValueObject()->getValue(PrevValue, ObjectRef.ArrayIndex());

						std::string strNewValue = SvUl::MakeLower(Value.c_str());
						TCHAR* p = nullptr;
						long lValue = 0;
						if (std::string::npos != strNewValue.find(_T('x')))
						{
							lValue = _tcstol(strNewValue.c_str(), &p, 16);
						}
						else
						{
							lValue = _tcstol(strNewValue.c_str(), &p, 10);
						}

						// eventually use CompareWithCurrentValueImpl
						bResetObject = static_cast<long> (PrevValue) != lValue;
					}

					hrSet = ObjectRef.getValueObject()->setValue(Value, ObjectRef.ArrayIndex());
				}// end else if

				if (SvOi::SVResetItemIP != rResetItem && bResetObject && SvOi::SVResetItemNone > ObjectRef.getValueObject()->getResetItem())
				{
					auto* pTool = dynamic_cast<SvTo::SVToolClass*> (ObjectRef.getObject()->GetAncestor(SvPb::SVToolObjectType));

					if (nullptr != pTool)
					{
						std::back_insert_iterator<std::vector<uint32_t>> Inserter(IdsToReset);
						SvOi::SVResetItemEnum eToolResetItem = AddToResetIds(ObjectRef.getValueObject()->getResetItem(), ObjectRef.getObject(), Inserter);

						if (eToolResetItem < rResetItem)
						{
							rResetItem = eToolResetItem;
						}
					}
					else
					{
						rResetItem = SvOi::SVResetItemIP;
					}

					if (SvOi::SVResetItemIP == rResetItem)
					{

						IdsToReset.clear();

					}
				}
			}// end if object exists

		}// end for all input requests

		// Processing phase
		if (nullptr != m_pCurrentToolset)
		{
			if (SvOi::SVResetItemIP == rResetItem)
			{
				bRet &= resetAllObjects();
			}
			else
			{
				SvUl::RemoveDuplicates(IdsToReset);
				for (const auto id : IdsToReset)
				{
					SVObjectClass* pObject = nullptr;
					SVObjectManagerClass::Instance().GetObjectByIdentifier(id, pObject);
					if (pObject)
					{
						bRet = pObject->resetAllObjects(nullptr, SvTo::cResetDepth) && bRet;
					}

				}
				IdsToReset.clear();


			}// end if( l_bResetIPDoc ) else

			if (rResetItem != SvOi::SVResetItemNone)
			{
				updateObserver(SVRemoveValues());
			}
		}// end if ( nullptr != m_pCurrentToolset )
	}// end while( m_lInputRequestMarkerCount > 0L )

	return bRet;
}

bool SVInspectionProcess::ProcessInputImageRequests(SVInspectionInfoStruct& rIpInfoStruct, const SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos)
{
	bool bRet = true;

	SVInputImageRequestInfoStructPtr l_pInRequest;
	while (m_InputImageRequests.RemoveHead(&l_pInRequest))
	{
		SvIe::SVImageClass* pImage = nullptr;

		if (l_pInRequest->m_bUsingCameraName)
		{
			SvIe::SVCameraImagePtrSet l_MainImages;

			GetMainImages(l_pInRequest->m_ObjectName, l_MainImages);

			SvIe::SVCameraImagePtrSet::iterator l_Iter = l_MainImages.begin();

			while (nullptr == pImage && l_Iter != l_MainImages.end())
			{
				SvIe::SVCameraImageTemplate* pCameraImage = (*l_Iter);

				if (nullptr != pCameraImage)
				{
					pImage = pCameraImage;
				}
				else
				{
					++l_Iter;
				}
			}
		}
		else if (l_pInRequest->m_bChangingToolInput)
		{
			auto* pTool = dynamic_cast<SvTo::SVToolClass*>(SVObjectManagerClass::Instance().GetObject(l_pInRequest->m_toolId));
			if (nullptr != pTool)
			{
				pTool->overwriteInputSource(l_pInRequest->m_ImageHandlePtr);
				l_pInRequest->m_ImageHandlePtr.reset();
				continue;
			}
		}
		else
		{
			SVObjectClass* pObject = nullptr;
			if (S_OK == SVObjectManagerClass::Instance().GetObjectByDottedName(l_pInRequest->m_ObjectName.c_str(), pObject))
			{
				pImage = dynamic_cast<SvIe::SVImageClass*> (pObject);
			}
		}

		if (nullptr != pImage)
		{
			SvOi::ITRCImagePtr pImageBuffer;
			SvIe::SVVirtualCamera* pVirtualCamera = nullptr;
			if (auto* l_psvMainImage = dynamic_cast<SvIe::SVMainImageClass*> (pImage))
			{
				const auto l_svIter = rCameraInfos.find(l_psvMainImage->GetCamera()->getObjectId());
				if (l_svIter != rCameraInfos.end())
				{
					pImageBuffer = l_psvMainImage->getTempImageBuffer();
					///Here a const_cast is made to set the temporary image which uses a temporary product not from PPQ
					///This is not called during normal run mode!
					auto& rCameraInfo = const_cast<SvIe::SVCameraInfoStruct&> (l_svIter->second);
					rCameraInfo.setImage(pImageBuffer);
				}
				pVirtualCamera = l_psvMainImage->GetCamera();
			}
			else
			{
				pImageBuffer = pImage->getImageToWrite(rIpInfoStruct.m_triggerRecordWrite);
			}

			if (nullptr != rIpInfoStruct.m_triggerRecordWrite && nullptr != pImageBuffer && !pImageBuffer->isEmpty() && nullptr != l_pInRequest->m_ImageHandlePtr)
			{
				/*HRESULT l_Code = */SVMatroxBufferInterface::CopyBuffer(pImageBuffer->getHandle()->GetBuffer(), l_pInRequest->m_ImageHandlePtr->GetBuffer());
				if (!SvimState::CheckState(SV_STATE_RUNNING) && nullptr != pVirtualCamera)
				{
					pVirtualCamera->setTempImage(l_pInRequest->m_ImageHandlePtr->GetBuffer());
				}
			}
			else
			{
				bRet = false;
			}
		}
		else
		{
			bRet = false;
		}

		l_pInRequest->m_ImageHandlePtr.reset();
	}

	return bRet;
}

void SVInspectionProcess::DisconnectToolSetMainImage()
{
	SvIe::SVCameraImagePtrSet::iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		SvIe::SVCameraImageTemplate* l_pCameraImage = (*l_Iter);

		if (nullptr != l_pCameraImage)
		{
			l_pCameraImage->DisconnectBuffers();

			++l_Iter;
		}
		else
		{
			l_Iter = m_CameraImages.erase(l_Iter);
		}
	}
}

void SVInspectionProcess::ConnectToolSetMainImage()
{
	m_svReset.AddState(SvDef::SVResetAutoMoveAndResize);

	SvIe::SVCameraImagePtrSet::iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		SvIe::SVCameraImageTemplate* l_pCameraImage = (*l_Iter);

		if (nullptr != l_pCameraImage)
		{
			l_pCameraImage->ReconnectBuffers();

			++l_Iter;
		}
		else
		{
			l_Iter = m_CameraImages.erase(l_Iter);
		}
	}

	m_svReset.RemoveState(SvDef::SVResetAutoMoveAndResize);
}

void SVInspectionProcess::RemoveCamera(const std::string& rCameraName)
{
	if (rCameraName == m_ToolSetCameraName)
	{
		SvOi::IObjectClass* pCamera = getFirstPPQCamera();

		if (nullptr != pCamera)
		{
			m_ToolSetCameraName = pCamera->GetName();
		}
		else
		{
			m_ToolSetCameraName.clear();
		}
	}

	SvIe::SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		if (nullptr != *l_Iter)
		{
			SvIe::SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if ((nullptr != pCamera) && (rCameraName == pCamera->GetName()))
			{
				(*l_Iter)->UpdateCameraImage(m_ToolSetCameraName.c_str());
			}
		}

		++l_Iter;
	}
}

bool SVInspectionProcess::IsNewDisableMethodSet() const
{
	return m_bNewDisableMethod;
}

void SVInspectionProcess::SetNewDisableMethod(bool bNewDisableMethod)
{
	m_bNewDisableMethod = bNewDisableMethod;
}

bool SVInspectionProcess::getEnableAuxiliaryExtent() const
{
	if (nullptr != m_pCurrentToolset)
	{
		return m_pCurrentToolset->getEnableAuxiliaryExtents();
	}

	return false;
}

void SVInspectionProcess::setEnableAuxiliaryExtent(bool Enabled)
{
	//This variable is only required to be able to read old configurations with auxiliary extents set in the old format
	//Due to the tool set being created new at a later stage this is overwritten so we need to store it
	m_initialAuxiliaryExtents = Enabled;
	if (nullptr != m_pCurrentToolset)
	{
		return m_pCurrentToolset->setEnableAuxiliaryExtents(Enabled);
	}
}

DWORD SVInspectionProcess::GetObjectIdIndex() const
{
	if (nullptr != m_pCurrentToolset)
	{
		return m_pCurrentToolset->GetObjectIdIndex();
	}
	//Default is object id index 0
	return 0;
}

void SVInspectionProcess::SetObjectIdIndex(DWORD objectIdIndex)
{
	if (nullptr != m_pCurrentToolset)
	{
		return m_pCurrentToolset->SetObjectIdIndex(objectIdIndex);
	}
}

HRESULT SVInspectionProcess::getOverlays(SvIe::SVImageClass& rImage, SVExtentMultiLineStructVector& rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	rMultiLineArray.clear();

	SVToolSet* pToolSet = GetToolSet();

	if (nullptr != pToolSet)
	{
		pToolSet->getOverlays(rImage, rMultiLineArray);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::AddInputImageRequestByCameraName(const std::string& rCameraName, const std::string& rFileName)
{
	HRESULT hrOk = S_OK;

	SVImageInfoClass svInfo;
	SvOi::SVImageBufferHandlePtr svHandleStruct;
	SVInputImageRequestInfoStructPtr l_pInRequest;
	try
	{
		l_pInRequest = SVInputImageRequestInfoStructPtr {new SVInputImageRequestInfoStruct};
	}
	catch (...)
	{
		//do something here...
		l_pInRequest = nullptr;
	}

	if (l_pInRequest)
	{
		l_pInRequest->m_bUsingCameraName = true;
		l_pInRequest->m_ObjectName = rCameraName;
		hrOk = SvIe::SVImageProcessingClass::LoadImageBuffer(rFileName.c_str(), l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr);

		if (hrOk == S_OK)
		{
			AddInputImageRequest(l_pInRequest);
		}
	}

	return hrOk;
}

void SVInspectionProcess::AddInputImageRequestToTool(const std::string& rName, uint32_t toolId, const std::string& rFileName)
{
	SVImageInfoClass svInfo;
	SvOi::SVImageBufferHandlePtr svHandleStruct;
	SVInputImageRequestInfoStructPtr pInRequest = std::make_shared<SVInputImageRequestInfoStruct>();

	if (pInRequest)
	{
		pInRequest->m_bUsingCameraName = false;
		pInRequest->m_bChangingToolInput = true;
		pInRequest->m_toolId = toolId;
		pInRequest->m_ObjectName = rName;
		SvIe::SVImageProcessingClass::LoadImageBuffer(rFileName.c_str(), pInRequest->m_ImageInfo, pInRequest->m_ImageHandlePtr);
		AddInputImageRequest(pInRequest);
	}
}

std::pair<SvTrig::SVTriggerInfoStruct, SVInspectionInfoStruct> SVInspectionProcess::getLastProductData() const
{
	if (m_lastRunProductValid)
	{
		std::lock_guard<std::mutex>  Autolock(m_inspectionMutex);
		auto ipInfoIter = m_lastRunProduct.m_svInspectionInfos.find(getObjectId());
		if (m_lastRunProduct.m_svInspectionInfos.end() != ipInfoIter)
		{
			return {m_lastRunProduct.m_triggerInfo, ipInfoIter->second};
		}
	}
	return {};
}

void SVInspectionProcess::resetLastProduct()
{
	std::lock_guard<std::mutex>  Autolock(m_inspectionMutex);
	m_lastRunProductValid = false;
	m_lastRunProduct.Reset();
}

HRESULT SVInspectionProcess::LastProductCopySourceImagesTo(SVProductInfoStruct* p_psvProduct)
{
	HRESULT Result = S_OK;

	if (nullptr != p_psvProduct)
	{
		SVProductInfoStruct l_Product = LastProductGet();
		for (auto& rCameraInfoPair : p_psvProduct->m_svCameraInfos)
		{
			SvIe::SVVirtualCamera* pCamera(dynamic_cast<SvIe::SVVirtualCamera*> (SvOi::getObject(rCameraInfoPair.first)));
			if (nullptr != pCamera)
			{
				bool Copied(false);
				const SvIe::SVObjectIdSVCameraInfoStructMap::iterator LastIter(l_Product.m_svCameraInfos.find(rCameraInfoPair.first));

				if (LastIter != l_Product.m_svCameraInfos.end() && nullptr != LastIter->second.getImage() && !LastIter->second.getImage()->isEmpty())
				{
					Copied = rCameraInfoPair.second.setImage(LastIter->second.getImage());
				}
				else
				{
					auto pImage = rCameraInfoPair.second.getImage();
					if (nullptr == pImage || pImage->isEmpty())
					{
						Log_Assert(false);
						pImage = rCameraInfoPair.second.GetNextImage();
					}
					if (nullptr != pImage && pImage->isValid())
					{
						auto tmpImage = pCamera->getTempImage();
						if (!tmpImage.empty())
						{
							Copied = (S_OK == SVMatroxBufferInterface::CopyBuffer(pImage->getHandle()->GetBuffer(), tmpImage));
						}
						else
						{
							SVMatroxBufferInterface::ClearBuffer(pImage->getHandle()->GetBuffer(), 0);
						}
					}
					else
					{
						Log_Assert(false);
					}
				}
				if (!Copied)
				{
					Result = E_FAIL;
				}
			}
		}
	}

	return Result;
}

void SVInspectionProcess::LastProductNotify()
{
	m_NotifyWithLastInspected = true;

	m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[InspectionFunction::NotifyLastInspected]));
}

bool SVInspectionProcess::IsColorCamera() const
{
	auto* pCamera = dynamic_cast<SvIe::SVVirtualCamera*> (SvOi::getObject(getFirstCamera()));

	if (nullptr != pCamera)
	{
		return pCamera->IsColor();
	}

	return false;
}

SvOi::IObjectClass* SVInspectionProcess::getFirstPPQCamera() const
{
	SvOi::IObjectClass* pCamera(nullptr);

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		SvIe::SVVirtualCameraPtrVector CameraVector = pPPQ->GetVirtualCameras();

		SvIe::SVVirtualCameraPtrVector::iterator iter = CameraVector.begin();

		if (iter != CameraVector.end())
		{
			pCamera = dynamic_cast<SvOi::IObjectClass*> (*iter);
		}
	}

	return pCamera;
}

SvIe::SVVirtualCameraPtrVector SVInspectionProcess::GetCameras() const
{
	SvIe::SVVirtualCameraPtrVector cameraVector;

	for (const auto* const pCameraImage : m_CameraImages)
	{
		if (nullptr != pCameraImage)
		{
			SvIe::SVVirtualCamera* pCamera = pCameraImage->GetCamera();

			if (nullptr != pCamera)
			{
				cameraVector.emplace_back(pCamera);
			}
		}
	}
	std::sort(cameraVector.begin(), cameraVector.end(), SvIe::isLessByName);
	//Remove duplicates
	cameraVector.erase(std::unique(cameraVector.begin(), cameraVector.end()), cameraVector.end());

	return cameraVector;
}

SvIe::SVVirtualCameraPtrVector SVInspectionProcess::GetCamerasForLut() const
{
	SvIe::SVVirtualCameraPtrVector cameraVector;

	for (const auto* const pCameraImage : m_CameraImages)
	{
		if (nullptr != pCameraImage)
		{
			SvIe::SVVirtualCamera* pCamera = pCameraImage->GetCamera();

			if (nullptr != pCamera)
			{
				if (!pCamera->IsFileAcquisition() && nullptr != pCamera->GetAcquisitionDevice())
				{
					SVLut lut;
					HRESULT hr = pCamera->GetLut(lut);
					if (S_OK == hr && lut.NumBands() > 0)
					{
						cameraVector.emplace_back(pCamera);
					}
				}
			}
		}
	}
	std::sort(cameraVector.begin(), cameraVector.end(), SvIe::isLessByName);
	//Remove duplicates
	cameraVector.erase(std::unique(cameraVector.begin(), cameraVector.end()), cameraVector.end());

	return cameraVector;
}

SvIe::SVVirtualCameraPtrVector SVInspectionProcess::GetCamerasForLightReference() const
{
	SvIe::SVVirtualCameraPtrVector cameraVector;

	for (const auto* const pCameraImage : m_CameraImages)
	{
		if (nullptr != pCameraImage)
		{
			SvIe::SVVirtualCamera* pCamera = pCameraImage->GetCamera();

			if (nullptr != pCamera)
			{
				if (!pCamera->IsFileAcquisition() && nullptr != pCamera->GetAcquisitionDevice())
				{
					SVLightReference lightReference;
					HRESULT hr = pCamera->GetLightReference(lightReference);
					if (S_OK == hr && lightReference.NumBands() > 0)
					{
						cameraVector.emplace_back(pCamera);
					}
				}
			}
		}
	}
	std::sort(cameraVector.begin(), cameraVector.end(), SvIe::isLessByName);
	//Remove duplicates
	cameraVector.erase(std::unique(cameraVector.begin(), cameraVector.end()), cameraVector.end());

	return cameraVector;
}

HRESULT SVInspectionProcess::GetMainImages(const std::string& rCameraName, SvIe::SVCameraImagePtrSet& rMainImages) const
{
	HRESULT l_Status = S_OK;

	rMainImages.clear();

	SvIe::SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		if (nullptr != (*l_Iter))
		{
			SvIe::SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if ((nullptr != pCamera) && (rCameraName == pCamera->GetName()))
			{
				rMainImages.insert(*l_Iter);
			}
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::RemoveImage(SvIe::SVImageClass* pImage)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pImage)
	{
		SVRemoveImages l_Data;

		l_Data.m_Images.insert(pImage->getObjectId());

		updateObserver(l_Data);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVToolSet* SVInspectionProcess::GetToolSet() const
{
	return m_pCurrentToolset;
}

bool SVInspectionProcess::CreateObject()
{
	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = this;

	m_isCreated = m_pCurrentToolset->createAllObjects(createStruct);

	m_RegressionTestPlayEquation.ConnectObject(createStruct);

	m_processFunctions[InspectionFunction::Inspection] = [this]() {ProcessInspection(); };
	m_processFunctions[InspectionFunction::NotifyLastInspected] = [this]() {ProcessNotifyWithLastInspected(); };
	m_processFunctions[InspectionFunction::ProcessCommand] = [this]() {ProcessCommandQueue(); };
	return m_isCreated;
}

bool SVInspectionProcess::ResetObject(SvStl::MessageContainerVector* pErrorMessages)
{
	m_resetting = true;
	bool Result = __super::ResetObject(pErrorMessages);

	if (!m_pCurrentToolset->IsCreated())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ToolsetNotCreated, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			pErrorMessages->push_back(Msg);
		}
	}

	m_pToolSetCamera = dynamic_cast<SvIe::SVVirtualCamera*> (SvOi::getObject(getFirstCamera()));

	m_bForceOffsetUpdate = true;

	m_resetting = false;
	return Result;
}

void SVInspectionProcess::UpdateMainImagesByProduct(SVInspectionInfoStruct& rIpInfoStruct, const SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos)
{
	SvIe::SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

	while (l_ImageIter != m_CameraImages.end())
	{
		SvIe::SVCameraImageTemplate* l_pImage = (*l_ImageIter);

		if (nullptr != l_pImage)
		{
			SvIe::SVVirtualCamera* pCamera = l_pImage->GetCamera();

			if (nullptr != pCamera)
			{
				SvIe::SVObjectIdSVCameraInfoStructMap::const_iterator Iter;

				Iter = rCameraInfos.find(pCamera->getObjectId());

				if (Iter != rCameraInfos.end())
				{
					if (nullptr == Iter->second.getImage())
					{
						SvStl::MessageManager Exception(SvStl::MsgType::Data);
						Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_InvalidImageBuffer, SvStl::SourceFileParams(StdMessageParams), getObjectId());
						Exception.Throw();
					}
					else
					{
						try
						{
							l_pImage->setImage(Iter->second.getImage(), rIpInfoStruct.m_triggerRecordWrite);
							if (!SvimState::CheckState(SV_STATE_RUNNING) && SvimState::CheckState(SV_STATE_READY) && Iter->second.getImage()->isValid())
							{
								pCamera->setTempImage(Iter->second.getImage()->getHandle()->GetBuffer());
							}
						}
						catch (const SvStl::MessageContainer& rExp)
						{
							//This is the topmost catch for MessageContainer exceptions
							SvStl::MessageManager Exception(SvStl::MsgType::Data);
							SvStl::MessageData Msg(rExp.getMessage());
							Msg.m_MessageCode = SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED;
							Exception.setMessage(Msg, getObjectId());
							Exception.Throw();
						}
					}
				}
				else
				{
					try
					{
						l_pImage->resetImage(rIpInfoStruct.m_triggerRecordWrite);
					}
					catch (const SvStl::MessageContainer&)
					{
						//do nothing, because it is only a repair try and this part throw an exception by its own.
					}

					SvStl::MessageManager Exception(SvStl::MsgType::Data);
					Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_CameraNotFound, SvStl::SourceFileParams(StdMessageParams), getObjectId());
					Exception.Throw();
				}
			}
			else
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Data);
				Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_InvalidCamera, SvStl::SourceFileParams(StdMessageParams), getObjectId());
				Exception.Throw();
			}
		}
		else
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_InvalidImage, SvStl::SourceFileParams(StdMessageParams), getObjectId());
			Exception.Throw();
		}

		++l_ImageIter;
	}
}

HRESULT SVInspectionProcess::copyValues2TriggerRecord(SvIe::RunStatus& rRunStatus)
{
	if (nullptr != rRunStatus.m_triggerRecord)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = std::format(_T("copyValues2TriggerRecord; {:d}\n"), rRunStatus.m_triggerRecord->getId());
		::OutputDebugString(DebugString.c_str());
#endif
		for (const auto& pValue : m_updateValueObjectSet)
		{
			if (nullptr != pValue)
			{
				pValue->updateMemBlockData();
			}
		}
		int32_t byteSize = static_cast<int32_t> (m_valueData.size());
		if (0 < byteSize)
		{
			rRunStatus.m_triggerRecord->writeValueData(&m_valueData.at(0), byteSize);
		}
		return S_OK;
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = std::format(_T("copyValues2TriggerRecord failed, no TR\n"), rRunStatus.m_triggerRecord->getId());
		::OutputDebugString(DebugString.c_str());
#endif
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TRC_Error_CopyValueObjData, SvStl::SourceFileParams(StdMessageParams));
	}
	return E_FAIL;
}

bool SVInspectionProcess::Run(SvIe::RunStatus& rRunStatus)
{
	// Validate IPDoc's values...
	bool retVal = IsCreated();

	if (retVal)
	{

		// Run the Toolset
		unsigned long l_state = rRunStatus.GetState();
		if (GetToolSet() && l_state != SvIe::RunStatus::SV_INVALID)
		{
			retVal &= GetToolSet()->Run(rRunStatus);
		}
		else
		{
			rRunStatus.SetInvalid();
			rRunStatus.SetInspectionStarted(false);
			if (SvIe::RunStatus::SV_INVALID == rRunStatus.GetState())
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_37_INVALID_RUN_STATUS, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		copyValues2TriggerRecord(rRunStatus);
	}
	else
	{
		SV_LOG_GLOBAL(trace) << "Inspection not valid";
		rRunStatus.SetInvalid();
		rRunStatus.SetInspectionStarted(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = _T("Inspection not valid\n");
		::OutputDebugString(DebugString.c_str());
#endif
	}

	return retVal;
	}

bool SVInspectionProcess::RunInspection(SVInspectionInfoStruct& rIPInfo, const SvIe::SVObjectIdSVCameraInfoStructMap& rCameraInfos, long triggerCount, bool p_UpdateCounts)
{
	if (true == m_resetting)
	{
		return false;
	}

	bool l_bOk = false;
	bool l_bInputRequest = false;
	bool l_bImageRequest = false;
	bool l_bUpdateMainImage = false;
	bool l_bRestMainImage = false;
	SvStl::MessageManager Exception(SvStl::MsgType::Log);

	m_runStatus.ResetRunStateAndToolSetTimes();

	m_runStatus.m_lTriggerCount = triggerCount;
	m_runStatus.m_UpdateCounters = p_UpdateCounts;

	if (isProductAlive(GetPPQ(), triggerCount))
	{
		// Process all input requests
		if (!ProcessInputRequests(m_bForceOffsetUpdate))
		{
			Exception.setMessage(SVMSG_SVO_38_INPUT_REQUEST_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));

			l_bInputRequest = true;
			m_runStatus.SetInvalid();  //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		rIPInfo.setNextTriggerRecord(SvOi::TriggerData {triggerCount});
		if (!ProcessInputImageRequests(rIPInfo, rCameraInfos))
		{
			Exception.setMessage(SVMSG_SVO_39_IMAGE_REQUEST_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));

			l_bImageRequest = true;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		if (nullptr != rIPInfo.m_triggerRecordWrite && rIPInfo.m_triggerRecordWrite->isObjectUpToTime())
		{
			auto pLastTriggerRecord = getLastProductData().second.m_triggerRecordComplete;
			if (nullptr != pLastTriggerRecord && pLastTriggerRecord->isObjectUpToTime())
			{
				rIPInfo.m_triggerRecordWrite->setImages(*pLastTriggerRecord);
			}
			else
			{
				rIPInfo.m_triggerRecordWrite->initImages();
			}
		}
		else
		{
			l_bImageRequest = true;
			Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunInspection_ResetTR, SvStl::SourceFileParams(StdMessageParams));
		}

		try
		{
			UpdateMainImagesByProduct(rIPInfo, rCameraInfos);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			Msg.setMessage(rSvE.getMessage());

			l_bUpdateMainImage = true;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}
	}
	else
	{
		//Product !bAlive
		SvIe::SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while (l_ImageIter != m_CameraImages.end())
		{
			SvIe::SVCameraImageTemplate* l_pImage = (*l_ImageIter);

			if (nullptr != l_pImage && nullptr != rIPInfo.m_triggerRecordWrite)
			{
				try
				{
					l_pImage->resetImage(rIPInfo.m_triggerRecordWrite);
				}
				catch (const SvStl::MessageContainer&)
				{
					//nothing to do.
				}
			}

			++l_ImageIter;
		}

		m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		//do a copy forward, and NOT Run();
		l_bRestMainImage = true;
	}

	// Tool set is disabled! // Put out tool set disabled...
	if (!GetToolSet()->IsEnabled())
	{
		rIPInfo.m_InspectedState = PRODUCT_INSPECTION_DISABLED;
	}// end if

	rIPInfo.m_BeginToolset = SvUl::GetTimeStamp();
	if (!rIPInfo.m_BeginToolset)
	{
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams));
		DebugBreak();
	}

	m_runStatus.m_triggerRecord = std::move(rIPInfo.m_triggerRecordWrite);
	if (!l_bInputRequest && !l_bImageRequest && !l_bUpdateMainImage && !l_bRestMainImage)
	{
		l_bOk = Run(m_runStatus);
	}
	else
	{
		copyValues2TriggerRecord(m_runStatus);
	}
	rIPInfo.m_triggerRecordWrite = std::move(m_runStatus.m_triggerRecord);
	m_runStatus.m_triggerRecord = nullptr;

	m_bForceOffsetUpdate = false;

	rIPInfo.m_EndToolset = SvUl::GetTimeStamp();
	if (!rIPInfo.m_EndToolset)
	{
		SvStl::MessageManager e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams));
		DebugBreak();
	}

	if (m_runStatus.IsValid() && !m_runStatus.IsCriticalFailure())
	{
		rIPInfo.m_InspectedState = PRODUCT_INSPECTION_RUN;
	}
	else
	{
		rIPInfo.m_InspectedState = PRODUCT_NOT_ACKNOWLEDGED;
	}

	rIPInfo.m_ToolSetAvgTime = m_runStatus.m_ToolSetAvgTime;
	rIPInfo.m_ToolSetEndTime = m_runStatus.m_ToolSetEndTime;

	return l_bOk;

}// end RunInspection

void SVInspectionProcess::SetDefaultInputs()
{
	// Setup Connections
	if (m_pCurrentToolset)
	{
		m_pCurrentToolset->SetDefaultInputs();
	}// end if
}

LPCTSTR SVInspectionProcess::GetDeviceName() const
{
	return m_DeviceName.c_str();
}

void SVInspectionProcess::SetDeviceName(LPCTSTR p_szDeviceName)
{
	m_DeviceName = p_szDeviceName;
}

void SVInspectionProcess::ResetName()
{
	SVObjectClass::ResetName();

	SVInspectionNameUpdate l_Data(GetName());

	updateObserver(l_Data);
}

void SVInspectionProcess::SetName(LPCTSTR StrString)
{
	SVObjectClass::SetName(StrString);

	SVInspectionNameUpdate l_Data(GetName());

	updateObserver(l_Data);

	if (nullptr != m_pCurrentToolset)
	{
		m_pCurrentToolset->setInspectionname(StrString);
	}
}

HRESULT SVInspectionProcess::RegisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	//! Object is an Image
	if (SvPb::SVImageObjectType == pObject->GetObjectType())
	{
		auto* pCameraImage = dynamic_cast<SvIe::SVCameraImageTemplate*> (pObject);

		if (nullptr != pCameraImage)
		{
			m_CameraImages.insert(pCameraImage);
		}
		Result = S_OK;
	}
	//! Object is a Value Object
	else if (nullptr != (pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject)))
	{
		m_ValueObjectSet.insert(pValueObject);
		Result = S_OK;
	}

	return Result;
}

HRESULT SVInspectionProcess::UnregisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	//! Object is an Image
	if (SvPb::SVImageObjectType == pObject->GetObjectType())
	{
		auto* pCameraImage = dynamic_cast<SvIe::SVCameraImageTemplate*>(pObject);

		if (nullptr != pCameraImage)
		{
			if (!(m_CameraImages.empty()))
			{
				m_CameraImages.erase(pCameraImage);
			}
		}
		Result = S_OK;
	}
	//! Object is a Value Object
	else if (nullptr != (pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject)))
	{
		if (!m_ValueObjectSet.empty())
		{
			m_ValueObjectSet.erase(pValueObject);
		}
		if (!m_updateValueObjectSet.empty())
		{
			m_updateValueObjectSet.erase(pValueObject);
		}
		Result = S_OK;
	}

	return Result;
}

void SVInspectionProcess::SetToolsetImage(const std::string& rToolsetImage)
{
	m_ToolSetCameraName = rToolsetImage;
}

LPCTSTR SVInspectionProcess::GetToolsetImage()
{
	return m_ToolSetCameraName.c_str();
}

SvIe::SVCameraImageTemplate* SVInspectionProcess::GetToolSetMainImage()
{
	SvIe::SVCameraImageTemplate* pImage = nullptr;

	SVToolSet* l_pToolSet = GetToolSet();

	if (nullptr != l_pToolSet)
	{
		pImage = dynamic_cast<SvIe::SVCameraImageTemplate*>(l_pToolSet->getCurrentImage());
	}

	return pImage;
}

void SVInspectionProcess::Persist(SvOi::IObjectWriter& rWriter, bool closeObject/* = true*/) const
{
	// until it becomes a task object list...
	rWriter.StartElement(GetObjectName()); // use internal name for node name
	SVObjectClass::Persist(rWriter);

	// Save NewDisableMethod
	_variant_t value;
	value.ChangeType(VT_I4); // was VT_INT...
	value.lVal = static_cast<BOOL>(IsNewDisableMethodSet());
	rWriter.WriteAttribute(SvXml::CTAG_INSPECTION_NEW_DISABLE_METHOD, value);
	value.Clear();

	if (m_InputObjects.size())
	{
		rWriter.StartElement(SvXml::CTAG_IO);
		for (auto pElement : m_InputObjects)
		{
			if (nullptr != pElement)
			{
				SVObjectClass* pInputObject = dynamic_cast<SVObjectClass*> (pElement.get());
				if (nullptr != pInputObject)
				{
					rWriter.WriteAttribute(pInputObject->GetName(), convertObjectIdToVariant(pInputObject->getObjectId()));
				}
			}
		}
		rWriter.EndElement();
	}

	// Save the Toolset
	SVToolSet* pToolSet = GetToolSet();
	pToolSet->Persist(rWriter);

	if (closeObject)
	{
		rWriter.EndElement();
	}
}

SVResultList* SVInspectionProcess::GetResultList() const
{
	SVResultList* retVal = nullptr;
	SVToolSet* pToolSet = GetToolSet();
	if (pToolSet)
	{
		retVal = pToolSet->GetResultList();
	}
	return retVal;
}

#pragma region IInspectionProcess methods
void SVInspectionProcess::fillPPQSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, const UINT attribute, SvPb::ObjectSelectorType type) const
{
	SVPPQObject* pPPQ = GetPPQ();
	SVObjectReferenceVector objectVector;

	if (nullptr != pPPQ)
	{
		std::string PpqName(pPPQ->GetName());

		SVObjectManagerClass::Instance().getTreeList(PpqName, objectVector, attribute);
		fillList(treeInserter, objectVector, type);
	}


	SVObjectPtrVector PpqVariables {getPPQVariables()};
	std::string InspectionName = GetName();

	for (auto pObject : PpqVariables)
	{
		if (nullptr != pObject && pObject->isCorrectType(type))
		{
			//Check if the attribute of the object is allowed
			if (0 != (attribute & pObject->ObjectAttributesAllowed()))
			{
				SVObjectReference ObjectRef(pObject);
				SvPb::TreeItem insertItem;

				std::string location(ObjectRef.GetCompleteName(true));
				insertItem.set_name(ObjectRef.GetName());
				if (nullptr != ObjectRef.getValueObject())
				{
					insertItem.set_type(ObjectRef.getValueObject()->getTypeName());
				}
				//Need to replace the inspection name with the PPQ Variables name
				// Only DIO and Remote Input, but is all that is in this list?
				SvUl::searchAndReplace(location, InspectionName.c_str(), SvDef::FqnPPQVariables);
				insertItem.set_location(location);
				insertItem.set_objectidindex(ObjectRef.GetObjectIdAndIndexOneBased());
				treeInserter = insertItem;
			}
		}
	}
}

void SVInspectionProcess::fillCameraSelectorList(std::back_insert_iterator<std::vector<SvPb::TreeItem>> treeInserter, const UINT attribute, SvPb::ObjectSelectorType type) const
{
	const auto& rCameras = GetCameras();
	for (const auto* pCamera : rCameras)
	{
		if (nullptr != pCamera)
		{
			std::string cameraName(pCamera->GetName());

			SVObjectReferenceVector objectVector;
			SVObjectManagerClass::Instance().getTreeList(cameraName, objectVector, attribute);

			fillList(treeInserter, objectVector, type);
		}
	}
}

SvOi::ITaskObject* SVInspectionProcess::GetToolSetInterface() const
{
	return GetToolSet();
}

HRESULT SVInspectionProcess::RunOnce()
{
	SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemNone;
	bool result = ProcessInputRequests(eResetItem);

	SingleRunModeLoop(true);

	return result ? S_OK : E_FAIL;
}

HRESULT SVInspectionProcess::SubmitCommand(const SvOi::ICommandPtr& rCommandPtr)
{
	HRESULT Result = S_OK;

	if (m_CommandQueue.AddTail(rCommandPtr))
	{
		m_processThread.Signal(reinterpret_cast<ULONG_PTR> (&m_processFunctions[InspectionFunction::ProcessCommand]));
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

uint32_t SVInspectionProcess::getFirstCamera() const
{
	uint32_t cameraId {SvDef::InvalidObjectId};

	if (!(m_ToolSetCameraName.empty()))
	{
		SvOi::IObjectClass* pCamera = SvOi::getObjectByDottedName(m_ToolSetCameraName);
		cameraId = (nullptr != pCamera) ? pCamera->getObjectId() : SvDef::InvalidObjectId;
	}

	if (SvDef::InvalidObjectId == cameraId)
	{
		SvOi::IObjectClass* pCamera = getFirstPPQCamera();
		cameraId = (nullptr != pCamera) ? pCamera->getObjectId() : SvDef::InvalidObjectId;
	}

	return cameraId;
}

HRESULT SVInspectionProcess::addSharedCamera(uint32_t cameraID)
{
	HRESULT result = S_OK;

	SVPPQObject* pPPQ = GetPPQ();

	auto* pCamera = dynamic_cast<SvIe::SVVirtualCamera*> (SvOi::getObject(cameraID));
	if (nullptr != pCamera && nullptr != pPPQ)
	{
		pPPQ->AddSharedCamera(pCamera);
	}
	else
	{
		result = E_FAIL;
	}

	return result;
}

HRESULT SVInspectionProcess::resetTool(SvOi::IObjectClass& rTool)
{
	HRESULT result {E_FAIL};
	m_resetting = true;
	m_bForceOffsetUpdate = true;
	/// correct tool size when it does not fit to the parent image 
	AddResetState(SvDef::SVResetAutoMoveAndResize);
	bool doRunOnce = rTool.resetAllObjects();
	m_resetting = false;
	if (doRunOnce)
	{
		result = RunOnce();
	}
	RemoveResetState(SvDef::SVResetAutoMoveAndResize);
	return result;
}

HRESULT SVInspectionProcess::resetToolAndDependends(SvOi::IObjectClass* pTool)
{
	HRESULT result {E_FAIL};
	m_resetting = true;
	m_bForceOffsetUpdate = true;
	/// correct tool size when it does not fit to the parent image 
	AddResetState(SvDef::SVResetAutoMoveAndResize);
	bool doRunOnce = pTool->resetAllObjects();

	bool resetDependentsOK {true};

	std::vector<uint32_t> vec;
	std::back_insert_iterator<std::vector<uint32_t>> backiter(vec);
	SvTo::InsertDependentTools(backiter, pTool->getObjectId());
	SvUl::RemoveDuplicates(vec);


	for (const auto id : vec)
	{
		SVObjectClass* pObject = nullptr;
		SVObjectManagerClass::Instance().GetObjectByIdentifier(id, pObject);
		if (pObject)
		{
			resetDependentsOK = pObject->resetAllObjects(nullptr, SvTo::cResetDepth) && resetDependentsOK;
			Log_Assert(pObject->getToolPtr() != nullptr);
		}

	}

	m_resetting = false;
	if (doRunOnce)
	{
		result = RunOnce();
	}
	RemoveResetState(SvDef::SVResetAutoMoveAndResize);
	return result;
}


HRESULT SVInspectionProcess::propagateSizeAndPosition()
{
	HRESULT retVal = S_OK;
	bool result = false;
	SVToolSet* pToolSet = GetToolSet();
	if (pToolSet)
	{
		for (int index = 0; index < pToolSet->GetSize(); index++)
		{
			auto* pTool = dynamic_cast<SvTo::SVToolClass*>(pToolSet->getTaskObject(index));
			if (nullptr != pTool)
			{
				result = pTool->propagateSizeAndPosition() || result;
			}
		}
	}
	if (result)
	{
		retVal = RunOnce();
	}
	else
	{
		retVal = E_FAIL;
	}
	return retVal;
}

bool SVInspectionProcess::usePropagateSizeAndPosition() const
{
	SVToolSet* pToolSet = GetToolSet();
	if (pToolSet)
	{
		for (int index = 0; index < pToolSet->GetSize(); index++)
		{
			auto* pTool = dynamic_cast<SvTo::SVToolClass*>(pToolSet->getTaskObject(index));
			if (nullptr != pTool)
			{
				if (pTool->usePropagateSizeAndPosition())
				{
					return true;
				}
			}
		}
	}
	return false;
}

SvPb::OverlayDesc SVInspectionProcess::getOverlayStruct(const SvOi::ISVImage& rImage) const
{
	auto* pToolSet = GetToolSet();
	if (nullptr != pToolSet)
	{
		return pToolSet->getOverlayStruct(rImage);
	}
	SvStl::MessageContainer msg(SVMSG_SVO_NULL_POINTER, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams), getObjectId());
	throw msg;
}

void SVInspectionProcess::setResultListUpdateFlag()
{
	auto* pResultList = GetResultList();
	if (pResultList)
	{
		pResultList->setUpdateFlag();
	}
}
#pragma endregion IInspectionProcess methods

SVObjectPtrVector SVInspectionProcess::getPPQVariables() const
{
	SVObjectPtrVector Result;
	std::map<std::string, SVObjectClass*> NameObjectMap;
	SvDef::StringVector PpqVariableNames;

	for (auto& pInput : m_InputObjects)
	{
		if (nullptr != pInput)
		{
			SVObjectClass* pObject {dynamic_cast<SVObjectClass*> (pInput.get())};
			if (nullptr != pObject)
			{
				std::string Name(pObject->GetCompleteName());
				NameObjectMap[Name] = pObject;
				PpqVariableNames.push_back(Name);
			}
		}
	}// end for

	//Sort the PPQ variable names and return the respective SVObjectClass pointers in order
	std::sort(PpqVariableNames.begin(), PpqVariableNames.end(), SvUl::NaturalStringCompare<std::string>()); // sort strings
	std::for_each(PpqVariableNames.begin(), PpqVariableNames.end(), [&Result, &NameObjectMap](const std::string& rItem)->void
	{
		Result.push_back(NameObjectMap[rItem]);
	}
	);

	return Result;
}

DWORD SVInspectionProcess::GetObjectColor() const
{
	return SvDef::DefaultWhiteColor;
}

void SVInspectionProcess::getToolMessages(SvStl::MessageContainerInserter& rInserter) const
{
	SVToolSet* pToolSet(GetToolSet());

	if (nullptr != pToolSet)
	{
		for (int i = 0; i < pToolSet->GetSize(); i++)
		{
			SvIe::SVTaskObjectClass* pTaskObject(pToolSet->getTaskObject(i));
			if (nullptr != pTaskObject)
			{
				const SvStl::MessageContainerVector& rToolMessages(pTaskObject->getErrorMessages());
				std::copy(rToolMessages.begin(), rToolMessages.end(), rInserter);
			}
		}
	}
}

bool SVInspectionProcess::createAllObjects()
{
	bool retValue = CreateObject();
	if (nullptr != GetToolSet())
	{
		return GetToolSet()->createAllObjects(SVObjectLevelCreateStruct {*this}) && retValue;
	}
	return false;
}

bool SVInspectionProcess::CreateChildObject(SVObjectClass* pChildObject, DWORD context)
{
	if (nullptr == pChildObject)
	{
		return false;
	}
	
	SVObjectLevelCreateStruct createStruct(*this);
	createStruct.m_pInspection = this;

	bool Return = pChildObject->createAllObjects(createStruct);

	if (SvDef::SVMFResetObject == (context & SvDef::SVMFResetObject))
	{
		pChildObject->resetAllObjects();
	}

	if (SvDef::SVMFSetDefaultInputs == (context & SvDef::SVMFSetDefaultInputs))
	{
		SetDefaultInputs();
	}

	if (SvDef::SVMFResetInspection == (context & SvDef::SVMFResetInspection))
	{
		resetAllObjects();
	}

	return Return;
}

void SVInspectionProcess::ConnectObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	__super::ConnectObject(rCreateStructure);

	if (nullptr != GetToolSet())
	{
		GetToolSet()->ConnectObject(rCreateStructure);
	}
}

bool SVInspectionProcess::DestroyChildObject(SVObjectClass* pChild)
{
	if (pChild == m_pCurrentToolset)
	{
		m_pCurrentToolset = nullptr;
	}

	// if we have an Object
	if (nullptr != pChild)
	{
		auto* pTaskObject = dynamic_cast<SvIe::SVTaskObjectClass*>(pChild);
		if (nullptr != pTaskObject)
		{
			// Notify the Owner of our inputs that they are not needed anymore
			pTaskObject->disconnectAllInputs();
		}

		// Close the Object
		pChild->CloseObject();

		// Destroy our Friends
		pChild->DestroyFriends();

		// Destruct it
		delete pChild;

		return true;
	}
	return false;
}

SvOi::IObjectClass* SVInspectionProcess::getFirstObject(const SvDef::SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
{
	SvOi::IObjectClass* retValue = SVObjectClass::getFirstObject(rObjectTypeInfo, useFriends, pRequestor);

	if (nullptr != GetToolSet() && nullptr == retValue)
	{
		return GetToolSet()->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
	}
	return retValue;
}

void SVInspectionProcess::OnObjectRenamed(const SVObjectClass& rRenamedObject, const std::string& rOldName)
{

	m_RegressionTestPlayEquation.OnObjectRenamed(rRenamedObject, rOldName);
	if (GetToolSet())
	{
		GetToolSet()->OnObjectRenamed(rRenamedObject, rOldName);
	}
}

bool SVInspectionProcess::connectAllInputs()
{
	if (GetToolSet())
	{
		return GetToolSet()->connectAllInputs();
	}
	return false;
}

void SVInspectionProcess::disconnectAllInputs()
{
	__super::disconnectAllInputs();
	if (GetToolSet())
	{
		GetToolSet()->disconnectAllInputs();
	}
}

bool SVInspectionProcess::replaceObject(SVObjectClass* pObject, uint32_t newId)
{
	if (nullptr != pObject)
	{
		// Kill the Friends
		// SVObjectClass can have Friends
		pObject->DestroyFriends();

		// Special code for Duplicates!!
		SVObjectClass* pDuplicateObject = SVObjectManagerClass::Instance().GetObject(newId);
		if (pDuplicateObject)
		{
			if (pDuplicateObject == m_pCurrentToolset)
			{
				m_pCurrentToolset = nullptr;
			}

			// Get the Owner
			SVObjectClass* pOwner = pDuplicateObject->GetParent();
			if (pOwner)
			{
				// Ask the owner to kill the imposter!
				bool isDestroyed = false;
				if (this == pOwner)
				{
					isDestroyed = DestroyChildObject(pDuplicateObject);
				}
				else
				{
					auto* pTaskListOwner = dynamic_cast<SvIe::SVTaskObjectListClass*>(pOwner);
					auto* pTaskList = dynamic_cast<SvIe::SVTaskObjectListClass*>(pDuplicateObject);
					if (nullptr != pTaskListOwner)
					{
						isDestroyed = pTaskList->DestroyChildObject(pTaskList);
					}
				}
				if (!isDestroyed)
				{
					// must be a Friend
					pOwner->DestroyFriends();
				}
			}
		}

		// Special code for Objects that allocate Friends on SetOwner()
		pObject->DestroyFriends();

		if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pObject, newId))
		{
			SVToolSet* l_pToolSet(nullptr);

			l_pToolSet = dynamic_cast<SVToolSet*>(pObject);

			if (nullptr != l_pToolSet)
			{
				if (nullptr != m_pCurrentToolset)
				{
					DestroyChildObject(m_pCurrentToolset);
				}

				m_pCurrentToolset = l_pToolSet;

				m_pCurrentToolset->SetObjectOwner(this);

				CreateChildObject(m_pCurrentToolset);

				if (nullptr != m_pToolSetConditional)
				{
					m_pCurrentToolset->AddFriend(m_pToolSetConditional);

					m_pCurrentToolset->CreateChildObject(m_pToolSetConditional);

					m_pToolSetConditional = nullptr;
				}
			}
			else
			{
				auto* pConditional = dynamic_cast<SvOp::SVConditional*>(pObject);

				if (nullptr != pConditional)
				{
					m_pToolSetConditional = pConditional;
					m_pToolSetConditional->SetObjectOwner(this);
				}
			}

			return true;
		}
	}
	return false;
}

SvOi::IFormulaControllerPtr SVInspectionProcess::getRegressionTestPlayConditionController()
{
	if (nullptr == m_pRegressionTestPlayEquationController)
	{
		m_pRegressionTestPlayEquationController = SvOi::IFormulaControllerPtr {new SvOgu::FormulaController(getObjectId(), getObjectId(), m_RegressionTestPlayEquation.getObjectId())};
	}
	return m_pRegressionTestPlayEquationController;
}

bool SVInspectionProcess::shouldPauseRegressionTestByCondition()
{
	if (!getRegressionTestPlayConditionController()->GetEquationText().empty())
	{
		double value = m_RegressionTestPlayEquation.RunAndGetResult();
		return 0 == value;
	}

	return false;
}

void SVInspectionProcess::buildValueObjectData()
{
	SvPb::DataDefinitionList dataDefList;

	m_updateValueObjectSet.clear();
	auto* pList = dataDefList.mutable_list();
	for (auto* pValueObject : m_ValueObjectSet)
	{
		if (nullptr != pValueObject)
		{
			long memSize = pValueObject->getByteSize(false, true);
			if (0 != memSize)
			{
				auto* pValueObjectDef = pList->Add();
				const SvOi::IObjectClass* const pObject = dynamic_cast<const SvOi::IObjectClass* const> (pValueObject);
				pValueObjectDef->set_objectid(pObject->getObjectId());
				pValueObjectDef->set_name(pObject->GetCompleteName());
				pValueObjectDef->set_type(pObject->GetObjectSubType());
				pValueObjectDef->set_typestring(pValueObject->getTypeName());
				pValueObjectDef->set_vttype(pValueObject->GetType());
				pValueObjectDef->set_arraysize(pValueObject->getArraySize());
				int valueObjectMemOffset = pValueObject->getMemOffset();
				if (-1 == valueObjectMemOffset)
				{
					///This value object has not yet been allocated in inspection memory block
					pValueObjectDef->set_memoffset(m_memValueDataOffset);
					pValueObject->setTrData(m_memValueDataOffset, memSize, pList->size() - 1);
					m_memValueDataOffset += memSize;
				}
				else
				{
					///When the memory offset has already been set then it must match the calculated offset
					pValueObjectDef->set_memoffset(valueObjectMemOffset);
					///Only update the trigger record position and delete the pointer
					pValueObject->setTrData(-1, -1, pList->size() - 1);
				}
				switch (pObject->GetObjectSubType())
				{
					case SvPb::DoubleSortValueObjectType:
					case SvPb::SVStringValueObjectType:
					case SvPb::SVVariantValueObjectType:
					{
						m_updateValueObjectSet.insert(pValueObject);
						break;
					}
					default:
					{
						break;
					}
				}
			}
			else
			{
				///Reset all TR settings including the memory block pointer
				pValueObject->setTrData(-1, -1, -1);
			}
		}
	}

	///We need to reallocate the memory block if the size not the same
	if (m_memValueDataOffset != static_cast<long> (m_valueData.size()))
	{
		m_valueData.resize(m_memValueDataOffset);
	}

	///We now need to set the value object pointer to the correct address
	for (auto* pValueObject : m_ValueObjectSet)
	{
		if (nullptr != pValueObject)
		{
			long memSize = pValueObject->getByteSize(false, true);
			if (0 != memSize && 0 < m_valueData.size())
			{
				pValueObject->setMemBlockPointer(&m_valueData.at(0));
			}
		}
	}
#ifdef TRACE_MEMORYBLOCK
	if (m_memValueDataOffsetPrev != m_memValueDataOffset)
	{

		std::string msg = std::format("memsize Changed from {} to {} for Insp{}\n", m_memValueDataOffsetPrev, m_memValueDataOffset, m_trcPos);
		OutputDebugString(msg.c_str());
		m_memValueDataOffsetPrev = m_memValueDataOffset;
	}
#endif 
	auto* pTrcRW = SvOi::getTriggerRecordControllerRWInstance();
	if (nullptr != pTrcRW)
	{
		pTrcRW->changeDataDef(std::move(dataDefList), m_memValueDataOffset, m_trcPos);
	}

}

void SVInspectionProcess::setIOObjectIdMap(std::map<std::string, uint32_t>&& ioObjectMap)
{
	m_ioObjectMap = std::move(ioObjectMap);
}

void SVInspectionProcess::attachObserver(SVDisplayObject* pDisplayObject)
{
	if (nullptr != pDisplayObject && std::ranges::none_of(m_observerList, [pDisplayObject](const auto& rEntry) { return pDisplayObject == rEntry; }))
	{
		m_observerList.push_back(pDisplayObject);
	}
}

void SVInspectionProcess::detachObserver(SVDisplayObject* pDisplayObject)
{
	std::erase(m_observerList, pDisplayObject);
}

void SVInspectionProcess::fillObjectList(std::back_insert_iterator<std::vector<SvOi::IObjectClass*>> inserter, const SvDef::SVObjectTypeInfoStruct& rObjectInfo, bool addHidden /*= false*/, bool stopIfClosed /*= false*/, bool /*firstObject = false*/)
{
	__super::fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
	if (nullptr != m_pCurrentToolset)
	{
		m_pCurrentToolset->fillObjectList(inserter, rObjectInfo, addHidden, stopIfClosed);
	}
}


void SVInspectionProcess::LastProductUpdate(const SVProductInfoStruct& rProduct)
{
	rProduct.SetProductComplete();
	std::lock_guard<std::mutex>  Autolock(m_inspectionMutex);
	m_lastRunProduct.Assign(rProduct);
	m_lastRunProductValid = true;
}

SVProductInfoStruct SVInspectionProcess::LastProductGet() const
{
	SVProductInfoStruct result;

	std::lock_guard<std::mutex>  Autolock(m_inspectionMutex);
	result.Assign(m_lastRunProduct);
	if (false == m_lastRunProductValid)
	{
		result.Reset();
	}

	return result;
}



SvOi::SVResetItemEnum  SVInspectionProcess::AddToResetIds(SvOi::SVResetItemEnum eResetItem, SVObjectClass* pObject, std::back_insert_iterator<std::vector<uint32_t>>  ResetIdIt)
{
	if (eResetItem == SvOi::SVResetItemIP || pObject == nullptr)
	{
		return SvOi::SVResetItemIP;
	}
	auto* pTool = dynamic_cast<SvTo::SVToolClass*> (pObject->GetAncestor(SvPb::SVToolObjectType));
	if (pTool == nullptr)
	{
		return SvOi::SVResetItemIP;
	}
	SvOi::SVResetItemEnum ret {eResetItem};
	switch (eResetItem)
	{
		case SvOi::SVResetItemIP:
		{
			break;
		}
		case SvOi::SVResetItemToolAndDependent:
		{

			SVObjectClass* pParent = pTool->GetParent();

			//@TODO[MEC][10.30][13.12.2022] find the supertool ????
			if (nullptr != pParent && SvPb::SVToolObjectType == pParent->GetObjectType())
			{
				ResetIdIt = pParent->getObjectId();
				SvTo::InsertDependentTools(ResetIdIt, pParent->getObjectId());

			}
			else
			{
				ResetIdIt = pTool->getObjectId();
				SvTo::InsertDependentTools(ResetIdIt, pTool->getObjectId());
			}
			break;
		}

		case SvOi::SVResetItemTool:
		{
			ResetIdIt = pTool->getObjectId();

			break;
		}
		case SvOi::SVResetItemOwner:
		{
			if (pObject->GetParent())
			{
				ResetIdIt = pObject->GetParent()->getObjectId();
			}
			break;
		}
		case SvOi::SVResetItemNone:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	return ret;
}
