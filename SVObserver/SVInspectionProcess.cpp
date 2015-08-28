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
#include "SVInspectionProcess.h"
#include "SVImageLibrary\SVImageBufferHandleImage.h"
#include "SVImageLibrary\SVImagingDeviceParams.h"
#include "SVObjectLibrary\SVObjectLevelCreateStruct.h"
#include "SVObjectLibrary\SVInspectionLevelCreateStruct.h"
#include "SVObjectLibrary\SVObjectManagerClass.h"
#include "SVSystemLibrary\SVAutoLockAndReleaseTemplate.h"
#include "SVConfigurationLibrary\SVConfigurationTags.h"
#include "SVObserver.h"
#include "SVGetObjectDequeByTypeVisitor.h"
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

		bool l_Process = ( hr == S_OK );

		if( !l_Process )
		{
			SVString l_Message;

			l_Message.Format( _T( "Inspection Thread-Name=%s-Error in getting Result Image Index" ), GetName() );

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (hr), l_Message.ToString(), StdMessageParams );
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

								if( l_pVariantObject != NULL )
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
				// The Toolset will return FALSE if it did not run due to a Conditional - SEJ
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
						SVString l_TempStateString;

						l_TempStateString.Format( _T( "SVInspectionProcess::ProcessInspection|%s|TRI=%ld\n" ),
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
			Exception.setMessage( SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvO::ErrorNotAllDataItemsFound, StdMessageParams, SvOi::Err_15021 );
		}
		bNotFound = false;

		const SVMonitorItemList& imageList = l_MonitorList.GetImageList();

		long l_ImageSize = 0;
		for (SVMonitorItemList::const_iterator l_ImageIter = imageList.begin(); l_ImageIter != imageList.end(); ++l_ImageIter)
		{
			const SVString& name = (*l_ImageIter);
			SVImageClass* l_pImage;
			if (GetInspectionImage(name.c_str(), l_pImage) == S_OK)
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
			Exception.setMessage( SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvO::ErrorNotAllImageItemsFound, StdMessageParams, SvOi::Err_15022 );
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
			if ( GetInspectionImage( name.c_str(), l_pImage ) == S_OK)
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
				SVString Message;
				Message.Format( SvO::ErrorProcessNotifyLastInspected, e.what() );

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, Message.c_str(), StdMessageParams, SvOi::Err_15023 );
			}
			catch (...)
			{
				SVString Message;
				Message.Format( SvO::ErrorProcessNotifyLastInspected, SvO::Unknown );

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, Message.c_str(), StdMessageParams, SvOi::Err_15024 );
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
		case SVInspectionMessageResetCH:
		{
			SVInspectionMessageDataStruct_CHReset* pData = dynamic_cast <SVInspectionMessageDataStruct_CHReset*> (p_Message.pData);
			ASSERT( pData );
			hr = m_ConditionalHistory.ResetObject();
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

	if ( hr == S_OK )
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
	outObjectInfo.ObjectTypeInfo.ObjectType = SVInspectionObjectType;

	m_LastRunLockPtr = new SVCriticalSection;
	m_LastRunProductNULL = false;

	//m_pFinishProc		= NULL;
	//m_pOwner = NULL;
	m_pCurrentToolset = NULL;
	m_PPQId.clear();
	//m_pPPQ = NULL;
	m_bNewDisableMethod	= FALSE;
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
	m_lGageToolCount = 0;

	m_svReset.RemoveState( SVResetStateAll );

	m_bForceOffsetUpdate = true;

	m_publishList.m_pInspection = this;

	m_pToolSetConditional = NULL;
}

SVInspectionProcess::~SVInspectionProcess()
{
	m_rgbMainImageObject.CloseObject();

	DestroyInspection();

	m_PPQId.clear();

	//m_pFinishProc		= NULL;
	m_pCurrentToolset	= NULL;
	//m_pPPQ = NULL;
	m_bNewDisableMethod	= FALSE;
	m_lEnableAuxiliaryExtents = 0;
	m_lInputRequestMarkerCount = 0L;
}

bool SVInspectionProcess::IsCameraInInspection( const CString& p_rName ) const
{
	bool l_Status = false;

	SVCameraImagePtrSet::const_iterator l_Iter = m_CameraImages.begin();

	while( !l_Status && l_Iter != m_CameraImages.end() )
	{
		if( ( *l_Iter ) != NULL )
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
		return FALSE;

	if( !m_CommandQueue.Create() )
		return false;

	if( !m_MonitorListQueue.Create() ) { return false; }

	if( !m_ImageNameDequeQueue.Create() ) { return false; }

	::InterlockedExchange( &m_NotifyWithLastInspected, 0 );

	if( m_AsyncProcedure.Create( &SVInspectionProcess::APCThreadProcess, boost::bind(&SVInspectionProcess::ThreadProcess, this, _1), GetName(), SVThreadAttribute::SVAffinityUser ) != S_OK )
	{
		return FALSE;
	}

	if ( !m_qTransactions.Create() )
	{
		return FALSE;
	}

	hr = CreateResultImageIndexManager();
	if( S_OK != hr )
		return FALSE;

	// Create Queues for input/output requests
	if( !m_InputRequests.Create() )
		return FALSE;

	if( !m_InputImageRequests.Create() )
		return FALSE;

	m_pCurrentToolset	= new SVToolSetClass( TRUE, this );

	if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pCurrentToolset), NULL ) != SVMR_SUCCESS )
	{
		return FALSE;
	}

	if ( m_ConditionalHistory.ResetObject() != S_OK )
		return FALSE;

	SVCommandStreamManager::Instance().InsertInspection( GetUniqueObjectID() );

	return TRUE;
}// end Create

HRESULT SVInspectionProcess::CreateResultImageIndexManager() const
{
	HRESULT l_Status = S_OK;

	// Create a managed index for the result data and image circle buffers
	SVString l_Temp;

	l_Temp.Format( _T( "%s Result Image" ), GetName() );

	l_Status = TheSVDataManager.CreateManagedIndexArray( m_pResultImageCircleBuffer, l_Temp.ToBSTR(), TheSVObserverApp.GetResultImageDepth() );

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

	if( m_pCurrentToolset != nullptr )
	{
		::SVSendMessage(this, SVM_DESTROY_CHILD_OBJECT,reinterpret_cast<DWORD_PTR>( m_pCurrentToolset ), NULL);
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
			ProcessTransaction( message );
	}

	m_qTransactions.Destroy();

	m_PPQInputs.clear();

	m_PPQId.clear();

	m_pResultImageCircleBuffer.clear();

	SVResultListClass* pResultlist = GetResultList();
	if(pResultlist)
	{
		
		pResultlist->RebuildReferenceVector(this);

	}

	return TRUE;
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

	SVValueObjectClass* l_pObject = NULL;

	hr = l_NameInfo.ParseObjectName( static_cast< LPCTSTR >( p_strName ) );

	if( hr == S_OK )
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

	if ( l_pObject != NULL )
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
	if ( hr == S_OK )
	{
		p_rRefObject = ref;
	}
	else	// if not a value object, try getting images
	{
		SVImageClass* pImage = NULL;
		hr = GetInspectionImage( p_strName, pImage );
		if ( hr == S_OK )
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

	l_bOk = ( InitializeRunOnce() == S_OK );

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

	l_bOk = ::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS;

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

	LastProductUpdate( NULL );

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

	SVSendMessage( this, SVM_GOING_OFFLINE, NULL, NULL ); // EB 2005 09 20

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
		l_Status = m_AsyncProcedure.Signal( NULL );
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
		long	ppqInputSize = m_PPQInputs.size();
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

			if( l_pImage != NULL )
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

			if( l_pImage != NULL )
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

		if( hr == S_OK && m_qInspectionsQueue.AddTail( l_TempProduct ) )
		{
			SVObjectManagerClass::Instance().IncrementInspectionIndicator();

			hr = m_AsyncProcedure.Signal( NULL );
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

			::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast< DWORD_PTR>(pValueObject), NULL );

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
	return AddInputRequest( NULL, SvO::SVTOOLPARAMETERLIST_MARKER );
}

BOOL SVInspectionProcess::AddInputRequest( SVInputRequestInfoStructPtr p_pInRequest )
{
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorLockingInputRequests, StdMessageParams, SvOi::Err_17013_ErrorLockingInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.AddTail( p_pInRequest ) )
	{
		if( !m_InputRequests.Unlock() )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17014_ErrorUnlockingInputRequests );
			DebugBreak();
		}

		return FALSE;
	}// end if


	SVString l_StringValue;

	if( p_pInRequest->m_Value.vt == VT_BSTR )
	{
		l_StringValue = p_pInRequest->m_Value;
	}

	if( l_StringValue == SvO::SVTOOLPARAMETERLIST_MARKER )
	{
		::InterlockedIncrement(const_cast<long*>(&m_lInputRequestMarkerCount));
	}// end if

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17015_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	return TRUE;
}// end AddInputRequest

