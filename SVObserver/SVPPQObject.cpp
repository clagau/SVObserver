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
//Moved to precompiled header: #include <mmsystem.h>
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <limits>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVPPQObject.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVDottedName.h"
#include "SVSharedMemoryLibrary/SVSharedPPQWriter.h"
#include "SVSharedMemoryLibrary/SVSharedConfiguration.h"
#include "SVSharedMemorySingleton.h"
#include "SVGlobal.h"
#include "SVObserver.h"
#include "SVOutputObjectList.h"
#include "SVInputObjectList.h"
#include "SVInfoStructs.h"
#include "SVMessage/SVMessage.h"
#include "SVDigitalOutputObject.h"
#include "TriggerHandling/SVTriggerObject.h"
#include "SVAcquisitionClass.h"
#include "TriggerHandling/SVTriggerClass.h"
#include "SVConfigurationObject.h"
#include "SVRemoteInputObject.h"
#include "SVPPQConstants.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVStatusLibrary\MessageManager.h"
#include "TextDefinesSvO.h"
#include "SVRunControlLibrary\SVRunControlLibrary.h"
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
#include "SVTimerLibrary\SVProfiler.h"
#endif
#include "SVStatusLibrary\MessageManagerResource.h"
#include "SVVisionProcessorHelper.h"
#include "SVToolSet.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const long g_lPPQExtraBufferSize = 50;

#pragma endregion Declarations

