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
#include "SVTool.h"
#include "SVOCore/SVImageProcessingClass.h"
#include "SVConditional.h"
#include "SVSVIMStateClass.h"
#include "SVCommandStreamManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVRunControlLibrary\SVRunControlLibrary.h"
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
#include "SVTimerLibrary\SVProfiler.h"
#endif
#pragma endregion Includes

SV_IMPLEMENT_CLASS( SVInspectionProcess, SVInspectionProcessGuid );

HRESULT SVInspectionProcess::ProcessInspection( bool& p_rProcessed, SVProductInfoStruct& p_rProduct )
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_qInspectionsQueue.GetCount() );

	if( p_rProcessed )
	{
		p_rProcessed = ( FALSE != m_qInspectionsQueue.GetHead( &p_rProduct ) );
	}

	if( p_rProcessed )
	{
#ifdef EnableTracking
		m_InspectionTracking.EventStart( _T( "Process Inspections" ) );
#endif
		SVInspectionInfoStruct* pIPInfo = nullptr;

		size_t l_InputXferCount = 0;
		m_bInspecting = true;	// do this before RemoveHead

		// Get the info struct for this inspection
		pIPInfo = &( p_rProduct.m_svInspectionInfos[ GetUniqueObjectID() ] );

		HRESULT hr = ReserveNextResultImage( &p_rProduct, SV_INSPECTION );

		bool l_Process = ( S_OK == hr );

		if( !l_Process )
		{
			SVStringVector msgList;
			msgList.push_back(GetName());
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (hr), SvOi::Tid_InspThreadGetResultImageIndexError, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}

		if( l_Process )
		{
			pIPInfo->m_BeginInspection = SVClock::GetTimeStamp();

			// Copy inputs to Inspection Process's Value objects
			for( size_t iList = 0; iList < m_PPQInputs.size(); iList++ )
			{
				SVIOEntryStruct& pListEntry = m_PPQInputs[iList];
				SVIOEntryStruct& pInEntry = p_rProduct.oPPQInfo.m_InputData[iList];

				if( pInEntry.m_IOEntryPtr->m_Enabled )
				{
					pListEntry.m_EntryValid = pInEntry.m_EntryValid;

					if( pInEntry.m_EntryValid )
					{
						switch ( pInEntry.m_IOEntryPtr->m_ObjectType )
						{
							case IO_DIGITAL_INPUT:
							{
								double l_bValue = 0.0;

								pInEntry.m_IOEntryPtr->m_pValueObject->GetValue( p_rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), l_bValue );
								pListEntry.m_IOEntryPtr->m_pValueObject->SetValue( p_rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), l_bValue );

								++l_InputXferCount;

								break;
							}
							case IO_REMOTE_INPUT:
							default:
							{
								_variant_t l_Value;

								pInEntry.m_IOEntryPtr->m_pValueObject->GetValue( p_rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), l_Value );

								SVVariantValueObjectClass* l_pVariantObject = dynamic_cast< SVVariantValueObjectClass* >( pListEntry.m_IOEntryPtr->m_pValueObject );

								if( nullptr != l_pVariantObject )
								{
									l_pVariantObject->SetValue( p_rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), l_Value.GetVARIANT() );
								}
								else
								{
									double l_Double = static_cast< double >( l_Value );

									pListEntry.m_IOEntryPtr->m_pValueObject->SetValue( p_rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), l_Double );
								}

								++l_InputXferCount;

								break;
							}
						}
					}
					else
					{
						if( !( pListEntry.m_IOEntryPtr->m_Enabled ) )
						{
							++l_InputXferCount;
						}
					}
				}
				else
				{
					if( !( pListEntry.m_IOEntryPtr->m_Enabled ) )
					{
						++l_InputXferCount;
					}
				}
			}

			l_Process = l_Process && ( l_InputXferCount == m_PPQInputs.size() );
			l_Process = l_Process && ( p_rProduct.IsAlive() );

			// Check if tool set is alive and enabled...
			if( !l_Process )
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
				l_svIndex.m_ResultDMIndexHandle.Assign( pIPInfo->m_ResultImageDMIndexHandle, SV_INSPECTION );
				l_svIndex.m_PublishedResultDMIndexHandle.Assign( p_rProduct.oPPQInfo.m_ResultImagePublishedDMIndexHandle, SV_INSPECTION );
				BOOL bPassed = RunInspection( p_rProduct.oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), l_svIndex, &p_rProduct );

				if ( PRODUCT_INSPECTION_NOT_RUN != ( pIPInfo->oInspectedState & PRODUCT_INSPECTION_NOT_RUN ) )
				{
					pIPInfo->oInspectedState = GetToolSet()->GetResultList()->GetInspectionState();

					pIPInfo->m_EndInspection = SVClock::GetTimeStamp();

#ifdef _DEBUG
					//					SVString l_TempStateString = SvUl_SF::Format( _T( "SVInspectionProcess::ProcessInspection|%s|TRI=%ld\n" ),
					//						GetName(), p_rProduct.ProcessCount() );
					//					::OutputDebugString( l_TempStateString.c_str() );
#endif

					// Mark last run product
					LastProductUpdate( &p_rProduct );
				}
			}
		}

		SVObjectManagerClass::Instance().DecrementInspectionIndicator();
		m_qInspectionsQueue.pop_front();

		if( l_Process )
		{
			::InterlockedIncrement( &m_NotifyWithLastInspected );

			pIPInfo->ClearIndexes();
		}

		m_bInspecting = false;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd( _T( "Process Inspections" ) );
#endif
	}
	return l_Status;
}

template<typename T>
struct SH { int i; T it; };

void SVInspectionProcess::InitSharedMemoryItemNames(const long ProductSlots, const long RejectSlots)
{
	SvSml::SVSharedInspectionWriter& rWriter = SvSml::SVSharedMemorySingleton::Instance().GetInspectionWriter(GetPPQIdentifier(), GetUniqueObjectID());
	SvSml::SVSharedData& rFirstData = rWriter.GetLastInspectedSlot(0);

	SH<SVFilterValueMap::iterator> sh = {0, m_SharedMemoryFilters.m_LastInspectedValues.begin()};
	for (sh; sh.it != m_SharedMemoryFilters.m_LastInspectedValues.end(); ++sh.it, sh.i++)
	{
		rFirstData.m_Values[sh.i].SetName(sh.it->first.c_str());
	}
	SH<SVFilterImageMap::iterator> oh_sh = {0, m_SharedMemoryFilters.m_LastInspectedImages.begin()};
	for (oh_sh;oh_sh.it != m_SharedMemoryFilters.m_LastInspectedImages.end();++oh_sh.it, oh_sh.i++)
	{
		rFirstData.m_Images[oh_sh.i].SetName(oh_sh.it->first.c_str());
	}
	for (int i = 1;i < ProductSlots;i++)
	{
		SvSml::SVSharedData& rData = rWriter.GetLastInspectedSlot(i);
		rData = rFirstData;
	}
	for (int i = 0;i < RejectSlots;i++)
	{
		SvSml::SVSharedData& rData = rWriter.GetRejectSlot(i);
		rData = rFirstData;
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

		for (SVMonitorItemList::const_iterator l_ValueIter = valueList.begin();l_ValueIter != valueList.end();++l_ValueIter)
		{
			const SVString& name = (*l_ValueIter);
			SVValueObjectReference l_RefObject;
			if (S_OK == GetInspectionValueObject(name.c_str(), l_RefObject))
			{
				m_SharedMemoryFilters.m_LastInspectedValues[name] = l_RefObject;
			}
			else
			{
				bNotFound = true;
			}
		}
		if (bNotFound)
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvOi::Tid_ErrorNotAllDataItemsFound, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15021 );
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
				m_SharedMemoryFilters.m_LastInspectedImages[ name ] = SVObjectReference(l_pImage);
			}
			else
			{
				bNotFound = true;
			}
		}
		if (bNotFound)
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvOi::Tid_ErrorNotAllImageItemsFound, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15022 );
		}
		::InterlockedIncrement( &m_NotifyWithLastInspected );
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
					SvSml::SVSharedInspectionWriter& rWriter = SvSml::SVSharedMemorySingleton::Instance().GetInspectionWriter(GetPPQIdentifier(), GetUniqueObjectID());
					SvSml::SVSharedData& rLastInspected = rWriter.GetLastInspectedSlot(sharedSlotIndex);
					FillSharedData(sharedSlotIndex, rLastInspected, m_SharedMemoryFilters.m_LastInspectedValues, m_SharedMemoryFilters.m_LastInspectedImages, l_Product, rWriter);
				}
			}
			catch (const std::exception& e)
			{
				SVStringVector msgList;
				msgList.push_back(e.what());
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15023 );
			}
			catch (...)
			{
				SVStringVector msgList;
				msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_Unknown));
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ErrorProcessNotifyLastInspected, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15024 );
			}
		}
		SVInspectionCompleteInfoStruct l_Data(GetUniqueObjectID(), l_Product);
		SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cInspectionProcessTag ), GetUniqueObjectID(), l_Data );

		p_rProcessed = true;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd(_T("Process Notify With Last Inspected"));
#endif
	}
	return l_Status;
}

HRESULT SVInspectionProcess::ProcessCommandQueue( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	if( !( m_CommandQueue.IsEmpty() ) )
	{
#ifdef EnableTracking
		m_InspectionTracking.EventStart( _T( "Process Command Queue" ) );
#endif
		SVCommandTemplatePtr l_CommandPtr;

		if ( m_CommandQueue.RemoveHead( &l_CommandPtr ) )
		{
			if( !( l_CommandPtr.empty() ) )
			{
				l_Status = l_CommandPtr->Execute();
			}
		}

		p_rProcessed = true;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd( _T( "Process Command Queue" ) );
#endif
	}

	return l_Status;
}

SVInspectionProcess::SVInspectionProcess( LPCSTR ObjectName )
: SVObjectClass( ObjectName )
{
	Init();
}

SVInspectionProcess::SVInspectionProcess( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
{
	Init();
}

void SVInspectionProcess::Init()
{
	// Set up your type...
	m_outObjectInfo.ObjectTypeInfo.ObjectType = SVInspectionObjectType;

	m_LastRunLockPtr = new SVCriticalSection;
	m_LastRunProductNULL = false;

	m_pCurrentToolset = nullptr;
	m_PPQId.clear();

	m_bNewDisableMethod	= false;
	m_lEnableAuxiliaryExtents = 0;
	m_lInputRequestMarkerCount = 0L;
	m_bInspecting = false;
	m_dwThreadId = 0;

	m_svReset.RemoveState( SVResetStateAll );

	m_bForceOffsetUpdate = true;

	m_publishList.m_pInspection = this;

	m_pToolSetConditional = nullptr;

	SVString ImageDirectory = SvSml::SVSharedConfiguration::GetImageDirectoryName() + "\\";
	memset(m_BufferImageFileName, 0, sizeof(TCHAR) * BUFFER_IMAGE_FILENAME_LEN );
	_tcscpy_s(m_BufferImageFileName, BUFFER_IMAGE_FILENAME_LEN, ImageDirectory.c_str());
	m_SecondPtrImageFileName = m_BufferImageFileName + static_cast<int>(strlen(m_BufferImageFileName));
	m_SecondPtrImageFileNameLen =  BUFFER_IMAGE_FILENAME_LEN - static_cast<int>(strlen(m_BufferImageFileName));
}

SVInspectionProcess::~SVInspectionProcess()
{
	m_rgbMainImageObject.CloseObject();

	DestroyInspection();

	m_PPQId.clear();

	m_pCurrentToolset	= nullptr;

	m_bNewDisableMethod	= false;
	m_lEnableAuxiliaryExtents = 0;
	m_lInputRequestMarkerCount = 0L;
}

bool SVInspectionProcess::IsCameraInInspection( const SVString& rCameraName ) const
{
	bool Result( false );

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( !Result && l_Iter != m_CameraImages.end() )
	{
		if( nullptr != ( *l_Iter ) )
		{
			Result = ( rCameraName == ( *l_Iter )->GetName() );
		}

		++l_Iter;
	}

	return Result;
}

bool SVInspectionProcess::IsResetStateSet( unsigned long p_State ) const
{
	return m_svReset.IsStateSet( p_State );
}

void SVInspectionProcess::AddResetState( unsigned long p_State )
{
	m_svReset.AddState( p_State );
}

void SVInspectionProcess::RemoveResetState( unsigned long p_State )
{
	m_svReset.RemoveState( p_State );
}

BOOL SVInspectionProcess::CreateInspection( LPCTSTR szDocName )
{
	HRESULT hr;

	SetName( szDocName );

	// Create Queues for Inspection Queue
	if( !m_qInspectionsQueue.Create() )
	{
		return false;
	}
	if( !m_CommandQueue.Create() )
	{
		return false;
	}
	if( !m_MonitorListQueue.Create() ) { return false; }

	::InterlockedExchange( &m_NotifyWithLastInspected, 0 );

	if( S_OK != m_AsyncProcedure.Create( &SVInspectionProcess::APCThreadProcess, boost::bind(&SVInspectionProcess::ThreadProcess, this, _1), GetName(), SVThreadAttribute::SVAffinityUser ) )
	{
		return false;
	}

	hr = CreateResultImageIndexManager();
	if( S_OK != hr )
	{
		return false;
	}
	// Create Queues for input/output requests
	if( !m_InputRequests.Create() )
	{
		return false;
	}
	if( !m_InputImageRequests.Create() )
	{
		return false;
	}

	m_pCurrentToolset = new SVToolSetClass( true, this );

	if( !CreateChildObject(m_pCurrentToolset) )
	{
		return false;
	}

	SVCommandStreamManager::Instance().InsertInspection( GetUniqueObjectID() );

	return true;
}// end Create

HRESULT SVInspectionProcess::CreateResultImageIndexManager() const
{
	HRESULT l_Status = S_OK;

	// Create a managed index for the result data and image circle buffers
	SVString l_Temp = SvUl_SF::Format( _T( "%s Result Image" ), GetName() );

	l_Status = TheSVDataManager.CreateManagedIndexArray( m_pResultImageCircleBuffer, _bstr_t(l_Temp.c_str()), TheSVObserverApp.GetResultImageDepth() );

	return l_Status;
}

void CALLBACK SVInspectionProcess::APCThreadProcess( DWORD_PTR dwParam )
{
}

void SVInspectionProcess::ThreadProcess( bool& p_WaitForEvents )
{
	bool l_Processed = false;
#ifdef EnableTracking
	m_InspectionTracking.SetStartTime();
#endif
	SVProductInfoStruct l_Product;

	ProcessInspection( l_Processed, l_Product );

	ProcessNotifyWithLastInspected( l_Processed, l_Product.m_svInspectionInfos[GetUniqueObjectID()].m_lastInspectedSlot );

	ProcessCommandQueue( l_Processed );

	if( !( l_Product.empty() ) && !( m_PPQId.empty() ) )
	{
		SVInspectionCompleteInfoStruct l_Data( GetUniqueObjectID(), l_Product );
		SVObjectManagerClass::Instance().UpdateObserver( m_PPQId, l_Data );
	}

	p_WaitForEvents = ! l_Processed;
}

BOOL SVInspectionProcess::DestroyInspection()
{
	SVIOEntryStruct pIOEntry;

	SVCommandStreamManager::Instance().EraseInspection( GetUniqueObjectID() );

	SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cInspectionProcessTag ), GetUniqueObjectID(), SVRemoveSubjectStruct() );

	::InterlockedExchange( &m_NotifyWithLastInspected, 0 );

	::Sleep( 0 );

	m_AsyncProcedure.Destroy();

	SVProductInfoStruct l_svProduct;

	LastProductUpdate( &l_svProduct );

	if( !( m_PPQId.empty() ) )
	{
		// Release all published outputs before we destroy the toolset
		m_publishList.Release( m_pCurrentToolset );
	}

	if( nullptr != m_pCurrentToolset )
	{
		DestroyChildObject( m_pCurrentToolset );
	}

	// Destroy Queues for input/output requests
	m_InputRequests.Destroy();
	m_InputImageRequests.Destroy();
	m_CommandQueue.Destroy();
	m_MonitorListQueue.Destroy();

	SVObjectManagerClass::Instance().AdjustInspectionIndicator( -( m_qInspectionsQueue.GetCount() ) );

	// Destroy Queues for Inspection Queue
	m_qInspectionsQueue.Destroy();

	m_PPQInputs.clear();

	m_PPQId.clear();

	m_pResultImageCircleBuffer.clear();

	SVResultListClass* pResultlist = GetResultList();
	if( pResultlist )
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

	SVValueObjectClassPtrSet::iterator l_oValueIter;

	for( l_oValueIter = m_svValueObjectSet.begin(); l_oValueIter != m_svValueObjectSet.end(); ++l_oValueIter )
	{
		m_mapValueObjects.insert( SVValueObjectMap::value_type( (*l_oValueIter)->GetCompleteName().c_str(), (*l_oValueIter) ) );
	}

	return l_hrOk;
}// end BuildValueObjectMap

