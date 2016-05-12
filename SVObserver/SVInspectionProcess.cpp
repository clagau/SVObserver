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
#include "SVImageProcessingClass.h"
#include "SVConditional.h"
#include "SVSVIMStateClass.h"
#include "SVCommandStreamManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVRunControlLibrary\SVRunControlLibrary.h"
#ifdef COLLECT_SHAREDMEMORY_STATS
#include "SVTimerLibrary\SVProfiler.h"
#else
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
#include "SVTimerLibrary\SVProfiler.h"
#endif
#endif
#pragma endregion Includes


SV_IMPLEMENT_CLASS( SVInspectionProcess, SVInspectionProcessGuid );

HRESULT SVInspectionProcess::ProcessInspection( bool& p_rProcessed, SVProductInfoStruct& p_rProduct )
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_qInspectionsQueue.GetCount() );

	if( p_rProcessed )
	{
		p_rProcessed = ( m_qInspectionsQueue.GetHead( &p_rProduct ) != FALSE );
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
			SVStringArray msgList;
			msgList.push_back(GetName());
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (hr), SvOi::Tid_InspThreadGetResultImageIndexError, msgList, StdMessageParams );
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

				if ( ( pIPInfo->oInspectedState & PRODUCT_INSPECTION_NOT_RUN ) != PRODUCT_INSPECTION_NOT_RUN )
				{
					pIPInfo->oInspectedState = GetToolSet()->GetResultList()->GetInspectionState();

					pIPInfo->m_EndInspection = SVClock::GetTimeStamp();

					#ifdef _DEBUG
					SVString l_TempStateString = SvUl_SF::Format( _T( "SVInspectionProcess::ProcessInspection|%s|TRI=%ld\n" ),
							GetName(), p_rProduct.ProcessCount() );

						::OutputDebugString( l_TempStateString.c_str() );
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

			// need RunStatus?? or is last set sufficient for images and values?
			// Joe: YES as long as this is done in the inspection
			CollectConditionalHistoryData();

			pIPInfo->ClearIndexes();
		}

		m_bInspecting = false;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd( _T( "Process Inspections" ) );
#endif
	}
	return l_Status;
}

// Update the list of items being monitored
HRESULT SVInspectionProcess::ProcessMonitorLists(bool& p_rProcessed)
{
	HRESULT hr = S_OK;
	bool bNotFound = false;

	SVMonitorList l_MonitorList;
		
	if (m_MonitorListQueue.RemoveHead(&l_MonitorList))
	{
		m_SharedMemoryFilters.clear();

		const SVMonitorItemList& valueList = l_MonitorList.GetDataList();

		for (SVMonitorItemList::const_iterator l_ValueIter = valueList.begin();l_ValueIter != valueList.end();++l_ValueIter)
		{
			const SVString& name = (*l_ValueIter);
			SVValueObjectReference l_RefObject;
			if (S_OK == GetInspectionValueObject(name.c_str(), l_RefObject))
			{
				m_SharedMemoryFilters.m_LastInspectedValues[name] = l_RefObject.Guid();
			}
			else
			{
				bNotFound = true;
			}
		}
		if (bNotFound)
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvOi::Tid_ErrorNotAllDataItemsFound, StdMessageParams, SvOi::Err_15021 );
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
				SVImageExtentClass l_extents = l_pImage->GetImageExtents(); 
				m_SharedMemoryFilters.m_LastInspectedImages[ name ] = l_pImage->GetUniqueObjectID();
			}
			else
			{
				bNotFound = true;
			}
		}
		if (bNotFound)
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvOi::Tid_ErrorNotAllImageItemsFound, StdMessageParams, SvOi::Err_15022 );
		}
		::InterlockedIncrement( &m_NotifyWithLastInspected );

		p_rProcessed = true;
	}
	return hr;
}

// Update the List of LastInspected Images being written to shared memory
HRESULT SVInspectionProcess::ProcessLastInspectedImages( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;
	SVMonitorItemList l_NameList;
		
	if ( m_ImageNameDequeQueue.RemoveHead( &l_NameList ) )
	{
		m_SharedMemoryFilters.m_LastInspectedImages.clear();

		for( SVMonitorItemList::const_iterator l_Iter = l_NameList.begin(); l_Iter != l_NameList.end(); ++l_Iter )
		{
			const SVString& name = (*l_Iter);
			SVImageClass* l_pImage;
			if ( S_OK == GetInspectionImage( name.c_str(), l_pImage ) )
			{
				m_SharedMemoryFilters.m_LastInspectedImages[ name ] = l_pImage->GetUniqueObjectID();
			}
		}

		::InterlockedIncrement( &m_NotifyWithLastInspected );

		p_rProcessed = true;
	}
	return l_Status;
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
#ifdef COLLECT_SHAREDMEMORY_STATS
					SvTl::SVProfiler totalProfiler;
					SvTl::SVProfiler rejectProfiler;
					SvTl::SVProfiler commitProfiler;
					totalProfiler.Start();
#endif
					SeidenaderVision::SVSharedInspectionWriter& rWriter = SVSharedMemorySingleton::Instance().GetInspectionWriter(GetPPQIdentifier(), GetUniqueObjectID());
					SVSharedData& rLastInspected = rWriter.GetLastInspectedSlot(sharedSlotIndex);
					FillSharedData(sharedSlotIndex, rLastInspected, m_SharedMemoryFilters.m_LastInspectedValues, m_SharedMemoryFilters.m_LastInspectedImages, l_Product, rWriter);
				
#ifdef COLLECT_SHAREDMEMORY_STATS
					commitProfiler.Start();
#endif
				}
			}
			catch (const std::exception& e)
			{
				SVStringArray msgList;
				msgList.push_back(e.what());
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ErrorProcessNotifyLastInspected, msgList, StdMessageParams, SvOi::Err_15023 );
			}
			catch (...)
			{
				SVStringArray msgList;
				msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_Unknown));
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ErrorProcessNotifyLastInspected, msgList, StdMessageParams, SvOi::Err_15024 );
			}
		}
		SVInspectionCompleteInfoStruct l_Data(GetUniqueObjectID(), l_Product);
		SVObjectManagerClass::Instance().UpdateObservers("SVInspectionProcess", GetUniqueObjectID(), l_Data);

		p_rProcessed = true;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd(_T("Process Notify With Last Inspected"));
#endif
	}
	return l_Status;
}

