//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVInspectionProcessResetStruct.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVImageLibrary\SVImagingDeviceParams.h"
#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"

#include "SVXMLLibrary\SVConfigurationTags.h"
#include "SVUtilityLibrary\NaturalStringCompare.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVObserver.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVToolSet.h"
#include "SVPPQObject.h"
#include "SVGlobal.h"
#include "Tools/SVTool.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "Operators/SVConditional.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "Definitions/Color.h"
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
#include "SVTimerLibrary\SVProfiler.h"
#endif
#include "SVSharedMemoryLibrary\SharedMemWriter.h"
#include "SVOGui\FormulaController.h"
#include "SVProtoBuf/ConverterHelper.h"
#include "SVLogLibrary/Logging.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVValueObjectLibrary/SVFileNameValueObjectClass.h"
#include "SVValueObjectLibrary/SVVariantValueObjectClass.h"
#include "InspectionEngine/SVImageClass.h"
#pragma endregion Includes

//#define TRACE_TRC

SV_IMPLEMENT_CLASS(SVInspectionProcess, SVInspectionProcessGuid);

bool isProductAlive(SVPPQObject* pPPQ, long triggerCount)
{
	bool l_Status = !SVSVIMStateClass::CheckState(SV_STATE_RUNNING);

	if (!l_Status && nullptr != pPPQ)
	{
		l_Status = pPPQ->IsProductAlive(triggerCount);
	}

	return l_Status;
}

