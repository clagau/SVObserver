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
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVImageLibrary\SVImagingDeviceParams.h"
#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary\SVInspectionLevelCreateStruct.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVSystemLibrary\SVAutoLockAndReleaseTemplate.h"
#include "SVXMLLibrary\SVConfigurationTags.h"
#include "SVUtilityLibrary\NaturalStringCompare.h"
#include "SVObserver.h"
#include "SVObjectLibrary\SVGetObjectDequeByTypeVisitor.h"
#include "SVToolSet.h"
#include "SVPPQObject.h"
#include "SVGlobal.h"
#include "SVOCore/SVTool.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVOCore/SVConditional.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#include "SVCommandStreamManager.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVRunControlLibrary\SVRunControlLibrary.h"
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
#include "SVTimerLibrary\SVProfiler.h"
#endif
#include "SVSharedMemoryLibrary\SVSharedConfiguration.h"
#include "SVSharedMemoryLibrary\SharedMemWriter.h"
#include "SVSharedMemoryLibrary\SVSharedImage.h"
#pragma endregion Includes

SV_IMPLEMENT_CLASS(SVInspectionProcess, SVInspectionProcessGuid);

HRESULT SVInspectionProcess::ProcessInspection(bool& rProcessed, SVProductInfoStruct& rProduct)
{
	HRESULT l_Status = S_OK;

	rProcessed = (0 < m_qInspectionsQueue.GetCount());

	if (rProcessed)
	{
		rProcessed = (FALSE != m_qInspectionsQueue.GetHead(&rProduct));
	}

	if (rProcessed)
	{
#ifdef EnableTracking
		m_InspectionTracking.EventStart(_T("Process Inspections"));
#endif
		SVInspectionInfoStruct* pIPInfo = nullptr;

		size_t l_InputXferCount = 0;
		m_bInspecting = true;	// do this before RemoveHead

		// Get the info struct for this inspection
		pIPInfo = &(rProduct.m_svInspectionInfos[GetUniqueObjectID()]);

		HRESULT hr = ReserveNextResultImage(&rProduct, SV_INSPECTION);

		bool l_Process = (S_OK == hr);

		if (!l_Process)
		{
			SVStringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(static_cast<DWORD> (hr), SvStl::Tid_InspThreadGetResultImageIndexError, msgList, SvStl::SourceFileParams(StdMessageParams));
		}

		if (l_Process)
		{
			pIPInfo->m_BeginInspection = SvTl::GetTimeStamp();

			// Copy inputs to Inspection Process's Value objects
			for (size_t iList = 0; iList < m_PPQInputs.size(); iList++)
			{
				SVIOEntryStruct& pListEntry = m_PPQInputs[iList];
				SVIOEntryStruct& pInEntry = rProduct.oPPQInfo.m_InputData[iList];

				if (pInEntry.m_IOEntryPtr->m_Enabled)
				{
					pListEntry.m_EntryValid = pInEntry.m_EntryValid;

					if (pInEntry.m_EntryValid)
					{
						switch (pInEntry.m_IOEntryPtr->m_ObjectType)
						{
							case IO_DIGITAL_INPUT:
							{
								_variant_t Value;

							if (nullptr != pInEntry.m_IOEntryPtr->getValueObject())
								{
								pInEntry.m_IOEntryPtr->getValueObject()->getValue(Value, rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex());
								}
							if (nullptr != pListEntry.m_IOEntryPtr->getValueObject())
								{
								pListEntry.m_IOEntryPtr->getValueObject()->setValue(Value, rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex());
								}

								++l_InputXferCount;

								break;
							}
							case IO_REMOTE_INPUT:
							default:
							{
								_variant_t Value;

							if (nullptr != pInEntry.m_IOEntryPtr->getValueObject())
								{
								pInEntry.m_IOEntryPtr->getValueObject()->getValue(Value, rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex());
								}
							if (nullptr != pListEntry.m_IOEntryPtr->getValueObject())
								{
								pListEntry.m_IOEntryPtr->getValueObject()->setValue(Value, rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex());
								}

								++l_InputXferCount;

								break;
							}
						}
					}
					else
					{
						if (!(pListEntry.m_IOEntryPtr->m_Enabled))
						{
							++l_InputXferCount;
						}
					}
				}
				else
				{
					if (!(pListEntry.m_IOEntryPtr->m_Enabled))
					{
						++l_InputXferCount;
					}
				}
			}

			l_Process = l_Process && (l_InputXferCount == m_PPQInputs.size());
			l_Process = l_Process && (rProduct.IsAlive());

			// Check if tool set is alive and enabled...
			if (!l_Process)
			{
				// Product has left the PPQ ... don't inspect
				pIPInfo->oInspectedState = PRODUCT_NOT_INSPECTED;
			}
			else
			{
				// The Toolset will return FALSE if it did not run due to a Conditional
				// That means: TRUE  - The Tool Set were running.
				//			   FALSE - The Tool Set Condition failed and the Tool Set were
				//					   NOT running!

				SVImageIndexStruct l_svIndex;
				l_svIndex.m_ResultDMIndexHandle.Assign(pIPInfo->m_ResultImageDMIndexHandle, SV_INSPECTION);
				l_svIndex.m_PublishedResultDMIndexHandle.Assign(rProduct.oPPQInfo.m_ResultImagePublishedDMIndexHandle, SV_INSPECTION);
				BOOL bPassed = RunInspection(rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), l_svIndex, &rProduct);

				if (PRODUCT_INSPECTION_NOT_RUN != (pIPInfo->oInspectedState & PRODUCT_INSPECTION_NOT_RUN))
				{
					pIPInfo->oInspectedState = GetToolSet()->GetResultList()->GetInspectionState();

					pIPInfo->m_EndInspection = SvTl::GetTimeStamp();

#ifdef _DEBUG
					//					SVString l_TempStateString = SvUl_SF::Format( _T( "SVInspectionProcess::ProcessInspection|%s|TRI=%ld\n" ),
					//						GetName(), p_rProduct.ProcessCount() );
					//					::OutputDebugString( l_TempStateString.c_str() );
#endif

					// Mark last run product
					LastProductUpdate(&rProduct);
				}
			}
		}

		SVObjectManagerClass::Instance().DecrementInspectionIndicator();
		m_qInspectionsQueue.pop_front();

		if (l_Process)
		{
			::InterlockedIncrement(&m_NotifyWithLastInspected);

			pIPInfo->ClearIndexes();
		}

		m_bInspecting = false;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd(_T("Process Inspections"));
#endif
	}
	return l_Status;
}



void SVInspectionProcess::InitSharedMemoryItemNames(const long ProductSlots, const long RejectSlots)
{

	//set offset and image store index later also an Sharedmemory filter index 
	SvSml::SVSharedInspectionWriter& rWriter = SvSml::SharedMemWriter::Instance().GetInspectionWriter(GetPPQIdentifier(), GetUniqueObjectID());
	SvSml::SVSharedData& rFirstData = rWriter.GetLastInspectedSlot(0);


	SVNameObjectMap::iterator FilterValueMapIt = m_SharedMemoryFilters.m_LastInspectedValues.begin();

	for (int index = 0; FilterValueMapIt != m_SharedMemoryFilters.m_LastInspectedValues.end(); ++FilterValueMapIt, index++)
	{
		for (int slotindex = 0; slotindex < static_cast<int>(rWriter.GetLastInspectedCacheSize()); slotindex++)
		{
			rWriter.GetLastInspectedSlot(slotindex).m_Values[index].SetName(FilterValueMapIt->first);
	}
		for (int rslotindex = 0; rslotindex < static_cast<int>(rWriter.GetRejectCacheSize()); rslotindex++)
	{
			rWriter.GetRejectSlot(rslotindex).m_Values[index].SetName(FilterValueMapIt->first);
	}


	}

	SVNameObjectMap::iterator FilterImageMapIt = m_SharedMemoryFilters.m_LastInspectedImages.begin();
	for (int index = 0; FilterImageMapIt != m_SharedMemoryFilters.m_LastInspectedImages.end(); ++FilterImageMapIt, index++)
	{
		SvSml::MonitorEntryPointer mEp = SvSml::SharedMemWriter::Instance().GetMonitorEntryPointer(FilterImageMapIt->first);
		assert(mEp->name == FilterImageMapIt->first.c_str());
		for (int slotindex = 0; slotindex < static_cast<int>(rWriter.GetLastInspectedCacheSize()); slotindex++)
		{
			rWriter.GetLastInspectedSlot(slotindex).m_Images[index].SetImageStoreProperties(FilterImageMapIt->first.c_str(), mEp->InspectionStoreId, mEp->ItemId, slotindex, false);
			rWriter.GetLastInspectedSlot(slotindex).m_Images[index].SetFileName(SvSml::SVSharedImage::BuildImageFileName(mEp->ItemId, mEp->InspectionStoreId, slotindex, false));
	}
		for (int rslotindex = 0; rslotindex < static_cast<int>(rWriter.GetRejectCacheSize()); rslotindex++)
	{
			rWriter.GetRejectSlot(rslotindex).m_Images[index].SetImageStoreProperties(FilterImageMapIt->first.c_str(), mEp->InspectionStoreId, mEp->ItemId, rslotindex, true);
			rWriter.GetRejectSlot(rslotindex).m_Images[index].SetFileName(SvSml::SVSharedImage::BuildImageFileName(mEp->ItemId, mEp->InspectionStoreId, rslotindex, true));
	}
	}
}

// Update the list of items being monitored
HRESULT SVInspectionProcess::ProcessMonitorLists()
{
	HRESULT hr = S_OK;

	SVMonitorList l_MonitorList;

	if (m_MonitorListQueue.RemoveHead(&l_MonitorList))
	{
		bool bNotFound = false;
		m_SharedMemoryFilters.clear();

		const SVMonitorItemList& valueList = l_MonitorList.GetDataList();

		for (SVMonitorItemList::const_iterator l_ValueIter = valueList.begin(); l_ValueIter != valueList.end(); ++l_ValueIter)
		{
			const SVString& name = (*l_ValueIter);
			SVObjectReference ObjectRef;
			if (S_OK == GetInspectionValueObject(name.c_str(), ObjectRef))
			{
				m_SharedMemoryFilters.m_LastInspectedValues[name] = ObjectRef;
			}
			else
			{
				bNotFound = true;
			}
		}
		if (bNotFound)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvStl::Tid_ErrorNotAllDataItemsFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15021);
		}
		bNotFound = false;

		const SVMonitorItemList& imageList = l_MonitorList.GetImageList();

		long l_ImageSize = 0;
		for (SVMonitorItemList::const_iterator l_ImageIter = imageList.begin(); l_ImageIter != imageList.end(); ++l_ImageIter)
		{
			const SVString& name = (*l_ImageIter);
			SVImageClass* l_pImage;
			if (S_OK == GetInspectionImage(name.c_str(), l_pImage))
			{
				m_SharedMemoryFilters.m_LastInspectedImages[name] = SVObjectReference(l_pImage);
			}
			else
			{
				bNotFound = true;
			}
		}
		if (bNotFound)
		{
			SvStl::MessageMgrStd Exception(SvStl::LogOnly);
			Exception.setMessage(SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvStl::Tid_ErrorNotAllImageItemsFound, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15022);
		}
		::InterlockedIncrement(&m_NotifyWithLastInspected);
	}
	return hr;
}