HRESULT SVInspectionProcess::ProcessConditionalHistory( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	if( !( m_qTransactions.IsEmpty() ) )
	{
#ifdef EnableTracking
		m_InspectionTracking.EventStart( _T( "Process Notify With Last Inspected" ) );
#endif

		// check the transaction queue
		SVInspectionTransactionStruct message;
			
		if ( m_qTransactions.RemoveHead( &message ) )
		{
			ProcessTransaction( message );
		}

		p_rProcessed = true;

#ifdef EnableTracking
		m_InspectionTracking.EventEnd( _T( "Process Notify With Last Inspected" ) );
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

HRESULT SVInspectionProcess::ProcessTransaction( SVInspectionTransactionStruct p_Message )
{
	HRESULT hr = S_OK;

	#ifndef _DEBUG
	try
	#endif
	{
	switch ( p_Message.dwMessage )
	{
		case SVInspectionMessageSetCHProperties:
		{
			SVInspectionMessageDataStruct_CHProperties* pData = dynamic_cast <SVInspectionMessageDataStruct_CHProperties*> (p_Message.pData);
			ASSERT( pData );
			hr = m_ConditionalHistory.SetProperties( *(pData->pvecProperties), pData->bResetObject );
			break;
		}
		case SVInspectionMessageGetCHProperties:
		{
			SVInspectionMessageDataStruct_CHProperties* pData = dynamic_cast <SVInspectionMessageDataStruct_CHProperties*> (p_Message.pData);
			ASSERT( pData );
			hr = m_ConditionalHistory.GetProperties( *(pData->pvecProperties) );
			break;
		}
		case SVInspectionMessageSetCHList:
		{
			SVInspectionMessageDataStruct_CHList* pData = dynamic_cast <SVInspectionMessageDataStruct_CHList*> (p_Message.pData);
			ASSERT( pData );
			hr = m_ConditionalHistory.SetList( pData->pvecData, pData->pvecImages, pData->pvecConditionals, pData->bResetObject );
			break;
		}
		case SVInspectionMessageGetCHList:
		{
			SVInspectionMessageDataStruct_CHList* pData = dynamic_cast <SVInspectionMessageDataStruct_CHList*> (p_Message.pData);
			ASSERT( pData );
			hr = m_ConditionalHistory.GetList( *(pData->pvecData), *(pData->pvecImages), *(pData->pvecConditionals) );
			break;
		}
		case SVInspectionMessageGetCHandClear:
		{
			SVInspectionMessageDataStruct_CHGetAll* pData = dynamic_cast <SVInspectionMessageDataStruct_CHGetAll*> (p_Message.pData);
			ASSERT( pData );
			hr = m_ConditionalHistory.GetHistoryAndClear( *(pData->pvecData), *(pData->pvecImages), *(pData->pvecConditionals), *(pData->pvecProcessCount) );
			break;
		}
		case SVInspectionMessageGetCHMostRecent:
		{
			SVInspectionMessageDataStruct_CHGet* pData = dynamic_cast <SVInspectionMessageDataStruct_CHGet*> (p_Message.pData);
			ASSERT( pData );
			hr = m_ConditionalHistory.GetMostRecentHistory( *(pData->pvecData), *(pData->pvecImages), *(pData->pvecConditionals), *(pData->plProcessCount) );
			break;
		}
	}// end switch ( p_Message.dwMessage )
	}// end try
	#ifndef _DEBUG
	catch (...)
	{
		hr = SV_FALSE;	// @TODO:  Add real error code here!!!
	}
	#endif

	if ( S_OK == hr )
	{
		p_Message.pData->bProcessed = true;
	}
	::SetEvent( p_Message.hCompletionEvent );

	return hr;
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
	m_ConditionalHistory.SetInspection( this );
	m_dwThreadId = 0;
#ifdef _DEBUG
	m_dwCHTimeout = INFINITE;	// to allow debugging
#else
	m_dwCHTimeout = 40000;	// not INFINITE
#endif

	m_svReset.RemoveState( SVResetStateAll );

	m_bForceOffsetUpdate = true;

	m_publishList.m_pInspection = this;

	m_pToolSetConditional = nullptr;
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

bool SVInspectionProcess::IsCameraInInspection( const CString& p_rName ) const
{
	bool l_Status = false;

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( !l_Status && l_Iter != m_CameraImages.end() )
	{
		if( nullptr != ( *l_Iter ) )
		{
			l_Status = ( p_rName == ( *l_Iter )->GetName() );
		}

		++l_Iter;
	}

	return l_Status;
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
		return false;

	if( !m_CommandQueue.Create() )
		return false;

	if( !m_MonitorListQueue.Create() ) { return false; }

	if( !m_ImageNameDequeQueue.Create() ) { return false; }

	::InterlockedExchange( &m_NotifyWithLastInspected, 0 );

	if( S_OK != m_AsyncProcedure.Create( &SVInspectionProcess::APCThreadProcess, boost::bind(&SVInspectionProcess::ThreadProcess, this, _1), GetName(), SVThreadAttribute::SVAffinityUser ) )
	{
		return false;
	}

	if ( !m_qTransactions.Create() )
	{
		return false;
	}

	hr = CreateResultImageIndexManager();
	if( S_OK != hr )
		return false;

	// Create Queues for input/output requests
	if( !m_InputRequests.Create() )
		return false;

	if( !m_InputImageRequests.Create() )
		return false;

	m_pCurrentToolset	= new SVToolSetClass( TRUE, this );

	if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pCurrentToolset), 0 ) != SVMR_SUCCESS )
	{
		return false;
	}

	if ( S_OK != m_ConditionalHistory.ResetObject() )
		return false;

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

	ProcessMonitorLists( l_Processed );

	ProcessLastInspectedImages( l_Processed );

	ProcessNotifyWithLastInspected( l_Processed, l_Product.m_svInspectionInfos[GetUniqueObjectID()].m_lastInspectedSlot );

	ProcessConditionalHistory( l_Processed );

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

	SVObjectManagerClass::Instance().UpdateObservers( "SVInspectionProcess", GetUniqueObjectID(), SVRemoveSubjectStruct() );

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
		::SVSendMessage(this, SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>( m_pCurrentToolset ), 0);
	}

	// Destroy Queues for input/output requests
	m_InputRequests.Destroy();
	m_InputImageRequests.Destroy();
	m_CommandQueue.Destroy();
	m_MonitorListQueue.Destroy();
	m_ImageNameDequeQueue.Destroy();

	SVObjectManagerClass::Instance().AdjustInspectionIndicator( -( m_qInspectionsQueue.GetCount() ) );

	// Destroy Queues for Inspection Queue
	m_qInspectionsQueue.Destroy();

	SVTransactionQueueObjectLock lock( m_qTransactions );
	
	while ( !m_qTransactions.IsEmpty() )
	{
		SVInspectionTransactionStruct message;
		if ( m_qTransactions.RemoveHead( &message ) )
		{
			ProcessTransaction( message );
		}
	}

	m_qTransactions.Destroy();

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
		m_mapValueObjects.insert( SVValueObjectMap::value_type( (*l_oValueIter)->GetCompleteObjectName(), (*l_oValueIter) ) );
	}

	return l_hrOk;
}// end BuildValueObjectMap

HRESULT SVInspectionProcess::GetInspectionValueObject( const CString& p_strName, SVValueObjectReference& p_rRefObject )
{
	HRESULT hr = S_FALSE;

	SVObjectNameInfo l_NameInfo;

	SVValueObjectClass* l_pObject = nullptr;

	hr = l_NameInfo.ParseObjectName( static_cast< LPCTSTR >( p_strName ) );

	if( S_OK == hr )
	{
		if( 0 < l_NameInfo.m_NameArray.size() && l_NameInfo.m_NameArray[ 0 ] != GetName() )
		{
			l_NameInfo.m_NameArray.push_front( GetName() );
		}

		SVString l_ObjectName = l_NameInfo.GetObjectName();

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

HRESULT SVInspectionProcess::GetInspectionObject( const CString& p_strName, SVObjectReference& p_rRefObject )
{
	SVValueObjectReference ref;
	HRESULT hr = GetInspectionValueObject( p_strName, ref );	// try the most efficient one first
	if ( S_OK == hr )
	{
		p_rRefObject = ref;
	}
	else	// if not a value object, try getting images
	{
		SVImageClass* pImage = nullptr;
		hr = GetInspectionImage( p_strName, pImage );
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

	l_bOk = ::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, 0, 0 ) == SVMR_SUCCESS;

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

	if( BuildValueObjectMap() != S_OK )
		return FALSE;

	return TRUE;
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

	SVSendMessage( this, SVM_GOING_OFFLINE, 0, 0 ); // EB 2005 09 20

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
					SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_Iter;

					l_Iter = pProduct->m_svCameraInfos.find( pCamera );

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
					SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_Iter;

					l_Iter = pProduct->m_svCameraInfos.find( pCamera );

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

			::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast< DWORD_PTR>(pValueObject), 0 );

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
		lSize = static_cast<long>(m_arViewedInputNames.GetSize());
		for( l = 0; l < lSize; l++ )
		{
			if( m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject && 
				m_PPQInputs[iList].m_IOEntryPtr->m_pValueObject->GetCompleteObjectName() == m_arViewedInputNames[l] )
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

BOOL SVInspectionProcess::AddInputRequest( SVValueObjectReference p_svObjectRef, const _variant_t& p_rValue )
{
	BOOL l_bOk = FALSE;

	try
	{
		SVInputRequestInfoStructPtr l_pInRequest = new SVInputRequestInfoStruct( p_svObjectRef, p_rValue );
		
		//add request to inspection process
		l_bOk = AddInputRequest( l_pInRequest );
	}
	catch( ... ) // @WARNING:  bad practice catching ...
	{
	}

	return l_bOk;
}

BOOL SVInspectionProcess::AddInputRequestMarker()
{
	return AddInputRequest( nullptr, SvO::SVTOOLPARAMETERLIST_MARKER );
}

BOOL SVInspectionProcess::AddInputRequest( SVInputRequestInfoStructPtr p_pInRequest )
{
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputRequests, StdMessageParams, SvOi::Err_17013_ErrorLockingInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.AddTail( p_pInRequest ) )
	{
		if( !m_InputRequests.Unlock() )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17014_ErrorUnlockingInputRequests );
			DebugBreak();
		}

		return FALSE;
	}// end if


	SVString l_StringValue;

	if( p_pInRequest->m_Value.vt == VT_BSTR )
	{
		l_StringValue = SvUl_SF::createSVString(p_pInRequest->m_Value);
	}

	if( l_StringValue == SvO::SVTOOLPARAMETERLIST_MARKER )
	{
		::InterlockedIncrement(const_cast<long*>(&m_lInputRequestMarkerCount));
	}// end if

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17015_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	return TRUE;
}// end AddInputRequest

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

				l_Status = SVImageProcessingClass::Instance().LoadImageBuffer( (void*)p_rbstrValue, 
					l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr, l_ImageInfo );

				l_pInRequest->m_bUsingCameraName = nullptr != l_psvMainImage;

				if( l_pInRequest->m_bUsingCameraName )
				{
					l_pInRequest->m_strObjectName = l_psvMainImage->GetCamera()->GetName();
				}
				else
				{
					l_pInRequest->m_strObjectName = p_psvImage->GetCompleteObjectName();
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

			SVCameraImageTemplate* l_psvMainImage = dynamic_cast< SVCameraImageTemplate* >( p_psvImage );

			l_Status = SVImageProcessingClass::Instance().LoadImageBuffer( p_rImageFileName.c_str(), 
				l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr, false );

			l_pInRequest->m_bUsingCameraName = nullptr != l_psvMainImage;

			if( l_pInRequest->m_bUsingCameraName )
			{
				l_pInRequest->m_strObjectName = l_psvMainImage->GetCamera()->GetName();
			}
			else
			{
				l_pInRequest->m_strObjectName = p_psvImage->GetCompleteObjectName();
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
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputImageRequests, StdMessageParams, SvOi::Err_17016_ErrorLockingInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.AddTail( p_pInRequest ) )
	{
		if( !m_InputImageRequests.Unlock() )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputImageRequests, StdMessageParams, SvOi::Err_17017_ErrorUnlockingInputImageRequests );
			DebugBreak();
		}

		return S_FALSE;
	}// end if

	if( !m_InputImageRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputImageRequests, StdMessageParams, SvOi::Err_17018_ErrorUnlockingInputImageRequests );
		DebugBreak();
	}

	return S_OK;
}// end AddInputImageRequest