HRESULT SVInspectionProcess::GetInspectionValueObject( LPCTSTR Name, SVValueObjectReference& p_rRefObject )
{
	HRESULT hr = S_FALSE;

	SVObjectNameInfo l_NameInfo;

	SVValueObjectClass* l_pObject = nullptr;

	hr = l_NameInfo.ParseObjectName( Name );

	if( S_OK == hr )
	{
		if( 0 < l_NameInfo.m_NameArray.size() && l_NameInfo.m_NameArray[ 0 ] != GetName() )
		{
			l_NameInfo.m_NameArray.push_front( GetName() );
		}

		const SVString& l_ObjectName = l_NameInfo.GetObjectName();

		SVValueObjectMap::index_const_iterator< from >::type l_Iter;

		l_Iter = m_mapValueObjects.get< from >().find( l_ObjectName.c_str() );

		if( l_Iter != m_mapValueObjects.get< from >().end() )
		{
			l_pObject = l_Iter->second;
		}
	}

	if ( nullptr != l_pObject )
	{
		p_rRefObject = SVValueObjectReference( l_pObject, l_NameInfo );
		hr = S_OK;
	}// end if

	return hr;
}

HRESULT SVInspectionProcess::GetInspectionObject( LPCTSTR Name, SVObjectReference& p_rRefObject )
{
	SVValueObjectReference ref;
	HRESULT hr = GetInspectionValueObject( Name, ref );	// try the most efficient one first
	if ( S_OK == hr )
	{
		p_rRefObject = ref;
	}
	else	// if not a value object, try getting images
	{
		SVImageClass* pImage = nullptr;
		hr = GetInspectionImage( Name, pImage );
		if ( S_OK == hr )
		{
			p_rRefObject = pImage;
		}
	}
	return hr;
}

BOOL SVInspectionProcess::CanGoOnline()
{
	BOOL l_bOk( true );

	CWaitCursor l_cwcMouse;

	SetResetCounts();
	m_svReset.AddState( SVResetStateInitializeOnReset |	SVResetStateArchiveToolCreateFiles | SVResetStateLoadFiles );

	l_bOk = ( S_OK == InitializeRunOnce() );

	m_svReset.RemoveState( SVResetStateInitializeOnReset |	SVResetStateArchiveToolCreateFiles | SVResetStateLoadFiles );

	ClearResetCounts();

	l_bOk = l_bOk && Validate();

	return l_bOk;
}// end CanGoOnline

BOOL SVInspectionProcess::CanRegressionGoOnline()
{
	BOOL l_bOk( true );

	CWaitCursor l_cwcMouse;

	SetResetCounts();
	m_svReset.AddState( SVResetStateInitializeOnReset |	SVResetStateArchiveToolCreateFiles | SVResetStateLoadFiles );

	l_bOk = resetAllObjects(true, false);

	m_svReset.RemoveState( SVResetStateInitializeOnReset |	SVResetStateArchiveToolCreateFiles | SVResetStateLoadFiles );

	ClearResetCounts();

	l_bOk = l_bOk && Validate();

	return l_bOk;
}

BOOL SVInspectionProcess::GoOnline()
{
#ifdef EnableTracking
	m_InspectionTracking.clear();
#endif

	LastProductUpdate( nullptr );

	m_AsyncProcedure.SetPriority( THREAD_PRIORITY_NORMAL );

	if( S_OK != BuildValueObjectMap() )
	{
		return false;
	}

	ProcessMonitorLists();
	SVPPQObject* pPPQ = GetPPQ();
	if (pPPQ && pPPQ->HasActiveMonitorList())
	{
		InitSharedMemoryItemNames(pPPQ->GetNumProductSlots(), pPPQ->GetNumRejectSlots());
	}
	return true;
}// end GoOnline