HRESULT SVInspectionProcess::AddInputImageRequest( SVImageClass* p_psvImage, BSTR& p_rbstrValue )
{
	HRESULT l_Status = S_OK;

	if( p_psvImage != NULL )
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

				l_pInRequest->m_bUsingCameraName = l_psvMainImage != NULL;

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

				if( l_Status == S_OK )
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

	if( p_psvImage != NULL )
	{
		SVImageInfoClass l_ImageInfo = p_psvImage->GetImageInfo();

		try
		{
			SVInputImageRequestInfoStructPtr l_pInRequest = new SVInputImageRequestInfoStruct;

			SVCameraImageTemplate* l_psvMainImage = dynamic_cast< SVCameraImageTemplate* >( p_psvImage );

			l_Status = SVImageProcessingClass::Instance().LoadImageBuffer( p_rImageFileName.c_str(), 
				l_pInRequest->m_ImageInfo, l_pInRequest->m_ImageHandlePtr, false );

			l_pInRequest->m_bUsingCameraName = l_psvMainImage != NULL;

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

			if( l_Status == S_OK )
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
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorLockingInputImageRequests, StdMessageParams, SvOi::Err_17016_ErrorLockingInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.AddTail( p_pInRequest ) )
	{
		if( !m_InputImageRequests.Unlock() )
		{
			SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
			e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorUnlockingInputImageRequests, StdMessageParams, SvOi::Err_17017_ErrorUnlockingInputImageRequests );
			DebugBreak();
		}

		return S_FALSE;
	}// end if

	if( !m_InputImageRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorUnlockingInputImageRequests, StdMessageParams, SvOi::Err_17018_ErrorUnlockingInputImageRequests );
		DebugBreak();
	}

	return S_OK;
}// end AddInputImageRequest

BOOL SVInspectionProcess::RemoveAllInputRequests()
{
	if( !m_InputRequests.Lock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorLockingInputRequests, StdMessageParams, SvOi::Err_17019_ErrorLockingInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.RemoveAll() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorRemovingAllInputRequests, StdMessageParams, SvOi::Err_17020_ErrorRemovingAllInputRequests );
		DebugBreak();
	}

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17021_ErrorUnlockingInputRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.Lock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorLockingInputImageRequests, StdMessageParams, SvOi::Err_17022_ErrorLockingInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.RemoveAll() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorRemovingAllInputImageRequests, StdMessageParams, SvOi::Err_17023_ErrorRemovingAllInputImageRequests );
		DebugBreak();
	}

	if( !m_InputImageRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorUnlockingInputImageRequests, StdMessageParams, SvOi::Err_17024_ErrorUnlockingInputImageRequests );
		DebugBreak();
	}

	return TRUE;
}// end RemoveAllInputRequests

//******************************************************************************
// Message Operation(s):
//******************************************************************************

long SVInspectionProcess::GetGageToolCount()
{
	return m_lGageToolCount;
}

void SVInspectionProcess::GageToolCountInc()
{
	::InterlockedIncrement( &m_lGageToolCount );
}

void SVInspectionProcess::GageToolCountDec()
{
	::InterlockedDecrement( &m_lGageToolCount );
}

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
	if ( TheSVObserverApp.IsColorSVIM() )
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
			SVAcquisitionClassPtr pCamDevice = NULL;

			if( pos->second != NULL )
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
					if ( iter->second.bColor )
						bCameraSupportsColor = true;
				}

				if ( pParam && pParam->options.find(pParam->strValue)->second.bColor )
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

				ASSERT(FALSE);
				AfxMessageBox(_T("Color camera not attached to this IPDoc"));
			}
		}
		else
		{
			if( ::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(&m_rgbMainImageObject), NULL ) != SVMR_SUCCESS )
			{
				l_Status = E_FAIL;
			}

			m_rgbMainImageObject.UpdateImage( SVImageTypePhysical, SV_GUID_NULL );
			m_rgbMainImageObject.SetObjectOwner( this );
		}
	}
	////////////////////////

	SVObjectLevelCreateStruct createStruct;

	if( ::SVSendMessage( this, SVM_CREATE_ALL_OBJECTS,reinterpret_cast<DWORD_PTR>(&createStruct), NULL ) != SVMR_SUCCESS )
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
	if( m_pCurrentToolset != NULL )
	{
		m_pCurrentToolset->ClearResetCounts();
	}
}

void SVInspectionProcess::SetResetCounts()
{
	if( m_pCurrentToolset != NULL )
	{
		m_pCurrentToolset->ResetCounts();
	}
}

void SVInspectionProcess::ValidateAndInitialize( bool p_Validate, bool p_IsNew )
{
	SVSVIMStateClass::AddState( SV_STATE_INTERNAL_RUN );

	// SEJ - Call ToolSet Validate (used to be PrepareForRunning)
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
					pChildObject->SetObjectDepthWithIndex( objectDepth, l_LastIndex );
					pChildObject->SetImageDepth( mlImageDepth );

					// Set defaults, to ensure that no invalid input info exists...
					//SetDefaultInputs();

					createStruct.OwnerObjectInfo = this;
					createStruct.InspectionObjectInfo = this;

					DWORD_PTR l_Return = SVSendMessage( pChildObject, SVM_CREATE_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );

					if( ( DwMessageContext & SVMFResetObject ) == SVMFResetObject )
					{
						::SVSendMessage( pChildObject, SVM_RESET_ALL_OBJECTS, NULL, NULL );
					}

					if( ( DwMessageContext & SVMFSetDefaultInputs ) == SVMFSetDefaultInputs )
					{
						SetDefaultInputs();
					}

					if( ( DwMessageContext & SVMFResetInspection ) == SVMFResetInspection )
					{
						::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, NULL, NULL );
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

				return SVSendMessage( pChildObject, SVM_CONNECT_ALL_OBJECTS, reinterpret_cast<DWORD_PTR>(&createStruct), NULL );
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
							m_pCurrentToolset = NULL;
						}

						// Get the Owner
						SVObjectInfoStruct ownerInfo = pObject->GetOwnerInfo();

						SVObjectClass* pOwner = SVObjectManagerClass::Instance().GetObject(ownerInfo.UniqueObjectID);
						if (pOwner)
						{
							// Ask the owner to kill the imposter!
							if (::SVSendMessage(pOwner, SVM_DESTROY_CHILD_OBJECT,reinterpret_cast<DWORD_PTR>(pObject), NULL) == SVMR_NO_SUCCESS)
							{
								// must be a Friend
								pOwner->DestroyFriends();
							}
						}
					}

					// SEJ - Sep 28,1999
					// Special code for Objects that allocate Friends on SetOwner()
					pMessageObject->DestroyFriends();

					if( SVObjectManagerClass::Instance().ChangeUniqueObjectID( pMessageObject, ObjectID ) )
					{
						SVToolSetClass* l_pToolSet( NULL );

						l_pToolSet = dynamic_cast< SVToolSetClass* >( pMessageObject );

						if( l_pToolSet != NULL )
						{
							if( m_pCurrentToolset != NULL )
							{
								::SVSendMessage( this, SVM_DESTROY_CHILD_OBJECT,reinterpret_cast<DWORD_PTR>( m_pCurrentToolset), NULL );
							}

							m_pCurrentToolset = l_pToolSet;

							m_pCurrentToolset->SetObjectOwner( this );

							::SVSendMessage( this, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pCurrentToolset), NULL );

							if( m_pToolSetConditional != NULL )
							{
								m_pCurrentToolset->AddFriend( m_pToolSetConditional->GetUniqueObjectID() );

								::SVSendMessage( m_pCurrentToolset, SVM_CREATE_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(m_pToolSetConditional), NULL );

								m_pToolSetConditional = NULL;
							}
						}
						else
						{
							SVConditionalClass* l_pConditional( NULL );

							l_pConditional = dynamic_cast< SVConditionalClass* >( pMessageObject );

							if( l_pConditional != NULL )
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
					m_pCurrentToolset = NULL;
				}

				// if we have an Object
				if( pObject != NULL )
				{
					SVTaskObjectClass* l_pTaskObject( NULL );

					l_pTaskObject = dynamic_cast< SVTaskObjectClass* >( pObject );

					if( l_pTaskObject != NULL )
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
						::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, NULL, NULL );
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
	ASSERT( ppObject != NULL );
	bool bReturn = false;

	if ( ppObject != NULL )
	{
		*ppObject = NULL;

		CString sChildName = tszChildName;
		CString sName = GetCompleteObjectName();
		if ( sChildName.Left(sName.GetLength()) == sName )
		{
			*ppObject = reinterpret_cast<SVObjectClass*>(::SVSendMessage( this, 
					( SVM_GET_OBJECT_BY_NAME | SVM_PARENT_TO_CHILD ) & ~SVM_NOTIFY_ONLY_THIS, 
					reinterpret_cast<DWORD_PTR>(tszChildName), NULL ));
			bReturn = ( *ppObject != NULL );
		}
	}

	return bReturn;
}