BOOL SVInspectionProcess::RemoveAllInputRequests()
{
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputRequests, StdMessageParams, SvOi::Err_17019_ErrorLockingInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.RemoveAll() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorRemovingAllInputRequests, StdMessageParams, SvOi::Err_17020_ErrorRemovingAllInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17021_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.Lock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputImageRequests, StdMessageParams, SvOi::Err_17022_ErrorLockingInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.RemoveAll() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorRemovingAllInputImageRequests, StdMessageParams, SvOi::Err_17023_ErrorRemovingAllInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputImageRequests, StdMessageParams, SvOi::Err_17024_ErrorUnlockingInputImageRequests );
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
		// get camera
		CString sKey;
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

			if( pCamDevice && pCamDevice->BandSize() == 3L )
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

				SvStl::MessageMgrDisplayAndNotify Msg( SvStl::LogAndDisplay );
				Msg.setMessage( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_IPDoc_NoCameraColorAttached, StdMessageParams, SvOi::Err_10052 ); 
			}
		}
		else
		{
			if( SVMR_SUCCESS != ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(&m_rgbMainImageObject), 0 ) )
			{
				l_Status = E_FAIL;
			}

			m_rgbMainImageObject.UpdateImage( SVImageTypePhysical, SV_GUID_NULL );
			m_rgbMainImageObject.SetObjectOwner( this );
		}
	}
	////////////////////////

	SVObjectLevelCreateStruct createStruct;

	if( SVMR_SUCCESS != ::SVSendMessage( this, SVM_CREATE_ALL_OBJECTS,reinterpret_cast<DWORD_PTR>(&createStruct), 0 ) )
	{
		l_Status = E_FAIL;
	}

	SetDefaultInputs();

	BuildConditionalHistoryListAfterLoad();

	CString strValueObjects;
	int iCount = static_cast<int>(m_mapValueObjects.size());
	strValueObjects.Format(_T("%s value object count=%d\n"), GetName(), iCount);
	::OutputDebugString( strValueObjects );

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

			SVObjectManagerClass::Instance().UpdateObservers( "SVInspectionProcess", GetUniqueObjectID(), l_Data );
		}
	}
}

DWORD_PTR SVInspectionProcess::processMessage(DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext)
{
	DWORD_PTR DwResult = SVObjectClass::processMessage(DwMessageID, DwMessageValue, DwMessageContext);

	SVInspectionLevelCreateStruct createStruct;

	if (DwResult == SVMR_NOT_PROCESSED)
	{
		if( ( DwMessageID & SVM_PURE_MESSAGE ) != SVMSGID_CREATE_CHILD_OBJECT && ( DwMessageID & SVM_PURE_MESSAGE ) != SVMSGID_CONNECT_CHILD_OBJECT )
		{
			for (size_t l = 0; l < m_PPQInputs.size(); l++)
			{
				if( !( m_PPQInputs[ l ].empty() ) )
				{
					if( (DwMessageID & SVM_PURE_MESSAGE) == SVMSGID_GET_OBJECT_BY_NAME )
					{
						CString strName = (LPCSTR)DwMessageValue;
						CString strInput;

						strInput = GetName();
						strInput += ".";
						strInput += m_PPQInputs[l].m_IOEntryPtr->m_pValueObject->GetName();
						if( strName == strInput )
						{
							DwResult = reinterpret_cast<DWORD_PTR>(m_PPQInputs[l].m_IOEntryPtr->m_pValueObject);
							break;
						}// end if

					}// end SVMSGID_GET_OBJECT_BY_NAME
					else
					{
						DwResult = SVSendMessage(m_PPQInputs[l].m_IOEntryPtr->m_pValueObject, DwMessageID, DwMessageValue, DwMessageContext);

						if (DwResult != SVMR_NOT_PROCESSED)
							break;
					}// end else
				}
			}// end for
		}
	}// end if

	if( DwResult == SVMR_NOT_PROCESSED || 
		( DwMessageID & SVM_NOTIFY_FIRST_RESPONDING ) != SVM_NOTIFY_FIRST_RESPONDING )
	{
		// Try to process message by yourself...
		DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
		switch( dwPureMessageID )
		{
		case SVMSGID_CREATE_CHILD_OBJECT:
			{
				// Send this message to the object owner to create an object.
				// If the owner object is not created yet, it returns SVMR_NOT_PROCESSED.
				// Otherwise the owner object sends SVM_CREATE_ALL_OBJECTS to the child object
				// and returns the result of this message.
				// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
				// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
				SVObjectClass* pChildObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);
				//@warning [MZA, 21.07.15] The DWORD_PTR DwMessageValue has to be converted via reinterpret_cast to SVObjectClass*. This is dangerous.
				//After the reinterpret_cast to SVOjectClass*, to use a kind_of or dynamic_cast does not help for more safety, because it will not check anything.
				//We should cleanup this and replace processMessage with other methods (see SVO-806)
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

					createStruct.OwnerObjectInfo = this;
					createStruct.InspectionObjectInfo = this;

					DWORD_PTR l_Return = SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );

					if( ( DwMessageContext & SVMFResetObject ) == SVMFResetObject )
					{
						::SVSendMessage( pChildObject, SVM_RESET_ALL_OBJECTS, 0, 0 );
					}

					if( ( DwMessageContext & SVMFSetDefaultInputs ) == SVMFSetDefaultInputs )
					{
						SetDefaultInputs();
					}

					if( ( DwMessageContext & SVMFResetInspection ) == SVMFResetInspection )
					{
						::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, 0, 0 );
					}

					return l_Return;
				}
				else // Not a Valid Object
				{
					return SVMR_NO_SUCCESS;
				}
			}
			break;

		case SVMSGID_CONNECT_CHILD_OBJECT:
			{
				// Send this message to the object owner to create an object.
				// If the owner object is not created yet, it returns SVMR_NOT_PROCESSED.
				// Otherwise the owner object sends SVM_CREATE_ALL_OBJECTS to the child object
				// and returns the result of this message.
				// ...use second message parameter ( DwMessageValue ) as SVObjectClass* of the child object
				// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS or SVMR_NOT_PROCESSED
				SVObjectClass* pChildObject = reinterpret_cast<SVObjectClass*>(DwMessageValue);
				//@warning [MZA, 21.07.15] The DWORD_PTR DwMessageValue has to be converted via reinterpret_cast to SVObjectClass*. This is dangerous.
				//After the reinterpret_cast to SVOjectClass*, to use a kind_of or dynamic_cast does not help for more safety, because it will not check anything.
				//We should cleanup this and replace processMessage with other methods (see SVO-806)
				createStruct.OwnerObjectInfo = this;
				createStruct.InspectionObjectInfo = this;

				return SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );
			}
			break;

		case SVMSGID_REPLACE_OBJECT:
			{
				// ...use second message parameter ( DwMessageValue ) as objectID ( GUID* )
				// ...use third message parameter ( DwMessageContext ) as SVObjectClass*
				// ...returns SVMR_SUCCESS, SVMR_NO_SUCCESS
				const GUID ObjectID = *( ( GUID* )DwMessageValue);
				SVObjectClass* pMessageObject = reinterpret_cast<SVObjectClass*>(DwMessageContext);
				if( nullptr != pMessageObject )
				{
					// Kill the Friends
					// SVObjectClass can have Friends
					pMessageObject->DestroyFriends();

					// Special code for Duplicates!!
					SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(ObjectID);
					if (pObject)
					{
						if( pObject == m_pCurrentToolset )
						{
							m_pCurrentToolset = nullptr;
						}

						// Get the Owner
						SVObjectInfoStruct ownerInfo = pObject->GetOwnerInfo();

						SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.UniqueObjectID);
						if (pOwner)
						{
							// Ask the owner to kill the imposter!
							if (::SVSendMessage(pOwner, SVM_DESTROY_CHILD_OBJECT,reinterpret_cast<DWORD_PTR>(pObject), 0) == SVMR_NO_SUCCESS)
							{
								// must be a Friend
								pOwner->DestroyFriends();
							}
						}
					}

					// Special code for Objects that allocate Friends on SetOwner()
					pMessageObject->DestroyFriends();

					if( SVObjectManagerClass::Instance().ChangeUniqueObjectID( pMessageObject, ObjectID ) )
					{
						SVToolSetClass* l_pToolSet( nullptr );

						l_pToolSet = dynamic_cast< SVToolSetClass* >( pMessageObject );

						if( nullptr != l_pToolSet )
						{
							if( nullptr != m_pCurrentToolset )
							{
								::SVSendMessage( this, SVM_DESTROY_CHILD_OBJECT,reinterpret_cast<DWORD_PTR>( m_pCurrentToolset), 0 );
							}

							m_pCurrentToolset = l_pToolSet;

							m_pCurrentToolset->SetObjectOwner( this );

							::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pCurrentToolset), 0 );

							if( nullptr != m_pToolSetConditional )
							{
								m_pCurrentToolset->AddFriend( m_pToolSetConditional->GetUniqueObjectID() );

								::SVSendMessage( m_pCurrentToolset, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pToolSetConditional), 0 );

								m_pToolSetConditional = nullptr;
							}
						}
						else
						{
							SVConditionalClass* l_pConditional( nullptr );

							l_pConditional = dynamic_cast< SVConditionalClass* >( pMessageObject );

							if( nullptr != l_pConditional )
							{
								m_pToolSetConditional = l_pConditional;

								m_pToolSetConditional->SetObjectOwner( this );
							}
						}

						return SVMR_SUCCESS;
					}
				}
				return SVMR_NO_SUCCESS;
			}

		case SVMSGID_DESTROY_CHILD_OBJECT:
			{
				SVObjectClass* pObject = dynamic_cast< SVObjectClass* >( ( SVObjectClass* ) DwMessageValue );

				if( pObject == m_pCurrentToolset )
				{
					m_pCurrentToolset = nullptr;
				}

				// if we have an Object
				if( nullptr != pObject )
				{
					SVTaskObjectClass* l_pTaskObject( nullptr );

					l_pTaskObject = dynamic_cast< SVTaskObjectClass* >( pObject );

					if( nullptr != l_pTaskObject )
					{
						// Notify the Owner of our inputs that they are not needed anymore
						l_pTaskObject->Disconnect();
					}

					// Close the Object
					pObject->CloseObject();

					// Destroy our Friends
					pObject->DestroyFriends();

					// Destruct it
					delete pObject;

					if( ( DwMessageContext & SVMFSetDefaultInputs ) == SVMFSetDefaultInputs )
					{
						SetDefaultInputs();
					}

					if( ( DwMessageContext & SVMFResetInspection ) == SVMFResetInspection )
					{
						::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, 0, 0 );
					}

					return SVMR_SUCCESS;
				}
				return SVMR_NO_SUCCESS;
			}

		case SVMSGID_DISCONNECT_OBJECT_INPUT:
			{
				SVInObjectInfoStruct* pInObjectInfo = ( SVInObjectInfoStruct* ) DwMessageValue;
				if( m_publishList.RemovePublishedEntry( pInObjectInfo->GetInputObjectInfo().UniqueObjectID ) )
					DwResult =  SVMR_SUCCESS;
				break;
			}

		case SVMSGID_RESET_ALL_OBJECTS:
			{
				HRESULT l_ResetStatus = ResetObject();
				if( l_ResetStatus != S_OK )
				{
					ASSERT( SUCCEEDED( l_ResetStatus ) );

					DwResult = SVMR_NO_SUCCESS;
				}
				else
				{
					DwResult = SVMR_SUCCESS;
				}
				///SVO725 avoid sending message SVM_RESET_ALL_OBJECTS twice.
				return DwResult;
				break;
			}
		}

		if ( GetToolSet() )
			return SVSendMessage( GetToolSet(), DwMessageID, DwMessageValue, DwMessageContext ) | DwResult;
	}

	return DwResult;
}