HRESULT SVInspectionProcess::ProcessInspection(bool& rProcessed, SVProductInfoStruct& rProduct)
{
	HRESULT l_Status = S_OK;

	rProcessed = (0 < m_qInspectionsQueue.GetCount());

	if (rProcessed)
	{
		rProcessed = m_qInspectionsQueue.GetHead(&rProduct);
	}

	if (rProcessed)
	{
		m_qInspectionsQueue.pop_front();
#ifdef EnableTracking
		m_InspectionTracking.EventStart(_T("Process Inspections"));
#endif
		size_t l_InputXferCount = 0;
		m_bInspecting = true;	// do this before RemoveHead

		// Get the info struct for this inspection
		SVInspectionInfoStruct& rIPInfo = rProduct.m_svInspectionInfos[GetUniqueObjectID()];
		rIPInfo.m_BeginInspection = SvTl::GetTimeStamp();

		double time {0.0};
		if (rProduct.m_triggerInfo.m_PreviousTrigger > 0.0)
		{
			time = (rProduct.m_triggerInfo.m_BeginProcess - rProduct.m_triggerInfo.m_PreviousTrigger) * SvTl::c_MicrosecondsPerMillisecond;
		}
		m_pCurrentToolset->setTime(time, ToolSetTimes::TriggerDelta);

		time = (rIPInfo.m_BeginInspection - rProduct.m_triggerInfo.m_BeginProcess) * SvTl::c_MicrosecondsPerMillisecond;
		m_pCurrentToolset->setTime(time, ToolSetTimes::TriggerToStart);

		double triggerToAcqTime {0.0};
		double acqTime {0.0};
		if (nullptr != m_pToolSetCamera)
		{
			SvIe::SVGuidSVCameraInfoStructMap::const_iterator iterCamera(rProduct.m_svCameraInfos.find(m_pToolSetCamera->GetUniqueObjectID()));
			if (rProduct.m_svCameraInfos.cend() != iterCamera)
			{
				triggerToAcqTime = (iterCamera->second.m_StartFrameTimeStamp - rProduct.m_triggerInfo.m_BeginProcess) * SvTl::c_MicrosecondsPerMillisecond;
				acqTime = (iterCamera->second.m_EndFrameTimeStamp - iterCamera->second.m_StartFrameTimeStamp) * SvTl::c_MicrosecondsPerMillisecond;
			}
		}
		//If tool set camera not found set values to 0.0
		m_pCurrentToolset->setTime(triggerToAcqTime, ToolSetTimes::TriggerToAcquisitionStart);
		m_pCurrentToolset->setTime(acqTime, ToolSetTimes::AcquisitionTime);

		SvTh::IntVariantMap::const_iterator iterData{rProduct.m_triggerInfo.m_Data.end()};
		iterData = rProduct.m_triggerInfo.m_Data.find(SvTh::TriggerDataEnum::ObjectID);
		if (rProduct.m_triggerInfo.m_Data.end() != iterData)
		{
			m_pCurrentToolset->setObjectID(static_cast<double> (iterData->second));
		}
		iterData = rProduct.m_triggerInfo.m_Data.find(SvTh::TriggerDataEnum::TriggerIndex);
		if (rProduct.m_triggerInfo.m_Data.end() != iterData)
		{
			m_pCurrentToolset->setTriggerIndex(iterData->second);
		}


		assert(m_PPQInputs.size() == rProduct.m_triggerInfo.m_Inputs.size());
		// Copy inputs to Inspection Process's Value objects
		for (size_t i = 0; i < m_PPQInputs.size(); i++)
		{
			auto& rInputEntry = m_PPQInputs[i];
			const auto& rInputValue = rProduct.m_triggerInfo.m_Inputs.at(i);
				
			//Is the input enabled
			if(nullptr != rInputEntry.m_IOEntryPtr && rInputEntry.m_IOEntryPtr->m_Enabled && VT_EMPTY != rInputValue.vt)
			{
				rInputEntry.m_EntryValid = true;
				switch(rInputEntry.m_IOEntryPtr->m_ObjectType)
				{
					case IO_DIGITAL_INPUT:
					case IO_REMOTE_INPUT:
					case IO_CAMERA_DATA_INPUT:
					{
						if (nullptr != rInputEntry.m_IOEntryPtr->getValueObject())
						{
							rInputEntry.m_IOEntryPtr->getValueObject()->setValue(rInputValue);
						}
						++l_InputXferCount;
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
				rInputEntry.m_EntryValid = false;
				if (!(rInputEntry.m_IOEntryPtr->m_Enabled))
				{
					++l_InputXferCount;
				}
			}
		}

		bool l_Process = (l_InputXferCount == m_PPQInputs.size()) && (isProductAlive(GetPPQ(), rProduct.m_triggerInfo.lTriggerCount));

		// Check if tool set is alive and enabled...
		if (!l_Process)
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
			RunInspection(rIPInfo, rProduct.m_svCameraInfos, rProduct.m_triggerInfo.lTriggerCount);

			if (PRODUCT_INSPECTION_NOT_RUN != (rIPInfo.m_InspectedState & PRODUCT_INSPECTION_NOT_RUN))
			{
				rIPInfo.m_InspectedState = GetToolSet()->GetResultList()->GetInspectionState();

				rIPInfo.m_EndInspection = SvTl::GetTimeStamp();
				rIPInfo.setTriggerRecordCompleted();

				time = (rIPInfo.m_EndInspection - rProduct.m_triggerInfo.m_BeginProcess) * SvTl::c_MicrosecondsPerMillisecond;
				m_pCurrentToolset->setTime(time, ToolSetTimes::TriggerToCompletion);

				rIPInfo.m_ObjectID = m_pCurrentToolset->getInspectedObjectID();
				long ppqPosition {0L};
				SVPPQObject* pPpq = GetPPQ();
				if (nullptr != pPpq)
				{
					ppqPosition = pPpq->getPpqPosition(rProduct.ProcessCount());
				}
				m_pCurrentToolset->setPpqPosition(ppqPosition);
#ifdef _DEBUG
				//					std::string l_TempStateString = SvUl::Format( _T( "SVInspectionProcess::ProcessInspection|%s|TRI=%ld\n" ),
				//						GetName(), p_rProduct.ProcessCount() );
				//					::OutputDebugString( l_TempStateString.c_str() );
#endif

					// Mark last run product
				LastProductUpdate(&rProduct);
			}
		}

		SVObjectManagerClass::Instance().DecrementInspectionIndicator();

		if (l_Process)
		{
			::InterlockedIncrement(&m_NotifyWithLastInspected);

			rIPInfo.ClearIndexes();
		}

		m_bInspecting = false;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd(_T("Process Inspections"));
#endif
	}
	return l_Status;
}


void SVInspectionProcess::BuildWatchlist()
{
	m_WatchListDatas.clear();

	if (m_StoreIndex < 0 || GetPPQ() == 0)
	{
		return;
	}
	SetSlotmanager(SvSml::SharedMemWriter::Instance().GetSlotManager(GetPPQ()->GetName()));
	const SvSml::MonitorListCpy* pMonitorlist(nullptr);
	pMonitorlist = SvSml::SharedMemWriter::Instance().GetMonitorListCpyPointerForPPQ(GetPPQ()->GetName());
	if (!pMonitorlist)
	{
		return;
	}

	DWORD RejectConditionFlag = SvSml::ListFlags[SvSml::ListType::rejectCondition];
	for (auto& it : pMonitorlist->m_EntriesMap)
	{
		if (it.second->data.InspectionStoreId == m_StoreIndex)
		{
			SVObjectReference ObjectRef;
			if (S_OK == GetInspectionObject(it.first.c_str(), ObjectRef))
			{
				if (it.second->data.ObjectType != SvPb::SVImageObjectType && (it.second->data.m_MonitorListFlag & RejectConditionFlag))
				{
					if (false == it.second->data.wholeArray)
					{
						m_WatchListDatas.push_back(WatchlistelementPtr(new WatchListElement(ObjectRef, it.second)));
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

bool SVInspectionProcess::isReject()
{
	bool result(false);
	for (auto & element : m_WatchListDatas)
	{
		if (element->MonEntryPtr.get())
		{
				int arrayindex(-1);
				if (TRUE == element->MonEntryPtr->data.isArray)
				{
					arrayindex = element->MonEntryPtr->data.arrayIndex;
				}

			SvOi::IObjectClass* pObj = dynamic_cast<SvOi::IObjectClass*>(element->ObjRef.getValueObject());
			if (pObj)
				{
					double val(0);
				pObj->getValue(val, arrayindex);
					if (val != 0)
					{
					result = true;
					}
				}
			}
			}
	return result;
}



HRESULT SVInspectionProcess::ProcessNotifyWithLastInspected(bool& p_rProcessed, SVProductInfoStruct& p_rProduct)
{
	HRESULT l_Status = S_OK;

	if (0 < m_NotifyWithLastInspected)
	{
#ifdef EnableTracking
		m_InspectionTracking.EventStart(_T("Process Notify With Last Inspected"));
#endif
		::InterlockedExchange(&m_NotifyWithLastInspected, 0);


		if (GetPPQ()->HasActiveMonitorList() && p_rProduct.m_triggered)
		{
			p_rProduct.m_svInspectionInfos[GetUniqueObjectID()].m_bReject = isReject();
		}
		std::pair<SVInspectionInfoStruct, long> data {p_rProduct.m_svInspectionInfos[GetUniqueObjectID()], p_rProduct.ProcessCount()};
		SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cInspectionProcessTag), GetUniqueObjectID(), data);

		p_rProcessed = true;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd(_T("Process Notify With Last Inspected"));
#endif
	}
	return l_Status;
}

HRESULT SVInspectionProcess::ProcessCommandQueue(bool& p_rProcessed)
{
	HRESULT l_Status = S_OK;

	if (!(m_CommandQueue.IsEmpty()))
	{
#ifdef EnableTracking
		m_InspectionTracking.EventStart(_T("Process Command Queue"));
#endif
		SvOi::ICommandPtr pCommand;

		if (m_CommandQueue.RemoveHead(&pCommand))
		{
			if (nullptr != pCommand)
			{
				l_Status = pCommand->Execute();
			}
		}

		p_rProcessed = true;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd(_T("Process Command Queue"));
#endif
	}

	return l_Status;
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVInspectionObjectType;
	m_LastRunProductNULL = false;
	m_pCurrentToolset = nullptr;
	m_PPQId.clear();
	m_bNewDisableMethod = false;
	m_lInputRequestMarkerCount = 0L;
	m_bInspecting = false;
	m_dwThreadId = 0;
	m_svReset.RemoveState(SvDef::SVResetStateAll);
	m_bForceOffsetUpdate = true;
	m_publishList.m_pInspection = this;
	m_pToolSetConditional = nullptr;
	m_StoreIndex = -1;
}

SVInspectionProcess::~SVInspectionProcess()
{
	DestroyInspection();
	m_PPQId.clear();
	m_WatchListDatas.clear();
	m_SlotManager.reset();
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
	// Create Queues for Inspection Queue
	if (!m_qInspectionsQueue.Create())
	{
		return false;
	}
	if (!m_CommandQueue.Create())
	{
		return false;
	}

	::InterlockedExchange(&m_NotifyWithLastInspected, 0);

	if (S_OK != m_AsyncProcedure.Create(&SVInspectionProcess::APCThreadProcess, boost::bind(&SVInspectionProcess::ThreadProcess, this, _1), GetName(), SVThreadAttribute::SVAffinityUser))
	{
		return false;
	}

	// Create Queues for input/output requests
	if (!m_InputRequests.Create())
	{
		return false;
	}
	if (!m_InputImageRequests.Create())
	{
		return false;
	}

	m_pCurrentToolset = new SVToolSetClass(this);

	if (!CreateChildObject(m_pCurrentToolset))
	{
		return false;
	}

	return true;
}// end Create

void CALLBACK SVInspectionProcess::APCThreadProcess(DWORD_PTR dwParam)
{
}

void SVInspectionProcess::ThreadProcess(bool& p_WaitForEvents)
{
	bool l_Processed = false;
#ifdef EnableTracking
	m_InspectionTracking.SetStartTime();
#endif
	SVProductInfoStruct l_Product;

	ProcessInspection(l_Processed, l_Product);
	ProcessNotifyWithLastInspected(l_Processed, l_Product);

	ProcessCommandQueue(l_Processed);

	if (!(l_Product.empty()) && !(m_PPQId.empty()))
	{
		std::pair<SVInspectionInfoStruct, long> data {l_Product.m_svInspectionInfos[GetUniqueObjectID()], l_Product.ProcessCount()};
		SVObjectManagerClass::Instance().UpdateObserver(m_PPQId, data);
	}

	p_WaitForEvents = !l_Processed;
}

void SVInspectionProcess::DestroyInspection()
{
	SVIOEntryStruct pIOEntry;
	SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cInspectionProcessTag), GetUniqueObjectID(), SVRemoveSubjectStruct());
	::InterlockedExchange(&m_NotifyWithLastInspected, 0);
	::Sleep(0);
	m_AsyncProcedure.Destroy();
	resetLastProcduct();
	if (!(m_PPQId.empty()))
	{
		// Release all published outputs before we destroy the toolset
		m_publishList.Release(m_pCurrentToolset);
	}
	if (nullptr != m_pCurrentToolset)
	{
		DestroyChildObject(m_pCurrentToolset);
	}
	// Destroy Queues for input/output requests
	m_InputRequests.Destroy();
	m_InputImageRequests.Destroy();
	m_CommandQueue.Destroy();
	SVObjectManagerClass::Instance().AdjustInspectionIndicator(-(m_qInspectionsQueue.GetCount()));
	m_qInspectionsQueue.Destroy();
	m_PPQInputs.clear();
	m_PPQId.clear();
	SVResultListClass* pResultlist = GetResultList();
	if (pResultlist)
	{
		pResultlist->RebuildReferenceVector(this);
	}
}// end Destroy

HRESULT SVInspectionProcess::GetInspectionValueObject(LPCTSTR Name, SVObjectReference& rObjectRef)
{
	SVObjectNameInfo NameInfo;
	SVObjectClass* pObject(nullptr);

	HRESULT hr = NameInfo.ParseObjectName(Name);

	if (S_OK == hr)
	{
		//If Inspections prefix is present remove it
		if (std::string(SvDef::FqnInspections) == NameInfo.m_NameArray[0])
		{
			NameInfo.RemoveTopName();
		}

		SVValueObjectMap::const_iterator iter = m_mapValueObjects.find(NameInfo.GetObjectName());

		if (m_mapValueObjects.end() != iter)
		{
			pObject = iter->second;
		}
	}

	if (nullptr != pObject)
	{
		rObjectRef = SVObjectReference(pObject, NameInfo);
		hr = S_OK;
	}
	else
	{
		hr = S_FALSE;
	}


	return hr;
}

HRESULT SVInspectionProcess::GetInspectionObject(LPCTSTR Name, SVObjectReference& rObjectRef)
{
	HRESULT hr = GetInspectionValueObject(Name, rObjectRef);	// try the most efficient one first
	if (S_OK == hr)
	{
		return S_OK;
	}
	else	// if not a value object, try getting images
	{
		SvIe::SVImageClass* pImage = nullptr;
		hr = GetInspectionImage(Name, pImage);
		if (S_OK == hr)
		{
			rObjectRef = SVObjectReference(pImage);
		}
	}
	return hr;
}

bool SVInspectionProcess::CanGoOnline()
{
	bool l_bOk(true);

	CWaitCursor l_cwcMouse;

	SetResetCounts();
	m_svReset.AddState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	l_bOk = (S_OK == InitializeRunOnce());

	m_svReset.RemoveState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	ClearResetCounts();

	m_StoreIndex = SvSml::SharedMemWriter::Instance().GetInspectionStoreId(GetName());

	if (0 <= m_StoreIndex)
	{
		if (0 > m_trcPos)
		{
			m_trcPos = SvTrc::getInspectionPos(GetUniqueObjectID());
		}
		SvSml::SharedMemWriter::Instance().addInspectionIdEntry(GetPPQ()->GetName(), m_StoreIndex, m_trcPos);
		const SvPb::DataDefinitionList& rDataDefList = SvTrc::getTriggerRecordControllerRInstance().getDataDefList(m_trcPos);
		const SvPb::ImageList& rImageDefList = SvTrc::getTriggerRecordControllerRInstance().getImageDefList(m_trcPos);
		SvSml::SharedMemWriter::Instance().setDataTrcPos(GetPPQ()->GetName(), m_StoreIndex, m_trcPos, rDataDefList, rImageDefList);
	}

	return l_bOk;
}// end CanGoOnline

bool SVInspectionProcess::CanRegressionGoOnline()
{
	bool l_bOk(true);

	CWaitCursor l_cwcMouse;

	SetResetCounts();
	m_svReset.AddState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	l_bOk = resetAllObjects();

	m_svReset.RemoveState(SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

	ClearResetCounts();

	return l_bOk;
}

bool SVInspectionProcess::GoOnline()
{
#ifdef EnableTracking
	m_InspectionTracking.clear();
#endif

	resetLastProcduct();

	m_AsyncProcedure.SetPriority(THREAD_PRIORITY_NORMAL);

	BuildValueObjectMap();

	try
	{
		BuildWatchlist();
	}

	catch (const SvStl::MessageContainer& rSvE)
	{
		//Now that we have caught the exception we would like to display it
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(rSvE.getMessage());
		return false;
	}
	catch (std::exception& e)
	{

		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, e.what(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16223);
		return false;
	}

	catch (...)
	{
		SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, _T("Unknown error Handler"), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16223);
		return false;
	}
	return true;
}

bool SVInspectionProcess::GoOffline()
{
#ifdef EnableTracking
	if (TheSVObserverApp.UpdateAndGetLogDataManager())
	{
		std::string l_FileName;

		l_FileName.Format(_T("C:\\SVObserver\\ProductLastIndexes_%ld-%s.log"),
			SVObjectManagerClass::Instance().GetFileSequenceNumber(), GetName());

		std::fstream l_Stream(l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out);

		if (l_Stream.is_open())
		{
			std::string l_Info;
			SVProductInfoStruct l_Product = LastProductGet();

			l_Product.DumpIndexInfo(l_Info);

			l_Stream << _T("Last : ");
			l_Stream << l_Info.ToString();
			l_Stream << std::endl;

			l_Stream.close();
		}

		std::string l_Name;

		l_Name.Format(_T("C:\\SVObserver\\%s-Counts-%ld.csv"), GetName(), SVObjectManagerClass::Instance().GetFileSequenceNumber());

		std::fstream l_TrackingStream(l_Name.ToString(), std::ios_base::trunc | std::ios_base::out);

		if (l_TrackingStream.is_open())
		{
			SVInspectionTracking::SVEventTrackingMap::iterator l_Iter;

			for (l_Iter = m_InspectionTracking.m_EventCounts.begin(); l_Iter != m_InspectionTracking.m_EventCounts.end(); ++l_Iter)
			{
				// Write Outputs Time...
				l_TrackingStream << _T("Name/Time ms");

				SVInspectionTrackingElement::SVTimeCountMap::iterator l_CountIter;

				for (l_CountIter = l_Iter->second.m_Start.begin(); l_CountIter != l_Iter->second.m_Start.end(); ++l_CountIter)
				{
					l_TrackingStream << _T(",") << l_CountIter->first;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << l_Iter->first.c_str();
				l_TrackingStream << _T(" - Start");

				for (l_CountIter = l_Iter->second.m_Start.begin(); l_CountIter != l_Iter->second.m_Start.end(); ++l_CountIter)
				{
					l_TrackingStream << _T(",") << l_CountIter->second;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << _T("Name/Time ms");

				for (l_CountIter = l_Iter->second.m_Duration.begin(); l_CountIter != l_Iter->second.m_Duration.end(); ++l_CountIter)
				{
					l_TrackingStream << _T(",") << l_CountIter->first;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << l_Iter->first.c_str();
				l_TrackingStream << _T(" - Duration");

				for (l_CountIter = l_Iter->second.m_Duration.begin(); l_CountIter != l_Iter->second.m_Duration.end(); ++l_CountIter)
				{
					l_TrackingStream << _T(",") << l_CountIter->second;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << std::endl;
			}

			l_TrackingStream.close();
		}
	}
#endif

	SVObjectManagerClass::Instance().AdjustInspectionIndicator(-(m_qInspectionsQueue.GetCount()));

	// wait for inspection queue to empty
	m_qInspectionsQueue.clear();

	while (m_bInspecting)
	{
		::Sleep(10);
	}

	m_AsyncProcedure.SetPriority(THREAD_PRIORITY_NORMAL);

	//save the last image to have it in the edit-mode
	if (0 < m_pCurrentToolset->getTriggerCount())
	{
		for (SvIe::SVCameraImageTemplate* pCameraImage : m_CameraImages)
		{
			if (nullptr != pCameraImage)
			{
				SvIe::SVVirtualCamera* pCamera = pCameraImage->GetCamera();
				auto pImageData = pCameraImage->getImageData();
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

bool SVInspectionProcess::CanProcess(SVProductInfoStruct *pProduct)
{
	bool bReady {true};

	if (pProduct)
	{
		// See if we have discrete inputs.
		assert(m_PPQInputs.size() == pProduct->m_triggerInfo.m_Inputs.size());
		for (size_t i = 0; bReady && i < m_PPQInputs.size(); i++)
		{
			auto& rInputEntry = m_PPQInputs[i];
			const auto& rInputValue = pProduct->m_triggerInfo.m_Inputs.at(i);
			if(rInputEntry.m_EntryValid)
			{
				//Is input value valid
				bReady &= (VT_EMPTY != rInputValue.vt);
			}
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
					SvIe::SVGuidSVCameraInfoStructMap::iterator l_Iter;

					l_Iter = pProduct->m_svCameraInfos.find(pCamera->GetUniqueObjectID());

					if (l_Iter != pProduct->m_svCameraInfos.end())
					{
						bReady &= (nullptr != l_Iter->second.getImage());
					}
				}
			}

			++l_ImageIter;
		}
	} // if( pProduct )
	else
	{
		bReady = false;
	}

	return bReady;
}// end CanProcess

HRESULT SVInspectionProcess::StartProcess(SVProductInfoStruct *pProduct)
{
	HRESULT hr = S_OK;

	if(nullptr == pProduct)
	{
		return E_FAIL;
	}

	SVInspectionInfoStruct* pIPInfo = &(pProduct->m_svInspectionInfos[GetUniqueObjectID()]);
	bool isTriggerRecordValid = nullptr != pIPInfo && nullptr != pIPInfo->m_triggerRecordWrite;
	// Make sure that the lists are the same size
	if (!isTriggerRecordValid || m_PPQInputs.size() != pProduct->m_triggerInfo.m_Inputs.size())
	{
		return E_FAIL;
	}// end if

	if (0 == m_qInspectionsQueue.GetCount())
	{
		// Find the info struct for this inspection
		pIPInfo->m_CanProcess = false;
		pIPInfo->m_InProcess = true;

		SVProductInfoStruct tempProduct = moveInspectionToNewProduct(*pProduct, GetUniqueObjectID());
		SvIe::SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while (l_ImageIter != m_CameraImages.end())
		{
			SvIe::SVCameraImageTemplate* l_pImage = (*l_ImageIter);

			if (nullptr != l_pImage && nullptr != l_pImage->GetCamera())
			{
				SvIe::SVGuidSVCameraInfoStructMap::iterator l_Iter;

				l_Iter = pProduct->m_svCameraInfos.find(l_pImage->GetCamera()->GetUniqueObjectID());

				if (l_Iter != pProduct->m_svCameraInfos.end())
				{
					if (nullptr == l_Iter->second.getImage())
					{
						hr = SVMSG_SVO_71_INVALID_SOURCE_IMAGE_INDEX;
					}
				}
			}

			++l_ImageIter;
		}

		// Set the flag in the Temp product because it gets reset when copying from the
		// pProduct->m_svInspectionInfos to the Temp product. And that get put in the m_qInspectionsQueue.
		tempProduct.m_svInspectionInfos[GetUniqueObjectID()].m_HasBeenQueued = true;

		if (S_OK == hr && m_qInspectionsQueue.AddTail(tempProduct))
		{
			SVObjectManagerClass::Instance().IncrementInspectionIndicator();

			hr = m_AsyncProcedure.Signal(nullptr);
		}
	}
	else
	{
		hr = S_FALSE;
	}

	return hr;
}// end StartProcess

bool SVInspectionProcess::RebuildInspectionInputList()
{
	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr == pPPQ)
	{
		return false;
	}
	// Save old list
	SVIOEntryStructVector OldPPQInputs = m_PPQInputs;
	const SVIOEntryHostStructPtrVector& rUsedInputs = pPPQ->GetUsedInputs();

	long lListSize = static_cast<long>(rUsedInputs.size());

	// Make new list
	m_PPQInputs.resize(lListSize);

	for (int iList = 0; iList < lListSize; iList++)
	{
		const SVIOEntryHostStructPtr& rNewEntry = rUsedInputs[iList];
		bool bFound = false;
		SVIOEntryStruct IOEntry;

		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(rNewEntry->m_IOId);

		for (size_t iTotal = 0; iTotal < OldPPQInputs.size(); iTotal++)
		{
			IOEntry = OldPPQInputs[iTotal];

			if (0 == strcmp(IOEntry.m_IOEntryPtr->getObject()->GetName(), pObject->GetName()))
			{
				// We found it
				bFound = true;
				m_PPQInputs[iList] = IOEntry;
				IOEntry.m_IOEntryPtr->m_PPQIndex = rNewEntry->m_PPQIndex;
				IOEntry.m_IOEntryPtr->m_Enabled = rNewEntry->m_Enabled;
				IOEntry.m_IOEntryPtr->getObject()->ResetObject();

				break;
			}// end if
		}// end for

		if (!bFound)
		{
			SVObjectClass* pNewObject = nullptr;

			switch (rNewEntry->m_ObjectType)
			{
				case IO_DIGITAL_INPUT:
				{
					SvVol::SVBoolValueObjectClass* pIOObject = new SvVol::SVBoolValueObjectClass(this);
					pIOObject->setResetOptions(false, SvOi::SVResetItemNone);
					pNewObject = dynamic_cast<SVObjectClass*> (pIOObject);
					pNewObject->SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
					break;
				}
				case IO_REMOTE_INPUT:
				{
					SvVol::SVVariantValueObjectClass* pIOObject = new SvVol::SVVariantValueObjectClass(this);
					pIOObject->setResetOptions(false, SvOi::SVResetItemNone);
					variant_t defaultValue;
					defaultValue.ChangeType(VT_R8);
					pIOObject->SetDefaultValue(defaultValue, false);
					pNewObject = dynamic_cast<SVObjectClass*> (pIOObject);
					pNewObject->SetObjectAttributesAllowed(SvPb::remotelySetable, SvOi::SetAttributeType::AddAttribute);
					break;
				}
				default:
				{
					SvVol::SVVariantValueObjectClass* pIOObject = new SvVol::SVVariantValueObjectClass(this);
					pIOObject->setResetOptions(false, SvOi::SVResetItemNone);
					pNewObject = dynamic_cast<SVObjectClass*> (pIOObject);
					break;
				}
			}

			pNewObject->SetName(pObject->GetName());
			pNewObject->SetObjectOwner(this);
			pNewObject->SetObjectAttributesSet(pNewObject->ObjectAttributesSet() & SvPb::publishable, SvOi::SetAttributeType::OverwriteAttribute);
			pNewObject->ResetObject();

			CreateChildObject(pNewObject);

			IOEntry.m_IOEntryPtr = SVIOEntryHostStructPtr {new SVIOEntryHostStruct};
			IOEntry.m_IOEntryPtr->setObject(pNewObject);
			IOEntry.m_IOEntryPtr->m_ObjectType = rNewEntry->m_ObjectType;
			IOEntry.m_IOEntryPtr->m_IOId = rNewEntry->m_IOId;
			IOEntry.m_IOEntryPtr->m_Enabled = rNewEntry->m_Enabled;

			m_PPQInputs[iList] = IOEntry;
		}// end if

		SvOi::SetAttributeType AddRemoveType = m_PPQInputs[iList].m_IOEntryPtr->m_Enabled ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_PPQInputs[iList].m_IOEntryPtr->getObject()->SetObjectAttributesAllowed(SvPb::selectableForEquation | SvPb::viewable, AddRemoveType);

	}// end for

	SVResultListClass* pResultlist = GetResultList();
	if (pResultlist)
	{
		pResultlist->RebuildReferenceVector(this);
	}

	BuildValueObjectMap();
	return true;
}

bool SVInspectionProcess::AddInputRequest(const SVObjectReference& rObjectRef, const _variant_t& rValue)
{
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

	if (SVSVIMStateClass::CheckState(notAllowedStates))
	{
		return false;
	}

	if (!m_InputRequests.Lock())
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17013_ErrorLockingInputRequests);
		DebugBreak();
	}

	if (!m_InputRequests.AddTail(pInRequest))
	{
		if (!m_InputRequests.Unlock())
		{
			SvStl::MessageMgrStd e(SvStl::MsgType::Log);
			e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17014_ErrorUnlockingInputRequests);
			DebugBreak();
		}

		return false;
	}


	std::string l_StringValue;

	if (VT_BSTR == pInRequest->m_Value.vt)
	{
		l_StringValue = SvUl::createStdString(pInRequest->m_Value);
	}

	if (l_StringValue == SvO::SVTOOLPARAMETERLIST_MARKER)
	{
		::InterlockedIncrement(const_cast<long*>(&m_lInputRequestMarkerCount));
	}// end if

	if (!m_InputRequests.Unlock())
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17015_ErrorUnlockingInputRequests);
		DebugBreak();
	}

	return true;
}

bool SVInspectionProcess::AddInputRequestMarker()
{
	return AddInputRequest(nullptr, SvO::SVTOOLPARAMETERLIST_MARKER);
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
				SvIe::SVCameraImageTemplate* l_psvMainImage = dynamic_cast<SvIe::SVCameraImageTemplate*>(p_psvImage);

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

			SvIe::SVCameraImageTemplate* pMainImage = dynamic_cast<SvIe::SVCameraImageTemplate*> (pImage);

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

	if (SVSVIMStateClass::CheckState(notAllowedStates))
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
		l_Product.setInspectionTriggerRecordComplete(GetUniqueObjectID());
		LastProductUpdate(&l_Product);
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

				if (pParam && pParam->options.find(pParam->strValue)->second.m_bColor)
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

				SvStl::MessageMgrStd Msg(SvStl::MsgType::Log | SvStl::MsgType::Display);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_IPDoc_NoCameraColorAttached, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10052);
			}
		}
	}
	////////////////////////

	SVObjectLevelCreateStruct createStruct;

	if (shouldCreateAllObject || !IsCreated())
	{
		if (createAllObjects(createStruct))
		{
			l_Status = E_FAIL;
		}
	}

	SetDefaultInputs();

	if (CheckAndResetConditionalHistory())
	{
		//Configuration has changed need to set the modified flag
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);

		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_CONDITIONAL_HISTORY, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_IP)
	int iCount = static_cast<int>(m_mapValueObjects.size());
	std::string Text = SvUl::Format(_T("%s value object count=%d\n"), GetName(), iCount);
	::OutputDebugString(Text.c_str());