HRESULT SVInspectionProcess::FindPPQInputObjectByName( SVObjectClass*& p_rpObject, LPCTSTR p_FullName ) const
{
	HRESULT l_Status = S_OK;

	for( size_t l = 0; p_rpObject == NULL && l < m_PPQInputs.size(); ++l )
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

	if( p_rpObject == NULL )
	{
		l_Status = S_FALSE;
	}

	return l_Status;
}

HRESULT SVInspectionProcess::GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index ) const
{
	HRESULT l_Status = SVObjectClass::GetChildObject( p_rpObject, p_rNameInfo, p_Index );

	if( l_Status != S_OK )
	{
		if( 0 < p_rNameInfo.m_NameArray.size() && p_rNameInfo.m_NameArray[ p_Index ] == GetName() )
		{
			SVString l_ObjectName = p_rNameInfo.GetObjectName( p_Index );

			l_Status = FindPPQInputObjectByName( p_rpObject, l_ObjectName.c_str() );

			if( l_Status != S_OK )
			{
				if( GetToolSet() )
				{
					l_Status = GetToolSet()->GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}
			}
		}
		else
		{
			if( 0 < p_rNameInfo.m_NameArray.size() && p_rNameInfo.m_NameArray[ p_Index ] == _T( "Tool Set" ) )
			{
				if( GetToolSet() )
				{
					l_Status = GetToolSet()->GetChildObject( p_rpObject, p_rNameInfo, p_Index );
				}
			}
		}
	}

	return l_Status;
}
	