BOOL SVInspectionProcess::GetChildObjectByName( LPCTSTR tszChildName, SVObjectClass** ppObject )
{
	ASSERT( nullptr != ppObject );
	bool bReturn = false;

	if ( nullptr != ppObject )
	{
		*ppObject = nullptr;

		CString sChildName = tszChildName;
		CString sName = GetCompleteObjectName();
		if ( sChildName.Left(sName.GetLength()) == sName )
		{
			*ppObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( this, 
					( SVM_GET_OBJECT_BY_NAME | SVM_PARENT_TO_CHILD ) & ~SVM_NOTIFY_ONLY_THIS, 
					reinterpret_cast<DWORD_PTR>(tszChildName), 0 ));
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
		if( ::SVSendMessage( GetToolSet(), SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(p_rData.m_pTool), SVMFResetObject ) == SVMR_SUCCESS ) 
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
		// Call conditional history list and remove tool
		// Conditional history needs references deleted before we delete the tool.
		CString l_strToolName = p_rData.m_pTool->GetCompleteObjectName();
		m_ConditionalHistory.DeleteTool( l_strToolName );
		
		// Delete the Tool Object
		::SVSendMessage( GetToolSet(), SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(p_rData.m_pTool), 0 );

		// Refresh Lists ( inputs,outputs,results,published )
		SetDefaultInputs();

		// Create Child closed object
		::SVSendMessage( this, SVM_CREATE_ALL_OBJECTS, 0, 0 );

		// Reset all objects
		::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, 0, 0 );

		BuildValueObjectMap();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::ObserverUpdate( const SVRenameObject& p_rData )
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( p_rData.m_ObjectId );

	if( nullptr != l_pObject )
	{
		::SVSendMessage( this, SVM_OBJECT_RENAMED,
						 reinterpret_cast <DWORD_PTR> ( static_cast <SVObjectClass*> (l_pObject) ),
						reinterpret_cast<DWORD_PTR>( static_cast<LPCTSTR>(p_rData.m_OldName.c_str())) );

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
}// end RunOnce

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
	BOOL l_bRet = TRUE;
	long l;

	SVInputRequestInfoStructPtr l_pInputRequest;

	// Process all input requests
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorLockingInputRequests, StdMessageParams, SvOi::Err_17025_ErrorLockingInputRequests );
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
				l_bRet = FALSE;

				break;
			}// end if

			SVString l_StringValue;

			if( ( l_pInputRequest->m_Value.vt & VT_ARRAY ) == VT_ARRAY )
			{
				SVSAFEARRAY::SVBounds l_Bounds;
				SVSAFEARRAY l_SafeArray = l_pInputRequest->m_Value;

				l_SafeArray.GetBounds( l_Bounds );

				if( 1 == l_SafeArray.size() )
				{
					_variant_t l_Variant;

					l_SafeArray.GetElement( l_Bounds[ 0 ].lLbound, l_Variant );

					l_StringValue = SvUl_SF::createSVString( l_Variant );
				}
				else if( 1 < l_SafeArray.size() )
				{
					for( size_t i = 0; i < l_Bounds[ 0 ].cElements; ++i )
					{
						_variant_t l_Variant;

						l_SafeArray.GetElement( ( l_Bounds[ 0 ].lLbound + i ), l_Variant );

						if ( i > 0 )
						{
							l_StringValue += _T(",");
						}
						l_StringValue += _T("`");
						l_StringValue += SvUl_SF::createSVString( l_Variant );
						l_StringValue += _T("`");
					}
				}
			}
			else
			{
				l_StringValue = SvUl_SF::createSVString(l_pInputRequest->m_Value);
			}

			// New delimiter added after each SVSetToolParameterList call
			// This breaks the list into pieces and we are only processing
			// 1 piece of the list per inspection iteration
			if( l_StringValue == SvO::SVTOOLPARAMETERLIST_MARKER )
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
						hrSet = SetObjectArrayValues<CFile>(refValueObject, l_iIndex, l_StringValue.c_str(), l_bResetObject);
					}
					else
					{
						CFileStatus rStatus;
	
						BOOL bOk = CFile::GetStatus( l_StringValue.c_str(), rStatus );
						if ( bOk )
						{
							bOk = 0L <= rStatus.m_size;
						}// end if
	
						if( bOk )
						{
							if( ! l_bResetObject )
							{
								CString l_csPrevValue;
	
								hrSet = refValueObject.GetValue( l_csPrevValue );
	
								l_bResetObject = l_csPrevValue.Compare( l_StringValue.c_str() ) != 0;
							}
	
							hrSet = refValueObject.SetValue( l_iIndex, l_StringValue.c_str() );
						}// end if
					}
				}// end if SVFileNameValueObjectClass
				else if( SV_IS_KIND_OF( refValueObject.Object(), SVStringValueObjectClass ) )
				{
					if (::IsArray(refValueObject))
					{
						hrSet = SetObjectArrayValues<CString>(refValueObject, l_iIndex, l_StringValue.c_str(), l_bResetObject);
					}
					else
					{
						if( ! l_bResetObject )
						{
							CString l_csPrevValue;
	
							hrSet = refValueObject.GetValue( l_csPrevValue );
	
							l_bResetObject = l_csPrevValue.Compare( l_StringValue.c_str() ) != 0;
						}
	
						hrSet = refValueObject.SetValue( l_iIndex, l_StringValue.c_str() );
					}
				}// end if SVStringValueObjectClass
				else if( SV_IS_KIND_OF( refValueObject.Object(), SVBoolValueObjectClass ) )
				{
					double l_dValue = FALSE;

					// Convert to the appropriate type of value
					if( SvUl_SF::CompareNoCase( l_StringValue, _T( "TRUE" ) ) == 0 )
					{
						l_dValue = TRUE;
					}// end if
					else
					{
						l_dValue = atof( l_StringValue.c_str() );
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
						hrSet = pvValueObject->SetValueKeepType( l_iIndex, refValueObject.ArrayIndex(), l_StringValue.c_str() );
					}
					else
					{
						hrSet = pvValueObject->SetValueKeepType( l_iIndex, l_StringValue.c_str() );
					}
				}// end else if SVVariantValueObjectClass
				else if ( nullptr != dynamic_cast <SVEnumerateValueObjectClass*> ( refValueObject.Object() ) )
				{
					// Convert to the appropriate type of value
					bool l_bTempReset = false;

					if( ! l_bResetObject )
					{
						HRESULT hrCompare = refValueObject->CompareWithCurrentValue( l_StringValue.c_str() );
						l_bTempReset = ( hrCompare != S_OK );	// reset if not the same
					}

					if( S_OK == (hrSet = refValueObject.SetValue( 1, l_StringValue.c_str() )) )
					{
						hrSet = refValueObject.SetValue( l_iIndex, l_StringValue.c_str() );
					}
					else
					{
						if ( l_StringValue.size() > 0 )
						{
							if(  _ismbcdigit( l_StringValue.c_str()[0] ))
							{
								double l_dValue = atof( l_StringValue.c_str() );

								if( ! l_bResetObject )
								{
									HRESULT hrCompare = refValueObject->CompareWithCurrentValue( l_StringValue.c_str() );
									l_bTempReset = ( hrCompare != S_OK );	// reset if not the same
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
						hrSet = SetObjectArrayValues<double>(refValueObject, l_iIndex, l_StringValue.c_str(), l_bResetObject);
					}
					else
					{
						if( ! l_bResetObject )
						{
							double l_dPrevValue;
	
							hrSet = refValueObject.GetValue( l_dPrevValue );
	
							// eventually use CompareWithCurrentValueImpl
							l_bResetObject = l_dPrevValue != atof( l_StringValue.c_str() );
						}
	
						hrSet = refValueObject.SetValue( l_iIndex, l_StringValue.c_str() );
					}
				}// end else if
				else // Long, DWord, Byte
				{
					if( ! l_bResetObject )
					{
						double l_dPrevValue;

						hrSet = refValueObject.GetValue( l_dPrevValue );

						CString strNewValue = l_StringValue.c_str();
						strNewValue.MakeLower();
						TCHAR* p = nullptr;
						long lValue = 0;
						if ( strNewValue.Find(_T('x')) != -1  )
							lValue = _tcstol(strNewValue, &p, 16);
						else
							lValue = _tcstol(strNewValue, &p, 10);
						double l_dNewValue = static_cast <double> (lValue);

					// eventually use CompareWithCurrentValueImpl
					l_bResetObject = l_dPrevValue != l_dNewValue;
					}

					hrSet = refValueObject.SetValue( l_iIndex, l_StringValue.c_str() );
				}// end else if

				if( p_reResetItem != SVResetItemIP && l_bResetObject && refValueObject.Object()->ResetItem() < SVResetItemNone )
				{
					SVToolClass *l_psvTool = refValueObject.Object()->GetTool();

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

					if( p_reResetItem == SVResetItemIP )
					{
						p_rsvToolMap.clear();
					}
				}
			}// end if object exists
			
		}// end for all input requests
		
		// Processing phase
		if ( nullptr != m_pCurrentToolset )
		{
			if( p_reResetItem == SVResetItemIP )
			{
				l_bRet &= SVMR_SUCCESS == ::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, 0, 0 );
				l_bRet &= S_OK == m_pCurrentToolset->ClearResetCounts();
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
								l_bRet &= SVMR_SUCCESS == ::SVSendMessage( l_psvTool, SVM_RESET_ALL_OBJECTS, 0, 0 );
							}
							else
							{
								SVStdSetSVObjectClassPtr::iterator l_oIter;
								
								l_oIter = p_rsvToolMap[ l_psvTool ].m_svObjectSet.begin();

								while( l_oIter != p_rsvToolMap[ l_psvTool ].m_svObjectSet.end() )
								{
									SVObjectClass *l_psvObject = *l_oIter;

									l_bRet &= SVMR_SUCCESS == ::SVSendMessage( l_psvObject, SVM_RESET_ALL_OBJECTS, 0, 0 );

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

				SVObjectManagerClass::Instance().UpdateObservers( "SVInspectionProcess", GetUniqueObjectID(), l_Data );
			}
		}// end if ( nullptr != m_pCurrentToolset )
	}// end while( m_lInputRequestMarkerCount > 0L )

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17026_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	return l_bRet;
	
}// end ProcessInputRequests

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

					GetMainImages( l_pInRequest->m_strObjectName, l_MainImages );

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

					if( GetChildObjectByName(l_pInRequest->m_strObjectName, &l_psvObject) )
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
						SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svIter;

						l_svIter = p_psvProduct->m_svCameraInfos.find( l_psvMainImage->GetCamera() );

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
}// end ProcessInputImageRequests

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