#endif

	return l_Status;
}

void SVInspectionProcess::ClearResetCounts()
{
	if (nullptr != m_pCurrentToolset)
	{
		m_pCurrentToolset->ClearResetCounts();
	}
}

void SVInspectionProcess::SetResetCounts()
{
	if (nullptr != m_pCurrentToolset)
	{
		m_pCurrentToolset->ResetCounts();
	}
}

void SVInspectionProcess::ValidateAndInitialize(bool p_Validate)
{
	// Call ToolSet Validate 
	// so the SVEquationClass can rebuild its symbol table
	if (p_Validate)
	{
		SetResetCounts();

		m_svReset.AddState(SvDef::SVResetAutoMoveAndResize | SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);

		SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemIP;

		ProcessInputRequests(eResetItem);

		m_svReset.RemoveState(SvDef::SVResetAutoMoveAndResize | SvDef::SVResetStateInitializeOnReset | SvDef::SVResetStateArchiveToolCreateFiles | SvDef::SVResetStateLoadFiles);
	}
}

void SVInspectionProcess::SingleRunModeLoop(bool p_Refresh)
{
	SVProductInfoStruct l_svProduct;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		l_svProduct = pPPQ->getProductReadyForRunOnce(GetUniqueObjectID());
	}

	LastProductCopySourceImagesTo(&l_svProduct);

	RunInspection(l_svProduct.m_svInspectionInfos[GetUniqueObjectID()], l_svProduct.m_svCameraInfos, l_svProduct.m_triggerInfo.lTriggerCount);
	l_svProduct.setInspectionTriggerRecordComplete(GetUniqueObjectID());

	LastProductUpdate(&l_svProduct);

	if (p_Refresh)
	{
		// Result View, Display Image and Overlay Update
		if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
		{
			std::pair<SVInspectionInfoStruct, long> data {l_svProduct.m_svInspectionInfos[GetUniqueObjectID()], l_svProduct.ProcessCount()};

			SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cInspectionProcessTag), GetUniqueObjectID(), data);
		}
	}
}