HRESULT SVInspectionProcess::ProcessNotifyWithLastInspected(bool& p_rProcessed, long sharedSlotIndex)
{
	HRESULT l_Status = S_OK;

	if (0 < m_NotifyWithLastInspected)
	{
#ifdef EnableTracking
		m_InspectionTracking.EventStart(_T("Process Notify With Last Inspected"));
#endif
		::InterlockedExchange(&m_NotifyWithLastInspected, 0);

		SVProductInfoStruct l_Product = LastProductGet(SV_LAST_INSPECTED);
		if (GetPPQ()->HasActiveMonitorList() && SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
		{
			try
			{
				if (l_Product.ProcessCount() > 0 && sharedSlotIndex >= 0)
				{
					SvSml::SVSharedInspectionWriter& rWriter = SvSml::SharedMemWriter::Instance().GetInspectionWriter(GetPPQIdentifier(), GetUniqueObjectID());
					SvSml::SVSharedData& rLastInspected = rWriter.GetLastInspectedSlot(sharedSlotIndex);
					FillSharedData(sharedSlotIndex, rLastInspected, m_SharedMemoryFilters.m_LastInspectedValues, m_SharedMemoryFilters.m_LastInspectedImages, l_Product, rWriter);
				}
			}
			catch (const std::exception& e)
			{
				SVStringVector msgList;
				msgList.push_back(e.what());
				SvStl::MessageMgrStd Exception(SvStl::LogOnly);
				Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15023);
			}
			catch (...)
			{
				SVStringVector msgList;
				msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvStl::Tid_Unknown));
				SvStl::MessageMgrStd Exception(SvStl::LogOnly);
				Exception.setMessage(SVMSG_SVO_44_SHARED_MEMORY, SvStl::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15024);
			}
		}
		SVInspectionCompleteInfoStruct l_Data(GetUniqueObjectID(), l_Product);
		SVObjectManagerClass::Instance().UpdateObservers(SVString(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);

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
		SVCommandTemplatePtr l_CommandPtr;

		if (m_CommandQueue.RemoveHead(&l_CommandPtr))
		{
			if (!(l_CommandPtr.empty()))
			{
				l_Status = l_CommandPtr->Execute();
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVInspectionObjectType;

	m_LastRunLockPtr = new SVCriticalSection;
	m_LastRunProductNULL = false;

	m_pCurrentToolset = nullptr;
	m_PPQId.clear();

	m_bNewDisableMethod	= false;
	m_lEnableAuxiliaryExtents = 0;
	m_lInputRequestMarkerCount = 0L;
	m_bInspecting = false;
	m_dwThreadId = 0;

	m_svReset.RemoveState(SvOi::SVResetStateAll);

	m_bForceOffsetUpdate = true;

	m_publishList.m_pInspection = this;

	m_pToolSetConditional = nullptr;

	SVString ImageDirectory = SvSml::SVSharedConfiguration::GetImageDirectoryName() + "\\";
	memset(m_BufferImageFileName, 0, sizeof(TCHAR) * BUFFER_IMAGE_FILENAME_LEN);
	_tcscpy_s(m_BufferImageFileName, BUFFER_IMAGE_FILENAME_LEN, ImageDirectory.c_str());
	m_SecondPtrImageFileName = m_BufferImageFileName + static_cast<int>(strlen(m_BufferImageFileName));
	m_SecondPtrImageFileNameLen =  BUFFER_IMAGE_FILENAME_LEN - static_cast<int>(strlen(m_BufferImageFileName));
}

SVInspectionProcess::~SVInspectionProcess()
{
	DestroyInspection();

	m_PPQId.clear();

	m_pCurrentToolset	= nullptr;

	m_bNewDisableMethod	= false;
	m_lEnableAuxiliaryExtents = 0;
	m_lInputRequestMarkerCount = 0L;
}

bool SVInspectionProcess::IsCameraInInspection(const SVString& rCameraName) const
{
	bool Result(false);

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while (!Result && l_Iter != m_CameraImages.end())
	{
		if (nullptr != (*l_Iter))
		{
			Result = (rCameraName == (*l_Iter)->GetName());
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

BOOL SVInspectionProcess::CreateInspection(LPCTSTR szDocName)
{
	HRESULT hr;

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
	if (!m_MonitorListQueue.Create()) { return false; }

	::InterlockedExchange(&m_NotifyWithLastInspected, 0);

	if (S_OK != m_AsyncProcedure.Create(&SVInspectionProcess::APCThreadProcess, boost::bind(&SVInspectionProcess::ThreadProcess, this, _1), GetName(), SVThreadAttribute::SVAffinityUser))
	{
		return false;
	}

	hr = CreateResultImageIndexManager();
	if (S_OK != hr)
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

	m_pCurrentToolset = new SVToolSetClass(true, this);

	if (!CreateChildObject(m_pCurrentToolset))
	{
		return false;
	}

	SVCommandStreamManager::Instance().InsertInspection(GetUniqueObjectID());

	return true;
}// end Create

HRESULT SVInspectionProcess::CreateResultImageIndexManager() const
{
	HRESULT l_Status = S_OK;

	// Create a managed index for the result data and image circle buffers
	SVString l_Temp = SvUl_SF::Format(_T("%s Result Image"), GetName());

	l_Status = TheSVDataManager.CreateManagedIndexArray(m_pResultImageCircleBuffer, _bstr_t(l_Temp.c_str()), TheSVObserverApp.GetResultImageDepth());

	return l_Status;
}

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

	ProcessNotifyWithLastInspected(l_Processed, l_Product.m_svInspectionInfos[GetUniqueObjectID()].m_lastInspectedSlot);

	ProcessCommandQueue(l_Processed);

	if (!(l_Product.empty()) && !(m_PPQId.empty()))
	{
		SVInspectionCompleteInfoStruct l_Data(GetUniqueObjectID(), l_Product);
		SVObjectManagerClass::Instance().UpdateObserver(m_PPQId, l_Data);
	}

	p_WaitForEvents = !l_Processed;
}

BOOL SVInspectionProcess::DestroyInspection()
{
	SVIOEntryStruct pIOEntry;

	SVCommandStreamManager::Instance().EraseInspection(GetUniqueObjectID());

	SVObjectManagerClass::Instance().UpdateObservers(SVString(SvO::cInspectionProcessTag), GetUniqueObjectID(), SVRemoveSubjectStruct());

	::InterlockedExchange(&m_NotifyWithLastInspected, 0);

	::Sleep(0);

	m_AsyncProcedure.Destroy();

	SVProductInfoStruct l_svProduct;

	LastProductUpdate(&l_svProduct);

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
	m_MonitorListQueue.Destroy();

	SVObjectManagerClass::Instance().AdjustInspectionIndicator(-(m_qInspectionsQueue.GetCount()));

	// Destroy Queues for Inspection Queue
	m_qInspectionsQueue.Destroy();

	m_PPQInputs.clear();

	m_PPQId.clear();

	m_pResultImageCircleBuffer.clear();

	SVResultListClass* pResultlist = GetResultList();
	if (pResultlist)
	{
		pResultlist->RebuildReferenceVector(this);
	}

	return true;
}// end Destroy

HRESULT SVInspectionProcess::BuildValueObjectMap()
{
	HRESULT l_hrOk = S_OK;
	SVIOEntryStruct pListEntry;

	m_mapValueObjects.clear();

	SvOi::IValueObjectPtrSet::iterator Iter(m_ValueObjectSet.begin());

	for (; m_ValueObjectSet.end() != Iter; ++Iter)
	{
		SVObjectClass* pObject = dynamic_cast<SVObjectClass*> (*Iter);
		m_mapValueObjects.insert(SVValueObjectMap::value_type(pObject->GetCompleteName().c_str(), pObject));
	}

	return l_hrOk;
}// end BuildValueObjectMap

HRESULT SVInspectionProcess::GetInspectionValueObject(LPCTSTR Name, SVObjectReference& rObjectRef)
{
	HRESULT hr = S_FALSE;

	SVObjectNameInfo l_NameInfo;

	SVObjectClass* pObject(nullptr);

	hr = l_NameInfo.ParseObjectName(Name);

	if (S_OK == hr)
	{
		if (0 < l_NameInfo.m_NameArray.size() && l_NameInfo.m_NameArray[0] != GetName())
		{
			l_NameInfo.m_NameArray.push_front(GetName());
		}

		const SVString& l_ObjectName = l_NameInfo.GetObjectName();

		SVValueObjectMap::index_const_iterator< from >::type l_Iter;

		l_Iter = m_mapValueObjects.get< from >().find(l_ObjectName.c_str());

		if (l_Iter != m_mapValueObjects.get< from >().end())
		{
			pObject = l_Iter->second;
		}
	}

	if (nullptr != pObject)
	{
		rObjectRef = SVObjectReference(pObject, l_NameInfo);
		hr = S_OK;
	}// end if

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
		SVImageClass* pImage = nullptr;
		hr = GetInspectionImage(Name, pImage);
		if (S_OK == hr)
		{
			rObjectRef = SVObjectReference(pImage);
		}
	}
	return hr;
}

BOOL SVInspectionProcess::CanGoOnline()
{
	BOOL l_bOk(true);

	CWaitCursor l_cwcMouse;

	SetResetCounts();
	m_svReset.AddState(SvOi::SVResetStateInitializeOnReset | SvOi::SVResetStateArchiveToolCreateFiles | SvOi::SVResetStateLoadFiles);

	l_bOk = (S_OK == InitializeRunOnce());

	m_svReset.RemoveState(SvOi::SVResetStateInitializeOnReset | SvOi::SVResetStateArchiveToolCreateFiles | SvOi::SVResetStateLoadFiles);

	ClearResetCounts();

	return l_bOk;
}// end CanGoOnline

BOOL SVInspectionProcess::CanRegressionGoOnline()
{
	BOOL l_bOk(true);

	CWaitCursor l_cwcMouse;

	SetResetCounts();
	m_svReset.AddState(SvOi::SVResetStateInitializeOnReset | SvOi::SVResetStateArchiveToolCreateFiles | SvOi::SVResetStateLoadFiles);

	l_bOk = resetAllObjects();

	m_svReset.RemoveState(SvOi::SVResetStateInitializeOnReset | SvOi::SVResetStateArchiveToolCreateFiles | SvOi::SVResetStateLoadFiles);

	ClearResetCounts();

	return l_bOk;
}

BOOL SVInspectionProcess::GoOnline()
{
#ifdef EnableTracking
	m_InspectionTracking.clear();
#endif

	LastProductUpdate(nullptr);

	m_AsyncProcedure.SetPriority(THREAD_PRIORITY_NORMAL);

	if (S_OK != BuildValueObjectMap())
	{
		return false;
	}

	ProcessMonitorLists();
	SVPPQObject* pPPQ = GetPPQ();
	if (pPPQ && pPPQ->HasActiveMonitorList())
	{
		try
		{
		InitSharedMemoryItemNames(pPPQ->GetNumProductSlots(), pPPQ->GetNumRejectSlots());
	}
		catch (const SvStl::MessageContainer& rSvE)
		{
			//Now that we have caught the exception we would like to display it
			SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
			Msg.setMessage(rSvE.getMessage());
		}
		catch (std::exception& e)
		{

			SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, e.what(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16223);
			return false;
		}

		catch (...)
		{
			SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
			Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, _T("Unknown error Handler"), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_16223);
			return false;
		}

	}
	return true;
}// end GoOnline

BOOL SVInspectionProcess::GoOffline()
{
#ifdef EnableTracking
	if (TheSVObserverApp.UpdateAndGetLogDataManager())
	{
		SVString l_FileName;

		l_FileName.Format(_T("C:\\SVObserver\\ProductLastIndexes_%ld-%s.log"),
			SVObjectManagerClass::Instance().GetFileSequenceNumber(), GetName());

		std::fstream l_Stream(l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out);

		if (l_Stream.is_open())
		{
			SVString l_Info;
			SVProductInfoStruct l_Product = LastProductGet(SV_OTHER);

			l_Product.DumpIndexInfo(l_Info);

			l_Stream << _T("Last : ");
			l_Stream << l_Info.ToString();
			l_Stream << std::endl;

			l_Stream.close();
		}

		SVString l_Name;

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

	if (nullptr != m_pCurrentToolset)
	{
		m_pCurrentToolset->goingOffline();
	}

	if (LastProductGet(SV_OTHER).ProcessCount() < 1)
	{
		RestoreCameraImages();
	}
	// Stop updating Shared Memory
	UpdateSharedMemoryFilters(SVMonitorList());

	return true;
}// end GoOffline

HRESULT SVInspectionProcess::SubmitCommand(const SVCommandTemplatePtr& p_rCommandPtr)
{
	HRESULT l_Status = S_OK;

	if (m_CommandQueue.AddTail(p_rCommandPtr))
	{
		l_Status = m_AsyncProcedure.Signal(nullptr);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVInspectionProcess::CanProcess(SVProductInfoStruct *pProduct)
{
	SVIOEntryStruct pInEntry;
	SVIOEntryStruct pListEntry;
	int iInSize;
	int iIn;
	bool bReady = true;

	if (pProduct)
	{
		size_t ppqInputSize = m_PPQInputs.size();
		// See if we have discrete inputs.

		for (size_t iList = 0; bReady && iList < ppqInputSize; iList++)
		{
			pListEntry = m_PPQInputs[iList];
			if (pListEntry.m_IOEntryPtr->m_Enabled)
			{
				iInSize = static_cast<int>(pProduct->oPPQInfo.m_InputData.size());

				for (iIn = 0; bReady && iIn < iInSize; iIn++)
				{
					pInEntry = pProduct->oPPQInfo.m_InputData[iIn];
					if (pInEntry.m_IOEntryPtr->m_IOId == pListEntry.m_IOEntryPtr->m_IOId)
					{
						bReady &= pInEntry.m_EntryValid;
						break;
					}// end if

				}// end for

			}// end if

		}// end for

		// See if we have Camera images.
		SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while (bReady && l_ImageIter != m_CameraImages.end())
		{
			SVCameraImageTemplate* l_pImage = (*l_ImageIter);

			if (nullptr != l_pImage)
			{
				SVVirtualCamera* pCamera = l_pImage->GetCamera();

				if (nullptr != pCamera)
				{
					SVGuidSVCameraInfoStructMap::iterator l_Iter;

					l_Iter = pProduct->m_svCameraInfos.find(pCamera->GetUniqueObjectID());

					if (l_Iter != pProduct->m_svCameraInfos.end())
					{
						bReady &= (0 <= l_Iter->second.GetIndex());
					}
				}
			}

			++l_ImageIter;
		}
	} // if( pProduct )
	else
		bReady = false;

	return bReady;
}// end CanProcess

HRESULT SVInspectionProcess::StartProcess(SVProductInfoStruct *pProduct)
{
	HRESULT hr = S_OK;

	// Make sure that the lists are the same size
	if (m_PPQInputs.size() != pProduct->oPPQInfo.m_InputData.size())
	{
		hr = E_FAIL;
	}// end if

	if (0 == m_qInspectionsQueue.GetCount())
	{
		// Find the info struct for this inspection
		SVInspectionInfoStruct& l_rIPInfo = pProduct->m_svInspectionInfos[GetUniqueObjectID()];

		l_rIPInfo.m_CanProcess = false;
		l_rIPInfo.m_InProcess = true;

		SVProductInfoStruct l_TempProduct;

		hr = l_TempProduct.Assign(*pProduct, SV_INSPECTION);

		SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while (l_ImageIter != m_CameraImages.end())
		{
			SVCameraImageTemplate* l_pImage = (*l_ImageIter);

			if (nullptr != l_pImage)
			{
				SVVirtualCamera* pCamera = l_pImage->GetCamera();

				if (nullptr != pCamera)
				{
					SVGuidSVCameraInfoStructMap::iterator l_Iter;

					l_Iter = pProduct->m_svCameraInfos.find(pCamera->GetUniqueObjectID());

					if (l_Iter != pProduct->m_svCameraInfos.end())
					{
						if (l_Iter->second.GetIndex() < 0)
						{
							hr = SVMSG_SVO_71_INVALID_SOURCE_IMAGE_INDEX;
						}
					}
				}
			}

			++l_ImageIter;
		}

		// Set the flag in the Temp product because it gets reset when copying from the
		// pProduct->m_svInspectionInfos to the Temp product. And that get put in the m_qInspectionsQueue.
		l_TempProduct.m_svInspectionInfos[GetUniqueObjectID()].m_HasBeenQueued = true;

		if (S_OK == hr && m_qInspectionsQueue.AddTail(l_TempProduct))
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

BOOL SVInspectionProcess::RebuildInspectionInputList()
{
	SVIOEntryStructVector ppOldPPQInputs;
	SVIOEntryHostStructPtrVector ppIOEntries;
	SVIOEntryHostStructPtr pNewEntry;
	SVIOEntryStruct pIOEntry;
	size_t iTotal;
	int iList;
	long l;
	long lSize;
	long lListSize;
	long lLength;
	BOOL bFound;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr == pPPQ)
	{
		return false;
	}

	// Save old list
	ppOldPPQInputs = m_PPQInputs;

	if (!pPPQ->GetAvailableInputs(ppIOEntries))
		return FALSE;

	lListSize = static_cast<long>(ppIOEntries.size());

	pPPQ->GetPPQLength(lLength);

	// Make new list
	m_PPQInputs.resize(lListSize);

	long l_LastIndex = 1;
	SVProductInfoStruct l_Product = LastProductGet(SV_INSPECTION);

	if (!(l_Product.empty()))
	{
		SVDataManagerHandle l_Handle;

		l_Product.GetResultDataIndex(l_Handle);

		l_LastIndex = l_Handle.GetIndex();
	}

	for (iList = 0; iList < lListSize; iList++)
	{
		pNewEntry = ppIOEntries[iList];
		bFound = FALSE;

		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject(pNewEntry->m_IOId);

		for (iTotal = 0; iTotal < ppOldPPQInputs.size(); iTotal++)
		{
			pIOEntry = ppOldPPQInputs[iTotal];

			if (0 == strcmp(pIOEntry.m_IOEntryPtr->getObject()->GetName(), l_pObject->GetName()))
			{
				// We found it
				bFound = TRUE;
				m_PPQInputs[iList] = pIOEntry;
				pIOEntry.m_IOEntryPtr->m_PPQIndex = pNewEntry->m_PPQIndex;
				pIOEntry.m_IOEntryPtr->m_Enabled = pNewEntry->m_Enabled;
				pIOEntry.m_IOEntryPtr->getObject()->SetObjectDepth(lLength + 50);
				pIOEntry.m_IOEntryPtr->getObject()->ResetObject();

				break;
			}// end if
		}// end for

		if (!bFound)
		{
			SVObjectClass* pObject = nullptr;

			switch (pNewEntry->m_ObjectType)
			{
				case IO_DIGITAL_INPUT:
				{
					SVBoolValueObjectClass* pIOObject = new SVBoolValueObjectClass(this);
				pIOObject->setResetOptions(false, SvOi::SVResetItemNone);
					pObject = dynamic_cast<SVObjectClass*> (pIOObject);
				pObject->SetObjectAttributesAllowed(SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
					break;
				}
				case IO_REMOTE_INPUT:
				{
					SVVariantValueObjectClass* pIOObject = new SVVariantValueObjectClass(this);
				pIOObject->setResetOptions(false, SvOi::SVResetItemNone);
					pObject = dynamic_cast<SVObjectClass*> (pIOObject);
				pObject->SetObjectAttributesAllowed(SvOi::SV_REMOTELY_SETABLE, SvOi::SetAttributeType::AddAttribute);
					break;
				}
				default:
				{
					SVVariantValueObjectClass* pIOObject = new SVVariantValueObjectClass(this);
				pIOObject->setResetOptions(false, SvOi::SVResetItemNone);
					pObject = dynamic_cast<SVObjectClass*> (pIOObject);
					break;
				}
			}

			pObject->SetName(l_pObject->GetName());
			pObject->SetObjectOwner(this);
			pObject->SetObjectDepth(lLength + 50);
			pObject->SetObjectAttributesSet(pObject->ObjectAttributesSet() & SvOi::SV_PUBLISHABLE, SvOi::SetAttributeType::OverwriteAttribute);
			pObject->ResetObject();

			CreateChildObject(pObject);

			pIOEntry.m_IOEntryPtr = new SVIOEntryHostStruct;
			pIOEntry.m_IOEntryPtr->setObject(pObject);
			pIOEntry.m_IOEntryPtr->m_ObjectType = pNewEntry->m_ObjectType;
			pIOEntry.m_IOEntryPtr->m_IOId = pNewEntry->m_IOId;
			pIOEntry.m_IOEntryPtr->m_Enabled = pNewEntry->m_Enabled;

			m_PPQInputs[iList] = pIOEntry;
		}// end if

		// We now store whether or not these PPQ inputs were viewed
		bFound = FALSE;
		lSize = static_cast<long>(m_arViewedInputNames.size());
		for (l = 0; l < lSize; l++)
		{
			if (nullptr != m_PPQInputs[iList].m_IOEntryPtr->getObject() &&
				m_PPQInputs[iList].m_IOEntryPtr->getObject()->GetCompleteName() == m_arViewedInputNames[l])
			{
				m_PPQInputs[iList].m_IOEntryPtr->getObject()->SetObjectAttributesSet(SvOi::SV_VIEWABLE, SvOi::SetAttributeType::AddAttribute);
				bFound = TRUE;
				break;
			}// end if
		}// end for

		if (!bFound || !m_PPQInputs[iList].m_IOEntryPtr->m_Enabled)
		{
			m_PPQInputs[iList].m_IOEntryPtr->getObject()->SetObjectAttributesSet(SvOi::SV_VIEWABLE, SvOi::SetAttributeType::RemoveAttribute);
		}// end if

		SvOi::SetAttributeType AddRemoveType = m_PPQInputs[iList].m_IOEntryPtr->m_Enabled ? SvOi::SetAttributeType::AddAttribute : SvOi::SetAttributeType::RemoveAttribute;
		m_PPQInputs[iList].m_IOEntryPtr->getObject()->SetObjectAttributesAllowed(SvOi::SV_SELECTABLE_FOR_EQUATION, AddRemoveType);

	}// end for

	SVResultListClass* pResultlist = GetResultList();
	if (pResultlist)
	{
		pResultlist->RebuildReferenceVector(this);
	}

	BuildValueObjectMap();
	return TRUE;
}

bool SVInspectionProcess::AddInputRequest(const SVObjectReference& rObjectRef, const _variant_t& rValue)
{
	bool Result(false);

	try
	{
		SVInputRequestInfoStructPtr pInRequest = new SVInputRequestInfoStruct(rObjectRef, rValue);

		//add request to inspection process
		Result = AddInputRequest(pInRequest);
	}
	catch (...) // @WARNING:  bad practice catching ...
	{
	}

	return Result;
}

bool SVInspectionProcess::AddInputRequest(SVInputRequestInfoStructPtr p_pInRequest)
{
	if (!m_InputRequests.Lock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17013_ErrorLockingInputRequests);
		DebugBreak();
	}

	if (!m_InputRequests.AddTail(p_pInRequest))
	{
		if (!m_InputRequests.Unlock())
		{
			SvStl::MessageMgrStd e(SvStl::LogOnly);
			e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17014_ErrorUnlockingInputRequests);
			DebugBreak();
		}

		return false;
	}


	SVString l_StringValue;

	if (VT_BSTR == p_pInRequest->m_Value.vt)
	{
		l_StringValue = SvUl_SF::createSVString(p_pInRequest->m_Value);
	}

	if (l_StringValue == SvO::SVTOOLPARAMETERLIST_MARKER)
	{
		::InterlockedIncrement(const_cast<long*>(&m_lInputRequestMarkerCount));
	}// end if

	if (!m_InputRequests.Unlock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17015_ErrorUnlockingInputRequests);
		DebugBreak();
	}

	return true;
}

HRESULT SVInspectionProcess::AddInputImageRequest(SVImageClass* p_psvImage, BSTR& p_rbstrValue)
{
	HRESULT l_Status = S_OK;

	if (nullptr != p_psvImage)
	{
		SVImageInfoClass l_ImageInfo = p_psvImage->GetImageInfo();

		try
		{
			SVInputImageRequestInfoStructPtr l_pInRequest = new SVInputImageRequestInfoStruct;

			try
			{
				SVCameraImageTemplate* l_psvMainImage = dynamic_cast<SVCameraImageTemplate*>(p_psvImage);

				l_Status = SVImageProcessingClass::LoadImageBuffer((void*)p_rbstrValue,
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

HRESULT SVInspectionProcess::AddInputImageFileNameRequest(SVImageClass* p_psvImage, const SVString& p_rImageFileName)
{
	HRESULT l_Status = S_OK;

	if (nullptr != p_psvImage)
	{
		SVImageInfoClass l_ImageInfo = p_psvImage->GetImageInfo();

		try
		{
			SVInputImageRequestInfoStructPtr l_pInRequest = new SVInputImageRequestInfoStruct;

			SVCameraImageTemplate* l_psvMainImage = dynamic_cast<SVCameraImageTemplate*> (p_psvImage);

			l_Status = SVImageProcessingClass::LoadImageBuffer(p_rImageFileName.c_str(), l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr);

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
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::AddInputImageRequest(SVInputImageRequestInfoStructPtr p_pInRequest)
{
	if (!m_InputImageRequests.Lock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorLockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17016_ErrorLockingInputImageRequests);
		DebugBreak();
	}

	if (!m_InputImageRequests.AddTail(p_pInRequest))
	{
		if (!m_InputImageRequests.Unlock())
		{
			SvStl::MessageMgrStd e(SvStl::LogOnly);
			e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17017_ErrorUnlockingInputImageRequests);
			DebugBreak();
		}

		return S_FALSE;
	}// end if

	if (!m_InputImageRequests.Unlock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17018_ErrorUnlockingInputImageRequests);
		DebugBreak();
	}

	return S_OK;
}// end AddInputImageRequest

BOOL SVInspectionProcess::RemoveAllInputRequests()
{
	if (!m_InputRequests.Lock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17019_ErrorLockingInputRequests);
		DebugBreak();
	}

	if (!m_InputRequests.RemoveAll())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorRemovingAllInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17020_ErrorRemovingAllInputRequests);
		DebugBreak();
	}

	if (!m_InputRequests.Unlock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17021_ErrorUnlockingInputRequests);
		DebugBreak();
	}

	if (!m_InputImageRequests.Lock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorLockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17022_ErrorLockingInputImageRequests);
		DebugBreak();
	}

	if (!m_InputImageRequests.RemoveAll())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorRemovingAllInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17023_ErrorRemovingAllInputImageRequests);
		DebugBreak();
	}

	if (!m_InputImageRequests.Unlock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17024_ErrorUnlockingInputImageRequests);
		DebugBreak();
	}

	return TRUE;
}// end RemoveAllInputRequests

//******************************************************************************
// Message Operation(s):
//******************************************************************************

HRESULT SVInspectionProcess::RebuildInspection()
{
	HRESULT l_Status = S_OK;

	SetDefaultInputs();

	long lLength = 2;
	long l_LastIndex = 1;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		pPPQ->GetPPQLength(lLength);

		SVProductInfoStruct l_Product = LastProductGet(SV_INSPECTION);

		if (l_Product.empty())
		{
			pPPQ->ReserveNextRunOnceProductInfoStruct(l_Product, SV_INSPECTION);

			ReserveNextResultImage(&l_Product, SV_INSPECTION, true);

			UpdateMainImagesByProduct(&l_Product);

			LastProductUpdate(&l_Product);
		}

		SVDataManagerHandle l_Handle;

		l_Product.GetResultDataIndex(l_Handle);

		l_LastIndex = l_Handle.GetIndex();
	}

	SetObjectDepthWithIndex(lLength + 50, l_LastIndex);
	SetImageDepth(TheSVObserverApp.GetResultImageDepth());

	////////////////////////
	// Color SVIM
	if (IsColorCamera())
	{
		SVVirtualCameraMap l_Cameras;

		if (nullptr != pPPQ)
		{
			pPPQ->GetVirtualCameras(l_Cameras);
		}

		bool bColorSourceImage = false;
		bool bCameraSupportsColor = false;
		SVVirtualCameraMap::iterator pos = l_Cameras.begin();
		while (pos != l_Cameras.end())
		{
			SVAcquisitionClassPtr pCamDevice = nullptr;

			if (nullptr != pos->second)
			{
				pCamDevice = pos->second->GetAcquisitionDevice();
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

			++pos;
		}

		if (!bColorSourceImage)
		{
			if (!bCameraSupportsColor)
			{
				l_Status = E_FAIL;

				SvStl::MessageMgrStd Msg(SvStl::LogAndDisplay);
				Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_IPDoc_NoCameraColorAttached, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_10052);
			}
		}
			}
	////////////////////////

	SVObjectLevelCreateStruct createStruct;

	if (createAllObjects(createStruct))
	{
		l_Status = E_FAIL;
	}

	SetDefaultInputs();

	if (CheckAndResetConditionalHistory())
	{
		//Configuration has changed need to set the modified flag
		SVSVIMStateClass::AddState(SV_STATE_MODIFIED);
		SvStl::MsgTypeEnum  MsgType = SVSVIMStateClass::CheckState(SV_STATE_REMOTE_CMD) ? SvStl::LogOnly : SvStl::LogAndDisplay;
		SvStl::MessageMgrStd Exception(MsgType);
		Exception.setMessage(SVMSG_SVO_CONDITIONAL_HISTORY, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
		}

#if defined (TRACE_THEM_ALL) || defined (TRACE_IP)
	int iCount = static_cast<int>(m_mapValueObjects.size());
	SVString Text = SvUl_SF::Format(_T("%s value object count=%d\n"), GetName(), iCount);
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

void SVInspectionProcess::ValidateAndInitialize(bool p_Validate, bool p_IsNew)
{
	SVSVIMStateClass::AddState(SV_STATE_INTERNAL_RUN);

	// Call ToolSet Validate 
	// so the SVEquationClass can rebuild its symbol table
	if (p_Validate)
	{
		SetResetCounts();

		m_svReset.AddState(SvOi::SVResetAutoMoveAndResize | SvOi::SVResetStateInitializeOnReset | SvOi::SVResetStateArchiveToolCreateFiles | SvOi::SVResetStateLoadFiles);

		SVStdMapSVToolClassPtrSVInspectionProcessResetStruct l_svToolMap;

		SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemIP;

		BOOL bok = ProcessInputRequests(1, eResetItem, l_svToolMap);

		m_svReset.RemoveState(SvOi::SVResetAutoMoveAndResize | SvOi::SVResetStateInitializeOnReset | SvOi::SVResetStateArchiveToolCreateFiles | SvOi::SVResetStateLoadFiles);
	}

	if (!p_IsNew)
	{
		RunOnce();
	}

	SVSVIMStateClass::RemoveState(SV_STATE_INTERNAL_RUN);
}

void SVInspectionProcess::SingleRunModeLoop(bool p_Refresh)
{
	SVProductInfoStruct l_svProduct;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		pPPQ->ReserveNextRunOnceProductInfoStruct(l_svProduct, SV_INSPECTION);
	}

	LastProductCopySourceImagesTo(&l_svProduct);
	ReserveNextResultImage(&l_svProduct, SV_INSPECTION, true);

	SVImageIndexStruct l_svResultImageIndex;

	l_svProduct.GetResultImageIndex(l_svResultImageIndex, GetUniqueObjectID());

	SVDataManagerHandle	l_ResultDataDMIndexHandle;

	l_svProduct.GetResultDataIndex(l_ResultDataDMIndexHandle);

	RunInspection(l_ResultDataDMIndexHandle.GetIndex(), l_svResultImageIndex, &l_svProduct);

	LastProductUpdate(&l_svProduct);

	if (p_Refresh)
	{
		// Result View, Display Image and Overlay Update
		if (!SVSVIMStateClass::CheckState(SV_STATE_RUNNING))
		{
			SVInspectionCompleteInfoStruct l_Data(GetUniqueObjectID(), l_svProduct);

			SVObjectManagerClass::Instance().UpdateObservers(SVString(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
		}
	}
}

bool SVInspectionProcess::resetAllObjects(SvStl::MessageContainerVector *pErrorMessages/*=nullptr */)
{
	SvStl::MessageContainerVector ErrorMessages;

	for (size_t l = 0; l < m_PPQInputs.size(); l++)
	{
		if (!(m_PPQInputs[l].empty()) && nullptr != m_PPQInputs[l].m_IOEntryPtr->getObject())
		{
			m_PPQInputs[l].m_IOEntryPtr->getObject()->resetAllObjects(&ErrorMessages);
			break;
		}
	}// end for

	bool Result = true;

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

	Result = __super::resetAllObjects( &ErrorMessages ) && Result;
	//log all error messages to event log
	for (SvStl::MessageContainerVector::iterator iter = ErrorMessages.begin(); ErrorMessages.end() != iter; iter++)
	{
		SvStl::MessageMgrStd message(SvStl::LogOnly);
		message.setMessage(iter->getMessage());
	}
	if (nullptr != pErrorMessages)
	{
		*pErrorMessages = ErrorMessages;
	}
	return Result;
}

BOOL SVInspectionProcess::GetChildObjectByName(LPCTSTR tszChildName, SVObjectClass** ppObject)
{
	ASSERT(nullptr != ppObject);
	bool bReturn = false;

	if (nullptr != ppObject)
	{
		*ppObject = nullptr;

		SVString ChildName = tszChildName;
		SVString Name = GetCompleteName();

		if (SvUl_SF::Left(ChildName, Name.size()) == Name)
		{
			SVObjectManagerClass::Instance().GetObjectByDottedName(tszChildName, *ppObject);
			bReturn = (nullptr != *ppObject);
		}
	}

	return bReturn;
}

HRESULT SVInspectionProcess::FindPPQInputObjectByName(SVObjectClass*& p_rpObject, LPCTSTR p_FullName) const
{
	HRESULT l_Status = S_OK;

	for (size_t l = 0; nullptr == p_rpObject && l < m_PPQInputs.size(); ++l)
	{
		if (!(m_PPQInputs[l].empty()))
		{
			SVString l_LocalName = GetName();

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
			SVString l_ObjectName = rNameInfo.GetObjectName(Index);

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

HRESULT SVInspectionProcess::ObserverUpdate(const SVAddTool& p_rData)
{
	HRESULT l_Status = S_OK;

	if (nullptr != p_rData.m_pTool)
	{
		// Insert tool in GetToolSet()...
		GetToolSet()->InsertAt(p_rData.m_Index, p_rData.m_pTool);

		// And finally try to create the object...
		if (GetToolSet()->CreateChildObject(p_rData.m_pTool, SvOi::SVMFResetObject))
		{
			BuildValueObjectMap();

			// Set default formulas in newly instantiated tool...
			SvStl::MessageContainerVector ErrorMessages;
			p_rData.m_pTool->SetDefaultFormulas(&ErrorMessages);
			for (SvStl::MessageContainerVector::iterator iter = ErrorMessages.begin(); ErrorMessages.end() != iter; iter++)
			{
				SvStl::MessageMgrStd message(SvStl::LogAndDisplay);
				message.setMessage(iter->getMessage());
			}

			RunOnce();
		}
		else
		{
			GetToolSet()->RemoveAt(p_rData.m_Index);

			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::ObserverUpdate(const SVDeleteTool& p_rData)
{
	HRESULT l_Status = S_OK;

	if (nullptr != p_rData.m_pTool)
	{
		// Delete the Tool Object
		GetToolSet()->DestroyChildObject(p_rData.m_pTool);

		// Refresh Lists ( inputs,outputs,results,published )
		SetDefaultInputs();

		//@WARNING[MZA][7.50][02.11.2016] I think to call the next comment is not necessary and do nothing by deleting a tool.
		//Create Child closed object
		SVObjectLevelCreateStruct createStruct;
		createAllObjects(createStruct);

		// Reset all objects
		resetAllObjects();
		BuildValueObjectMap();
	}
	else
	{
		l_Status = E_FAIL;
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

BOOL SVInspectionProcess::RunOnce(SVToolClass *p_psvTool)
{
	BOOL bRet(true);

	SVDataManagerHandle l_Handle;

	SVProductInfoStruct l_Product = LastProductGet(SV_OTHER);

	l_Product.GetResultDataIndex(l_Handle);

	SVStdMapSVToolClassPtrSVInspectionProcessResetStruct l_svToolMap;

	SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemNone;

	bRet = ProcessInputRequests(l_Handle.GetIndex(), eResetItem, l_svToolMap);

	if (SvOi::SVResetItemIP < eResetItem && nullptr != p_psvTool)
	{
		bRet = p_psvTool->GetInspection() == this;

		if (bRet)
		{
			SVRunStatusClass runStatus;

			l_Product.GetResultImageIndex(runStatus.Images, GetUniqueObjectID());

			runStatus.m_lResultDataIndex = l_Handle.GetIndex();

			m_bForceOffsetUpdate = true;

			p_psvTool->Run(runStatus);

			LastProductNotify();
		}
	}
	else
	{
		SingleRunModeLoop(true);
	}

	return bRet;
}

HRESULT SVInspectionProcess::InitializeRunOnce()
{
	HRESULT l_Status = S_OK;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		SVProductInfoStruct l_svProduct;
		SVImageIndexStruct l_svResultImageIndex;
		SVDataManagerHandle	l_ResultDataDMIndexHandle;

		if (!pPPQ->ReserveNextRunOnceProductInfoStruct(l_svProduct, SV_INSPECTION))
		{
			l_Status = E_FAIL;
		}

		HRESULT l_Temp = S_OK;

		l_Temp = SetSourceImagesTo(&l_svProduct);

		if (S_OK == l_Status) { l_Status = l_Temp; }

		l_Temp = ReserveNextResultImage(&l_svProduct, SV_INSPECTION, true);

		if (S_OK == l_Status) { l_Status = l_Temp; }

		l_Temp = l_svProduct.GetResultImageIndex(l_svResultImageIndex, GetUniqueObjectID());

		if (S_OK == l_Status) { l_Status = l_Temp; }

		l_Temp = l_svProduct.GetResultDataIndex(l_ResultDataDMIndexHandle);

		if (S_OK == l_Status) { l_Status = l_Temp; }

		if (!(RunInspection(l_ResultDataDMIndexHandle.GetIndex(), l_svResultImageIndex, &l_svProduct, false)) && S_OK == l_Status)
		{
			l_Status = E_FAIL;
		}

		l_Temp = LastProductUpdate(&l_svProduct);

		if (S_OK == l_Status) { l_Status = l_Temp; }
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVInspectionProcess::ProcessInputRequests(long Bucket, bool &rForceOffsetUpdate)
{
	SVStdMapSVToolClassPtrSVInspectionProcessResetStruct l_svToolMap;

	SvOi::SVResetItemEnum eResetItem = SvOi::SVResetItemNone;

	BOOL l_bOk = ProcessInputRequests(Bucket, eResetItem, l_svToolMap);

	rForceOffsetUpdate |= (eResetItem < SvOi::SVResetItemNone);

	return l_bOk;
}

BOOL SVInspectionProcess::ProcessInputRequests(long Bucket, SvOi::SVResetItemEnum &rResetItem, SVStdMapSVToolClassPtrSVInspectionProcessResetStruct &rToolMap)
{
	bool bRet = true;
	long l;

	SVInputRequestInfoStructPtr l_pInputRequest;

	// Process all input requests
	if (!m_InputRequests.Lock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17025_ErrorLockingInputRequests);
		DebugBreak();
	}

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

			SVString Value;

			if (VT_ARRAY == (l_pInputRequest->m_Value.vt & VT_ARRAY))
			{
				SVSAFEARRAY::SVBounds l_Bounds;
				SVSAFEARRAY l_SafeArray = l_pInputRequest->m_Value;

				l_SafeArray.GetBounds(l_Bounds);

				if (1 == l_SafeArray.size())
				{
					_variant_t l_Variant;

					l_SafeArray.GetElement(l_Bounds[0].lLbound, l_Variant);

					Value = SvUl_SF::createSVString(l_Variant);
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
						Value += SvUl_SF::Format(_T("`%s`"), SvUl_SF::createSVString(l_Variant).c_str());
					}
				}
			}
			else
			{
				Value = SvUl_SF::createSVString(l_pInputRequest->m_Value);
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
				Bucket = (0 < Bucket) ? Bucket : 1;

				bool bResetObject = ObjectRef.getValueObject()->ResetAlways();

				// Value objects don't accept each different type for sets
				// so convert to the appropriate type
				if (SVFileNameValueObjectClass* pFileNameObj = dynamic_cast<SVFileNameValueObjectClass*> (ObjectRef.getObject()))
				{
					if (ObjectRef.isArray() && ObjectRef.isEntireArray())
					{
						hrSet = SetObjectArrayValues<CFile>(ObjectRef, Bucket, Value, bResetObject);
					}
					else
					{
						CFileStatus rStatus;

						BOOL bOk = CFile::GetStatus(Value.c_str(), rStatus);
						if (bOk)
						{
							bOk = 0L <= rStatus.m_size;
						}// end if

						if (bOk)
						{
							if (!bResetObject)
							{
								SVString PrevValue;
								
								hrSet = pFileNameObj->GetValue(PrevValue, -1, ObjectRef.ArrayIndex());

								bResetObject = (PrevValue != Value);
							}

							hrSet = pFileNameObj->SetValue(Value, Bucket, ObjectRef.ArrayIndex());
						}// end if
					}
				}// end if SVFileNameValueObjectClass
				else if (SVStringValueObjectClass* pStringValueObj = dynamic_cast<SVStringValueObjectClass*> (ObjectRef.getObject()))
				{
					if (ObjectRef.isArray() && ObjectRef.isEntireArray())
					{
						hrSet = SetObjectArrayValues<SVString>(ObjectRef, Bucket, Value, bResetObject);
					}
					else
					{
						if (!bResetObject)
						{
							SVString PrevValue;

							hrSet = pStringValueObj->GetValue(PrevValue, -1, ObjectRef.ArrayIndex());

							bResetObject = (PrevValue != Value);
						}

						hrSet = pStringValueObj->SetValue(Value, Bucket, ObjectRef.ArrayIndex());
					}
				}
				else if (SVBoolValueObjectClass* pBoolValueObj = dynamic_cast<SVBoolValueObjectClass*> (ObjectRef.getObject()))
				{
					BOOL NewValue(false);

					// Convert to the appropriate type of value
					if (0 == SvUl_SF::CompareNoCase(Value, _T("TRUE")))
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

						hrSet = pBoolValueObj->GetValue(PrevValue, -1, ObjectRef.ArrayIndex());

						// eventually use CompareWithCurrentValueImpl
						bResetObject = NewValue != PrevValue;
					}

					hrSet = pBoolValueObj->SetValue(NewValue, Bucket, ObjectRef.ArrayIndex());
				}// end else if SVBoolValueObjectClass
				else if (SVVariantValueObjectClass* pvValueObject = dynamic_cast <SVVariantValueObjectClass*> (ObjectRef.getObject()))
				{
					// Convert to the appropriate type of value

					// eventually use CompareWithCurrentValueImpl
					bResetObject = true;

					if (ObjectRef.isArray())
					{
						hrSet = pvValueObject->SetValueKeepType(Value.c_str(), Bucket, ObjectRef.ArrayIndex());
					}
					else
					{
						hrSet = pvValueObject->SetValueKeepType( Value.c_str(), Bucket);
					}
				}// end else if SVVariantValueObjectClass
				else if (SVEnumerateValueObjectClass* pEnumerateValueObj = dynamic_cast <SVEnumerateValueObjectClass*> (ObjectRef.getObject()))
				{
					// Convert to the appropriate type of value
					bool l_bTempReset = false;

					if (!bResetObject)
					{
						//Reset if values not the same
						l_bTempReset = !pEnumerateValueObj->CompareWithCurrentValue(Value);
					}

					if (S_OK == (hrSet = pEnumerateValueObj->SetValue(atol(Value.c_str()), 1, ObjectRef.ArrayIndex())))
					{
						hrSet = pEnumerateValueObj->SetValue(atol(Value.c_str()), Bucket, ObjectRef.ArrayIndex());
					}
					else
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

								hrSet = pEnumerateValueObj->SetValue(EnumValue, Bucket, ObjectRef.ArrayIndex());
							}
						}
					}

					bResetObject = l_bTempReset;
				}// end else if SVEnumerateValueObjectClass
				else if (SVDoubleValueObjectClass* pDoubleValueObj = dynamic_cast <SVDoubleValueObjectClass*> (ObjectRef.getObject()))
				{
					if (ObjectRef.isArray() && ObjectRef.isEntireArray())
					{
						hrSet = SetObjectArrayValues<double>(ObjectRef, Bucket, Value.c_str(), bResetObject);
					}
					else
					{
						double NewValue = atof(Value.c_str());
						if (!bResetObject)
						{
							double PrevValue;

							hrSet = pDoubleValueObj->GetValue(PrevValue, -1, ObjectRef.ArrayIndex());

							// eventually use CompareWithCurrentValueImpl
							bResetObject = PrevValue != NewValue;
						}

						hrSet = pDoubleValueObj->SetValue(NewValue, Bucket, ObjectRef.ArrayIndex());
					}
				}// end else if
				else // Long, DWord, Byte
				{
					if (!bResetObject)
					{
						_variant_t PrevValue;

						hrSet = ObjectRef.getValueObject()->getValue(PrevValue, -1, ObjectRef.ArrayIndex());

						SVString strNewValue(Value);
						SvUl_SF::MakeLower(strNewValue);
						TCHAR* p = nullptr;
						long lValue = 0;
						if (SVString::npos != strNewValue.find(_T('x')))
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

					hrSet = ObjectRef.getValueObject()->setValue(Value, Bucket, ObjectRef.ArrayIndex());
				}// end else if

				if (SvOi::SVResetItemIP != rResetItem && bResetObject && SvOi::SVResetItemNone > ObjectRef.getValueObject()->getResetItem())
				{
					SVToolClass* l_psvTool = dynamic_cast<SVToolClass*>(ObjectRef.getObject()->GetAncestor(SVToolObjectType));

					if (nullptr != l_psvTool)
					{
						SvOi::SVResetItemEnum eToolResetItem = rToolMap[l_psvTool].SetResetData(ObjectRef.getValueObject()->getResetItem(), ObjectRef.getObject());

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
						rToolMap.clear();
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
					SVToolClass *l_psvTool = dynamic_cast<SVToolClass *>(m_pCurrentToolset->GetAt(l));

					if (nullptr != l_psvTool)
					{
						if (rToolMap.find(l_psvTool) != rToolMap.end())
						{
							if (rToolMap[l_psvTool].m_ObjectSet.empty())
							{
								bRet &= l_psvTool->resetAllObjects();
							}
							else
							{
								SVObjectPtrSet::iterator l_oIter;

								l_oIter = rToolMap[l_psvTool].m_ObjectSet.begin();

								while (l_oIter != rToolMap[l_psvTool].m_ObjectSet.end())
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

				SVObjectManagerClass::Instance().UpdateObservers(SVString(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
			}
		}// end if ( nullptr != m_pCurrentToolset )
	}// end while( m_lInputRequestMarkerCount > 0L )

	if (!m_InputRequests.Unlock())
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17026_ErrorUnlockingInputRequests);
		DebugBreak();
	}

	return bRet;
}

BOOL SVInspectionProcess::ProcessInputImageRequests(SVProductInfoStruct *p_psvProduct)
{
	BOOL l_bOk = TRUE;

	if (!m_InputImageRequests.IsEmpty())
	{
		long l_lSize = 0;

		m_InputImageRequests.GetSize(l_lSize);

		for (long l = 0; l < l_lSize; l++)
		{
			SVInputImageRequestInfoStructPtr l_pInRequest;

			if (m_InputImageRequests.RemoveHead(&l_pInRequest))
			{
				SVImageClass *pImage = nullptr;

				if (l_pInRequest->m_bUsingCameraName) //not using camera name...
				{
					SVCameraImagePtrSet l_MainImages;

					GetMainImages(l_pInRequest->m_ObjectName.c_str(), l_MainImages);

					SVCameraImagePtrSet::iterator l_Iter = l_MainImages.begin();

					while (nullptr == pImage && l_Iter != l_MainImages.end())
					{
						SVCameraImageTemplate* pCameraImage = (*l_Iter);

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
					SVObjectClass* l_psvObject = nullptr;

					if (GetChildObjectByName(l_pInRequest->m_ObjectName.c_str(), &l_psvObject))
					{
						pImage = dynamic_cast<SVImageClass*>(l_psvObject);
					}
				}

				l_bOk = nullptr != pImage;

				if (l_bOk)
				{
					SVImageIndexStruct l_svIndex;

					if (SVMainImageClass* l_psvMainImage = dynamic_cast<SVMainImageClass*>(pImage))
					{
						SVGuidSVCameraInfoStructMap::iterator l_svIter;

						l_svIter = p_psvProduct->m_svCameraInfos.find(l_psvMainImage->GetCamera()->GetUniqueObjectID());

						if (l_svIter != p_psvProduct->m_svCameraInfos.end())
						{
							SVCameraInfoStruct& rCameraInfo = l_svIter->second;

							l_svIndex.m_CameraDMIndexHandle.Assign(rCameraInfo.GetSourceImageDMIndexHandle(), SV_INSPECTION);
						}
					}
					else
					{
						p_psvProduct->GetResultImageIndex(l_svIndex, GetUniqueObjectID());
					}

					l_bOk = !l_svIndex.IsNull();

					if (l_bOk)
					{
						l_bOk = pImage->SetImageHandleIndex(l_svIndex);

						if (l_bOk)
						{
							SVSmartHandlePointer l_ImageHandle;

							if (pImage->GetImageHandle(l_ImageHandle) && !(l_ImageHandle.empty()) && !(l_pInRequest->m_ImageHandlePtr.empty()))
							{
								HRESULT l_Code;

								SVImageBufferHandleImage l_MilHandle;
								SVImageBufferHandleImage l_RequestMilHandle;

								l_ImageHandle->GetData(l_MilHandle);
								l_pInRequest->m_ImageHandlePtr->GetData(l_RequestMilHandle);

								l_Code = SVMatroxBufferInterface::CopyBuffer(l_MilHandle.GetBuffer(), l_RequestMilHandle.GetBuffer());
							}
							else
							{
								l_bOk = FALSE;
							}
						}
					}
				}

				l_pInRequest->m_ImageHandlePtr.clear();
			}
			else
			{
				l_bOk = FALSE;
			}
		}// end for
	}

	return l_bOk;
}

HRESULT SVInspectionProcess::ReserveNextResultImage(SVProductInfoStruct *p_pProduct, SVDataManagerLockTypeEnum p_eLockType, bool p_ClearOtherInspections)
{
	// We will always lock/unlock the published index along with the regular index

	HRESULT l_hrOk = S_FALSE;

	if (p_ClearOtherInspections)
	{
		SVGUIDSVInspectionInfoStructMap::iterator l_Iter = p_pProduct->m_svInspectionInfos.begin();

		while (l_Iter != p_pProduct->m_svInspectionInfos.end())
		{
			l_Iter->second.ClearIndexes();

			++l_Iter;
		}
	}

	SVInspectionInfoStruct& l_svIPInfo = p_pProduct->m_svInspectionInfos[GetUniqueObjectID()];

	l_hrOk = GetNextAvailableIndexes(l_svIPInfo, p_eLockType);

	return l_hrOk;
}

BOOL SVInspectionProcess::DisconnectToolSetMainImage()
{
	BOOL bRet = true;

	SVCameraImagePtrSet::iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		SVCameraImageTemplate* l_pCameraImage = (*l_Iter);

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

	return bRet;
}

BOOL SVInspectionProcess::ConnectToolSetMainImage()
{
	BOOL bRet = true;

	m_svReset.AddState(SvOi::SVResetAutoMoveAndResize);

	SVCameraImagePtrSet::iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		SVCameraImageTemplate* l_pCameraImage = (*l_Iter);

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

	m_svReset.RemoveState(SvOi::SVResetAutoMoveAndResize);

	return bRet;
}

BOOL SVInspectionProcess::RemoveCamera(const SVString& rCameraName)
{
	BOOL bRet = TRUE;

	if (rCameraName == m_ToolSetCameraName)
	{
		SVVirtualCamera* pCamera = GetFirstPPQCamera();

		if (nullptr != pCamera)
		{
			m_ToolSetCameraName = pCamera->GetName();
		}
		else
		{
			m_ToolSetCameraName.clear();
		}
	}

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		if (nullptr != *l_Iter)
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if ((nullptr != pCamera) && (rCameraName == pCamera->GetName()))
			{
				(*l_Iter)->UpdateCameraImage(m_ToolSetCameraName.c_str());
			}
		}

		++l_Iter;
	}

	return bRet;
}

BOOL SVInspectionProcess::IsNewDisableMethodSet()
{
	return m_bNewDisableMethod;
}

void SVInspectionProcess::SetNewDisableMethod(BOOL bNewDisableMethod)
{
	m_bNewDisableMethod = bNewDisableMethod;
}

long SVInspectionProcess::GetEnableAuxiliaryExtent() const
{
	return m_lEnableAuxiliaryExtents;
}

void SVInspectionProcess::SetEnableAuxiliaryExtent(long p_lEnableAuxiliaryExtents)
{
	m_lEnableAuxiliaryExtents = p_lEnableAuxiliaryExtents;
}

HRESULT SVInspectionProcess::CollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructVector& p_rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	p_rMultiLineArray.clear();

	SVToolSetClass *l_pToolSet = GetToolSet();

	if (nullptr != l_pToolSet)
	{
		l_Status = l_pToolSet->CollectOverlays(p_pImage, p_rMultiLineArray);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::AddInputImageRequestByCameraName(const SVString& rCameraName, const SVString& rFileName)
{
	HRESULT hrOk = S_OK;

	SVImageInfoClass svInfo;
	SVSmartHandlePointer svHandleStruct;
	SVInputImageRequestInfoStructPtr l_pInRequest;
	try
	{
		l_pInRequest = new SVInputImageRequestInfoStruct;
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
		SVImageProcessingClass::LoadImageBuffer(rFileName.c_str(), l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr);

		AddInputImageRequest(l_pInRequest);
	}

	return hrOk;
}

SVProductInfoStruct SVInspectionProcess::LastProductGet(SVDataManagerLockTypeEnum p_LockType) const
{
	SVProductInfoStruct l_Temp;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign(m_LastRunLockPtr.get());

	l_Temp.Assign(m_svLastRunProduct, p_LockType);

	if (m_LastRunProductNULL)
	{
		l_Temp.Reset();
	}

	return l_Temp;
}

HRESULT SVInspectionProcess::LastProductUpdate(SVProductInfoStruct *p_psvProduct)
{
	HRESULT l_hrOk = S_OK;

	m_LastRunProductNULL = nullptr == p_psvProduct;

	if (!m_LastRunProductNULL)
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		l_AutoLock.Assign(m_LastRunLockPtr.get());

		l_hrOk = m_svLastRunProduct.Assign(*p_psvProduct, SV_LAST_INSPECTED);

		m_svLastRunProduct.SetProductComplete();
	}

	return l_hrOk;
}

HRESULT SVInspectionProcess::LastProductCopySourceImagesTo(SVProductInfoStruct *p_psvProduct)
{
	HRESULT Result = S_OK;

	if (nullptr != p_psvProduct)
	{
		SVProductInfoStruct l_Product = LastProductGet(SV_OTHER);

		SVGuidSVCameraInfoStructMap::iterator Iter(p_psvProduct->m_svCameraInfos.begin());

		for (; Iter != p_psvProduct->m_svCameraInfos.end(); ++Iter)
		{
			SVGuidSVCameraInfoStructMap::iterator LastIter(l_Product.m_svCameraInfos.find(Iter->first));

			if (LastIter != l_Product.m_svCameraInfos.end())
			{
				BOOL Copied(false);
				SVVirtualCamera* pCamera(dynamic_cast<SVVirtualCamera*> (SvOi::getObject(Iter->first)));
				if (nullptr != pCamera)
				{
					Copied = pCamera->CopyValue(LastIter->second.GetSourceImageDMIndexHandle(), Iter->second.GetSourceImageDMIndexHandle());
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

HRESULT SVInspectionProcess::SetSourceImagesTo(SVProductInfoStruct *p_psvProduct)
{
	HRESULT Result(S_OK);

	if (nullptr != p_psvProduct)
	{
		SVGuidSVCameraInfoStructMap::iterator Iter(p_psvProduct->m_svCameraInfos.begin());

		for (; Iter != p_psvProduct->m_svCameraInfos.end(); ++Iter)
		{
			bool IndexReserved(false);
			SVVirtualCamera* pCamera(dynamic_cast<SVVirtualCamera*> (SvOi::getObject(Iter->first)));
			if (nullptr != pCamera)
			{
				IndexReserved = pCamera->ReserveImageHandleIndex(Iter->second.GetSourceImageDMIndexHandle());
			}
			if (!IndexReserved)
			{
				Result = E_FAIL;
			}
		}
	}

	return Result;
}

HRESULT SVInspectionProcess::RestoreCameraImages()
{
	HRESULT l_svOk = S_OK;

	SVCameraImagePtrSet::iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		SVCameraImageTemplate* l_pMainImage = (*l_Iter);

		if (nullptr != l_pMainImage)
		{
			::KeepPrevError(l_svOk, l_pMainImage->RestoreMainImage(this));
			}
			else
			{
				l_svOk = S_FALSE;
			}
		++l_Iter;
	}

	::KeepPrevError(l_svOk, RunOnce());

	return l_svOk;
}

HRESULT SVInspectionProcess::GetNextAvailableIndexes(SVInspectionInfoStruct& p_rInspectionInfo, SVDataManagerLockTypeEnum p_LockType) const
{
	HRESULT l_Status = S_OK;

	if (m_pResultImageCircleBuffer.empty())
	{
		l_Status = CreateResultImageIndexManager();
	}

	if (S_OK == l_Status)
	{
		l_Status = TheSVDataManager.GetNextAvailableBufferIndex(m_pResultImageCircleBuffer, p_LockType, p_rInspectionInfo.m_ResultImageDMIndexHandle);

#ifdef _DEBUG
		if (S_OK != l_Status)
		{
			DumpDMInfo(GetName());
		}
#endif
	}

	return l_Status;
}

void SVInspectionProcess::DumpDMInfo(LPCTSTR p_szName) const
{
	if (!(m_pResultImageCircleBuffer.empty()))
	{
		m_pResultImageCircleBuffer->Dump(p_szName);
	}
}

bool SVInspectionProcess::CheckAndResetConditionalHistory()
{
	bool Result(false);

	SVTaskObjectListClass* pToolSet = static_cast <SVTaskObjectListClass*> (m_pCurrentToolset);
	if (nullptr != pToolSet)
	{
		SVObjectReferenceVector vecObjects;
		pToolSet->GetOutputListFiltered(vecObjects, SvOi::SV_CH_CONDITIONAL | SvOi::SV_CH_VALUE, false);
		if (0 < vecObjects.size())
		{
			SVObjectReferenceVector::iterator iter;
			for (iter = vecObjects.begin(); iter != vecObjects.end(); ++iter)
			{
				iter->SetObjectAttributesSet(SvOi::SV_CH_CONDITIONAL | SvOi::SV_CH_VALUE, SvOi::SetAttributeType::RemoveAttribute);
			}
			Result = true;
		}

		SVImageListClass listImages;
		pToolSet->GetImageList(listImages, SvOi::SV_CH_IMAGE);
		int NumberOfImages = listImages.GetSize();
		if (0 < NumberOfImages)
		{
			for (int i = 0; i < NumberOfImages; i++)
			{
				SVObjectReference refImage(listImages.GetAt(i));
				refImage.getObject()->SetObjectAttributesSet(SvOi::SV_CH_IMAGE, SvOi::SetAttributeType::RemoveAttribute);
			}
			Result = true;
		}

	}
	return Result;
}

bool SVInspectionProcess::IsColorCamera() const
{
	bool Result(false);
	SVVirtualCamera* pCamera(GetFirstCamera());

	if (nullptr != pCamera)
	{
		Result = pCamera->IsColor();
	}

	return Result;
}

SVVirtualCamera* SVInspectionProcess::GetFirstCamera() const
{
	SVVirtualCamera* pCamera(nullptr);

	if (!(m_ToolSetCameraName.empty()))
	{
		pCamera = dynamic_cast<SVVirtualCamera*>(SVObjectManagerClass::Instance().GetObjectCompleteName(m_ToolSetCameraName.c_str()));
	}

	if (nullptr == pCamera)
	{
		pCamera = GetFirstPPQCamera();
	}

	return pCamera;
}

SVVirtualCamera* SVInspectionProcess::GetFirstPPQCamera() const
{
	SVVirtualCamera* pCamera(nullptr);

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		SVVirtualCameraMap l_Cameras;

		pPPQ->GetVirtualCameras(l_Cameras);

		SVVirtualCameraMap::const_iterator l_Iter = l_Cameras.begin();

		if (l_Iter != l_Cameras.end())
		{
			pCamera = l_Iter->second;
		}
	}

	return pCamera;
}

HRESULT SVInspectionProcess::GetPPQCameras(SVVirtualCameraPtrSet& p_rCameras) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pPPQ)
	{
		SVVirtualCameraMap l_Cameras;

		pPPQ->GetVirtualCameras(l_Cameras);

		SVVirtualCameraMap::const_iterator l_Iter = l_Cameras.begin();

		while (l_Iter != l_Cameras.end())
		{
			if (nullptr != l_Iter->second)
			{
				p_rCameras.insert(l_Iter->second);
			}

			++l_Iter;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetCameras(SVVirtualCameraPtrSet& p_rCameras) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		if (nullptr != (*l_Iter))
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if (nullptr != pCamera)
			{
				p_rCameras.insert(pCamera);
			}
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetCamerasForLut(SVVirtualCameraPtrSet& p_rCameras) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	for (SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin(); l_Iter != m_CameraImages.end(); ++l_Iter)
	{
		if (nullptr != (*l_Iter))
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if (nullptr != pCamera)
			{
				if (!pCamera->IsFileAcquisition() && nullptr != pCamera->GetAcquisitionDevice())
				{
					SVLut lut;
					HRESULT hr = pCamera->GetLut(lut);
					if (S_OK == hr && lut.NumBands() > 0)
					{
						p_rCameras.insert(pCamera);
					}
				}
			}
		}
	}
	return l_Status;
}

HRESULT SVInspectionProcess::GetCamerasForLightReference(SVVirtualCameraPtrSet& p_rCameras) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	for (SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin(); l_Iter != m_CameraImages.end(); ++l_Iter)
	{
		if (nullptr != (*l_Iter))
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if (nullptr != pCamera)
			{
				if (!pCamera->IsFileAcquisition() && nullptr != pCamera->GetAcquisitionDevice())
				{
					SVLightReference lightReference;
					HRESULT hr = pCamera->GetLightReference(lightReference);
					if (S_OK == hr && lightReference.NumBands() > 0)
					{
						p_rCameras.insert(pCamera);
					}
				}
			}
		}
	}
	return l_Status;
}

HRESULT SVInspectionProcess::AddSharedCamera(SVVirtualCamera* pCamera)
{
	HRESULT l_Status = S_OK;

	SVPPQObject* pPPQ = GetPPQ();

	if (nullptr != pCamera && nullptr != pPPQ)
	{
		pPPQ->AddSharedCamera(pCamera);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetMainImages(const SVString& rCameraName, SVCameraImagePtrSet& rMainImages) const
{
	HRESULT l_Status = S_OK;

	rMainImages.clear();

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while (l_Iter != m_CameraImages.end())
	{
		if (nullptr != (*l_Iter))
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if ((nullptr != pCamera) && (rCameraName == pCamera->GetName()))
			{
				rMainImages.insert(*l_Iter);
			}
		}

		++l_Iter;
	}

	return l_Status;
}

void SVInspectionProcess::UpdateSharedMemoryFilters(const SVMonitorList& p_rMonitorList)
{
	m_MonitorListQueue.clear();
	m_MonitorListQueue.AddTail(p_rMonitorList);
	m_AsyncProcedure.Signal(nullptr);
}

HRESULT SVInspectionProcess::RemoveImage(SVImageClass* pImage)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pImage)
	{
		SVRemoveImages l_Data;

		l_Data.m_Images.insert(pImage->GetUniqueObjectID());

		SVObjectManagerClass::Instance().UpdateObservers(SVString(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
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

BOOL SVInspectionProcess::CreateObject(SVObjectLevelCreateStruct* PCreateStruct)
{
	BOOL l_bOk = SVObjectClass::CreateObject(PCreateStruct);

	SVInspectionLevelCreateStruct createStruct;

	createStruct.OwnerObjectInfo = this;
	createStruct.InspectionObjectInfo	= this;

	l_bOk = l_bOk && m_pCurrentToolset->createAllObjects(createStruct);

	m_isCreated = l_bOk;

	return m_isCreated;
}

bool SVInspectionProcess::ResetObject(SvStl::MessageContainerVector *pErrorMessages)
{
	bool Result = __super::ResetObject(pErrorMessages);

	if (Result && !m_pCurrentToolset->IsCreated())
	{
		Result = false;
		if (nullptr != pErrorMessages)
		{
			SvStl::MessageContainer Msg(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_ToolsetNotCreated, SvStl::SourceFileParams(StdMessageParams), 0, GetUniqueObjectID());
			pErrorMessages->push_back(Msg);
		}
	}

	BuildValueObjectMap();

	m_bForceOffsetUpdate = true;

	return Result;
}

void SVInspectionProcess::SetInvalid()
{
	SVObjectClass::SetInvalid();

	if (GetToolSet())
	{
		GetToolSet()->SetInvalid();
	}
}

int SVInspectionProcess::UpdateMainImagesByProduct(SVProductInfoStruct* p_psvProduct)
{
	int Result(0);

	SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

	while (l_ImageIter != m_CameraImages.end())
	{
		SVCameraImageTemplate* l_pImage = (*l_ImageIter);

		if (nullptr != l_pImage)
		{
			SVVirtualCamera* pCamera = l_pImage->GetCamera();

			if (nullptr != pCamera)
			{
				SVGuidSVCameraInfoStructMap::const_iterator Iter;

				Iter = p_psvProduct->m_svCameraInfos.find(pCamera->GetUniqueObjectID());

				if (Iter != p_psvProduct->m_svCameraInfos.end())
				{
					if (Iter->second.GetIndex() < 0)
					{
						Result = SvStl::Err_25043_InvalidSourceIndex;
					}

					SVImageIndexStruct l_svIndex;

					l_svIndex.m_CameraDMIndexHandle.Assign(Iter->second.GetSourceImageDMIndexHandle(), SV_INSPECTION);

					if (!(l_pImage->SetImageHandleIndex(l_svIndex)))
					{
						Result = SvStl::Err_25042_SetImageHandleIndex;
					}
				}
				else
				{
					l_pImage->ResetImageIndex();

					Result = SvStl::Err_25044_CameraNotFound;
				}
			}
			else
			{
				Result = SvStl::Err_25045_InvalidCamera;
			}
		}
		else
		{
			Result = SvStl::Err_25046_InvalidImage;
		}

		++l_ImageIter;
	}

	return Result;
}

HRESULT SVInspectionProcess::OnlyCopyForward(SVRunStatusClass& rRunStatus)
{
	HRESULT hRet = S_OK;

	SvOi::IValueObjectPtrSet::iterator ValueIter(m_ValueObjectSet.begin());
	for (; m_ValueObjectSet.end() != ValueIter; ++ValueIter)
	{
		(*ValueIter)->CopyLastSetValue(rRunStatus.m_lResultDataIndex);
	}

	SVImageClassPtrSet::iterator ImageIter(m_ImageObjectSet.begin());
	for (; m_ImageObjectSet.end() != ImageIter; ++ImageIter)
	{
		(*ImageIter)->CopyImageTo(rRunStatus.Images);
	}


	return hRet;
}

bool SVInspectionProcess::Run(SVRunStatusClass& rRunStatus)
{
	// Validate IPDoc's values...
	bool retVal = IsValid();

	if (retVal)
	{
		if( !IsNewDisableMethodSet() )
		{
			OnlyCopyForward(rRunStatus);
		}// end if

		// Run the Toolset
		unsigned long l_state = m_runStatus.GetState();
		if (GetToolSet() && l_state != SVRunStatusClass::SV_INVALID)
		{
			retVal &= GetToolSet()->Run(rRunStatus);
		}
		else
		{
			rRunStatus.SetInvalid();
			rRunStatus.SetInspectionStarted(false);
			if (SVRunStatusClass::SV_INVALID == m_runStatus.GetState())
			{
				SvStl::MessageMgrStd Exception(SvStl::LogOnly);
				Exception.setMessage(SVMSG_SVO_37_INVALID_RUN_STATUS, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));
			}
		}
	}
	else
	{
		rRunStatus.SetInvalid();
		rRunStatus.SetInspectionStarted(false);
	}

	return retVal;
}

BOOL SVInspectionProcess::RunInspection(long lResultDataIndex, SVImageIndexStruct svResultImageIndex, SVProductInfoStruct *pProduct, bool p_UpdateCounts)
{
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
	double del = SvTl::setReferenceTime();
	SVString DebugString = SvUl_SF::.Format(_T("!\n!!Reset, %7.1lf: SVInspectionProcess::RunInspection(), del = %7.1lf\n"), SvTl::GetRelTimeStamp(), del);
	::OutputDebugString(DebugString.c_str());
#endif

	BOOL l_bOk = FALSE;
	BOOL l_bInputRequest = FALSE;
	BOOL l_bImageRequest = FALSE;
	BOOL l_bUpdateMainImage = FALSE;
	BOOL l_bRestMainImage = FALSE;
	SvStl::MessageMgrStd Exception(SvStl::LogOnly);

	// Get the info struct for this inspection
	SVInspectionInfoStruct& l_rIPInfo = pProduct->m_svInspectionInfos[GetUniqueObjectID()];

	m_runStatus.ResetRunStateAndToolSetTimes();

	m_runStatus.m_PreviousTriggerTime = m_runStatus.m_CurrentTriggerTime;
	m_runStatus.m_CurrentTriggerTime  = pProduct->oTriggerInfo.m_BeginProcess;

	m_runStatus.m_lResultDataIndex = lResultDataIndex;
	m_runStatus.Images = svResultImageIndex;
	m_runStatus.m_lTriggerCount = pProduct->oTriggerInfo.lTriggerCount;
	m_runStatus.m_UpdateCounters = p_UpdateCounts;

	if (pProduct->IsAlive())
	{
		// Process all input requests
		if (!ProcessInputRequests(lResultDataIndex, m_bForceOffsetUpdate))
		{
			Exception.setMessage(SVMSG_SVO_38_INPUT_REQUEST_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));

			l_bInputRequest = TRUE;
			m_runStatus.SetInvalid();  //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		if (!ProcessInputImageRequests(pProduct))
		{
			Exception.setMessage(SVMSG_SVO_39_IMAGE_REQUEST_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams));

			l_bImageRequest = TRUE;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		int MainImageStatus = UpdateMainImagesByProduct(pProduct);

		if (0 != MainImageStatus)
		{
			Exception.setMessage(SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvStl::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), MainImageStatus);

			l_bUpdateMainImage = TRUE;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}// end if
	}
	else
	{
		//Product !bAlive
		SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while (l_ImageIter != m_CameraImages.end())
		{
			SVCameraImageTemplate* l_pImage = (*l_ImageIter);

			if (nullptr != l_pImage)
			{
				l_pImage->ResetImageIndex();
			}

			++l_ImageIter;
		}

		m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		//do a copy forward, and NOT Run();
		l_bRestMainImage = TRUE;
	}

	// Tool set is disabled! // Put out tool set disabled...
	if (!GetToolSet()->IsEnabled())
	{
		l_rIPInfo.oInspectedState = PRODUCT_INSPECTION_DISABLED;
	}// end if

	l_rIPInfo.m_BeginToolset = SvTl::GetTimeStamp();
	if (!l_rIPInfo.m_BeginToolset)
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17027_ErrorGettingTimeStamp);
		DebugBreak();
	}

	if (!l_bInputRequest && !l_bImageRequest && !l_bUpdateMainImage && !l_bRestMainImage)
	{
		l_bOk = Run(m_runStatus);
	}
	else
	{
		OnlyCopyForward(m_runStatus);
	}

	m_bForceOffsetUpdate = false;

	l_rIPInfo.m_EndToolset = SvTl::GetTimeStamp();
	if (!l_rIPInfo.m_EndToolset)
	{
		SvStl::MessageMgrStd e(SvStl::LogOnly);
		e.setMessage(SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvStl::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams), SvStl::Err_17028_ErrorGettingTimeStamp);
		DebugBreak();
	}

	if (m_runStatus.IsValid() && !m_runStatus.IsCriticalFailure())
	{
		l_rIPInfo.oInspectedState = PRODUCT_INSPECTION_RUN;
	}
	else
	{
		l_rIPInfo.oInspectedState = PRODUCT_NOT_ACKNOWLEDGED;
	}

	l_rIPInfo.m_ToolSetAvgTime = m_runStatus.m_ToolSetAvgTime;
	l_rIPInfo.m_ToolSetEndTime = m_runStatus.m_ToolSetEndTime;

	SVPPQObject *pPPQ = GetPPQ();

	m_runStatus.m_WorkloadInfoRsc = (nullptr != pPPQ) ? pPPQ->GetMostRecentWorkLoadInformation() : ProductWorkloadInformation();

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

BOOL SVInspectionProcess::SetObjectDepth(int NewObjectDepth)
{
	BOOL l_bOk = SVObjectClass::SetObjectDepth(NewObjectDepth);

	l_bOk &= nullptr != GetToolSet() && GetToolSet()->SetObjectDepth(NewObjectDepth);

	return l_bOk;
}

BOOL SVInspectionProcess::SetObjectDepthWithIndex(int NewObjectDepth, int NewLastSetIndex)
{
	BOOL l_bOk = SVObjectClass::SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);

	l_bOk &= nullptr != GetToolSet() && GetToolSet()->SetObjectDepthWithIndex(NewObjectDepth, NewLastSetIndex);

	return l_bOk;
}

BOOL SVInspectionProcess::SetImageDepth(long lDepth)
{
	BOOL l_bOk = SVObjectClass::SetImageDepth(lDepth);

	l_bOk &= nullptr != GetToolSet() && GetToolSet()->SetImageDepth(lDepth);

	return l_bOk;
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

		if (!(l_rIOEntry.m_IOEntryPtr.empty()))
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

	SVObjectManagerClass::Instance().UpdateObservers(SVString(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
}

void SVInspectionProcess::SetName(LPCTSTR StrString)
{
	SVObjectClass::SetName(StrString);

	SVInspectionNameUpdate l_Data(GetName());

	SVObjectManagerClass::Instance().UpdateObservers(SVString(SvO::cInspectionProcessTag), GetUniqueObjectID(), l_Data);
}

HRESULT SVInspectionProcess::RegisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	//! Object is an Image
	if (SVImageObjectType == pObject->GetObjectInfo().m_ObjectTypeInfo.ObjectType)
	{
		SVCameraImageTemplate* pCameraImage = dynamic_cast<SVCameraImageTemplate*> (pObject);
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);

		if (nullptr != pCameraImage)
		{
			m_CameraImages.insert(pCameraImage);
		}
		else
		{
			m_ImageObjectSet.insert(pImage);
		}
		Result = S_OK;
	}
	//! Object is a Value Object
	else if (nullptr != (pValueObject = dynamic_cast<SvOi::IValueObject*> (pObject)))
	{
		m_ValueObjectSet.insert(pValueObject);
		m_mapValueObjects.insert(SVValueObjectMap::value_type(pObject->GetCompleteName(), pObject));
		Result = S_OK;
	}

	return Result;
}

HRESULT SVInspectionProcess::UnregisterSubObject(SVObjectClass* pObject)
{
	HRESULT Result(E_FAIL);
	SvOi::IValueObject* pValueObject(nullptr);

	//! Object is an Image
	if (SVImageObjectType == pObject->GetObjectInfo().m_ObjectTypeInfo.ObjectType)
	{
		SVCameraImageTemplate* pCameraImage = dynamic_cast<SVCameraImageTemplate*>(pObject);
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);

		if (nullptr != pCameraImage)
		{
			if (!(m_CameraImages.empty()))
			{
				m_CameraImages.erase(pCameraImage);
			}
		}
		else if (nullptr != pImage)
		{
			if (!(m_ImageObjectSet.empty()))
			{
				m_ImageObjectSet.erase(pImage);
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
		m_mapValueObjects.get< to >().erase(pObject);

		Result = S_OK;
	}

	return Result;
}

void SVInspectionProcess::SetToolsetImage(const SVString& rToolsetImage)
{
	m_ToolSetCameraName = rToolsetImage;
}

LPCTSTR SVInspectionProcess::GetToolsetImage()
{
	return m_ToolSetCameraName.c_str();
}

SVCameraImageTemplate* SVInspectionProcess::GetToolSetMainImage()
{
	SVCameraImageTemplate* l_pImage = nullptr;

	SVToolSetClass* l_pToolSet = GetToolSet();

	if (nullptr != l_pToolSet)
	{
		l_pImage = dynamic_cast<SVCameraImageTemplate*>(l_pToolSet->getCurrentImage());
	}

	return l_pImage;
}

HRESULT SVInspectionProcess::GetInspectionImage(LPCTSTR Name, SVImageClass*& p_rRefObject)
{
	// Set to Defaults in case of failure
	p_rRefObject = nullptr;

	// Specify that we are looking only for images
	SVObjectTypeInfoStruct imageObjectInfo;
	imageObjectInfo.ObjectType = SVImageObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor(imageObjectInfo);

	SVObjectManagerClass::Instance().VisitElements(l_Visitor, GetUniqueObjectID());

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for (l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter)
	{
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(const_cast<SVObjectClass*>(*l_Iter));

		if (pImage->GetCompleteName() == Name)
		{
			// We found the image by name
			p_rRefObject = pImage;

			return S_OK;
		}
	}
	return E_FAIL;
}

// This method fills the Toolset structure in Shared Memory (for either LastInspected or Rejects )
void SVInspectionProcess::FillSharedData(long sharedSlotIndex, SvSml::SVSharedData& rData, const SVNameObjectMap& rValues, const SVNameObjectMap& rImages, SVProductInfoStruct& rProductInfo, SvSml::SVSharedInspectionWriter& rWriter)
{
	SVProductInfoStruct& l_rProductInfo = rProductInfo;

	SVDataManagerHandle dmHandle = l_rProductInfo.oPPQInfo.m_ResultDataDMIndexHandle;
	long dataIndex = dmHandle.GetIndex();

	SvSml::SVSharedValueVector& rSharedValues = rData.m_Values;
	SvSml::SVSharedImageVector& rSharedImages = rData.m_Images;

	for (SvSml::SVSharedValueVector::iterator it = rSharedValues.begin(); it != rSharedValues.end(); ++it)
	{
		HRESULT hr = S_OK;
		SVString Value;
		bool bItemNotFound = true;
		SVNameObjectMap::const_iterator ValueIter = rValues.find(it->m_ElementName.c_str());
		if (ValueIter != rValues.end())
		{
			const SVObjectReference& ObjectRef = ValueIter->second;
			const SvOi::IValueObject* pValueObject = ObjectRef.getValueObject();

			if (nullptr != pValueObject)
			{
				// for now just a single item (no full array)
				if (!ObjectRef.isEntireArray())
				{
					HRESULT hrGet = pValueObject->getValue(Value, dataIndex, ObjectRef.getValidArrayIndex());
					if (SVMSG_SVO_33_OBJECT_INDEX_INVALID == hrGet || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hrGet)
					{
						hr = hrGet;
						Value = ObjectRef.DefaultValue();
						if (Value.empty())
						{
							Value = SvUl_SF::Format(_T("%i"), -1);
						}
					}
					else if (S_OK != hrGet)	// some generic error; currently should not get here
					{
						hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
						Value = SvUl_SF::Format(_T("%i"), -1); // did not get value. set value to -1
					}
					else if (S_OK == hrGet)
					{
						bItemNotFound = false;
					}
				}
				else // Get Entire Array
				{
					// get all results and put them into a parsable string
					int NumResults = ObjectRef.getValueObject()->getResultSize(dataIndex);
					SVString ArrayValues;
					for (int iArrayIndex = 0; iArrayIndex < NumResults && S_OK == hr; iArrayIndex++)
					{
						SVString Value;
						hr = pValueObject->getValue(Value, dataIndex, iArrayIndex);
						if (S_OK == hr)
						{
							if (iArrayIndex > 0)
							{
								ArrayValues += _T(",");
							}
							ArrayValues += SvUl_SF::Format(_T("`%s`"), Value.c_str());
						}
					}
					if (S_OK == hr)
					{
						bItemNotFound = false;
						Value = ArrayValues;
					}
				}
			}
		}
		if (bItemNotFound)
		{
			hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
		it->SetData(SvSml::SVSharedValue::StringType, Value.c_str(), hr);
	}

	for (SvSml::SVSharedImageVector::iterator it = rSharedImages.begin(); it != rSharedImages.end(); ++it)
	{
		HRESULT hr = S_OK;
		bool bImgNotFound = true;
		SVNameObjectMap::const_iterator imageIter = rImages.find(it->m_ElementName.c_str());
		if (imageIter != rImages.end())
		{
			SVImageClass* pImage = dynamic_cast<SVImageClass*>(imageIter->second.getObject());

			if (nullptr != pImage)
			{
				SVSmartHandlePointer imageHandlePtr;

					pImage->GetImageHandle(imageHandlePtr);

				if (!imageHandlePtr.empty())
				{
					bImgNotFound = false;
					HRESULT hr(S_FALSE);

					SVImageBufferHandleImage FromImageBufferHandle;
					HRESULT Result = imageHandlePtr->GetData(FromImageBufferHandle);
					SVMatroxBuffer& rToBuffer
						= SvSml::SharedMemWriter::Instance().GetImageBuffer(sharedSlotIndex, SvSml::SharedImageStore::last, it->m_ImageStoreIndex, it->m_ImageIndex);
					//Write Image to buffer
					if (rToBuffer.empty() == false && FromImageBufferHandle.empty() == false)
					{
						hr = SVMatroxBufferInterface::CopyBuffer(rToBuffer, FromImageBufferHandle.GetBuffer());
					}

					it->SetImageStoreStatus(hr);
				}

			}
		}
		if (bImgNotFound)
		{
			hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			SVString filename;
			it->SetData(filename.c_str(), hr);
		}
	}
	rData.m_TriggerCount = l_rProductInfo.ProcessCount();
}

SVInspectionProcess::SVSharedMemoryFilters::SVSharedMemoryFilters()
	: m_LastInspectedValues()
	, m_LastInspectedImages()
{
}

void SVInspectionProcess::SVSharedMemoryFilters::clear()
{
	m_LastInspectedValues.clear();
	m_LastInspectedImages.clear();
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

void SVInspectionProcess::SVInspectionTracking::EventStart(const SVString& p_rName)
{
	SvTl::SVTimeStamp l_StartTime = SvTl::GetTimeStamp();
	__int64 l_EventTime = static_cast<__int64>(l_StartTime - m_StartTime);

	m_EventCounts[p_rName].m_StartTime = l_StartTime;

	++(m_EventCounts[p_rName].m_Start[l_EventTime]);
}

void SVInspectionProcess::SVInspectionTracking::EventEnd(const SVString& p_rName)
{
	SvTl::SVTimeStamp l_EndTime = SvTl::GetTimeStamp();
	__int64 l_Duration = static_cast<__int64>(l_EndTime - m_EventCounts[p_rName].m_StartTime);
	__int64 l_EventTime = static_cast<__int64>(l_EndTime - m_StartTime);

	++(m_EventCounts[p_rName].m_End[l_EventTime]);
	++(m_EventCounts[p_rName].m_Duration[l_Duration]);
}
#endif

void SVInspectionProcess::Persist(SVObjectWriter& rWriter)
{
	// until it becomes a task object list...
	rWriter.StartElement(GetObjectName()); // use internal name for node name
	SVObjectClass::Persist(rWriter);

	// Save NewDisableMethod
	_variant_t value;
	value.ChangeType(VT_I4); // was VT_INT...
	value.lVal = IsNewDisableMethodSet();
	rWriter.WriteAttribute(SvXml::CTAG_INSPECTION_NEW_DISABLE_METHOD, value);
	value.Clear();

	// Save EnableAuxillaryExtents
	value = GetEnableAuxiliaryExtent();
	rWriter.WriteAttribute(SvXml::CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, value);
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

long  SVInspectionProcess::GetResultDataIndex() const
{
	return m_runStatus.m_lResultDataIndex;
}

#pragma region IInspectionProcess methods
SvOi::ISelectorItemVectorPtr SVInspectionProcess::GetPPQSelectorList(const UINT Attribute) const
{
	SvOsl::SelectorItemVector *pSelectorList = new SvOsl::SelectorItemVector();
	SvOi::ISelectorItemVectorPtr Result = static_cast<SvOi::ISelectorItemVector*> (pSelectorList);

	SVPPQObject *pPPQ = GetPPQ();
	if (nullptr != pPPQ)
	{
		SVString PpqName(pPPQ->GetName());

		SVObjectReferenceVector ObjectList;
		SVObjectManagerClass::Instance().getTreeList(PpqName, ObjectList, Attribute);

		std::for_each(ObjectList.begin(), ObjectList.end(), [&pSelectorList](const SVObjectReference& rObjectRef)->void
		{
			SvOsl::SelectorItem InsertItem;

			InsertItem.setName(rObjectRef.GetName().c_str());
			InsertItem.setItemKey(rObjectRef.getObject()->GetUniqueObjectID().ToVARIANT());
			if (nullptr != rObjectRef.getValueObject())
			{
				InsertItem.setItemTypeName(rObjectRef.getValueObject()->getTypeName().c_str());
			}
			if (rObjectRef.isArray())
			{
				// add array elements
				int iArraySize = rObjectRef.getValueObject()->getArraySize();
				for (int i = 0; i < iArraySize; i++)
				{
					//We make a copy to be able to set the index rObjectRef is const
					SVObjectReference ArrayObjectRef(rObjectRef);
					ArrayObjectRef.SetArrayIndex(i);
					InsertItem.setLocation(ArrayObjectRef.GetCompleteOneBasedObjectName().c_str());
					InsertItem.setArrayIndex(i);
					pSelectorList->push_back(InsertItem);
				}
			}
			else
			{
				InsertItem.setLocation(rObjectRef.GetCompleteOneBasedObjectName().c_str());
				pSelectorList->push_back(InsertItem);
			}
		});
	}


	SVString InspectionName = GetName();

	SVObjectPtrVector PpqVariables;
	PpqVariables = getPPQVariables();
	std::for_each(PpqVariables.begin(), PpqVariables.end(), [&pSelectorList, &InspectionName, &Attribute](SVObjectClass* pObject)->void
	{
		if (nullptr != pObject)
		{
			//Check if the attribute of the object is allowed
			if (0 != (Attribute & pObject->ObjectAttributesAllowed()))
			{
				SVObjectReference ObjectRef(pObject);
				SvOsl::SelectorItem InsertItem;

				SVString Location(ObjectRef.GetCompleteOneBasedObjectName());
				InsertItem.setName(ObjectRef.GetName().c_str());
				InsertItem.setLocation(Location.c_str());
				//Need to replace the inspection name with the PPQ Variables name
				// Only DIO and Remote Input, but is all that is in this list?
				SvUl_SF::searchAndReplace(Location, InspectionName.c_str(), SvOl::FqnPPQVariables);
				InsertItem.setDisplayLocation(Location.c_str());
				InsertItem.setItemKey(ObjectRef.getObject()->GetUniqueObjectID().ToVARIANT());
				if (nullptr != ObjectRef.getValueObject())
				{
					InsertItem.setItemTypeName(ObjectRef.getValueObject()->getTypeName().c_str());
				}

				pSelectorList->push_back(InsertItem);
			}
		}
	});

	return Result;
}

SvOi::ITaskObject* SVInspectionProcess::GetToolSetInterface() const
{
	return GetToolSet();
}


HRESULT SVInspectionProcess::RunOnce(SvOi::ITaskObject* pTask)
{
	return RunOnce(dynamic_cast<SVToolClass *>(pTask)) ? S_OK : E_FAIL;
}

long SVInspectionProcess::GetLastIndex() const
{
	long lastIndex = 0;
	SVProductInfoStruct l_Product = LastProductGet(SV_INSPECTION);

	if (!(l_Product.empty()))
	{
		SVDataManagerHandle l_Handle;
		l_Product.GetResultDataIndex(l_Handle);
		lastIndex = l_Handle.GetIndex();
	}
	return lastIndex;
}

bool SVInspectionProcess::AddInputRequest(const SVGUID& rGuid, const _variant_t& rValue, long Index)
{
	SVObjectReference ObjectRef(SVObjectManagerClass::Instance().GetObject(rGuid), Index);

	bool Result = AddInputRequest(ObjectRef, rValue);

	return Result;
}

bool SVInspectionProcess::AddInputRequestMarker()
{
	return AddInputRequest(nullptr, SvO::SVTOOLPARAMETERLIST_MARKER);
}
#pragma endregion IInspectionProcess methods

bool SVInspectionProcess::IsEnabledPPQVariable(const SVObjectClass* pObject) const
{
	for (size_t i = 0; i < m_PPQInputs.size(); i++)
	{
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;
		if (ioEntryPtr->m_Enabled)
		{
			if (ioEntryPtr->getObject() == pObject)
			{
				return true;
			}
		}
	}

	return false;
}

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
	std::map< SVString, SVObjectClass* > NameObjectMap;
	SVStringVector PpqVariableNames;

	for (size_t i = 0; i < m_PPQInputs.size(); i++)
	{
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;

		//check if input is enable for this inspection
		if (ioEntryPtr->m_Enabled)
		{
			SVObjectClass* pObject(ioEntryPtr.get()->getObject());
			if (nullptr != pObject)
			{
				SVString Name(pObject->GetCompleteName());
				NameObjectMap[Name] = pObject;
				PpqVariableNames.push_back(Name);
			}
		}
	}// end for

	//Sort the PPQ variable names and return the respective SVObjectClass pointers in order
	std::sort(PpqVariableNames.begin(), PpqVariableNames.end(), SvUl::NaturalStringCompare<SVString>()); // sort strings
	std::for_each(PpqVariableNames.begin(), PpqVariableNames.end(), [&Result, &NameObjectMap](const SVString& rItem)->void
	{
		Result.push_back(NameObjectMap[rItem]);
	}
	);

	return Result;
}

DWORD SVInspectionProcess::GetObjectColor() const
{
	return SV_DEFAULT_WHITE_COLOR;
}



bool   SVInspectionProcess::LoopOverTools(pToolFunc pf, int& counter)
{
	bool ret = false;
	counter = 0;
	SVToolSetClass* pToolSet(nullptr);
	SVTaskObjectClass* pTaskObject(nullptr);
	pToolSet = GetToolSet();
	if (pToolSet)
	{
		ret = true;
		for (int index = 0; index < pToolSet->GetSize(); index++)
		{
			pTaskObject = pToolSet->GetAt(index);
			if (nullptr != pTaskObject)
			{
				int result = pf(pTaskObject);
				if (result < 0)
				{
					return false;
				}
				else
				{
					counter += result;
				}

			}
	}
	}
	return ret;
}

void SVInspectionProcess::getToolMessages(SvStl::MessageContainerInserter& rInserter) const
{
	SVToolSetClass* pToolSet(GetToolSet());

	if (nullptr != pToolSet)
	{
		for (int i = 0; i < pToolSet->GetSize(); i++)
		{
			SVTaskObjectClass* pTaskObject(pToolSet->GetAt(i));
			if (nullptr != pTaskObject)
			{
				const SvStl::MessageContainerVector& rToolMessages(pTaskObject->getErrorMessages());
				std::copy(rToolMessages.begin(), rToolMessages.end(), rInserter);
			}
	}
	}
}

bool SVInspectionProcess::DisconnectObjectInput(SVInObjectInfoStruct* pObjectInInfo)
{
	bool Result = m_publishList.RemovePublishedEntry(pObjectInInfo->GetInputObjectInfo().m_UniqueObjectID);

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
		long l_LastIndex = 1;
		SVProductInfoStruct l_Product = LastProductGet(SV_INSPECTION);

		if (!(l_Product.empty()))
		{
			SVDataManagerHandle l_Handle;

			l_Product.GetResultDataIndex(l_Handle);

			l_LastIndex = l_Handle.GetIndex();
		}

		// Set first object depth...
		pChildObject->SetObjectDepthWithIndex(m_objectDepth, l_LastIndex);
		pChildObject->SetImageDepth(m_lImageDepth);

		SVInspectionLevelCreateStruct createStruct;
		createStruct.OwnerObjectInfo = this;
		createStruct.InspectionObjectInfo = this;

		bool Return = pChildObject->createAllObjects(createStruct);

		if (SvOi::SVMFResetObject == (context & SvOi::SVMFResetObject))
		{
			pChildObject->resetAllObjects();
		}

		if (SvOi::SVMFSetDefaultInputs == (context & SvOi::SVMFSetDefaultInputs))
		{
			SetDefaultInputs();
		}

		if (SvOi::SVMFResetInspection == (context & SvOi::SVMFResetInspection))
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
		SVTaskObjectClass* pTaskObject = dynamic_cast<SVTaskObjectClass*>(pChild);

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

SvOi::IObjectClass* SVInspectionProcess::getFirstObject(const SVObjectTypeInfoStruct& rObjectTypeInfo, bool useFriends, const SvOi::IObjectClass* pRequestor) const
{
	SvOi::IObjectClass* retValue = SVObjectClass::getFirstObject(rObjectTypeInfo, useFriends, pRequestor);

	if (nullptr != GetToolSet() && nullptr == retValue)
	{
		return GetToolSet()->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
	}
	return retValue;
}

void SVInspectionProcess::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
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

			SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.m_UniqueObjectID);
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
					SVTaskObjectListClass* pTaskListOwner = dynamic_cast<SVTaskObjectListClass*>(pOwner);
					SVTaskObjectListClass* pTaskList = dynamic_cast<SVTaskObjectListClass*>(pDuplicateObject);
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
				SVConditionalClass* l_pConditional(nullptr);

				l_pConditional = dynamic_cast<SVConditionalClass*>(pObject);

				if (nullptr != l_pConditional)
				{
					m_pToolSetConditional = l_pConditional;

					m_pToolSetConditional->SetObjectOwner(this);
				}
			}

			return true;
		}
	}
	return false;
}