BOOL SVInspectionProcess::RemoveCamera( CString sCameraName )
{
	BOOL bRet = TRUE;

	if( sCameraName == m_ToolSetCameraName )
	{
		SVVirtualCamera* pCamera = GetFirstPPQCamera();

		if( nullptr != pCamera )
		{
			m_ToolSetCameraName = pCamera->GetName();
		}
		else
		{
			m_ToolSetCameraName = _T( "" );
		}
	}

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( l_Iter != m_CameraImages.end() )
	{
		if( nullptr != ( *l_Iter ) )
		{
			SVVirtualCamera* pCamera = ( *l_Iter )->GetCamera();

			if( ( nullptr != pCamera ) && ( sCameraName == pCamera->GetName() ) )
			{
				( *l_Iter )->UpdateCameraImage( m_ToolSetCameraName );
			}
		}

		++l_Iter;
	}

	return bRet;
}

BOOL SVInspectionProcess::GetNewDisableMethod()
{
	return m_bNewDisableMethod;
}// end GetNewDisableMethod

void SVInspectionProcess::SetNewDisableMethod( BOOL bNewDisableMethod )
{
	m_bNewDisableMethod = bNewDisableMethod;
}// end SetNewDisableMethod

long SVInspectionProcess::GetEnableAuxiliaryExtent() const
{
	return m_lEnableAuxiliaryExtents;
}// end GetEnableAuxiliaryExtent

void SVInspectionProcess::SetEnableAuxiliaryExtent( long p_lEnableAuxiliaryExtents )
{
	m_lEnableAuxiliaryExtents = p_lEnableAuxiliaryExtents;
}// end SetEnableAuxiliaryExtent