bool SVInspectionProcess::resetAllObjects(SvStl::MessageContainerVector *pErrorMessages/*=nullptr */)
{
	SvStl::MessageContainerVector ErrorMessages;
	bool Result = true;
	try
	{
		bool shouldResetTRC = !SvTrc::getTriggerRecordControllerRWInstance().isResetLocked();

		if (shouldResetTRC)
		{
			SvTrc::getTriggerRecordControllerRWInstance().startResetTriggerRecordStructure(m_trcPos);
		}

		for (size_t l = 0; l < m_PPQInputs.size(); l++)
		{
			if (!(m_PPQInputs[l].empty()) && nullptr != m_PPQInputs[l].m_IOEntryPtr->getObject())
			{
				m_PPQInputs[l].m_IOEntryPtr->getObject()->resetAllObjects(&ErrorMessages);
				break;
			}
		}// end for

		if (nullptr != m_pCurrentToolset)
		{
			Result = m_pCurrentToolset->resetAllObjects(&ErrorMessages) && Result;
		}
		else
		{
			Result = false;
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ToolsetNotCreated, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			ErrorMessages.push_back(Msg);
		}

		Result = __super::resetAllObjects(&ErrorMessages) && Result;

		if(!SVSVIMStateClass::CheckState(SV_STATE_RUNNING | SV_STATE_REGRESSION))
		{
			buildValueObjectDefList();
		}

		if (shouldResetTRC)
		{
			SvTrc::getTriggerRecordControllerRWInstance().finishResetTriggerRecordStructure();
		}
	}
	catch (const SvStl::MessageContainer& rExp)
	{
		if (SVMSG_TRC_GENERAL_ERROR == rExp.getMessage().m_MessageCode && SvStl::Tid_TRC_Error_CreateBuffer == rExp.getMessage().m_AdditionalTextId)
		{
			SvStl::MessageMgrStd oldException(SvStl::MsgType::Log);
			oldException.setMessage(rExp.getMessage());
			SvStl::MessageMgrStd newException(SvStl::MsgType::Display);
			newException.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TRC_Error_ResetInspectionFailed, SvStl::SourceFileParams(StdMessageParams));
			ErrorMessages.insert(ErrorMessages.begin(), newException.getMessageContainer());
		}
		else if (SVMSG_TRC_GENERAL_ERROR == rExp.getMessage().m_MessageCode && SvStl::Tid_TRC_Error_ResetBuffer_TooMany == rExp.getMessage().m_AdditionalTextId)
		{
			SvStl::MessageMgrStd oldException(SvStl::MsgType::Display | SvStl::MsgType::Log);
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
		SvStl::MessageMgrStd message(SvStl::MsgType::Log);
		message.setMessage(iter->getMessage());
	}
	if (nullptr != pErrorMessages)
	{
		*pErrorMessages = ErrorMessages;
	}
	return Result;
}