HRESULT SVInspectionProcess::ObserverUpdate( const SVAddTool& p_rData )
{
	HRESULT l_Status = S_OK;

	if( p_rData.m_pTool != NULL )
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

	if( p_rData.m_pTool != NULL )
	{
		// Call conditional history list and remove tool
		// Conditional history needs references deleted before we delete the tool.
		CString l_strToolName = p_rData.m_pTool->GetCompleteObjectName();
		m_ConditionalHistory.DeleteTool( l_strToolName );
		
		// Delete the Tool Object
		::SVSendMessage( GetToolSet(), SVM_DESTROY_CHILD_OBJECT, reinterpret_cast<DWORD_PTR>(p_rData.m_pTool), NULL );

		// Refresh Lists ( inputs,outputs,results,published )
		SetDefaultInputs();

		// Create Child closed object
		::SVSendMessage( this, SVM_CREATE_ALL_OBJECTS, NULL, NULL );

		// Reset all objects
		::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, NULL, NULL );

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

	if( l_pObject != NULL )
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

	if( SVResetItemIP < l_eResetItem && p_psvTool != NULL )
	{
		bRet = p_psvTool->GetInspection() == this;

		if( bRet )
		{
			SVRunStatusClass runStatus;
			runStatus.ClearAll();

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
		
		if( l_Status == S_OK ) { l_Status = l_Temp; }

		l_Temp = ReserveNextResultImage( &l_svProduct, SV_INSPECTION, true );

		if( l_Status == S_OK ) { l_Status = l_Temp; }

		l_Temp = l_svProduct.GetResultImageIndex( l_svResultImageIndex, GetUniqueObjectID() );

		if( l_Status == S_OK ) { l_Status = l_Temp; }

		l_Temp = l_svProduct.GetResultDataIndex( l_ResultDataDMIndexHandle );

		if( l_Status == S_OK ) { l_Status = l_Temp; }

		if( !( RunInspection( l_ResultDataDMIndexHandle.GetIndex(), l_svResultImageIndex, &l_svProduct, false ) ) && l_Status == S_OK )
		{
			l_Status = E_FAIL;
		}

		l_Temp = LastProductUpdate( &l_svProduct );

		if( l_Status == S_OK ) { l_Status = l_Temp; }
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
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorLockingInputRequests, StdMessageParams, SvOi::Err_17025_ErrorLockingInputRequests );
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

					l_StringValue = SVString( l_Variant );
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
						l_StringValue += SVString( l_Variant );
						l_StringValue += _T("`");
					}
				}
			}
			else
			{
				l_StringValue = l_pInputRequest->m_Value;
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
			if( refValueObject.Object() != NULL )
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
					if( l_StringValue.CompareNoCase( _T( "TRUE" ) ) == 0 )
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

					/* How to compare to previous??????
					if( ! l_bResetObject )
					{
						CString l_csPrevValue;

						hrSet = refValueObject.GetValue( l_csPrevValue );

						l_bResetObject = l_csPrevValue.Compare( l_StringValue.c_str() ) != 0;
					}
					//*/

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
				else if ( dynamic_cast <SVEnumerateValueObjectClass*> ( refValueObject.Object() ) != NULL )
				{
					// Convert to the appropriate type of value
					bool l_bTempReset = false;

					if( ! l_bResetObject )
					{
						HRESULT hrCompare = refValueObject->CompareWithCurrentValue( l_StringValue.c_str() );
						l_bTempReset = ( hrCompare != S_OK );	// reset if not the same
					}

					if( (hrSet = refValueObject.SetValue( 1, l_StringValue.c_str() )) == S_OK )
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
				else if ( dynamic_cast <SVDoubleValueObjectClass*> ( refValueObject.Object() ) != NULL ) // Double
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
						TCHAR* p = NULL;
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

					if( l_psvTool != NULL )
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
		if ( m_pCurrentToolset != NULL )
		{
			if( p_reResetItem == SVResetItemIP )
			{
				l_bRet &= ::SVSendMessage( this, SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS;
				l_bRet &= m_pCurrentToolset->ClearResetCounts() == S_OK;
			}
			else
			{
				int l_iSize = m_pCurrentToolset->GetSize();

				for ( l = 0; l < l_iSize; l++ )
				{
					SVToolClass *l_psvTool = dynamic_cast<SVToolClass *>(m_pCurrentToolset->GetAt( l ));

					if ( l_psvTool != NULL )
					{
						if( p_rsvToolMap.find( l_psvTool ) != p_rsvToolMap.end() )
						{
							if( p_rsvToolMap[ l_psvTool ].m_svObjectSet.empty() )
							{
								l_bRet &= ::SVSendMessage( l_psvTool, SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS;
							}
							else
							{
								SVStdSetSVObjectClassPtr::iterator l_oIter;
								
								l_oIter = p_rsvToolMap[ l_psvTool ].m_svObjectSet.begin();

								while( l_oIter != p_rsvToolMap[ l_psvTool ].m_svObjectSet.end() )
								{
									SVObjectClass *l_psvObject = *l_oIter;

									l_bRet &= ::SVSendMessage( l_psvObject, SVM_RESET_ALL_OBJECTS, NULL, NULL ) == SVMR_SUCCESS;

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
		}// end if ( m_pCurrentToolset != NULL )
	}// end while( m_lInputRequestMarkerCount > 0L )

	if( !m_InputRequests.Unlock() )
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorUnlockingInputRequests, StdMessageParams, SvOi::Err_17026_ErrorUnlockingInputRequests );
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
				SVImageClass *l_psvImage = NULL;

				if ( l_pInRequest->m_bUsingCameraName ) //not using camera name...
				{
					SVCameraImagePtrSet l_MainImages;

					GetMainImages( l_pInRequest->m_strObjectName, l_MainImages );

					SVCameraImagePtrSet::iterator l_Iter = l_MainImages.begin();

					while( l_psvImage == NULL && l_Iter != l_MainImages.end() )
					{
						SVCameraImageTemplate* l_pCameraImage = ( *l_Iter );

						if( ( l_pCameraImage != NULL ) && ( l_pCameraImage->GetObjectSubType() != SVRGBMainImageObjectType ) )
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
					SVObjectClass* l_psvObject = NULL;

					if( GetChildObjectByName(l_pInRequest->m_strObjectName, &l_psvObject) )
					{
						l_psvImage = dynamic_cast<SVImageClass*>( l_psvObject );
					}
				}

				l_bOk = l_psvImage != NULL;

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

		if( l_pCameraImage != NULL )
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

		if( l_pCameraImage != NULL )
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
		if( ( *l_Iter ) != NULL )
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

long SVInspectionProcess::GetEnableAuxiliaryExtent()
{
	long l_lTemp = 0;

	if( GetGageToolCount() < 1 )
	{
		l_lTemp = m_lEnableAuxiliaryExtents;
	}

	return l_lTemp;
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

	if( l_pToolSet != NULL )
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
		l_pInRequest = NULL;
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

	m_LastRunProductNULL = p_psvProduct == NULL;

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

	if( p_psvProduct != NULL )
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

	m_AsyncProcedure.Signal( NULL );

	return l_hrOk;
}

HRESULT SVInspectionProcess::SetSourceImagesTo( SVProductInfoStruct *p_psvProduct )
{
	HRESULT l_hrOk = S_OK;

	if( p_psvProduct != NULL )
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

		if( dynamic_cast<SVRGBMainImageClass*>(l_pMainImage) == NULL )
		{
			if( l_pMainImage != NULL )
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

	if( l_Status == S_OK )
	{
		l_Status = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultImageCircleBuffer, p_LockType, p_rInspectionInfo.m_ResultImageDMIndexHandle );

		#ifdef _DEBUG
			if( l_Status != S_OK )
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

HRESULT SVInspectionProcess::SetConditionalHistoryProperties( SVScalarValueVectorType& p_rvecProperties, bool p_bResetObject )
{
	HANDLE hEvent = NULL;
	SVInspectionMessageDataStruct_CHProperties data( &p_rvecProperties, p_bResetObject );

	hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	SVInspectionTransactionStruct message( SVInspectionMessageSetCHProperties, hEvent, &data );

	/*	// possible optimization for use in inspection thread
	if ( ::GetCurrentThreadId() == m_dwThreadId )
		return ProcessTransaction( message );	// in this case don't CreateEvent
	else
	//*/
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

HRESULT SVInspectionProcess::GetConditionalHistoryProperties( SVScalarValueVectorType& p_rvecProperties )
{
	HANDLE hEvent = NULL;
	hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

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
	SVScalarValueVectorType vecProperties;
	HRESULT hr = GetConditionalHistoryProperties( vecProperties );
	if ( hr == S_OK )
	{
		for ( size_t i=0; i < vecProperties.size(); ++i )
			rmapProperties[ vecProperties[i].strName ]  = vecProperties[i];
	}
	return hr;
}

HRESULT SVInspectionProcess::SetConditionalHistoryList( std::vector <SVScalarValue>* p_pvecData, std::vector <SVScalarValue>* p_pvecImages, std::vector <SVScalarValue>* p_pvecConditionals, bool p_bResetObject )
{
	HANDLE hEvent = NULL;
	hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

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
	HANDLE hEvent = NULL;
	hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

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
		HANDLE hEvent = NULL;
		hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

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
		HANDLE hEvent = NULL;
		hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

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

HRESULT SVInspectionProcess::ResetConditionalHistory()
{
	HANDLE hEvent = NULL;
	hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );

	SVInspectionMessageDataStruct_CHReset data;
	SVInspectionTransactionStruct message( SVInspectionMessageResetCH, hEvent, &data );
	AddTransaction( message );

	DWORD dwReturn = ::WaitForSingleObjectEx( hEvent, m_dwCHTimeout, TRUE );

	::CloseHandle( hEvent );
	if ( dwReturn == WAIT_OBJECT_0 )
		return S_OK;
	else
		return (HRESULT) dwReturn;
}

HRESULT SVInspectionProcess::BuildConditionalHistoryListAfterLoad()
{
	// build lists from value object attributes. Must happen after BuildValueObjectMap.
	ASSERT( m_mapValueObjects.size() > 0 );

	SVTaskObjectListClass* pToolSet = static_cast <SVTaskObjectListClass*> ( m_pCurrentToolset );
	if( pToolSet != NULL )
	{
		std::vector<SVScalarValue> vecValues;
		std::vector<SVScalarValue> vecConditionals;
		std::vector<SVScalarValue> vecImages;
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

	}// end if( pToolSet != NULL )


	return S_OK;
}

/*
HRESULT SVInspectionProcess::SetConditionalHistoryProperties( const SVScalarValueMapType& rmapProperties )
{
	return m_ConditionalHistory.SetProperties( rmapProperties );
}

HRESULT SVInspectionProcess::GetConditionalHistoryProperties( SVScalarValueMapType& rmapProperties )
{
	return m_ConditionalHistory.GetProperties( rmapProperties );
}

HRESULT SVInspectionProcess::SetConditionalHistoryList( std::vector <SVScalarValue>* pvecData, std::vector <SVScalarValue>* pvecImages, std::vector <SVScalarValue>* pvecConditionals )
{
	return m_ConditionalHistory.SetList( pvecData, pvecImages, pvecConditionals );
}

HRESULT SVInspectionProcess::GetConditionalHistoryList( std::vector <SVScalarValue>& rvecData, std::vector <SVScalarValue>& rvecImages, std::vector <SVScalarValue>& rvecConditionals )
{
	return m_ConditionalHistory.GetList( rvecData, rvecImages, rvecConditionals );
}

HRESULT SVInspectionProcess::GetConditionalHistoryAndClear( std::vector <SVScalarValue>& rvecData, std::vector <SVImageBufferStruct>& rvecImages )
{
	return m_ConditionalHistory.GetHistoryAndClear( rvecData, rvecImages );
}

HRESULT SVInspectionProcess::GetMostRecentConditionalHistory( std::vector <SVScalarValue>& rvecData, std::vector <SVImageBufferStruct>& rvecImages )
{
	return m_ConditionalHistory.GetMostRecentHistory( rvecData, rvecImages );
}
//*/

HRESULT SVInspectionProcess::CollectConditionalHistoryData()
{
	// should we collect the data here? or in CH?
	// for now, do it in CH
	HRESULT hr = m_ConditionalHistory.CollectDataAndStore();
	return hr;
}


BOOL SVInspectionProcess::IsColorInspectionDocument() const
{
	BOOL bRetVal = false;

	SVVirtualCameraMap l_Cameras;

	SVPPQObject* pPPQ = GetPPQ();

	if( nullptr != pPPQ )
	{
		pPPQ->GetVirtualCameras( l_Cameras );
	}

	SVVirtualCameraMap::const_iterator pos = l_Cameras.begin();

	while( !bRetVal && pos != l_Cameras.end() )
	{
		if( pos->second != NULL )
		{
			SVAcquisitionClassPtr pCamDevice = pos->second->GetAcquisitionDevice();

			bRetVal = ( !( pCamDevice.empty() ) && pCamDevice->BandSize() == 3L );
		}

		++pos;
	}

	return bRetVal;
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
			if( l_Iter->second != NULL )
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
		if( ( *l_Iter ) != NULL )
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
		if( ( *l_Iter ) != NULL )
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if( nullptr != pCamera )
			{
				if (!pCamera->IsFileAcquisition() && pCamera->GetAcquisitionDevice() != NULL)
				{
					SVLut lut;
					HRESULT hr = pCamera->GetLut(lut);
					if (hr == S_OK && lut.NumBands() > 0)
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
		if( ( *l_Iter ) != NULL )
		{
			SVVirtualCamera* pCamera = (*l_Iter)->GetCamera();

			if( nullptr != pCamera )
			{
				if (!pCamera->IsFileAcquisition() && pCamera->GetAcquisitionDevice() != NULL)
				{
					SVLightReference lightReference;
					HRESULT hr = pCamera->GetLightReference(lightReference);
					if (hr == S_OK && lightReference.NumBands() > 0)
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
		if( ( *l_Iter ) != NULL )
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

HRESULT SVInspectionProcess::UpdateSharedMemoryFilters( const SVMonitorList& p_rMonitorList )
{
	HRESULT l_Status = S_OK;

	m_MonitorListQueue.clear();
	m_ImageNameDequeQueue.clear();

	m_MonitorListQueue.AddTail( p_rMonitorList );

	m_AsyncProcedure.Signal( NULL );

	return l_Status;
}

HRESULT SVInspectionProcess::UpdateSharedMemoryLastInspectedImages( const SVMonitorItemList& p_rImageList )
{
	HRESULT l_Status = S_OK;

	m_ImageNameDequeQueue.clear();

	m_ImageNameDequeQueue.AddTail( p_rImageList );

	m_AsyncProcedure.Signal( NULL );

	return l_Status;
}

HRESULT SVInspectionProcess::AddTransaction( SVInspectionTransactionStruct& p_rMessage )
{
	BOOL bAdd = m_qTransactions.AddTail( p_rMessage );

	m_AsyncProcedure.Signal( NULL );

	return bAdd ? S_OK : S_FALSE;
}

HRESULT SVInspectionProcess::RemoveImage(SVImageClass* pImage)
{
	HRESULT l_Status = S_OK;

	if( pImage != NULL )
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

	l_bOk = l_bOk && ::SVSendMessage( m_pCurrentToolset, SVM_CREATE_ALL_OBJECTS,reinterpret_cast<DWORD_PTR>(&createStruct), NULL ) == SVMR_SUCCESS;

	isCreated = l_bOk;

	return isCreated;
}

HRESULT SVInspectionProcess::ResetObject()
{
	HRESULT l_hrOk = SVObjectClass::ResetObject();

	if( TheSVObserverApp.IsColorSVIM() )
	{
		if( m_rgbMainImageObject.ResetObject() != S_OK )
		{
			l_hrOk = S_FALSE;
		}
	}

	if( ::SVSendMessage( m_pCurrentToolset, SVM_RESET_ALL_OBJECTS, NULL, NULL ) != SVMR_SUCCESS )
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
		GetToolSet()->SetInvalid();
}

int SVInspectionProcess::UpdateMainImagesByProduct( SVProductInfoStruct* p_psvProduct )
{
	int Result( 0 );

	SVCameraImagePtrSet::iterator l_ImageIter = m_CameraImages.begin();

	while( l_ImageIter != m_CameraImages.end() )
	{
		SVCameraImageTemplate* l_pImage = ( *l_ImageIter );

		if( l_pImage != NULL )
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
				Exception.setMessage( SVMSG_SVO_37_INVALID_RUN_STATUS, nullptr, StdMessageParams );
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
	BOOL l_bOk = FALSE;
	BOOL l_bInputRequest = FALSE;
	BOOL l_bImageRequest = FALSE;
	BOOL l_bUpdateMainImage = FALSE;
	BOOL l_bRestMainImage = FALSE;
	SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );

	// Get the info struct for this inspection
	SVInspectionInfoStruct& l_rIPInfo = pProduct->m_svInspectionInfos[ GetUniqueObjectID() ];

	m_runStatus.ClearAll();
	m_runStatus.m_lResultDataIndex = lResultDataIndex;
	m_runStatus.Images = svResultImageIndex;
	m_runStatus.m_lTriggerCount = pProduct->oTriggerInfo.lTriggerCount;
	m_runStatus.m_UpdateCounters = p_UpdateCounts;

	if ( pProduct->IsAlive() )
	{
		// Process all input requests
		if( !ProcessInputRequests( lResultDataIndex, m_bForceOffsetUpdate ) )
		{
			Exception.setMessage( SVMSG_SVO_38_INPUT_REQUEST_FAILED, nullptr, StdMessageParams );

			l_bInputRequest = TRUE;
			m_runStatus.SetInvalid();  //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		if( !ProcessInputImageRequests( pProduct ) )
		{
			Exception.setMessage( SVMSG_SVO_39_IMAGE_REQUEST_FAILED, nullptr, StdMessageParams );

			l_bImageRequest = TRUE;
			m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		}

		int MainImageStatus = UpdateMainImagesByProduct( pProduct );

		if( 0 != MainImageStatus )
		{
			Exception.setMessage( SVMSG_SVO_40_INFO_UPDATE_MAINIMAGE_FAILED, nullptr, StdMessageParams, MainImageStatus );

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

			if( l_pImage != NULL )
			{
				l_pImage->ResetImageIndex();
			}

			++l_ImageIter;
		}

		m_runStatus.SetInvalid(); //sets run.status.valid = false, and since no bits are set = SV_INVALID
		//do a copy forward, and NOT Run();
		l_bRestMainImage = TRUE;
	}

	// Tool set is disabled! // Put out tool set diabled...
	if( !GetToolSet()->IsEnabled() )
	{
		l_rIPInfo.oInspectedState = PRODUCT_INSPECTION_DISABLED;
	}// end if

	l_rIPInfo.m_BeginToolset = SVClock::GetTimeStamp();
	if (!l_rIPInfo.m_BeginToolset)
	{
		SvStl::MessageMgrNoDisplay e( SvStl::LogOnly );
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingTimeStamp, StdMessageParams, SvOi::Err_17027_ErrorGettingTimeStamp );
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
		e.setMessage( SVMSG_SVO_55_DEBUG_BREAK_ERROR, SvO::ErrorGettingTimeStamp, StdMessageParams, SvOi::Err_17028_ErrorGettingTimeStamp );
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

	return l_bOk;

}// end RunInspection

void SVInspectionProcess::SetDefaultInputs()
{
	// Setup Connections
	if( m_pCurrentToolset )
	{
		m_pCurrentToolset->GetAllInputObjects();
	}// end if

	::SVSendMessage( this, SVM_CONNECT_ALL_INPUTS, NULL, NULL );

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

	l_bOk &= GetToolSet() != NULL && GetToolSet()->SetObjectDepth( NewObjectDepth );


	return l_bOk;
}

BOOL SVInspectionProcess::SetObjectDepthWithIndex( int NewObjectDepth, int NewLastSetIndex )
{
	BOOL l_bOk = SVObjectClass::SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	l_bOk &= GetToolSet() != NULL && GetToolSet()->SetObjectDepthWithIndex( NewObjectDepth, NewLastSetIndex );

	return l_bOk;
}

BOOL SVInspectionProcess::SetImageDepth( long lDepth )
{
	BOOL l_bOk = SVObjectClass::SetImageDepth( lDepth );

	l_bOk &= GetToolSet() != NULL && GetToolSet()->SetImageDepth( lDepth );

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
			if( l_rIOEntry.m_IOEntryPtr->m_pValueObject != NULL )
			{
				l_Objects.push_back( l_rIOEntry.m_IOEntryPtr->m_pValueObject );
			}
		}
	}

	if( TheSVObserverApp.IsColorSVIM() )
	{
		SVRGBMainImageClass* l_pImage = const_cast< SVRGBMainImageClass* >( &m_rgbMainImageObject );

		l_Objects.push_back( l_pImage );
	}

	if( GetToolSet() != NULL )
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
	if( TheSVObserverApp.IsColorSVIM() )
		return GetToolSet()->getCurrentImage();
	else
		return NULL;
}

SVImageClass* SVInspectionProcess::GetHSIMainImage()
{
	if( TheSVObserverApp.IsColorSVIM() )
		return &m_rgbMainImageObject;
	else
		return NULL;
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

	if( p_pValueObject != NULL )
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

	if( p_pImageObject != NULL )
	{
		SVCameraImageTemplate* l_pCameraImage = dynamic_cast< SVCameraImageTemplate* >( p_pImageObject );

		if( l_pCameraImage != NULL )
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

	if( p_pValueObject != NULL )
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

	if( p_pImageObject != NULL )
	{
		SVCameraImageTemplate* l_pCameraImage = dynamic_cast< SVCameraImageTemplate* >( p_pImageObject );

		if( l_pCameraImage != NULL )
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

	if( l_hrOk == S_OK )
	{
		long l_lSize = p_rsvEmbeddedList.GetSize();

		for ( long l = 0; l < l_lSize; l++ )
		{
			SVValueObjectClass *l_psvValueObject = dynamic_cast<SVValueObjectClass *>( p_rsvEmbeddedList[ l ] );

			if( l_psvValueObject != NULL )
			{
				m_svValueObjectSet.insert( l_psvValueObject );
				m_mapValueObjects.insert( SVValueObjectMap::value_type( l_psvValueObject->GetCompleteObjectName(), l_psvValueObject ) );
			}
			else
			{
				SVImageClass *l_psvImageObject = dynamic_cast<SVImageClass *>( p_rsvEmbeddedList[ l ] );

				if( l_psvImageObject != NULL )
				{
					SVCameraImageTemplate* l_pCameraImage = dynamic_cast< SVCameraImageTemplate* >( l_psvImageObject );

					if( l_pCameraImage != NULL )
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
		if( *l_oValueIter == NULL || (*l_oValueIter)->GetOwner() == p_psvOwner )
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
		if( *l_oImageIter == NULL || (*l_oImageIter)->GetOwner() == p_psvOwner )
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
		if( *l_oCameraIter == NULL || (*l_oCameraIter)->GetOwner() == p_psvOwner )
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
	SVCameraImageTemplate* l_pImage = NULL;

	SVToolSetClass* l_pToolSet = GetToolSet();

	if( l_pToolSet != NULL )
	{
		l_pImage = dynamic_cast< SVCameraImageTemplate* >( l_pToolSet->getCurrentImage() );
	}

	return l_pImage;
}

HRESULT SVInspectionProcess::GetInspectionImage( const CString& p_strName, SVImageClass*& p_rRefObject )
{
	// Set to Defaults in case of failure
	p_rRefObject = NULL;

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
		rSharedValues.insert(SVSharedValuePair(char_string(ValueIter->first.c_str(), rData.m_Allocator), 
					SVSharedValue(SVSharedValue::StringType, static_cast<const char*>(value), hr, rData.m_Allocator)));
	}

	for (SVFilterElementMap::const_iterator imageIter = rImages.begin(); imageIter != rImages.end(); ++imageIter)
	{
		HRESULT hr = S_OK;
		bool bImgNotFound = true;
		SVImageClass* pImage = nullptr;
		SVObjectClass* pObject = SVObjectManagerClass::Instance().GetObject(imageIter->second.ToGUID());

		if (pObject != nullptr)
		{
			pImage = dynamic_cast<SVImageClass *>(pObject);

			if (pImage != nullptr)
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

// SEJ - this needs to be moved out of here into another more generic class
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





SVStringArray SVInspectionProcess::getPPQVariableNames() const
{
	std::vector<SVString> retVals;

	SVIOEntryHostStructPtrList PPQVariables;
	for( size_t i = 0; i < m_PPQInputs.size(); i++ )
	{	
		SVIOEntryHostStructPtr ioEntryPtr = m_PPQInputs[i].m_IOEntryPtr;

		//check if input is enable for this inspection
		if( ioEntryPtr->m_Enabled )
		{
			PPQVariables.push_back( ioEntryPtr );
		}
	}// end for

	std::sort( PPQVariables.begin(), PPQVariables.end(), &SVIOEntryHostStruct::PtrGreater );

	SVIOEntryHostStructPtrList::iterator Iter( PPQVariables.begin() );
	while( Iter != PPQVariables.end() )
	{
		SVString Name = Iter->get()->m_pValueObject->GetCompleteObjectName();
		retVals.push_back( Name );

		++Iter;
	}

	return retVals;
}

DWORD SVInspectionProcess::GetObjectColor() const
{
	return SV_DEFAULT_WHITE_COLOR;
}


//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInspectionProcess.cpp_v  $
 * 
 *    Rev 1.26   19 Jan 2015 11:41:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Removed code from the method RebuildInspectionInputList that added and removed the SV_VIEWABLE attribute from the allowed list.  Removed the condition around the call to Validate from the method ValidateAndInitialize.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   15 Jan 2015 08:27:50   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   set ObjectAttributAllowed for enabled or disabled digital Inputs.
 * Avoid Assertions when Ranges are invalid
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   07 Jan 2015 17:41:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  980
 * SCR Title:  Add Non-Inspection Objects to the Result View
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Build reference vector when m_ppQInputs are changing
 * Changed method DestroyInspection to call BuildReferenceVector.
 * Changed method RebuildInspectionInputList to call BuildReferenceVector.
 * Added method GetResultList.
 * Replaced code in ResetObject which was previously removed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   19 Dec 2014 04:08:58   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  978
 * SCR Title:  Copy and Paste a Tool within an Inspection or Between Different Inspections
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   ResetObject called twice removed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   17 Dec 2014 07:15:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed Enum name for thread attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   10 Dec 2014 06:44:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  977
 * SCR Title:  Fix Double Processing of Inspections with 2 Inspections on 1 PPQ
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added flag to InspectionInfoStruct to prevent additional queuing in Inspection::StartProcess from the PPQ::StartInspection. New Flag m_HasBeenQueued.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   09 Dec 2014 09:20:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Operator Move
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   01 Dec 2014 13:06:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   SVThreadManager thread attribute and lables.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   02 Sep 2014 13:23:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised ProcessNotifyWithLastInspected method to check for an invalid shared memory slot index.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   20 Aug 2014 12:55:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised ProcessNotifyWithLastInspected to check if the state is set to Running before trying to access the shared memory inspection writer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   14 Aug 2014 18:14:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added FillSharedData method.
 * Revised ThreadProcess to call ProcessNotifyWithLastInspected with the shared memory slot index
 * Revised ProcessNotifyWithLastInspected method to pass the shared memory slot index.
 * Revised ProcessMOnitorList to use SETEXCEPTION5
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   10 Jul 2014 17:10:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised ProcessNotifyWithLastInspected to not be passed a SVProductnNfoStruct referenec, but rather call GetLastPorduct to obtain it to fix an issue with the image display.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   03 Jul 2014 16:49:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised ProcessNotifyWithLastInspection.
 * Revised SVSharedMemoryFilters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   12 Jun 2014 16:19:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  906
 * SCR Title:  SVObserver Tool Grouping
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Removed include for SVToolSetTabView.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   15 May 2014 12:50:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised processMessage to use DWORD_PTR
 * Revised SVSendMessage to use DWORD_PTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   28 Apr 2014 15:38:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised the GoOffline method to clear the MonitorList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   25 Apr 2014 12:30:36   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed type of variable used to store return value from SVSendMessage to LONG_PTR in the processMessage method to avoid warning on x64.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   17 Apr 2014 16:59:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added functionality for the Remote Monitor List and shared memory
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   14 Mar 2014 09:12:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  891
 * SCR Title:  Remove tracking elements that hinder performance in release mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifdef EnableTracking around tracking code so it can be enabled for testing but not in the normal build.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   05 Feb 2014 09:35:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modifications to processMessage to use LONG_PTR  instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   01 Feb 2014 10:48:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   01 Oct 2013 14:31:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   07 Aug 2013 13:29:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 May 2013 12:16:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.197   02 May 2013 11:05:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  810
 * SCR Title:  Remove support for Legacy Tools
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated process message method to fix formatting issues and updated the reset object section to only assert when the HRESULT is a failed status.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 May 2013 16:16:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.196   30 Apr 2013 09:31:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  831
 * SCR Title:  Fix Conditional History crash when tools are deleted
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added call to SVConditionalHistory::DeleteTool in DeleteTool callback.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.195   29 Apr 2013 14:15:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 May 2013 20:06:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.194   16 Apr 2013 14:24:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 14:38:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 009.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.193   10 Apr 2013 14:49:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed unused m_bShowLicenseError flag
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 11:02:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.192   05 Apr 2013 12:30:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection process to change the return types from BOOL to HRESULT for the add input request methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.191   25 Mar 2013 13:15:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the check to see if there were license errors after the RunOnce
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.190   28 Feb 2013 11:48:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed issue with showing the license errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.189   27 Feb 2013 11:22:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added flag to show license errors
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.188   18 Feb 2013 13:21:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection input info structs from using a CString as the data element to a variant.  Updated all methods that call the inspection interface from CString to variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.187   11 Feb 2013 14:13:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code that uses the subject/observer design patten as part of the object manager with the new parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.186   10 Jan 2013 15:04:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.185   11 Oct 2012 09:35:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged changes in to main line of code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.184   04 Oct 2012 10:45:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.183   18 Sep 2012 18:21:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.182   11 Sep 2012 12:15:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update the persist method to fix an issues with the BOOL conversion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.181   06 Sep 2012 09:59:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the Persist method to include missing data for export and Inspection Process object attributes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.180   04 Sep 2012 15:17:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.179.1.1   08 Jan 2013 12:15:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  805
 * SCR Title:  Fix Value Object Indexing Problem when Shortening PPQ
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new virtual method to classes to fix adjusting Object Depth of Value Object and Last Set Index.  Also, updated code with new methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.179.1.0   11 Oct 2012 08:53:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new start process state to the inspection info struct and update the variable when the inspection is queued and whe the product gets reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.179   17 Aug 2012 16:16:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new flag around reset object call to force an extent check and resize if necessary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.178   15 Aug 2012 17:07:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with automatic move and resize logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.177   08 Aug 2012 15:39:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  761
 * SCR Title:  Fix issue with Clip on Adjust LUT Dialog using GigE cameras
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated code to fix issues with LUT and Light Reference Dialogs when camera has no LUT or Light Reference.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.176   03 Aug 2012 10:41:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Rebuild Inspection method to set the default index for the main image objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.175   01 Aug 2012 12:51:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed all overlay collection functionality that uses data at a particular index.  The current overlay collection functionality collects the data in the inspection thread, so that all data is consistant based on the last executed inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.174   30 Jul 2012 12:57:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the parameter change for the visitor fucntionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.173   27 Jul 2012 08:55:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include information and updated overlay collection functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.172   18 Jul 2012 17:06:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem when removing an isnpection from a PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.171   09 Jul 2012 14:03:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.170   02 Jul 2012 16:58:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.169   28 Jun 2012 15:54:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removing Shared Memory for the webserver from the SVObserver application.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.168   25 Jun 2012 16:55:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to monitor delay problems with shared memory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.167   25 Jun 2012 11:05:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed ProcessInputRequests to make the Variant case work with non-array values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.166   20 Jun 2012 14:09:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed display issues with run once logic.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.165   19 Jun 2012 12:52:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Protected Object Information structure attributes.  This change will add accessor methods to get and set attributes.
 * Converted GUID to SVGUID.   This allows the use of standard operators for comparison and methods for conversion.
 * Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.164   15 Mar 2012 08:42:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  757
 * SCR Title:  Fix results view to sort PPQ object selected for viewing
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed SortPPQInputs method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.163   13 Mar 2012 13:35:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  745
 * SCR Title:  Fix contention issue during load process and display
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated SetSelectedToolForOperatorMove method to move IPDoc funcitonality back to IPDoc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.162   12 Mar 2012 09:59:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  757
 * SCR Title:  Fix results view to sort PPQ object selected for viewing
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added SortPPQInputs function.
 * Added call to sortPPQInputs from RebuildInputList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.161   10 Feb 2012 09:29:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated functionality to remove bugs associated with the remote input tool and communications to that tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.160   16 Jan 2012 15:22:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  747
 * SCR Title:  Add New Remote Input Tool to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated inspection process and ip doc to allow the tool to queue a command for the inspection thread to process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.159   12 Jan 2012 18:48:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Revised to log exception if shared memory creation fails due to low disk space
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.158   19 Dec 2011 15:58:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow data to be processed into shared memory and create shared memory structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.157   03 Oct 2011 10:47:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix a unregister problem when changing PPQ connections and update inspection when adding new image to image view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.156   28 Sep 2011 12:56:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.155   21 Sep 2011 09:51:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upated functionality to allow the display to be updated with the last inspected data when the image pane gets changed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.154   16 Sep 2011 16:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.153   14 Jul 2011 16:19:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove un-used product info struct attribute and inspection attribute.  Added logging attribute to product info struct for product state.  Remove extra Inspection Complete notification.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.152   14 Jul 2011 08:59:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.151   14 Jul 2011 08:16:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging in places that cause performance issues and added a registry controlled variable to handle the go offline data manager dumps to files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.150   05 Jul 2011 14:45:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.149   09 Jun 2011 15:14:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed the regression can go online functionality to not run the initialize run method when entering regression mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.148   01 Jun 2011 11:14:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated reset logic to include run once instead of the reset and run once logic separately.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.147   18 May 2011 08:35:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  706
 * SCR Title:  Fix Histogram Drawing Problem in Linear Analyzer Dialogs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the sources code to change the index used for updating the tool parameters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.146   17 May 2011 08:39:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated reserve index logic to clear non-used inspection indexes before saving last inspected product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.145   29 Apr 2011 08:06:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow for inspection to for the display to give up when an inspection get queued.  The display is now regulated to a maximum of 10 frames per second.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.144   19 Apr 2011 16:17:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.143   18 Apr 2011 10:15:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.142   13 Apr 2011 19:15:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to handle new fixed image type and fix color regression issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.141   13 Apr 2011 15:19:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated system to fix color processing issues with color configurations.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.140   11 Apr 2011 18:46:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with logical image to main image connection when going on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.139   08 Apr 2011 13:33:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection functionality to use the update counters flag.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.138   05 Apr 2011 09:07:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue when closing a color configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.137   04 Apr 2011 11:31:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to use new bi-directional map container for fast register and unregister of value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.136   30 Mar 2011 16:24:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix performance problems with the extent changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.135   30 Mar 2011 16:04:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.134   22 Mar 2011 07:54:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated static global classes to singletons to promote proper and defined construction and destruction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133   16 Mar 2011 09:22:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  715
 * SCR Title:  Change the reset functionality for Input Object in the Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to include change in reset requirements for input value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.132   25 Feb 2011 12:10:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated classes needed to consolidate SVImageClass and SVImageObjectClass updating calls to one method name.  Fixed updating problems with child buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.131   10 Feb 2011 14:54:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.130   27 Jan 2011 11:20:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.129   18 Jan 2011 13:55:42   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  710
 * SCR Title:  Fix Condition History to allow for larger amounts of data to be added
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed conditional history timeout.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.128   18 Jan 2011 10:59:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated initialization functionality to include proper connection to the RGB Main Image Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.126   15 Dec 2010 10:00:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.125   08 Dec 2010 12:51:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.124   09 Nov 2010 16:21:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.123   04 Nov 2010 13:39:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.122   02 Nov 2010 10:52:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the assignment functionality for remote inputs in the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.121   28 Oct 2010 13:54:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.120   21 Oct 2010 11:02:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change internal value object to variant value object from double value object.  Added persistance to configuration object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.119   19 Jul 2010 13:23:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with logging NAKed image in Inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.118   01 Jun 2010 10:43:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.117   24 Feb 2010 10:42:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use new store and restore methodolgy for camera image.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.116   19 Jan 2010 15:56:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.115   16 Dec 2009 11:07:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.114   13 Oct 2009 15:44:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated main line code with branch code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.113   24 Sep 2009 09:38:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code with branch changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.112   30 Jul 2009 11:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.111   03 Feb 2009 16:14:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  647
 * SCR Title:  Add Information to Event Log
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   roll back changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.110   28 Jan 2009 11:54:12   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  640
 * SCR Title:  Update Archive Tool to expose Selected List to remote access.
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added handling of array value objects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.109   27 Jan 2009 13:58:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  647
 * SCR Title:  Add Information to Event Log
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Additional Logging to event log.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.108   21 Jan 2009 09:44:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  646
 * SCR Title:  Archive Tool Buffer until Offline Mode Problem
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the Destroy method to call the Destroy method of the SVDisplayObject class instance.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.107   30 Oct 2007 15:34:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   - Added calls to DestroyIndexArray to free memory for Published Images.
 * - Added Jim's logic to prevent race condition when creating Event for InspectionThread.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.106   24 Jul 2007 13:31:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update to MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.105   21 Jun 2007 13:05:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104.1.3   13 Oct 2009 15:03:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem with inspection when the system goes online and offline without a trigger.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104.1.2   22 Sep 2009 18:12:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to handle changes in adding output requests to the inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104.1.1   14 Jul 2009 14:00:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104.1.0   30 Oct 2007 09:59:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added call to Data manager to free memory for Published images.
 * Added Jims changes to prevent race condition when creating event in inspection thread.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104   22 Jan 2007 09:29:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added code for the new Inspection Timeout Threshold for the Extended Time Delay Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.103   27 Jan 2006 16:11:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed CH queue size comparison logic
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.102   26 Jan 2006 12:28:04   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added queue size check for ConditionalHistory requests
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.101   25 Jan 2006 12:37:06   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added BuildConditionalHistoryListAfterLoad
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.100   20 Jan 2006 08:58:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  538
 * SCR Title:  Remove instances when displaying RAID error message on event log read errors
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection process to not allow RAID to be checked if not setup as intel in the hardware.ini and no the right processor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.99   19 Jan 2006 08:54:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added Process Count to Conditional History
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.98   17 Jan 2006 10:01:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented Conditional History
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.97   16 Jan 2006 17:07:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  534
 * SCR Title:  Add Image Select for Auxiliary Extent Source
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved removed embedded object to create object method for load of configuration to happen properly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.96   15 Nov 2005 12:44:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   include Published image buffers in the Reserve / ReleaseResultImage functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.95   11 Nov 2005 14:37:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  525
 * SCR Title:  Fix problem with Shape Mask editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   in ProcessInputRequests changed Enumerate logic to use CompareWithCurrentValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.94   02 Nov 2005 09:07:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented published result images
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.93.1.0   11 Nov 2005 14:31:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  525
 * SCR Title:  Fix problem with Shape Mask editor
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   in ProcessInputRequests changed Enumerate logic to use CompareWithCurrentValue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.93   24 Oct 2005 09:46:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changes to support adding a flag to the Inspection that will enable Auxiliary Extents.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.92   19 Oct 2005 12:11:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include changes that do not require locking and reduce the workload for setting up the main images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.91   07 Oct 2005 16:05:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Modified filename value object in process input requests to allow a file to be set with a size of zero bytes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.90   21 Sep 2005 09:30:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed GoOffline functionality so it waits until the inspection queue is empty and the active inspection is done before returning.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.89   08 Sep 2005 12:27:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new methods to store and restore camera images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.88   01 Sep 2005 12:12:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  510
 * SCR Title:  Add source image extents to all image using tools
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Reset logic for resetting the result list if a tool is reset in the process input requests function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.87   31 Aug 2005 08:39:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update source code call to handle new refresh methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.86   26 Aug 2005 15:01:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added m_bLoadFiles flag to reset structure so ResetObject in Pattern analyzer will reload files when going online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.85   24 Aug 2005 13:57:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problem with last product info struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.84   23 Aug 2005 10:11:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to handle copy forward correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.83   22 Aug 2005 13:42:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated method to fix result image locking problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.82   22 Aug 2005 10:17:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.81   17 Aug 2005 14:21:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   correctly handle double, long, DWORD, BYTE, etc. value object sets in ProcessInputRequest
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.80   17 Aug 2005 11:32:22   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   added more checking in ProcessInputRequests
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.79   11 Aug 2005 13:37:10   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the StartProcess method to manage the queue size of products agains the PPQ length.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.78   10 Aug 2005 13:27:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated GetValueObject to lookup unqualified object names in the inspection 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.77   10 Aug 2005 12:32:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.76   10 Aug 2005 11:27:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed code for a different SCR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.75   08 Aug 2005 16:02:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated objects and dialogs to use the ActiveX Interface to update inspection data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.74   03 Aug 2005 13:10:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  486
 * SCR Title:  Add Trigger Count to Tool Set
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug in reset counts logic and cleaned up code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.73   02 Aug 2005 09:41:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new methods to handle adding requests to the input request queue for the inspection process.
 * 
 * Also added a wait cursor for reset all object calls from the GUI.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.72   01 Aug 2005 10:04:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added reset flag in CanGoOnline. 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.71   27 Jul 2005 16:04:08   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   renamed SVInspectionProcess::FindValueObject to GetValueObject
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   26 Jul 2005 14:12:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed archive tool reset on ProcessInputRequests
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   26 Jul 2005 14:04:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Reset Flag struct to replace flag in IPDoc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   26 Jul 2005 13:38:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVIPDoc SVResetStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   19 Jul 2005 14:49:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new value object Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   15 Jul 2005 11:30:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   updated ProcessInputImageRequest to lookup main image using the camera name and added new methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   08 Jul 2005 13:25:50   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated FindValueObject to make use of new SVValueObjectReference constructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   30 Jun 2005 15:12:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to use SVObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   29 Jun 2005 09:35:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   change return type of FindValueObject to SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   27 Jun 2005 14:54:18   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   use SVValueObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   27 Jun 2005 09:39:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  497
 * SCR Title:  Remove Freeze Set from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed the execution of the freeze set from the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   22 Jun 2005 12:35:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   value objects use SVHRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   22 Jun 2005 09:36:30   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes use accessor functions
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   19 May 2005 07:37:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added include <mbstring.h>
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   18 May 2005 14:57:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  485
 * SCR Title:  Add a way to Reset Counts remotely
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified ProcessInputRequests(  to reset the IPDoc if the requested value object is not a tool.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   18 Feb 2005 12:12:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   changed ConnectToolsetMainImage to take a set of valid cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   18 Feb 2005 11:34:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  262
 * SCR Title:  Improve performance when loading a configuration in SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the methodology to rebuild the value object list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   17 Feb 2005 14:30:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   02 Nov 2004 15:31:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  459
 * SCR Title:  Fix a problem with inspection lock-up
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   StartProcess now returns HRESULT
 * added logging
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   30 Sep 2004 12:24:00   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  427
 * SCR Title:  Fix Bug in SVObserver where Inspection Locks Up after sending SetToolParameter
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   When going online, it will no longer remove the input request 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   12 Jul 2004 10:26:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   updated to handle External Tools and VariantValueObjects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   09 Jun 2004 06:57:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated SVInspectionThreadProc to fix the methodology for copying inputs into the inspection.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   08 Jun 2004 11:26:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated RebuildInputList to fix the issue when the inspection attaches the wrong value object type to an input.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   06 Feb 2004 13:57:04   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fix bug in processing tool parameter and inputs changes from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   04 Feb 2004 14:10:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to fix SetInputs so that it works correctly with the new SetToolParameterList code
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   04 Feb 2004 12:02:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   made change to process all InputRequest segments, not just the first one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   08 Jan 2004 11:10:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made change to the processing to only process one set of changes per inspection iteration. Also added check to disallow the same value to be set twice in one set of changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   17 Dec 2003 09:17:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed ProcessInputRequests to accept True/False or 1/0 for SVBoolValueObjects
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   06 Nov 2003 08:31:24   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  385
 * SCR Title:  Expose additional parameters for SIAC
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made changes for threshold equations to be remotely settable and correct in the config print.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   04 Nov 2003 14:38:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  394
 * SCR Title:  Implement new RankingFilter for WindowTool that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new ranking filter class that has a 1-7x1-7 customizable neighborhood.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   04 Nov 2003 14:11:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Update the custom filter class to be 1-7x1-7 customizable kernel instead of a static 3x3 one.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   13 Oct 2003 11:09:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed bug to convert string parameters to double for setinputlist processing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   24 Sep 2003 11:20:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated SVInspectionThreadProc function to check inspection state for PRODUCT_INSPECTION_NOT_RUN to determine if inspection state should change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   22 Sep 2003 16:46:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made fix to make sure that mirrored inputs are also added to the value object map of the inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   17 Sep 2003 13:02:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   17 Sep 2003 12:28:16   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  387
 * SCR Title:  Change display logic to delay updating while the inspection has a backlog
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added check during the processing of output requests to verify that there is no inspection backlog. if there is, delay the display cycle until the next iteration and retry.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   05 Aug 2003 13:50:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  373
 * SCR Title:  Add a new disable method that doesn't copy forward all result values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the saving/loading/editing of configurations to add a new property for the inspection's disable method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   11 Jul 2003 11:47:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed bug by readding removed code that marks the inspection begin process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   08 Jul 2003 11:21:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  362
 * SCR Title:  Streaming data - not all data being sent
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Made changes to the StreamingData code so that UnregisterStream will work correctly and will remove itself from the InspectionProcesses' OutputRequestQueues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   08 Jul 2003 10:42:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new code the processing of InputRequests (from SetToolParameterList) to verify that extents have changed or the change is ignored.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   08 Jul 2003 10:36:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  378
 * SCR Title:  Implement new CustomFilter for WindowTools that will also be remotely settable
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new code and classes for the new custom filters objects and dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   08 Jul 2003 09:28:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed BOOL to HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   17 Jun 2003 15:18:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added prototype code for using a map in the inspection to lookup objects that are used in the input request queue. It will be finalized after further testing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   02 Jun 2003 15:06:48   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Checked in some preliminary beta code for the new methode of disabling tools and toolsets.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   22 May 2003 16:48:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  361
 * SCR Title:  SVObserver's display may become incorrect when running at very high speeds
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed inspection so that its state is updated even if it fails, otherwise display won't update for not inspected results.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   08 May 2003 14:26:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  340
 * SCR Title:  SetToolParameterList from SIAC doesn't work for thresholds & pass/fail values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed ProcessInputRequests to correctly modify all inputs from the SetInputList part of the RunOnces funcitonality in the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   22 Apr 2003 10:25:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   16 Apr 2003 17:45:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  343
 * SCR Title:  Fix RunOnce functions so later calls to GetDataList and GetImageList work
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Modified SVIPDoc so that the SVInspectionProcess's last run product is erased on a RunOnce operation. Also forced index 1 of the circle buffers to always be reserved for RunOnce.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   16 Apr 2003 17:31:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made modifications to the DDE inputs/outputs. Some objects are now available again and fixed some bugs with existing code. State output and PPQ inputs are again available as DDE outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   16 Apr 2003 17:15:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  340
 * SCR Title:  SetToolParameterList from SIAC doesn't work for thresholds & pass/fail values
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed ProcessInputRequests to correctly modify all remotely settable objects, not just extent based ones (size & position)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   10 Apr 2003 13:24:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  339
 * SCR Title:  Deleting a camera used by an Acquisition Tool causes problems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method RemoveCamera
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   20 Mar 2003 10:34:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed RemoveAllXXXRequests to not return FALSE and leave the queue objects permanently locked.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   19 Mar 2003 13:29:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added methods:
 *   AddInputImageRequest
 *  ProcessInputImageRequest
 * 
 * added queue object - m_qInputImageRequests
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   11 Mar 2003 16:16:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  337
 * SCR Title:  Memory leak when editing a configuration
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added virtual functions for DisconnectToolSetMainImage and ConnectToolSetMainImage
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   06 Mar 2003 10:59:14   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Forced the ResultView to update after removing inputs from the PPQBar. Also changed code to not go online if validating the inspection failed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   05 Mar 2003 12:41:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed extra HRESULT hr definition.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   05 Mar 2003 12:13:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Restructured and recoded the SetToolParameter code. It now uses a standard struct and each SVToolClass based class is responsible for moving itself based on the values passed in through the struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   27 Feb 2003 16:09:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added ReleaseSourceImages method to consoladate this methodology to one place.
 * Updated StartProcess method and SVInspectionThreadFunc  to fix a resource leak in the index management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   14 Feb 2003 10:30:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed error with how the source image index is locked through the Data Manager between the PPQ and the Inspection Process
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   30 Jan 2003 10:57:48   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated StartProcess method, Rebuild method, Create method and SVInspectThreadFunc global function to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   30 Jan 2003 10:45:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   ProcessMessage Updated to handle message from GetChildObjectByName to fix problem with external Inspection name.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   23 Jan 2003 15:46:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   22 Jan 2003 13:42:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bugs with SetInputs and SetToolParameterList from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   16 Jan 2003 18:37:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to support image overlays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   14 Jan 2003 13:56:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed ProcessInputRequest to handle the positioning of analyzers attached to profile tools
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Jan 2003 18:15:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  309
 * SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Remove all references to SVDataManager::DumpAllLocks
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Jan 2003 17:26:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  309
 * SCR Title:  Implement the ability to lock acquired or processed image from the SIAC.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add Get CircleBuffer and Index fuctions for images so they can be locked correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   03 Jan 2003 15:25:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed the method ProcessInputRequest to handle the resizing of tools
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jan 2003 11:18:52   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Methods were added to perform operations requested by the ActiveX control
 * RunOnce
 * processMessage
 * processInputRequests
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   17 Dec 2002 15:19:46   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   long GetResultDataIndex(SVProductInfoStruct* pProduct);
 * long GetResultImageIndex(SVProductInfoStruct* pProduct);	// returns -1
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 19:29:40   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated to fix problems and track issues with result data indexes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 12:55:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226, 272
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/