HRESULT SVInspectionProcess::CollectOverlayData(SVImageClass *p_pImage, SVImageOverlayClass *p_pOverlayData)
{
	HRESULT hrRet = S_OK;

	SVToolSetClass *l_pToolSet = GetToolSet();

	SVExtentMultiLineStructCArray l_MultiLineArray;

	l_pToolSet->CollectOverlays(p_pImage,l_MultiLineArray);

	*p_pOverlayData = l_MultiLineArray;

	return hrRet;
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

HRESULT SVInspectionProcess::AddInputImageRequestByCameraName(CString sCameraName, CString sFileName)
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
		l_pInRequest->m_strObjectName = sCameraName;
		SVImageProcessingClass::Instance().LoadImageBuffer(sFileName,l_pInRequest->m_ImageInfo,l_pInRequest->m_ImageHandlePtr,FALSE);

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
	HRESULT l_hrOk = S_OK;

	if( nullptr != p_psvProduct )
	{
		SVProductInfoStruct l_Product = LastProductGet( SV_OTHER );

		SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svIter;
		SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svLastIter;

		for( l_svIter = p_psvProduct->m_svCameraInfos.begin(); 
			l_svIter != p_psvProduct->m_svCameraInfos.end(); ++l_svIter )
		{
			l_svLastIter = l_Product.m_svCameraInfos.find( l_svIter->first );

			if( l_svLastIter != l_Product.m_svCameraInfos.end() )
			{
				if( l_svIter->first->CopyValue( l_svLastIter->second.GetSourceImageDMIndexHandle(), 
					                            l_svIter->second.GetSourceImageDMIndexHandle() ) )
				{
					l_hrOk = S_FALSE;
				}
			}
		}
	}

	return l_hrOk;
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
	HRESULT l_hrOk = S_OK;

	if( nullptr != p_psvProduct )
	{
		SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svIter;

		for( l_svIter = p_psvProduct->m_svCameraInfos.begin(); 
			l_svIter != p_psvProduct->m_svCameraInfos.end(); ++l_svIter )
		{
			if( ! l_svIter->first->ReserveImageHandleIndex( l_svIter->second.GetSourceImageDMIndexHandle() ) )
			{
				l_hrOk = S_FALSE;
			}
		}
	}

	return l_hrOk;
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

HRESULT SVInspectionProcess::SetConditionalHistoryProperties( SVScalarValueVector& p_rvecProperties, bool p_bResetObject )
{
	HANDLE hEvent = nullptr;
	SVInspectionMessageDataStruct_CHProperties data( &p_rvecProperties, p_bResetObject );

	hEvent = ::CreateEvent( nullptr, true, false, nullptr );
	SVInspectionTransactionStruct message( SVInspectionMessageSetCHProperties, hEvent, &data );

	/*	// possible optimization for use in inspection thread
	if ( ::GetCurrentThreadId() == m_dwThreadId )
		return ProcessTransaction( message );	// in this case don't CreateEvent
	else
	*/
	{

		AddTransaction( message );

		DWORD dwReturn = ::WaitForSingleObjectEx( hEvent, m_dwCHTimeout, TRUE );

		::CloseHandle( hEvent );
		if ( dwReturn == WAIT_OBJECT_0 )
			return S_OK;
		else
			return (HRESULT) dwReturn;
	}
}

HRESULT SVInspectionProcess::GetConditionalHistoryProperties( SVScalarValueVector& p_rvecProperties )
{
	HANDLE hEvent = nullptr;
	hEvent = ::CreateEvent( nullptr, true, false, nullptr );

	SVInspectionMessageDataStruct_CHProperties data( &p_rvecProperties, false );
	SVInspectionTransactionStruct message( SVInspectionMessageGetCHProperties, hEvent, &data );
	AddTransaction( message );

	DWORD dwReturn = ::WaitForSingleObjectEx( hEvent, m_dwCHTimeout, TRUE );

	::CloseHandle( hEvent );
	if ( dwReturn == WAIT_OBJECT_0 )
		return S_OK;
	else
		return (HRESULT) dwReturn;
}

HRESULT SVInspectionProcess::GetConditionalHistoryProperties( SVScalarValueMapType& rmapProperties )
{
	rmapProperties.clear();
	SVScalarValueVector vecProperties;
	HRESULT hr = GetConditionalHistoryProperties( vecProperties );
	if ( S_OK == hr )
	{
		for ( size_t i=0; i < vecProperties.size(); ++i )
			rmapProperties[ vecProperties[i].strName ]  = vecProperties[i];
	}
	return hr;
}

HRESULT SVInspectionProcess::SetConditionalHistoryList( std::vector <SVScalarValue>* p_pvecData, std::vector <SVScalarValue>* p_pvecImages, std::vector <SVScalarValue>* p_pvecConditionals, bool p_bResetObject )
{
	HANDLE hEvent = nullptr;
	hEvent = ::CreateEvent( nullptr, true, false, nullptr );

	SVInspectionMessageDataStruct_CHList data( p_pvecData, p_pvecImages, p_pvecConditionals, p_bResetObject );
	SVInspectionTransactionStruct message( SVInspectionMessageSetCHList, hEvent, &data );
	AddTransaction( message );

	DWORD dwReturn = ::WaitForSingleObjectEx( hEvent, m_dwCHTimeout, TRUE );

	::CloseHandle( hEvent );
	if ( dwReturn == WAIT_OBJECT_0 )
		return S_OK;
	else
		return (HRESULT) dwReturn;
}

HRESULT SVInspectionProcess::GetConditionalHistoryList( std::vector <SVScalarValue>& p_rvecData, std::vector <SVScalarValue>& p_rvecImages, std::vector <SVScalarValue>& p_rvecConditionals )
{
	HANDLE hEvent = nullptr;
	hEvent = ::CreateEvent( nullptr, true, false, nullptr );

	SVInspectionMessageDataStruct_CHList data( &p_rvecData, &p_rvecImages, &p_rvecConditionals, false );
	SVInspectionTransactionStruct message( SVInspectionMessageGetCHList, hEvent, &data );
	AddTransaction( message );

	DWORD dwReturn = ::WaitForSingleObjectEx( hEvent, m_dwCHTimeout, TRUE );

	::CloseHandle( hEvent );
	if ( dwReturn == WAIT_OBJECT_0 )
		return S_OK;
	else
		return (HRESULT) dwReturn;
}

HRESULT SVInspectionProcess::GetConditionalHistoryAndClear( std::vector < std::vector <SVScalarValue> >& rvecValues, std::vector < std::vector <SVImageBufferStruct> >& rvecImages, std::vector < std::vector <SVScalarValue> >& rvecConditionals, std::vector<long>& rvecProcessCount )
{
	// check Queue size
	if ( m_qTransactions.GetCount() <= 3 )
	{
		HANDLE hEvent = nullptr;
		hEvent = ::CreateEvent( nullptr, true, false, nullptr );

		SVInspectionMessageDataStruct_CHGetAll data( &rvecValues, &rvecImages, &rvecConditionals, &rvecProcessCount );
		SVInspectionTransactionStruct message( SVInspectionMessageGetCHandClear, hEvent, &data );
		AddTransaction( message );

		DWORD dwReturn = ::WaitForSingleObjectEx( hEvent, m_dwCHTimeout, TRUE );

		::CloseHandle( hEvent );
		if ( dwReturn == WAIT_OBJECT_0 )
			return S_OK;
		else
			return (HRESULT) dwReturn;
	}
	else
	{
		return SVMSG_54_SVIM_BUSY;
	}
}

HRESULT SVInspectionProcess::GetMostRecentConditionalHistory( std::vector <SVScalarValue>& rvecData, std::vector <SVImageBufferStruct>& rvecImages, std::vector <SVScalarValue>& rvecConditionals, long& rlProcessCount )
{
	if ( m_qTransactions.GetCount() <= 3 )
	{
		HANDLE hEvent = nullptr;
		hEvent = ::CreateEvent( nullptr, true, false, nullptr );

		SVInspectionMessageDataStruct_CHGet data( &rvecData, &rvecImages, &rvecConditionals, &rlProcessCount );
		SVInspectionTransactionStruct message( SVInspectionMessageGetCHMostRecent, hEvent, &data );
		AddTransaction( message );

		DWORD dwReturn = ::WaitForSingleObjectEx( hEvent, m_dwCHTimeout, TRUE );

		::CloseHandle( hEvent );
		if ( dwReturn == WAIT_OBJECT_0 )
			return S_OK;
		else
			return (HRESULT) dwReturn;
	}
	else
	{
		return SVMSG_54_SVIM_BUSY;
	}
}

HRESULT SVInspectionProcess::BuildConditionalHistoryListAfterLoad()
{
	// build lists from value object attributes. Must happen after BuildValueObjectMap.
	ASSERT( m_mapValueObjects.size() > 0 );

	SVTaskObjectListClass* pToolSet = static_cast <SVTaskObjectListClass*> ( m_pCurrentToolset );
	if( nullptr != pToolSet )
	{
		SVScalarValueVector vecValues;
		SVScalarValueVector vecConditionals;
		SVScalarValueVector vecImages;
		// VALUE OBJECTS
		{
			std::vector<SVValueObjectReference> vecObjects;
			pToolSet->GetOutputListFiltered( vecObjects, SV_CH_CONDITIONAL | SV_CH_VALUE, false );
			SVValueObjectReferenceVector::iterator iter;
			for( iter = vecObjects.begin(); iter != vecObjects.end(); ++iter )
			{
				SVValueObjectReference ref = *iter;
				UINT uiAttributes = ref.ObjectAttributesSet();
				if ( uiAttributes & SV_CH_VALUE )
				{
					vecValues.push_back( SVScalarValue( ref ) );
				}
				if ( uiAttributes & SV_CH_CONDITIONAL )
				{
					vecConditionals.push_back( SVScalarValue( ref ) );
				}
			}// end for( iter = vecObjects.begin(); iter != vecObjects.end(); ++iter )
		}// end block

		// IMAGE OBJECTS
		{
			SVImageListClass listImages;
			pToolSet->GetImageList( listImages, SV_CH_IMAGE );
			int nCount = listImages.GetSize();
			for(int i=0; i<nCount; i++)
			{
				SVImageClass* pImage = listImages.GetAt(i);
				SVObjectReference refImage( pImage );
				vecImages.push_back( SVScalarValue( refImage ) );
			}
		}// end block

		SetConditionalHistoryList( &vecValues, &vecImages, &vecConditionals, true );

	}// end if( nullptr != pToolSet )

	return S_OK;
}

HRESULT SVInspectionProcess::CollectConditionalHistoryData()
{
	// should we collect the data here? or in CH?
	// for now, do it in CH
	HRESULT hr = m_ConditionalHistory.CollectDataAndStore();
	return hr;
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

	if( !( m_ToolSetCameraName.IsEmpty() ) )
	{
		pCamera = dynamic_cast< SVVirtualCamera* >( SVObjectManagerClass::Instance().GetObjectCompleteName( m_ToolSetCameraName ) );
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

HRESULT SVInspectionProcess::GetMainImages( const CString& p_rCameraName, SVCameraImagePtrSet& p_rMainImages ) const
{
	HRESULT l_Status = S_OK;

	p_rMainImages.clear();

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( l_Iter != m_CameraImages.end() )
	{
		if( nullptr != ( *l_Iter ) )
		{
			SVVirtualCamera* pCamera = ( *l_Iter )->GetCamera();

			if( ( nullptr != pCamera ) && ( p_rCameraName == pCamera->GetName() ) )
			{
				p_rMainImages.insert( *l_Iter );
			}
		}

		++l_Iter;
	}

	return l_Status;
}

void SVInspectionProcess::UpdateSharedMemoryFilters( const SVMonitorList& p_rMonitorList )
{
	m_MonitorListQueue.clear();
	m_ImageNameDequeQueue.clear();

	m_MonitorListQueue.AddTail( p_rMonitorList );

	m_AsyncProcedure.Signal( nullptr );
}

HRESULT SVInspectionProcess::UpdateSharedMemoryLastInspectedImages( const SVMonitorItemList& p_rImageList )
{
	HRESULT l_Status = S_OK;

	m_ImageNameDequeQueue.clear();

	m_ImageNameDequeQueue.AddTail( p_rImageList );

	m_AsyncProcedure.Signal( nullptr );

	return l_Status;
}

HRESULT SVInspectionProcess::AddTransaction( SVInspectionTransactionStruct& p_rMessage )
{
	BOOL bAdd = m_qTransactions.AddTail( p_rMessage );

	m_AsyncProcedure.Signal( nullptr );

	return bAdd ? S_OK : S_FALSE;
}

HRESULT SVInspectionProcess::RemoveImage(SVImageClass* pImage)
{
	HRESULT l_Status = S_OK;

	if( nullptr != pImage )
	{
		SVRemoveImages l_Data;

		l_Data.m_Images.insert( pImage->GetUniqueObjectID() );

		SVObjectManagerClass::Instance().UpdateObservers( "SVInspectionProcess", GetUniqueObjectID(), l_Data );
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

	l_bOk = l_bOk && SVMR_SUCCESS == ::SVSendMessage( m_pCurrentToolset, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), 0 );

	m_isCreated = l_bOk;

	return m_isCreated;
}

HRESULT SVInspectionProcess::ResetObject()
{
	HRESULT l_hrOk = SVObjectClass::ResetObject();

	if( IsColorCamera() )
	{
		if( S_OK != m_rgbMainImageObject.ResetObject() )
		{
			l_hrOk = S_FALSE;
		}
	}

	if( SVMR_SUCCESS != ::SVSendMessage( m_pCurrentToolset, SVM_RESET_ALL_OBJECTS, 0, 0 ) )
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
				SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_Iter;

				l_Iter = p_psvProduct->m_svCameraInfos.find( pCamera );

				if( l_Iter != p_psvProduct->m_svCameraInfos.end() )
				{
					if( l_Iter->second.GetIndex() < 0 )
					{
						Result = SvOi::Err_25043_InvalidSourceIndex;
					}

					SVImageIndexStruct l_svIndex;

					l_svIndex.m_CameraDMIndexHandle.Assign( l_Iter->second.GetSourceImageDMIndexHandle(), SV_INSPECTION );

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
			if( m_runStatus.GetState() == SVRunStatusClass::SV_INVALID )
			{
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_37_INVALID_RUN_STATUS, SvOi::Tid_Empty, StdMessageParams );
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

BOOL SVInspectionProcess::RunInspection( long lResultDataIndex, 
										 SVImageIndexStruct svResultImageIndex, 
										 SVProductInfoStruct *pProduct, 
										 bool p_UpdateCounts )
{
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
	double del = SvTl::setReferenceTime();
	CString infostring;
	infostring.Format(_T("!\n!!Reset, %7.1lf: SVInspectionProcess::RunInspection(), del = %7.1lf\n"),SvTl::GetRelTimeStamp(),del);
	::OutputDebugString(infostring);
#endif

	BOOL l_bOk = FALSE;
	BOOL l_bInputRequest = FALSE;
	BOOL l_bImageRequest = FALSE;
	BOOL l_bUpdateMainImage = FALSE;
	BOOL l_bRestMainImage = FALSE;
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

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
			Exception.setMessage( SVMSG_SVO_38_INPUT_REQUEST_FAILED, SvOi::Tid_Empty, StdMessageParams );

			l_bInputRequest = TRUE;
			m_runStatus.SetInvalid();  //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		if( !ProcessInputImageRequests( pProduct ) )
		{
			Exception.setMessage( SVMSG_SVO_39_IMAGE_REQUEST_FAILED, SvOi::Tid_Empty, StdMessageParams );

			l_bImageRequest = TRUE;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		int MainImageStatus = UpdateMainImagesByProduct( pProduct );

		if( 0 != MainImageStatus )
		{
			Exception.setMessage( SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, SvOi::Tid_Empty, StdMessageParams, MainImageStatus );

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
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingTimeStamp, StdMessageParams, SvOi::Err_17027_ErrorGettingTimeStamp );
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
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvOi::Tid_ErrorGettingTimeStamp, StdMessageParams, SvOi::Err_17028_ErrorGettingTimeStamp );
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

	::SVSendMessage( this, SVM_CONNECT_ALL_INPUTS, 0, 0 );

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
	return m_DeviceName;
}

void SVInspectionProcess::SetDeviceName( LPCTSTR p_szDeviceName )
{
	m_DeviceName = p_szDeviceName;
}

void SVInspectionProcess::ResetName()
{
	SVObjectClass::ResetName();

	SVInspectionNameUpdate l_Data( GetName() );

	SVObjectManagerClass::Instance().UpdateObservers( "SVInspectionProcess", GetUniqueObjectID(), l_Data );
}

void SVInspectionProcess::SetName( const CString& StrString )
{
	SVObjectClass::SetName( StrString );

	SVInspectionNameUpdate l_Data( GetName() );

	SVObjectManagerClass::Instance().UpdateObservers( "SVInspectionProcess", GetUniqueObjectID(), l_Data );
}

HRESULT SVInspectionProcess::RegisterSubObject( SVValueObjectClass* p_pValueObject )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pValueObject )
	{
		m_svValueObjectSet.insert( p_pValueObject );
		m_mapValueObjects.insert( SVValueObjectMap::value_type( p_pValueObject->GetCompleteObjectName(), p_pValueObject ) );
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

HRESULT SVInspectionProcess::RegisterSubObjects( SVTaskObjectClass *p_psvOwner, SVObjectClassPtrArray &p_rsvEmbeddedList )
{
	HRESULT l_hrOk = UnregisterSubObjects( p_psvOwner );

	if( S_OK == l_hrOk )
	{
		long l_lSize = p_rsvEmbeddedList.GetSize();

		for ( long l = 0; l < l_lSize; l++ )
		{
			SVValueObjectClass *l_psvValueObject = dynamic_cast<SVValueObjectClass *>( p_rsvEmbeddedList[ l ] );

			if( nullptr != l_psvValueObject )
			{
				m_svValueObjectSet.insert( l_psvValueObject );
				m_mapValueObjects.insert( SVValueObjectMap::value_type( l_psvValueObject->GetCompleteObjectName(), l_psvValueObject ) );
			}
			else
			{
				SVImageClass *l_psvImageObject = dynamic_cast<SVImageClass *>( p_rsvEmbeddedList[ l ] );

				if( nullptr != l_psvImageObject )
				{
					SVCameraImageTemplate* l_pCameraImage = dynamic_cast< SVCameraImageTemplate* >( l_psvImageObject );

					if( nullptr != l_pCameraImage )
					{
						m_CameraImages.insert( l_pCameraImage );
					}
					else
					{
						m_svImageObjectSet.insert( l_psvImageObject );
					}
				}
			}
		}
	}

	return l_hrOk;
}

HRESULT SVInspectionProcess::UnregisterSubObjects( SVTaskObjectClass *p_psvOwner )
{
	HRESULT l_hrOk = S_OK;

	SVValueObjectClassPtrSet::iterator l_oValueIter = m_svValueObjectSet.begin();

	while( l_oValueIter != m_svValueObjectSet.end() )
	{
		if( nullptr == *l_oValueIter || (*l_oValueIter)->GetOwner() == p_psvOwner )
		{
			m_mapValueObjects.get< to >().erase( *l_oValueIter );
			l_oValueIter = m_svValueObjectSet.erase( l_oValueIter );
		}
		else
		{
			++l_oValueIter;
		}
	}

	SVImageClassPtrSet::iterator l_oImageIter = m_svImageObjectSet.begin();

	while( l_oImageIter != m_svImageObjectSet.end() )
	{
		if( nullptr == *l_oImageIter || (*l_oImageIter)->GetOwner() == p_psvOwner )
		{
			l_oImageIter = m_svImageObjectSet.erase( l_oImageIter );
		}
		else
		{
			++l_oImageIter;
		}
	}

	SVCameraImagePtrSet::iterator l_oCameraIter = m_CameraImages.begin();

	while( l_oCameraIter != m_CameraImages.end() )
	{
		if( nullptr == *l_oCameraIter || (*l_oCameraIter)->GetOwner() == p_psvOwner )
		{
			l_oCameraIter = m_CameraImages.erase( l_oCameraIter );
		}
		else
		{
			++l_oCameraIter;
		}
	}

	return l_hrOk;
}

void SVInspectionProcess::SetToolsetImage( CString sToolsetImage )
{
	m_ToolSetCameraName = sToolsetImage;
}

LPCTSTR SVInspectionProcess::GetToolsetImage()
{
	return m_ToolSetCameraName;
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

HRESULT SVInspectionProcess::GetInspectionImage( const CString& p_strName, SVImageClass*& p_rRefObject )
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

		if( pImage->GetCompleteObjectName() == p_strName )
		{
			// We found the image by name
			p_rRefObject = pImage;
			
			return S_OK;
		}
	}
	return E_FAIL;
}

// This method fills the Toolset structure in Shared Memory (for either LastInspected or Rejects )
void SVInspectionProcess::FillSharedData(long sharedSlotIndex, SVSharedData& rData, const SVFilterElementMap& rValues, const SVFilterElementMap& rImages, SVProductInfoStruct& rProductInfo, SeidenaderVision::SVSharedInspectionWriter& rWriter)
{
	SVProductInfoStruct& l_rProductInfo = rProductInfo;
	
	SVDataManagerHandle dmHandle = l_rProductInfo.oPPQInfo.m_ResultDataDMIndexHandle;
	long dataIndex = dmHandle.GetIndex();

	SVSharedValueMap& rSharedValues = rData.m_Values;
	SVSharedImageMap& rSharedImages = rData.m_Images;

	rSharedValues.clear();
	rSharedImages.clear();

	for (SVFilterElementMap::const_iterator ValueIter = rValues.begin(); ValueIter != rValues.end(); ++ValueIter)
	{
		HRESULT hr = S_OK;
		CString value;
		bool bItemNotFound = true;
		SVValueObjectClass* pValueObject = nullptr;
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(ValueIter->second.ToGUID());

		if (pObject)
		{
			pValueObject = dynamic_cast<SVValueObjectClass *>(pObject);
			if (pValueObject) 
			{
				SVObjectNameInfo nameInfo;
				SVObjectNameInfo::ParseObjectName(nameInfo, ValueIter->first.c_str());
				SVValueObjectReference ref(pObject, nameInfo);
				
				// for now just a single item (no full array)
				if (!ref.IsEntireArray())
				{
					HRESULT hrGet = ref.GetValue(dataIndex, value);
					if (hrGet == SVMSG_SVO_33_OBJECT_INDEX_INVALID  || hrGet == SVMSG_SVO_34_OBJECT_INDEX_OUT_OF_RANGE)
					{
						hr = hrGet;
						value = ref.DefaultValue(); // did not get value. set value to default
						if (value.IsEmpty())
						{
							value.Format("%i", -1);
						}
					}
					else if (S_OK != hrGet)	// some generic error; currently should not get here
					{
						hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
						value.Format("%i", -1); // did not get value. set value to -1
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
					CString sArrayValues;
					for (int iArrayIndex = 0;iArrayIndex < iNumResults && S_OK == hr;iArrayIndex++)
					{
						CString sValue;
						hr = ref.Object()->GetValue(dataIndex, iArrayIndex, sValue);
						if (S_OK == hr)
						{
							if (iArrayIndex > 0)
							{
								sArrayValues += _T(",");
							}
							sArrayValues += _T("`");
							sArrayValues += sValue;
							sArrayValues += _T("`");
						}
					}
					if (S_OK == hr)
					{
						bItemNotFound = false;
						value = sArrayValues;
					}
				}
			}
		}
		if (bItemNotFound)
		{
			hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
		
		std::pair< SVSharedValueMap::iterator,bool>  mRet; 	
		mRet = rSharedValues.insert(SVSharedValuePair(char_string(ValueIter->first.c_str(), rData.m_Allocator), 
					SVSharedValue(SVSharedValue::StringType, static_cast<const char*>(value), hr, rData.m_Allocator)));
	}

	for (SVFilterElementMap::const_iterator imageIter = rImages.begin(); imageIter != rImages.end(); ++imageIter)
	{
		HRESULT hr = S_OK;
		bool bImgNotFound = true;
		SVImageClass* pImage = nullptr;
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(imageIter->second.ToGUID());

		if (nullptr != pObject)
		{
			pImage = dynamic_cast<SVImageClass *>(pObject);

			if (nullptr != pImage)
			{
				SVSmartHandlePointer imageHandlePtr;
			
				// Special check for Color Tool's RGBMainImage which is HSI
				if (SV_IS_KIND_OF(pImage, SVRGBMainImageClass))
				{
					// this will make a copy...
					SVImageProcessingClass::Instance().CreateImageBuffer(pImage->GetImageInfo(), imageHandlePtr);
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
					SVString name = imageIter->first.c_str();
					SVString filename = SeidenaderVision::SVSharedConfiguration::GetImageDirectoryName() + "\\" + SVSharedImage::filename(name.c_str(), sharedSlotIndex, img::bmp);
					// Write Image to disk
					HRESULT hr = SVImageProcessingClass::Instance().SaveImageBuffer(filename.c_str(), imageHandlePtr);
					std::pair<SVSharedImageMap::iterator, bool> IterPair = rSharedImages.insert(SVSharedImagePair(char_string(name.c_str(), rData.m_Allocator), SVSharedImage(filename.c_str(), hr, rData.m_Allocator)));
					// if the Insert failed - update the status
					if (!IterPair.second && IterPair.first != rSharedImages.end())
					{
						IterPair.first->second.m_Status = E_FAIL;
					}
				}
			}
		}
		if (bImgNotFound)
		{
			hr = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			SVString name = imageIter->first.c_str();
			SVString filename;
			std::pair<SVSharedImageMap::iterator, bool> IterPair = rSharedImages.insert(SVSharedImagePair(char_string(name.c_str(), rData.m_Allocator), SVSharedImage(filename.c_str(), hr, rData.m_Allocator)));
			// if the Insert failed - update the status
			if (!IterPair.second && IterPair.first != rSharedImages.end())
			{
				IterPair.first->second.m_Status = E_FAIL;
			}
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

			InsertItem.setName( rObjectRef.GetName() );
			InsertItem.setItemKey( rObjectRef->GetUniqueObjectID().ToVARIANT() );
			if ( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (rObjectRef.Object()) )
			{
				CString typeName = _T("");
				pValueObject->GetTypeName( typeName );
				InsertItem.setItemTypeName( typeName );
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
					InsertItem.setLocation( ArrayObjectRef.GetCompleteOneBasedObjectName() );
					InsertItem.setArrayIndex( i );
					pSelectorList->push_back( InsertItem );
				}
			}
			else
			{
				InsertItem.setLocation( rObjectRef.GetCompleteOneBasedObjectName() );
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

				//Need to replace the inspection name with the PPQ Variables name
				// Only DIO and Remote Input, but is all that is in this list?
				SVString Location( ObjectRef.GetCompleteOneBasedObjectName() );
				SvUl_SF::searchAndReplace(Location, InspectionName.c_str(), SvOl::FqnPPQVariables);
				InsertItem.setName( ObjectRef.GetName() );
				InsertItem.setLocation( Location.c_str() );
				InsertItem.setItemKey( ObjectRef->GetUniqueObjectID().ToVARIANT() );
				if ( const SVValueObjectClass* pValueObject = dynamic_cast<const SVValueObjectClass*> (pObject) )
				{
					CString typeName = _T("");
					pValueObject->GetTypeName( typeName );
					InsertItem.setItemTypeName( typeName );
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
		
		if( ioEntryPtr->m_Enabled == false )
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
	SVStringArray PpqVariableNames;

	for( size_t i = 0; i < m_PPQInputs.size(); i++ )
	{	
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;

		//check if input is enable for this inspection
		if( ioEntryPtr->m_Enabled )
		{
			SVObjectClass* pObject( ioEntryPtr.get()->m_pValueObject );
			if( nullptr != pObject )
			{
				SVString Name( pObject->GetCompleteObjectName() );
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