HRESULT SVInspectionProcess::FindPPQInputObjectByName(SVObjectClass*& p_rpObject, LPCTSTR p_FullName) const
{
	HRESULT l_Status = S_OK;

	for (size_t l = 0; nullptr == p_rpObject && l < m_PPQInputs.size(); ++l)
	{
		if (!(m_PPQInputs[l].empty()))
		{
			std::string l_LocalName = GetName();

			l_LocalName += _T(".");
			l_LocalName += m_PPQInputs[l].m_IOEntryPtr->getObject()->GetName();

			if (l_LocalName == p_FullName)
			{
				p_rpObject = m_PPQInputs[l].m_IOEntryPtr->getObject();
			}
		}
	}

	if (nullptr == p_rpObject)
	{
		l_Status = S_FALSE;
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

void SVInspectionProcess::SetPPQIdentifier(const SVGUID& p_rPPQId)
{
	if (!(m_PPQId.empty()) && (m_PPQId != p_rPPQId))
	{
		// Release all published outputs before we destroy the toolset
		m_publishList.Release(m_pCurrentToolset);
	}

	m_PPQId = p_rPPQId;
}

const SVGUID& SVInspectionProcess::GetPPQIdentifier() const
{
	return m_PPQId;
}

SVPPQObject* SVInspectionProcess::GetPPQ() const
{
	SVPPQObject* pPPQ(nullptr);

	if (!(m_PPQId.empty()))
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
		SVProductInfoStruct product = pPPQ->getProductReadyForRunOnce(GetUniqueObjectID());

		if (0 >= product.m_svInspectionInfos.size())
		{
			l_Status = E_FAIL;
		}

		if (!(RunInspection(product.m_svInspectionInfos[GetUniqueObjectID()], product.m_svCameraInfos, product.m_triggerInfo.lTriggerCount, false)) && S_OK == l_Status)
		{
			l_Status = E_FAIL;
		}
		product.setInspectionTriggerRecordComplete(GetUniqueObjectID());

		HRESULT l_Temp = LastProductUpdate(&product);

		if (S_OK == l_Status) { l_Status = l_Temp; }
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVInspectionProcess::ProcessInputRequests(bool &rForceOffsetUpdate)
{
	SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemNone;

	bool l_bOk = ProcessInputRequests(eResetItem);

	rForceOffsetUpdate |= (eResetItem < SvOi::SVResetItemNone);

	return l_bOk;
}

bool SVInspectionProcess::ProcessInputRequests(SvOi::SVResetItemEnum &rResetItem)
{
	bool bRet = true;
	long l;

	SVInputRequestInfoStructPtr l_pInputRequest;

	// Process all input requests
	if (!m_InputRequests.Lock())
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17025_ErrorLockingInputRequests);
		DebugBreak();
	}

	SVStdMapSVToolClassPtrSVInspectionProcessResetStruct toolMap;
	while (m_lInputRequestMarkerCount > 0L)
	{
		long l_lSize = 0;

		// Only loop through the InputRequests that are here now
		// Preprocessing phase
		m_InputRequests.GetSize(l_lSize);

		for (l = 0; l < l_lSize; l++)
		{
			if (!m_InputRequests.RemoveHead(&l_pInputRequest))
			{
				bRet = false;

				break;
			}// end if

			std::string Value;

			if (VT_ARRAY == (l_pInputRequest->m_Value.vt & VT_ARRAY))
			{
				SvUl::SVSAFEARRAY::SVBounds l_Bounds;
				SvUl::SVSAFEARRAY l_SafeArray = l_pInputRequest->m_Value;

				l_SafeArray.GetBounds(l_Bounds);

				if (1 == l_SafeArray.size())
				{
					_variant_t l_Variant;

					l_SafeArray.GetElement(l_Bounds[0].lLbound, l_Variant);

					Value = SvUl::createStdString(l_Variant);
				}
				else if (1 < l_SafeArray.size())
				{
					for (size_t i = 0; i < l_Bounds[0].cElements; ++i)
					{
						_variant_t l_Variant;

						l_SafeArray.GetElement((l_Bounds[0].lLbound + i), l_Variant);

						if (i > 0)
						{
							Value += _T(",");
						}
						Value += SvUl::Format(_T("`%s`"), SvUl::createStdString(l_Variant).c_str());
					}
				}
			}
			else
			{
				Value = SvUl::createStdString(l_pInputRequest->m_Value);
			}

			// New delimiter added after each SVSetToolParameterList call
			// This breaks the list into pieces and we are only processing
			// 1 piece of the list per inspection iteration
			if (Value == SvO::SVTOOLPARAMETERLIST_MARKER)
			{
				::InterlockedDecrement(const_cast<long*>(&m_lInputRequestMarkerCount));
				break;
			}// end if

			// Get the ValueObject that they are trying to set
			SVObjectReference ObjectRef = l_pInputRequest->m_ValueObjectRef;
			HRESULT hrSet = S_OK;
			if (nullptr != ObjectRef.getValueObject())
			{
				bool bResetObject = ObjectRef.getValueObject()->ResetAlways();

				// Value objects don't accept each different type for sets
				// so convert to the appropriate type
				if (SvVol::SVFileNameValueObjectClass* pFileNameObj = dynamic_cast<SvVol::SVFileNameValueObjectClass*> (ObjectRef.getObject()))
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

								hrSet = pFileNameObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

								bResetObject = (PrevValue != Value);
							}

							hrSet = pFileNameObj->SetValue(Value, ObjectRef.ArrayIndex());
						}// end if
					}
				}
				else if (SvVol::SVStringValueObjectClass* pStringValueObj = dynamic_cast<SvVol::SVStringValueObjectClass*> (ObjectRef.getObject()))
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

							hrSet = pStringValueObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

							bResetObject = (PrevValue != Value);
						}

						hrSet = pStringValueObj->SetValue(Value, ObjectRef.ArrayIndex());
					}
				}
				else if (SvVol::SVBoolValueObjectClass* pBoolValueObj = dynamic_cast<SvVol::SVBoolValueObjectClass*> (ObjectRef.getObject()))
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

						hrSet = pBoolValueObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

						// eventually use CompareWithCurrentValueImpl
						bResetObject = NewValue != PrevValue;
					}

					hrSet = pBoolValueObj->SetValue(NewValue, ObjectRef.ArrayIndex());
				}// end else if SVBoolValueObjectClass
				else if (SvVol::SVVariantValueObjectClass* pvValueObject = dynamic_cast <SvVol::SVVariantValueObjectClass*> (ObjectRef.getObject()))
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
				else if (SvVol::SVEnumerateValueObjectClass* pEnumerateValueObj = dynamic_cast <SvVol::SVEnumerateValueObjectClass*> (ObjectRef.getObject()))
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
						if (Value.size() > 0)
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
				else if (SvVol::SVDoubleValueObjectClass* pDoubleValueObj = dynamic_cast <SvVol::SVDoubleValueObjectClass*> (ObjectRef.getObject()))
				{
					if (ObjectRef.isArray() && ObjectRef.isEntireArray())
					{
						hrSet = SetObjectArrayValues<double>(ObjectRef, Value.c_str(), bResetObject);
					}
					else
					{
						double NewValue = atof(Value.c_str());
						if (!bResetObject)
						{
							double PrevValue;

							hrSet = pDoubleValueObj->GetValue(PrevValue, ObjectRef.ArrayIndex());

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

						hrSet = ObjectRef.getValueObject()->getValue(PrevValue, ObjectRef.ArrayIndex());

						std::string strNewValue(Value);
						SvUl::MakeLower(strNewValue);
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
					SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*> (ObjectRef.getObject()->GetAncestor(SvPb::SVToolObjectType));

					if (nullptr != pTool)
					{
						SvOi::SVResetItemEnum eToolResetItem = toolMap[pTool].SetResetData(ObjectRef.getValueObject()->getResetItem(), ObjectRef.getObject());

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
						toolMap.clear();
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
				bRet &= S_OK == m_pCurrentToolset->ClearResetCounts();
			}
			else
			{
				int l_iSize = m_pCurrentToolset->GetSize();

				for (l = 0; l < l_iSize; l++)
				{
					SvTo::SVToolClass *pTool = dynamic_cast<SvTo::SVToolClass*> (m_pCurrentToolset->GetAt(l));

					if (nullptr != pTool)
					{
						if (toolMap.find(pTool) != toolMap.end())
						{
							if (toolMap[pTool].m_ObjectSet.empty())
							{
								bRet &= pTool->resetAllObjects();
							}
							else
							{
								SVObjectPtrSet::iterator l_oIter;

								l_oIter = toolMap[pTool].m_ObjectSet.begin();

								while (l_oIter != toolMap[pTool].m_ObjectSet.end())
								{
									SVObjectClass *l_psvObject = *l_oIter;

									if (nullptr != l_psvObject)
									{
										bRet &= l_psvObject->resetAllObjects();
									}
									else
									{
										bRet = false;
									}

									++l_oIter;
								}
							}
						}
					}
				}
			}// end if( l_bResetIPDoc ) else

			if (rResetItem < SvOi::SVResetItemNone)
			{
				SVRemoveValues l_Data;

				SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
			}
		}// end if ( nullptr != m_pCurrentToolset )
	}// end while( m_lInputRequestMarkerCount > 0L )

	if (!m_InputRequests.Unlock())
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17026_ErrorUnlockingInputRequests);
		DebugBreak();
	}

	return bRet;
}