HRESULT CALLBACK SVFinishTriggerCallback( void *pOwner, void *pCaller, void *pTriggerInfo )
{
	SVPPQObject* pPPQ = reinterpret_cast< SVPPQObject* >( pOwner );
	SvTh::SVTriggerInfoStruct* pInfo = reinterpret_cast< SvTh::SVTriggerInfoStruct* >( pTriggerInfo );

	BOOL bRet = ( nullptr != pPPQ && nullptr != pInfo );

	if( bRet )
	{
		bRet = pPPQ->FinishTrigger( pCaller, *pInfo );
	}

	if( bRet )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT CALLBACK SVFinishCameraCallback( void *pOwner, void *pCaller, void *pResponse)
{
	BOOL bRet(false);

	SVPPQObject* pPPQ = reinterpret_cast< SVPPQObject* > (pOwner);

	if( nullptr != pPPQ ){ bRet = pPPQ->FinishCamera( pCaller, reinterpret_cast< SVODataResponseClass* >( pResponse ) ); }

	if( bRet )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information

void logWorkloadInformation(const ProductWorkloadInformation &pwi, LPCSTR heading)
{
	CString infostring;
	infostring.Format(_T("!\t.%7.1lf: %s:\n"), SvTl::GetRelTimeStamp(), heading);
	::OutputDebugString(infostring);
	infostring.Format(_T("!\t\ttt = %7.1lf, pst = %7.1lf, ct = %7.1lf, "), 
		pwi.m_TriggerTime - SvTl::getReferenceTime(),
		pwi.m_ProcessingStartTime - SvTl::getReferenceTime(), 
		pwi.m_CompletionTime - SvTl::getReferenceTime());
	::OutputDebugString(infostring);
	infostring.Format(_T("TtoCo = %7.1lf, TtoSt = %7.1lf\n"),
		pwi.TriggerToCompletionInMilliseconds(), 
		
		pwi.TriggerToStartInMilliseconds());
	::OutputDebugString(infostring);
	infostring.Format(_T("!\trd\ttt = %.0lf, pst = %.0lf, ct = %.0lf, "), 
		pwi.m_TriggerTime,
		pwi.m_ProcessingStartTime, 
		pwi.m_CompletionTime);
	::OutputDebugString(infostring);
	infostring.Format(_T("pst-tt = %7.1lf, ct-tt = %7.1lf, ct-pst= %7.1lf\n"), 
		pwi.m_ProcessingStartTime- pwi.m_TriggerTime,
		pwi.m_CompletionTime- pwi.m_TriggerTime,
		pwi.m_CompletionTime- pwi.m_ProcessingStartTime);
	::OutputDebugString(infostring);
}

#endif


HRESULT SVPPQObject::ProcessDelayOutputs( bool& p_rProcessed )
	/// Used in data completion mode, but not in next trigger mode.
	/// Retrieves the process (i.e. trigger) count from the m_oOutputsDelayQueue and uses it to get a "product pointer"
	/// from m_ppPPQPositions via GetProductInfoStruct()
	/// If the delay time is already over, calls ProcessTimeDelay/AndDataComplete/Outputs().
	/// from where WriteOutputs() will be called.
	/// Otherwise, puts the trigger count back into the outputs delay queue, 
{
	HRESULT l_Status = S_OK;

	SVProductInfoStruct* l_pProduct = nullptr;

	SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

	while( 0 < m_oOutputsDelayQueue.GetCount() && nullptr == l_pProduct )
	{
		long l_ProcessCount = 0;

		if( m_oOutputsDelayQueue.RemoveHead( &l_ProcessCount ) )
		{
			GetProductInfoStruct( l_pProduct, l_ProcessCount );
		}
		else
		{
			l_Status = E_FAIL;

			break;
		}

		if( nullptr != l_pProduct )
		{
			if( 0 < m_oOutputsDelayQueue.GetCount() )
			{
				SVProductInfoStruct* l_pNextProduct = nullptr;

				if( m_oOutputsDelayQueue.GetHead( &l_ProcessCount ) )
				{
					GetProductInfoStruct( l_pNextProduct, l_ProcessCount );
				}

				if( nullptr != l_pNextProduct && l_pNextProduct->oOutputsInfo.m_EndOutputDelay <= l_CurrentTime )
				{
					l_pProduct = nullptr;
				}
			}
		}

		if( nullptr != l_pProduct )
		{
			// if time delay has not expired yet, then put back on the 
			// m_oOutputsDelayQueue.  Then why did we remove it to begin with?
			if( l_CurrentTime < l_pProduct->oOutputsInfo.m_EndOutputDelay )
			{
				m_NextOutputDelayTimestamp = l_pProduct->oOutputsInfo.m_EndOutputDelay;

				m_oOutputsDelayQueue.AddHead( l_pProduct->ProcessCount() );

				l_pProduct = nullptr;

				break;
			}
		}
	}

	p_rProcessed = ( nullptr != l_pProduct );

	if( p_rProcessed )
	{
		SVPPQOutputModeEnum oOutputMode = SVPPQUnknownMode;

		if( GetPPQOutputMode( oOutputMode ) )
		{
			switch( oOutputMode )
			{
			case SVPPQTimeDelayMode:
			case SVPPQExtendedTimeDelayMode:
				{
					l_Status = ProcessTimeDelayOutputs( *l_pProduct );

					break;
				}
			case SVPPQTimeDelayAndDataCompleteMode:
			case SVPPQExtendedTimeDelayAndDataCompleteMode:
				{
					l_Status = ProcessTimeDelayAndDataCompleteOutputs( *l_pProduct, p_rProcessed );

					break;
				}
			default:
				{
					l_Status = E_FAIL;

					break;
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	::InterlockedExchange( &( m_ProcessingOutputDelay ), 0 );

	return l_Status;
}

HRESULT SVPPQObject::ProcessTimeDelayOutputs( SVProductInfoStruct& p_rProduct )
{
	HRESULT l_Status = S_OK;

	if( WriteOutputs( &p_rProduct ) )
	{
		long lResetDelay = 0;

		if( GetResetDelay( lResetDelay ) )
		{
			if( lResetDelay )
			{
				p_rProduct.oOutputsInfo.m_EndProcess = SVClock::GetTimeStamp();

				p_rProduct.oOutputsInfo.m_EndResetDelay = p_rProduct.oOutputsInfo.m_EndProcess + 
					p_rProduct.oOutputsInfo.lResetDelay;

				m_oOutputsResetQueue.AddTail( p_rProduct.ProcessCount() );

				SetProductComplete( p_rProduct );
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

HRESULT SVPPQObject::ProcessTimeDelayAndDataCompleteOutputs( SVProductInfoStruct& p_rProduct, bool& p_rProcessed  )
{
	HRESULT l_Status = S_OK;

	if( p_rProduct.bDataComplete )
	{
		l_Status = ProcessTimeDelayOutputs( p_rProduct );
	}
	else
	{
		m_NextOutputDelayTimestamp = p_rProduct.oOutputsInfo.m_EndOutputDelay;

		m_oOutputsDelayQueue.AddHead( p_rProduct.ProcessCount() );

		p_rProcessed = false;
	}

	return l_Status;
}

#pragma region Constructor
SVPPQObject::SVPPQObject( LPCSTR ObjectName )
	: SVObjectClass( ObjectName )
	, m_oTriggerQueue( 10 )
	, m_NAKCount( 0 )
	, m_conditionalOutputName( PPQ_CONDITIONAL_OUTPUT_ALWAYS )
	, m_bActiveMonitorList(false)
	, m_AttributesAllowedFilterForFillChildObjectList( 0 )
{
	init();
}

SVPPQObject::SVPPQObject( SVObjectClass* POwner, int StringResourceID )
	: SVObjectClass( POwner, StringResourceID )
	, m_oTriggerQueue( 10 )
	, m_NAKCount( 0 )
	, m_conditionalOutputName ( PPQ_CONDITIONAL_OUTPUT_ALWAYS ) 
	, m_bActiveMonitorList(false)
	, m_AttributesAllowedFilterForFillChildObjectList( 0 )
{
	init();
}
#pragma endregion Constructor

SVPPQObject::~SVPPQObject()
{
	m_childObjectsForFillChildObjectList.clear();

	SVObjectManagerClass::Instance().ClearShortPPQIndicator();

	// Stop the multimedia timer thread for the output and reset time delays
	if( 0 != m_uOutputTimer )
	{
		::timeKillEvent( m_uOutputTimer );

		::timeEndPeriod( 1 );

		m_uOutputTimer = 0;
	}

	if( m_isCreated )
	{
		Destroy();
	}

	m_pTriggerToggle.clear();
	m_pOutputToggle.clear();
	m_pDataValid.clear();
}

void SVPPQObject::init()
{
	m_oOutputMode                = SVPPQNextTriggerMode;
	m_isCreated                  = false;
	m_bOnline                    = false;
	m_bMaintainSourceImages      = false;
	m_lInspectionTimeoutMillisec = 0;
	m_lOutputDelay               = 100;
	m_lResetDelay                = 0;
	m_DataValidDelay			 = TheSVObserverApp.getDataValidDelay();

	m_ProcessingOutputDelay = 0;
	m_NextOutputDelayTimestamp = 0;
	m_ProcessingOutputReset = 0;
	m_NextOutputResetTimestamp = 0;

	m_uOutputTimer = 0;

	m_pTrigger				= nullptr;
	m_pMasterProductInfos	= nullptr;
	m_pInputList			= nullptr;
	m_pOutputList			= nullptr;

	m_voTriggerToggle.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voOutputToggle.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voACK.ObjectAttributesAllowedRef()			= SV_EMBEDABLE | SV_PRINTABLE;
	m_voNAK.ObjectAttributesAllowedRef()			= SV_EMBEDABLE | SV_PRINTABLE;
	m_voMasterFault.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voMasterWarning.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voNotInspected.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voDataValid.ObjectAttributesAllowedRef()		= SV_EMBEDABLE | SV_PRINTABLE;
	m_voOutputState.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;

	BasicValueObjectPtr pPpqLength =  m_PpqValues.setValueObject( PpqLength, StandardPpqLength, this );
	SVObjectManagerClass::Instance().IncrementShortPPQIndicator();

	//fill up the child object list
	m_childObjects.push_back(&m_voTriggerToggle);
	m_childObjects.push_back(&m_voOutputToggle);
	m_childObjects.push_back(&m_voACK);
	m_childObjects.push_back(&m_voNAK);
	m_childObjects.push_back(&m_voMasterFault);
	m_childObjects.push_back(&m_voMasterWarning);
	m_childObjects.push_back(&m_voNotInspected);
	m_childObjects.push_back(&m_voDataValid);
	m_childObjects.push_back(&m_voOutputState);
	m_childObjects.push_back(&m_voTriggerCount);
	m_childObjects.push_back(pPpqLength.get());

	//copy full child object list to the temporary list for the method fillChildObjectList,
	//because this is the same as filter = 0 (default)
	m_childObjectsForFillChildObjectList = m_childObjects;
}

HRESULT SVPPQObject::GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index ) const
{
	HRESULT l_Status = SVObjectClass::GetChildObject( rpObject, rNameInfo, Index );

	if( S_OK != l_Status  )
	{
		if( static_cast<const size_t> (Index) < rNameInfo.m_NameArray.size() && rNameInfo.m_NameArray[Index] == GetName() )
		{
			if( static_cast<const size_t> ( Index + 1 ) == ( rNameInfo.m_NameArray.size() - 1 ) )
			{
				SVObjectPtrDeque::const_iterator iter = m_childObjects.cbegin();
				while (iter != m_childObjects.cend() && nullptr == rpObject )
				{
					const SVObjectClass* pObject = *iter;
					l_Status = pObject->GetChildObject( rpObject, rNameInfo, Index + 1 );
					++iter;
				}
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ObserverUpdate( const SVInspectionCompleteInfoStruct& p_rData )
{
	HRESULT l_Status = S_OK;

	SVGUIDSVInspectionInfoStructMap::const_iterator l_Iter = p_rData.m_ProductInfo.m_svInspectionInfos.find( p_rData.m_InspectionID );

	if( l_Iter != p_rData.m_ProductInfo.m_svInspectionInfos.end() )
	{
		SVInspectionInfoStruct l_InspectInfo = l_Iter->second;

		l_InspectInfo.m_CallbackReceived = SVClock::GetTimeStamp();

		l_InspectInfo.ClearIndexes();

		SVInspectionInfoPair l_Info( p_rData.m_ProductInfo.ProcessCount(), l_InspectInfo );

		m_oInspectionQueue.AddTail( l_Info );

		m_AsyncProcedure.Signal( nullptr );
	}

	return l_Status;
}

BOOL SVPPQObject::Create()
{
	BSTR bstrName;
	CString strName;
	HRESULT hr;
	int i;
	int iSize;

	// Return if already created
	if( m_isCreated ) { return false; }

	strName = _T( "PPQ Result Data" );
	bstrName = strName.AllocSysString();

	hr = TheSVDataManager.CreateManagedIndexArray( m_pResultDataCircleBuffer, bstrName, GetPPQLength() + g_lPPQExtraBufferSize );
	::SysFreeString( bstrName );
	if( S_OK != hr ) { return FALSE; }

	strName = _T( "PPQ Result Image Published" );
	bstrName = strName.AllocSysString();

	hr = TheSVDataManager.CreateManagedIndexArray( m_pResultImagePublishedCircleBuffer, bstrName, TheSVObserverApp.GetSourceImageDepth() );
	::SysFreeString( bstrName );
	if( S_OK != hr ) { return FALSE; }

	for( i = 0; i < static_cast< long >( m_ppPPQPositions.size() ); ++i )
	{
		RecycleProductInfo( m_ppPPQPositions.GetProductAt( i ) );

		m_ppPPQPositions.SetProductAt( i, nullptr );
	}

	// Create buckets for the PPQ positions
	m_ppPPQPositions.resize( GetPPQLength() );

	// Create a set of ProductInfoStructs to use
	if( !m_qAvailableProductInfos.Create() ) { return FALSE; }

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct l_CameraInfos;

	BuildCameraInfos( l_CameraInfos );

	// Set up all the ProductInfo Structs
	m_pMasterProductInfos = new SVProductInfoStruct[GetPPQLength() + g_lPPQExtraBufferSize];
	for( int j = 0; j < GetPPQLength() + g_lPPQExtraBufferSize; j++ )
	{
		m_pMasterProductInfos[j].oPPQInfo.pPPQ = this;
		m_pMasterProductInfos[j].oTriggerInfo.pTrigger = m_pTrigger;
		m_pMasterProductInfos[j].m_svCameraInfos = l_CameraInfos;
		m_pMasterProductInfos[j].m_svInspectionInfos.clear();

		iSize = m_arInspections.GetSize();
		for( i = 0; i < iSize; i++ )
		{
			SVInspectionInfoStruct l_svInspectionStruct;

			l_svInspectionStruct.pInspection = m_arInspections[i];

			m_pMasterProductInfos[j].m_svInspectionInfos[ m_arInspections[i]->GetUniqueObjectID() ] = l_svInspectionStruct;
		}// end for

		m_qAvailableProductInfos.AddTail( &m_pMasterProductInfos[j] );
	}// end for

	SVProductInfoStruct* l_pProduct = nullptr;

	if( m_qAvailableProductInfos.RemoveHead( &l_pProduct ) && nullptr != l_pProduct )
	{
		if( S_OK == m_ppPPQPositions.SetProductAt( 0, l_pProduct ) )
		{
			l_pProduct->InitProductInfo();
			l_pProduct->GetNextAvailableIndexes( SV_PPQ );

			for( i = 0; i < m_arInspections.GetSize(); i++ )
			{
				m_arInspections[ i ]->LastProductUpdate( l_pProduct );
			}
		}
		else
		{
			m_qAvailableProductInfos.AddTail( l_pProduct );
		}
	}

	m_isCreated = true;

	// Create the Queues that run the threads
	m_isCreated &= m_oCamerasQueue.Create();
	m_isCreated &= m_CameraResponseQueue.Create();
	m_isCreated &= m_oInspectionQueue.Create();
	m_isCreated &= m_oOutputsDelayQueue.Create();
	m_isCreated &= m_oOutputsResetQueue.Create();
	m_isCreated &= m_DataValidDelayQueue.Create();
	m_isCreated &= m_ProductRequests.Create();

	m_oNotifyInspectionsSet.clear();
	m_ProcessInspectionsSet.clear();

	if( m_isCreated )
	{
		// Force the Inspections to rebuild
		iSize = m_arInspections.GetSize();
		for( i = 0; i < iSize; i++ )
		{
			m_arInspections[i]->RebuildInspection();
		}// end for
	}

	m_voTriggerToggle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;	
	m_voOutputToggle.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voACK.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voNAK.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMasterFault.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voMasterWarning.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voNotInspected.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voDataValid.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;
	m_voOutputState.ObjectAttributesAllowedRef() &= ~SV_PRINTABLE;


	m_TriggerToggle = false;
	m_OutputToggle = false;

	return m_isCreated;
}

BOOL SVPPQObject::Rebuild()
{
	int j;
	BSTR bstrName;
	CString strName;
	HRESULT hr;
	int i;
	int iSize;

	// Return if not created
	if( !m_isCreated ) { return false; }

	// Delete buckets for the PPQ positions
	m_ppPPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete [] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	// Create new data manager buffers of new size
	strName = _T( "PPQ Result Data" );
	bstrName = strName.AllocSysString();

	hr = TheSVDataManager.CreateManagedIndexArray( m_pResultDataCircleBuffer, bstrName, GetPPQLength() + g_lPPQExtraBufferSize );
	::SysFreeString( bstrName );
	if( S_OK != hr ) { return FALSE; }

	strName = _T( "PPQ Result Image Published" );
	bstrName = strName.AllocSysString();

	hr = TheSVDataManager.CreateManagedIndexArray( m_pResultImagePublishedCircleBuffer, bstrName, TheSVObserverApp.GetSourceImageDepth() );
	::SysFreeString( bstrName );
	if( S_OK != hr ) { return FALSE; }

	for( i = 0; i < static_cast< long >( m_ppPPQPositions.size() ); ++i )
	{
		RecycleProductInfo( m_ppPPQPositions.GetProductAt( i ) );

		m_ppPPQPositions.SetProductAt( i, nullptr );
	}

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct l_CameraInfos;

	BuildCameraInfos( l_CameraInfos );

	// Create buckets for the PPQ positions
	m_ppPPQPositions.resize( GetPPQLength() );

	// Set up all the ProductInfo Structs
	m_pMasterProductInfos = new SVProductInfoStruct[GetPPQLength() + g_lPPQExtraBufferSize];
	for( j = 0; j < GetPPQLength() + g_lPPQExtraBufferSize; j++ )
	{
		m_pMasterProductInfos[j].oPPQInfo.pPPQ = this;
		m_pMasterProductInfos[j].oTriggerInfo.pTrigger	= m_pTrigger;
		m_pMasterProductInfos[j].m_svCameraInfos = l_CameraInfos;
		m_pMasterProductInfos[j].m_svInspectionInfos.clear();

		iSize = m_arInspections.GetSize();
		for( i = 0; i < iSize; i++ )
		{
			SVInspectionInfoStruct l_svInspectionStruct;

			l_svInspectionStruct.pInspection = m_arInspections[i];

			m_pMasterProductInfos[j].m_svInspectionInfos[ m_arInspections[i]->GetUniqueObjectID() ] = l_svInspectionStruct;
		}// end for

		m_qAvailableProductInfos.AddTail( &m_pMasterProductInfos[j] );
	}// end for

	SVProductInfoStruct* l_pProduct = nullptr;

	if( m_qAvailableProductInfos.RemoveHead( &l_pProduct ) && nullptr != l_pProduct )
	{
		if( S_OK == m_ppPPQPositions.SetProductAt( 0, l_pProduct ) )
		{
			l_pProduct->InitProductInfo();
			l_pProduct->GetNextAvailableIndexes( SV_PPQ );

			for( i = 0; i < m_arInspections.GetSize(); i++ )
			{
				m_arInspections[ i ]->LastProductUpdate( l_pProduct );
			}
		}
		else
		{
			m_qAvailableProductInfos.AddTail( l_pProduct );
		}
	}

	// Force the Inspections to rebuild as well
	iSize = m_arInspections.GetSize();
	for( i = 0; i < iSize; i++ )
	{
		m_arInspections[i]->RebuildInspection();
	}// end for

	return TRUE;
}// end Rebuild

BOOL SVPPQObject::Destroy()
{
	// Return if not created
	if( !m_isCreated ) { return false; }

	// Stop the multimedia timer thread for the output and reset time delays
	if( 0 != m_uOutputTimer )
	{
		::timeKillEvent( m_uOutputTimer );

		::timeEndPeriod( 1 );

		m_uOutputTimer = 0;
	}

	// Destroy the Queues that run the threads
	bool l_Processed = true;

	m_oTriggerQueue.clear();

	while( l_Processed )
	{
		ProcessProductRequests( l_Processed );
	}

	m_oNotifyInspectionsSet.clear();
	m_ProcessInspectionsSet.clear();

	if( !( m_PendingCameraResponses.empty() ) )
	{
		SVObjectManagerClass::Instance().AdjustPendingImageIndicator( -( static_cast< long >( m_PendingCameraResponses.size() ) ) );

		m_PendingCameraResponses.clear();
	}

	m_ProductRequests.Destroy();
	m_DataValidDelayQueue.Destroy();
	m_oOutputsDelayQueue.Destroy();
	m_oOutputsResetQueue.Destroy();
	m_oInspectionQueue.Destroy();
	m_CameraResponseQueue.Destroy();
	m_oCamerasQueue.Destroy();

	// Delete buckets for the PPQ positions
	m_ppPPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete [] m_pMasterProductInfos;

	m_pMasterProductInfos = nullptr;

	m_qAvailableProductInfos.Destroy();

	m_AllInputs.clear();
	m_UsedInputs.clear();
	m_AllOutputs.clear();
	m_UsedOutputs.clear();

	// Destroy the managed index for the input circle buffer
	m_pResultDataCircleBuffer.clear();
	m_pResultImagePublishedCircleBuffer.clear();

	if (SVSharedMemorySingleton::HasShares())
	{
		SVSharedMemorySingleton::Instance().ErasePPQSharedMemory(GetUniqueObjectID());
	}

	m_isCreated = false;
	return true;
}// end Destroy

BOOL SVPPQObject::DetachAll()
{
	DetachTrigger(m_pTrigger);

	std::deque< SVVirtualCamera* > l_Cameras;

	GetCameraList( l_Cameras );

	std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

	while( l_Iter != l_Cameras.end() )
	{
		SVVirtualCamera* pCamera = ( *l_Iter );

		DetachCamera( pCamera );

		++l_Iter;
	}

	int iSize = m_arInspections.GetSize();
	for(int i = iSize - 1; i >= 0; i--)
	{
		DetachInspection(m_arInspections.GetAt(i));
	}

	return TRUE;
}

BOOL SVPPQObject::SetPPQOutputMode( SVPPQOutputModeEnum oPPQOutputMode )
{
	m_oOutputMode = oPPQOutputMode;
	return TRUE;
}

BOOL SVPPQObject::SetOutputDelay( long lDelayTime )
{
	m_lOutputDelay = lDelayTime;
	return TRUE;
}

BOOL SVPPQObject::SetResetDelay( long lResetTime )
{
	m_lResetDelay = lResetTime;
	return TRUE;
}

BOOL SVPPQObject::SetPPQLength( long lPPQLength )
{
	long oldPPQLength = GetPPQLength();
	if( oldPPQLength != lPPQLength )
	{
		if( 2 < oldPPQLength && lPPQLength < 3 )
		{
			SVObjectManagerClass::Instance().IncrementShortPPQIndicator();
		}
		else if( 2 < lPPQLength && oldPPQLength < 3 )
		{
			SVObjectManagerClass::Instance().DecrementShortPPQIndicator();
		}
	}

	m_PpqValues.setValueObject( PpqLength, lPPQLength );

	if( GetPPQLength() != m_ppPPQPositions.size() )
	{
		Rebuild();
	}

	return TRUE;
}

BOOL SVPPQObject::SetMaintainSourceImages( bool bMaintainImages )
{
	m_bMaintainSourceImages = bMaintainImages;
	return TRUE;
}

BOOL SVPPQObject::SetInspectionTimeout( long lTimeoutMillisec )
{
	m_lInspectionTimeoutMillisec = lTimeoutMillisec;
	return TRUE;
}

BOOL SVPPQObject::GetPPQOutputMode( SVPPQOutputModeEnum &oPPQOutputMode ) const
{
	oPPQOutputMode = m_oOutputMode;
	return TRUE;
}

BOOL SVPPQObject::GetOutputDelay( long &lDelayTime ) const
{
	lDelayTime = m_lOutputDelay;
	return TRUE;
}

BOOL SVPPQObject::GetResetDelay( long &lResetTime ) const
{
	lResetTime = m_lResetDelay;
	return TRUE;
}

BOOL SVPPQObject::GetPPQLength( long &lPPQLength ) const
{
	lPPQLength = GetPPQLength();
	return TRUE;
}

long SVPPQObject::GetPPQLength() const
{
	long length = 0;
	BasicValueObjectPtr pValue  = m_PpqValues.getValueObject(PpqLength);
	if( !pValue.empty() )
	{
		pValue->getValue(length);
	}
	return length;
}

BOOL SVPPQObject::GetMaintainSourceImages( bool& bMaintainImages ) const
{
	bMaintainImages = m_bMaintainSourceImages;
	return TRUE;
}

BOOL SVPPQObject::GetInspectionTimeout( long& rlTimeoutMillisec ) const
{
	rlTimeoutMillisec = m_lInspectionTimeoutMillisec;
	return TRUE;
}

BOOL SVPPQObject::AttachTrigger( SvTh::SVTriggerObject* pTrigger )
{
	if( nullptr == pTrigger ) { return FALSE; }

	m_pTrigger = pTrigger;
	if( !m_pTrigger->RegisterFinishProcess( this, SVFinishTriggerCallback ) ) { return FALSE; }

	return TRUE;
}

BOOL SVPPQObject::AttachCamera( SVVirtualCamera* pCamera, long lPosition, bool p_AllowMinusOne )
{
	BOOL l_bOk = TRUE;

	if( nullptr == pCamera ) { return FALSE; }

	if( !p_AllowMinusOne && lPosition < 0 ) { lPosition = 0; }

	SVCameraInfoMap::iterator l_Iter = m_Cameras.find( pCamera );

	if( l_Iter != m_Cameras.end() )
	{
		l_Iter->second.m_CameraPPQIndex = lPosition;
	}
	else
	{
		SVString l_Name = SvUl_SF::Format( _T( "%s Toggle" ), pCamera->GetName() );

		m_Cameras[ pCamera ].m_CameraPPQIndex = lPosition;
		m_Cameras[ pCamera ].m_ToggleState = true;

		l_bOk = RebuildProductInfoStructs();

		l_bOk &= pCamera->RegisterFinishProcess( this, SVFinishCameraCallback );
	}

	return l_bOk;
}

BOOL SVPPQObject::AttachInspection( SVInspectionProcess* pInspection )
{
	if( nullptr == pInspection ) { return FALSE; }

	m_arInspections.Add( pInspection );

	pInspection->SetPPQIdentifier( GetUniqueObjectID() );

	return TRUE;
}

BOOL SVPPQObject::DetachTrigger( SvTh::SVTriggerObject* pTrigger )
{
	BOOL bOk = nullptr != m_pTrigger && m_pTrigger == pTrigger;

	if ( bOk )
	{	
		bOk = m_pTrigger->UnregisterFinishProcess( this );

		m_pTrigger = nullptr;
	}

	return bOk;
}// end DetachTrigger

BOOL SVPPQObject::DetachCamera( SVVirtualCamera* pCamera, BOOL bRemoveDepends/*=FALSE*/)
{
	if( nullptr == pCamera ) { return FALSE; }

	BOOL l_Status = true;

	l_Status &= pCamera->UnregisterFinishProcess( this, SVFinishCameraCallback );

	SVCameraInfoMap::iterator l_svIter = m_Cameras.find( pCamera );

	if( l_svIter != m_Cameras.end() )
	{
		m_Cameras.erase( l_svIter );
	}

	//check inspections and remove camera from them also...
	if ( bRemoveDepends )
	{
		SVInspectionProcess* pInspection( nullptr );
		int iCnt;
		int iInsSize = m_arInspections.GetSize();

		for ( iCnt = 0; iCnt < iInsSize; iCnt++ )
		{
			pInspection = m_arInspections.GetAt(iCnt);
			if( nullptr != pInspection ){ pInspection->RemoveCamera(pCamera->GetName()); }
		}
	}

	RebuildProductInfoStructs();

	return l_Status;
}// end DetachCamera

BOOL SVPPQObject::DetachInspection( SVInspectionProcess* pInspection )
{
	if( nullptr == pInspection ) { return FALSE; }

	int i;
	int iSize;
	BOOL bFound;

	bFound = FALSE;
	iSize = m_arInspections.GetSize();
	for( i = 0; i < iSize; i++ )
	{
		if( pInspection == m_arInspections[i] )
		{
			bFound = TRUE;
			break;
		}// end if
	}// end for

	if( !bFound ) { return FALSE; }

	pInspection->SetPPQIdentifier( SV_GUID_NULL );

	SVProductInfoStruct l_svProduct;

	m_arInspections[i]->LastProductUpdate( &l_svProduct );

	m_arInspections.RemoveAt( i );

	return TRUE;
}// end DetachInspection

BOOL SVPPQObject::AddSharedCamera( SVVirtualCamera* pCamera )
{
	BOOL l_Status = ( nullptr != pCamera );

	if( l_Status )
	{
		SVCameraInfoMap::iterator l_svIter = m_Cameras.find( pCamera );

		l_Status = ( l_svIter != m_Cameras.end() );

		if( l_Status && l_svIter->second.m_CameraPPQIndex == -1 )
		{
			l_svIter->second.m_CameraPPQIndex = 0;
		}
	}

	if( !l_Status )
	{
		l_Status = AttachCamera( pCamera, 0 );

		Rebuild();
	}

	return l_Status;
}

BOOL SVPPQObject::GetInspectionCount( long &lSize ) const
{
	lSize = m_arInspections.GetSize();

	return TRUE;
}// end GetInspectionCount	

size_t SVPPQObject::GetCameraCount() const
{
	return m_Cameras.size();
}

HRESULT SVPPQObject::GetCameraList( std::deque< SVVirtualCamera* >& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVCameraInfoMap::const_iterator l_svIter;

	l_svIter = m_Cameras.begin(); 

	while( l_svIter != m_Cameras.end() )
	{
		p_rCameras.push_back( l_svIter->first );

		++l_svIter;
	}

	return l_Status;
}

HRESULT SVPPQObject::GetVirtualCameras( SVVirtualCameraMap& p_rCameras ) const
{
	HRESULT l_Status = S_OK;

	p_rCameras.clear();

	SVCameraInfoMap::const_iterator l_svIter = m_Cameras.begin(); 

	while( l_svIter != m_Cameras.end() )
	{
		if( nullptr != l_svIter->first )
		{
			p_rCameras[ l_svIter->first->GetName() ] = l_svIter->first;
		}

		++l_svIter;
	}

	return l_Status;
}

BOOL SVPPQObject::GetTrigger( SvTh::SVTriggerObject*& ppTrigger )
{
	ppTrigger = m_pTrigger;

	return TRUE;
}// end GetTrigger

BOOL SVPPQObject::GetInspection( long lIndex, SVInspectionProcess*& ppInspection ) const
{
	if( lIndex < 0 || lIndex >= m_arInspections.GetSize() ) { return FALSE; }

	ppInspection = m_arInspections.GetAt( lIndex );

	return TRUE;
}// end GetInspection

BOOL SVPPQObject::SetCameraPPQPosition( long lPosition, SVVirtualCamera* pCamera )
{
	BOOL l_Status = true;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pCamera );

	if( m_isCreated && -1 <= lPosition && lPosition < static_cast< long >( m_ppPPQPositions.size() ) - 1 )
	{
		l_Status = AttachCamera( pCamera, lPosition, true );
	}
	else
	{
		l_Status = false;
	}

	return l_Status;
}// end SetCameraPPQPosition

BOOL SVPPQObject::GetCameraPPQPosition( long &lPosition, SVVirtualCamera* pCamera ) const
{
	BOOL bFound;

	//Only do an assert check so that in release mode no check is made
	ASSERT( nullptr != pCamera );

	lPosition = -1;

	if( !m_isCreated )
	{
		return false;
	}// end if

	// Try to find to the Camera they sent it
	SVCameraInfoMap::const_iterator l_svIter = m_Cameras.find( pCamera );

	bFound = l_svIter != m_Cameras.end();

	if( !bFound ) { return FALSE; }

	lPosition = l_svIter->second.m_CameraPPQIndex;

	return TRUE;
}// end GetCameraPPQPosition

BOOL SVPPQObject::RebuildProductInfoStructs()
{
	BOOL l_bOk = TRUE;

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct l_CameraInfos;

	BuildCameraInfos( l_CameraInfos );

	for( int j = 0; nullptr != m_pMasterProductInfos && j < GetPPQLength() + g_lPPQExtraBufferSize; j++ )
	{
		m_pMasterProductInfos[j].m_svCameraInfos = l_CameraInfos;
	}// end for

	return l_bOk;
}

void SVPPQObject::AssignCameraToAcquisitionTrigger()
{
	if (m_pTrigger->IsAcquisitionTrigger() && m_pTrigger->mpsvDevice)
	{
		// Get Camera by Trigger DigNum
		int iDigNum = m_pTrigger->mpsvDevice->miChannelNumber;

		for (SVCameraInfoMap::iterator it = m_Cameras.begin(); it != m_Cameras.end(); ++it)
		{
			SVVirtualCamera* pCamera = it->first;
			if ( nullptr != pCamera )
			{
				SVAcquisitionClassPtr acquisitionPtr = pCamera->GetAcquisitionDevice();
				if (!acquisitionPtr.empty() && acquisitionPtr->DigNumber() == iDigNum)
				{
					m_pTrigger->mpsvDevice->m_ulHandle = acquisitionPtr->m_hDigitizer;
					break;
				}
			}
		}
	}
}

void SVPPQObject::PrepareGoOnline()
{
	// Fixup Acquisition triggers (as Cameras can be in a different order than Triggers)
	AssignCameraToAcquisitionTrigger();

	if( ! m_pTrigger->CanGoOnline() )
	{
		SVStringArray msgList;
		msgList.push_back(SVString(m_pTrigger->GetCompleteObjectName()));

		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CanGoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
		throw Msg;
	}

	for( SVCameraInfoMap::iterator l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter )
	{
		if ( ! ( l_svIter->second.m_CameraPPQIndex >= 0 && l_svIter->first->CanGoOnline() ) )
		{
			SVStringArray msgList;
			msgList.push_back(SVString(l_svIter->first->GetCompleteObjectName()));

			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CanGoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
			throw Msg;
		}
	}

	// Reset the PPQ
	for( size_t i = 0; i < m_ppPPQPositions.size(); ++i )
	{
		SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt( i );
		if( nullptr != l_pProduct )
		{
			if( !RecycleProductInfo( l_pProduct ) )
			{
				SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
				throw Msg;
			}

			m_ppPPQPositions.SetProductAt( i, nullptr );
		}

		if(  m_qAvailableProductInfos.RemoveHead( &l_pProduct ) && nullptr != l_pProduct )
		{
			HRESULT hRTemp = m_ppPPQPositions.SetProductAt( i, l_pProduct );

			if( S_OK == hRTemp  )
			{
				if( 0 == i )
				{
					hRTemp = GetNextAvailableIndexes( l_pProduct->oPPQInfo, SV_PPQ );
				}
				else
				{
					hRTemp = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultDataCircleBuffer, SV_PPQ, l_pProduct->oPPQInfo.m_ResultDataDMIndexHandle );
				}
			}

			if( S_OK != hRTemp )
			{
				SVStringArray msgList;
				msgList.push_back(SvUl_SF::Format(_T("%X"), hRTemp));
				SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_RecycleProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
				throw Msg;
			}
		}
		else
		{
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_RecycleProduct, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
			throw Msg;
		}
	}// end for

	size_t iSize = m_arInspections.GetSize();
	for( size_t i = 0; i < iSize; i++ )
	{
		if ( ! m_arInspections[i]->CanGoOnline() )
		{
			SvOi::MessageTextEnum messageId = SvOi::Tid_Empty;
			SVStringArray msgList;
			bool bShowToolError = false;

			//@TODO[gra][7.40][25.05.2016]: This should at a later stage show all the tool errors not only the first error
			SvStl::MessageContainerVector AllToolMessages;
			m_arInspections[i]->getToolMessages( std::back_inserter( AllToolMessages ) );
			SvStl::MessageContainerVector::const_iterator Iter( AllToolMessages.begin() );
			if( AllToolMessages.end() != Iter )
			{
				bShowToolError = true;
				SVString sToolName = SVObjectManagerClass::Instance().GetCompleteObjectName( Iter->getObjectId() );
				msgList.push_back( sToolName );
				msgList.push_back( SVString( Iter->what() ) );
				messageId = SvOi::Tid_CanGoOnlineFailure_InspectionTool;
			}
			if (!bShowToolError)
			{
				messageId = SvOi::Tid_CanGoOnlineFailure_Inspection;
				msgList.push_back(SVString(m_arInspections[i]->GetCompleteObjectName()));
			}
			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, messageId, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
			throw Msg;
		}
	}// end for

	if (!ResolveConditionalOutput())
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_CanGoOnlineFailure_ConditionalOutput, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
		throw Msg;
	}
}// end PrepareGoOnline

void SVPPQObject::GoOnline()
{
	CString sFailureObjectName;

#ifdef EnableTracking
	m_PPQTracking.clear();
	m_PPQTracking.m_QueueLength = m_ppPPQPositions.size();
	m_PPQTracking.m_TimeLength = m_lOutputDelay + 50;
#endif
	RebuildOutputList();

	// First, make sure the trigger toggle output is set to the right default
	if( !( m_pTriggerToggle.empty() ) && m_pTriggerToggle->m_pValueObject )
	{
		BOOL bValue = false;
		m_pTriggerToggle->m_pValueObject->SetValue( 1, bValue );
		m_TriggerToggle = ( bValue != FALSE );
	}// end if

	// Also, make sure the output toggle output is set to the right default
	if( !( m_pOutputToggle.empty() ) && m_pOutputToggle->m_pValueObject )
	{
		BOOL bValue = false;
		m_pOutputToggle->m_pValueObject->SetValue( 1, bValue );
		m_OutputToggle = ( bValue != FALSE );
	}// end if

	// Reset the Outputs
	if( !( m_pTriggerToggle.empty() ) )
	{
		m_pTriggerToggle->m_Enabled = TRUE;
	}

	if( !( m_pOutputToggle.empty() ) )
	{
		m_pOutputToggle->m_Enabled = TRUE;
	}

	ResetOutputs();

	if( !( m_pTriggerToggle.empty() ) )
	{
		m_pTriggerToggle->m_Enabled = FALSE;
	}

	if( !( m_pOutputToggle.empty() ) )
	{
		m_pOutputToggle->m_Enabled = FALSE;
	}

	size_t lSize = m_arInspections.GetSize();
	bool bInspGoOnline = true;
	for( size_t i = 0; i < lSize; i++ )
	{
		if ( ! m_arInspections[i]->GoOnline() )
		{
			sFailureObjectName = m_arInspections[i]->GetCompleteObjectName();
			bInspGoOnline = false;
			break;
		}
	}// end for

	if( !bInspGoOnline )
	{
		lSize = m_arInspections.GetSize();
		for( size_t i = 0; i < lSize; i++ )
		{
			m_arInspections[i]->GoOffline();
		}// end for

		SVStringArray msgList;
		msgList.push_back(SVString(sFailureObjectName));

		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_Inspection, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
		throw Msg;
	}// end if

	if (m_pTrigger->IsSoftwareTrigger())
	{
		// must do this before the Camera starts for Digital cameras
		HRESULT hr = m_pTrigger->EnableInternalTrigger();
		if (S_OK != hr)
		{
			SVStringArray msgList;
			msgList.push_back(SvUl_SF::Format(_T("%X"), hr));

			SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_InternalTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
			throw Msg;
		}
	}

	SVCameraInfoMap::iterator l_svIter;
	bool bCameraGoOnline = true;
	for( l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter )
	{
		if ( ! l_svIter->first->GoOnline() )
		{
			sFailureObjectName = l_svIter->first->GetCompleteObjectName();

			bCameraGoOnline = false;
			break;
		}
	}// end for

	if( !bCameraGoOnline )
	{
		for( l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter )
		{
			l_svIter->first->GoOffline();
		}// end for

		lSize = m_arInspections.GetSize();
		for( size_t i = 0; i < lSize; i++ )
		{
			m_arInspections[i]->GoOffline();
		}// end for

		SVStringArray msgList;
		msgList.push_back(SVString(sFailureObjectName));

		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_Acquisition, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
		throw Msg;
	}// end if

	if( ! m_pTrigger->GoOnline() )
	{
		sFailureObjectName = m_pTrigger->GetCompleteObjectName();
		m_pTrigger->GoOffline();

		SVCameraInfoMap::iterator l_svIter;

		for( l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter )
		{
			l_svIter->first->GoOffline();
		}// end for

		lSize = m_arInspections.GetSize();
		for( size_t i = 0; i < lSize; i++ )
		{
			m_arInspections[i]->GoOffline();
		}// end for

		SVStringArray msgList;
		msgList.push_back(SVString(sFailureObjectName));

		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_Trigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
		throw Msg;
	}// end if

	// Create a multimedia timer thread for the output and reset time delays
	if( TIMERR_NOERROR != ::timeBeginPeriod( 1 ) )
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_CreateTimerThread, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
		throw Msg;
	}

	//The timer should start when not "Next Trigger Mode" or when reset or data valid delay are not 0
	bool StartTimer( SVPPQNextTriggerMode != m_oOutputMode );
	StartTimer |= 0 < m_lResetDelay;
	StartTimer |= 0 < m_DataValidDelay;
	if( StartTimer )
	{
		m_uOutputTimer = ::timeSetEvent( 1, 0, SVPPQObject::OutputTimerCallback, reinterpret_cast<DWORD_PTR>(this),
			TIME_PERIODIC | TIME_CALLBACK_FUNCTION );
	}// end if

	// Create the PPQ's threads
	if ( S_OK != m_AsyncProcedure.Create( &SVPPQObject::APCThreadProcess, boost::bind(&SVPPQObject::ThreadProcess, this, _1), GetName(), SVAffinityPPQ ) )
	{
		SvStl::MessageContainer Msg( SVMSG_SVO_93_GENERAL_WARNING, SvOi::Tid_GoOnlineFailure_CreatePPQThread, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_10185 );
		throw Msg;
	}
	
	// Kick the threads up a notch, for real!
	m_AsyncProcedure.SetPriority( THREAD_PRIORITY_HIGHEST );

	m_bOnline = true;

	// This snippet of code here apears to do nothing, but it is actually 
	// initializing the RTTI for the output object parents.  This insures that there 
	// is no appreciable delay in the writeOutputs function that uses dynaic_cast.
	for( size_t i = 0; i < m_UsedOutputs.size(); i++ )
	{
		void* l_pInit = dynamic_cast<SVPPQObject*>( m_UsedOutputs[i]->m_pValueParent );
	}

	if( SVPPQNextTriggerMode == m_oOutputMode )
	{
		m_NAKCount = -static_cast< long >( m_ppPPQPositions.size() );
	}
	else
	{
		m_NAKCount = 0;
	}
}// end GoOnline

static void QuieseSharedMemory()
{
	SvSml::SVShareControlHandler& rControlShare = SVSharedMemorySingleton::Instance().GetIPCShare();
	if (rControlShare.IsCreated())
	{
		rControlShare.ClearReady();
		bool bStop = false;
		long timetoWait = 25; // ticks are approx 10 to 16 ms
		long startTime = GetTickCount();
		while (!bStop)
		{
			bStop = rControlShare.GotAck();
			if (!bStop)
			{
				long curTime = GetTickCount();
				if ((curTime - startTime) > timetoWait)
				{
					bStop = true;
				}
				else
				{
					::Sleep(20);
				}
			}
		}
	}
}

BOOL SVPPQObject::GoOffline()
{
	long i;
	long lSize;

	if( !m_bOnline ) { return FALSE; }

#ifdef EnableTracking
	if( TheSVObserverApp.UpdateAndGetLogDataManager() )
	{
		SVString l_FileName;

		l_FileName.Format( _T( "C:\\SVObserver\\ProductIndexes_%ld-%s.log" ), 
			SVObjectManagerClass::Instance().GetFileSequenceNumber(), GetName() );

		std::fstream l_Stream( l_FileName.ToString(), std::ios_base::trunc | std::ios_base::out );

		if( l_Stream.is_open() )
		{
			for( size_t i = 0; i < m_ppPPQPositions.size(); ++i )
			{
				SVString l_Info;

				SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt( i );

				if( nullptr != l_pProduct )
				{
					l_pProduct->DumpIndexInfo( l_Info );
				}

				l_Stream << _T( "Position " ) << i << _T( " : " );
				l_Stream << l_Info.ToString();
				l_Stream << std::endl;
			}

			l_Stream.close();
		}

		SVString l_Name;

		l_Name.Format( _T( "%s-%ld" ), GetName(), SVObjectManagerClass::Instance().GetFileSequenceNumber() );

		DumpDMInfo( l_Name.ToString() );

		l_Name.Format( _T( "C:\\SVObserver\\%s-Counts-%ld.csv" ), GetName(), SVObjectManagerClass::Instance().GetFileSequenceNumber() );

		std::fstream l_TrackingStream( l_Name.ToString(), std::ios_base::trunc | std::ios_base::out );

		if( l_TrackingStream.is_open() )
		{
			l_TrackingStream << _T( "Name,Count" ) << std::endl;

			SVPPQTracking::SVPPQTrackingMap::iterator l_PPQIter = m_PPQTracking.m_Counts.begin();

			while( l_PPQIter != m_PPQTracking.m_Counts.end() )
			{
				l_TrackingStream << l_PPQIter->first.c_str() << _T( "," );
				l_TrackingStream << l_PPQIter->second;
				l_TrackingStream << std::endl;

				++l_PPQIter;
			}

			l_TrackingStream << std::endl;

			l_TrackingStream << _T( "Name" );

			for( size_t i = 0; i < m_ppPPQPositions.size(); ++i )
			{
				l_TrackingStream << _T( "," ) << i;
			}

			l_TrackingStream << std::endl;

			SVPPQTracking::SVQueueTrackingMap::iterator l_QueueIter = m_PPQTracking.m_QueueCounts.begin();

			while( l_QueueIter != m_PPQTracking.m_QueueCounts.end() )
			{
				l_TrackingStream << l_QueueIter->first.c_str();

				SVPPQTrackingElement& l_rElement = l_QueueIter->second;

				for( size_t i = 0; i < l_rElement.m_TrackedCounts.size(); ++i )
				{
					l_TrackingStream << _T( "," ) << l_rElement.m_TrackedCounts[ i ];
				}

				l_TrackingStream << std::endl;

				++l_QueueIter;
			}

			l_TrackingStream << std::endl;

			// Write Outputs Time...
			l_TrackingStream << _T( "Time ms" );

			for( size_t i = 0 ; i < m_PPQTracking.m_TimeLength ; ++i )
			{
				l_TrackingStream << _T( "," ) << i ;
			}

			l_TrackingStream << std::endl;

			l_QueueIter = m_PPQTracking.m_QueueWriteTimeCounts.begin();  // m_QueueCounts.begin();

			while( l_QueueIter != m_PPQTracking.m_QueueWriteTimeCounts.end() )
			{
				l_TrackingStream << l_QueueIter->first.c_str();

				SVPPQTrackingElement& l_rElement = l_QueueIter->second;

				for( size_t i = 0; i < l_rElement.m_TrackedCounts.size(); ++i )
				{
					l_TrackingStream << _T( "," ) << l_rElement.m_TrackedCounts[ i ];
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

	for( l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter )
	{
		l_svIter->first->GoOffline();
	}// end for

	if (HasActiveMonitorList())
	{
		QuieseSharedMemory();
	}

	lSize = m_arInspections.GetSize();
	for( i = 0; i < lSize; i++ )
	{
		m_arInspections[i]->GoOffline();
	}// end for

	// Stop the multimedia timer thread for the output and reset time delays
	if( 0 != m_uOutputTimer )
	{
		::timeKillEvent( m_uOutputTimer );

		::timeEndPeriod( 1 );

		m_uOutputTimer = 0;
	}

	// Bring the threads back down to earth
	m_AsyncProcedure.SetPriority( THREAD_PRIORITY_NORMAL );

	m_AsyncProcedure.Destroy();

	m_bOnline = FALSE;

	unsigned long l_ClearCount = 0;

	for( i = static_cast< long >( m_ppPPQPositions.size() ) - 1; l_ClearCount < 5 && 0 < i; --i )
	{
		SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt( i );

		if( nullptr != l_pProduct )
		{
			if( ! l_pProduct->oPPQInfo.m_ResultImagePublishedDMIndexHandle.empty() )
			{
				l_pProduct->oPPQInfo.m_ResultImagePublishedDMIndexHandle.clear();

				++l_ClearCount;
			}

			l_pProduct->SetProductComplete();
		}
	}
	if (HasActiveMonitorList())
	{
		SetMonitorList(ActiveMonitorList(false, RejectDepthAndMonitorList()));
		SVSharedMemorySingleton::Instance().ErasePPQSharedMemory(GetUniqueObjectID());
	}

	return TRUE;
}// end GoOffline

BOOL SVPPQObject::IsOnline() const
{
	return m_bOnline;
}// end IsOnline

BOOL SVPPQObject::AddInput( SVIOEntryHostStructPtr pInput )
{
	m_AllInputs.push_back( pInput );

	return TRUE;
}// end AddInput

BOOL SVPPQObject::RemoveInput( SVIOEntryHostStructPtr pInput )
{
	BOOL l_Status = false;

	CString strName;

	if( pInput->m_pValueObject )
	{
		strName = pInput->m_pValueObject->GetCompleteObjectName();
	}
	else if( !( pInput->m_IOId.empty() ) )
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pInput->m_IOId );

		strName = l_pObject->GetCompleteObjectName();
	}

	SVIOEntryHostStructPtrList::iterator l_Iter = m_AllInputs.begin();

	while( l_Iter != m_AllInputs.end() )
	{
		l_Status = ( strName == ( *l_Iter )->m_pValueObject->GetCompleteObjectName() );

		if( l_Status )
		{
			m_AllInputs.erase( l_Iter );

			break;
		}

		++l_Iter;
	}

	return l_Status;
}

// AssignInputs () ----------------------------------------------------------
// This is only ever called from SVPPQObject::InitializeProduct ().
// InitializeProduct () is only ever called from ProcessTrigger ().

BOOL SVPPQObject::AssignInputs( const SVVariantBoolVector& p_rInputValues )
{
	for( size_t i = 0; i < m_UsedInputs.size(); i++ )
	{
		SVIOEntryHostStructPtr pIOEntry = m_UsedInputs[ i ];

		if( !( pIOEntry.empty() ) && pIOEntry->m_Enabled)
		{
			if( 0 <= pIOEntry->m_PPQIndex && pIOEntry->m_PPQIndex < static_cast< long >( m_ppPPQPositions.size() ) )
			{
				SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt( pIOEntry->m_PPQIndex );

				SVIOEntryStruct& l_pIOEntry = l_pProduct->oPPQInfo.m_InputData[ i ];
				long lDataIndex = l_pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

				if (pIOEntry->m_ObjectType != IO_CAMERA_DATA_INPUT)
				{
					_bstr_t l_String = p_rInputValues[ i ].first;

					l_pIOEntry.m_IOEntryPtr->m_pValueObject->SetValue( lDataIndex, l_String );
					l_pIOEntry.m_EntryValid = p_rInputValues[ i ].second;
				}
				else
				{
					l_pIOEntry.m_EntryValid = true;
				}

				// @Warning - I believe this is redundant.  AssignInputs () 
				// is only processed through ProcessTrigger (), where it is 
				// also inserted.  Should not cause a problem because it is an
				// std::set, but only needs done in one place and that should
				// be within ProcessTrigger ().
				m_oNotifyInspectionsSet.insert( l_pProduct->ProcessCount() );
			}
		}
	}

	return true;
}

HRESULT SVPPQObject::GetInputIOValues( SVVariantBoolVector& p_rInputValues ) const
{
	HRESULT l_Status( S_OK );

	if( !m_pInputList->ReadInputs( m_UsedInputs, p_rInputValues ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

BOOL SVPPQObject::RebuildInputList(bool bHasCameraTrigger)
{
	SVIOEntryHostStructPtrList ppNewInputs;
	SVIOEntryHostStructPtr pOldInput;
	SVIOEntryHostStructPtr pNewInput;
	size_t iNew;
	size_t iOld;
	size_t j;
	size_t lNewSize;

	// Make sure all the defaults are here for old configurations
	AddDefaultInputs();

	if( m_pInputList && m_pInputList->FillInputs( ppNewInputs ) )
	{
		RemoveCameraDataInputs(ppNewInputs);
		if (bHasCameraTrigger)
		{
			AddCameraDataInputs(ppNewInputs);
		}
		lNewSize = ppNewInputs.size();

		for( iOld = 0; iOld < m_AllInputs.size(); iOld++ )
		{
			pOldInput = m_AllInputs[iOld];

			for( iNew = 0; iNew < lNewSize; iNew++ )
			{
				pNewInput = ppNewInputs[iNew];

				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pNewInput->m_IOId );

				if( ( 0 == strcmp( l_pObject->GetName(), pOldInput->m_pValueObject->GetName() ) ) &&
					( pNewInput->m_ObjectType == pOldInput->m_ObjectType ) )
				{
					pNewInput->m_DeleteValueObject = false;
					pNewInput->m_pValueObject = pOldInput->m_pValueObject;
					pNewInput->m_pValueParent = pOldInput->m_pValueParent;
					pNewInput->m_PPQIndex	= pOldInput->m_PPQIndex;
					pNewInput->m_Enabled		= pOldInput->m_Enabled;
					pOldInput->m_ObjectType	= pNewInput->m_ObjectType;
					pOldInput->m_IOId	= pNewInput->m_IOId;
					pOldInput->m_pValueObject->SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
					pOldInput->m_pValueObject->ResetObject();
					break;
				}// end if
			}// end for
		}// end for

		m_UsedInputs = ppNewInputs;

		std::sort( m_UsedInputs.begin(), m_UsedInputs.end(), &SVIOEntryHostStruct::PtrGreater );

		// Fill out all the product info structs with the current inputs
		for( j = 0; j < ( m_ppPPQPositions.size() + g_lPPQExtraBufferSize ); j++ )
		{
			m_pMasterProductInfos[j].oPPQInfo.m_InputData.resize( m_UsedInputs.size() );

			for( size_t z = 0; z < m_UsedInputs.size(); ++z )
			{
				m_pMasterProductInfos[j].oPPQInfo.m_InputData[z].m_IOEntryPtr = m_UsedInputs[z]; 
			}
		}

		return TRUE;
	}// end if

	return FALSE;
}// end RebuildInputList

typedef boost::function<bool (SVIOEntryHostStructPtr ioEntry, const SVIOObjectType& type)> CompareIOEntryTypeFunc;
static bool CompareTypeWithIOEntry(SVIOEntryHostStructPtr ioEntry, const SVIOObjectType& type)
{
	bool bRetVal = false;
	if (ioEntry)
	{
		bRetVal = (ioEntry->m_ObjectType == type);
	}
	return bRetVal; 
}

typedef boost::function<bool (SVIOEntryHostStructPtr ioEntry, const SVString& name)> CompareIOEntryNameFunc;
static bool CompareNameWithIOEntry(SVIOEntryHostStructPtr ioEntry, const SVString& name)
{
	bool bRetVal = false;
	if (ioEntry && ioEntry->m_pValueObject)
	{
		bRetVal = ( SVString( ioEntry->m_pValueObject->GetName() ) == name);
	}
	return bRetVal; 
}

static bool CompareCompleteNameWithIOEntry(SVIOEntryHostStructPtr ioEntry, const SVString& name)
{
	bool bRetVal = false;
	if (ioEntry && ioEntry->m_pValueObject)
	{
		bRetVal = ( SVString( ioEntry->m_pValueObject->GetCompleteObjectName() ) == name);
	}
	return bRetVal; 
}

// @CLEAN This is a different class that should be moved to its own file.
template<typename CompareTo, typename CompareFunc>
class FindIOEntry
{
public:
	FindIOEntry(const CompareTo& data, CompareFunc func)
		: m_data(data), m_compareFunc(func)	{}

	bool operator()(SVIOEntryHostStructPtr ioEntry) const
	{
		return m_compareFunc(ioEntry, m_data);
	}

private:
	CompareTo m_data;
	CompareFunc m_compareFunc;
};

void SVPPQObject::AddCameraDataInputs(SVIOEntryHostStructPtrList& list)
{
	// Added the new Camera inputs...
	SVIOEntryHostStructPtrList::iterator it = std::find_if(list.begin(), list.end(), FindIOEntry<SVString, CompareIOEntryNameFunc>(m_CameraInputData.GetTimestampName(), CompareNameWithIOEntry));
	if (it == list.end())
	{
		// Get the Input from the All list
		SVIOEntryHostStructPtrList::const_iterator refIt = std::find_if(m_AllInputs.begin(), m_AllInputs.end(), FindIOEntry<SVString, CompareIOEntryNameFunc>(m_CameraInputData.GetTimestampName(), CompareNameWithIOEntry));
		if (refIt != m_AllInputs.end())
		{
			list.push_back(*refIt);
		}
	}
	it = std::find_if(list.begin(), list.end(), FindIOEntry<SVString, CompareIOEntryNameFunc>(m_CameraInputData.GetLineStateName(), CompareNameWithIOEntry));
	if (it == list.end() )
	{
		// Get the Input from the All list
		SVIOEntryHostStructPtrList::const_iterator refIt = std::find_if(m_AllInputs.begin(), m_AllInputs.end(), FindIOEntry<SVString, CompareIOEntryNameFunc>(m_CameraInputData.GetLineStateName(), CompareNameWithIOEntry));
		if (refIt != m_AllInputs.end())
		{
			list.push_back(*refIt);
		}
	}
}

void SVPPQObject::RemoveCameraDataInputs(SVIOEntryHostStructPtrList& list)
{
	SVIOEntryHostStructPtrList::iterator it = std::remove_if(list.begin(), list.end(), FindIOEntry<SVIOObjectType, CompareIOEntryTypeFunc>(IO_CAMERA_DATA_INPUT, CompareTypeWithIOEntry));
	if (it != list.end())
	{
		list.erase(it, list.end());
	}
}

void SVPPQObject::SetDefaultConditionalOutput()
{
	m_conditionalOutputName = PPQ_CONDITIONAL_OUTPUT_ALWAYS;
	m_conditionalOutputValueID = GUID_NULL;
}

bool SVPPQObject::HasCameraDataInputForConditionalOutput() const
{
	return (m_conditionalOutputName.find(_T("Camera")) != SVString::npos);
}

bool SVPPQObject::HasDigitalInputForConditionalOutput() const
{
	return (m_conditionalOutputName.find(_T("DIO")) != SVString::npos);
}

// AddToAvailableInputs searches the m_AllInputs by name. If it does not exist,
// then the new input is added. Two types are supported: 
// IO_REMOTE_INPUT and IO_DIGITAL_INPUT.
// A variant value object is created for the remote input.
// A Boolean value object is created for the digital input.
BOOL SVPPQObject::AddToAvailableInputs(SVIOObjectType eType, CString strName )
{
	bool bFound = false;
	BOOL bRet = FALSE;

	// search for the input. 
	for(size_t k = 0 ; k < m_AllInputs.size(); k++ )
	{
		SVIOEntryHostStructPtr pOldInput = m_AllInputs[k];
		if( 0 == strcmp( strName, pOldInput->m_pValueObject->GetName() ) )
		{
			bFound = true;
			break;
		}// end if
	}// end for

	if( !bFound )
	{
		SVValueObjectClass* pObject = nullptr;
		if( eType == IO_REMOTE_INPUT )
		{
			// new variant value object for Remote Inputs.
			pObject = new SVVariantValueObjectClass();
		}
		else
		{
			// new Bool Value Object for Digital Inputs.
			pObject = new SVBoolValueObjectClass();
		}

		pObject->SetName( strName );
		pObject->SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
		pObject->ResetObject();

		SVIOEntryHostStructPtr pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_pValueObject	= pObject;
		pIOEntry->m_pValueParent	= pObject;
		pIOEntry->m_ObjectType	= eType;
		pIOEntry->m_PPQIndex		= -1;
		pIOEntry->m_Enabled		= FALSE;

		// Add input to PPQ.
		bRet = AddInput( pIOEntry );
	}// end if
	return bRet;
}

// Add Default Inputs adds inputs to the m_AllInputs 
// m_AllInputs appears to be the available input list.
BOOL SVPPQObject::AddDefaultInputs()
{
	CString strName;
	unsigned long ulCount = 0;
	unsigned long l;

	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputCount( ulCount );

	// Create all the default Digital Inputs
	for( l = 0; l < ulCount; l++ )
	{
		strName.Format( "DIO.Input%d", l + 1 );
		AddToAvailableInputs(IO_DIGITAL_INPUT, strName );
	}// end for

	SVConfigurationObject* pConfig( nullptr );
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVInputObjectList* pInputObjectList( nullptr );
	if( nullptr != pConfig ){ pInputObjectList = pConfig->GetInputObjectList( ); }
	long lCount=0;
	//If pointer is a nullptr then count is 0
	if( nullptr != pInputObjectList ){ pInputObjectList->GetRemoteInputCount(lCount); }

	// Create all the default Remote Inputs
	for( l = 0; l < static_cast<unsigned long>(lCount); l++ )
	{
		strName.Format( "Remote Input %d", l + 1 );
		AddToAvailableInputs(IO_REMOTE_INPUT, strName);
	}// end for

	return TRUE;
}

BOOL SVPPQObject::GetAvailableInputs( SVIOEntryHostStructPtrList& ppIOEntries ) const
{
	ppIOEntries = m_UsedInputs;

	return TRUE;
}

BOOL SVPPQObject::GetAllInputs( SVIOEntryHostStructPtrList& ppIOEntries ) const
{
	ppIOEntries = m_AllInputs;

	return TRUE;
}

SVIOEntryHostStructPtr SVPPQObject::GetInput( const SVString& name ) const
{
	for (SVIOEntryHostStructPtrList::const_iterator it = m_AllInputs.begin();it != m_AllInputs.end();++it)
	{
		if (!it->empty())
		{
			SVIOEntryHostStructPtr ptr = (*it);
			if (name == ptr->m_pValueObject->GetName())
			{
				return ptr;
			}
		}
	}
	return SVIOEntryHostStructPtr();
}

BOOL SVPPQObject::AddOutput( SVIOEntryHostStructPtr pOutput )
{
	m_AllOutputs.push_back( pOutput );

	return TRUE;
}

BOOL SVPPQObject::RemoveOutput( SVIOEntryHostStructPtr pOutput )
{
	BOOL l_Status = false;

	if ( m_voOutputState.GetName() != "State" )
	{
		m_voOutputState.SetName( _T( "State" ) );
		m_voOutputState.SetObjectOwner(this);
	}

	CString strName;

	strName.Empty();
	if( pOutput->m_pValueObject )
		strName = pOutput->m_pValueObject->GetCompleteObjectName();
	else if( !( pOutput->m_IOId.empty() ) )
	{
		SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pOutput->m_IOId );

		strName = l_pObject->GetCompleteObjectName();
	}

	SVIOEntryHostStructPtrList::iterator l_Iter = m_AllOutputs.begin();

	while( l_Iter != m_AllOutputs.end() )
	{
		CString l_csPPQName;
		l_csPPQName = ( *l_Iter )->m_pValueObject->GetCompleteObjectName();

		l_Status = ( strName == l_csPPQName );

		if( l_Status )
		{
			m_AllOutputs.erase( l_Iter );

			break;
		}

		++l_Iter;
	}

	return l_Status;
}

bool SVPPQObject::ResolveConditionalOutput()
{
	bool bRetVal = true;

	m_conditionalOutputValueID = GUID_NULL;

	if (!AlwaysWriteOutputs())
	{
		// Get Input with this name and assign guid
		SVIOEntryHostStructPtrList::const_iterator it = std::find_if(m_UsedInputs.begin(), m_UsedInputs.end(), FindIOEntry<SVString, CompareIOEntryNameFunc>(m_conditionalOutputName, CompareNameWithIOEntry));
		if (it != m_UsedInputs.end())
		{
			SVIOEntryHostStructPtr ioEntry = (*it);
			if (!ioEntry.empty() && ioEntry->m_pValueObject)
			{
				m_conditionalOutputValueID = ioEntry->m_pValueObject->GetUniqueObjectID();
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

bool SVPPQObject::EvaluateConditionalOutput(long dataIndex) const
{
	bool bRetVal = AlwaysWriteOutputs();
	if (!bRetVal)
	{
		SVObjectClass* pObject = nullptr;
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByIdentifier(m_conditionalOutputValueID, pObject);
		if (S_OK == hr && nullptr != pObject )
		{
			SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass *>(pObject);
			if (pValueObject)
			{
				double value;
				hr = pValueObject->GetValue(dataIndex, value);
				if (S_OK == hr )
				{
					bRetVal = (value > std::numeric_limits<double>::epsilon()) ? true : false;
				}
			}
		}
	}
	return bRetVal;
}

BOOL SVPPQObject::WriteOutputs( SVProductInfoStruct *pProduct )
{
	BOOL bRet = TRUE;
	SVDataManagerHandle l_Handle;
	long lDataIndex = -1;
	long l_TriggerCount = 0;
	bool l_TriggerToggle = false;
	bool l_ACK = false;
	bool l_NAK = true;

#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
	SVString l_ProductState;
#endif
#endif

	m_OutputToggle = ! m_OutputToggle;

	if( nullptr != pProduct )
	{
		l_TriggerCount = pProduct->ProcessCount();
		l_TriggerToggle = pProduct->oTriggerInfo.m_ToggleState;

		pProduct->oTriggerInfo.m_PushedOutputs = SVClock::GetTimeStamp();

		lDataIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

		if( !( pProduct->bDataComplete ) )
		{
			SetProductIncomplete( *pProduct );
		}

#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
		l_ProductState = pProduct->m_ProductState;
#endif
#endif
	}

	bool bWriteOutputs = true;
	if (!AlwaysWriteOutputs())
	{
		// get value that represenst condition
		bWriteOutputs = EvaluateConditionalOutput(lDataIndex);
	}

	if( 0 <= lDataIndex )
	{
		m_voACK.GetValue( lDataIndex, l_ACK );

		m_voNAK.GetValue( lDataIndex, l_NAK );

		// First, write the trigger toggle output if it is okay
		if( !( m_pTriggerToggle.empty() ) && m_pTriggerToggle->m_pValueObject )
		{
			m_pTriggerToggle->m_pValueObject->SetValue( lDataIndex, l_TriggerToggle );
		}

		// Toggle the Output Toggle if it is okay
		if( !( m_pOutputToggle.empty() ) && m_pOutputToggle->m_pValueObject )
		{
			m_pOutputToggle->m_pValueObject->SetValue( lDataIndex, m_OutputToggle );
		}
	}
	else
	{
		if( S_OK == TheSVDataManager.GetNextAvailableBufferIndex( m_pResultDataCircleBuffer, SV_PPQ, l_Handle )  )
		{
			lDataIndex = l_Handle.GetIndex();

			m_voNotInspected.SetValue( lDataIndex, TRUE );
			m_voNAK.SetValue( lDataIndex, TRUE );
			m_voMasterWarning.SetValue(	lDataIndex, TRUE );
			m_voMasterFault.SetValue(	lDataIndex, TRUE );
			m_voDataValid.SetValue( lDataIndex, FALSE );
			m_voACK.SetValue( lDataIndex, FALSE );
		}

		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//m_PPQTracking.IncrementCount( _T( "Product Missing" ) );
#endif
	}

	if( 0 <= lDataIndex )
	{
		if (bWriteOutputs)
		{
			bRet = m_pOutputList->WriteOutputs( m_UsedOutputs, lDataIndex, l_ACK, l_NAK );

			SVConfigurationObject* pConfig( nullptr );
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );


			bool l_bValue=false;
			m_voDataValid.GetValue(lDataIndex, l_bValue);
			if( 0 == m_DataValidDelay )
			{
				if( !m_pDataValid.empty() )
				{
					m_pOutputList->WriteOutputValue( m_pDataValid, l_bValue );
				}
				if( !m_pOutputToggle.empty() )
				{
					m_pOutputList->WriteOutputValue( m_pOutputToggle, m_OutputToggle );
				}
			}
			else
			{
				pProduct->oOutputsInfo.DataValidResult = l_bValue;
				pProduct->oOutputsInfo.OutputToggleResult = m_OutputToggle;
				// Set output data valid expire time
				pProduct->oOutputsInfo.m_EndDataValidDelay = SVClock::GetTimeStamp() + pProduct->oOutputsInfo.lDataValidDelay;
				m_DataValidDelayQueue.AddTail( pProduct->ProcessCount() );
			}
		}
		pProduct->oTriggerInfo.m_PushedOutputs = SVClock::GetTimeStamp();
		long l_lTime = static_cast<long>(pProduct->oTriggerInfo.m_PushedOutputs - pProduct->oTriggerInfo.m_ToggleTimeStamp);
		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//m_PPQTracking.IncrementTimeCount( _T( "Output Toggle"), l_lTime );
#endif
	}
	else
	{
		bRet = false;
	}

#ifdef _DEBUG
#ifdef SHOW_PPQ_STATE
	if( !( l_ProductState.empty() ) )
	{
		l_ProductState += _T( "\n" );
		OutputDebugString( l_ProductState.c_str() );
	}
#endif
#endif

	if( l_NAK )
	{
		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//m_PPQTracking.IncrementCount( _T( "Product NAK" ) );
#endif
		::InterlockedIncrement( &m_NAKCount );
	}

	return bRet;
}// end WriteOutputs

BOOL SVPPQObject::ResetOutputs()
{
	BOOL l_bRet = FALSE;

	if( !( m_pDataValid.empty() ) )
	{
		m_pDataValid->m_Enabled = TRUE;
	}

	if( m_pOutputList )
	{
		l_bRet = m_pOutputList->ResetOutputs( m_UsedOutputs );
	}

	if( !( m_pDataValid.empty() ) )
	{
		m_pDataValid->m_Enabled = FALSE;
	}

	return l_bRet;
}// end ResetOutputs

BOOL SVPPQObject::RebuildOutputList()
{
	SVIOEntryHostStructPtrList ppNewOutputs;
	SVIOEntryHostStructPtr pOldOutput;
	SVIOEntryHostStructPtr pNewOutput;
	CString l_csOldName;
	CString l_csNewName;
	size_t iOld;
	size_t iNew;
	size_t lNewSize;

	// Make sure all the defaults are here for old configurations
	AddDefaultOutputs();

	if( m_pOutputList && m_pOutputList->FillOutputs( ppNewOutputs ) )
	{
		lNewSize = ppNewOutputs.size();

		m_pTriggerToggle.clear();
		m_pOutputToggle.clear();
		m_pDataValid.clear();

		for( iOld = 0; iOld < m_AllOutputs.size(); iOld++ )
		{
			pOldOutput = m_AllOutputs[iOld];
			pOldOutput->m_IOId.clear();

			for( iNew = 0; iNew < lNewSize; iNew++ )
			{
				pNewOutput = ppNewOutputs[iNew];

				SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( pNewOutput->m_IOId );

				l_csNewName = l_pObject->GetCompleteObjectName();
				l_csOldName = pOldOutput->m_pValueObject->GetCompleteObjectName();

				if ( pNewOutput->m_ObjectType == pOldOutput->m_ObjectType &&
					l_csNewName == l_csOldName )
				{
					// Copy information to new Output object
					l_pObject->SetName( l_csOldName );

					pNewOutput->m_DeleteValueObject = false;
					pNewOutput->m_pValueObject = pOldOutput->m_pValueObject;
					pNewOutput->m_pValueParent = pOldOutput->m_pValueParent;
					pNewOutput->m_Enabled	 = pOldOutput->m_Enabled;
					pOldOutput->m_ObjectType	 = pNewOutput->m_ObjectType;
					pOldOutput->m_IOId	 = pNewOutput->m_IOId;
					pOldOutput->m_pValueObject->SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
					pOldOutput->m_pValueObject->ResetObject();

					// Check for prefix of PPQ for these special signals...
					if( l_csNewName.Find( _T("PPQ_")) == 0 )
					{
						// Disable Trigger Toggle since it is not written with the outputs
						if( l_csNewName.Find( _T( "Trigger Toggle" ) ) >= 0 )
						{
							m_pTriggerToggle = pNewOutput;
							pNewOutput->m_Enabled = FALSE;
						}// end if

						// Find Output Toggle now to make it quicker later
						if( l_csNewName.Find( _T( "Output Toggle" ) ) >= 0 )
						{
							m_pOutputToggle = pNewOutput;
							pNewOutput->m_Enabled = FALSE;
						}// end if

						// Find Data Valid now to make it quicker later
						if( l_csNewName.Find( _T( "Data Valid" ) ) >= 0 )
						{
							m_pDataValid = pNewOutput;
							pNewOutput->m_Enabled = FALSE;
						}// end if
					}
					break;
				}// end if
			}// end for
		}// end for

		m_UsedOutputs = ppNewOutputs;

		return TRUE;
	}// end if

	return FALSE;
}// end RebuildOutputList

BOOL SVPPQObject::AddDefaultOutputs()
{
	// Setup PPQ variables that are available as outputs
	SVIOEntryHostStructPtr pIOEntry;
	CString strName;
	BOOL	bFound;
	size_t	l;

	strName = _T( "Not Inspected" );
	for( l = 0, bFound = FALSE; !bFound && l < m_AllOutputs.size(); l++ )
	{
		pIOEntry = m_AllOutputs[l];

		if( 0 == strcmp( strName, pIOEntry->m_pValueObject->GetName() ) )
		{
			bFound = TRUE;
			break;
		}// end if
	}// end for

	m_voNotInspected.SetName( _T( "Not Inspected" ) );
	m_voNotInspected.SetObjectOwner( this );
	m_voNotInspected.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voNotInspected.ResetObject();
	m_voNotInspected.SetDefaultValue( TRUE, TRUE );
	m_voNotInspected.SetValue( 1, TRUE );

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voNotInspected;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= TRUE;

		AddOutput( pIOEntry );
	}// end if

	m_voNAK.SetName( _T( "NAK" ) );
	m_voNAK.SetObjectOwner( this );
	m_voNAK.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voNAK.ResetObject();
	m_voNAK.SetDefaultValue( TRUE, TRUE );
	m_voNAK.SetValue( 1, TRUE );

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voNAK;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= TRUE;

		AddOutput( pIOEntry );
	}// end if

	m_voMasterWarning.SetName( _T( "Master Warning" ) );
	m_voMasterWarning.SetObjectOwner( this );
	m_voMasterWarning.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voMasterWarning.ResetObject();
	m_voMasterWarning.SetDefaultValue( TRUE, TRUE );
	m_voMasterWarning.SetValue( 1, TRUE );

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voMasterWarning;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= TRUE;

		AddOutput( pIOEntry );
	}// end if

	m_voMasterFault.SetName( _T( "Master Fault" ) );
	m_voMasterFault.SetObjectOwner( this );
	m_voMasterFault.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voMasterFault.ResetObject();
	m_voMasterFault.SetDefaultValue( TRUE, TRUE );
	m_voMasterFault.SetValue( 1, TRUE );

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voMasterFault;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= TRUE;

		AddOutput( pIOEntry );
	}// end if

	m_voDataValid.SetName( _T( "Data Valid" ) );
	m_voDataValid.SetObjectOwner( this );
	m_voDataValid.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voDataValid.ResetObject();
	m_voDataValid.SetDefaultValue( FALSE, TRUE );
	m_voDataValid.SetValue( 1, FALSE );

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voDataValid;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= TRUE;

		AddOutput( pIOEntry );
	}// end if

	m_voACK.SetName( _T( "ACK" ) );
	m_voACK.SetObjectOwner( this );
	m_voACK.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voACK.ResetObject();
	m_voACK.SetDefaultValue( FALSE, TRUE );
	m_voACK.SetValue( 1, FALSE );

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voACK;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= TRUE;

		AddOutput( pIOEntry );
	}// end if

	m_voTriggerToggle.SetName( _T( "Trigger Toggle" ) );
	m_voTriggerToggle.SetObjectOwner( this );
	m_voTriggerToggle.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voTriggerToggle.ResetObject();
	m_voTriggerToggle.SetDefaultValue( TRUE, TRUE );
	m_voTriggerToggle.SetValue( 1, TRUE );
	m_TriggerToggle = true;

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voTriggerToggle;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= FALSE;

		AddOutput( pIOEntry );
	}// end if

	m_voOutputToggle.SetName( _T( "Output Toggle" ) );
	m_voOutputToggle.SetObjectOwner( this );
	m_voOutputToggle.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voOutputToggle.ResetObject();
	m_voOutputToggle.SetDefaultValue( TRUE, TRUE );
	m_voOutputToggle.SetValue( 1, TRUE );
	m_OutputToggle = true;

	if( !bFound )
	{
		pIOEntry = new SVIOEntryHostStruct;
		pIOEntry->m_DeleteValueObject = false;
		pIOEntry->m_pValueObject	= &m_voOutputToggle;
		pIOEntry->m_pValueParent	= this;
		pIOEntry->m_ObjectType	= IO_DIGITAL_OUTPUT;
		pIOEntry->m_Enabled		= FALSE;

		AddOutput( pIOEntry );
	}// end if

	// This value object is used as the first default PLC output.
	m_voTriggerCount.SetName( _T( "Trigger Count" ) );
	m_voTriggerCount.SetObjectOwner( this );
	m_voTriggerCount.SetObjectDepth( GetPPQLength() + g_lPPQExtraBufferSize );
	m_voTriggerCount.ResetObject();
	m_voTriggerCount.SetDefaultValue( 0L, TRUE );
	m_voTriggerCount.SetValue( 1, 0L );

	return TRUE;
}// end AddDefaultOutputs

BOOL SVPPQObject::GetAllOutputs( SVIOEntryHostStructPtrList& ppIOEntries ) const
{
	ppIOEntries = m_AllOutputs;

	return TRUE;
}

SVProductInfoStruct* SVPPQObject::IndexPPQ( SvTh::SVTriggerInfoStruct& p_rTriggerInfo )
{
	SVProductInfoStruct* l_pProduct = nullptr;
	SVProductInfoStruct* l_pNewProduct = nullptr;
	SVProductInfoStruct* l_pPrevProduct = m_ppPPQPositions.GetProductAt( 0 );
	SVProductInfoStruct* l_pLastProduct = m_ppPPQPositions.IndexRegister();

	if( m_qAvailableProductInfos.RemoveHead( &l_pNewProduct ) && nullptr != l_pNewProduct )
	{
		l_pNewProduct->oTriggerInfo = p_rTriggerInfo;

		if( nullptr != l_pPrevProduct )
		{
			l_pNewProduct->oTriggerInfo.m_PreviousTrigger = l_pPrevProduct->oTriggerInfo.m_BeginProcess;
		}

		SVString l_TriggerCount = SvUl_SF::Format( _T( "%ld" ), p_rTriggerInfo.lTriggerCount );

		l_pNewProduct->bTriggered = TRUE;
		l_pNewProduct->m_ProductState += _T( "|" );
		l_pNewProduct->m_ProductState += GetName();
		l_pNewProduct->m_ProductState += _T( "|TRI=" );
		l_pNewProduct->m_ProductState += l_TriggerCount;

		l_pNewProduct->SetProductActive();

		m_ppPPQPositions.SetProductAt( 0, l_pNewProduct );
	}

	if( m_oOutputMode == SVPPQNextTriggerMode )
	{
		l_pProduct = m_ppPPQPositions.GetProductAt( m_ppPPQPositions.size() - 1 );
	}
	else
	{
		l_pProduct = l_pNewProduct;
	}

	// Start the Outputs. This will notify the cameras to acquire
	StartOutputs( l_pProduct );

	if( nullptr != l_pLastProduct )
	{
		// Recycle the exiting SVProductInfoStruct
		if( !RecycleProductInfo( l_pLastProduct ) )
		{
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_69_PPQ_INDEX_NOT_RELEASED, SvOi::Tid_Empty, SvStl::SourceFileParams(StdMessageParams) );
		}
	}

#ifdef _DEBUG
	if( TheSVObserverApp.GetLogDataManager() )
	{
		SVString l_ProductStates;

		l_ProductStates += GetName();
		l_ProductStates += _T( "|SVPPQObject::IndexPPQ\n" );

		m_ppPPQPositions.GetProductStates( l_ProductStates );

		::OutputDebugString( l_ProductStates.c_str() );
	}
#endif

	return l_pNewProduct;
}

BOOL SVPPQObject::InitializeProduct( SVProductInfoStruct* p_pNewProduct, const SVVariantBoolVector& p_rInputValues )
{
	HRESULT hr;
	CString strDisplay;

	// Now we need to get the IO ready for this Product. Make sure that all locks are set
	// and that all indexes are set correctly
	// ************************************************************************
	// Begin preparing the IO
	// Get Next available indexes from THE Data Manager
	hr = GetNextAvailableIndexes( p_pNewProduct->oPPQInfo, SV_PPQ );

	// ************************************************************************
	// Now we need to get the IO ready for this Product. Make sure that all locks are set
	// and that all indexes are set correctly
	// Set all IO input objects for this product to use the new input data index

	// Reset all IO output objects for this product to use the new output data index
	m_voNotInspected.SetValue(	p_pNewProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
	m_voNAK.SetValue(			p_pNewProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
	m_voMasterWarning.SetValue(	p_pNewProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
	m_voMasterFault.SetValue(	p_pNewProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
	m_voDataValid.SetValue(		p_pNewProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), FALSE );
	m_voACK.SetValue(			p_pNewProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), FALSE );
	// End preparing the IO
	// ************************************************************************


	// ************************************************************************
	// Now we need to read the inputs. Make sure that we record the timestamps
	// Begin reading the inputs
	p_pNewProduct->oInputsInfo.m_BeginProcess = SVClock::GetTimeStamp();

	if( !AssignInputs( p_rInputValues ) )
	{
		p_pNewProduct->hrPPQStatus = -12387;
	}

	p_pNewProduct->oInputsInfo.m_EndProcess = SVClock::GetTimeStamp();

	// End reading the inputs
	// ************************************************************************

	return TRUE;
}// end IndexPPQ


// p_Offset represents PPQ position of product.
HRESULT SVPPQObject::NotifyInspections( long p_Offset )
{
	HRESULT l_Status = S_OK; 

	// ************************************************************************
	// Now we need to check if any inspections were waiting on inputs read after
	// the first position. Only check the products at positions that have a input set

	// Begin checking inspections to start processing
	if( 0 <= p_Offset && static_cast< size_t >( p_Offset ) < m_ppPPQPositions.size() )
	{
		SVProductInfoStruct* pTempProduct = m_ppPPQPositions.GetProductAt( p_Offset );

		// See if the Inspection Processes can inspect this product
		int iSize = m_arInspections.GetSize();

		bool l_Start = true;

		for( int i = 0; i < iSize; i++ )
		{
			SVInspectionInfoStruct& l_rInfo = pTempProduct->m_svInspectionInfos[ m_arInspections[ i ]->GetUniqueObjectID() ];
			bool tmpIsProductActive =	pTempProduct->IsProductActive();

			// if product has not been previously determined that it "can 
			// process".  That would generally mean that it's been here 
			// before, and should not be able to happen.
			if( !( l_rInfo.m_CanProcess ) && !( l_rInfo.m_InProcess ) && tmpIsProductActive)
			{
				// Tests if we have all required inputs.  Includes assigned 
				// discrete inputs and camera images.
				// If images or inputs are missing, CanProcess will return 
				// FALSE(0).  m_CanProcess will also equal FALSE.
				l_rInfo.m_CanProcess = ( m_arInspections[ i ]->CanProcess( pTempProduct ) != FALSE );

				if( l_rInfo.m_CanProcess )
				{
					if( pTempProduct->IsProductActive() )
					{
						m_ProcessInspectionsSet.insert( m_arInspections[ i ]->GetUniqueObjectID() );

						pTempProduct->m_ProductState += _T( "|CP=" );
						pTempProduct->m_ProductState += m_arInspections[ i ]->GetName();

						SVString l_Title = m_arInspections[ i ]->GetName();

						l_Title += _T( " CP" );
#ifdef EnableTracking
						m_PPQTracking.IncrementCount( l_Title, p_Offset );
#endif
					}
				} //	if( l_rInfo.m_CanProcess )
				else
				{
					// If it can not process, this still does not change the 
					// l_Status, so the returned status will still be S_OK
					// which indicates NOT "Processed".
					l_Status = S_FALSE; // 1
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

HRESULT SVPPQObject::StartInspection( const SVGUID& p_rInspectionID )
{
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
	CString infostring;
	infostring.Format(_T("!\t.%7.1lf: SVPPQObject::StartInspection(%s)\n"),SvTl::GetRelTimeStamp(),p_rInspectionID.ToString().c_str());
	::OutputDebugString(infostring);
#endif

	HRESULT l_Status = S_OK;

	SVProductInfoStruct* l_pProduct = nullptr;

	size_t l_ProductIndex = m_ppPPQPositions.size();
	size_t l_Count = m_ppPPQPositions.size();

	if( 0 < m_NAKCount )
	{
		long l_NAKCount = m_NAKCount;

		if( ( l_NAKCount + 2 ) < static_cast< long >( l_Count ) )
		{
			l_Count -= l_NAKCount;
		}
		else
		{
			l_Count = std::min< size_t >( 2, m_ppPPQPositions.size() );
		}

#ifdef _DEBUG
		SVString l_String = SvUl_SF::Format( _T( "%s:NAK=%ld\n" ), GetName(), l_NAKCount );

		::OutputDebugString( l_String.c_str() );
#endif
	}

	// Begin checking inspections to start processing
	for( size_t i = 0; i < l_Count; ++i )
	{
		SVProductInfoStruct* pTempProduct = m_ppPPQPositions.GetProductAt( i );

		if( nullptr != pTempProduct )
		{
			if( pTempProduct->IsProductActive() )
			{
				SVInspectionInfoStruct& l_rInfo = pTempProduct->m_svInspectionInfos[ p_rInspectionID ];

				if( l_rInfo.m_CanProcess &&				// all inputs are available and inspection can start
					!( l_rInfo.m_InProcess ) &&			// inspection is not currently running
					!( l_rInfo.m_HasBeenQueued ) )		// This flag prevents the inspection from getting queued more than once
				{
					l_pProduct = pTempProduct; // product info
					l_ProductIndex = i;

				}
			}
			else
			{
				break;
			}
		}
	}

	if( nullptr != l_pProduct && nullptr != l_pProduct->m_svInspectionInfos[ p_rInspectionID ].pInspection )
	{

		l_pProduct->m_WorkloadInfo.m_ProcessingStartTime = SVClock::GetTimeStamp(); //ProductWorkloadInformation may be incorrect if there are several inspections per product
#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information

		CString infostring;
		infostring.Format(_T("set m_ProcessingStartTime, trID = %ld"),l_pProduct->ProcessCount());

		logWorkloadInformation(l_pProduct->m_WorkloadInfo,infostring);
#endif

		l_Status = l_pProduct->m_svInspectionInfos[ p_rInspectionID ].pInspection->StartProcess( l_pProduct );

		SVString l_Title = l_pProduct->m_svInspectionInfos[ p_rInspectionID ].pInspection->GetName();

		l_Title += _T( " Start" );
#ifdef EnableTracking
		m_PPQTracking.IncrementCount( l_Title, l_ProductIndex );
#endif

#ifdef _DEBUG
		if( TheSVObserverApp.GetLogDataManager() )
		{
			SVString l_ProductStates;

			l_ProductStates += GetName();
			l_ProductStates += _T( "|SVPPQObject::StartInspection\n" );

			m_ppPPQPositions.GetProductStates( l_ProductStates );

			::OutputDebugString( l_ProductStates.c_str() );
		}
#endif
	}

	return l_Status;
}

BOOL SVPPQObject::StartOutputs( SVProductInfoStruct* p_pProduct )
{
	CString strDisplay;

	if( p_pProduct )
	{
		p_pProduct->oOutputsInfo.m_BeginProcess = SVClock::GetTimeStamp();
		p_pProduct->oOutputsInfo.lOutputDelay = m_lOutputDelay;
		p_pProduct->oOutputsInfo.lResetDelay  = m_lResetDelay;
		p_pProduct->oOutputsInfo.lDataValidDelay  = m_DataValidDelay;
		p_pProduct->oOutputsInfo.m_EndOutputDelay = SVClock::GetMinTimeStamp();
		p_pProduct->oOutputsInfo.m_EndResetDelay  = SVClock::GetMinTimeStamp();
		p_pProduct->oOutputsInfo.m_EndDataValidDelay  = SVClock::GetMinTimeStamp();

		switch( m_oOutputMode )
		{
		case SVPPQNextTriggerMode :
			{
				// The trigger has come, time to write the outputs
				WriteOutputs( p_pProduct );

				p_pProduct->oOutputsInfo.m_EndProcess = SVClock::GetTimeStamp();

				// Check if we should fire up the reset outputs thread
				if( 0 < m_lResetDelay )
				{
					// Set output reset expire time
					p_pProduct->oOutputsInfo.m_EndResetDelay = p_pProduct->oOutputsInfo.m_EndProcess + 
						p_pProduct->oOutputsInfo.lResetDelay;

					m_oOutputsResetQueue.AddTail( p_pProduct->ProcessCount() );
				}

				SetProductComplete( *p_pProduct );

				break;
			}
		case SVPPQTimeDelayMode :
		case SVPPQTimeDelayAndDataCompleteMode :
		case SVPPQExtendedTimeDelayMode :
		case SVPPQExtendedTimeDelayAndDataCompleteMode :
			{
				SVClock::SVTimeStamp l_Offset = p_pProduct->oOutputsInfo.lOutputDelay;

				// Set output delay expire time
				p_pProduct->oOutputsInfo.m_EndOutputDelay = p_pProduct->oTriggerInfo.m_ToggleTimeStamp + l_Offset;

				if( p_pProduct->oOutputsInfo.m_BeginProcess < p_pProduct->oOutputsInfo.m_EndOutputDelay )
				{
					m_oOutputsDelayQueue.AddTail( p_pProduct->ProcessCount() );
				}

				break;
			}
		default :
			DWORD dw = GetLastError();
			return FALSE;
			break;
		}// end switch

		// Now we need to write the inputs. Make sure that we record the timestamps
		// ************************************************************************
		// End writing the outputs
	}

	return TRUE;
}// end StartOutputs

bool SVPPQObject::AddResultsToPPQ( long p_PPQIndex )
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt( p_PPQIndex );

	bool bValid = ( nullptr != pProduct );

	if( bValid )
	{
		long lIndex = -1;
		long oState = 0x0000;

		SVGUIDSVInspectionInfoStructMap::iterator l_Iter = pProduct->m_svInspectionInfos.begin();

		while( l_Iter != pProduct->m_svInspectionInfos.end() )
		{
			oState |= l_Iter->second.oInspectedState;

			++l_Iter;
		}// end for

		lIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

		// Set the value objects depending on the inspection state result
		if( PRODUCT_INSPECTION_NOT_RUN & oState )
		{
			if( PRODUCT_NOT_INSPECTED & oState )
			{
				m_voACK.SetValue( lIndex, FALSE );
				m_voNAK.SetValue( lIndex, TRUE );
				m_voMasterFault.SetValue( lIndex, TRUE );
				m_voMasterWarning.SetValue( lIndex, TRUE );
				m_voNotInspected.SetValue( lIndex, TRUE );

				pProduct->m_ProductState += _T( "|NI" );
			}// end if
			else if( PRODUCT_NOT_ACKNOWLEDGED & oState )
			{
				m_voACK.SetValue( lIndex, FALSE );
				m_voNAK.SetValue( lIndex, TRUE );
				m_voMasterFault.SetValue( lIndex, TRUE );
				m_voMasterWarning.SetValue( lIndex, TRUE );
				m_voNotInspected.SetValue( lIndex, TRUE );

				pProduct->m_ProductState += _T( "|NAK" );
			}// end else if
			else if( PRODUCT_INSPECTION_DISABLED & oState )
			{
				m_voACK.SetValue( lIndex, TRUE );
				m_voNAK.SetValue( lIndex, FALSE );
				m_voMasterFault.SetValue( lIndex, TRUE );
				m_voMasterWarning.SetValue( lIndex, TRUE );
				m_voNotInspected.SetValue( lIndex, TRUE );

				pProduct->m_ProductState += _T( "|DISABLED" );
			}// end else if
			else
			{
				m_voACK.SetValue( lIndex, FALSE );
				m_voNAK.SetValue( lIndex, TRUE );
				m_voMasterFault.SetValue( lIndex, TRUE );
				m_voMasterWarning.SetValue( lIndex, TRUE );
				m_voNotInspected.SetValue( lIndex, TRUE );

				pProduct->m_ProductState += _T( "|E_UNKNOWN" );
			}// end else
		}// end if
		else
		{
			if( PRODUCT_INSPECTION_FAILED & oState )
			{
				m_voACK.SetValue( lIndex, TRUE );
				m_voNAK.SetValue( lIndex, FALSE );
				m_voMasterFault.SetValue( lIndex, TRUE );
				m_voMasterWarning.SetValue( lIndex, TRUE );
				m_voNotInspected.SetValue( lIndex, FALSE );

				pProduct->m_ProductState += _T( "|FAILED" );
			}// end if
			else if( PRODUCT_INSPECTION_WARNING & oState )
			{
				m_voACK.SetValue( lIndex, TRUE );
				m_voNAK.SetValue( lIndex, FALSE );
				m_voMasterFault.SetValue( lIndex, FALSE );
				m_voMasterWarning.SetValue( lIndex, TRUE );
				m_voNotInspected.SetValue( lIndex, FALSE );

				pProduct->m_ProductState += _T( "|WARNING" );
			}// end else if
			else if( PRODUCT_INSPECTION_PASSED & oState )
			{
				m_voACK.SetValue( lIndex, TRUE );
				m_voNAK.SetValue( lIndex, FALSE );
				m_voMasterFault.SetValue( lIndex, FALSE );
				m_voMasterWarning.SetValue( lIndex, FALSE );
				m_voNotInspected.SetValue( lIndex, FALSE );

				pProduct->m_ProductState += _T( "|PASSED" );
			}// end else if
			else
			{
				m_voACK.SetValue( lIndex, FALSE );
				m_voNAK.SetValue( lIndex, TRUE );
				m_voMasterFault.SetValue( lIndex, TRUE );
				m_voMasterWarning.SetValue( lIndex, TRUE );
				m_voNotInspected.SetValue( lIndex, TRUE );

				pProduct->m_ProductState += _T( "|S_UNKNOWN" );
			}// end else
		}// end else

		SVGUIDSVInspectionInfoStructMap::iterator l_InspectIter = pProduct->m_svInspectionInfos.begin();

		while( l_InspectIter != pProduct->m_svInspectionInfos.end() )
		{
			bValid &= ( l_InspectIter->second.m_EndInspection > 0 );

			++l_InspectIter;
		}// end for

		// Set the Data Valid to true
		m_voDataValid.SetValue( lIndex, bValid );
	}

	return TRUE;
}// end AddResultsToPPQ

bool SVPPQObject::SetInspectionComplete( long p_PPQIndex )
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt( p_PPQIndex );
	bool bValid = ( nullptr != pProduct );

	if( bValid )
	{
		SVGUIDSVInspectionInfoStructMap::iterator l_Iter = pProduct->m_svInspectionInfos.begin();

		while( l_Iter != pProduct->m_svInspectionInfos.end() )
		{
			bool l_Complete = ( l_Iter->second.m_EndInspection > 0 );

			bValid &= l_Complete;

			++l_Iter;
		}
	}

	if( bValid )
	{
		// Currently only used for Remote Outputs and Fail Status Stream.
		// returns E_FAIL when there are no listeners/observers.  Not having 
		// Remote Outputs or Fail Status is not an error in this case.
		SVObjectManagerClass::Instance().UpdateObservers( SVString( SvO::cPPQObjectTag ), GetUniqueObjectID(), *pProduct );

	}

	if( bValid )
	{
		bValid = AddResultsToPPQ( p_PPQIndex );
	}

	//Only place that could set bDataComplete to true
	pProduct->bDataComplete = bValid;

	if( pProduct->bDataComplete )
	{
		if( ( SVPPQTimeDelayAndDataCompleteMode == m_oOutputMode ) ||
			( SVPPQExtendedTimeDelayAndDataCompleteMode == m_oOutputMode ) )
		{
			NotifyProcessTimerOutputs();
		}
	}

	return bValid;
}

bool SVPPQObject::SetProductComplete( long p_PPQIndex )
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt( p_PPQIndex );

	bool l_Status = false;

	if( nullptr != pProduct )
	{
		// record the current time and PPQ position for later display
		pProduct->m_WorkloadInfo.m_CompletionTime = SVClock::GetTimeStamp();
		pProduct->m_WorkloadInfo.m_PPQIndexAtCompletion = p_PPQIndex;

		pProduct->m_WorkloadInfo.m_TriggerTime = pProduct->oTriggerInfo.m_BeginProcess;

#ifdef _DEBUG_PERFORMANCE_INFO //Arvid 160212 this is helpful for debugging the creation of Performance Information
		CString infostring;
		infostring.Format(_T("SVPPQObject::SetProductComplete(@ppq: %d) >> m_CompletionTime\n\t\t(oTriggerInfo.m_BeginProcess -> m_TriggerTime)"),
			p_PPQIndex);
		logWorkloadInformation(pProduct->m_WorkloadInfo,infostring);
#endif

		m_MostRecentWorkLoadInfo = pProduct->GetWorkloadInformation();

		CommitSharedMemory(*pProduct);
		l_Status = SetProductComplete( *pProduct );


#ifdef EnableTracking
		m_PPQTracking.IncrementCount( _T( "Product Complete" ), p_PPQIndex );
#endif
	}

	return l_Status;
}

bool SVPPQObject::SetProductComplete( SVProductInfoStruct& p_rProduct )
{
	bool l_Status = true;

	if( m_ppPPQPositions.IsProductAlive( p_rProduct.ProcessCount() ) )
	{
		::InterlockedExchange( &m_NAKCount, 0 );
	}

	bool bMaintainImages;

	// if bInspecting is FALSE, this loop will fall through
	// if we are maintaining source images it will also fall through
	// we don't want to release all the source images unless all the IPs have started
	GetMaintainSourceImages( bMaintainImages );

	if( ! bMaintainImages )
	{
		SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svIter;

		l_svIter = p_rProduct.m_svCameraInfos.begin();

		while( l_svIter != p_rProduct.m_svCameraInfos.end() )
		{
			l_svIter->second.ClearCameraInfo();

			++l_svIter;
		}

		p_rProduct.oPPQInfo.m_ResultImagePublishedDMIndexHandle.clear();
	}

	p_rProduct.SetProductComplete();

	p_rProduct.m_ProductState += _T( "|COMPLETE" );

	return l_Status;
}

bool SVPPQObject::SetProductIncomplete( long p_PPQIndex )
{
	SVProductInfoStruct* pProduct = m_ppPPQPositions.GetProductAt( p_PPQIndex );
	bool l_Status = ( nullptr != pProduct );

	if( l_Status )
	{
		// Release from Shared Memory
		ReleaseSharedMemory(*pProduct); 
		l_Status = SetProductIncomplete( *pProduct );

#ifdef EnableTracking
		m_PPQTracking.IncrementCount( _T( "Product Incomplete" ), p_PPQIndex );
#endif
	}

	return l_Status;
}

bool SVPPQObject::SetProductIncomplete( SVProductInfoStruct& p_rProduct )
{
	bool l_Status = true;

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svIter;

	l_svIter = p_rProduct.m_svCameraInfos.begin();

	while( l_svIter != p_rProduct.m_svCameraInfos.end() )
	{
		l_svIter->second.ClearCameraInfo();

		++l_svIter;
	}

	p_rProduct.oPPQInfo.m_ResultImagePublishedDMIndexHandle.clear();

	p_rProduct.SetProductComplete();

	p_rProduct.m_ProductState += _T( "|INCOMPLETE" );

	return l_Status;
}

BOOL SVPPQObject::RecycleProductInfo( SVProductInfoStruct *pProduct )
{
	BOOL l_Status = false;

	if( nullptr != pProduct )
	{
		if( -1 < pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex() )
		{
			m_voNotInspected.SetValue( pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
			m_voNAK.SetValue( pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
			m_voMasterWarning.SetValue( pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
			m_voMasterFault.SetValue( pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), TRUE );
			m_voDataValid.SetValue( pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), FALSE );
			m_voACK.SetValue( pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), FALSE );
		}

		pProduct->InitProductInfo();
	}

	l_Status = m_qAvailableProductInfos.AddTail( pProduct );

	return l_Status;
}// end RecycleProductInfo

HRESULT SVPPQObject::ProcessCameraResponse( const SVCameraQueueElement& p_rElement )
	/// Either gets the appropriate product information (if available) or increments the pending image indicator.
	/// If product information is available: 
	/// Adds the process count to the cameras queue and signals the asynchronous procedure
{
	HRESULT l_Status = S_OK;

	if( ( nullptr != p_rElement.m_pCamera ) && ( 0 <= p_rElement.m_Data.mDMHandle.GetIndex() ) )
	{
		SVClock::SVTimeStamp l_StartTick = 0;
		size_t l_CameraPositionOnPPQ = -1;
		size_t l_ProductIndex = -1;
		SVProductInfoStruct* l_pProduct = nullptr;
		SVCameraInfoMap::iterator l_svIter;
		long	ppqSize = static_cast<long> (m_ppPPQPositions.size());

		l_svIter = m_Cameras.find( p_rElement.m_pCamera );

		if( l_svIter != m_Cameras.end() )
		{
			l_CameraPositionOnPPQ = l_svIter->second.m_CameraPPQIndex;
		}

		p_rElement.m_Data.GetStartTick( l_StartTick );

		if( l_CameraPositionOnPPQ < ppqSize )
		{
			long l_Position = -1; // gets initialized to ppq size (default) by 
			// GetProductIndex (). 

			// Attempts for find the trigger (which may not have happened 
			// yet) that correlates to this image, based on image time stamp.
			GetProductIndex( l_Position, l_StartTick );

			// If trigger has not occurred yet, l_Position will equal 0.
			if( 0 <= l_Position && l_Position < ppqSize )
			{
				size_t l_Index = l_Position + l_CameraPositionOnPPQ;

				if( l_Index < ppqSize )
				{
					l_ProductIndex = l_Index;
					l_pProduct = m_ppPPQPositions.GetProductAt( l_Index );
				}
			}
			else if( l_Position < 0 ) // l_Position can never be less than 0??
			{
				m_PendingCameraResponses[ p_rElement.m_pCamera ] = p_rElement;

				SVObjectManagerClass::Instance().IncrementPendingImageIndicator();

				SVString l_Title = _T( "Pending " );

				l_Title += p_rElement.m_pCamera->GetName();

#ifdef EnableTracking
				m_PPQTracking.IncrementCount( l_Title );
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

		if( nullptr != l_pProduct )
		{
			SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svIter1;

			l_svIter1 = l_pProduct->m_svCameraInfos.find( p_rElement.m_pCamera );

			if( l_svIter1 != l_pProduct->m_svCameraInfos.end() )
			{
				SVClock::SVTimeStamp	priorCameraSF = l_svIter1->second.m_StartFrameTimeStamp;
				long					productComplete = l_pProduct->bDataComplete;

				// Attempting to make sure we don't have the previous trigger 
				// count where the image has already been assigned (and 
				// possibly inspected).  This scenario occurs when the Camera
				// Response Queue notification arrives before the Trigger 
				// Queue notification.
				if (( priorCameraSF == 0 ) && (1 != productComplete))
				{
					SVClock::SVTimeStamp iEF=0;

					p_rElement.m_Data.GetEndTick( iEF );

					if( p_rElement.m_Data.IsComplete() )
					{
						l_svIter1->second.Assign( l_StartTick, iEF, p_rElement.m_Data.mDMHandle, SV_PPQ );
					}
					else
					{
						l_svIter1->second.ClearInfo();
					}

					l_pProduct->m_ProductState += _T( "|" );
					l_pProduct->m_ProductState += p_rElement.m_pCamera->GetName();

					if( l_svIter1->second.GetIndex() < 0 )
					{
						l_pProduct->m_ProductState += _T( "=NAK" );

#ifdef EnableTracking
						SVString l_Title = p_rElement.m_pCamera->GetName();

						l_Title += _T( " Index NAK" );

						m_PPQTracking.IncrementCount( l_Title );
						m_PPQTracking.IncrementCount( l_Title, l_ProductIndex );
#endif
					}
					else
					{
#ifdef EnableTracking
						m_PPQTracking.IncrementCount( p_rElement.m_pCamera->GetName(), l_ProductIndex );
#endif
					}

					for( size_t i = ( l_ProductIndex + 1 ); i < m_ppPPQPositions.size(); ++i )
					{
						SVProductInfoStruct* l_pAcqProduct = m_ppPPQPositions.GetProductAt( i );

						if( nullptr != l_pAcqProduct )
						{
							SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_Iter = l_pAcqProduct->m_svCameraInfos.find( p_rElement.m_pCamera );

							if( l_Iter != l_pAcqProduct->m_svCameraInfos.end()  )
							{
								if( l_Iter->second.m_CallbackTimeStamp != 0 )
								{
									break;
								}
								l_Iter->second.m_CallbackTimeStamp = SVClock::GetTimeStamp();
							}
#ifdef EnableTracking
							SVString l_Title = p_rElement.m_pCamera->GetName();

							l_Title += _T( " NAK" );

							m_PPQTracking.IncrementCount( l_Title );
							m_PPQTracking.IncrementCount( l_Title, l_ProductIndex );
#endif

							MarkProductInspectionsMissingAcquisiton( *l_pAcqProduct, p_rElement.m_pCamera );
						}
					}

					l_svIter1->second.m_CallbackTimeStamp  = SVClock::GetTimeStamp();

					if( m_oCamerasQueue.AddTail( l_pProduct->ProcessCount() ) )
					{
						m_AsyncProcedure.Signal( nullptr );
					}
					else
					{
						l_Status = E_FAIL;
					}
				} // if (( priorCameraSF == 0 ) && (1 != productComplete))
				else
				{

					// trying to use the wrong Trigger Count.  
					// Trigger even for this camera image has not 
					// occurred yet.
					m_PendingCameraResponses[ p_rElement.m_pCamera ] = p_rElement;

					SVObjectManagerClass::Instance().IncrementPendingImageIndicator();

#ifdef EnableTracking
					SVString l_Title = _T( "Pending " );
					l_Title += p_rElement.m_pCamera->GetName();
					m_PPQTracking.IncrementCount( l_Title );
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

HRESULT SVPPQObject::BuildCameraInfos( SVStdMapSVVirtualCameraPtrSVCameraInfoStruct& p_rCameraInfos ) const
{
	HRESULT l_Status = S_OK;

	p_rCameraInfos.clear();

	SVCameraInfoMap::const_iterator l_Iter = m_Cameras.begin();

	while( l_Iter != m_Cameras.end() )
	{
		if( -1 != l_Iter->second.m_CameraPPQIndex )
		{
			p_rCameraInfos[ l_Iter->first ].pCamera = l_Iter->first;
		}

		++l_Iter;
	}

	return l_Status;
}

BOOL SVPPQObject::FinishCamera( void *pCaller, SVODataResponseClass *pResponse )
{
	BOOL l_Status = ( m_bOnline && ( nullptr != pCaller ) && ( nullptr != pResponse ) && ( 0 <= pResponse->mDMHandle.GetIndex() ) );

	if( l_Status )
	{
		SVVirtualCamera* pCamera = reinterpret_cast< SVVirtualCamera* >( pCaller );

		if( nullptr != pCamera && m_CameraResponseQueue.AddTail( SVCameraQueueElement( pCamera, *pResponse ) ) )
		{
			m_AsyncProcedure.Signal( nullptr );
		}
	}

	return l_Status;
}

BOOL SVPPQObject::FinishTrigger( void *pCaller, SvTh::SVTriggerInfoStruct& p_rTriggerInfo )
{
	BOOL l_Status = m_bOnline;

	if( l_Status )
	{
		SVTriggerQueueElement l_TriggerInfo;

		l_TriggerInfo.m_TriggerInfo = p_rTriggerInfo;

		l_TriggerInfo.m_TriggerInfo.m_CallbackReceived = SVClock::GetTimeStamp();

		GetInputIOValues( l_TriggerInfo.m_Inputs );

		switch( m_oOutputMode )
		{
		case SVPPQTimeDelayMode :
		case SVPPQTimeDelayAndDataCompleteMode :
			{
				ResetOutputs();
				break;
			}
		}

		m_TriggerToggle = ! m_TriggerToggle;

		if( !( m_pTriggerToggle.empty() ) && m_pTriggerToggle->m_pValueObject )
		{
			m_pOutputList->WriteOutputValue( m_pTriggerToggle, m_TriggerToggle );
		}

		l_TriggerInfo.m_TriggerInfo.m_ToggleState = m_TriggerToggle;
		l_TriggerInfo.m_TriggerInfo.m_ToggleTimeStamp = SVClock::GetTimeStamp();

		// Caution! enabling the logging here will cause thread contention because
		// the tracking class is not lock-less. It needs more work before we can use it here.
#ifdef EnableTracking
		//long l_lTime = static_cast<long>( l_TriggerInfo.m_TriggerInfo.m_ToggleTimeStamp - l_TriggerInfo.m_TriggerInfo.m_BeginProcess );
		//m_PPQTracking.IncrementTimeCount( _T( "Trigger Toggle"), l_lTime );
#endif

		m_oTriggerQueue.PushTail( l_TriggerInfo );

		m_AsyncProcedure.Signal( nullptr );
	}

	return l_Status;
}

bool SVPPQObject::IsProductAlive( long p_ProductCount ) const
{
	bool l_Status = ( S_OK == m_ppPPQPositions.IsProductAlive( p_ProductCount ) );

	return l_Status;
}

HRESULT SVPPQObject::GetNextAvailableIndexes( SVPPQInfoStruct& p_rPPQInfo, SVDataManagerLockTypeEnum p_LockType ) const
{
	HRESULT l_Status = S_OK;
	HRESULT l_Temp = S_OK;

	l_Temp = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultDataCircleBuffer, p_LockType, p_rPPQInfo.m_ResultDataDMIndexHandle );

	if( S_OK == l_Status  )
	{
		l_Status = l_Temp;
	}

	l_Temp = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultImagePublishedCircleBuffer, p_LockType, p_rPPQInfo.m_ResultImagePublishedDMIndexHandle );

	if( S_OK == l_Status  )
	{
		l_Status = l_Temp;
	}

	return l_Status;
}

HRESULT SVPPQObject::GetProductIndex( long& p_rIndex, long lProcessCount ) const
{
	HRESULT l_Status = m_ppPPQPositions.GetIndexByTriggerCount( p_rIndex, lProcessCount );

	return l_Status;
}

HRESULT SVPPQObject::GetProductIndex( long& p_rIndex, SVClock::SVTimeStamp p_TimeStamp ) const
{
	HRESULT l_Status = S_OK;

	bool l_SkipUpperThreshold = false;

	l_SkipUpperThreshold = l_SkipUpperThreshold || ( GetPPQLength() == 1 );
	l_SkipUpperThreshold = l_SkipUpperThreshold || ( m_oOutputMode == SVPPQExtendedTimeDelayAndDataCompleteMode );
	l_SkipUpperThreshold = l_SkipUpperThreshold || ( ( m_oOutputMode == SVPPQNextTriggerMode ) && ( GetPPQLength() == 2 ) );

	if( l_SkipUpperThreshold )
	{
		l_Status = m_ppPPQPositions.GetIndexByTriggerTimeStamp( p_rIndex, p_TimeStamp, -2 );
	}
	else
	{
		l_Status = m_ppPPQPositions.GetIndexByTriggerTimeStamp( p_rIndex, p_TimeStamp, -2, 73 );
	}

	return l_Status;
}

HRESULT SVPPQObject::GetProductInfoStruct( SVProductInfoStruct*& p_rpProduct, long lProcessCount ) const
{
	HRESULT l_Status = m_ppPPQPositions.GetProductByTriggerCount( p_rpProduct, lProcessCount );

	return l_Status;
}

HRESULT SVPPQObject::GetProductInfoStruct( SVProductInfoStruct*& p_rpProduct, SVClock::SVTimeStamp p_TimeStamp ) const
{
	HRESULT l_Status = S_OK;
	long l_Index = -1;

	p_rpProduct = nullptr;

	l_Status = GetProductIndex( l_Index, p_TimeStamp );

	if( 0 <= l_Index && l_Index < static_cast< long >( m_ppPPQPositions.size() ) )
	{
		p_rpProduct = m_ppPPQPositions.GetProductAt( l_Index );
	}
	else
	{
		if( S_OK == l_Status )
		{
			l_Status = S_FALSE;
		}
	}

	return l_Status;
}

BOOL SVPPQObject::ReserveNextRunOnceProductInfoStruct( SVProductInfoStruct& p_rsvProduct, SVDataManagerLockTypeEnum p_LockType )
{
	BOOL l_bOk = TRUE;

	SVProductInfoStruct l_svProduct;

	l_svProduct.oPPQInfo.pPPQ = this;

	l_svProduct.oTriggerInfo.pTrigger = m_pTrigger;

	BuildCameraInfos( l_svProduct.m_svCameraInfos );

	l_svProduct.m_svInspectionInfos.clear();

	int iSize = m_arInspections.GetSize();
	for( int i = 0; i < iSize; i++ )
	{
		SVInspectionInfoStruct l_svInspectionStruct;

		l_svInspectionStruct.pInspection = m_arInspections[i];

		l_svProduct.m_svInspectionInfos[ m_arInspections[i]->GetUniqueObjectID() ] = l_svInspectionStruct;
	}// end for

	l_bOk = ( S_OK == l_svProduct.GetNextAvailableIndexes( p_LockType )  );

	if( l_bOk )
	{
		p_rsvProduct = l_svProduct;
	}

	return l_bOk;
}

HRESULT SVPPQObject::GetInspections( std::vector< SVInspectionProcess* >& rvecInspections ) const
{
	std::copy( m_arInspections.begin(), m_arInspections.end(), std::inserter( rvecInspections, rvecInspections.begin() ) );

	return S_OK;
}

bool SVPPQObject::IsObjectInPPQ( const SVObjectClass& object ) const
{
	bool retValue = false;
	const SVObjectClass *inspectObject = object.GetAncestor(SVInspectionObjectType);

	if (nullptr != inspectObject)
	{
		for (SVPPQInspectionProcessVector::const_iterator it = m_arInspections.begin(); it != m_arInspections.end(); ++it)
		{
			if (inspectObject == (*it))
			{
				return true;
			}
		}
	}

	return retValue;
}

bool SVPPQObject::IsProductExpired( const SVProductInfoStruct* pProduct ) const
{
	if ( m_oOutputMode == SVPPQExtendedTimeDelayMode && m_lInspectionTimeoutMillisec > 0 )
	{
		SVClock::SVTimeStamp l_CurrentTimestamp = SVClock::GetTimeStamp();
		SVClock::SVTimeStamp l_ProductTimestamp = pProduct->oTriggerInfo.m_BeginProcess;
		return ( ( l_CurrentTimestamp - l_ProductTimestamp ) > static_cast< double >( m_lOutputDelay - m_lInspectionTimeoutMillisec ) );
	}
	else
	{
		return false;
	}
}

void SVPPQObject::DumpDMInfo( LPCTSTR p_szName ) const
{
	// Camera Info Struct - Source Image Circle Buffer Just the first Camera....
	SVCameraInfoMap::const_iterator l_Iter;

	l_Iter = m_Cameras.begin();

	while( l_Iter != m_Cameras.end() )
	{
		l_Iter->first->DumpDMInfo( p_szName );

		++l_Iter;
	}

	if( ! m_pResultDataCircleBuffer.empty() )
	{
		m_pResultDataCircleBuffer->Dump( p_szName );
	}

	if( ! m_pResultImagePublishedCircleBuffer.empty() )
	{
		m_pResultImagePublishedCircleBuffer->Dump( p_szName );
	}

	for( long i = 0; i < m_arInspections.GetSize(); ++i )
	{
		m_arInspections[ i ]->DumpDMInfo( p_szName );
	}
}

void CALLBACK SVPPQObject::OutputTimerCallback( UINT uTimerID, UINT uRsvd, DWORD_PTR dwUser, DWORD_PTR dwRsvd1, DWORD_PTR dwRsvd2 )
{
	SVPPQObject* pPPQ = reinterpret_cast< SVPPQObject* >( dwUser );

	if( nullptr != pPPQ )
	{
		if( pPPQ->m_AsyncProcedure.IsActive() )
		{
			pPPQ->NotifyProcessTimerOutputs();
		}
	}
}

HRESULT SVPPQObject::MarkProductInspectionsMissingAcquisiton( SVProductInfoStruct& p_rProduct, SVVirtualCamera* pCamera )
{
	HRESULT l_Status = S_OK;

	if( nullptr != pCamera )
	{
		int iSize = m_arInspections.GetSize();

		for( int i = 0; i < iSize; i++ )
		{
			SVInspectionProcess* pInspection = m_arInspections[ i ];

			if( nullptr != pInspection && pInspection->IsCameraInInspection( pCamera->GetName() ) )
			{
				SVInspectionInfoStruct l_InspectInfo = p_rProduct.m_svInspectionInfos[ pInspection->GetUniqueObjectID() ];

				l_InspectInfo.oInspectedState = PRODUCT_NOT_INSPECTED;
				l_InspectInfo.m_CanProcess = false;
				l_InspectInfo.m_InProcess = true;

				l_InspectInfo.m_EndInspection  = SVClock::GetTimeStamp();

				SVInspectionInfoPair l_Info( p_rProduct.ProcessCount(), l_InspectInfo );

				p_rProduct.m_ProductState += _T( "|MC=" );
				p_rProduct.m_ProductState += m_arInspections[ i ]->GetName();
				p_rProduct.m_ProductState += _T( "-" );
				p_rProduct.m_ProductState += pCamera->GetName();

				m_oInspectionQueue.AddTail( l_Info );
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void CALLBACK SVPPQObject::APCThreadProcess( DWORD_PTR dwParam )
{
}

void SVPPQObject::ThreadProcess( bool& p_WaitForEvents )
{
	bool l_Processed = false;

	// will only execute if 0 < m_oTriggerQueue.size().
	// will then pop trigger queue.
	// pushes onto notify inspection queue.
	ProcessTrigger( l_Processed );

	if( ! l_Processed )
	{
		ProcessDelayOutputs( l_Processed );
	}

	if( ! l_Processed )
	{
		ProcessResetOutputs( l_Processed );
	}

	if( ! l_Processed )
	{
		ProcessDataValidDelay( l_Processed );
	}

	if( ! l_Processed )
	{
		// Inserts items onto the m_oCamerasQueue.
		ProcessCameraResponses( l_Processed );
	}

	if( ! l_Processed )
	{
		// will only execute if 0 == m_oTriggerQueue.size().
		// pushes onto notify inspection queue.
		// The only function that can place items onto the m_oCameraQueue is
		// ProcessCameraResponses.
		ProcessCameraInputs( l_Processed );
	}

	if( ! l_Processed )
	{
		// will only execute if 0 == m_oTriggerQueue.size().
		ProcessNotifyInspections( l_Processed );
	}

	if( ! l_Processed )
	{
		// will only execute if 0 == m_oTriggerQueue.size().
		ProcessInspections( l_Processed );
	}

	if( ! l_Processed )
	{
		ProcessCompleteInspections( l_Processed );
	}

	if( ! l_Processed )
	{
		ProcessProductRequests( l_Processed );
	}

	p_WaitForEvents = ! l_Processed;
}

HRESULT SVPPQObject::NotifyProcessTimerOutputs()
{
	HRESULT l_Status = S_OK;

	bool l_Process = false;

	if( 0 < m_lOutputDelay && ::InterlockedCompareExchange( &( m_ProcessingOutputDelay ), 1, 0 ) == 0 )
	{
		SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

		if( 0 < m_oOutputsDelayQueue.GetCount() || ( 0 < m_NextOutputDelayTimestamp && m_NextOutputDelayTimestamp <= l_CurrentTime ) )
		{
			l_Process = true;
		}
		else
		{
			::InterlockedExchange( &( m_ProcessingOutputDelay ), 0 );
		}
	}

	if( 0 < m_lResetDelay && ::InterlockedCompareExchange( &( m_ProcessingOutputReset ), 1, 0 ) == 0 )
	{
		SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

		if( 0 < m_oOutputsResetQueue.GetCount() || ( 0 < m_NextOutputResetTimestamp && m_NextOutputResetTimestamp <= l_CurrentTime ) )
		{
			l_Process = true;
		}
		else
		{
			::InterlockedExchange( &( m_ProcessingOutputReset ), 0 );
		}
	}

	if( 0 < m_DataValidDelay && ::InterlockedCompareExchange( &( m_ProcessingDataValidDelay ), 1, 0 ) == 0 )
	{
		SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

		if( 0 < m_DataValidDelayQueue.GetCount() || ( 0 < m_NextDataValidDelayTimestamp && m_NextDataValidDelayTimestamp <= l_CurrentTime ) )
		{
			l_Process = true;
		}
		else
		{
			::InterlockedExchange( &( m_ProcessingDataValidDelay ), 0 );
		}
	}

	if( l_Process )
	{
		m_AsyncProcedure.Signal( nullptr );
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessTrigger( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	/// Works through the queued triggers (e.g., pushed by FinishTrigger()).
	/// This is done by taking a SVTriggerQueueElement from the head of m_oTriggerQueue 
	/// and using the information in the SVTriggerInfoStruct therein to "create" a new product
	/// (the ProcessCount of which will be added to m_oNotifyInspectionsSet).
	/// In NextTriggerMode the results of the oldest product will be output through IndexPPQ().
	/// m_CameraInputData will contain trigger-data including the data index 
	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( p_rProcessed )
	{
		SVTriggerQueueElement l_TriggerInfo; ///would it not be preferable to call this variable "QueueElement"?

		if( S_OK == m_oTriggerQueue.PopHead( l_TriggerInfo )  )
		{
			if( m_bOnline )
			{
				SVProductInfoStruct* pProduct = IndexPPQ( l_TriggerInfo.m_TriggerInfo );

				if( nullptr != pProduct )
				{
					if( InitializeProduct( pProduct, l_TriggerInfo.m_Inputs ) )
					{
						long lDataIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

						m_voTriggerCount.SetValue( lDataIndex, l_TriggerInfo.m_TriggerInfo.lTriggerCount );

						m_oNotifyInspectionsSet.insert( pProduct->ProcessCount() );

						if (!l_TriggerInfo.m_TriggerInfo.m_Data.empty())
						{
							m_CameraInputData.Set(lDataIndex, boost::any_cast<SvTh::SVCameraTriggerData::NameVariantMap>(l_TriggerInfo.m_TriggerInfo.m_Data));
						}
						// Get Shared Memory Slot
						if (HasActiveMonitorList())
						{
							try
							{
								SvSml::SVSharedPPQWriter& rWriter = SVSharedMemorySingleton::Instance().GetPPQWriter(GetUniqueObjectID());
								long idx = 0;
								SvSml::SVSharedProduct& rSharedProduct = rWriter.RequestNextProductSlot(idx);
								rSharedProduct.m_TriggerCount = pProduct->ProcessCount();
								pProduct->m_lastInspectedSlot = idx;
								std::for_each(pProduct->m_svInspectionInfos.begin(), pProduct->m_svInspectionInfos.end(), 
									[&idx](SVGUIDSVInspectionInfoStructMap::value_type& value) { value.second.m_lastInspectedSlot = idx; } );
							}
							catch (const std::exception& e)
							{
								SVStringArray msgList;
								msgList.push_back(e.what());
								SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
								Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ProcessTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15026 );
							}
							catch (...)
							{
								SVStringArray msgList;
								msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_Unknown));
								SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
								Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ProcessTrigger, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15027 );
							}
						} // if (HasActiveMonitorList())
					}
				}
			} // if( m_bOnline )
		} // if( S_OK == m_oTriggerQueue.PopHead( l_TriggerInfo )  )
		else
		{
			l_Status = E_FAIL;
		}
	} // if( p_rProcessed )

	return l_Status;
}

HRESULT SVPPQObject::ProcessNotifyInspections( bool& p_rProcessed )
{
	/// Does nothing if the trigger queue is nonempty or the m_oNotifyInspectionsSet is empty.
	/// Otherwise works through m_oNotifyInspectionsSet trying to notify one (and only one) inspection.
	HRESULT l_Status = S_OK;

	size_t	triggerQueueSize = m_oTriggerQueue.size();

	p_rProcessed = ( 0 <  triggerQueueSize );

	if( ! p_rProcessed ) // if there are no triggers remaining on the queue.
	{

		// Currently, 
		// SVPPQOjbect::AssignInputs () - called from ProcessTrigger (),
		// SVPPQOjbect::ProcessTrigger (), and
		// SVPPQOjbect::ProcessCameraInputs ()
		// can add items to m_oNotifyInspectionSet.
		p_rProcessed = ( 0 < m_oNotifyInspectionsSet.size() );

		if( p_rProcessed ) // if there is a request to notify.
		{
			// offset is from the beginning of the PPQ.  
			long l_Offset = -1;
			p_rProcessed = false;

			SVProcessCountSet::iterator l_Iter( m_oNotifyInspectionsSet.begin() );

			// if the offset is outside the range of the PPQ, then execute 
			// this while statement.  The offset is initially forced outside 
			// the range in order to force the first execution.  
			do 
			{
				long l_ProcessCount = *l_Iter;

				m_ppPPQPositions.GetIndexByTriggerCount( l_Offset, l_ProcessCount );

				p_rProcessed = ( 0 <= l_Offset && l_Offset < static_cast< long >( m_ppPPQPositions.size() ) );

				if (true == p_rProcessed) // if offset for element is within PPQ.
				{
					l_Status = NotifyInspections( l_Offset );

					if (S_OK == l_Status)
					{
						// event has been placed on the inspection queue and
						// should be removed from the NotifyInspectionQueue.
						// Processed is already set to true.
						l_Iter = m_oNotifyInspectionsSet.erase( l_Iter );
					}
					else if (S_FALSE == l_Status)
					{
						// this means that inspection was not notified 
						// because inputs or image were not ready.  
						// proceed to next inspection.
						l_Iter++;
						p_rProcessed = false;
						l_Status = S_OK;
					}
				}
				else // of offset for element is outside PPQ (fell off/dead).
				{
					// product is dead and process should be killed here 
					// so as to now waste inspection time.
					// processed is already false.
					l_Iter = m_oNotifyInspectionsSet.erase( l_Iter );
				}

			} while ((true != p_rProcessed) && 
				 (l_Iter != m_oNotifyInspectionsSet.end ()) &&
				 SUCCEEDED (l_Status));


		} // if there is an item on the notify inspection queue.

	} // if there are no triggers on trigger event queue.

	return l_Status;
}

HRESULT SVPPQObject::ProcessInspections( bool& p_rProcessed )
	/// Does nothing if there is at least one trigger in the trigger queue.
	/// Otherwise, extracts all the GUIDs from m_ProcessInspectionsSet and starts the 
	/// corresponding inspections via StartInspection().
	/// They will be completed asynchronously in the class SVInspectionProcess.
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_ProcessInspectionsSet.size() );

		if( p_rProcessed )
		{
			SVInspectionIDSet::iterator l_Iter( m_ProcessInspectionsSet.begin() );

			while( l_Iter != m_ProcessInspectionsSet.end() )
			{
				StartInspection( *l_Iter );
				m_ProcessInspectionsSet.erase( l_Iter++ );
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessResetOutputs( bool& p_rProcessed )
	/// Removes the "head process count" from the outputs reset queue
	/// and determines from it the corresponding product pointer.
	/// If there is a new "head process count" in the outputs reset queue:
	/// Determines if the corresponding product's m_EndResetDelay has already been reached.
	/// If so, calls ResetOutputs().
	/// If not, puts the original "head process count" back onto the outputs reset queue
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_oOutputsResetQueue.GetCount() );

		if( p_rProcessed )
		{
			SVProductInfoStruct* l_pProduct = nullptr;

			SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

			while( 0 < m_oOutputsResetQueue.GetCount() && nullptr == l_pProduct )
			{
				long l_ProcessCount = 0;

				if( m_oOutputsResetQueue.RemoveHead( &l_ProcessCount ) )
				{
					GetProductInfoStruct( l_pProduct, l_ProcessCount );
				}
				else
				{
					l_Status = E_FAIL;

					break;
				}

				if( nullptr != l_pProduct && 0 < m_oOutputsResetQueue.GetCount() )
				{
					if( 0 < m_oOutputsResetQueue.GetCount() )
					{
						SVProductInfoStruct* l_pNextProduct = nullptr;

						if( m_oOutputsResetQueue.GetHead( &l_ProcessCount ) )
						{
							GetProductInfoStruct( l_pNextProduct, l_ProcessCount );
						}

						if( nullptr != l_pNextProduct && l_pNextProduct->oOutputsInfo.m_EndResetDelay <= l_CurrentTime )
						{
							l_pProduct = nullptr;
						}
					}
				}

				if( nullptr != l_pProduct )
				{
					if( l_CurrentTime < l_pProduct->oOutputsInfo.m_EndResetDelay )
					{
						m_NextOutputResetTimestamp = l_pProduct->oOutputsInfo.m_EndResetDelay;

						m_oOutputsResetQueue.AddHead( l_pProduct->ProcessCount() );

						l_pProduct = nullptr;

						break;
					}
				}
			}

			p_rProcessed = ( nullptr != l_pProduct );

			if( p_rProcessed )
			{
				ResetOutputs();
			}
			else
			{
				l_Status = E_FAIL;
			}

			::InterlockedExchange( &( m_ProcessingOutputReset ), 0 );
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessDataValidDelay( bool& rProcessed )
{
	HRESULT Status = S_OK;

	rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( !rProcessed )
	{
		rProcessed = ( 0 < m_DataValidDelayQueue.GetCount() );

		if( rProcessed )
		{
			SVProductInfoStruct* pProduct(nullptr);

			SVClock::SVTimeStamp CurrentTime = SVClock::GetTimeStamp();

			while( 0 < m_DataValidDelayQueue.GetCount() && nullptr == pProduct )
			{
				long ProcessCount = 0;

				if( m_DataValidDelayQueue.RemoveHead( &ProcessCount ) )
				{
					GetProductInfoStruct( pProduct, ProcessCount );
				}
				else
				{
					Status = E_FAIL;
					break;
				}

				if( nullptr != pProduct  && 0 < m_DataValidDelayQueue.GetCount() )
				{
					SVProductInfoStruct* pNextProduct( nullptr );

					if( m_DataValidDelayQueue.GetHead( &ProcessCount ) )
					{
						GetProductInfoStruct( pNextProduct, ProcessCount );
					}
					if( nullptr != pNextProduct && pNextProduct->oOutputsInfo.m_EndDataValidDelay <= CurrentTime )
					{
						pProduct = nullptr;
					}
				}

				if( nullptr != pProduct )
				{
					if( CurrentTime < pProduct->oOutputsInfo.m_EndDataValidDelay )
					{
						m_NextOutputDelayTimestamp = pProduct->oOutputsInfo.m_EndDataValidDelay;

						m_DataValidDelayQueue.AddHead( pProduct->ProcessCount() );

						pProduct = nullptr;
						break;
					}
				}
			}

			rProcessed = ( nullptr != pProduct );

			if( rProcessed )
			{
				if( !m_pDataValid.empty() )
				{
					m_pOutputList->WriteOutputValue( m_pDataValid,  pProduct->oOutputsInfo.DataValidResult );
				}
				if( !m_pOutputToggle.empty() )
				{
					m_pOutputList->WriteOutputValue( m_pOutputToggle,  pProduct->oOutputsInfo.OutputToggleResult );
				}
			}
			else
			{
				Status = E_FAIL;
			}

			::InterlockedExchange( &( m_ProcessingDataValidDelay ), 0 );
		}
	}

	return Status;
}

HRESULT SVPPQObject::ProcessCameraResponses( bool& p_rProcessed )
	/// If camera responses pending AND trigger queue empty:
	/// Removes camera responses from m_PendingCameraResponses and tries to process them
	/// until the first processing succeeds.
	/// If no responses processed: Removes the head of m_CameraResponseQueue and processes that
	/// until one processing attempt is successful or m_CameraResponseQueue is empty.
{
	HRESULT l_Status = S_OK;
	size_t	triggerQueueSize = 0;
	size_t	pendingCameraQueueSize = m_PendingCameraResponses.size();
	p_rProcessed = false;

	if( 0 < pendingCameraQueueSize)
	{
		SVCameraInfoMap::const_iterator l_CameraIter = m_Cameras.begin();

		while( S_OK == l_Status  && l_CameraIter != m_Cameras.end() && ! p_rProcessed )
		{
			SVCameraQueueElement l_Data;
			triggerQueueSize = m_oTriggerQueue.size();

			// stop processing camera responses if there is an element on the trigger queue.
			if ( 0 >= triggerQueueSize )
			{
				SVPendingCameraResponseMap::iterator l_PendingIter = m_PendingCameraResponses.find( l_CameraIter->first );

				if( l_PendingIter != m_PendingCameraResponses.end() )
				{
					l_Data = l_PendingIter->second;

					m_PendingCameraResponses.erase( l_PendingIter );

					SVObjectManagerClass::Instance().DecrementPendingImageIndicator();

					p_rProcessed = ( S_OK == ProcessCameraResponse( l_Data )  );
				}
			}

			++l_CameraIter;
		}
	} // if( 0 < m_PendingCameraResponses.size() )

	while( S_OK == l_Status  && 0 < m_CameraResponseQueue.GetCount() && ! p_rProcessed )
	{
		SVCameraQueueElement l_Data;

		triggerQueueSize = m_oTriggerQueue.size();
		p_rProcessed = ( 0 < m_oTriggerQueue.size() );

		if( 0 >= triggerQueueSize )
		{
			if( m_CameraResponseQueue.RemoveHead( &l_Data ) )
			{
				p_rProcessed = ( S_OK == ProcessCameraResponse( l_Data )  );
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

HRESULT SVPPQObject::ProcessCameraInputs( bool& p_rProcessed )
	/// If possible, removes the first item from m_oCamerasQueue and gets its product information.
	/// Sets the product information's bFinishAcquisition member to true.
	/// Adds the product's process count to m_oNotifyInspectionsSet.
{
	HRESULT l_Status = S_OK;
	long	cameraCount = -1;
	long	triggerCount = -1;
	long	tmpProcessCount = -1;

	p_rProcessed = false; // p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	// if trigger queue size <= 0
	if( ! p_rProcessed )
	{

		cameraCount = m_oCamerasQueue.GetCount();
		p_rProcessed = ( 0 <  cameraCount);

		if( p_rProcessed )
		{
			SVProductInfoStruct* l_pProduct = nullptr;
			long l_PPQIndex = static_cast<long>(m_ppPPQPositions.size());

			while( 0 < m_oCamerasQueue.GetCount() && nullptr == l_pProduct )
			{
				long l_ProcessCount = 0;

				if( m_oCamerasQueue.RemoveHead( &l_ProcessCount ) )
				{
					GetProductIndex( l_PPQIndex, l_ProcessCount );

					if( 0 <= l_PPQIndex && l_PPQIndex < static_cast< long >( m_ppPPQPositions.size() ) )
					{
						l_pProduct = m_ppPPQPositions.GetProductAt( l_PPQIndex );
						triggerCount = l_pProduct->oTriggerInfo.lTriggerCount;
					}
				}
				else
				{
					l_Status = E_FAIL;
					break;
				}
			}

			p_rProcessed = ( nullptr != l_pProduct );

			if( p_rProcessed )
			{
				if( !( l_pProduct->bFinishAcquisition ) )
				{
					l_pProduct->bFinishAcquisition = true;

					SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_Iter = l_pProduct->m_svCameraInfos.begin();

					while( l_Iter != l_pProduct->m_svCameraInfos.end() )
					{
						l_pProduct->bFinishAcquisition &= ( l_Iter->second.GetIndex() != -1 );

						++l_Iter;
					}
				}

				// Doesn't necessarilly have the correct Trigger Count yet.
				tmpProcessCount = l_pProduct->ProcessCount ();
				m_oNotifyInspectionsSet.insert( tmpProcessCount );
			} // if( p_rProcessed )
		} // if( p_rProcessed ) - if camera queue > 0
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessCompleteInspections( bool& p_rProcessed )
	/// Does nothing unless m_oTriggerQueue is empty and m_oInspectionQueue is non-empty.
	/// If so, gets all SVInspectionInfoPairs from m_oInspectionQueue and inserts all
	/// inspection GUIDs into m_ProcessInspectionsSet.
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

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_oInspectionQueue.GetCount() );

		if( p_rProcessed )
		{
			SVInspectionInfoPair l_Info;
			SVProductInfoStruct* l_pPPQProduct = nullptr;
			long l_PPQIndex = static_cast<long>(m_ppPPQPositions.size());

			for( int i = 0; i < m_oInspectionQueue.GetCount(); ++i )
			{
				if( m_oInspectionQueue.GetAt( i, &l_Info ) )
				{
					if( nullptr != l_Info.second.pInspection )
					{
						m_ProcessInspectionsSet.insert( l_Info.second.pInspection->GetUniqueObjectID() );
					}
				}
			}

			if( !( m_ProcessInspectionsSet.empty() ) )
			{
				bool l_Processed = false;

				ProcessInspections( l_Processed );
			}

			while( 0 < m_oInspectionQueue.GetCount() && nullptr == l_pPPQProduct )
			{
				if( m_oInspectionQueue.RemoveHead( &l_Info ) )
				{
					GetProductIndex( l_PPQIndex, l_Info.first );

					if( 0 <= l_PPQIndex && l_PPQIndex < static_cast< long >( m_ppPPQPositions.size() ) )
					{
						l_pPPQProduct = m_ppPPQPositions.GetProductAt( l_PPQIndex );
					}
				}
				else
				{
					l_Status = E_FAIL;

					break;
				}
			}

			p_rProcessed = ( nullptr != l_pPPQProduct );

			if( p_rProcessed )
			{
				if( l_pPPQProduct->oOutputsInfo.m_EndProcess == 0.0 )
				{
					SVInspectionInfoStruct& l_rInspectInfo = l_Info.second;
					SVInspectionInfoStruct& l_rPPQInspectInfo = l_pPPQProduct->m_svInspectionInfos[l_rInspectInfo.pInspection->GetUniqueObjectID()];

					l_rPPQInspectInfo = l_rInspectInfo;

					l_rPPQInspectInfo.ClearIndexes();

					l_rPPQInspectInfo.m_CallbackReceived = SVClock::GetTimeStamp();

					l_pPPQProduct->m_ProductState += _T( "|" );
					l_pPPQProduct->m_ProductState += l_rInspectInfo.pInspection->GetName();

					SVString l_Title = l_rInspectInfo.pInspection->GetName();

					l_Title += _T( " Complete" );

#ifdef EnableTracking
					m_PPQTracking.IncrementCount( l_Title, l_PPQIndex );
#endif

					// Inspection Process is done, let everyone know.
					if( ! SetInspectionComplete( l_PPQIndex ) )
					{
						l_Status = E_FAIL;
					}
				}

				if( l_pPPQProduct->bDataComplete )
				{

					SetProductComplete( l_PPQIndex ); 


					for( size_t i = l_PPQIndex + 1; i < m_ppPPQPositions.size(); ++i )
					{
						l_pPPQProduct = m_ppPPQPositions.GetProductAt( l_PPQIndex );

						if( nullptr != l_pPPQProduct && l_pPPQProduct->IsProductActive() )
						{
							SetProductIncomplete( static_cast<long>(i) );
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

HRESULT SVPPQObject::ProcessProductRequests( bool& p_rProcessed )
	/// Does nothing unless both m_oTriggerQueue and m_ProductRequests are nonempty.
	/// Otherwise extracts the first SVProductRequestPair from m_ProductRequests,
	/// gets the SVProductInfoStruct specified in the request.
	/// Then copies the SVProductInfoStruct into the m_pProduct member of the SVProductInfoRequestStruct
	/// contained in the request pair.
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_ProductRequests.GetCount() );

		if( p_rProcessed )
		{
			SVProductRequestPair l_Request;

			if( m_ProductRequests.RemoveHead( &l_Request ) )
			{
				if( nullptr != l_Request.second.m_pProduct )
				{
					SVProductInfoStruct* pProduct = nullptr;

					GetProductInfoStruct( pProduct, l_Request.first );

					if( nullptr != pProduct )
					{
						*( l_Request.second.m_pProduct ) = *( pProduct );
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

HRESULT SVPPQObject::GetProduct( SVProductInfoStruct& p_rProduct, long lProcessCount ) const
{
	HRESULT l_Status = S_OK;

	p_rProduct.InitProductInfo();

	if( m_AsyncProcedure.IsActive() && m_ProductRequests.IsCreated() )
	{
		SVProductInfoStruct* l_pProduct = new SVProductInfoStruct;

		if( nullptr != l_pProduct )
		{
			SVProductRequestPair l_Request( lProcessCount, SVProductInfoRequestStruct( l_pProduct ) );

			if( m_ProductRequests.AddTail( l_Request ) )
			{
				m_AsyncProcedure.Signal( nullptr );

				l_Status = l_Request.second.WaitForRequest();

				if( S_OK == l_Status  )
				{
					p_rProduct = *l_pProduct;

					if( p_rProduct.ProcessCount() < 1 )
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

void SVPPQObject::PersistInputs(SVObjectWriter& rWriter)
{
	SVIOEntryHostStructPtrList ppIOEntries;
	_variant_t l_svValue;
	SVString l_svName;

	rWriter.StartElement(CTAG_INPUT);

	GetAllInputs( ppIOEntries );

	long lCount = static_cast<long>(ppIOEntries.size());

	for(long lInput = 0; lInput < lCount; lInput++ )
	{
		SVIOEntryHostStructPtr pIOEntry = ppIOEntries[lInput];
		if( !pIOEntry.empty() 
			&& pIOEntry->m_ObjectType != IO_CAMERA_DATA_INPUT ) 
		{
			l_svName = SvUl_SF::Format(CTAGF_INPUT_X, lInput);
			rWriter.StartElement(l_svName.c_str());

			switch (pIOEntry->m_ObjectType)
			{
			case IO_DIGITAL_INPUT:
				{
					l_svValue.SetString( _T("Digital") );
					rWriter.WriteAttribute(CTAG_IO_TYPE, l_svValue);
					l_svValue.Clear();

					l_svValue.SetString( pIOEntry->m_pValueObject->GetName() );
					rWriter.WriteAttribute(CTAG_ITEM_NAME, l_svValue);
					l_svValue.Clear();

					l_svValue = -1;
					rWriter.WriteAttribute(CTAG_ATTRIBUTE, l_svValue);
					l_svValue.Clear();

					l_svValue = pIOEntry->m_PPQIndex;
					rWriter.WriteAttribute(CTAG_PPQ_POSITION, l_svValue);
					l_svValue.Clear();
				}
				break;

			case IO_REMOTE_INPUT:
				{
					l_svValue.SetString( _T("Remote") );
					rWriter.WriteAttribute(CTAG_IO_TYPE, l_svValue);
					l_svValue.Clear();

					l_svValue.SetString( pIOEntry->m_pValueObject->GetName() );
					rWriter.WriteAttribute(CTAG_ITEM_NAME, l_svValue);
					l_svValue.Clear();

					SVRemoteInputObject* l_pRemote = nullptr;

					m_pInputList->GetInput( pIOEntry->m_IOId, l_pRemote );

					if( nullptr != l_pRemote )
					{
						l_svValue = l_pRemote->m_lIndex;
						rWriter.WriteAttribute(CTAG_REMOTE_INDEX, l_svValue);
						l_svValue.Clear();
					}

					l_svValue = pIOEntry->m_PPQIndex;
					rWriter.WriteAttribute(CTAG_PPQ_POSITION, l_svValue);
					l_svValue.Clear();

					if( nullptr != l_pRemote )
					{
						l_pRemote->GetCache( l_svValue );

						if( l_svValue.vt != VT_EMPTY )
						{
							rWriter.WriteAttribute(CTAG_REMOTE_INITIAL_VALUE, l_svValue);
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
	}
	rWriter.EndElement();
}

SvTh::SVCameraTriggerData& SVPPQObject::GetCameraInputData()
{
	return m_CameraInputData;
}

long SVPPQObject::GetExtraBufferSize() const
{
	return g_lPPQExtraBufferSize;
}

static bool CompareInspectionName(const SVString& name, const SVString& dottedName)
{
	SVDottedName parsedName(dottedName.c_str());
	if (parsedName.size())
	{
		SVString inspectionName = parsedName[0];
		int cmp = inspectionName.compare(name);
		return (cmp > 0) ? true : false;
	}
	return false;
}

static HRESULT GetValueObject(const SVString& rName, SVValueObjectReference& rRefObject)
{
	HRESULT hr = S_FALSE;
	SVObjectClass* pObject(nullptr);
	hr = SVObjectManagerClass::Instance().GetObjectByDottedName(rName, pObject);
	if (nullptr != pObject)
	{
		rRefObject = SVValueObjectReference(pObject);
		hr = S_OK;
	}
	return hr;
}

void SVPPQObject::SetMonitorList(const ActiveMonitorList& rActiveList)
{
	m_bActiveMonitorList = rActiveList.first;
	if (m_bActiveMonitorList)
	{
		SVSharedMemorySingleton::Instance().SetRejectDepth(rActiveList.second.rejectDepth);
		SVSharedMemorySingleton::Instance().SetProductDepth(GetPPQLength(), g_lPPQExtraBufferSize);
		const SVMonitorList& rList = rActiveList.second.monitorList;

		// separate the list by Inspection and send to each Inspection
		const SVMonitorItemList& valList = rList.GetDataList();
		const SVMonitorItemList& imgList = rList.GetImageList();
		const SVMonitorItemList& rejectCondList = rList.GetConditionalDataList();

		typedef std::pair<SVMonitorItemList::const_iterator, SVMonitorItemList::const_iterator> Bounds;

		for (SVPPQInspectionProcessVector::iterator it = m_arInspections.begin(); it != m_arInspections.end(); ++it)
		{
			SVInspectionProcess* pInspection = (*it);
			if( nullptr != pInspection )
			{
				const SVString& inspectionName = pInspection->GetName();
				Bounds valBounds = std::equal_range(valList.begin(), valList.end(), inspectionName, CompareInspectionName);
				Bounds imgBounds = std::equal_range(imgList.begin(), imgList.end(), inspectionName, CompareInspectionName);

				SVMonitorList inspectionMonitorList(SVMonitorItemList(valBounds.first, valBounds.second),
					SVMonitorItemList(imgBounds.first, imgBounds.second),
					SVMonitorItemList(), SVMonitorItemList(), SVMonitorItemList());

				pInspection->UpdateSharedMemoryFilters(inspectionMonitorList);
			}
		}
		SetRejectConditionList(rejectCondList);

		// Get List of Inspections for this PPQ
		// SVSharedPPQWriter will create the inspection shares
		SvSml::InspectionIDs sharedInspectionWriterCreationInfo;
		size_t iSize = m_arInspections.GetSize();
		for( size_t i = 0; i < iSize; i++ )
		{
			sharedInspectionWriterCreationInfo.push_back(SvSml::InspectionID(m_arInspections[i]->GetName(), m_arInspections[i]->GetUniqueObjectID()));
		}
		HRESULT hr = SVSharedMemorySingleton::Instance().InsertPPQSharedMemory(GetName(), GetUniqueObjectID(), sharedInspectionWriterCreationInfo);
		if (S_OK != hr)
		{
			//Set MonitorList failed, clear momitorList
			SetMonitorList(ActiveMonitorList(false, RejectDepthAndMonitorList()));

			SVStringArray msgList;
			msgList.push_back(GetName());
			SvStl::MessageContainer Exception( SVMSG_SVO_46_SHARED_MEMORY_DISK_SPACE, SvOi::Tid_ErrorNotEnoughDiskSpace, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15025 );
			throw;
		}
		else
		{
			SvSml::SVShareControlHandler& rControlHandler = SVSharedMemorySingleton::Instance().GetIPCShare();
			if (rControlHandler.IsCreated())
			{
				rControlHandler.SetReady();
				// do we wait for ack?
			}
		}
	}
	else
	{
		// Clear the Reject Condition List
		SetRejectConditionList(SVMonitorItemList());

		// Clear the MonitorList(s)
		for (SVPPQInspectionProcessVector::iterator it = m_arInspections.begin(); it != m_arInspections.end(); ++it)
		{
			SVInspectionProcess* pInspection = (*it);
			if( nullptr != pInspection )
			{
				pInspection->UpdateSharedMemoryFilters(SVMonitorList());
			}
		}
	}
}

bool SVPPQObject::HasActiveMonitorList() const
{
	return m_bActiveMonitorList;
}

void SVPPQObject::fillChildObjectList(SVObjectClass::SVObjectPtrDeque& objectList, UINT AttributesAllowedFilter) const
{
	if (AttributesAllowedFilter != m_AttributesAllowedFilterForFillChildObjectList)
	{ //only recreate the list if the attributes changed.
		m_childObjectsForFillChildObjectList.clear();
		for (SVObjectPtrDeque::const_iterator iter = m_childObjects.cbegin(); iter != m_childObjects.cend(); ++iter)
		{
			if( ((*iter)->ObjectAttributesAllowed() & AttributesAllowedFilter) == AttributesAllowedFilter )
			{
				m_childObjectsForFillChildObjectList.push_back(*iter);
			}
		}
		//save the attribute value for the created list.
		m_AttributesAllowedFilterForFillChildObjectList = AttributesAllowedFilter;
	}

	objectList.insert(objectList.end(), m_childObjectsForFillChildObjectList.begin(), m_childObjectsForFillChildObjectList.end());
}

const SVString& SVPPQObject::GetConditionalOutputName() const
{
	return m_conditionalOutputName;
}

void SVPPQObject::SetConditionalOutputName(const SVString& conditionalOutputName)
{
	m_conditionalOutputName = conditionalOutputName;
}

#pragma region SVTriggerQueueElement Constructor
SVPPQObject::SVTriggerQueueElement::SVTriggerQueueElement()
	: m_TriggerInfo()
	, m_Inputs()
{
}

SVPPQObject::SVTriggerQueueElement::SVTriggerQueueElement( const SVTriggerQueueElement& p_rObject )
	: m_TriggerInfo( p_rObject.m_TriggerInfo )
	, m_Inputs( p_rObject.m_Inputs )
{
}

SVPPQObject::SVTriggerQueueElement::~SVTriggerQueueElement()
{
}
#pragma endregion SVTriggerQueueElement Constructor

#pragma region SVCameraQueueElement Constructor
SVPPQObject::SVCameraQueueElement::SVCameraQueueElement()
	: m_pCamera( nullptr )
	, m_Data()
{
}

SVPPQObject::SVCameraQueueElement::SVCameraQueueElement( const SVCameraQueueElement& rObject )
	: m_pCamera( rObject.m_pCamera )
	, m_Data( rObject.m_Data )
{
}

SVPPQObject::SVCameraQueueElement::SVCameraQueueElement( SVVirtualCamera* pCamera, const SVODataResponseClass& rData )
	: m_pCamera( pCamera )
	, m_Data( rData )
{
}

SVPPQObject::SVCameraQueueElement::~SVCameraQueueElement()
{
}
#pragma endregion SVCameraQueueElement Constructor

#pragma region SVCameraInfoElement Constructor
SVPPQObject::SVCameraInfoElement::SVCameraInfoElement()
	: m_CameraPPQIndex( -1 )
	, m_IOEntryPtr()
	, m_ToggleState( true )
{
}

SVPPQObject::SVCameraInfoElement::SVCameraInfoElement( const SVCameraInfoElement& p_rObject )
	: m_CameraPPQIndex( p_rObject.m_CameraPPQIndex )
	, m_IOEntryPtr( p_rObject.m_IOEntryPtr )
	, m_ToggleState( p_rObject.m_ToggleState )
{
}

SVPPQObject::SVCameraInfoElement::~SVCameraInfoElement()
{
}
#pragma endregion SVCameraInfoElement Constructor

#ifdef EnableTracking
#pragma region SVPPQTrackingElement Constructor
SVPPQObject::SVPPQTrackingElement::SVPPQTrackingElement()
	: m_TrackedCounts()
{
}

SVPPQObject::SVPPQTrackingElement::SVPPQTrackingElement( const SVPPQTrackingElement& p_rObject )
	: m_TrackedCounts( p_rObject.m_TrackedCounts )
{
}

SVPPQObject::SVPPQTrackingElement::~SVPPQTrackingElement()
{
	clear();
}
#pragma endregion SVPPQTrackingElement Constructor

void SVPPQObject::SVPPQTrackingElement::clear()
{
	if( !( m_TrackedCounts.empty() ) )
	{
		m_TrackedCounts.clear();
	}
}

void SVPPQObject::SVPPQTrackingElement::IncrementCount( size_t p_Index, size_t p_VectorSize )
{
	if( p_Index < p_VectorSize )
	{
		if( m_TrackedCounts.size() != p_VectorSize )
		{
			m_TrackedCounts.resize( p_VectorSize, 0 );
		}

		++( m_TrackedCounts[ p_Index ] );
	}
}

#pragma region SVPPQTracking Constructor
SVPPQObject::SVPPQTracking::SVPPQTracking()
	: m_QueueLength( 0 )
	, m_Counts()
	, m_QueueCounts()
	, m_QueueWriteTimeCounts()
{
}

SVPPQObject::SVPPQTracking::SVPPQTracking( const SVPPQTracking& p_rObject )
	: m_QueueLength( p_rObject.m_QueueLength )
	, m_Counts( p_rObject.m_Counts )
	, m_QueueCounts( p_rObject.m_QueueCounts )
	, m_TimeLength( p_rObject.m_TimeLength)
	, m_QueueWriteTimeCounts( p_rObject.m_QueueWriteTimeCounts )
{
}

SVPPQObject::SVPPQTracking::~SVPPQTracking()
{
	clear();
}
#pragma endregion SVPPQTracking Constructor

void SVPPQObject::SVPPQTracking::clear()
{
	m_QueueLength = 0;

	if( !( m_Counts.empty() ) )
	{
		m_Counts.clear();
	}

	if( !( m_QueueCounts.empty() ) )
	{
		m_QueueCounts.clear();
	}
	if( !( m_QueueWriteTimeCounts.empty() ) )
	{
		m_QueueWriteTimeCounts.clear();
	}
}

void SVPPQObject::SVPPQTracking::IncrementCount( const SVString& p_rName )
{
	++( m_Counts[ p_rName ] );
}

void SVPPQObject::SVPPQTracking::IncrementCount( const SVString& p_rName, size_t p_Index )
{
	m_QueueCounts[ p_rName ].IncrementCount( p_Index, m_QueueLength );
}

void SVPPQObject::SVPPQTracking::IncrementTimeCount( const SVString& p_rName, size_t p_Index )
{
	m_QueueWriteTimeCounts[ p_rName ].IncrementCount( p_Index, m_TimeLength );
}
#endif //EnableTracking

static SVGUID GetInspectionGuid(const SVString& rName)
{
	SVGUID guid;
	SVDottedName dottedName(rName.c_str());

	if (dottedName.size())
	{
		guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(dottedName[0].c_str());
	}
	return guid;
}

static long GetProductInfoInspectionIndex(const SVGUID& rGuid, const SVProductInfoStruct& rProduct)
{
	long index = -1;
	SVGUIDSVInspectionInfoStructMap::const_iterator it = rProduct.m_svInspectionInfos.find(rGuid);
	if (it != rProduct.m_svInspectionInfos.end())
	{
		index = it->second.m_lastInspectedSlot;
	}
	return index;
}

void SVPPQObject::SetRejectConditionList(const SVMonitorItemList& rRejectCondList)
{
	bool bNotFound = false;
	m_SharedMemoryItems.m_RejectConditionValues.clear();
	// keep a copy of the reject condition
	for (SVMonitorItemList::const_iterator it = rRejectCondList.begin();it != rRejectCondList.end();++it)
	{
		const SVString& name = (*it);
		SVGUID inspectionGuid = GetInspectionGuid(name);
		if (!inspectionGuid.empty())
		{
			SVValueObjectReference l_RefObject;
			if (S_OK == GetValueObject(name, l_RefObject))
			{
				m_SharedMemoryItems.m_RejectConditionValues[inspectionGuid][name] = l_RefObject.Guid();
			}
			else
			{
				bNotFound = true;
			}
		}
	}
	if (bNotFound)
	{
		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, SvOi::Tid_ErrorNotAllRejectConditionItemsFound, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15028 );
	}
}

// This method is for releasing the writer lock for Inspections when the product is incomplete
void SVPPQObject::ReleaseSharedMemory(const SVProductInfoStruct& rProduct)
{
	if (HasActiveMonitorList())
	{
		try
		{
			// Release Shared Product
			SvSml::SVSharedPPQWriter& rWriter = SVSharedMemorySingleton::Instance().GetPPQWriter(GetUniqueObjectID());
			SvSml::SVSharedProduct& rSharedProduct = rWriter.GetProductSlot(rProduct.m_lastInspectedSlot);
			rSharedProduct.m_TriggerCount = -1;
			rSharedProduct.m_Inspections.clear();

			rWriter.ReleaseProduct(rSharedProduct);
		}
		catch (const std::exception& e)
		{
			SVStringArray msgList;
			msgList.push_back(e.what());
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ReleaseProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15029 );
		}
		catch (...)
		{
			SVStringArray msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_Unknown));
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_ReleaseProduct, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15030 );
		}
	}
}

// This method writes the inspection info to the PPQ shared memory when the product is Complete
void SVPPQObject::CommitSharedMemory(const SVProductInfoStruct& rProduct)
{
	if (HasActiveMonitorList())
	{
		try
		{
			long shareSlotIndex = rProduct.m_lastInspectedSlot;
			SvSml::SVSharedPPQWriter& rWriter = SVSharedMemorySingleton::Instance().GetPPQWriter(GetUniqueObjectID());

			// Get next Available Product Slot
			SvSml::SVSharedProduct& rSharedProduct = rWriter.GetProductSlot(shareSlotIndex);

			rSharedProduct.m_TriggerCount = rProduct.ProcessCount();
			rSharedProduct.m_Inspections.clear();
			rSharedProduct.m_Inspections.reserve(rProduct.m_svInspectionInfos.size());

			// Build List of Inspections and their Slot Numbers
			for (SVGUIDSVInspectionInfoStructMap::const_iterator it = rProduct.m_svInspectionInfos.begin();it != rProduct.m_svInspectionInfos.end();++it)
			{
				const SVString& rShareName = rWriter[it->first].GetShareName();
				std::pair< SvSml::SVSharedInspectionMap::iterator,bool>  mRet; 	
				mRet = rSharedProduct.m_Inspections.insert(SvSml::SVSharedInspectionPair(SvSml::char_string(rShareName.c_str(), rSharedProduct.m_Allocator), 
					SvSml::SVSharedInspection(rShareName.c_str(), it->second.m_lastInspectedSlot, rSharedProduct.m_Allocator)));
			}
			// A Reject Depth of Zero is allowed and means we aren't keeping any rejects
			// Check for Reject - if Reject copy Last Inspected to reject (this includes images(file copies))
			if (SVSharedMemorySingleton::Instance().GetRejectDepth() && S_OK == CheckRejectCondition(rProduct, rWriter))
			{
				HRESULT hr = rWriter.CopyLastInspectedToReject(rSharedProduct);
				if (S_OK != hr)
				{
					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( SVMSG_SVO_50_SHARED_MEMORY_COPY_LASTINSPECTED_TO_REJECT, SvOi::Tid_ErrorCopyLastInspectedToReject, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15031 );
				}
			}
			// Mark Product Share as ready for reading
			rWriter.ReleaseProduct(rSharedProduct);
		}
		catch (const std::exception& e)
		{
			SVStringArray msgList;
			msgList.push_back(e.what());
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_CommitSharedMemory, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15032 );
		}
		catch (...)
		{
			SVStringArray msgList;
			msgList.push_back(SvStl::MessageData::convertId2AddtionalText(SvOi::Tid_Unknown));
			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_44_SHARED_MEMORY, SvOi::Tid_CommitSharedMemory, msgList, SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15033 );
		}
	}
}

// This method checks the Monitor List Reject Condition (usage is for shared memory only)
HRESULT SVPPQObject::CheckRejectCondition(const SVProductInfoStruct& rProduct, SvSml::SVSharedPPQWriter& rWriter) const
{
	HRESULT hr = S_FALSE; // means not a reject

	for (SVInspectionFilterElementMap::const_iterator it = m_SharedMemoryItems.m_RejectConditionValues.begin();it != m_SharedMemoryItems.m_RejectConditionValues.end() && S_OK != hr;++it)
	{
		// Get the writer, the last inspected index and the lastInspected data
		SvSml::SVSharedInspectionWriter& rInspectionWriter = rWriter[it->first];
		long index = GetProductInfoInspectionIndex(it->first, rProduct);
		if (-1 != index)
		{
			const SvSml::SVSharedData& rData = rInspectionWriter.GetLastInspectedSlot(index);
			for (SVFilterElementMap::const_iterator itItem = it->second.begin();itItem != it->second.end() && S_OK != hr;++itItem)
			{
				// Find the named item in the Shared memory
				SvSml::SVSharedValueMap::const_iterator valIt = rData.m_Values.find(SvSml::char_string(itItem->first.c_str(), rData.m_Allocator));
				if (valIt != rData.m_Values.end())
				{
					// Get the data value and convert to double...
					double dValue = 0.0;
					std::string val = valIt->second.m_Result.c_str();
					// handle boolean, as std::stod doesn't like it
					if ("TRUE" == val || "true" == val)
					{
						dValue = 1.0;
					}
					else if ("FALSE" == val || "false" == val)
					{
						dValue = 0.0;
					}
					else
					{
						dValue = std::stod(val);
					}
					if (dValue != 0.0)
					{
						hr = S_OK; // signifies a reject
					}
				}
			}
		}
	}
	return hr;
}

SVPPQObject::SVSharedMemoryFilters::SVSharedMemoryFilters()
	: m_RejectConditionValues()
{
}

void SVPPQObject::SVSharedMemoryFilters::clear()
{
	m_RejectConditionValues.clear();
}

DWORD SVPPQObject::GetObjectColor() const
{
	return SV_DEFAULT_WHITE_COLOR;
}