BOOL SVInspectionProcess::GoOffline()
{
#ifdef EnableTracking
	if( TheSVObserverApp.UpdateAndGetLogDataManager() )
	{
		SVString l_FileName;

		l_FileName.Format( _T( "C:\\SVObserver\\ProductLastIndexes_%ld-%s.log" ), 
			SVObjectManagerClass::Instance().GetFileSequenceNumber(), GetName() );

		std::fstream l_Stream( l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out );

		if( l_Stream.is_open() )
		{
			SVString l_Info;
			SVProductInfoStruct l_Product = LastProductGet( SV_OTHER );

			l_Product.DumpIndexInfo( l_Info );

			l_Stream << _T( "Last : " );
			l_Stream << l_Info.ToString();
			l_Stream << std::endl;

			l_Stream.close();
		}

		SVString l_Name;

		l_Name.Format( _T( "C:\\SVObserver\\%s-Counts-%ld.csv" ), GetName(), SVObjectManagerClass::Instance().GetFileSequenceNumber() );

		std::fstream l_TrackingStream( l_Name.ToString(), std::ios_base::trunc | std::ios_base::out );

		if( l_TrackingStream.is_open() )
		{
			SVInspectionTracking::SVEventTrackingMap::iterator l_Iter;

			for( l_Iter =	m_InspectionTracking.m_EventCounts.begin(); l_Iter != m_InspectionTracking.m_EventCounts.end(); ++l_Iter )
			{
				// Write Outputs Time...
				l_TrackingStream << _T( "Name/Time ms" );

				SVInspectionTrackingElement::SVTimeCountMap::iterator l_CountIter;

				for( l_CountIter = l_Iter->second.m_Start.begin(); l_CountIter != l_Iter->second.m_Start.end(); ++l_CountIter )
				{
					l_TrackingStream << _T( "," ) << l_CountIter->first;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << l_Iter->first.c_str();
				l_TrackingStream << _T( " - Start" );

				for( l_CountIter = l_Iter->second.m_Start.begin(); l_CountIter != l_Iter->second.m_Start.end(); ++l_CountIter )
				{
					l_TrackingStream << _T( "," ) << l_CountIter->second;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << _T( "Name/Time ms" );

				for( l_CountIter = l_Iter->second.m_Duration.begin(); l_CountIter != l_Iter->second.m_Duration.end(); ++l_CountIter )
				{
					l_TrackingStream << _T( "," ) << l_CountIter->first;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << l_Iter->first.c_str();
				l_TrackingStream << _T( " - Duration" );

				for( l_CountIter = l_Iter->second.m_Duration.begin(); l_CountIter != l_Iter->second.m_Duration.end(); ++l_CountIter )
				{
					l_TrackingStream << _T( "," ) << l_CountIter->second;
				}

				l_TrackingStream << std::endl;

				l_TrackingStream << std::endl;
			}

			l_TrackingStream.close();
		}
	}
#endif

	SVObjectManagerClass::Instance().AdjustInspectionIndicator( -( m_qInspectionsQueue.GetCount() ) );

	// wait for inspection queue to empty
	m_qInspectionsQueue.clear();

	while( m_bInspecting )
	{
		::Sleep( 10 );
	}

	m_AsyncProcedure.SetPriority( THREAD_PRIORITY_NORMAL );

	if ( nullptr != m_pCurrentToolset )
	{
		m_pCurrentToolset->goingOffline();
	}

	if( LastProductGet( SV_OTHER ).ProcessCount() < 1 )
	{
		RestoreCameraImages();
	}
	// Stop updating Shared Memory
	UpdateSharedMemoryFilters(SVMonitorList());

	return true;
}// end GoOffline

HRESULT SVInspectionProcess::SubmitCommand( const SVCommandTemplatePtr& p_rCommandPtr )
{
	HRESULT l_Status = S_OK;

	if( m_CommandQueue.AddTail( p_rCommandPtr ) )
	{
		l_Status = m_AsyncProcedure.Signal( nullptr );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVInspectionProcess::CanProcess( SVProductInfoStruct *pProduct )
{
	SVIOEntryStruct pInEntry;
	SVIOEntryStruct pListEntry;
	int iInSize;
	int iIn;
	bool bReady = true;

	if( pProduct )
	{
		size_t ppqInputSize = m_PPQInputs.size();
		// See if we have discrete inputs.

		for( size_t iList = 0; bReady && iList < ppqInputSize; iList++ )
		{
			pListEntry = m_PPQInputs[iList];
			if( pListEntry.m_IOEntryPtr->m_Enabled )
			{
				iInSize = static_cast<int>(pProduct->oPPQInfo.m_InputData.size());

				for( iIn = 0; bReady && iIn < iInSize; iIn++ )
				{
					pInEntry = pProduct->oPPQInfo.m_InputData[iIn];
					if( pInEntry.m_IOEntryPtr->m_IOId == pListEntry.m_IOEntryPtr->m_IOId )
					{
						bReady &= pInEntry.m_EntryValid;
						break;
					}// end if

				}// end for

			}// end if

		}// end for

		// See if we have Camera images.
		SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while( bReady && l_ImageIter != m_CameraImages.end() )
		{
			SVCameraImageTemplate* l_pImage = ( *l_ImageIter );

			if( nullptr != l_pImage )
			{
				SVVirtualCamera* pCamera = l_pImage->GetCamera();

				if( nullptr != pCamera )
				{
					SVGuidSVCameraInfoStructMap::iterator l_Iter;

					l_Iter = pProduct->m_svCameraInfos.find( pCamera->GetUniqueObjectID() );

					if( l_Iter != pProduct->m_svCameraInfos.end() )
					{
						bReady &= ( 0 <= l_Iter->second.GetIndex() );
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

HRESULT SVInspectionProcess::StartProcess( SVProductInfoStruct *pProduct )
{
	HRESULT hr = S_OK;

	// Make sure that the lists are the same size
	if( m_PPQInputs.size() != pProduct->oPPQInfo.m_InputData.size() )
	{
		hr = E_FAIL;
	}// end if

	if( 0 == m_qInspectionsQueue.GetCount() )
	{
		// Find the info struct for this inspection
		SVInspectionInfoStruct& l_rIPInfo = pProduct->m_svInspectionInfos[ GetUniqueObjectID() ];

		l_rIPInfo.m_CanProcess = false;
		l_rIPInfo.m_InProcess = true;

		SVProductInfoStruct l_TempProduct;

		hr = l_TempProduct.Assign( *pProduct, SV_INSPECTION );

		SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while( l_ImageIter != m_CameraImages.end() )
		{
			SVCameraImageTemplate* l_pImage = ( *l_ImageIter );

			if( nullptr != l_pImage )
			{
				SVVirtualCamera* pCamera = l_pImage->GetCamera();

				if( nullptr != pCamera )
				{
					SVGuidSVCameraInfoStructMap::iterator l_Iter;

					l_Iter = pProduct->m_svCameraInfos.find( pCamera->GetUniqueObjectID() );

					if( l_Iter != pProduct->m_svCameraInfos.end() )
					{
						if( l_Iter->second.GetIndex() < 0 )
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

		if( S_OK == hr && m_qInspectionsQueue.AddTail( l_TempProduct ) )
		{
			SVObjectManagerClass::Instance().IncrementInspectionIndicator();

			hr = m_AsyncProcedure.Signal( nullptr );
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
	SVIOEntryHostStructPtrList ppIOEntries;
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

	if( nullptr == pPPQ )
	{
		return false;
	}

	// Save old list
	ppOldPPQInputs = m_PPQInputs;

	if( !pPPQ->GetAvailableInputs( ppIOEntries ) )
		return FALSE;

	lListSize = static_cast<long>(ppIOEntries.size());

	pPPQ->GetPPQLength( lLength );

	// Make new list
	m_PPQInputs.resize( lListSize );

	long l_LastIndex = 1;
	SVProductInfoStruct l_Product = LastProductGet( SV_INSPECTION );

	if( !( l_Product.empty() ) )
	{
		SVDataManagerHandle l_Handle;

		l_Product.GetResultDataIndex( l_Handle );

		l_LastIndex = l_Handle.GetIndex();
	}

	for( iList = 0; iList < lListSize; iList++ )
	{
		pNewEntry = ppIOEntries[iList];
		bFound = FALSE;

		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pNewEntry->m_IOId );

		for( iTotal = 0; iTotal < ppOldPPQInputs.size(); iTotal++ )
		{
			pIOEntry = ppOldPPQInputs[iTotal];

			if( 0 == strcmp( pIOEntry.m_IOEntryPtr->m_pValueObject->GetName(), l_pObject->GetName() ) )
			{
				// We found it
				bFound = TRUE;
				m_PPQInputs[iList] = pIOEntry;
				pIOEntry.m_IOEntryPtr->m_PPQIndex = pNewEntry->m_PPQIndex;
				pIOEntry.m_IOEntryPtr->m_Enabled = pNewEntry->m_Enabled;
				pIOEntry.m_IOEntryPtr->m_pValueObject->SetObjectDepth( lLength + 50 );
				pIOEntry.m_IOEntryPtr->m_pValueObject->ResetObject();

				break;
			}// end if
		}// end for

		if( !bFound )
		{
			SVValueObjectClass* pValueObject = nullptr;

			switch ( pNewEntry->m_ObjectType )
			{
				case IO_DIGITAL_INPUT:
				{
					pValueObject = new SVBoolValueObjectClass(this);
					pValueObject->ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
					break;
				}
				case IO_REMOTE_INPUT:
				{
					pValueObject = new SVVariantValueObjectClass(this);
					pValueObject->ObjectAttributesAllowedRef() |= SV_REMOTELY_SETABLE;
					break;
				}
				default:
				{
					pValueObject = new SVVariantValueObjectClass(this);
					break;
				}
			}

			pValueObject->SetName( l_pObject->GetName() );
			pValueObject->SetObjectDepth( lLength + 50 );
			pValueObject->SetResetOptions( false, SVResetItemNone );
			pValueObject->ObjectAttributesSetRef() &= SV_PUBLISHABLE;
			pValueObject->ResetObject();

			CreateChildObject(pValueObject);

			pIOEntry.m_IOEntryPtr = new SVIOEntryHostStruct;
			pIOEntry.m_IOEntryPtr->m_pValueObject = pValueObject;
			pIOEntry.m_IOEntryPtr->m_pValueParent = pValueObject;
			pIOEntry.m_IOEntryPtr->m_ObjectType = pNewEntry->m_ObjectType;
			pIOEntry.m_IOEntryPtr->m_IOId = pNewEntry->m_IOId;
			pIOEntry.m_IOEntryPtr->m_Enabled = pNewEntry->m_Enabled;

			m_PPQInputs[iList] = pIOEntry;
		}// end if

		// We now store whether or not these PPQ inputs were viewed
		bFound = FALSE;
		lSize = static_cast<long>(m_arViewedInputNames.size());
		for( l = 0; l < lSize; l++ )
		{
			if( m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject && 
				m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject->GetCompleteName() == m_arViewedInputNames[l] )
			{
				m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject->ObjectAttributesSetRef() |= SV_VIEWABLE;
				bFound = TRUE;
				break;
			}// end if
		}// end for

		if( !bFound || !m_PPQInputs[iList].m_IOEntryPtr->m_Enabled )
		{
			m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject->ObjectAttributesSetRef() &= ~SV_VIEWABLE;
		}// end if

		if( true == m_PPQInputs[iList].m_IOEntryPtr->m_Enabled)
		{
			m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject->ObjectAttributesAllowedRef()  |= SV_SELECTABLE_FOR_EQUATION;
		}
		else
		{
			m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject->ObjectAttributesAllowedRef()  &= ~SV_SELECTABLE_FOR_EQUATION;
		}


	}// end for

	SVResultListClass* pResultlist = GetResultList();
	if(pResultlist)
	{
		pResultlist->RebuildReferenceVector(this);
	}

	BuildValueObjectMap();
	return TRUE;
}

bool SVInspectionProcess::AddInputRequest( const SVValueObjectReference& rObjectRef, const _variant_t& rValue )
{
	bool Result( false );

	try
	{
		SVInputRequestInfoStructPtr pInRequest = new SVInputRequestInfoStruct( rObjectRef, rValue );

		//add request to inspection process
		Result = AddInputRequest( pInRequest );
	}
	catch( ... ) // @WARNING:  bad practice catching ...
	{
	}

	return Result;
}

bool SVInspectionProcess::AddInputRequest( SVInputRequestInfoStructPtr p_pInRequest )
{
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17013_ErrorLockingInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.AddTail( p_pInRequest ) )
	{
		if( !m_InputRequests.Unlock() )
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17014_ErrorUnlockingInputRequests );
			DebugBreak();
		}

		return false;
	}


	SVString l_StringValue;

	if( VT_BSTR == p_pInRequest->m_Value.vt )
	{
		l_StringValue = SvUl_SF::createSVString(p_pInRequest->m_Value);
	}

	if( l_StringValue == SvO::SVTOOLPARAMETERLIST_MARKER )
	{
		::InterlockedIncrement(const_cast<long*>(&m_lInputRequestMarkerCount));
	}// end if

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17015_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	return true;
}

HRESULT SVInspectionProcess::AddInputImageRequest( SVImageClass* p_psvImage, BSTR& p_rbstrValue )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_psvImage )
	{
		SVImageInfoClass l_ImageInfo = p_psvImage->GetImageInfo();

		try
		{
			SVInputImageRequestInfoStructPtr l_pInRequest = new SVInputImageRequestInfoStruct;

			try
			{
				SVCameraImageTemplate* l_psvMainImage = dynamic_cast< SVCameraImageTemplate* >( p_psvImage );

				l_Status = SVImageProcessingClass::LoadImageBuffer( (void*)p_rbstrValue, 
					l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr, l_ImageInfo );

				l_pInRequest->m_bUsingCameraName = nullptr != l_psvMainImage;

				if( l_pInRequest->m_bUsingCameraName )
				{
					l_pInRequest->m_ObjectName = l_psvMainImage->GetCameraName();
				}
				else
				{
					l_pInRequest->m_ObjectName = p_psvImage->GetCompleteName();
				}

				//add request to inspection process
				HRESULT l_Temp = AddInputImageRequest( l_pInRequest );

				if( S_OK == l_Status )
				{
					l_Status = l_Temp;
				}
			}
			catch( ... )
			{
				l_Status = E_FAIL;
			}
		}
		catch( ... )
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

HRESULT SVInspectionProcess::AddInputImageFileNameRequest( SVImageClass* p_psvImage, const SVString& p_rImageFileName )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_psvImage )
	{
		SVImageInfoClass l_ImageInfo = p_psvImage->GetImageInfo();

		try
		{
			SVInputImageRequestInfoStructPtr l_pInRequest = new SVInputImageRequestInfoStruct;

			SVCameraImageTemplate* l_psvMainImage = dynamic_cast<SVCameraImageTemplate*> (p_psvImage);

			l_Status = SVImageProcessingClass::LoadImageBuffer( p_rImageFileName.c_str(), l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr );

			l_pInRequest->m_bUsingCameraName = nullptr != l_psvMainImage;

			if( l_pInRequest->m_bUsingCameraName )
			{
				l_pInRequest->m_ObjectName = l_psvMainImage->GetCameraName();
			}
			else
			{
				l_pInRequest->m_ObjectName = p_psvImage->GetCompleteName();
			}

			//add request to inspection process
			HRESULT l_Temp = AddInputImageRequest( l_pInRequest );

			if( S_OK == l_Status )
			{
				l_Status = l_Temp;
			}
		}
		catch( ... )
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

HRESULT SVInspectionProcess::AddInputImageRequest( SVInputImageRequestInfoStructPtr p_pInRequest )
{
	if( !m_InputImageRequests.Lock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17016_ErrorLockingInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.AddTail( p_pInRequest ) )
	{
		if( !m_InputImageRequests.Unlock() )
		{
			SvStl::MessageMgrStd e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17017_ErrorUnlockingInputImageRequests );
			DebugBreak();
		}

		return S_FALSE;
	}// end if

	if( !m_InputImageRequests.Unlock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17018_ErrorUnlockingInputImageRequests );
		DebugBreak();
	}

	return S_OK;
}// end AddInputImageRequest

BOOL SVInspectionProcess::RemoveAllInputRequests()
{
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17019_ErrorLockingInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.RemoveAll() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorRemovingAllInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17020_ErrorRemovingAllInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17021_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.Lock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17022_ErrorLockingInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.RemoveAll() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorRemovingAllInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17023_ErrorRemovingAllInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.Unlock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputImageRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17024_ErrorUnlockingInputImageRequests );
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

	if( nullptr != pPPQ )
	{
		pPPQ->GetPPQLength( lLength );

		SVProductInfoStruct l_Product = LastProductGet( SV_INSPECTION );

		if( l_Product.empty() )
		{
			pPPQ->ReserveNextRunOnceProductInfoStruct( l_Product, SV_INSPECTION );

			ReserveNextResultImage( &l_Product, SV_INSPECTION, true );

			UpdateMainImagesByProduct( &l_Product );

			LastProductUpdate( &l_Product );
		}

		SVDataManagerHandle l_Handle;

		l_Product.GetResultDataIndex( l_Handle );

		l_LastIndex = l_Handle.GetIndex();
	}

	SetObjectDepthWithIndex( lLength + 50, l_LastIndex );
	SetImageDepth( TheSVObserverApp.GetResultImageDepth() );

	////////////////////////
	// Color SVIM
	if ( IsColorCamera() )
	{
		SVVirtualCameraMap l_Cameras;

		if( nullptr != pPPQ )
		{
			pPPQ->GetVirtualCameras( l_Cameras );
		}

		bool bColorSourceImage = false;
		bool bCameraSupportsColor = false;
		SVVirtualCameraMap::iterator pos = l_Cameras.begin();
		while( pos != l_Cameras.end() )
		{
			SVAcquisitionClassPtr pCamDevice = nullptr;

			if( nullptr != pos->second )
			{
				pCamDevice = pos->second->GetAcquisitionDevice();
			}

			if( pCamDevice && 3L == pCamDevice->BandSize() )
			{
				bColorSourceImage = true;
				break;
			}

			SVDeviceParamCollection params;
			SVDeviceParamCollection CFParams;
			pCamDevice->GetDeviceParameters( params );
			pCamDevice->GetCameraFileParameters( CFParams );
			const SVCameraFormatsDeviceParam* pParam = params.Parameter( DeviceParamCameraFormats ).DerivedValue( pParam );
			const SVCameraFormatsDeviceParam* pCFParam = CFParams.Parameter( DeviceParamCameraFormats ).DerivedValue( pCFParam );

			if ( pParam && pCFParam )
			{
				SVCameraFormatsDeviceParam::OptionsType::const_iterator iter;
				for ( iter = pCFParam->options.begin(); iter != pCFParam->options.end(); ++iter)
				{
					if ( iter->second.m_bColor )
					{
						bCameraSupportsColor = true;
					}
				}

				if ( pParam && pParam->options.find(pParam->strValue)->second.m_bColor )
				{
					bColorSourceImage = true;
					break;
				}
			}

			++pos;
		}

		if ( !bColorSourceImage )
		{
			if ( ! bCameraSupportsColor )
			{
				l_Status = E_FAIL;

				SvStl::MessageMgrStd Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_IPDoc_NoCameraColorAttached, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10052 ); 
			}
		}
		else
		{
			if( !CreateChildObject(&m_rgbMainImageObject) )
			{
				l_Status = E_FAIL;
			}

			m_rgbMainImageObject.UpdateImage( SVImageTypePhysical, SV_GUID_NULL );
			m_rgbMainImageObject.SetObjectOwner( this );
		}
	}
	////////////////////////

	SVObjectLevelCreateStruct createStruct;

	if( createAllObjects(createStruct) )
	{
		l_Status = E_FAIL;
	}

	SetDefaultInputs();

	if( CheckAndResetConditionalHistory() )
	{
		//Configuration has changed need to set the modified flag
		SVSVIMStateClass::AddState( SV_STATE_MODIFIED );
		if( SVSVIMStateClass::CheckState( SV_STATE_REMOTE_CMD ) )
		{
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_CONDITIONAL_HISTORY, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
		else
		{
			SvStl::MessageMgrStd Exception( SvStl::LogAndDisplay );
			Exception.setMessage( SVMSG_SVO_CONDITIONAL_HISTORY, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
	}

#if defined (TRACE_THEM_ALL) || defined (TRACE_IP)
	int iCount = static_cast<int>(m_mapValueObjects.size());
	SVString Text = SvUl_SF::Format(_T("%s value object count=%d\n"), GetName(), iCount);
	::OutputDebugString( Text.c_str() );
#endif

	return l_Status;
}

void SVInspectionProcess::ClearResetCounts()
{
	if( nullptr != m_pCurrentToolset )
	{
		m_pCurrentToolset->ClearResetCounts();
	}
}

void SVInspectionProcess::SetResetCounts()
{
	if( nullptr != m_pCurrentToolset )
	{
		m_pCurrentToolset->ResetCounts();
	}
}

void SVInspectionProcess::ValidateAndInitialize( bool p_Validate, bool p_IsNew )
{
	SVSVIMStateClass::AddState( SV_STATE_INTERNAL_RUN );

	// Call ToolSet Validate 
	// so the SVEquationClass can rebuild its symbol table
	if ( p_Validate )
	{
		SetResetCounts();

		m_svReset.AddState( SVResetAutoMoveAndResize | SVResetStateInitializeOnReset |	SVResetStateArchiveToolCreateFiles | SVResetStateLoadFiles );

		SVStdMapSVToolClassPtrSVInspectionProcessResetStruct l_svToolMap;

		SVResetItemEnum l_eResetItem = SVResetItemIP;

		BOOL bok = ProcessInputRequests( 1, l_eResetItem, l_svToolMap );

		m_svReset.RemoveState( SVResetAutoMoveAndResize | SVResetStateInitializeOnReset | SVResetStateArchiveToolCreateFiles | SVResetStateLoadFiles );

		Validate();
	}

	if( !p_IsNew )
	{
		RunOnce();
	}

	SVSVIMStateClass::RemoveState( SV_STATE_INTERNAL_RUN );
}

void SVInspectionProcess::SingleRunModeLoop( bool p_Refresh )
{
	SVProductInfoStruct l_svProduct;

	SVPPQObject* pPPQ = GetPPQ();

	if( nullptr != pPPQ )
	{
		pPPQ->ReserveNextRunOnceProductInfoStruct( l_svProduct, SV_INSPECTION );
	}

	LastProductCopySourceImagesTo( &l_svProduct );
	ReserveNextResultImage( &l_svProduct, SV_INSPECTION, true );

	SVImageIndexStruct l_svResultImageIndex;

	l_svProduct.GetResultImageIndex( l_svResultImageIndex, GetUniqueObjectID() );

	SVDataManagerHandle	l_ResultDataDMIndexHandle;

	l_svProduct.GetResultDataIndex( l_ResultDataDMIndexHandle );

	RunInspection( l_ResultDataDMIndexHandle.GetIndex(), l_svResultImageIndex, &l_svProduct );

	LastProductUpdate( &l_svProduct );

	if( p_Refresh )
	{
		// Result View, Display Image and Overlay Update
		if ( ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING ) )
		{
			SVInspectionCompleteInfoStruct l_Data( GetUniqueObjectID(), l_svProduct );

			SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cInspectionProcessTag ), GetUniqueObjectID(), l_Data );
		}
	}
}

bool SVInspectionProcess::resetAllObjects( bool shouldNotifyFriends, bool silentReset )
{
	for (size_t l = 0; l < m_PPQInputs.size(); l++)
	{
		if( !( m_PPQInputs[ l ].empty() ) && nullptr != m_PPQInputs[l].m_IOEntryPtr->m_pValueObject )
		{
			m_PPQInputs[l].m_IOEntryPtr->m_pValueObject->resetAllObjects(shouldNotifyFriends, silentReset);
			break;
		}
	}// end for

	bool Result = ( S_OK == ResetObject() );
	ASSERT( Result );
	return Result;
}

BOOL SVInspectionProcess::GetChildObjectByName( LPCTSTR tszChildName, SVObjectClass** ppObject )
{
	ASSERT( nullptr != ppObject );
	bool bReturn = false;

	if ( nullptr != ppObject )
	{
		*ppObject = nullptr;

		SVString ChildName = tszChildName;
		SVString Name = GetCompleteName();

		if( SvUl_SF::Left( ChildName, Name.size() ) == Name )
		{
			SVObjectManagerClass::Instance().GetObjectByDottedName(tszChildName, *ppObject);
			bReturn = ( nullptr != *ppObject );
		}
	}

	return bReturn;
}

HRESULT SVInspectionProcess::FindPPQInputObjectByName( SVObjectClass*& p_rpObject, LPCTSTR p_FullName ) const
{
	HRESULT l_Status = S_OK;

	for( size_t l = 0; nullptr == p_rpObject && l < m_PPQInputs.size(); ++l )
	{
		if( !( m_PPQInputs[ l ].empty() ) )
		{
			SVString l_LocalName = GetName();

			l_LocalName += _T( "." );
			l_LocalName += m_PPQInputs[l].m_IOEntryPtr->m_pValueObject->GetName();

			if( l_LocalName == p_FullName )
			{
				p_rpObject = m_PPQInputs[l].m_IOEntryPtr->m_pValueObject;
			}
		}
	}

	if( nullptr == p_rpObject )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	HRESULT l_Status = SVObjectClass::GetChildObject( rpObject, rNameInfo, Index );

	if( l_Status != S_OK )
	{
		if( static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName() )
		{
			SVString l_ObjectName = rNameInfo.GetObjectName( Index );

			l_Status = FindPPQInputObjectByName( rpObject, l_ObjectName.c_str() );

			if( l_Status != S_OK )
			{
				if( GetToolSet() )
				{
					l_Status = GetToolSet()->GetChildObject( rpObject, rNameInfo, Index + 1 );
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVInspectionProcess::ObserverUpdate( const SVAddTool& p_rData )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_rData.m_pTool )
	{
		// Insert tool in GetToolSet()...
		GetToolSet()->InsertAt( p_rData.m_Index, p_rData.m_pTool );

		// And finally try to create the object...
		if( GetToolSet()->CreateChildObject( p_rData.m_pTool, SVMFResetObject ) ) 
		{
			BuildValueObjectMap();

			// Set default formulas in newly instantiated tool...
			p_rData.m_pTool->SetDefaultFormulas();

			RunOnce();
		}
		else
		{
			GetToolSet()->RemoveAt( p_rData.m_Index );

			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::ObserverUpdate( const SVDeleteTool& p_rData )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_rData.m_pTool )
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
		resetAllObjects(true, false);
		BuildValueObjectMap();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVInspectionProcess::SetPPQIdentifier( const SVGUID& p_rPPQId )
{
	if( !( m_PPQId.empty() ) && ( m_PPQId != p_rPPQId ) )
	{
		// Release all published outputs before we destroy the toolset
		m_publishList.Release( m_pCurrentToolset );
	}

	m_PPQId = p_rPPQId;
}

const SVGUID& SVInspectionProcess::GetPPQIdentifier() const
{
	return m_PPQId;
}

SVPPQObject* SVInspectionProcess::GetPPQ() const
{
	SVPPQObject* pPPQ( nullptr );

	if( !( m_PPQId.empty() ) )
	{
		pPPQ = dynamic_cast< SVPPQObject* >( SVObjectManagerClass::Instance().GetObject( m_PPQId ) );
	}

	return pPPQ; 
}

SvOi::IObjectClass* SVInspectionProcess::GetPPQInterface() const
{
	return GetPPQ();
}

BOOL SVInspectionProcess::RunOnce( SVToolClass *p_psvTool )
{
	BOOL bRet( true );

	SVDataManagerHandle l_Handle;

	SVProductInfoStruct l_Product = LastProductGet( SV_OTHER );

	l_Product.GetResultDataIndex( l_Handle );

	SVStdMapSVToolClassPtrSVInspectionProcessResetStruct l_svToolMap;

	SVResetItemEnum l_eResetItem = SVResetItemNone;

	bRet = ProcessInputRequests( l_Handle.GetIndex(), l_eResetItem, l_svToolMap );

	if( SVResetItemIP < l_eResetItem && nullptr != p_psvTool )
	{
		bRet = p_psvTool->GetInspection() == this;

		if( bRet )
		{
			SVRunStatusClass runStatus;

			l_Product.GetResultImageIndex( runStatus.Images, GetUniqueObjectID() );

			runStatus.m_lResultDataIndex = l_Handle.GetIndex();

			m_bForceOffsetUpdate = true;

			p_psvTool->Run( runStatus );

			LastProductNotify();
		}
	}
	else
	{
		SingleRunModeLoop( true );
	}

	return bRet;
}

HRESULT SVInspectionProcess::InitializeRunOnce()
{
	HRESULT l_Status = S_OK;

	SVPPQObject* pPPQ = GetPPQ();

	if( nullptr != pPPQ )
	{
		SVProductInfoStruct l_svProduct;
		SVImageIndexStruct l_svResultImageIndex;
		SVDataManagerHandle	l_ResultDataDMIndexHandle;

		if( ! pPPQ->ReserveNextRunOnceProductInfoStruct( l_svProduct, SV_INSPECTION ) )
		{
			l_Status = E_FAIL;
		}

		HRESULT l_Temp = S_OK;

		l_Temp = SetSourceImagesTo( &l_svProduct );

		if( S_OK == l_Status ) { l_Status = l_Temp; }

		l_Temp = ReserveNextResultImage( &l_svProduct, SV_INSPECTION, true );

		if( S_OK == l_Status ) { l_Status = l_Temp; }

		l_Temp = l_svProduct.GetResultImageIndex( l_svResultImageIndex, GetUniqueObjectID() );

		if( S_OK == l_Status ) { l_Status = l_Temp; }

		l_Temp = l_svProduct.GetResultDataIndex( l_ResultDataDMIndexHandle );

		if( S_OK == l_Status ) { l_Status = l_Temp; }

		if( !( RunInspection( l_ResultDataDMIndexHandle.GetIndex(), l_svResultImageIndex, &l_svProduct, false ) ) && S_OK == l_Status )
		{
			l_Status = E_FAIL;
		}

		l_Temp = LastProductUpdate( &l_svProduct );

		if( S_OK == l_Status ) { l_Status = l_Temp; }
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVInspectionProcess::ProcessInputRequests( long p_DataIndex, bool &p_rbForceOffsetUpdate )
{
	SVStdMapSVToolClassPtrSVInspectionProcessResetStruct l_svToolMap;

	SVResetItemEnum l_eResetItem = SVResetItemNone;

	BOOL l_bOk = ProcessInputRequests( p_DataIndex, l_eResetItem, l_svToolMap );

	p_rbForceOffsetUpdate |= ( l_eResetItem < SVResetItemNone );

	return l_bOk;
}

namespace
{
	bool inline IsArray(SVValueObjectReference & ref)
	{
		return ref->IsArray() && ref.IsEntireArray();
	}
}

BOOL SVInspectionProcess::ProcessInputRequests( long p_DataIndex, SVResetItemEnum &p_reResetItem, SVStdMapSVToolClassPtrSVInspectionProcessResetStruct &p_rsvToolMap )
{
	bool bRet = true;
	long l;

	SVInputRequestInfoStructPtr l_pInputRequest;

	// Process all input requests
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17025_ErrorLockingInputRequests );
		DebugBreak();
	}

	while( m_lInputRequestMarkerCount > 0L )
	{
		long l_lSize = 0;

		// Only loop through the InputRequests that are here now
		// Preprocessing phase
		m_InputRequests.GetSize( l_lSize );

		for( l = 0; l < l_lSize; l++ )
		{
			if( !m_InputRequests.RemoveHead( &l_pInputRequest ) )
			{
				bRet = false;

				break;
			}// end if

			SVString Value;

			if( VT_ARRAY == ( l_pInputRequest->m_Value.vt & VT_ARRAY ) )
			{
				SVSAFEARRAY::SVBounds l_Bounds;
				SVSAFEARRAY l_SafeArray = l_pInputRequest->m_Value;

				l_SafeArray.GetBounds( l_Bounds );

				if( 1 == l_SafeArray.size() )
				{
					_variant_t l_Variant;

					l_SafeArray.GetElement( l_Bounds[ 0 ].lLbound, l_Variant );

					Value = SvUl_SF::createSVString( l_Variant );
				}
				else if( 1 < l_SafeArray.size() )
				{
					for( size_t i = 0; i < l_Bounds[ 0 ].cElements; ++i )
					{
						_variant_t l_Variant;

						l_SafeArray.GetElement( ( l_Bounds[ 0 ].lLbound + i ), l_Variant );

						if ( i > 0 )
						{
							Value += _T(",");
						}
						Value += SvUl_SF::Format( _T("`%s`"), SvUl_SF::createSVString( l_Variant ).c_str() );
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
			if( Value == SvO::SVTOOLPARAMETERLIST_MARKER )
			{
				::InterlockedDecrement(const_cast<long*>(&m_lInputRequestMarkerCount));
				break;
			}// end if

			// Get the ValueObject that they are trying to set
			SVValueObjectReference refValueObject = l_pInputRequest->refValueObject;
			HRESULT hrSet = S_OK;
			if( nullptr != refValueObject.Object() )
			{
				long l_iIndex = ( 0 < p_DataIndex ) ? p_DataIndex : 1;

				bool l_bResetObject = refValueObject.Object()->ResetAlways();

				ASSERT(!::IsArray(refValueObject) || refValueObject.ArrayIndex() < 0);

				// Value objects don't accept each different type for sets
				// so convert to the appropriate type
				if( SV_IS_KIND_OF( refValueObject.Object(), SVFileNameValueObjectClass ) )
				{
					if (::IsArray(refValueObject))
					{
						hrSet = SetObjectArrayValues<CFile>(refValueObject, l_iIndex, Value, l_bResetObject);
					}
					else
					{
						CFileStatus rStatus;

						BOOL bOk = CFile::GetStatus( Value.c_str(), rStatus );
						if ( bOk )
						{
							bOk = 0L <= rStatus.m_size;
						}// end if

						if( bOk )
						{
							if( ! l_bResetObject )
							{
								SVString PrevValue;

								hrSet = refValueObject.GetValue( PrevValue );

								l_bResetObject = (PrevValue != Value);
							}

							hrSet = refValueObject.SetValue( l_iIndex, Value );
						}// end if
					}
				}// end if SVFileNameValueObjectClass
				else if( SV_IS_KIND_OF( refValueObject.Object(), SVStringValueObjectClass ) )
				{
					if (::IsArray(refValueObject))
					{
						hrSet = SetObjectArrayValues<SVString>(refValueObject, l_iIndex, Value, l_bResetObject);
					}
					else
					{
						if( ! l_bResetObject )
						{
							SVString PrevValue;

							hrSet = refValueObject.GetValue( PrevValue );

							l_bResetObject = (PrevValue != Value );
						}

						hrSet = refValueObject.SetValue( l_iIndex, Value );
					}
				}// end if SVStringValueObjectClass
				else if( SV_IS_KIND_OF( refValueObject.Object(), SVBoolValueObjectClass ) )
				{
					double l_dValue = FALSE;

					// Convert to the appropriate type of value
					if( 0 == SvUl_SF::CompareNoCase( Value, _T( "TRUE" ) ) )
					{
						l_dValue = TRUE;
					}// end if
					else
					{
						l_dValue = atof( Value.c_str() );
					}// end else

					if( ! l_bResetObject )
					{
						double l_dPrevValue = FALSE;

						hrSet = refValueObject.GetValue( l_dPrevValue );

						// eventually use CompareWithCurrentValueImpl
						l_bResetObject = l_dPrevValue != l_dValue;
					}

					hrSet = refValueObject.SetValue( l_iIndex, l_dValue );
				}// end else if SVBoolValueObjectClass
				else if ( SVVariantValueObjectClass* pvValueObject = dynamic_cast <SVVariantValueObjectClass*> ( refValueObject.Object() ) )
				{
					// Convert to the appropriate type of value

					// eventually use CompareWithCurrentValueImpl
					l_bResetObject = true;

					if( refValueObject->IsArray() )
					{
						hrSet = pvValueObject->SetValueKeepType( l_iIndex, refValueObject.ArrayIndex(), Value.c_str() );
					}
					else
					{
						hrSet = pvValueObject->SetValueKeepType( l_iIndex, Value.c_str() );
					}
				}// end else if SVVariantValueObjectClass
				else if ( nullptr != dynamic_cast <SVEnumerateValueObjectClass*> ( refValueObject.Object() ) )
				{
					// Convert to the appropriate type of value
					bool l_bTempReset = false;

					if( ! l_bResetObject )
					{
						HRESULT hrCompare = refValueObject->CompareWithCurrentValue( Value.c_str() );
						l_bTempReset = ( S_OK != hrCompare );	// reset if not the same
					}

					if( S_OK == (hrSet = refValueObject.SetValue( 1, Value.c_str() )) )
					{
						hrSet = refValueObject.SetValue( l_iIndex, Value.c_str() );
					}
					else
					{
						if ( Value.size() > 0 )
						{
							if(  _ismbcdigit( Value.c_str()[0] ))
							{
								double l_dValue = atof( Value.c_str() );

								if( ! l_bResetObject )
								{
									HRESULT hrCompare = refValueObject->CompareWithCurrentValue( Value.c_str() );
									l_bTempReset = ( S_OK != hrCompare );	// reset if not the same
								}

								hrSet = refValueObject.SetValue( l_iIndex, l_dValue );
							}
						}
					}

					l_bResetObject = l_bTempReset;
				}// end else if SVEnumerateValueObjectClass
				else if ( nullptr != dynamic_cast <SVDoubleValueObjectClass*> ( refValueObject.Object() ) ) // Double
				{
					if (::IsArray(refValueObject))
					{
						hrSet = SetObjectArrayValues<double>(refValueObject, l_iIndex, Value.c_str(), l_bResetObject);
					}
					else
					{
						if( ! l_bResetObject )
						{
							double l_dPrevValue;

							hrSet = refValueObject.GetValue( l_dPrevValue );

							// eventually use CompareWithCurrentValueImpl
							l_bResetObject = l_dPrevValue != atof( Value.c_str() );
						}

						hrSet = refValueObject.SetValue( l_iIndex, Value.c_str() );
					}
				}// end else if
				else // Long, DWord, Byte
				{
					if( ! l_bResetObject )
					{
						double l_dPrevValue;

						hrSet = refValueObject.GetValue( l_dPrevValue );

						SVString strNewValue( Value );
						SvUl_SF::MakeLower( strNewValue );
						TCHAR* p = nullptr;
						long lValue = 0;
						if ( SVString::npos != strNewValue.find(_T('x')) )
						{
							lValue = _tcstol(strNewValue.c_str(), &p, 16);
						}
						else
						{
							lValue = _tcstol(strNewValue.c_str(), &p, 10);
						}
						double l_dNewValue = static_cast <double> (lValue);

						// eventually use CompareWithCurrentValueImpl
						l_bResetObject = l_dPrevValue != l_dNewValue;
					}

					hrSet = refValueObject.SetValue( l_iIndex, Value.c_str() );
				}// end else if

				if( SVResetItemIP != p_reResetItem && l_bResetObject && refValueObject.Object()->ResetItem() < SVResetItemNone )
				{
					SVToolClass* l_psvTool = dynamic_cast<SVToolClass*>(refValueObject.Object()->GetAncestor(SVToolObjectType));

					if( nullptr != l_psvTool )
					{
						SVResetItemEnum l_eToolResetItem = p_rsvToolMap[ l_psvTool ].SetResetData( refValueObject.Object()->ResetItem(), refValueObject.Object() );

						if( l_eToolResetItem < p_reResetItem )
						{
							p_reResetItem = l_eToolResetItem;
						}
					}
					else
					{
						p_reResetItem = SVResetItemIP;
					}

					if( SVResetItemIP == p_reResetItem )
					{
						p_rsvToolMap.clear();
					}
				}
			}// end if object exists

		}// end for all input requests

		// Processing phase
		if ( nullptr != m_pCurrentToolset )
		{
			if( SVResetItemIP == p_reResetItem )
			{
				bRet &= resetAllObjects(true, false);
				bRet &= S_OK == m_pCurrentToolset->ClearResetCounts();
			}
			else
			{
				int l_iSize = m_pCurrentToolset->GetSize();

				for ( l = 0; l < l_iSize; l++ )
				{
					SVToolClass *l_psvTool = dynamic_cast<SVToolClass *>(m_pCurrentToolset->GetAt( l ));

					if ( nullptr != l_psvTool )
					{
						if( p_rsvToolMap.find( l_psvTool ) != p_rsvToolMap.end() )
						{
							if( p_rsvToolMap[ l_psvTool ].m_svObjectSet.empty() )
							{
								bRet &= l_psvTool->resetAllObjects(true, false);
							}
							else
							{
								SVStdSetSVObjectClassPtr::iterator l_oIter;

								l_oIter = p_rsvToolMap[ l_psvTool ].m_svObjectSet.begin();

								while( l_oIter != p_rsvToolMap[ l_psvTool ].m_svObjectSet.end() )
								{
									SVObjectClass *l_psvObject = *l_oIter;

									if (nullptr != l_psvObject)
									{
										bRet &= l_psvObject->resetAllObjects(true, false);
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

			if( p_reResetItem < SVResetItemNone )
			{
				SVRemoveValues l_Data;

				SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cInspectionProcessTag ), GetUniqueObjectID(), l_Data );
			}
		}// end if ( nullptr != m_pCurrentToolset )
	}// end while( m_lInputRequestMarkerCount > 0L )

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17026_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	return bRet;
}

BOOL SVInspectionProcess::ProcessInputImageRequests( SVProductInfoStruct *p_psvProduct )
{
	BOOL l_bOk = TRUE;

	if( ! m_InputImageRequests.IsEmpty() )
	{
		long l_lSize = 0;

		m_InputImageRequests.GetSize( l_lSize );

		for( long l = 0; l < l_lSize; l++ )
		{
			SVInputImageRequestInfoStructPtr l_pInRequest;

			if( m_InputImageRequests.RemoveHead( &l_pInRequest ) )
			{
				SVImageClass *l_psvImage = nullptr;

				if ( l_pInRequest->m_bUsingCameraName ) //not using camera name...
				{
					SVCameraImagePtrSet l_MainImages;

					GetMainImages( l_pInRequest->m_ObjectName.c_str(), l_MainImages );

					SVCameraImagePtrSet::iterator l_Iter = l_MainImages.begin();

					while( nullptr == l_psvImage && l_Iter != l_MainImages.end() )
					{
						SVCameraImageTemplate* l_pCameraImage = ( *l_Iter );

						if( ( nullptr != l_pCameraImage ) && ( SVRGBMainImageObjectType != l_pCameraImage->GetObjectSubType() ) )
						{
							l_psvImage = l_pCameraImage;
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

					if( GetChildObjectByName(l_pInRequest->m_ObjectName.c_str(), &l_psvObject) )
					{
						l_psvImage = dynamic_cast<SVImageClass*>( l_psvObject );
					}
				}

				l_bOk = nullptr != l_psvImage;

				if( l_bOk )
				{
					SVImageIndexStruct l_svIndex;

					if( SVMainImageClass* l_psvMainImage = dynamic_cast< SVMainImageClass* >( l_psvImage ) )
					{
						SVGuidSVCameraInfoStructMap::iterator l_svIter;

						l_svIter = p_psvProduct->m_svCameraInfos.find( l_psvMainImage->GetCamera()->GetUniqueObjectID() );

						if( l_svIter != p_psvProduct->m_svCameraInfos.end() )
						{
							SVCameraInfoStruct& rCameraInfo = l_svIter->second;

							l_svIndex.m_CameraDMIndexHandle.Assign( rCameraInfo.GetSourceImageDMIndexHandle(), SV_INSPECTION );
						}
					}
					else
					{
						p_psvProduct->GetResultImageIndex( l_svIndex, GetUniqueObjectID() );
					}

					l_bOk = ! l_svIndex.IsNull();

					if( l_bOk )
					{
						l_bOk = l_psvImage->SetImageHandleIndex( l_svIndex );

						if( l_bOk )
						{
							SVSmartHandlePointer l_ImageHandle;

							if ( l_psvImage->GetImageHandle( l_ImageHandle ) && !( l_ImageHandle.empty() ) && !( l_pInRequest->m_ImageHandlePtr.empty() ) )
							{
								SVMatroxBufferInterface::SVStatusCode l_Code;

								SVImageBufferHandleImage l_MilHandle;
								SVImageBufferHandleImage l_RequestMilHandle;

								l_ImageHandle->GetData( l_MilHandle );
								l_pInRequest->m_ImageHandlePtr->GetData( l_RequestMilHandle );

								l_Code = SVMatroxBufferInterface::CopyBuffer( l_MilHandle.GetBuffer(), l_RequestMilHandle.GetBuffer() );
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

HRESULT SVInspectionProcess::ReserveNextResultImage( SVProductInfoStruct *p_pProduct, SVDataManagerLockTypeEnum p_eLockType, bool p_ClearOtherInspections )
{
	// We will always lock/unlock the published index along with the regular index

	HRESULT l_hrOk = S_FALSE;

	if( p_ClearOtherInspections )
	{
		SVGUIDSVInspectionInfoStructMap::iterator l_Iter = p_pProduct->m_svInspectionInfos.begin();

		while( l_Iter != p_pProduct->m_svInspectionInfos.end() )
		{
			l_Iter->second.ClearIndexes();	

			++l_Iter;
		}
	}

	SVInspectionInfoStruct& l_svIPInfo = p_pProduct->m_svInspectionInfos[ GetUniqueObjectID() ];

	l_hrOk = GetNextAvailableIndexes( l_svIPInfo, p_eLockType );

	return l_hrOk;
}

BOOL SVInspectionProcess::DisconnectToolSetMainImage()
{
	BOOL bRet = true;

	SVCameraImagePtrSet::iterator l_Iter = m_CameraImages.begin();

	while( l_Iter != m_CameraImages.end() )
	{
		SVCameraImageTemplate* l_pCameraImage = ( *l_Iter );

		if( nullptr != l_pCameraImage )
		{
			l_pCameraImage->DisconnectBuffers();

			++l_Iter;
		}
		else
		{
			l_Iter = m_CameraImages.erase( l_Iter );
		}
	}

	return bRet;
}

BOOL SVInspectionProcess::ConnectToolSetMainImage()
{
	BOOL bRet = true;

	m_svReset.AddState( SVResetAutoMoveAndResize );

	SVCameraImagePtrSet::iterator l_Iter = m_CameraImages.begin();

	while( l_Iter != m_CameraImages.end() )
	{
		SVCameraImageTemplate* l_pCameraImage = ( *l_Iter );

		if( nullptr != l_pCameraImage )
		{
			l_pCameraImage->ReconnectBuffers();

			++l_Iter;
		}
		else
		{
			l_Iter = m_CameraImages.erase( l_Iter );
		}
	}

	m_svReset.RemoveState( SVResetAutoMoveAndResize );

	return bRet;
}

BOOL SVInspectionProcess::RemoveCamera( const SVString& rCameraName )
{
	BOOL bRet = TRUE;

	if( rCameraName == m_ToolSetCameraName )
	{
		SVVirtualCamera* pCamera = GetFirstPPQCamera();

		if( nullptr != pCamera )
		{
			m_ToolSetCameraName = pCamera->GetName();
		}
		else
		{
			m_ToolSetCameraName.clear();
		}
	}

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( l_Iter != m_CameraImages.end() )
	{
		if( nullptr != *l_Iter )
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if( ( nullptr != pCamera ) && ( rCameraName == pCamera->GetName() ) )
			{
				(*l_Iter)->UpdateCameraImage( m_ToolSetCameraName.c_str() );
			}
		}

		++l_Iter;
	}

	return bRet;
}

BOOL SVInspectionProcess::GetNewDisableMethod()
{
	return m_bNewDisableMethod;
}

void SVInspectionProcess::SetNewDisableMethod( BOOL bNewDisableMethod )
{
	m_bNewDisableMethod = bNewDisableMethod;
}

long SVInspectionProcess::GetEnableAuxiliaryExtent() const
{
	return m_lEnableAuxiliaryExtents;
}

void SVInspectionProcess::SetEnableAuxiliaryExtent( long p_lEnableAuxiliaryExtents )
{
	m_lEnableAuxiliaryExtents = p_lEnableAuxiliaryExtents;
}

HRESULT SVInspectionProcess::CollectOverlays(SVImageClass* p_pImage, SVExtentMultiLineStructCArray& p_rMultiLineArray)
{
	HRESULT l_Status = S_OK;

	p_rMultiLineArray.clear();

	SVToolSetClass *l_pToolSet = GetToolSet();

	if( nullptr != l_pToolSet )
	{
		l_Status = l_pToolSet->CollectOverlays(p_pImage,p_rMultiLineArray);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::AddInputImageRequestByCameraName( const SVString& rCameraName, const SVString& rFileName)
{
	HRESULT hrOk = S_OK;

	SVImageInfoClass svInfo;
	SVSmartHandlePointer svHandleStruct;
	SVInputImageRequestInfoStructPtr l_pInRequest;
	try
	{
		l_pInRequest = new SVInputImageRequestInfoStruct;
	}
	catch(...)
	{ 
		//do something here...
		l_pInRequest = nullptr;
	}

	if ( l_pInRequest )
	{
		l_pInRequest->m_bUsingCameraName = true;
		l_pInRequest->m_ObjectName = rCameraName;
		SVImageProcessingClass::LoadImageBuffer( rFileName.c_str(), l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr );

		AddInputImageRequest(l_pInRequest);
	}

	return hrOk;
}

SVProductInfoStruct SVInspectionProcess::LastProductGet( SVDataManagerLockTypeEnum p_LockType ) const
{
	SVProductInfoStruct l_Temp;

	SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

	l_AutoLock.Assign( m_LastRunLockPtr.get() );

	l_Temp.Assign( m_svLastRunProduct, p_LockType );

	if( m_LastRunProductNULL )
	{
		l_Temp.Reset();
	}

	return l_Temp;
}

HRESULT SVInspectionProcess::LastProductUpdate( SVProductInfoStruct *p_psvProduct )
{
	HRESULT l_hrOk = S_OK;

	m_LastRunProductNULL = nullptr == p_psvProduct;

	if( ! m_LastRunProductNULL )
	{
		SVAutoLockAndReleaseTemplate< SVCriticalSection > l_AutoLock;

		l_AutoLock.Assign( m_LastRunLockPtr.get() );

		l_hrOk = m_svLastRunProduct.Assign( *p_psvProduct, SV_LAST_INSPECTED );

		m_svLastRunProduct.SetProductComplete();
	}

	return l_hrOk;
}

HRESULT SVInspectionProcess::LastProductCopySourceImagesTo( SVProductInfoStruct *p_psvProduct )
{
	HRESULT Result = S_OK;

	if( nullptr != p_psvProduct )
	{
		SVProductInfoStruct l_Product = LastProductGet( SV_OTHER );

		SVGuidSVCameraInfoStructMap::iterator Iter( p_psvProduct->m_svCameraInfos.begin() );

		for( ; Iter != p_psvProduct->m_svCameraInfos.end(); ++Iter )
		{
			SVGuidSVCameraInfoStructMap::iterator LastIter( l_Product.m_svCameraInfos.find( Iter->first ) );

			if( LastIter != l_Product.m_svCameraInfos.end() )
			{
				BOOL Copied( false );
				SVVirtualCamera* pCamera( dynamic_cast<SVVirtualCamera*> (SvOi::getObject( Iter->first )) );
				if( nullptr != pCamera )
				{
					Copied = pCamera->CopyValue( LastIter->second.GetSourceImageDMIndexHandle(), Iter->second.GetSourceImageDMIndexHandle() );
				}
				if( !Copied )
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

	::InterlockedIncrement( &m_NotifyWithLastInspected );

	m_AsyncProcedure.Signal( nullptr );

	return l_hrOk;
}

HRESULT SVInspectionProcess::SetSourceImagesTo( SVProductInfoStruct *p_psvProduct )
{
	HRESULT Result( S_OK );

	if( nullptr != p_psvProduct )
	{
		SVGuidSVCameraInfoStructMap::iterator Iter( p_psvProduct->m_svCameraInfos.begin() );

		for( ; Iter != p_psvProduct->m_svCameraInfos.end(); ++Iter )
		{
			bool IndexReserved( false );
			SVVirtualCamera* pCamera( dynamic_cast<SVVirtualCamera*> (SvOi::getObject( Iter->first )) );
			if( nullptr != pCamera )
			{
				IndexReserved = pCamera->ReserveImageHandleIndex( Iter->second.GetSourceImageDMIndexHandle() );
			}
			if( !IndexReserved )
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

	while( l_Iter != m_CameraImages.end() )
	{
		SVCameraImageTemplate* l_pMainImage = ( *l_Iter );

		if( nullptr == dynamic_cast<SVRGBMainImageClass*>(l_pMainImage) )
		{
			if( nullptr != l_pMainImage )
			{
				::KeepPrevError( l_svOk, l_pMainImage->RestoreMainImage( this ) );
			}
			else
			{
				l_svOk = S_FALSE;
			}
		}

		++l_Iter;
	}

	::KeepPrevError( l_svOk, RunOnce() );

	return l_svOk;
}

HRESULT SVInspectionProcess::GetNextAvailableIndexes( SVInspectionInfoStruct& p_rInspectionInfo, SVDataManagerLockTypeEnum p_LockType ) const
{
	HRESULT l_Status = S_OK;

	if( m_pResultImageCircleBuffer.empty() )
	{
		l_Status = CreateResultImageIndexManager();
	}

	if( S_OK == l_Status )
	{
		l_Status = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultImageCircleBuffer, p_LockType, p_rInspectionInfo.m_ResultImageDMIndexHandle );

#ifdef _DEBUG
		if( S_OK != l_Status )
		{
			DumpDMInfo( GetName() );
		}
#endif
	}

	return l_Status;
}

void SVInspectionProcess::DumpDMInfo( LPCTSTR p_szName ) const
{
	if( !( m_pResultImageCircleBuffer.empty() ) )
	{
		m_pResultImageCircleBuffer->Dump( p_szName );
	}
}

bool SVInspectionProcess::CheckAndResetConditionalHistory()
{
	bool Result( false );

	SVTaskObjectListClass* pToolSet = static_cast <SVTaskObjectListClass*> ( m_pCurrentToolset );
	if( nullptr != pToolSet )
	{
		std::vector<SVValueObjectReference> vecObjects;
		pToolSet->GetOutputListFiltered( vecObjects, SV_CH_CONDITIONAL | SV_CH_VALUE, false );
		if( 0 < vecObjects.size() )
		{
			SVValueObjectReferenceVector::iterator iter;
			for( iter = vecObjects.begin(); iter != vecObjects.end(); ++iter )
			{
				iter->ObjectAttributesSetRef() &= ~( SV_CH_CONDITIONAL | SV_CH_VALUE );
			}
			Result = true;
		}

		SVImageListClass listImages;
		pToolSet->GetImageList( listImages, SV_CH_IMAGE );
		int NumberOfImages = listImages.GetSize();
		if( 0 < NumberOfImages )
		{
			for( int i=0; i < NumberOfImages; i++)
			{
				SVObjectReference refImage( listImages.GetAt(i) );
				refImage->ObjectAttributesSetRef() &= ~SV_CH_IMAGE;
			}
			Result = true;
		}

	}
	return Result;
}

bool SVInspectionProcess::IsColorCamera() const
{
	bool Result( false );
	SVVirtualCamera* pCamera( GetFirstCamera() );

	if( nullptr != pCamera )
	{
		Result = pCamera->IsColor();
	}

	return Result;
}

SVVirtualCamera* SVInspectionProcess::GetFirstCamera() const
{
	SVVirtualCamera* pCamera( nullptr );

	if( !( m_ToolSetCameraName.empty() ) )
	{
		pCamera = dynamic_cast< SVVirtualCamera* >( SVObjectManagerClass::Instance().GetObjectCompleteName( m_ToolSetCameraName.c_str() ) );
	}

	if( nullptr == pCamera )
	{
		pCamera = GetFirstPPQCamera();
	}

	return pCamera;
}

SVVirtualCamera* SVInspectionProcess::GetFirstPPQCamera() const
{
	SVVirtualCamera* pCamera( nullptr );

	SVPPQObject* pPPQ = GetPPQ();

	if( nullptr != pPPQ )
	{
		SVVirtualCameraMap l_Cameras;

		pPPQ->GetVirtualCameras( l_Cameras );

		SVVirtualCameraMap::const_iterator l_Iter = l_Cameras.begin();

		if( l_Iter != l_Cameras.end() )
		{
			pCamera = l_Iter->second;
		}
	}

	return pCamera;
}

HRESULT SVInspectionProcess::GetPPQCameras( SVVirtualCameraPtrSet& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVPPQObject* pPPQ = GetPPQ();

	if( nullptr != pPPQ )
	{
		SVVirtualCameraMap l_Cameras;

		pPPQ->GetVirtualCameras( l_Cameras );

		SVVirtualCameraMap::const_iterator l_Iter = l_Cameras.begin();

		while( l_Iter != l_Cameras.end() )
		{
			if( nullptr != l_Iter->second )
			{
				p_rCameras.insert( l_Iter->second );
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

HRESULT SVInspectionProcess::GetCameras( SVVirtualCameraPtrSet& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( l_Iter != m_CameraImages.end() )
	{
		if( nullptr != ( *l_Iter ) )
		{
			SVVirtualCamera* pCamera = ( *l_Iter )->GetCamera();

			if( nullptr != pCamera )
			{
				p_rCameras.insert( pCamera );
			}
		}

		++l_Iter;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetCamerasForLut( SVVirtualCameraPtrSet& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	for (SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();l_Iter != m_CameraImages.end();++l_Iter)
	{
		if( nullptr != ( *l_Iter ) )
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if( nullptr != pCamera )
			{
				if (!pCamera->IsFileAcquisition() && nullptr != pCamera->GetAcquisitionDevice())
				{
					SVLut lut;
					HRESULT hr = pCamera->GetLut(lut);
					if (S_OK == hr && lut.NumBands() > 0)
					{
						p_rCameras.insert( pCamera );
					}
				}
			}
		}
	}
	return l_Status;
}

HRESULT SVInspectionProcess::GetCamerasForLightReference( SVVirtualCameraPtrSet& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	for (SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();l_Iter != m_CameraImages.end();++l_Iter)
	{
		if( nullptr != ( *l_Iter ) )
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if( nullptr != pCamera )
			{
				if (!pCamera->IsFileAcquisition() && nullptr != pCamera->GetAcquisitionDevice() )
				{
					SVLightReference lightReference;
					HRESULT hr = pCamera->GetLightReference(lightReference);
					if (S_OK == hr && lightReference.NumBands() > 0)
					{
						p_rCameras.insert( pCamera );
					}
				}
			}
		}
	}
	return l_Status;
}

HRESULT SVInspectionProcess::AddSharedCamera( SVVirtualCamera* pCamera )
{
	HRESULT l_Status = S_OK;

	SVPPQObject* pPPQ = GetPPQ();

	if( nullptr != pCamera && nullptr != pPPQ )
	{
		pPPQ->AddSharedCamera( pCamera );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetMainImages( const SVString& rCameraName, SVCameraImagePtrSet& rMainImages ) const
{
	HRESULT l_Status = S_OK;

	rMainImages.clear();

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( l_Iter != m_CameraImages.end() )
	{
		if( nullptr != ( *l_Iter ) )
		{
			SVVirtualCamera* pCamera = ( *l_Iter )->GetCamera();

			if( ( nullptr != pCamera ) && ( rCameraName == pCamera->GetName() ) )
			{
				rMainImages.insert( *l_Iter );
			}
		}

		++l_Iter;
	}

	return l_Status;
}

void SVInspectionProcess::UpdateSharedMemoryFilters( const SVMonitorList& p_rMonitorList )
{
	m_MonitorListQueue.clear();
	m_MonitorListQueue.AddTail( p_rMonitorList );
	m_AsyncProcedure.Signal( nullptr );
}

HRESULT SVInspectionProcess::RemoveImage(SVImageClass* pImage)
{
	HRESULT l_Status = S_OK;

	if( nullptr != pImage )
	{
		SVRemoveImages l_Data;

		l_Data.m_Images.insert( pImage->GetUniqueObjectID() );

		SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cInspectionProcessTag ), GetUniqueObjectID(), l_Data );
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

BOOL SVInspectionProcess::CreateObject( SVObjectLevelCreateStruct* PCreateStruct )
{
	BOOL l_bOk = SVObjectClass::CreateObject( PCreateStruct );

	SVInspectionLevelCreateStruct createStruct;

	createStruct.OwnerObjectInfo = this;
	createStruct.InspectionObjectInfo	= this;

	l_bOk = l_bOk && m_pCurrentToolset->createAllObjects(createStruct);

	m_isCreated = l_bOk;

	return m_isCreated;
}

HRESULT SVInspectionProcess::ResetObject()
{
	HRESULT l_hrOk = S_OK;

	if( IsColorCamera() )
	{
		if( S_OK != m_rgbMainImageObject.ResetObject() )
		{
			l_hrOk = S_FALSE;
		}
	}

	if( nullptr == m_pCurrentToolset || !m_pCurrentToolset->resetAllObjects(true, false) )
	{
		l_hrOk = S_FALSE;
	}

	BuildValueObjectMap();

	m_bForceOffsetUpdate = true;

	return l_hrOk;
}

void SVInspectionProcess::SetInvalid()
{
	SVObjectClass::SetInvalid();

	if( GetToolSet() )
	{
		GetToolSet()->SetInvalid();
	}
}

int SVInspectionProcess::UpdateMainImagesByProduct( SVProductInfoStruct* p_psvProduct )
{
	int Result( 0 );

	SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

	while( l_ImageIter != m_CameraImages.end() )
	{
		SVCameraImageTemplate* l_pImage = ( *l_ImageIter );

		if( nullptr != l_pImage )
		{
			SVVirtualCamera* pCamera = l_pImage->GetCamera();

			if( nullptr != pCamera )
			{
				SVGuidSVCameraInfoStructMap::const_iterator Iter;

				Iter = p_psvProduct->m_svCameraInfos.find( pCamera->GetUniqueObjectID() );

				if( Iter != p_psvProduct->m_svCameraInfos.end() )
				{
					if( Iter->second.GetIndex() < 0 )
					{
						Result = SvOi::Err_25043_InvalidSourceIndex;
					}

					SVImageIndexStruct l_svIndex;

					l_svIndex.m_CameraDMIndexHandle.Assign( Iter->second.GetSourceImageDMIndexHandle(), SV_INSPECTION );

					if( !( l_pImage->SetImageHandleIndex( l_svIndex ) ) )
					{
						Result = SvOi::Err_25042_SetImageHandleIndex;
					}
				}
				else
				{
					l_pImage->ResetImageIndex();

					Result = SvOi::Err_25044_CameraNotFound;
				}
			}
			else
			{
				Result = SvOi::Err_25045_InvalidCamera;
			}
		}
		else
		{
			Result = SvOi::Err_25046_InvalidImage;
		}

		++l_ImageIter;
	}

	return Result;
}

BOOL SVInspectionProcess::OnValidate()
{
	BOOL retVal = SVObjectClass::OnValidate();

	retVal = retVal && GetToolSet() && GetToolSet()->IsCreated();

	if( !retVal )
		SetInvalid();

	return retVal;
}

BOOL SVInspectionProcess::Validate()
{
	//
	// Broadcast: 'going on line' - 08 Feb 2000 - frb.
	//

	BOOL retVal = SVObjectClass::Validate();

	// Prep the conditonal
	// The equation should only be executed when the following items are true:
	// 1. The ToolSet is Enabled
	// 2. The Condition is Enabled
	// The purpose for these checks are that the operator can start to build 
	// an equation and disable either the Toolset or the Condition.
	// This will leave the equation in a not validated state, since validation
	// of the Equation only occurs ( in the SVConditionalDialogClass::OnOK() )
	// when both the ToolSet and the Condition are NOT disabled.

	retVal = retVal && GetToolSet()->Validate();

	return retVal;
}

HRESULT SVInspectionProcess::OnlyCopyForward( SVRunStatusClass& rRunStatus)
{
	HRESULT hRet = S_OK;

	SVValueObjectClassPtrSet::iterator l_oValueIter;

	for( l_oValueIter = m_svValueObjectSet.begin(); l_oValueIter != m_svValueObjectSet.end(); ++l_oValueIter )
	{
		(*l_oValueIter)->CopyLastSetValue( rRunStatus.m_lResultDataIndex );
	}

	SVImageClassPtrSet::iterator l_oImageIter;

	for( l_oImageIter = m_svImageObjectSet.begin(); l_oImageIter != m_svImageObjectSet.end(); ++l_oImageIter )
	{
		(*l_oImageIter)->CopyImageTo( rRunStatus.Images );
	}


	return hRet;
}

BOOL SVInspectionProcess::Run( SVRunStatusClass& RRunStatus )
{
	// Validate IPDoc's values...
	BOOL retVal = OnValidate();

	if( retVal )
	{
		if( !GetNewDisableMethod() )
		{
			SVValueObjectClassPtrSet::iterator l_oValueIter;

			for( l_oValueIter = m_svValueObjectSet.begin(); l_oValueIter != m_svValueObjectSet.end(); ++l_oValueIter )
			{
				(*l_oValueIter)->CopyLastSetValue( RRunStatus.m_lResultDataIndex );
			}

			SVImageClassPtrSet::iterator l_oImageIter;

			for( l_oImageIter = m_svImageObjectSet.begin(); l_oImageIter != m_svImageObjectSet.end(); ++l_oImageIter )
			{
				(*l_oImageIter)->CopyImageTo( RRunStatus.Images );
			}
		}// end if

		// Run the Toolset
		unsigned long l_state = m_runStatus.GetState();
		if( GetToolSet() && l_state != SVRunStatusClass::SV_INVALID )
		{
			retVal &= GetToolSet()->Run( RRunStatus );
		}
		else
		{
			RRunStatus.SetInvalid();
			RRunStatus.SetInspectionStarted(false);
			if( SVRunStatusClass::SV_INVALID == m_runStatus.GetState() )
			{
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_37_INVALID_RUN_STATUS, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
			}
		}
	}
	else
	{
		RRunStatus.SetInvalid();
		RRunStatus.SetInspectionStarted(false);
	}

	return retVal;
}

BOOL SVInspectionProcess::RunInspection( long lResultDataIndex, SVImageIndexStruct svResultImageIndex, SVProductInfoStruct *pProduct, bool p_UpdateCounts )
{
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
	double del = SvTl::setReferenceTime();
	SVString DebugString = SvUl_SF::.Format(_T("!\n!!Reset, %7.1lf: SVInspectionProcess::RunInspection(), del = %7.1lf\n"), SvTl::GetRelTimeStamp() ,del);
	::OutputDebugString(DebugString.c_str());
#endif

	BOOL l_bOk = FALSE;
	BOOL l_bInputRequest = FALSE;
	BOOL l_bImageRequest = FALSE;
	BOOL l_bUpdateMainImage = FALSE;
	BOOL l_bRestMainImage = FALSE;
	SvStl::MessageMgrStd Exception( SvStl::LogOnly );

	// Get the info struct for this inspection
	SVInspectionInfoStruct& l_rIPInfo = pProduct->m_svInspectionInfos[ GetUniqueObjectID() ];

	m_runStatus.ResetRunStateAndToolSetTimes();

	m_runStatus.m_PreviousTriggerTime = m_runStatus.m_CurrentTriggerTime;
	m_runStatus.m_CurrentTriggerTime  = pProduct->oTriggerInfo.m_BeginProcess;

	m_runStatus.m_lResultDataIndex = lResultDataIndex;
	m_runStatus.Images = svResultImageIndex;
	m_runStatus.m_lTriggerCount = pProduct->oTriggerInfo.lTriggerCount;
	m_runStatus.m_UpdateCounters = p_UpdateCounts;

	if ( pProduct->IsAlive() )
	{
		// Process all input requests
		if( !ProcessInputRequests( lResultDataIndex, m_bForceOffsetUpdate ) )
		{
			Exception.setMessage( SVMSG_SVO_38_INPUT_REQUEST_FAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );

			l_bInputRequest = TRUE;
			m_runStatus.SetInvalid();  //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		if( !ProcessInputImageRequests( pProduct ) )
		{
			Exception.setMessage( SVMSG_SVO_39_IMAGE_REQUEST_FAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );

			l_bImageRequest = TRUE;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		int MainImageStatus = UpdateMainImagesByProduct( pProduct );

		if( 0 != MainImageStatus )
		{
			Exception.setMessage( SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams), MainImageStatus );

			l_bUpdateMainImage = TRUE;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}// end if
	}
	else
	{
		//Product !bAlive
		SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

		while( l_ImageIter != m_CameraImages.end() )
		{
			SVCameraImageTemplate* l_pImage = ( *l_ImageIter );

			if( nullptr != l_pImage )
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
	if( !GetToolSet()->IsEnabled() )
	{
		l_rIPInfo.oInspectedState = PRODUCT_INSPECTION_DISABLED;
	}// end if

	l_rIPInfo.m_BeginToolset = SVClock::GetTimeStamp();
	if (!l_rIPInfo.m_BeginToolset)
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17027_ErrorGettingTimeStamp );
		DebugBreak();
	}

	if ( !l_bInputRequest && !l_bImageRequest && !l_bUpdateMainImage && !l_bRestMainImage )
	{
		l_bOk = Run( m_runStatus );
	}
	else
	{
		OnlyCopyForward( m_runStatus );
	}

	m_bForceOffsetUpdate = false;

	l_rIPInfo.m_EndToolset = SVClock::GetTimeStamp();
	if (!l_rIPInfo.m_EndToolset)
	{
		SvStl::MessageMgrStd e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingTimeStamp, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_17028_ErrorGettingTimeStamp );
		DebugBreak();
	}

	if ( m_runStatus.IsValid() && ! m_runStatus.IsCriticalFailure() )
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
	if( m_pCurrentToolset )
	{
		m_pCurrentToolset->GetAllInputObjects();
	}// end if

	ConnectAllInputs();

	// Setup Connections
	if( m_pCurrentToolset )
	{
		m_pCurrentToolset->SetDefaultInputs();
	}// end if

	BuildValueObjectMap();

	// Update published List
	GetPublishList().Refresh( m_pCurrentToolset );
}

BOOL SVInspectionProcess::SetObjectDepth( int NewObjectDepth )
{
	BOOL l_bOk = SVObjectClass::SetObjectDepth( NewObjectDepth );

	l_bOk &= nullptr != GetToolSet() && GetToolSet()->SetObjectDepth( NewObjectDepth );

	return l_bOk;
}

BOOL SVInspectionProcess::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_bOk = SVObjectClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	l_bOk &= nullptr != GetToolSet() && GetToolSet()->SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	return l_bOk;
}

BOOL SVInspectionProcess::SetImageDepth( long lDepth )
{
	BOOL l_bOk = SVObjectClass::SetImageDepth( lDepth );

	l_bOk &= nullptr != GetToolSet() && GetToolSet()->SetImageDepth( lDepth );

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

	for( l_Iter = m_PPQInputs.begin(); l_Iter != m_PPQInputs.end(); ++l_Iter )
	{
		const SVIOEntryStruct& l_rIOEntry = *l_Iter;

		if( !( l_rIOEntry.m_IOEntryPtr.empty() ) )
		{
			if( nullptr != l_rIOEntry.m_IOEntryPtr->m_pValueObject )
			{
				l_Objects.push_back( l_rIOEntry.m_IOEntryPtr->m_pValueObject );
			}
		}
	}

	if( IsColorCamera() )
	{
		SVRGBMainImageClass* l_pImage = const_cast< SVRGBMainImageClass* >( &m_rgbMainImageObject );

		l_Objects.push_back( l_pImage );
	}

	if( nullptr != GetToolSet() )
	{
		l_Objects.push_back( GetToolSet() );
	}

	return l_Objects;
}

SVObjectClass *SVInspectionProcess::UpdateObject( const GUID &p_oFriendGuid, SVObjectClass *p_psvObject, SVObjectClass *p_psvNewOwner )
{
	p_psvObject->SetObjectOwner( p_psvNewOwner );

	return p_psvObject ; //l_psvObject;
}

SVImageClass* SVInspectionProcess::GetRGBMainImage()
{
	SVImageClass* pResult( nullptr );

	if( IsColorCamera() )
	{
		pResult = GetToolSet()->getCurrentImage();
	}

	return pResult;
}

SVImageClass* SVInspectionProcess::GetHSIMainImage()
{
	SVImageClass* pResult( nullptr );

	if( IsColorCamera() )
	{
		pResult  = &m_rgbMainImageObject;
	}

	return pResult;
}

LPCTSTR SVInspectionProcess::GetDeviceName() const
{
	return m_DeviceName.c_str();
}

void SVInspectionProcess::SetDeviceName( LPCTSTR p_szDeviceName )
{
	m_DeviceName = p_szDeviceName;
}

void SVInspectionProcess::ResetName()
{
	SVObjectClass::ResetName();

	SVInspectionNameUpdate l_Data( GetName() );

	SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cInspectionProcessTag ), GetUniqueObjectID(), l_Data );
}

void SVInspectionProcess::SetName( LPCTSTR StrString )
{
	SVObjectClass::SetName( StrString );

	SVInspectionNameUpdate l_Data( GetName() );

	SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cInspectionProcessTag ), GetUniqueObjectID(), l_Data );
}

HRESULT SVInspectionProcess::RegisterSubObject( SVObjectClass* pObject )
{
	HRESULT hr = S_FALSE;
	if (SVImageObjectType == pObject->GetObjectInfo().ObjectTypeInfo.ObjectType)
	{
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);
		if (pImage)
		{
			hr = RegisterSubObject(pImage);
		}
	}
	else
	{
		SVValueObjectClass* pValue = dynamic_cast<SVValueObjectClass*>(pObject);
		if (pValue)
		{
			hr = RegisterSubObject(pValue);
		}
	}
	return hr;
}

HRESULT SVInspectionProcess::UnregisterSubObject( SVObjectClass* pObject )
{
	HRESULT hr = S_FALSE;
	if (SVImageObjectType == pObject->GetObjectInfo().ObjectTypeInfo.ObjectType)
	{
		SVImageClass* pImage = dynamic_cast<SVImageClass*>(pObject);
		if (pImage)
		{
			hr = UnregisterSubObject(pImage);
		}
	}
	else
	{
		SVValueObjectClass* pValue = dynamic_cast<SVValueObjectClass*>(pObject);
		if (pValue)
		{
			hr = UnregisterSubObject(pValue);
		}
	}
	return hr;
}

HRESULT SVInspectionProcess::RegisterSubObject( SVValueObjectClass* p_pValueObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pValueObject )
	{
		m_svValueObjectSet.insert( p_pValueObject );
		m_mapValueObjects.insert( SVValueObjectMap::value_type( p_pValueObject->GetCompleteName(), p_pValueObject ) );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::RegisterSubObject( SVImageClass* p_pImageObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pImageObject )
	{
		SVCameraImageTemplate* l_pCameraImage = dynamic_cast< SVCameraImageTemplate* >( p_pImageObject );

		if( nullptr != l_pCameraImage )
		{
			m_CameraImages.insert( l_pCameraImage );
		}
		else
		{
			m_svImageObjectSet.insert( p_pImageObject );
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::UnregisterSubObject( SVValueObjectClass* p_pValueObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pValueObject )
	{
		if( !( m_svValueObjectSet.empty() ) )
		{
			m_svValueObjectSet.erase( p_pValueObject );
		}

		m_mapValueObjects.get< to >().erase( p_pValueObject );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::UnregisterSubObject( SVImageClass* p_pImageObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pImageObject )
	{
		SVCameraImageTemplate* l_pCameraImage = dynamic_cast< SVCameraImageTemplate* >( p_pImageObject );

		if( nullptr != l_pCameraImage )
		{
			if( !( m_CameraImages.empty() ) )
			{
				m_CameraImages.erase( l_pCameraImage );
			}
		}
		else
		{
			if( !( m_svImageObjectSet.empty() ) )
			{
				m_svImageObjectSet.erase( p_pImageObject );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVInspectionProcess::SetToolsetImage( const SVString& rToolsetImage )
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

	if( nullptr != l_pToolSet )
	{
		l_pImage = dynamic_cast< SVCameraImageTemplate* >( l_pToolSet->getCurrentImage() );
	}

	return l_pImage;
}

HRESULT SVInspectionProcess::GetInspectionImage( LPCTSTR Name, SVImageClass*& p_rRefObject )
{
	// Set to Defaults in case of failure
	p_rRefObject = nullptr;

	// Specify that we are looking only for images
	SVObjectTypeInfoStruct imageObjectInfo;
	imageObjectInfo.ObjectType = SVImageObjectType;

	SVGetObjectDequeByTypeVisitor l_Visitor( imageObjectInfo );

	SVObjectManagerClass::Instance().VisitElements( l_Visitor, GetUniqueObjectID() );

	SVGetObjectDequeByTypeVisitor::SVObjectPtrDeque::const_iterator l_Iter;

	for( l_Iter = l_Visitor.GetObjects().begin(); l_Iter != l_Visitor.GetObjects().end(); ++l_Iter )
	{
		SVImageClass* pImage = dynamic_cast< SVImageClass* >( const_cast< SVObjectClass* >( *l_Iter ) );

		if( pImage->GetCompleteName() == Name )
		{
			// We found the image by name
			p_rRefObject = pImage;

			return S_OK;
		}
	}
	return E_FAIL;
}

// This method fills the Toolset structure in Shared Memory (for either LastInspected or Rejects )
void SVInspectionProcess::FillSharedData(long sharedSlotIndex, SvSml::SVSharedData& rData, const SVFilterValueMap& rValues, const SVFilterImageMap& rImages, SVProductInfoStruct& rProductInfo, SvSml::SVSharedInspectionWriter& rWriter)
{
	SVProductInfoStruct& l_rProductInfo = rProductInfo;

	SVDataManagerHandle dmHandle = l_rProductInfo.oPPQInfo.m_ResultDataDMIndexHandle;
	long dataIndex = dmHandle.GetIndex();

	SvSml::SVSharedValueContainer& rSharedValues = rData.m_Values;
	SvSml::SVSharedImageContainer& rSharedImages = rData.m_Images;

	for (SvSml::SVSharedValueContainer::iterator it = rSharedValues.begin(); it != rSharedValues.end(); ++it)
	{
		HRESULT hr = S_OK;
		SVString Value;
		bool bItemNotFound = true;
		SVFilterValueMap::const_iterator ValueIter = rValues.find(it->m_ElementName.c_str());
		if (ValueIter != rValues.end())
		{
			const SVValueObjectReference& ref = ValueIter->second;
			const SVValueObjectClass* pValueObject = ref.Object();

			if (pValueObject)
			{
				// for now just a single item (no full array)
				if (!ref.IsEntireArray())
				{
					HRESULT hrGet = ref.GetValue(dataIndex, Value);
					if (SVMSG_SVO_33_OBJECT_INDEX_INVALID == hrGet || SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE == hrGet)
					{
						hr = hrGet;
						Value = ref.DefaultValue(); // did not get value. set value to default
						if( Value.empty() )
						{
							Value = SvUl_SF::Format( _T("%i"), -1 );
						}
					}
					else if (S_OK != hrGet)	// some generic error; currently should not get here
					{
						hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
						Value = SvUl_SF::Format( _T("%i"), -1 ); // did not get value. set value to -1
					}
					else if (S_OK == hrGet)
					{
						bItemNotFound = false;
					}
				}
				else // Get Entire Array
				{
					// get all results and put them into a parsable string
					int iNumResults = 0;
					ref.Object()->GetResultSize(dataIndex, iNumResults);
					SVString ArrayValues;
					for (int iArrayIndex = 0;iArrayIndex < iNumResults && S_OK == hr;iArrayIndex++)
					{
						SVString Value;
						hr = ref.Object()->GetValue(dataIndex, iArrayIndex, Value);
						if (S_OK == hr)
						{
							if (iArrayIndex > 0)
							{
								ArrayValues += _T(",");
							}
							ArrayValues += SvUl_SF::Format( _T("`%s`"), Value.c_str() );
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

	for (SvSml::SVSharedImageContainer::iterator it = rSharedImages.begin(); it != rSharedImages.end(); ++it)
	{
		HRESULT hr = S_OK;
		bool bImgNotFound = true;
		SVFilterImageMap::const_iterator imageIter = rImages.find(it->m_ElementName.c_str());
		if (imageIter != rImages.end())
		{
			SVImageClass* pImage = dynamic_cast<SVImageClass*>(imageIter->second.Object());

			if (nullptr != pImage)
			{
				SVSmartHandlePointer imageHandlePtr;
			
				// Special check for Color Tool's RGBMainImage which is HSI
				if (SV_IS_KIND_OF(pImage, SVRGBMainImageClass))
				{
					// this will make a copy...
					SVImageProcessingClass::CreateImageBuffer(pImage->GetImageInfo(), imageHandlePtr);
					pImage->SafeImageConvertToHandle(imageHandlePtr, SVImageHLSToRGB);
				}
				else
				{
					pImage->GetImageHandle(imageHandlePtr);
				}

				if (!imageHandlePtr.empty())
				{
					bImgNotFound = false;
					// Add Drive and Directory to base filename
					// What a horrible unsafe practice and programmer trap!
					// m_SecondPtrImageFileName points to m_BufferImageFileName, sort of
					// You must use m_BufferImageFileName and not m_SecondPtrImageFileName after calling SvSml::SVSharedImage::BuildImageFileName
					SVString name = imageIter->first.c_str();
					SvSml::SVSharedImage::BuildImageFileName(m_SecondPtrImageFileName, m_SecondPtrImageFileNameLen, name.c_str(), sharedSlotIndex, false, SVFileBitmap);
					// Write Image to disk
					HRESULT hr = SVImageProcessingClass::SaveImageBuffer(m_BufferImageFileName, SVFileBitmap, imageHandlePtr);
					if(S_OK != hr)
					{
						SvStl::MessageMgrStd MesMan( SvStl::LogOnly );
						MesMan.setMessage( SVMSG_RRS_4_GENERAL_WARNING, SvOi::Tid_FailedToSaveImage, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_16220_SaveImageBuffer);
					}

#if defined (TRACE_THEM_ALL) || defined (TRACE_SHARED)
					TRACE1( "Produktstate: %s \n",  l_rProductInfo.m_ProductState.c_str());
					TRACE3( "Save Image buffer %s   index %i  result %i\n", m_BufferImageFileName, sharedSlotIndex, hr);
#endif 
					it->SetData(m_BufferImageFileName, hr);
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
: m_StartTime( 0 ), m_Start(), m_End(), m_Duration()
{
}

SVInspectionProcess::SVInspectionTrackingElement::SVInspectionTrackingElement( const SVInspectionTrackingElement& p_rObject )
: m_StartTime( p_rObject.m_StartTime ), m_Start( p_rObject.m_Start ), m_End( p_rObject.m_End ), m_Duration( p_rObject.m_Duration )
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
: m_StartTime( 0.0 ), m_EventCounts()
{
}

SVInspectionProcess::SVInspectionTracking::SVInspectionTracking( const SVInspectionTracking& p_rObject )
: m_StartTime( p_rObject.m_StartTime ), m_EventCounts( p_rObject.m_EventCounts )
{
}

SVInspectionProcess::SVInspectionTracking::~SVInspectionTracking()
{
	m_StartTime = 0;

	if( !( m_EventCounts.empty() ) )
	{
		m_EventCounts.clear();
	}
}

void SVInspectionProcess::SVInspectionTracking::clear()
{
	if( !( m_EventCounts.empty() ) )
	{
		m_EventCounts.clear();
	}
}

void SVInspectionProcess::SVInspectionTracking::SetStartTime()
{
	m_StartTime = SVClock::GetTimeStamp();
}

void SVInspectionProcess::SVInspectionTracking::EventStart( const SVString& p_rName )
{
	SVClock::SVTimeStamp l_StartTime = SVClock::GetTimeStamp();
	__int64 l_EventTime = static_cast< __int64 >( l_StartTime - m_StartTime );

	m_EventCounts[ p_rName ].m_StartTime = l_StartTime;

	++( m_EventCounts[ p_rName ].m_Start[ l_EventTime ] );
}

void SVInspectionProcess::SVInspectionTracking::EventEnd( const SVString& p_rName )
{
	SVClock::SVTimeStamp l_EndTime = SVClock::GetTimeStamp();
	__int64 l_Duration = static_cast< __int64 >( l_EndTime - m_EventCounts[ p_rName ].m_StartTime );
	__int64 l_EventTime = static_cast< __int64 >( l_EndTime - m_StartTime );

	++( m_EventCounts[ p_rName ].m_End[ l_EventTime ] );
	++( m_EventCounts[ p_rName ].m_Duration[ l_Duration ] );
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
	value.lVal = GetNewDisableMethod();
	rWriter.WriteAttribute(CTAG_INSPECTION_NEW_DISABLE_METHOD, value);
	value.Clear();

	// Save EnableAuxillaryExtents
	value = GetEnableAuxiliaryExtent();
	rWriter.WriteAttribute(CTAG_INSPECTION_ENABLE_AUXILIARY_EXTENT, value);
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
	if(pToolSet)
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
SvOi::ISelectorItemVectorPtr SVInspectionProcess::GetPPQSelectorList( const UINT Attribute ) const
{
	SvOsl::SelectorItemVector *pSelectorList = new SvOsl::SelectorItemVector();
	SvOi::ISelectorItemVectorPtr Result = static_cast<SvOi::ISelectorItemVector*> (pSelectorList);

	SVPPQObject *pPPQ = GetPPQ();
	if( nullptr != pPPQ )
	{
		SVString PpqName( pPPQ->GetName() );

		SVObjectReferenceVector ObjectList;
		SVObjectManagerClass::Instance().getTreeList( PpqName, ObjectList, Attribute );

		std::for_each( ObjectList.begin(), ObjectList.end(), [&pSelectorList](const SVObjectReference& rObjectRef)->void
		{
			SvOsl::SelectorItem InsertItem;

			InsertItem.setName( rObjectRef.GetName().c_str() );
			InsertItem.setItemKey( rObjectRef->GetUniqueObjectID().ToVARIANT() );
			if ( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (rObjectRef.Object()) )
			{
				InsertItem.setItemTypeName( pValueObject->GetTypeName().c_str() );
			}
			else if( const BasicValueObject* pBasicObject = dynamic_cast<const BasicValueObject*> (rObjectRef.Object()) )
			{
				InsertItem.setItemTypeName( pBasicObject->getTypeName().c_str() );
			}
			if( rObjectRef->IsArray() )
			{
				// add array elements
				int iArraySize = rObjectRef->GetArraySize();
				for ( int i = 0; i < iArraySize; i++ )
				{
					//We make a copy to be able to set the index rObjectRef is const
					SVObjectReference ArrayObjectRef( rObjectRef );
					ArrayObjectRef.SetArrayIndex( i );
					InsertItem.setLocation( ArrayObjectRef.GetCompleteOneBasedObjectName().c_str() );
					InsertItem.setArrayIndex( i );
					pSelectorList->push_back( InsertItem );
				}
			}
			else
			{
				InsertItem.setLocation( rObjectRef.GetCompleteOneBasedObjectName().c_str() );
				pSelectorList->push_back( InsertItem );
			}
		});
	}


	SVString InspectionName = GetName();

	SVObjectVector PpqVariables;
	PpqVariables = getPPQVariables();
	std::for_each( PpqVariables.begin(), PpqVariables.end(), [&pSelectorList, &InspectionName, &Attribute](SVObjectClass* pObject)->void
	{
		if( nullptr != pObject )
		{
			//Check if the attribute of the object is allowed
			if( 0 != (Attribute & pObject->ObjectAttributesAllowed())  )
			{
				SVObjectReference ObjectRef( pObject );
				SvOsl::SelectorItem InsertItem;

				SVString Location( ObjectRef.GetCompleteOneBasedObjectName() );
				InsertItem.setName( ObjectRef.GetName().c_str() );
				InsertItem.setLocation( Location.c_str() );
				//Need to replace the inspection name with the PPQ Variables name
				// Only DIO and Remote Input, but is all that is in this list?
				SvUl_SF::searchAndReplace(Location, InspectionName.c_str(), SvOl::FqnPPQVariables);
				InsertItem.setDisplayLocation( Location.c_str() );
				InsertItem.setItemKey( ObjectRef->GetUniqueObjectID().ToVARIANT() );
				if ( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (pObject) )
				{
					InsertItem.setItemTypeName( pValueObject->GetTypeName().c_str() );
				}

				pSelectorList->push_back( InsertItem );
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
	SVProductInfoStruct l_Product = LastProductGet( SV_INSPECTION );

	if( !( l_Product.empty() ) )
	{
		SVDataManagerHandle l_Handle;
		l_Product.GetResultDataIndex( l_Handle );
		lastIndex = l_Handle.GetIndex();
	}
	return lastIndex;
}

bool SVInspectionProcess::AddInputRequest( const SVGUID& rGuid, const _variant_t& rValue )
{
	SVValueObjectReference ObjectRef( SVObjectManagerClass::Instance().GetObject( rGuid ) );
	bool Result = AddInputRequest( ObjectRef, rValue );

	return Result;
}

bool SVInspectionProcess::AddInputRequestMarker()
{
	return AddInputRequest( nullptr, SvO::SVTOOLPARAMETERLIST_MARKER );
}
#pragma endregion IInspectionProcess methods

bool SVInspectionProcess::IsEnabledPPQVariable(SVValueObjectClass* pValueObject)
{
	for( size_t i = 0; i < m_PPQInputs.size(); i++ )
	{	
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;
		if( ioEntryPtr->m_Enabled )
		{
			if(ioEntryPtr->m_pValueObject == pValueObject)
			{
				return true;
			}
		}
	}// end for
	return false;
}

bool SVInspectionProcess::IsDisabledPPQVariable(SVValueObjectClass* pValueObject)
{
	for( size_t i = 0; i < m_PPQInputs.size(); i++ )
	{	
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;

		if( false == ioEntryPtr->m_Enabled )
		{
			if(ioEntryPtr->m_pValueObject == pValueObject)
			{
				return true;
			}
		}
	}// end for
	return false;
}

SVObjectVector SVInspectionProcess::getPPQVariables() const
{
	SVObjectVector Result;
	std::map< SVString, SVObjectClass* > NameObjectMap;
	SVStringVector PpqVariableNames;

	for( size_t i = 0; i < m_PPQInputs.size(); i++ )
	{	
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;

		//check if input is enable for this inspection
		if( ioEntryPtr->m_Enabled )
		{
			SVObjectClass* pObject( ioEntryPtr.get()->m_pValueObject );
			if( nullptr != pObject )
			{
				SVString Name( pObject->GetCompleteName() );
				NameObjectMap[Name] = pObject;
				PpqVariableNames.push_back( Name );
			}
		}
	}// end for

	//Sort the PPQ variable names and return the respective SVObjectClass pointers in order
	std::sort( PpqVariableNames.begin(), PpqVariableNames.end(), SvUl::NaturalStringCompare<SVString>()); // sort strings
	std::for_each(PpqVariableNames.begin(), PpqVariableNames.end(), [&Result, &NameObjectMap](const SVString& rItem)->void
	{
		Result.push_back( NameObjectMap[rItem] );
	}
	);

	return Result;
}

DWORD SVInspectionProcess::GetObjectColor() const
{
	return SV_DEFAULT_WHITE_COLOR;
}



bool   SVInspectionProcess::LoopOverTools(pToolFunc pf, int& counter ) 
{
	bool ret = false;
	counter =0;
	SVToolSetClass* pToolSet(nullptr);
	SVTaskObjectClass* pTaskObject(nullptr);
	pToolSet = GetToolSet();
	if(pToolSet)
	{
		ret = true;
		for(int index =0; index < pToolSet->GetSize(); index ++)
		{
			pTaskObject = pToolSet->GetAt(index);
			if(nullptr != pTaskObject)
			{
				int result = pf(pTaskObject);
				if(result < 0)
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

void SVInspectionProcess::clearToolMessages()
{
	SVToolSetClass* pToolSet( GetToolSet() );

	if( nullptr != pToolSet )
	{
		for(int i =0; i < pToolSet->GetSize(); i++)
		{
			SVTaskObjectClass* pTaskObject( pToolSet->GetAt(i) );
			if(nullptr != pTaskObject)
			{
				pTaskObject->clearTaskMessages();
			}
		}		
	}
}

void SVInspectionProcess::getToolMessages( SvStl::MessageContainerInserter& rInserter ) const
{
	SVToolSetClass* pToolSet( GetToolSet() );

	if( nullptr != pToolSet )
	{
		for(int i =0; i < pToolSet->GetSize(); i++)
		{
			SVTaskObjectClass* pTaskObject( pToolSet->GetAt(i) );
			if(nullptr != pTaskObject)
			{
				const SvStl::MessageContainerVector& rToolMessages( pTaskObject->getTaskMessages() );
				std::copy( rToolMessages.begin(), rToolMessages.end(), rInserter );
			}
		}		
	}
}

bool SVInspectionProcess::DisconnectObjectInput( SVInObjectInfoStruct* pObjectInInfo )
{
	bool Result = m_publishList.RemovePublishedEntry( pObjectInInfo->GetInputObjectInfo().UniqueObjectID );

	if ( nullptr != GetToolSet())
	{
		return GetToolSet()->DisconnectObjectInput(pObjectInInfo);
	}
	return Result;
}

bool SVInspectionProcess::createAllObjects( const SVObjectLevelCreateStruct& rCreateStructure )
{
	bool Result = __super::createAllObjects( rCreateStructure );

	if ( nullptr != GetToolSet())
	{
		Result &= GetToolSet()->createAllObjects(rCreateStructure);
	}
	return Result;
}

bool SVInspectionProcess::CreateChildObject( SVObjectClass* pChildObject, DWORD context )
{
	if (nullptr != pChildObject)
	{
		long l_LastIndex = 1;
		SVProductInfoStruct l_Product = LastProductGet( SV_INSPECTION );

		if( !( l_Product.empty() ) )
		{
			SVDataManagerHandle l_Handle;

			l_Product.GetResultDataIndex( l_Handle );

			l_LastIndex = l_Handle.GetIndex();
		}

		// Set first object depth...
		pChildObject->SetObjectDepthWithIndex( m_objectDepth, l_LastIndex );
		pChildObject->SetImageDepth( m_lImageDepth );

		SVInspectionLevelCreateStruct createStruct;
		createStruct.OwnerObjectInfo = this;
		createStruct.InspectionObjectInfo = this;

		bool Return = pChildObject->createAllObjects(createStruct);

		if( SVMFResetObject == ( context & SVMFResetObject ) )
		{
			pChildObject->resetAllObjects(true, false);
		}

		if( SVMFSetDefaultInputs == ( context & SVMFSetDefaultInputs ) )
		{
			SetDefaultInputs();
		}

		if( SVMFResetInspection == ( context & SVMFResetInspection ) )
		{
			resetAllObjects(true, false);
		}

		return Return;
	}
	else // Not a Valid Object
	{
		return false;
	}
}

void SVInspectionProcess::ConnectObject( const SVObjectLevelCreateStruct& rCreateStructure )
{
	__super::ConnectObject( rCreateStructure );

	if ( nullptr != GetToolSet())
	{
		GetToolSet()->ConnectObject(rCreateStructure);
	}
}

bool SVInspectionProcess::DestroyChildObject(SVObjectClass* pChild)
{
	if( pChild == m_pCurrentToolset )
	{
		m_pCurrentToolset = nullptr;
	}

	// if we have an Object
	if( nullptr != pChild )
	{
		SVTaskObjectClass* pTaskObject = dynamic_cast< SVTaskObjectClass* >( pChild );

		if( nullptr != pTaskObject )
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

	if ( nullptr != GetToolSet() && nullptr == retValue)
	{
		return GetToolSet()->getFirstObject(rObjectTypeInfo, useFriends, pRequestor);
	}
	return retValue;
}

void SVInspectionProcess::OnObjectRenamed(const SVObjectClass& rRenamedObject, const SVString& rOldName)
{
	if ( GetToolSet() )
	{
		GetToolSet()->OnObjectRenamed(rRenamedObject, rOldName);
	}
}

bool SVInspectionProcess::ConnectAllInputs()
{
	if ( GetToolSet() )
	{
		return GetToolSet()->ConnectAllInputs();
	}
	return false;
}

bool SVInspectionProcess::replaceObject(SVObjectClass* pObject, const GUID& rNewGuid)
{
	if( nullptr != pObject )
	{
		// Kill the Friends
		// SVObjectClass can have Friends
		pObject->DestroyFriends();

		// Special code for Duplicates!!
		SVObjectClass* pDuplicateObject = SVObjectManagerClass::Instance().GetObject(rNewGuid);
		if (pDuplicateObject)
		{
			if( pDuplicateObject == m_pCurrentToolset )
			{
				m_pCurrentToolset = nullptr;
			}

			// Get the Owner
			SVObjectInfoStruct ownerInfo = pDuplicateObject->GetOwnerInfo();

			SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.UniqueObjectID);
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

		if( SVObjectManagerClass::Instance().ChangeUniqueObjectID( pObject, rNewGuid ) )
		{
			SVToolSetClass* l_pToolSet( nullptr );

			l_pToolSet = dynamic_cast< SVToolSetClass* >( pObject );

			if( nullptr != l_pToolSet )
			{
				if( nullptr != m_pCurrentToolset )
				{
					DestroyChildObject( m_pCurrentToolset );
				}

				m_pCurrentToolset = l_pToolSet;

				m_pCurrentToolset->SetObjectOwner( this );

				CreateChildObject(m_pCurrentToolset);

				if( nullptr != m_pToolSetConditional )
				{
					m_pCurrentToolset->AddFriend( m_pToolSetConditional->GetUniqueObjectID() );

					m_pCurrentToolset->CreateChildObject(m_pToolSetConditional);

					m_pToolSetConditional = nullptr;
				}
			}
			else
			{
				SVConditionalClass* l_pConditional( nullptr );

				l_pConditional = dynamic_cast< SVConditionalClass* >( pObject );

				if( nullptr != l_pConditional )
				{
					m_pToolSetConditional = l_pConditional;

					m_pToolSetConditional->SetObjectOwner( this );
				}
			}

			return true;
		}
	}
	return false;
}