bool SVInspectionProcess::ProcessInputImageRequests(SVInspectionInfoStruct& rIpInfoStruct, SvIe::SVGuidSVCameraInfoStructMap& rCameraInfos)
{
	bool bRet = true;

	SVInputImageRequestInfoStructPtr l_pInRequest;
	while (m_InputImageRequests.RemoveHead(&l_pInRequest))
	{
		SvIe::SVImageClass *pImage = nullptr;

		if (l_pInRequest->m_bUsingCameraName) //not using camera name...
		{
			SvIe::SVCameraImagePtrSet l_MainImages;

			GetMainImages(l_pInRequest->m_ObjectName.c_str(), l_MainImages);

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
			SvTrc::IImagePtr pImageBuffer;
			SvIe::SVVirtualCamera* pVirtualCamera = nullptr;
			if (SvIe::SVMainImageClass* l_psvMainImage = dynamic_cast<SvIe::SVMainImageClass*> (pImage))
			{
				SvIe::SVGuidSVCameraInfoStructMap::iterator l_svIter = rCameraInfos.find(l_psvMainImage->GetCamera()->GetUniqueObjectID());
				if (l_svIter != rCameraInfos.end())
				{
					pImageBuffer = l_psvMainImage->GetTempImageBuffer();
					l_svIter->second.setImage(pImageBuffer);
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
				if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING) && nullptr != pVirtualCamera)
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

bool SVInspectionProcess::IsNewDisableMethodSet()
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

HRESULT SVInspectionProcess::CollectOverlays(SvIe::SVImageClass* pImage, SVExtentMultiLineStructVector& rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	rMultiLineArray.clear();

	SVToolSetClass *l_pToolSet = GetToolSet();

	if (nullptr != l_pToolSet)
	{
		l_Status = l_pToolSet->CollectOverlays(pImage, rMultiLineArray);
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
		SvIe::SVImageProcessingClass::LoadImageBuffer(rFileName.c_str(), l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr);

		AddInputImageRequest(l_pInRequest);
	}

	return hrOk;
}

std::pair<SvTi::SVTriggerInfoStruct, SVInspectionInfoStruct> SVInspectionProcess::getLastProductData() const
{
	if (!m_LastRunProductNULL)
	{
		std::lock_guard<std::mutex>  Autolock(m_LastRunMutex);
		auto ipInfoIter = m_svLastRunProduct.m_svInspectionInfos.find(GetUniqueObjectID());
		if (m_svLastRunProduct.m_svInspectionInfos.end() != ipInfoIter)
		{
			return {m_svLastRunProduct.m_triggerInfo, ipInfoIter->second};
		}
	}
	return {};
}

void SVInspectionProcess::resetLastProcduct()
{
	m_LastRunProductNULL = true;
	m_svLastRunProduct.Assign({});
}

HRESULT SVInspectionProcess::LastProductCopySourceImagesTo(SVProductInfoStruct *p_psvProduct)
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
				const SvIe::SVGuidSVCameraInfoStructMap::iterator LastIter(l_Product.m_svCameraInfos.find(rCameraInfoPair.first));

				if (LastIter != l_Product.m_svCameraInfos.end() && nullptr != LastIter->second.getImage() && !LastIter->second.getImage()->isEmpty())
				{
					Copied = rCameraInfoPair.second.setImage(LastIter->second.getImage());
				}
				else
				{
					auto pImage = rCameraInfoPair.second.GetNextImage();
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

HRESULT SVInspectionProcess::LastProductNotify()
{
	HRESULT l_hrOk = S_OK;

	::InterlockedIncrement(&m_NotifyWithLastInspected);

	m_AsyncProcedure.Signal(nullptr);

	return l_hrOk;
}

bool SVInspectionProcess::CheckAndResetConditionalHistory()
{
	//Conditional History has been deprecated, these are used to check and reset the attributes of previously saved configurations
	constexpr UINT SV_CH_CONDITIONAL = 0x00002000;
	constexpr UINT SV_CH_IMAGE = 0x00004000;
	constexpr UINT SV_CH_VALUE = 0x00008000;

	bool Result(false);

	SvIe::SVTaskObjectListClass* pToolSet = static_cast <SvIe::SVTaskObjectListClass*> (m_pCurrentToolset);
	if (nullptr != pToolSet)
	{
		SVObjectReferenceVector vecObjects;
		pToolSet->GetOutputListFiltered(vecObjects, SV_CH_CONDITIONAL | SV_CH_VALUE, false);
		if (0 < vecObjects.size())
		{
			SVObjectReferenceVector::iterator iter;
			for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
			{
				iter->SetObjectAttributesSet(SV_CH_CONDITIONAL | SV_CH_VALUE, SvOi::SetAttributeType::RemoveAttribute);
			}
			Result = true;
		}

		SvIe::SVImageClassPtrVector listImages;
		pToolSet->GetImageList(listImages, SV_CH_IMAGE);
		int NumberOfImages = static_cast<int> (listImages.size());
		if (0 < NumberOfImages)
		{
			for (int i = 0; i < NumberOfImages; i++)
			{
				SVObjectReference refImage(listImages[i]);
				refImage.getObject()->SetObjectAttributesSet(SV_CH_IMAGE, SvOi::SetAttributeType::RemoveAttribute);
			}
			Result = true;
		}

	}
	return Result;
}

bool SVInspectionProcess::IsColorCamera() const
{
	SvIe::SVVirtualCamera* pCamera = dynamic_cast<SvIe::SVVirtualCamera*> (SvOi::getObject(getFirstCamera()));

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

		l_Data.m_Images.insert(pImage->GetUniqueObjectID());

		SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVToolSetClass* SVInspectionProcess::GetToolSet() const
{
	return m_pCurrentToolset;
}

bool SVInspectionProcess::CreateObject(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool l_bOk = SVObjectClass::CreateObject(rCreateStructure);

	SVObjectLevelCreateStruct createStruct;

	createStruct.OwnerObjectInfo.SetObject(this);
	createStruct.m_pInspection = this;

	l_bOk = l_bOk && m_pCurrentToolset->createAllObjects(createStruct);

	m_RegressionTestPlayEquation.ConnectObject(createStruct);

	m_isCreated = l_bOk;

	return m_isCreated;
}

bool SVInspectionProcess::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (!m_pCurrentToolset->IsCreated())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ToolsetNotCreated, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	m_pToolSetCamera = dynamic_cast<SvIe::SVVirtualCamera*> (SvOi::getObject(getFirstCamera()));

	BuildValueObjectMap();

	m_bForceOffsetUpdate = true;

	return Result;
}

void SVInspectionProcess::UpdateMainImagesByProduct(SVInspectionInfoStruct& rIpInfoStruct, SvIe::SVGuidSVCameraInfoStructMap& rCameraInfos)
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
				SvIe::SVGuidSVCameraInfoStructMap::const_iterator Iter;

				Iter = rCameraInfos.find(pCamera->GetUniqueObjectID());

				if (Iter != rCameraInfos.end())
				{
					if (nullptr == Iter->second.getImage())
					{
						SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
						Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_InvalidImageBuffer, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
						Exception.Throw();
					}
					else
					{
						try
						{
							l_pImage->setImage(Iter->second.getImage(), rIpInfoStruct.m_triggerRecordWrite);
							if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING) && SVSVIMStateClass::CheckState(SV_STATE_READY) && nullptr != pCamera && Iter->second.getImage()->isValid())
							{
								pCamera->setTempImage(Iter->second.getImage()->getHandle()->GetBuffer());
							}
						}
						catch (const SvStl::MessageContainer& rExp)
						{
							//This is the topmost catch for MessageContainer exceptions
							SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
							SvStl::MessageData Msg(rExp.getMessage());
							Msg.m_MessageCode = SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED;
							Exception.setMessage(Msg, GetUniqueObjectID());
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

					SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
					Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_CameraNotFound, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
					Exception.Throw();
				}
			}
			else
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
				Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_InvalidCamera, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
				Exception.Throw();
			}
		}
		else
		{
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Data);
			Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_UpdateMainImages_InvalidImage, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			Exception.Throw();
		}

		++l_ImageIter;
	}
}

HRESULT SVInspectionProcess::copyValues2TriggerRecord(SVRunStatusClass& rRunStatus) const
{
	if(nullptr != rRunStatus.m_triggerRecord)
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("copyValues2TriggerRecord; %d\n"), rRunStatus.m_triggerRecord->getId());
		::OutputDebugString(DebugString.c_str());
#endif
		std::vector<_variant_t> valueList{copyValueObjectList()};
		rRunStatus.m_triggerRecord->writeValueData(std::move(valueList));
		return S_OK;
	}
	else
	{
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = SvUl::Format(_T("copyValues2TriggerRecord failed, no TR\n"), rRunStatus.m_triggerRecord->getId());
		::OutputDebugString(DebugString.c_str());
#endif
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_TRC_Error_CopyValueObjData, SvStl::SourceFileParams(StdMessageParams));
	}
	return E_FAIL;
}

bool SVInspectionProcess::Run(SVRunStatusClass& rRunStatus)
{
	// Validate IPDoc's values...
	bool retVal = IsCreated();

	if (retVal)
	{

		// Run the Toolset
		unsigned long l_state = rRunStatus.GetState();
		if (GetToolSet() && l_state != SVRunStatusClass::SV_INVALID)
		{
			retVal &= GetToolSet()->Run(rRunStatus);
		}
		else
		{
			rRunStatus.SetInvalid();
			rRunStatus.SetInspectionStarted(false);
			if (SVRunStatusClass::SV_INVALID == rRunStatus.GetState())
			{
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_SVO_37_INVALID_RUN_STATUS, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		copyValues2TriggerRecord(rRunStatus);
	}
	else
	{
		rRunStatus.SetInvalid();
		rRunStatus.SetInspectionStarted(false);
#if defined (TRACE_THEM_ALL) || defined (TRACE_TRC)
		std::string DebugString = _T("Inspection not valid\n");
		::OutputDebugString(DebugString.c_str());
#endif
	}

	return retVal;
}

bool SVInspectionProcess::RunInspection(SVInspectionInfoStruct& rIPInfo, SvIe::SVGuidSVCameraInfoStructMap& rCameraInfos, long triggerCount, bool p_UpdateCounts)
{
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
	double del = SvTl::setReferenceTime();
	std::string DebugString = SvUl::.Format(_T("!\n!!Reset, %7.1lf: SVInspectionProcess::RunInspection(), del = %7.1lf\n"), SvTl::GetRelTimeStamp(), del);
	::OutputDebugString(DebugString.c_str());
#endif

	bool l_bOk = false;
	bool l_bInputRequest = false;
	bool l_bImageRequest = false;
	bool l_bUpdateMainImage = false;
	bool l_bRestMainImage = false;
	SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);

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
			rIPInfo.setNextTriggerRecord(SvTrc::TriggerData{triggerCount});
			if (nullptr != rIPInfo.m_triggerRecordWrite)
			{
				rIPInfo.m_triggerRecordWrite->initImages();
				Exception.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_RunInspection_ResetTR, SvStl::SourceFileParams(StdMessageParams));
			}
			else
			{
				l_bImageRequest = true;
				Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_RunInspection_ResetTR, SvStl::SourceFileParams(StdMessageParams));
			}
		}

		try
		{
			UpdateMainImagesByProduct(rIPInfo, rCameraInfos);
		}
		catch (const SvStl::MessageContainer& rSvE)
		{
			SvStl::MessageMgrStd Msg(SvStl::MsgType::Log);
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

	rIPInfo.m_BeginToolset = SvTl::GetTimeStamp();
	if (!rIPInfo.m_BeginToolset)
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17027_ErrorGettingTimeStamp);
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

	rIPInfo.m_EndToolset = SvTl::GetTimeStamp();
	if (!rIPInfo.m_EndToolset)
	{
		SvStl::MessageMgrStd e(SvStl::MsgType::Log);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17028_ErrorGettingTimeStamp);
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
		m_pCurrentToolset->GetAllInputObjects();
	}// end if

	ConnectAllInputs();

	// Setup Connections
	if (m_pCurrentToolset)
	{
		m_pCurrentToolset->SetDefaultInputs();
	}// end if

	BuildValueObjectMap();

	// Update published List
	GetPublishList().Refresh(m_pCurrentToolset);
}

SVPublishListClass& SVInspectionProcess::GetPublishList()
{
	return m_publishList;
}

SVInspectionProcess::SVObjectPtrDeque SVInspectionProcess::GetPreProcessObjects() const
{
	SVObjectPtrDeque l_Objects = SVObjectClass::GetPreProcessObjects();

	return l_Objects;
}

SVInspectionProcess::SVObjectPtrDeque SVInspectionProcess::GetPostProcessObjects() const
{
	SVObjectPtrDeque l_Objects = SVObjectClass::GetPostProcessObjects();

	SVIOEntryStructVector::const_iterator l_Iter;

	for (l_Iter = m_PPQInputs.begin(); l_Iter != m_PPQInputs.end(); ++l_Iter)
	{
		const SVIOEntryStruct& l_rIOEntry = *l_Iter;

		if (nullptr != l_rIOEntry.m_IOEntryPtr)
		{
			if (nullptr != l_rIOEntry.m_IOEntryPtr->getObject())
			{
				l_Objects.push_back(l_rIOEntry.m_IOEntryPtr->getObject());
			}
		}
	}

	if (nullptr != GetToolSet())
	{
		l_Objects.push_back(GetToolSet());
	}

	return l_Objects;
}

SVObjectClass *SVInspectionProcess::UpdateObject(const GUID &p_oFriendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner)
{
	p_psvObject->SetObjectOwner(p_psvNewOwner);

	return p_psvObject; //l_psvObject;
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

	SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
}

void SVInspectionProcess::SetName(LPCTSTR StrString)
{
	SVObjectClass::SetName(StrString);

	SVInspectionNameUpdate l_Data(GetName());

	SVObjectManagerClass::Instance().UpdateObservers(std::string(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
}

HRESULT SVInspectionProcess::RegisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	//! Object is an Image
	if (SvPb::SVImageObjectType == pObject->GetObjectInfo().m_ObjectTypeInfo.ObjectType)
	{
		SvIe::SVCameraImageTemplate* pCameraImage = dynamic_cast<SvIe::SVCameraImageTemplate*> (pObject);

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
		m_mapValueObjects.insert({pObject->GetCompleteName(), pObject});
		Result = S_OK;
	}

	return Result;
}

HRESULT SVInspectionProcess::UnregisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	//! Object is an Image
	if (SvPb::SVImageObjectType == pObject->GetObjectInfo().m_ObjectTypeInfo.ObjectType)
	{
		SvIe::SVCameraImageTemplate* pCameraImage = dynamic_cast<SvIe::SVCameraImageTemplate*>(pObject);

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
		if (!(m_ValueObjectSet.empty()))
		{
			m_ValueObjectSet.erase(pValueObject);
		}
		SVValueObjectMap::iterator iter =  m_mapValueObjects.find(pObject->GetCompleteName());
		if(m_mapValueObjects.end() != iter)
		{
			m_mapValueObjects.erase(iter);
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

	SVToolSetClass* l_pToolSet = GetToolSet();

	if (nullptr != l_pToolSet)
	{
		pImage = dynamic_cast<SvIe::SVCameraImageTemplate*>(l_pToolSet->getCurrentImage());
	}

	return pImage;
}

HRESULT SVInspectionProcess::GetInspectionImage(LPCTSTR Name, SvIe::SVImageClass*& p_rRefObject)
{
	// Set to Defaults in case of failure
	p_rRefObject = nullptr;

	// Specify that we are looking only for images
	SvDef::SVObjectTypeInfoStruct imageObjectInfo;
	imageObjectInfo.ObjectType = SvPb::SVImageObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor(imageObjectInfo);

	SVObjectManagerClass::Instance().VisitElements(l_Visitor, GetUniqueObjectID());

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	std::string ImageName {Name};
	std::string Inspections {SvDef::FqnInspections};
	Inspections += '.';
	//If Inspections prefix is present remove it
	if (0 == ImageName.find(Inspections))
	{
		ImageName = ImageName.substr(Inspections.size(), ImageName.size() - Inspections.size());
	}

	for (l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter)
	{
		SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*>(const_cast<SVObjectClass*>(*l_Iter));

		if (pImage->GetCompleteName() == ImageName)
		{
			// We found the image by name
			p_rRefObject = pImage;

			return S_OK;
		}
	}
	return E_FAIL;
}


// @TODO - this needs to be moved out of here into another more generic class
#ifdef EnableTracking
SVInspectionProcess::SVInspectionTrackingElement::SVInspectionTrackingElement()
	: m_StartTime(0), m_Start(), m_End(), m_Duration()
{
}

SVInspectionProcess::SVInspectionTrackingElement::SVInspectionTrackingElement(const SVInspectionTrackingElement& p_rObject)
	: m_StartTime(p_rObject.m_StartTime), m_Start(p_rObject.m_Start), m_End(p_rObject.m_End), m_Duration(p_rObject.m_Duration)
{
}

SVInspectionProcess::SVInspectionTrackingElement::~SVInspectionTrackingElement()
{
	m_StartTime = 0;

	m_Start.clear();
	m_End.clear();
	m_Duration.clear();
}

void SVInspectionProcess::SVInspectionTrackingElement::clear()
{
	m_StartTime = 0;

	m_Start.clear();
	m_End.clear();
	m_Duration.clear();
}

SVInspectionProcess::SVInspectionTracking::SVInspectionTracking()
	: m_StartTime(0.0), m_EventCounts()
{
}

SVInspectionProcess::SVInspectionTracking::SVInspectionTracking(const SVInspectionTracking& p_rObject)
	: m_StartTime(p_rObject.m_StartTime), m_EventCounts(p_rObject.m_EventCounts)
{
}

SVInspectionProcess::SVInspectionTracking::~SVInspectionTracking()
{
	m_StartTime = 0;

	if (!(m_EventCounts.empty()))
	{
		m_EventCounts.clear();
	}
}

void SVInspectionProcess::SVInspectionTracking::clear()
{
	if (!(m_EventCounts.empty()))
	{
		m_EventCounts.clear();
	}
}

void SVInspectionProcess::SVInspectionTracking::SetStartTime()
{
	m_StartTime = SvTl::GetTimeStamp();
}

void SVInspectionProcess::SVInspectionTracking::EventStart(const std::string& p_rName)
{
	double l_StartTime = SvTl::GetTimeStamp();
	__int64 l_EventTime = static_cast<__int64>(l_StartTime - m_StartTime);

	m_EventCounts[p_rName].m_StartTime = l_StartTime;

	++(m_EventCounts[p_rName].m_Start[l_EventTime]);
}

void SVInspectionProcess::SVInspectionTracking::EventEnd(const std::string& p_rName)
{
	double l_EndTime = SvTl::GetTimeStamp();
	__int64 l_Duration = static_cast<__int64>(l_EndTime - m_EventCounts[p_rName].m_StartTime);
	__int64 l_EventTime = static_cast<__int64>(l_EndTime - m_StartTime);

	++(m_EventCounts[p_rName].m_End[l_EventTime]);
	++(m_EventCounts[p_rName].m_Duration[l_Duration]);
}
#endif

void SVInspectionProcess::Persist(SvOi::IObjectWriter& rWriter)
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

	// Save the Toolset
	SVToolSetClass* pToolSet = GetToolSet();
	pToolSet->Persist(rWriter);

	rWriter.EndElement();
}

SVResultListClass* SVInspectionProcess::GetResultList() const
{
	SVResultListClass* retVal = nullptr;
	SVToolSetClass *pToolSet = GetToolSet();
	if (pToolSet)
	{
		retVal = pToolSet->GetResultList();
	}
	return retVal;
}

#pragma region IInspectionProcess methods
std::vector<SvPb::TreeItem> SVInspectionProcess::GetPPQSelectorList(const UINT attribute) const
{
	std::vector<SvPb::TreeItem> result;

	SVPPQObject *pPPQ = GetPPQ();
	SVObjectReferenceVector objectVector;

	if (nullptr != pPPQ)
	{
		std::string PpqName(pPPQ->GetName());

		SVObjectManagerClass::Instance().getTreeList(PpqName, objectVector, attribute);

		result.reserve(objectVector.size());

		for(auto ObjectRef : objectVector)
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
					insertItem.set_objectidindex(ObjectRef.GetGuidAndIndexOneBased());
					result.emplace_back(insertItem);
				}
			}
			else
			{
				insertItem.set_location(ObjectRef.GetCompleteName(true));
				insertItem.set_objectidindex(ObjectRef.GetGuidAndIndexOneBased());
				result.emplace_back(insertItem);
			}
		}
	}


	SVObjectPtrVector PpqVariables {getPPQVariables()};
	std::string InspectionName = GetName();

	for (auto pObject : PpqVariables)
	{
		if (nullptr != pObject)
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
				insertItem.set_objectidindex(ObjectRef.GetGuidAndIndexOneBased());
				result.emplace_back(insertItem);
			}
		}
	}

	return result;
}

SvOi::ITaskObject* SVInspectionProcess::GetToolSetInterface() const
{
	return GetToolSet();
}


HRESULT SVInspectionProcess::RunOnce()
{
	SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemNone;
	bool bRet = ProcessInputRequests(eResetItem);

	SingleRunModeLoop(true);

	return bRet ? S_OK : SvStl::cCustomHRSVO_RunOnceFailed;
}

HRESULT SVInspectionProcess::SubmitCommand(const SvOi::ICommandPtr& rCommandPtr)
{
	HRESULT Result = S_OK;

	if (m_CommandQueue.AddTail(rCommandPtr))
	{
		Result = m_AsyncProcedure.Signal(nullptr);
	}
	else
	{
		Result = E_FAIL;
	}

	return Result;
}

void SVInspectionProcess::BuildValueObjectMap()
{
	SVIOEntryStruct pListEntry;

	m_mapValueObjects.clear();

	SvOi::IValueObjectPtrSet::iterator Iter(m_ValueObjectSet.begin());

	for (; m_ValueObjectSet.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (*Iter);
		if(nullptr != pObject)
		{
			m_mapValueObjects.insert({pObject->GetCompleteName(), pObject});
		}
	}
}// end BuildValueObjectMap

GUID SVInspectionProcess::getFirstCamera() const
{
	GUID cameraGuid(GUID_NULL);

	if (!(m_ToolSetCameraName.empty()))
	{
		SvOi::IObjectClass* pCamera = SvOi::getObjectByDottedName(m_ToolSetCameraName.c_str());
		cameraGuid = (nullptr != pCamera) ? pCamera->GetUniqueObjectID() : GUID_NULL;
	}

	if (GUID_NULL == cameraGuid)
	{
		SvOi::IObjectClass* pCamera = getFirstPPQCamera();
		cameraGuid = (nullptr != pCamera) ? pCamera->GetUniqueObjectID() : GUID_NULL;
	}

	return cameraGuid;
}

HRESULT SVInspectionProcess::addSharedCamera(GUID cameraID)
{
	HRESULT result = S_OK;

	SVPPQObject* pPPQ = GetPPQ();

	SvIe::SVVirtualCamera* pCamera = dynamic_cast<SvIe::SVVirtualCamera*> (SvOi::getObject(cameraID));
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
	HRESULT retVal = E_FAIL;
	m_bForceOffsetUpdate = true;
	/// correct tool size when it does not fit to the parent image 
	AddResetState(SvDef::SVResetAutoMoveAndResize);
	bool result = rTool.resetAllObjects();
	if (result)
	{
		retVal = RunOnce();
	}
	RemoveResetState(SvDef::SVResetAutoMoveAndResize);
	return retVal;
}

HRESULT SVInspectionProcess::propagateSizeAndPosition()
{
	HRESULT retVal = S_OK;
	bool result = false;
	SVToolSetClass* pToolSet = GetToolSet();
	if (pToolSet)
	{
		for (int index = 0; index < pToolSet->GetSize(); index++)
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(pToolSet->GetAt(index));
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
		retVal = SvStl::Err_10005_SVCommandInspectionExtentUpdater_ResetAllObjects;
	}
	return retVal;
}

bool SVInspectionProcess::usePropagateSizeAndPosition() const
{
	SVToolSetClass* pToolSet = GetToolSet();
	if (pToolSet)
	{
		for (int index = 0; index < pToolSet->GetSize(); index++)
		{
			SvTo::SVToolClass* pTool = dynamic_cast<SvTo::SVToolClass*>(pToolSet->GetAt(index));
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
	SvStl::MessageContainer msg(SVMSG_SVO_NULL_POINTER, SvStl::Tid_Default, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
	throw msg;
}
#pragma endregion IInspectionProcess methods

bool SVInspectionProcess::IsDisabledPPQVariable(const SVObjectClass* pObject) const
{
	for (size_t i = 0; i < m_PPQInputs.size(); i++)
	{
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;

		if (!ioEntryPtr->m_Enabled)
		{
			if (pObject == ioEntryPtr->getObject())
			{
				return true;
			}
		}
	}

	return false;
}

SVObjectPtrVector SVInspectionProcess::getPPQVariables() const
{
	SVObjectPtrVector Result;
	std::map<std::string, SVObjectClass*> NameObjectMap;
	SvDef::StringVector PpqVariableNames;

	for (size_t i = 0; i < m_PPQInputs.size(); i++)
	{
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;

		//check if input is enable for this inspection
		if (ioEntryPtr->m_Enabled)
		{
			SVObjectClass* pObject(ioEntryPtr.get()->getObject());
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
	SVToolSetClass* pToolSet(GetToolSet());

	if (nullptr != pToolSet)
	{
		for (int i = 0; i < pToolSet->GetSize(); i++)
		{
			SvIe::SVTaskObjectClass* pTaskObject(pToolSet->GetAt(i));
			if (nullptr != pTaskObject)
			{
				const SvStl::MessageContainerVector& rToolMessages(pTaskObject->getErrorMessages());
				std::copy(rToolMessages.begin(), rToolMessages.end(), rInserter);
			}
		}
	}
}

bool SVInspectionProcess::DisconnectObjectInput(SvOl::SVInObjectInfoStruct* pObjectInInfo)
{
	bool Result = m_publishList.RemovePublishedEntry(pObjectInInfo->GetInputObjectInfo().getUniqueObjectID());

	if (nullptr != GetToolSet())
	{
		return GetToolSet()->DisconnectObjectInput(pObjectInInfo);
	}
	return Result;
}

bool SVInspectionProcess::createAllObjects(const SVObjectLevelCreateStruct& rCreateStructure)
{
	bool Result = __super::createAllObjects(rCreateStructure);

	if (nullptr != GetToolSet())
	{
		Result &= GetToolSet()->createAllObjects(rCreateStructure);
	}
	return Result;
}

bool SVInspectionProcess::CreateChildObject(SVObjectClass* pChildObject, DWORD context)
{
	if (nullptr != pChildObject)
	{
		SVObjectLevelCreateStruct createStruct;
		createStruct.OwnerObjectInfo.SetObject(this);
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
	else // Not a Valid Object
	{
		return false;
	}
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
		SvIe::SVTaskObjectClass* pTaskObject = dynamic_cast<SvIe::SVTaskObjectClass*>(pChild);

		if (nullptr != pTaskObject)
		{
			// Notify the Owner of our inputs that they are not needed anymore
			pTaskObject->Disconnect();
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

bool SVInspectionProcess::ConnectAllInputs()
{
	if (GetToolSet())
	{
		return GetToolSet()->ConnectAllInputs();
	}
	return false;
}

bool SVInspectionProcess::replaceObject(SVObjectClass* pObject, const GUID& rNewGuid)
{
	if (nullptr != pObject)
	{
		// Kill the Friends
		// SVObjectClass can have Friends
		pObject->DestroyFriends();

		// Special code for Duplicates!!
		SVObjectClass* pDuplicateObject = SVObjectManagerClass::Instance().GetObject(rNewGuid);
		if (pDuplicateObject)
		{
			if (pDuplicateObject == m_pCurrentToolset)
			{
				m_pCurrentToolset = nullptr;
			}

			// Get the Owner
			SVObjectInfoStruct ownerInfo = pDuplicateObject->GetOwnerInfo();

			SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.getUniqueObjectID());
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
					SvIe::SVTaskObjectListClass* pTaskListOwner = dynamic_cast<SvIe::SVTaskObjectListClass*>(pOwner);
					SvIe::SVTaskObjectListClass* pTaskList = dynamic_cast<SvIe::SVTaskObjectListClass*>(pDuplicateObject);
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

		if (SVObjectManagerClass::Instance().ChangeUniqueObjectID(pObject, rNewGuid))
		{
			SVToolSetClass* l_pToolSet(nullptr);

			l_pToolSet = dynamic_cast<SVToolSetClass*>(pObject);

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
					m_pCurrentToolset->AddFriend(m_pToolSetConditional->GetUniqueObjectID());

					m_pCurrentToolset->CreateChildObject(m_pToolSetConditional);

					m_pToolSetConditional = nullptr;
				}
			}
			else
			{
				SvOp::SVConditionalClass* pConditional = dynamic_cast<SvOp::SVConditionalClass*>(pObject);

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
		m_pRegressionTestPlayEquationController = SvOi::IFormulaControllerPtr {new SvOg::FormulaController(GetUniqueObjectID(), GetUniqueObjectID(), static_cast<GUID>(m_RegressionTestPlayEquation.GetUniqueObjectID()))};
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

void SVInspectionProcess::SetSlotmanager(const SvSml::RingBufferPointer& Slotmanager)
{
	if (m_SlotManager.get())
	{
		m_SlotManager.reset();
	}
	m_SlotManager = Slotmanager;
}

void SVInspectionProcess::buildValueObjectDefList() const
{
	SvPb::DataDefinitionList dataDefList;

	auto* pList = dataDefList.mutable_list();
	for (const auto* const pValueObject : m_ValueObjectSet)
	{
		const SvOi::IObjectClass* pObject = dynamic_cast<const SvOi::IObjectClass*> (pValueObject);
		if (nullptr != pValueObject && nullptr != pObject && 0 != pObject->ObjectAttributesAllowed())
		{
			auto* pValueObjectDef = pList->Add();
			const SvOi::IObjectClass* const pObject = dynamic_cast<const SvOi::IObjectClass* const> (pValueObject);
			SvPb::SetGuidInProtoBytes(pValueObjectDef->mutable_guidid(), pObject->GetUniqueObjectID().ToGUID());
			pValueObjectDef->set_name(pObject->GetCompleteName());
			pValueObjectDef->set_type(pObject->GetObjectSubType());
			pValueObjectDef->set_typestring(pValueObject->getTypeName());
			pValueObject->setTrPos(pList->size() - 1);
		}
	}

	std::vector<_variant_t> valueObjectList{copyValueObjectList(true)};
	
	SvTrc::getTriggerRecordControllerRWInstance().changeDataDef(std::move(dataDefList), std::move(valueObjectList), m_trcPos);
}

std::vector<_variant_t> SVInspectionProcess::copyValueObjectList(bool determineSize /*=false*/) const
{
	constexpr int cStringExtraBuffer = 15;		//For value objects which are saved as strings reserve 15 characters extra

	std::vector<_variant_t> result;

	result.reserve(m_ValueObjectSet.size());
	for (const auto* const pValueObject : m_ValueObjectSet)
	{
		const SvOi::IObjectClass* pObject = dynamic_cast<const SvOi::IObjectClass*> (pValueObject);
		if (nullptr != pValueObject && nullptr != pObject && 0 != pObject->ObjectAttributesAllowed())
		{
			_variant_t value;
			//when determineSize is true then the array size not the result size is returned to be able to determine the memory requirements
			if (S_OK == pValueObject->getValue(value, -1, !determineSize))
			{
				//This sets a standard size for strings of 15 characters
				if (VT_BSTR == value.vt && determineSize)
				{
					std::string temp = SvUl::createStdString(value);
					temp.resize(temp.size() + cStringExtraBuffer, ' ');
					value.SetString(temp.c_str());
				}
				//				
				if (VT_EMPTY != value.vt || !determineSize)
				{
				result.emplace_back(value);
			}
			}
			else
			{
				value.Clear();
				if (!determineSize)
				{
					result.emplace_back(value);
				}
			}

			if (VT_EMPTY == value.vt && determineSize)
			{
				value = pValueObject->getDefaultValue();
				if (VT_EMPTY != value.vt)
				{
					int arraySize = pValueObject->getArraySize();
					if (arraySize > 1 && VT_BSTR != value.vt)
					{
						variant_t arrayValue;
						SAFEARRAYBOUND arrayBound;
						arrayBound.lLbound = 0;
						arrayBound.cElements = arraySize;
						arrayValue.parray = ::SafeArrayCreate(value.vt, 1, &arrayBound);
						arrayValue.vt = value.vt | VT_ARRAY;
						value = arrayValue;
					}
				}
					result.emplace_back(value);
				}
			}
		}
	return result;
}


SvSml::RingBufferPointer SVInspectionProcess::GetSlotmanager()
{
	return m_SlotManager;
}

HRESULT SVInspectionProcess::LastProductUpdate(SVProductInfoStruct *p_psvProduct)
{
	HRESULT l_hrOk = S_OK;

	m_LastRunProductNULL = nullptr == p_psvProduct;

	if (!m_LastRunProductNULL)
	{
		p_psvProduct->SetProductComplete();
		std::lock_guard<std::mutex>  Autolock(m_LastRunMutex);

		l_hrOk = m_svLastRunProduct.Assign(*p_psvProduct);
	}

	return l_hrOk;
}

SVProductInfoStruct SVInspectionProcess::LastProductGet() const
{
	SVProductInfoStruct l_Temp;

	std::lock_guard<std::mutex>  Autolock(m_LastRunMutex);

	l_Temp.Assign(m_svLastRunProduct);

	if (m_LastRunProductNULL)
	{
		l_Temp.Reset();
	}

	return l_Temp;
}