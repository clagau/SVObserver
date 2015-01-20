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
#include <Mmsystem.h>
#include <fstream>
#include <algorithm>
#include <limits>
#include <boost/bind.hpp>
#include "SVPPQObject.h"
#include "SVDataManagerLibrary/DataManager.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVConfigurationLibrary/SVConfigurationTags.h"
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
#include "SVSystemLibrary/SVCrash.h"
#include "SVDigitalOutputObject1.h"
#include "SVTriggerObject.h"
#include "SVAcquisitionClass.h"
#include "SVTriggerClass.h"
#include "SVConfigurationObject.h"
#include "SVRemoteInputObject.h"
#include "SVPPQConstants.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SEJ_ErrorBase 15000
#define Err_15025 (SEJ_ErrorBase+25)
#define Err_15026 (SEJ_ErrorBase+26)
#define Err_15027 (SEJ_ErrorBase+27)
#define Err_15028 (SEJ_ErrorBase+28)
#define Err_15029 (SEJ_ErrorBase+29)
#define Err_15030 (SEJ_ErrorBase+30)
#define Err_15031 (SEJ_ErrorBase+31)
#define Err_15032 (SEJ_ErrorBase+32)
#define Err_15033 (SEJ_ErrorBase+33)

const long g_lPPQExtraBufferSize = 50;

#pragma endregion Declarations

HRESULT CALLBACK SVFinishTriggerCallback( void *pOwner, void *pCaller, void *pTriggerInfo )
{
	SVPPQObject* l_pPPQ = reinterpret_cast< SVPPQObject* >( pOwner );
	SVTriggerInfoStruct* l_pInfo = reinterpret_cast< SVTriggerInfoStruct* >( pTriggerInfo );

	BOOL bRet = ( l_pPPQ != NULL && l_pInfo != NULL );

	if( bRet )
	{
		bRet = l_pPPQ->FinishTrigger( pCaller, *l_pInfo );
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
	BOOL bRet;

	bRet = ( reinterpret_cast< SVPPQObject* >( pOwner ) )->FinishCamera( pCaller, reinterpret_cast< SVODataResponseClass* >( pResponse ) );

	if( bRet )
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

HRESULT SVPPQObject::ProcessDelayOutputs( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	SVProductInfoStruct* l_pProduct = NULL;

	SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

	while( 0 < m_oOutputsDelayQueue.GetCount() && l_pProduct == NULL )
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

		if( l_pProduct != NULL )
		{
			if( 0 < m_oOutputsDelayQueue.GetCount() )
			{
				SVProductInfoStruct* l_pNextProduct = NULL;

				if( m_oOutputsDelayQueue.GetHead( &l_ProcessCount ) )
				{
					GetProductInfoStruct( l_pNextProduct, l_ProcessCount );
				}

				if( l_pNextProduct != NULL && l_pNextProduct->oOutputsInfo.m_EndOutputDelay <= l_CurrentTime )
				{
					l_pProduct = NULL;
				}
			}
		}

		if( l_pProduct != NULL )
		{
			if( l_CurrentTime < l_pProduct->oOutputsInfo.m_EndOutputDelay )
			{
				m_NextOutputDelayTimestamp = l_pProduct->oOutputsInfo.m_EndOutputDelay;

				m_oOutputsDelayQueue.AddHead( l_pProduct->ProcessCount() );

				l_pProduct = NULL;

				break;
			}
		}
	}

	p_rProcessed = ( l_pProduct != NULL );

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
{
	init();
}

SVPPQObject::SVPPQObject( SVObjectClass* POwner, int StringResourceID )
: SVObjectClass( POwner, StringResourceID )
, m_oTriggerQueue( 10 )
, m_NAKCount( 0 )
, m_conditionalOutputName ( PPQ_CONDITIONAL_OUTPUT_ALWAYS ) 
, m_bActiveMonitorList(false)
{
	init();
}

SVPPQObject::~SVPPQObject()
{
	if( GetPPQLength() < 3 )
	{
		SVObjectManagerClass::Instance().DecrementShortPPQIndicator();
	}

	// Stop the multimedia timer thread for the output and reset time delays
	if( m_uOutputTimer != NULL )
	{
		::timeKillEvent( m_uOutputTimer );

		::timeEndPeriod( 1 );

		m_uOutputTimer = NULL;
	}

	if( m_bCreated )
		Destroy();

	m_oOutputMode                = SVPPQNextTriggerMode;
	m_bCreated                   = false;
	m_bOnline                    = false;
	m_bMaintainSourceImages      = false;
	m_lInspectionTimeoutMillisec = 0;
	SetPPQLength(StandardPpqLength);
	m_lOutputDelay               = 100;
	m_lResetDelay                = 0;

	m_ProcessingOutputDelay = 0;
	m_NextOutputDelayTimestamp = 0;
	m_ProcessingOutputReset = 0;
	m_NextOutputResetTimestamp = 0;

	m_pTriggerToggle.clear();
	m_pOutputToggle.clear();
	m_pDataValid.clear();
	m_uOutputTimer = NULL;

	m_pTrigger				= NULL;
	m_pMasterProductInfos	= NULL;
	m_pInputList			= NULL;
	m_pOutputList			= NULL;
}
#pragma endregion Constructor

void SVPPQObject::init()
{
	m_oOutputMode                = SVPPQNextTriggerMode;
	m_bCreated                   = false;
	m_bOnline                    = false;
	m_bMaintainSourceImages      = false;
	m_lInspectionTimeoutMillisec = 0;
	m_lOutputDelay               = 100;
	m_lResetDelay                = 0;

	m_ProcessingOutputDelay = 0;
	m_NextOutputDelayTimestamp = 0;
	m_ProcessingOutputReset = 0;
	m_NextOutputResetTimestamp = 0;

	m_uOutputTimer = NULL;

	m_pTrigger				= NULL;
	m_pMasterProductInfos	= NULL;
	m_pInputList			= NULL;
	m_pOutputList			= NULL;

	m_voTriggerToggle.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voOutputToggle.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voACK.ObjectAttributesAllowedRef()			= SV_EMBEDABLE | SV_PRINTABLE;
	m_voNAK.ObjectAttributesAllowedRef()			= SV_EMBEDABLE | SV_PRINTABLE;
	m_voMasterFault.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voMasterWarning.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voNotInspected.ObjectAttributesAllowedRef()	= SV_EMBEDABLE | SV_PRINTABLE;
	m_voDataValid.ObjectAttributesAllowedRef()		= SV_EMBEDABLE | SV_PRINTABLE;

	m_PpqValues.setValueObject( PpqLength, StandardPpqLength, this );
	SVObjectManagerClass::Instance().IncrementShortPPQIndicator();
}

HRESULT SVPPQObject::GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, long p_Index ) const
{
	HRESULT l_Status = SVObjectClass::GetChildObject( p_rpObject, p_rNameInfo, p_Index );

	if( l_Status != S_OK )
	{
		if( 0 < p_rNameInfo.m_NameArray.size() && p_rNameInfo.m_NameArray[ p_Index ] == GetName() )
		{
			if( ( p_Index + 1 ) == ( p_rNameInfo.m_NameArray.size() - 1 ) )
			{
				l_Status = m_voTriggerToggle.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );

				if( p_rpObject == NULL )
				{
					l_Status = m_voOutputToggle.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voACK.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voNAK.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voMasterFault.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voMasterWarning.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voNotInspected.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voDataValid.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voOutputState.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					l_Status = m_voTriggerCount.GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
				}

				if( p_rpObject == NULL )
				{
					BasicValueObject* pValue  = m_PpqValues.getValueObject(PpqLength);
					if( nullptr != pValue )
					{
						l_Status = pValue->GetChildObject( p_rpObject, p_rNameInfo, p_Index + 1 );
					}
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

		m_AsyncProcedure.Signal( NULL );
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
	if( m_bCreated ) { return FALSE; }

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

		m_ppPPQPositions.SetProductAt( i, NULL );
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

	SVProductInfoStruct* l_pProduct = NULL;

	if( m_qAvailableProductInfos.RemoveHead( &l_pProduct ) && l_pProduct != NULL )
	{
		if( m_ppPPQPositions.SetProductAt( 0, l_pProduct ) == S_OK )
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

	m_bCreated = TRUE;

	// Create the Queues that run the threads
	m_bCreated &= m_oCamerasQueue.Create();
	m_bCreated &= m_CameraResponseQueue.Create();
	m_bCreated &= m_oInspectionQueue.Create();
	m_bCreated &= m_oOutputsDelayQueue.Create();
	m_bCreated &= m_oOutputsResetQueue.Create();
	m_bCreated &= m_ProductRequests.Create();

	m_oNotifyInspectionsSet.clear();
	m_ProcessInspectionsSet.clear();

	if( m_bCreated )
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

	// This is the temporary PPQ_? to PLC_? connection
	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

#ifndef _WIN64
	if( pConfig != NULL )
	{
		m_strPLCId = pConfig->AssociatePPQToPLC( GetName() );
	}
#endif

	m_TriggerToggle = false;
	m_OutputToggle = false;

	return m_bCreated;
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
	if( !m_bCreated ) { return FALSE; }

	// Delete buckets for the PPQ positions
	m_ppPPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete [] m_pMasterProductInfos;

	m_pMasterProductInfos = NULL;

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

		m_ppPPQPositions.SetProductAt( i, NULL );
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

	SVProductInfoStruct* l_pProduct = NULL;

	if( m_qAvailableProductInfos.RemoveHead( &l_pProduct ) && l_pProduct != NULL )
	{
		if( m_ppPPQPositions.SetProductAt( 0, l_pProduct ) == S_OK )
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
	if( !m_bCreated ) { return FALSE; }

	// Stop the multimedia timer thread for the output and reset time delays
	if( m_uOutputTimer != NULL )
	{
		::timeKillEvent( m_uOutputTimer );

		::timeEndPeriod( 1 );

		m_uOutputTimer = NULL;
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
	m_oOutputsDelayQueue.Destroy();
	m_oOutputsResetQueue.Destroy();
	m_oInspectionQueue.Destroy();
	m_CameraResponseQueue.Destroy();
	m_oCamerasQueue.Destroy();

	// Delete buckets for the PPQ positions
	m_ppPPQPositions.clear();

	m_qAvailableProductInfos.RemoveAll();

	delete [] m_pMasterProductInfos;

	m_pMasterProductInfos = NULL;

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

	m_bCreated = FALSE;
	return TRUE;
}// end Destroy

BOOL SVPPQObject::DetachAll()
{
	DetachTrigger(m_pTrigger);

	std::deque< SVVirtualCamera* > l_Cameras;

	GetCameraList( l_Cameras );

	std::deque< SVVirtualCamera* >::iterator l_Iter = l_Cameras.begin();

	while( l_Iter != l_Cameras.end() )
	{
		SVVirtualCamera* l_psvCamera = ( *l_Iter );

		DetachCamera( l_psvCamera );

		++l_Iter;
	}

	int iSize = m_arInspections.GetSize();
	for(int i = iSize - 1; i >= 0; i--)
	{
		DetachInspection(m_arInspections.GetAt(i));
	}

	return TRUE;
}

BOOL SVPPQObject::IsCreated()
{
	return m_bCreated;
}// end IsCreated

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
	BasicValueObject* pValue  = m_PpqValues.getValueObject(PpqLength);
	if( nullptr != pValue )
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

BOOL SVPPQObject::AttachTrigger( SVTriggerObject *pTrigger )
{
	if( !pTrigger ) { return FALSE; }

	m_pTrigger = pTrigger;
	if( !m_pTrigger->RegisterFinishProcess( this, SVFinishTriggerCallback ) ) { return FALSE; }

	return TRUE;
}

BOOL SVPPQObject::AttachCamera( SVVirtualCamera *pCamera, long lPosition, bool p_AllowMinusOne )
{
	BOOL l_bOk = TRUE;

	if( !pCamera ) { return FALSE; }

	if( !p_AllowMinusOne && lPosition < 0 ) { lPosition = 0; }

	SVCameraInfoMap::iterator l_Iter = m_Cameras.find( pCamera );

	if( l_Iter != m_Cameras.end() )
	{
		l_Iter->second.m_CameraPPQIndex = lPosition;
	}
	else
	{
		SVString l_Name;

		l_Name.Format( _T( "%s Toggle" ), pCamera->GetName() );

		m_Cameras[ pCamera ].m_CameraPPQIndex = lPosition;
		m_Cameras[ pCamera ].m_ToggleState = true;

		l_bOk = RebuildProductInfoStructs();

		l_bOk &= pCamera->RegisterFinishProcess( this, SVFinishCameraCallback );
	}

	return l_bOk;
}

BOOL SVPPQObject::AttachInspection( SVInspectionProcess *pInspection )
{
	if( !pInspection ) { return FALSE; }

	m_arInspections.Add( pInspection );

	pInspection->SetPPQIdentifier( GetUniqueObjectID() );

	return TRUE;
}

BOOL SVPPQObject::DetachTrigger( SVTriggerObject *pTrigger )
{
	BOOL bOk = m_pTrigger != NULL && m_pTrigger == pTrigger;

	if ( bOk )
	{	
		bOk = m_pTrigger->UnregisterFinishProcess( this );

		m_pTrigger = NULL;
	}

	return bOk;
}// end DetachTrigger

BOOL SVPPQObject::DetachCamera( SVVirtualCamera *pCamera, BOOL bRemoveDepends/*=FALSE*/)
{
	if( !pCamera ) { return FALSE; }

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
		SVInspectionProcess *pInspection;
		int iCnt;
		int iInsSize = m_arInspections.GetSize();

		for ( iCnt = 0; iCnt < iInsSize; iCnt++ )
		{
			pInspection = m_arInspections.GetAt(iCnt);
			pInspection->RemoveCamera(pCamera->GetName());
		}
	}

	RebuildProductInfoStructs();

	return l_Status;
}// end DetachCamera

BOOL SVPPQObject::DetachInspection( SVInspectionProcess *pInspection )
{
	if( !pInspection ) { return FALSE; }

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

BOOL SVPPQObject::AddSharedCamera( SVVirtualCamera *pCamera )
{
	BOOL l_Status = ( pCamera != NULL );

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

BOOL SVPPQObject::GetInspectionCount( long &lSize )
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
		if( l_svIter->first != NULL )
		{
			p_rCameras[ l_svIter->first->GetName() ] = l_svIter->first;
		}

		++l_svIter;
	}

	return l_Status;
}

BOOL SVPPQObject::GetTrigger( SVTriggerObject *&ppTrigger )
{
	ppTrigger = m_pTrigger;

	return TRUE;
}// end GetTrigger

BOOL SVPPQObject::GetInspection( long lIndex, SVInspectionProcess *&ppInspection )
{
	if( lIndex < 0 || lIndex >= m_arInspections.GetSize() ) { return FALSE; }

	ppInspection = m_arInspections.GetAt( lIndex );

	return TRUE;
}// end GetInspection

BOOL SVPPQObject::SetCameraPPQPosition( long lPosition, SVVirtualCamera *pCamera )
{
	BOOL l_Status = true;

	if( m_bCreated && -1 <= lPosition && lPosition < static_cast< long >( m_ppPPQPositions.size() ) - 1 )
	{
		l_Status = AttachCamera( pCamera, lPosition, true );
	}
	else
	{
		l_Status = false;
	}

	return l_Status;
}// end SetCameraPPQPosition

BOOL SVPPQObject::GetCameraPPQPosition( long &lPosition, SVVirtualCamera *pCamera )
{
	BOOL bFound;

	lPosition = -1;

	if( !m_bCreated )
	{
		return FALSE;
	}// end if

	// Try to find to the Camera they sent it
	SVCameraInfoMap::iterator l_svIter;

	l_svIter = m_Cameras.find( pCamera );

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

	for( int j = 0; m_pMasterProductInfos != NULL && j < GetPPQLength() + g_lPPQExtraBufferSize; j++ )
	{
		m_pMasterProductInfos[j].m_svCameraInfos = l_CameraInfos;
	}// end for

	return l_bOk;
}

HRESULT SVPPQObject::DisplayGoOnlineError(const CString& sReason, HRESULT hr)
{
	const bool bDisplayError = TheSVObserverApp.GetProfileInt(_T("Debug"), _T("ReportGoOnlineFailure"), 0) != 0;
	TheSVObserverApp.WriteProfileInt(_T("Debug"), _T("ReportGoOnlineFailure"), (int) bDisplayError);
	
	if ( bDisplayError )
	{
		CString sMsg;
		
		if ( hr != S_OK )
		{
			sMsg.Format(_T("%s\nError code = %x"), sReason, hr);
		}
		else
		{
			sMsg.Format(_T("%s"), sReason);
		}

		AfxMessageBox(sMsg, MB_OK);
	}

	return S_OK;
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
			if (pCamera)
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

HRESULT SVPPQObject::CanGoOnline()
{
	long lPos = -1;
	HRESULT l_hrOk = S_OK;

	// Fixup Acquisition triggers (as Cameras can be in a different order than Triggers)
	AssignCameraToAcquisitionTrigger();

	if( ! m_pTrigger->CanGoOnline() )
	{
		CString sName = m_pTrigger->GetCompleteObjectName();
		CString sMsg;
		sMsg.Format(_T("%s cannot go online"), sName);
		DisplayGoOnlineError(sMsg);

		l_hrOk = SV_CAN_GO_ONLINE_FAILURE_TRIGGER;
	}

	if ( l_hrOk == S_OK )
	{
		SVCameraInfoMap::iterator l_svIter;

		for( l_svIter = m_Cameras.begin(); 
			l_hrOk == S_OK && l_svIter != m_Cameras.end(); ++l_svIter )
		{
			if ( ! ( l_svIter->second.m_CameraPPQIndex >= 0 && l_svIter->first->CanGoOnline() ) )
			{
				CString sName = l_svIter->first->GetCompleteObjectName();
				CString sMsg;
				sMsg.Format(_T("%s cannot go online"), sName);
				DisplayGoOnlineError(sMsg);

				l_hrOk = SV_CAN_GO_ONLINE_FAILURE_ACQUISITION;
			}
		}
	}

	// Reset the PPQ
	for( size_t i = 0; i < m_ppPPQPositions.size(); ++i )
	{
		SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt( i );
		if( l_pProduct != NULL )
		{
			if( !RecycleProductInfo( l_pProduct ) )
			{
				l_hrOk = SV_GO_ONLINE_FAILURE_RECYCLE_PRODUCT;
			}

			m_ppPPQPositions.SetProductAt( i, NULL );
		}

		if(  m_qAvailableProductInfos.RemoveHead( &l_pProduct ) && l_pProduct != NULL )
		{
			HRESULT l_HRTemp = m_ppPPQPositions.SetProductAt( i, l_pProduct );

			if( l_HRTemp == S_OK )
			{
				if( i == 0 )
				{
					l_HRTemp = GetNextAvailableIndexes( l_pProduct->oPPQInfo, SV_PPQ );
				}
				else
				{
					l_HRTemp = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultDataCircleBuffer, SV_PPQ, l_pProduct->oPPQInfo.m_ResultDataDMIndexHandle );
				}
			}

			if( l_hrOk == S_OK )
			{
				l_hrOk = l_HRTemp;
			}
		}
		else
		{
			if( l_hrOk == S_OK )
			{
				l_hrOk = SV_GO_ONLINE_FAILURE_RECYCLE_PRODUCT;
			}
		}
	}// end for

	SeidenaderVision::InspectionIDs sharedInspectionWriterCreationInfo;
	if ( l_hrOk == S_OK )
	{
		size_t iSize = m_arInspections.GetSize();
		for( size_t i = 0; S_OK == l_hrOk && i < iSize; i++ )
		{
			sharedInspectionWriterCreationInfo.push_back(SeidenaderVision::InspectionID(m_arInspections[i]->GetName(), m_arInspections[i]->GetUniqueObjectID()));
			if ( ! m_arInspections[i]->CanGoOnline() )
			{
				CString sName = m_arInspections[i]->GetCompleteObjectName();
				CString sMsg;
				sMsg.Format(_T("%s cannot go online"), sName);
				DisplayGoOnlineError(sMsg);

				l_hrOk = SV_CAN_GO_ONLINE_FAILURE_INSPECTION; // JMS ERROR - Create error message for inspection CanGoOnline failure
			}
		}// end for
	}

	if (l_hrOk == S_OK && !ResolveConditionalOutput())
	{
		l_hrOk = SV_CAN_GO_ONLINE_FAILURE_CONDITIONAL_OUTPUT;
	}

	if (S_OK == l_hrOk && HasActiveMonitorList())
	{
		// Get List of Inspections for this PPQ
		// SVSharedPPQWriter will create the inspection shares
		l_hrOk = SVSharedMemorySingleton::Instance().InsertPPQSharedMemory(GetName(), GetUniqueObjectID(), sharedInspectionWriterCreationInfo);
		if (S_OK != l_hrOk)
		{
			// Log Exception
			SVException l_Exception;
			CString l_Message;
			l_Message.Format(_T("CanGoOnline %s - Not enough Disk Space Available"), GetName());
			// Need to show a more specific error here
			SETEXCEPTION5(l_Exception, SVMSG_SVO_46_SHARED_MEMORY_DISK_SPACE, Err_15025, l_Message);
			l_Exception.LogException( l_Message );
		}
		else
		{
			SeidenaderVision::SVShareControlHandler& rControlHandler = SVSharedMemorySingleton::Instance().GetIPCShare();
			if (rControlHandler.IsCreated())
			{
				rControlHandler.SetReady();
				// do we wait for ack?
			}
		}
	}
	if (S_OK != l_hrOk && HasActiveMonitorList())
	{
		// clear the list
		SetMonitorList(ActiveMonitorList(false, RejectDepthAndMonitorList()));
	}

	return l_hrOk;
}// end CanGoOnline

HRESULT SVPPQObject::GoOnline()
{
	BOOL bValue;
	size_t i;
	size_t lSize;

	HRESULT l_hrOk = S_OK;

	CString sFailureObjectName;

	if( m_bOnline )
	{
		return SV_GO_ONLINE_FAILURE_ALREADY_ONLINE;
	}
#ifdef EnableTracking
	m_PPQTracking.clear();
	m_PPQTracking.m_QueueLength = m_ppPPQPositions.size();
	m_PPQTracking.m_TimeLength = m_lOutputDelay + 50;
#endif
	RebuildOutputList();

	// First, make sure the trigger toggle output is set to the right default
	if( !( m_pTriggerToggle.empty() ) && m_pTriggerToggle->m_pValueObject )
	{
		bValue = false;
		m_pTriggerToggle->m_pValueObject->SetValue( 1, bValue );
		m_TriggerToggle = ( bValue != FALSE );
	}// end if

	// Also, make sure the output toggle output is set to the right default
	if( !( m_pOutputToggle.empty() ) && m_pOutputToggle->m_pValueObject )
	{
		bValue = false;
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

	if ( l_hrOk == S_OK )
	{
		lSize = m_arInspections.GetSize();
		for( i = 0; l_hrOk == S_OK && i < lSize; i++ )
		{
			if ( ! m_arInspections[i]->GoOnline() )
			{
				sFailureObjectName = m_arInspections[i]->GetCompleteObjectName();

				l_hrOk = SV_GO_ONLINE_FAILURE_INSPECTION;
			}
		}// end for

		if( l_hrOk != S_OK )
		{
			lSize = m_arInspections.GetSize();
			for( i = 0; i < lSize; i++ )
			{
				m_arInspections[i]->GoOffline();
			}// end for

			CString sMsg;
			sMsg.Format(_T("%s failed to go online"), sFailureObjectName);
			DisplayGoOnlineError(sMsg);

			return l_hrOk;
		}// end if
	}

	if( l_hrOk == S_OK )
	{
		if (m_pTrigger->IsSoftwareTrigger())
		{
			// must do this before the Camera starts for 1394 cameras
			l_hrOk = m_pTrigger->EnableInternalTrigger();
		}
	}

	if( l_hrOk == S_OK )
	{
		SVCameraInfoMap::iterator l_svIter;

		for( l_svIter = m_Cameras.begin(); l_hrOk == S_OK && l_svIter != m_Cameras.end(); ++l_svIter )
		{
			if ( ! l_svIter->first->GoOnline() )
			{
				sFailureObjectName = l_svIter->first->GetCompleteObjectName();

				l_hrOk = SV_GO_ONLINE_FAILURE_ACQUISITION;
			}
		}// end for

		if( l_hrOk != S_OK )
		{
			for( l_svIter = m_Cameras.begin(); l_svIter != m_Cameras.end(); ++l_svIter )
			{
				l_svIter->first->GoOffline();
			}// end for

			lSize = m_arInspections.GetSize();
			for( i = 0; i < lSize; i++ )
			{
				m_arInspections[i]->GoOffline();
			}// end for

			CString sMsg;
			sMsg.Format(_T("%s failed to go online"), sFailureObjectName);
			DisplayGoOnlineError(sMsg);

			return l_hrOk;
		}// end if
	}

	if ( l_hrOk == S_OK )
	{
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
			for( i = 0; i < lSize; i++ )
			{
				m_arInspections[i]->GoOffline();
			}// end for

			CString sMsg;
			sMsg.Format(_T("%s failed to go online"), sFailureObjectName);
			DisplayGoOnlineError(sMsg);

			l_hrOk = SV_GO_ONLINE_FAILURE_TRIGGER;

			return l_hrOk;
		}// end if
	}

	// Create a multimedia timer thread for the output and reset time delays
	if( TIMERR_NOERROR != ::timeBeginPeriod( 1 ) )
	{
		return S_FALSE;
	}

	if( SVPPQNextTriggerMode != m_oOutputMode || m_lOutputDelay > 0 )
	{
		m_uOutputTimer = ::timeSetEvent( 1, 0, SVPPQObject::OutputTimerCallback, reinterpret_cast<DWORD_PTR>(this),
			TIME_PERIODIC | TIME_CALLBACK_FUNCTION );
	}// end if

	if ( l_hrOk == S_OK )
	{
		// Create the PPQ's threads
		l_hrOk = m_AsyncProcedure.Create( &SVPPQObject::APCThreadProcess, boost::bind(&SVPPQObject::ThreadProcess, this, _1), GetName(), SVAffinityPPQ );
	}
	
	if ( l_hrOk == S_OK )
	{
		// Kick the threads up a notch, for real!
		m_AsyncProcedure.SetPriority( THREAD_PRIORITY_HIGHEST );
	}

	m_bOnline = ( l_hrOk == S_OK );

	// This snippet of code here apears to do nothing, but it is actually 
	// initializing the RTTI for the output object parents.  This insures that there 
	// is no appreciable delay in the writeOutputs function that uses dynaic_cast.
	for( i = 0; i < m_UsedOutputs.size(); i++ )
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

	return l_hrOk;
}// end GoOnline

static void QuieseSharedMemory()
{
	SeidenaderVision::SVShareControlHandler& rControlShare = SVSharedMemorySingleton::Instance().GetIPCShare();
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

				if( l_pProduct != NULL )
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
	if( m_uOutputTimer != NULL )
	{
		::timeKillEvent( m_uOutputTimer );

		::timeEndPeriod( 1 );

		m_uOutputTimer = NULL;
	}

	// Bring the threads back down to earth
	m_AsyncProcedure.SetPriority( THREAD_PRIORITY_NORMAL );

	m_AsyncProcedure.Destroy();

	m_bOnline = FALSE;

	unsigned long l_ClearCount = 0;

	for( i = static_cast< long >( m_ppPPQPositions.size() ) - 1; l_ClearCount < 5 && 0 < i; --i )
	{
		SVProductInfoStruct* l_pProduct = m_ppPPQPositions.GetProductAt( i );

		if( l_pProduct != NULL )
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
		bRetVal = (ioEntry->m_pValueObject->GetName() == name);
	}
	return bRetVal; 
}

static bool CompareCompleteNameWithIOEntry(SVIOEntryHostStructPtr ioEntry, const SVString& name)
{
	bool bRetVal = false;
	if (ioEntry && ioEntry->m_pValueObject)
	{
		bRetVal = (ioEntry->m_pValueObject->GetCompleteObjectName() == name);
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

	SVConfigurationObject* pConfig = NULL;
	SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );
	SVInputObjectList* pInputObjectList=NULL;
	pConfig->GetInputObjectList(&pInputObjectList);
	long lCount=0;
	pInputObjectList->GetRemoteInputCount(lCount);

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
		SVObjectClass* pObject = NULL;
		HRESULT hr = SVObjectManagerClass::Instance().GetObjectByIdentifier(m_conditionalOutputValueID, pObject);
		if (hr == S_OK && pObject != NULL)
		{
			SVValueObjectClass* pValueObject = dynamic_cast<SVValueObjectClass *>(pObject);
			if (pValueObject)
			{
				double value;
				hr = pValueObject->GetValue(dataIndex, value);
				if (hr == S_OK)
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

	if( pProduct != NULL )
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
		if( TheSVDataManager.GetNextAvailableBufferIndex( m_pResultDataCircleBuffer, SV_PPQ, l_Handle ) == S_OK )
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
		
			SVConfigurationObject* pConfig = NULL;
			SVObjectManagerClass::Instance().GetConfigurationObject( pConfig );

#ifndef _WIN64
			// Do not call the PLC write Outputs if it is empty.
			if( 0 < pConfig->GetPLCCount() )
			{
				// PLC Outputs......
				HRESULT l_hr = pConfig->WriteOutputs( GetPLCName(), pProduct );
			}
#endif
			if( !m_pDataValid.empty() )
			{
				bool l_bValue=false;
				m_voDataValid.GetValue(lDataIndex, l_bValue);
				m_pOutputList->WriteOutputValue( m_pDataValid, l_bValue );
			}
			m_pOutputList->WriteOutputValue( m_pOutputToggle, m_OutputToggle );
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
	// TRACE( "%s - PPQ Reset Outputs \n", GetName() );
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

SVProductInfoStruct* SVPPQObject::IndexPPQ( SVTriggerInfoStruct& p_rTriggerInfo )
{
	SVProductInfoStruct* l_pProduct = NULL;
	SVProductInfoStruct* l_pNewProduct = NULL;
	SVProductInfoStruct* l_pPrevProduct = m_ppPPQPositions.GetProductAt( 0 );
	SVProductInfoStruct* l_pLastProduct = m_ppPPQPositions.IndexRegister();

	if( m_qAvailableProductInfos.RemoveHead( &l_pNewProduct ) && l_pNewProduct != NULL )
	{
		l_pNewProduct->oTriggerInfo = p_rTriggerInfo;

		if( l_pPrevProduct != NULL )
		{
			l_pNewProduct->oTriggerInfo.m_PreviousTrigger = l_pPrevProduct->oTriggerInfo.m_BeginProcess;
		}

		SVString l_TriggerCount;

		l_TriggerCount.Format( _T( "%ld" ), p_rTriggerInfo.lTriggerCount );

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

	if( l_pLastProduct != NULL )
	{
		// Recycle the exiting SVProductInfoStruct
		if( !RecycleProductInfo( l_pLastProduct ) )
		{
			SVException svE;
			CString l_sErrorStr;
			l_sErrorStr.Format("SVPPQObject::IndexPPQ-cannot release Input Buffer Index");
			SETEXCEPTION1(svE,-6506,l_sErrorStr);
			svE.LogException(l_sErrorStr);
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

			if( !( l_rInfo.m_CanProcess ) && !( l_rInfo.m_InProcess ) && pTempProduct->IsProductActive() )
			{
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
				}
			}
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
	HRESULT l_Status = S_OK;

	SVProductInfoStruct* l_pProduct = NULL;

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
			SVString l_String;

			l_String.Format( _T( "%s:NAK=%ld\n" ), GetName(), l_NAKCount );

			::OutputDebugString( l_String.c_str() );
		#endif
	}

	// Begin checking inspections to start processing
	for( size_t i = 0; i < l_Count; ++i )
	{
		SVProductInfoStruct* pTempProduct = m_ppPPQPositions.GetProductAt( i );

		if( pTempProduct != NULL )
		{
			if( pTempProduct->IsProductActive() )
			{
				SVInspectionInfoStruct& l_rInfo = pTempProduct->m_svInspectionInfos[ p_rInspectionID ];

				
				if( l_rInfo.m_CanProcess &&				// all inputs are available and inspection can start
					!( l_rInfo.m_InProcess ) &&			// inspection in not currently running
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

	if( l_pProduct != NULL && l_pProduct->m_svInspectionInfos[ p_rInspectionID ].pInspection != NULL )
	{
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
		// TRACE( "%s - 0x%08X - Output Processing \n", GetName(), pProduct );

		p_pProduct->oOutputsInfo.m_BeginProcess = SVClock::GetTimeStamp();
		p_pProduct->oOutputsInfo.lOutputDelay = m_lOutputDelay;
		p_pProduct->oOutputsInfo.lResetDelay  = m_lResetDelay;
		p_pProduct->oOutputsInfo.m_EndOutputDelay = SVClock::GetMinTimeStamp();
		p_pProduct->oOutputsInfo.m_EndResetDelay  = SVClock::GetMinTimeStamp();

		switch( m_oOutputMode )
		{
		case SVPPQNextTriggerMode :
			{
				// The trigger has come, time to write the outputs
				WriteOutputs( p_pProduct );

				p_pProduct->oOutputsInfo.m_EndProcess = SVClock::GetTimeStamp();

				// Check if we should fire up the reset outputs thread
				if( m_lResetDelay )
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

	bool bValid = ( pProduct != NULL );

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

		//TRACE( "%s - AddResultsToPPQ - Product=%d\n", GetName(), pProduct->ProcessCount() );

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
	bool bValid = ( pProduct != NULL );

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
		SVObjectManagerClass::Instance().UpdateObservers( "SVPPQObject", GetUniqueObjectID(), *pProduct );
	}

	if( bValid )
	{
		bValid = AddResultsToPPQ( p_PPQIndex );
	}

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
	bool l_Status = ( pProduct != NULL );

	if( l_Status )
	{
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
	bool l_Status = ( pProduct != NULL );

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

	if( pProduct != NULL )
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
{
	HRESULT l_Status = S_OK;

	if( ( p_rElement.m_pCamera != NULL ) && ( 0 <= p_rElement.m_Data.mDMHandle.GetIndex() ) )
	{
		SVClock::SVTimeStamp l_StartTick = 0;
		size_t l_PPQIndex = -1;
		size_t l_ProductIndex = -1;
		SVProductInfoStruct* l_pProduct = NULL;
		SVCameraInfoMap::iterator l_svIter;

		l_svIter = m_Cameras.find( p_rElement.m_pCamera );
					
		if( l_svIter != m_Cameras.end() )
		{
			l_PPQIndex = l_svIter->second.m_CameraPPQIndex;
		}

		p_rElement.m_Data.GetStartTick( l_StartTick );

		if( l_PPQIndex < m_ppPPQPositions.size() )
		{
			long l_Position = static_cast<long>(m_ppPPQPositions.size());

			GetProductIndex( l_Position, l_StartTick );

			if( 0 <= l_Position && l_Position < static_cast< long >( m_ppPPQPositions.size() ) )
			{
				size_t l_Index = l_Position + l_PPQIndex;

				if( l_Index < m_ppPPQPositions.size() )
				{
					l_ProductIndex = l_Index;
					l_pProduct = m_ppPPQPositions.GetProductAt( l_Index );
				}
			}
			else if( l_Position < 0 )
			{
				m_PendingCameraResponses[ p_rElement.m_pCamera ] = p_rElement;

				SVObjectManagerClass::Instance().IncrementPendingImageIndicator();

				SVString l_Title = _T( "Pending " );

				l_Title += p_rElement.m_pCamera->GetName();

#ifdef EnableTracking
				m_PPQTracking.IncrementCount( l_Title );
#endif
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

		if( l_pProduct != NULL )
		{
			SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svIter1;

			l_svIter1 = l_pProduct->m_svCameraInfos.find( p_rElement.m_pCamera );
			
			if( l_svIter1 != l_pProduct->m_svCameraInfos.end() )
			{
				if( l_svIter1->second.m_StartFrameTimeStamp == 0 )
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

						if( l_pAcqProduct != NULL )
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
						m_AsyncProcedure.Signal( NULL );
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
	BOOL l_Status = ( m_bOnline && ( pCaller != NULL ) && ( pResponse != NULL ) && ( 0 <= pResponse->mDMHandle.GetIndex() ) );

	if( l_Status )
	{
		SVVirtualCamera* pCamera = reinterpret_cast< SVVirtualCamera* >( pCaller );

		if( m_CameraResponseQueue.AddTail( SVCameraQueueElement( pCamera, *pResponse ) ) )
		{
			m_AsyncProcedure.Signal( NULL );
		}
	}

	return l_Status;
}

BOOL SVPPQObject::FinishTrigger( void *pCaller, SVTriggerInfoStruct& p_rTriggerInfo )
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
				// We need to reset the outputs in some modes so that the PLC can see the change after the delay
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

		m_AsyncProcedure.Signal( NULL );
	}

	return l_Status;
}

DWORD_PTR SVPPQObject::processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext )
{
	DWORD_PTR DwResult = NULL;
	// Try to process message by yourself...

	DWORD dwPureMessageID = DwMessageID & SVM_PURE_MESSAGE;
	switch( dwPureMessageID )
	{
		// handle renaming of toolset variables with regards to outputs
		case SVMSGID_OBJECT_RENAMED:
		{
			SVObjectClass* pObject = (SVObjectClass*) DwMessageValue;
			CString strOldName = (LPCTSTR) DwMessageContext;

			break;
		}// end case
	}// end switch

	return( SVObjectClass::processMessage( DwMessageID, DwMessageValue, DwMessageContext ) | DwResult );
}// end processMessage

bool SVPPQObject::IsProductAlive( long p_ProductCount ) const
{
	bool l_Status = ( m_ppPPQPositions.IsProductAlive( p_ProductCount ) == S_OK );

	return l_Status;
}

HRESULT SVPPQObject::GetNextAvailableIndexes( SVPPQInfoStruct& p_rPPQInfo, SVDataManagerLockTypeEnum p_LockType ) const
{
	HRESULT l_Status = S_OK;
	HRESULT l_Temp = S_OK;

	l_Temp = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultDataCircleBuffer, p_LockType, p_rPPQInfo.m_ResultDataDMIndexHandle );

	if( l_Status == S_OK )
	{
		l_Status = l_Temp;
	}

	l_Temp = TheSVDataManager.GetNextAvailableBufferIndex( m_pResultImagePublishedCircleBuffer, p_LockType, p_rPPQInfo.m_ResultImagePublishedDMIndexHandle );

	if( l_Status == S_OK )
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

	p_rpProduct = NULL;

	l_Status = GetProductIndex( l_Index, p_TimeStamp );

	if( 0 <= l_Index && l_Index < static_cast< long >( m_ppPPQPositions.size() ) )
	{
		p_rpProduct = m_ppPPQPositions.GetProductAt( l_Index );
	}
	else
	{
		if( l_Status == S_OK )
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

	l_bOk = ( l_svProduct.GetNextAvailableIndexes( p_LockType ) == S_OK );
	
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
	SVPPQObject* l_pPPQ = reinterpret_cast< SVPPQObject* >( dwUser );

	if( l_pPPQ != NULL )
	{
		if( l_pPPQ->m_AsyncProcedure.IsActive() )
		{
			l_pPPQ->NotifyProcessTimerOutputs();
		}
	}
}

HRESULT SVPPQObject::MarkProductInspectionsMissingAcquisiton( SVProductInfoStruct& p_rProduct, SVVirtualCamera* p_pCamera )
{
	HRESULT l_Status = S_OK;

	if( p_pCamera != NULL )
	{
		int iSize = m_arInspections.GetSize();

		for( int i = 0; i < iSize; i++ )
		{
			SVInspectionProcess* l_pInspect = m_arInspections[ i ];

			if( l_pInspect != NULL && l_pInspect->IsCameraInInspection( p_pCamera->GetName() ) )
			{
				SVInspectionInfoStruct l_InspectInfo = p_rProduct.m_svInspectionInfos[ l_pInspect->GetUniqueObjectID() ];

				l_InspectInfo.oInspectedState = PRODUCT_NOT_INSPECTED;
				l_InspectInfo.m_CanProcess = false;
				l_InspectInfo.m_InProcess = true;

				l_InspectInfo.m_EndInspection  = SVClock::GetTimeStamp();

				SVInspectionInfoPair l_Info( p_rProduct.ProcessCount(), l_InspectInfo );

				p_rProduct.m_ProductState += _T( "|MC=" );
				p_rProduct.m_ProductState += m_arInspections[ i ]->GetName();
				p_rProduct.m_ProductState += _T( "-" );
				p_rProduct.m_ProductState += p_pCamera->GetName();

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
		ProcessCameraResponses( l_Processed );
	}

	if( ! l_Processed )
	{
		ProcessCameraInputs( l_Processed );
	}

	if( ! l_Processed )
	{
		ProcessNotifyInspections( l_Processed );
	}

	if( ! l_Processed )
	{
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

	if( ::InterlockedCompareExchange( &( m_ProcessingOutputDelay ), 1, 0 ) == 0 )
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

	if( ::InterlockedCompareExchange( &( m_ProcessingOutputReset ), 1, 0 ) == 0 )
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

	if( l_Process )
	{
		m_AsyncProcedure.Signal( NULL );
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessTrigger( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( p_rProcessed )
	{
		SVTriggerQueueElement l_TriggerInfo;

		if( m_oTriggerQueue.PopHead( l_TriggerInfo ) == S_OK )
		{
			if( m_bOnline )
			{
				SVProductInfoStruct* pProduct = IndexPPQ( l_TriggerInfo.m_TriggerInfo );

				if( pProduct != NULL )
				{
					if( InitializeProduct( pProduct, l_TriggerInfo.m_Inputs ) )
					{
						long lDataIndex = pProduct->oPPQInfo.m_ResultDataDMIndexHandle.GetIndex();

						m_voTriggerCount.SetValue( lDataIndex, l_TriggerInfo.m_TriggerInfo.lTriggerCount );

						m_oNotifyInspectionsSet.insert( pProduct->ProcessCount() );
						
						if (!l_TriggerInfo.m_TriggerInfo.m_Data.empty())
						{
							m_CameraInputData.Set(lDataIndex, boost::any_cast<SVMaterials>(l_TriggerInfo.m_TriggerInfo.m_Data));
						}
						// Get Shared Memory Slot
						if (HasActiveMonitorList())
						{
							try
							{
								SeidenaderVision::SVSharedPPQWriter& rWriter = SVSharedMemorySingleton::Instance().GetPPQWriter(GetUniqueObjectID());
								long idx = 0;
								SeidenaderVision::SVSharedProduct& rSharedProduct = rWriter.RequestNextProductSlot(idx);
								rSharedProduct.m_TriggerCount = pProduct->ProcessCount();
								pProduct->m_lastInspectedSlot = idx;
								std::for_each(pProduct->m_svInspectionInfos.begin(), pProduct->m_svInspectionInfos.end(), 
									[&idx](SVGUIDSVInspectionInfoStructMap::value_type& value) { value.second.m_lastInspectedSlot = idx; } );
							}
							catch (const std::exception& e)
							{
								SVException l_Exception;
								CString l_Message;
								l_Message.Format(_T("ProcessTrigger - %s"), e.what());

								SETEXCEPTION5(l_Exception, SVMSG_SVO_44_SHARED_MEMORY, Err_15026, l_Message);
								l_Exception.LogException(l_Message);
							}
							catch (...)
							{
								SVException l_Exception;
								CString l_Message;
								l_Message.Format(_T("ProcessTrigger - Unknown"));

								SETEXCEPTION5(l_Exception, SVMSG_SVO_44_SHARED_MEMORY, Err_15027, l_Message);
								l_Exception.LogException(l_Message);
							}
						}
					}
				}
			}
		}
		else
		{
			l_Status = E_FAIL;
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessNotifyInspections( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_oNotifyInspectionsSet.size() );

		if( p_rProcessed )
		{
			long l_Offset = -1;

			SVProcessCountSet::iterator l_Iter( m_oNotifyInspectionsSet.begin() );

			while( l_Iter != m_oNotifyInspectionsSet.end() && ( l_Offset < 0 || static_cast< long >( m_ppPPQPositions.size() ) <= l_Offset ) )
			{
				long l_ProcessCount = *l_Iter;

				m_ppPPQPositions.GetIndexByTriggerCount( l_Offset, l_ProcessCount );

				l_Iter = m_oNotifyInspectionsSet.erase( l_Iter );
			}

			p_rProcessed = ( 0 <= l_Offset && l_Offset < static_cast< long >( m_ppPPQPositions.size() ) );

			if( p_rProcessed )
			{
				l_Status = NotifyInspections( l_Offset );
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessInspections( bool& p_rProcessed )
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
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_oOutputsResetQueue.GetCount() );

		if( p_rProcessed )
		{
			SVProductInfoStruct* l_pProduct = NULL;

			SVClock::SVTimeStamp l_CurrentTime = SVClock::GetTimeStamp();

			while( 0 < m_oOutputsResetQueue.GetCount() && l_pProduct == NULL )
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

				if( l_pProduct != NULL && 0 < m_oOutputsResetQueue.GetCount() )
				{
					if( 0 < m_oOutputsResetQueue.GetCount() )
					{
						SVProductInfoStruct* l_pNextProduct = NULL;

						if( m_oOutputsResetQueue.GetHead( &l_ProcessCount ) )
						{
							GetProductInfoStruct( l_pNextProduct, l_ProcessCount );
						}

						if( l_pNextProduct != NULL && l_pNextProduct->oOutputsInfo.m_EndResetDelay <= l_CurrentTime )
						{
							l_pProduct = NULL;
						}
					}
				}

				if( l_pProduct != NULL )
				{
					if( l_CurrentTime < l_pProduct->oOutputsInfo.m_EndResetDelay )
					{
						m_NextOutputResetTimestamp = l_pProduct->oOutputsInfo.m_EndResetDelay;

						m_oOutputsResetQueue.AddHead( l_pProduct->ProcessCount() );

						l_pProduct = NULL;

						break;
					}
				}
			}

			p_rProcessed = ( l_pProduct != NULL );

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

HRESULT SVPPQObject::ProcessCameraResponses( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	if( 0 < m_PendingCameraResponses.size() )
	{
		SVCameraInfoMap::const_iterator l_CameraIter = m_Cameras.begin();

		while( l_Status == S_OK && l_CameraIter != m_Cameras.end() && ! p_rProcessed )
		{
			SVCameraQueueElement l_Data;

			p_rProcessed = ( 0 < m_oTriggerQueue.size() );

			if( ! p_rProcessed )
			{
				SVPendingCameraResponseMap::iterator l_PendingIter = m_PendingCameraResponses.find( l_CameraIter->first );

				if( l_PendingIter != m_PendingCameraResponses.end() )
				{
					l_Data = l_PendingIter->second;

					m_PendingCameraResponses.erase( l_PendingIter );

					SVObjectManagerClass::Instance().DecrementPendingImageIndicator();

					ProcessCameraResponse( l_Data );
				}
			}

			++l_CameraIter;
		}
	}

	while( l_Status == S_OK && 0 < m_CameraResponseQueue.GetCount() && ! p_rProcessed )
	{
		SVCameraQueueElement l_Data;

		p_rProcessed = ( 0 < m_oTriggerQueue.size() );

		if( ! p_rProcessed )
		{
			if( m_CameraResponseQueue.RemoveHead( &l_Data ) )
			{
				p_rProcessed = ( ProcessCameraResponse( l_Data ) == S_OK );
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
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_oCamerasQueue.GetCount() );

		if( p_rProcessed )
		{
			SVProductInfoStruct* l_pProduct = NULL;
			long l_PPQIndex = static_cast<long>(m_ppPPQPositions.size());

			while( 0 < m_oCamerasQueue.GetCount() && l_pProduct == NULL )
			{
				long l_ProcessCount = 0;

				if( m_oCamerasQueue.RemoveHead( &l_ProcessCount ) )
				{
					GetProductIndex( l_PPQIndex, l_ProcessCount );

					if( 0 <= l_PPQIndex && l_PPQIndex < static_cast< long >( m_ppPPQPositions.size() ) )
					{
						l_pProduct = m_ppPPQPositions.GetProductAt( l_PPQIndex );
					}
				}
				else
				{
					l_Status = E_FAIL;

					break;
				}
			}

			p_rProcessed = ( l_pProduct != NULL );

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

				m_oNotifyInspectionsSet.insert( l_pProduct->ProcessCount() );
			}
		}
	}

	return l_Status;
}

HRESULT SVPPQObject::ProcessCompleteInspections( bool& p_rProcessed )
{
	HRESULT l_Status = S_OK;

	p_rProcessed = ( 0 < m_oTriggerQueue.size() );

	if( ! p_rProcessed )
	{
		p_rProcessed = ( 0 < m_oInspectionQueue.GetCount() );

		if( p_rProcessed )
		{
			SVInspectionInfoPair l_Info;
			SVProductInfoStruct* l_pPPQProduct = NULL;
			long l_PPQIndex = static_cast<long>(m_ppPPQPositions.size());

			for( int i = 0; i < m_oInspectionQueue.GetCount(); ++i )
			{
				if( m_oInspectionQueue.GetAt( i, &l_Info ) )
				{
					if( l_Info.second.pInspection != NULL )
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

			while( 0 < m_oInspectionQueue.GetCount() && l_pPPQProduct == NULL )
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

			p_rProcessed = ( l_pPPQProduct != NULL );

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

						if( l_pPPQProduct != NULL && l_pPPQProduct->IsProductActive() )
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
				if( l_Request.second.m_pProduct != NULL )
				{
					SVProductInfoStruct* pProduct = NULL;

					GetProductInfoStruct( pProduct, l_Request.first );

					if( pProduct != NULL )
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

		if( l_pProduct != NULL )
		{
			SVProductRequestPair l_Request( lProcessCount, SVProductInfoRequestStruct( l_pProduct ) );

			if( m_ProductRequests.AddTail( l_Request ) )
			{
				m_AsyncProcedure.Signal( NULL );

				l_Status = l_Request.second.WaitForRequest();

				if( l_Status == S_OK )
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

#ifndef _WIN64
HRESULT SVPPQObject::SetPLCName( CString p_rstrName )
{
	m_strPLCId = p_rstrName;
	return S_OK;
}

HRESULT SVPPQObject::GetPLCName( CString& p_rstrName ) const
{
	p_rstrName = m_strPLCId;
	return S_OK;
}

const CString& SVPPQObject::GetPLCName() const
{
	return m_strPLCId;
}
#endif

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
			l_svName.Format(CTAGF_INPUT_X, lInput);
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

					SVRemoteInputObject* l_pRemote = NULL;

					m_pInputList->GetInput( pIOEntry->m_IOId, l_pRemote );

					if( l_pRemote != NULL )
					{
						l_svValue = l_pRemote->m_lIndex;
						rWriter.WriteAttribute(CTAG_REMOTE_INDEX, l_svValue);
						l_svValue.Clear();
					}

					l_svValue = pIOEntry->m_PPQIndex;
					rWriter.WriteAttribute(CTAG_PPQ_POSITION, l_svValue);
					l_svValue.Clear();

					if( l_pRemote != NULL )
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

SVCameraTriggerData& SVPPQObject::GetCameraInputData()
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
		int cmp = inspectionName.Compare(name);
		return (cmp > 0) ? true : false;
	}
	return false;
}

static HRESULT GetValueObject(const SVString& rName, SVValueObjectReference& rRefObject)
{
	HRESULT hr = S_FALSE;
	SVObjectClass* pObject(nullptr);
	hr = SVObjectManagerClass::Instance().GetObjectByDottedName(rName, pObject);
	if (pObject != nullptr)
	{
		rRefObject = SVValueObjectReference(pObject);
		hr = S_OK;
	}
	return hr;
}

HRESULT SVPPQObject::SetMonitorList(const ActiveMonitorList& rActiveList)
{
	HRESULT hr = S_OK;
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
			if (pInspection)
			{
				const SVString& inspectionName = pInspection->GetName();
				Bounds valBounds = std::equal_range(valList.begin(), valList.end(), inspectionName, CompareInspectionName);
				Bounds imgBounds = std::equal_range(imgList.begin(), imgList.end(), inspectionName, CompareInspectionName);

				SVMonitorList inspectionMonitorList(SVMonitorItemList(valBounds.first, valBounds.second),
													SVMonitorItemList(imgBounds.first, imgBounds.second),
													SVMonitorItemList(), SVMonitorItemList(), SVMonitorItemList());

				hr = pInspection->UpdateSharedMemoryFilters(inspectionMonitorList);
			}
		}
		SetRejectConditionList(rejectCondList);
	}
	else
	{
		// Clear the Reject Condition List
		SetRejectConditionList(SVMonitorItemList());

		// Clear the MonitorList(s)
		for (SVPPQInspectionProcessVector::iterator it = m_arInspections.begin(); it != m_arInspections.end(); ++it)
		{
			SVInspectionProcess* pInspection = (*it);
			if (pInspection)
			{
				pInspection->UpdateSharedMemoryFilters(SVMonitorList());
			}
		}
	}
	return hr;
}

bool SVPPQObject::HasActiveMonitorList() const
{
	return m_bActiveMonitorList;
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
: m_pCamera( NULL )
, m_Data()
{
}

SVPPQObject::SVCameraQueueElement::SVCameraQueueElement( const SVCameraQueueElement& p_rObject )
: m_pCamera( p_rObject.m_pCamera )
, m_Data( p_rObject.m_Data )
{
}

SVPPQObject::SVCameraQueueElement::SVCameraQueueElement( SVVirtualCamera* p_pCamera, const SVODataResponseClass& p_rData )
: m_pCamera( p_pCamera )
, m_Data( p_rData )
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
		guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(dottedName[0]);
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
		// Event Log...
		SVException l_Exception;
		CString message;
		message.Format(_T("Not All Reject Condition List items found\n"));

		SETEXCEPTION5(l_Exception, SVMSG_SVO_45_SHARED_MEMORY_SETUP_LISTS, Err_15028, message);
		l_Exception.LogException(message);
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
			SeidenaderVision::SVSharedPPQWriter& rWriter = SVSharedMemorySingleton::Instance().GetPPQWriter(GetUniqueObjectID());
			SeidenaderVision::SVSharedProduct& rSharedProduct = rWriter.GetProductSlot(rProduct.m_lastInspectedSlot);
			rSharedProduct.m_TriggerCount = -1;
			rSharedProduct.m_Inspections.clear();
			
			rWriter.ReleaseProduct(rSharedProduct);
		}
		catch (const std::exception& e)
		{
			SVException l_Exception;
			CString l_Message;
			l_Message.Format(_T("ReleaseProductSharedMemory - %s"), e.what());

			SETEXCEPTION5(l_Exception, SVMSG_SVO_44_SHARED_MEMORY, Err_15029, l_Message);
			l_Exception.LogException(l_Message);
		}
		catch (...)
		{
			SVException l_Exception;
			CString l_Message;
			l_Message.Format(_T("ReleaseProductSharedMemory - Unknown"));

			SETEXCEPTION5(l_Exception, SVMSG_SVO_44_SHARED_MEMORY, Err_15030, l_Message);
			l_Exception.LogException(l_Message);
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
			SeidenaderVision::SVSharedPPQWriter& rWriter = SVSharedMemorySingleton::Instance().GetPPQWriter(GetUniqueObjectID());

			// Get next Available Product Slot
			SeidenaderVision::SVSharedProduct& rSharedProduct = rWriter.GetProductSlot(shareSlotIndex);

			rSharedProduct.m_TriggerCount = rProduct.ProcessCount();
			rSharedProduct.m_Inspections.clear();
			rSharedProduct.m_Inspections.reserve(rProduct.m_svInspectionInfos.size());

			// Build List of Inspections and their Slot Numbers
			for (SVGUIDSVInspectionInfoStructMap::const_iterator it = rProduct.m_svInspectionInfos.begin();it != rProduct.m_svInspectionInfos.end();++it)
			{
				const SVString& rShareName = rWriter[it->first].GetShareName();
				rSharedProduct.m_Inspections.insert(SVSharedInspectionPair(char_string(rShareName.c_str(), rSharedProduct.m_Allocator), 
						SVSharedInspection(rShareName.c_str(), it->second.m_lastInspectedSlot, rSharedProduct.m_Allocator)));
			}
			// A Reject Depth of Zero is allowed and means we aren't keeping any rejects
			// Check for Reject - if Reject copy Last Inspected to reject (this includes images(file copies))
			if (SVSharedMemorySingleton::Instance().GetRejectDepth() && S_OK == CheckRejectCondition(rProduct, rWriter))
			{
				HRESULT hr = rWriter.CopyLastInspectedToReject(rSharedProduct);
				if (S_OK != hr)
				{
					// log an exception
					SVException l_Exception;
					CString message;
					message.Format(_T("CopyLastInspectedToReject failed\n"));

					SETEXCEPTION5(l_Exception, SVMSG_SVO_50_SHARED_MEMORY_COPY_LASTINSPECTED_TO_REJECT, Err_15031, message);
					l_Exception.LogException(message);
				}
			}
			// Mark Product Share as ready for reading
			rWriter.ReleaseProduct(rSharedProduct);
		}
		catch (const std::exception& e)
		{
			SVException l_Exception;
			CString l_Message;
			l_Message.Format(_T("CommitSharedMemory - %s"), e.what());

			SETEXCEPTION5(l_Exception, SVMSG_SVO_44_SHARED_MEMORY, Err_15032, l_Message);
			l_Exception.LogException(l_Message);
		}
		catch (...)
		{
			SVException l_Exception;
			CString l_Message;
			l_Message.Format(_T("CommitSharedMemory - Unknown"));

			SETEXCEPTION5(l_Exception, SVMSG_SVO_44_SHARED_MEMORY, Err_15033, l_Message);
			l_Exception.LogException(l_Message);
		}
	}
}

// This method checks the Monitor List Reject Condition (usage is for shared memory only)
HRESULT SVPPQObject::CheckRejectCondition(const SVProductInfoStruct& rProduct, SeidenaderVision::SVSharedPPQWriter& rWriter) const
{
	HRESULT hr = S_FALSE; // means not a reject

	for (SVInspectionFilterElementMap::const_iterator it = m_SharedMemoryItems.m_RejectConditionValues.begin();it != m_SharedMemoryItems.m_RejectConditionValues.end() && S_OK != hr;++it)
	{
		// Get the writer, the last inspected index and the lastInspected data
		SeidenaderVision::SVSharedInspectionWriter& rInspectionWriter = rWriter[it->first];
		long index = GetProductInfoInspectionIndex(it->first, rProduct);
		if (-1 != index)
		{
			const SVSharedData& rData = rInspectionWriter.GetLastInspectedSlot(index);
			for (SVFilterElementMap::const_iterator itItem = it->second.begin();itItem != it->second.end() && S_OK != hr;++itItem)
			{
				// Find the named item in the Shared memory
				SVSharedValueMap::const_iterator valIt = rData.m_Values.find(char_string(itItem->first.c_str(), rData.m_Allocator));
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVPPQObject.cpp_v  $
 * 
 *    Rev 1.36   19 Jan 2015 11:42:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Changed the method AddToAvailableInputs to fix DIO and remote inputs being displayed in the Object Selector when the shouldn't be.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   10 Dec 2014 06:44:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  977
 * SCR Title:  Fix Double Processing of Inspections with 2 Inspections on 1 PPQ
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added flag to InspectionInfoStruct to prevent additional queuing in Inspection::StartProcess from the PPQ::StartInspection. New Flag m_HasBeenQueued.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   01 Dec 2014 13:22:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Thread lables and attributes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   20 Nov 2014 05:03:12   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   add method IsObjectInPPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   30 Sep 2014 15:48:12   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  925
 * SCR Title:  Add PPQ Items and SVObserver Modes to Equation Editor Object Selector
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed destructor to use constant StandardPpqLength instead of using the 
 * 
 * literal 2.
 * Changed init to set the addributes for PPQ items.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   02 Sep 2014 14:53:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised ReleaseSharedMemory method to set the Product's TriggerCount in the share.This is used to designate an invalid product for the consumers of the product share.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   27 Aug 2014 01:30:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Fixed input variable sort problem
 * Moved sort method to SVIOEntryHostStruct
 * Changed methods: RebuildInputList
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   22 Aug 2014 08:42:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised GoOffline to only call ErasePPQSharedMemory if there is an active MOnitor List.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   21 Aug 2014 13:41:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised GoOffline to only call QuieseSharedMemory if there is an active Monitor List.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   21 Aug 2014 12:08:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised destructor to call SVSharedMemorySingleton::HasShares before trying to erase the PPQ share
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   18 Aug 2014 18:29:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised CanGoOnline to correct an issue where the HRESULT was not preserved and overridden.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   14 Aug 2014 18:25:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised Destroy method to remove the ppq and inspections shared memory shares.
 * Revised CanGoOnline to create the pq and inspection shared memory shares.
 * Added QuieseSharedMemory function.
 * Revised GoOffline to call QuieseSharedMemory function and remove the ppq and inspection shared memory shares.
 * Revised SetProductComplete to call CommitSharedMemory.
 * Revised SetProductIncomplete to call ReleaseSharedMemory.
 * Revised ProcessTrigger to acquire the ppq and inspection shared memory slot, if there is an active monitor list.
 * Revised SetMonitorList to set the reject depth.
 * Added GetProductInfoInspectionIndex function.
 * Added ReleaseSharedMemory method.
 * Added CommitSharedMemory method.
 * Added CheckRejectCondition method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   17 Jul 2014 20:13:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Removed namespaces and code review changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   03 Jul 2014 16:39:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised CanGoOnline to capture return code from SetMonitorList.
 * Removed dead code from SetMonitorList.
 * Revised CompareInspectionName to use SVDottedName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   19 Jun 2014 18:00:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SVSharedMemoryFilters methods.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   19 Jun 2014 17:50:58   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised SetMonitorList method to also accept reject depth.
 * Added HasActiveMonitorList method.
 * Added SetRejectConditionList method.
 * Added SVFilterElementMap struct for reject condition list (Monitor List).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   02 Jun 2014 10:17:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added required spaces.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   15 May 2014 11:56:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed processMessage signature to use DWORD_PTR.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   28 Apr 2014 15:37:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Revised SetMonitorList method to check for a valid pointer to the InspectionProcess.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   28 Apr 2014 14:24:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added SetMonitorList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   17 Mar 2014 15:32:16   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed "PPQ Length" to a Basic Value Object for access by Object Manager.
 *   Changed GetChildObject to support Basic Value Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   14 Mar 2014 09:12:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  891
 * SCR Title:  Remove tracking elements that hinder performance in release mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifdef EnableTracking around tracking code so it can be enabled for testing but not in the normal build.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   07 Mar 2014 18:22:58   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 *   Added init() and GetPPQLength() methods.
 *   Made methods const.
 *   Various code changes to better follow coding guidelines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   01 Feb 2014 12:00:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed SVSendmessage and processmessage to use LONG_PTR instead of DWORD.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   13 Jan 2014 12:50:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  881
 * SCR Title:  Fix Bug where Remote Inputs are missing after adding a new PPQ (e100)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated AddDefaultInputs to include RemoteInputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   25 Nov 2013 12:44:20   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  875
 * SCR Title:  Cleanup Memory Leak in SVPPQObject's Create Method
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed unnecessary string assignment:  "PPQ Input Data" 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   30 Oct 2013 11:00:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added #ifndef _WIN64 to remove deprecated code from the 64bit solution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Oct 2013 07:12:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   09 Aug 2013 10:23:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Revised RemoveCameraInputs to correct issue with remove_if
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   07 Aug 2013 13:34:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Renamed SVIOEntryHostStructPtrVector to SVIOEntryHostStructPtrList.
 * Revised RebuildInputList method to correct an issue with camera inputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Jul 2013 13:28:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 023.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.143   03 Jul 2013 13:41:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added AssignCameraToAcquisitionTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   05 Jul 2013 09:11:18   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 020.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.142   03 Jun 2013 11:42:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Modified PersistInputs to not write inputs of type IO_CAMERA_DATA_INPUT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   13 May 2013 12:37:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.141   02 May 2013 09:51:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated variable name for the trigger toggle state variable from trigger toggle to toggle state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 May 2013 13:26:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 013.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.140   01 May 2013 14:55:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update Write and Start Oputs methods to fix a problem with trigger toggle time stamp variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.139   01 May 2013 09:12:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output delay variables to us the min timestamp value insetad of the max time stamp value.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   08 May 2013 16:31:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.138   29 Apr 2013 14:25:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:11:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.137   16 Apr 2013 15:35:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:33:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.136   11 Mar 2013 07:18:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  818
 * SCR Title:  Fix Bug where "Data Valid" in tool name does not work as expected.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified reset outputs to look for "PPQ_" in the output name for special signals Data Valid, Trigger and Output Toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.135   11 Feb 2013 14:13:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code that uses the subject/observer design patten as part of the object manager with the new parameter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.134   21 Jan 2013 11:29:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added SetConditionalOutputName method
 * Added GetConditionalOutputName method
 * Added AddCameraDataInputs method
 * Added RemoveCameraDataInputa method
 * Added SetDefaultConditionalOutput method
 * Added HasCameraDataInputForConditionalOutput method
 * Added HasDigitalInputForConditionalOutput method
 * Added GetCameraDataInput method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.133   16 Jan 2013 16:43:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Migrating branch code into primary code base.  
 * 
 * Updated Ouput fucntionality assoicated with the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.132   10 Jan 2013 15:11:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged 6.01 branch code into main development code base.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.131   11 Oct 2012 09:35:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged changes in to main line of code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.130   04 Oct 2012 10:53:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.129   01 Oct 2012 10:46:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  793
 * SCR Title:  Fix PPQ Memory Consumption Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed Notify Camera Queue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.128   18 Sep 2012 18:29:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.127   04 Sep 2012 15:17:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to add new scripting and object creation/update funcitonality to the object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.126.1.2   04 Jan 2013 09:52:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code in SVPPQObject::SetInspectionComplete method to not use the return code from Update Observers call.  The return code does not have any revelance to this operation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.126.1.1   11 Oct 2012 08:53:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new start process state to the inspection info struct and update the variable when the inspection is queued and whe the product gets reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.126.1.0   01 Oct 2012 10:37:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  793
 * SCR Title:  Fix PPQ Memory Consumption Problem
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed Notify Camera Queue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.126   15 Aug 2012 14:41:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  779
 * SCR Title:  Fix Problems with DDE Inputs and DDE Outputs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new Singleton version of the SVIOConfigurationInterfaceClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.125   25 Jul 2012 14:44:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed processing bug with pended camera images.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.124   02 Jul 2012 17:30:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.123   25 Jun 2012 16:55:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to monitor delay problems with shared memory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.122   19 Jun 2012 14:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * Centralized the location of the configuration object identifier and removed synchronization problems.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.121   02 May 2012 11:22:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  762
 * SCR Title:  Fix Data Valid Signal Latency
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed bug with Data Valid State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.120   16 Apr 2012 09:54:08   tBair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  762
 * SCR Title:  Fix Data Valid Signal Latency
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Reset Outputs and Write Outputs to insure the data valid signal is written just before the output toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.119   16 Mar 2012 14:55:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  757
 * SCR Title:  Fix results view to sort PPQ object selected for viewing
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   modified sort function to account for different string lengths.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.118   15 Mar 2012 08:46:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  757
 * SCR Title:  Fix results view to sort PPQ object selected for viewing
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added sorting fucntionality to RebuildInputList method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.117   30 Jan 2012 11:47:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  748
 * SCR Title:  Add Remote Output Steams to SVObserver
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Modified source code to consolidate PLC and New Output stream functionality to I/O Controller.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.116   19 Dec 2011 16:09:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated #define to exclude a status trace to improve debug performance and reduce the trace clutter.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.115   12 Oct 2011 16:41:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated PPQ to allow for the removal but not detachment of a camera from processing.  This will prevent all execution of inspections associated with that camera on that PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.114   06 Oct 2011 09:06:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated PPQ Object to fix a crash when triggering the SVIM while the system is going on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.113   03 Oct 2011 10:47:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix a unregister problem when changing PPQ connections and update inspection when adding new image to image view.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.112   28 Sep 2011 13:11:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to fix issues with camera image interface and methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.111   26 Sep 2011 09:13:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Logging for Trigger and Output Toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.110   16 Sep 2011 16:18:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects with new device and resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.109   14 Jul 2011 16:19:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove un-used product info struct attribute and inspection attribute.  Added logging attribute to product info struct for product state.  Remove extra Inspection Complete notification.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.108   14 Jul 2011 08:59:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.107   14 Jul 2011 08:16:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  729
 * SCR Title:  Adjust SVObserver to fix perfomance problems due to logging
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove logging in places that cause performance issues and added a registry controlled variable to handle the go offline data manager dumps to files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.106   13 Jul 2011 16:18:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  728
 * SCR Title:  Fix IO Bug associated with PPQ Entries
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated method to add the trigger identifier to the inspection set to be checked to allow the inspection to be queued.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.105   05 Jul 2011 14:45:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.104   01 Jun 2011 11:14:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated reset logic to include run once instead of the reset and run once logic separately.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.103   24 May 2011 15:39:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ Object to use the lockless Ring Buffer for trigger notifications.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.102   02 May 2011 16:16:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added additional cleanup functionality to PPQObject to prevent crash associated with output timer.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.101   19 Apr 2011 16:22:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix issues with IO Assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.100   18 Apr 2011 10:59:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.99   11 Apr 2011 18:51:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with logical image to main image connection when going on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.98   10 Feb 2011 13:42:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Cleaned up output timer when system is going offline or closing the configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.97   27 Jan 2011 11:50:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.96   15 Dec 2010 14:05:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated combind logic back into Output List from the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.95   15 Dec 2010 10:36:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.94   13 Dec 2010 11:28:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Change the usage of _variant_t to clear the variable after use to eliminate invalid variant type for assignment.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.93   08 Dec 2010 13:26:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.92   09 Nov 2010 16:30:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate container objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.91   05 Nov 2010 11:13:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  703
 * SCR Title:  Allow SVObserver to Change Remote Input Values
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove redundent data objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.90   28 Oct 2010 14:08:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.89   30 Jul 2010 14:03:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  698
 * SCR Title:  Update Output State on IP Results when a NAK Occurs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated write outputs method to handle setting IP results to zero/false when product has NAKed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.88   19 Jul 2010 13:23:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with logging NAKed image in Inspection process.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.87   01 Jul 2010 08:55:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  696
 * SCR Title:  Fix Problem with PPQ Reset Delay
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix the evaluation for starting the timer for the delay functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.86   29 Jun 2010 14:08:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to change object validity test and casting changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.85   24 May 2010 14:52:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.  Updated thread creation in PPQ and version number 5.32 Beta 002.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.83   26 Feb 2010 12:27:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Udpated PPQ to fix lock-up problem with time delay data completion mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.82   25 Feb 2010 09:20:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ to move the indexing of the PPQ to the PPQ thread.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.81   24 Feb 2010 10:46:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use separated object to handle new indexing funtionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.80   18 Feb 2010 14:28:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to add includable logging for the output toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.79   16 Feb 2010 10:28:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated output toggle functionality to operate similar to the trigger toggle.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.78   12 Feb 2010 15:44:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  685
 * SCR Title:  Update PPQ Bar to allow camera(s) to be moved from the first position
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVPPQObject to fix problems with unassigning and indexing unassigned cameras.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.77   19 Jan 2010 15:56:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.76   16 Dec 2009 12:29:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.75   06 Nov 2009 16:22:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated issues with archiving and image tracking.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.74   05 Nov 2009 15:08:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated configuration type to fix the slescted system problem.  And fixed the offset problem in the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.73   28 Oct 2009 14:42:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix indexing published images and setting defaults.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.72   28 Oct 2009 14:36:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix indexing published images and setting defaults.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.71   27 Oct 2009 18:53:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with disconnecting cameras and invalid image behavior.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.70   23 Oct 2009 14:35:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Released additional prublished image buffers when going off-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.69   22 Oct 2009 19:34:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  633
 * SCR Title:  Upgrade Intek FireStack Driver and API to version 2.32
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Updated code to fix issues with const correctness.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.68   12 Oct 2009 16:00:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code that was included in the 4.82 branch.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.67   24 Sep 2009 09:38:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update code with branch changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.66   08 Sep 2009 15:56:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix go on-line problem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.65   03 Sep 2009 10:38:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.64   20 Aug 2009 11:08:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issue with extra brace.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.63   20 Aug 2009 09:27:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated branch code into main line to fix issues with trigger toggles ad NAKs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.62   14 Aug 2009 14:46:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to merge branch functionality back into main line code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.61   30 Jul 2009 12:23:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branched changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.60   03 Feb 2009 16:18:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  647
 * SCR Title:  Add Information to Event Log
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   roll back changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.59   29 Jan 2009 12:47:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added condition to call write PLC Outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.58   27 Jan 2009 13:58:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  647
 * SCR Title:  Add Information to Event Log
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Additional Logging to event log.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.57   21 Nov 2008 14:03:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fix bug in FinishTrigger for writing trigger count object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.56   14 Nov 2008 14:47:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PLC Outputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.55   17 Oct 2008 10:33:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised GoOnline to call EnableInternalTrigger for Software Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.54   31 Oct 2007 08:25:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified AddDefaultInputs to free up allocated memory.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.53   21 Jun 2007 14:41:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.9   12 Oct 2009 13:25:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problems with go-online and startup naks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.8   22 Sep 2009 18:18:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix delay when getting a product from a product count.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.7   08 Sep 2009 15:23:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem when going on-line while resetting Product Position Queue structures.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.6   19 Aug 2009 14:21:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed problem with NAKing startup product.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.5   18 Aug 2009 09:13:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problem with trigger toggle and output delays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.4   14 Aug 2009 09:10:24   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to perform the reset outputs for the time delay modes in the trigger callback.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.3   14 Aug 2009 08:31:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated PPQ to reset the outputs first when in time delay modes and to queue the notify inspection instead of notifying the inspection when indexing the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.2   13 Aug 2009 08:20:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved Camera queuing separate from the PPQ indexing, input reading, and output writing.  It will be an independent operation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.1   14 Jul 2009 15:05:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code touse the new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52.1.0   30 Oct 2007 10:46:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Free allocated memory in AddDefaultInputs
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.52   20 Mar 2007 15:21:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  579
 * SCR Title:  Upgrade SVObserver to 4.79 Release
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added initialization in GoOnline for output object parents by calling RTTI dynamic_cast function.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.51   22 Jan 2007 09:32:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  587
 * SCR Title:  Add Timeout Threshold to Extended Time Delay Mode
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new methods SetInspectionTimeout and GetInspectionTimeout
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.50   17 Jan 2006 09:54:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added GetInspections
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.49   04 Oct 2005 08:47:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed new and delete in ReadInputs.  Added member variable m_apInputsVect to handle the temperary inputentries in readInputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.48   30 Aug 2005 16:26:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix Crash when changing PPQ size by calling Rebuild if sizes are different.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.47   24 Aug 2005 13:58:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problem with last product info struct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.46   22 Aug 2005 10:27:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.45   19 Jul 2005 14:49:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   implemented new value object Get/Set
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.44   21 Jun 2005 08:27:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   object attributes now use accessor methods
 * value object functions now use HRESULT
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.43   17 Feb 2005 15:20:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  456
 * SCR Title:  Update Image and Tool Objects to use the new Extent Classes
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update class with new methodologies based on new base object, extents and reset structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.42   02 Nov 2004 15:37:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  459
 * SCR Title:  Fix a problem with inspection lock-up
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed lock-up bug (inspection StartProcess return on failure)
 * added logging
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.41   15 Jun 2004 14:01:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated RebuildOutputList method to handle concerns with whether the value is a string or a number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.40   11 Jun 2004 14:23:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Updated all methods that searched the ppq output list and made sure the state value object was setup with the correct name and owner.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.39   11 Jun 2004 07:50:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with DDE State.  State will now appear as a PPQ variable PPQ_X.State
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.38   08 Jun 2004 14:40:52   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  410
 * SCR Title:  Fix DDE IO
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with DDE State showing up multiple times on one PPQ
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   17 Dec 2003 14:14:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed Trigger Toggle and other outputs that are written at times different than the rest of the outputs so that they no longer conflict.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   29 Oct 2003 15:14:18   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  389
 * SCR Title:  Present Error Messages for Invalid Hardware using User Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated CanGoOnline and GoOnline methods to use HRESULTs as return codes to determine why the SVIM will not go on-line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   25 Sep 2003 14:29:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Add code to support the combining of digital outputs with PPQ values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   24 Sep 2003 11:12:02   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Changed product information enum from PRODUCT_INSPECT_ERROR to PRODUCT_INSPECT_NOT_RUN and remarked out some DebugBreak calls.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   19 Sep 2003 10:37:12   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   write out registry setting
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   27 Aug 2003 10:28:38   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  320
 * SCR Title:  Integrate Matrox Meteor II / 1394 Board and 1394 camera into SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added support for error message describing why SVO can't go online
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   18 Aug 2003 16:04:38   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Removed the use of the SVDigitalIOControlClass.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   08 Jul 2003 09:31:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated AddDefaultInputs method to change call to I/O resource.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   15 May 2003 16:26:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made fix so that the PPQ forces the delay queues to flush before going online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   01 May 2003 16:14:34   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed some small bugs and changed the display on the I/O Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   29 Apr 2003 10:51:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   25 Apr 2003 11:25:52   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Fixed several problems so that the DDE inputs and outputs will work like they used to in pre SVO 4.00 versions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   22 Apr 2003 14:07:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   16 Apr 2003 17:31:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  341
 * SCR Title:  DDE Input and Outputs are no longer working correctly
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made modifications to the DDE inputs/outputs. Some objects are now available again and fixed some bugs with existing code. State output and PPQ inputs are again available as DDE outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   10 Apr 2003 13:28:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  339
 * SCR Title:  Deleting a camera used by an Acquisition Tool causes problems
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added a second parameter to DetachCamera.  The second parameter is defaulted to FALSE, if true it will go thru and remove all dependencies of the camera.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   19 Mar 2003 15:20:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Fixed some problems with input/outputs and the Go online/Go offline process. Also, made changes to force the PPQ to always have valid inputs and outputs even without an inspection attached.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   18 Mar 2003 17:16:02   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  332
 * SCR Title:  Module Ready not available after being unassigned and other small I/O bugs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to the GoOnline procedure to make sure things back out successfully when there is an error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   11 Mar 2003 16:31:08   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed problem with m_bMaintainSourceImages not being initially set to FALSE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   07 Mar 2003 12:40:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   fixed bug added by last change so that the objects will go online.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   07 Mar 2003 09:59:54   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Reworked ::GoOnline so that the camera and trigger go online last, after the inspection. This should allow the system to be triggered while going online, even though that is still highly frowned upon.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   21 Feb 2003 14:13:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  316
 * SCR Title:  Add option to extent acquisition image source buffers to the length of the PPQ
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added code to the PPQ to maintain source images if requested.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   14 Feb 2003 10:30:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed error with how the source image index is locked through the Data Manager between the PPQ and the Inspection Process
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   12 Feb 2003 16:37:36   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Disable the locking of Result Image Indexes in the PPQ. We
 * currently only have 5 and don't allow the ability to maintain
 * them for the PPQ.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   30 Jan 2003 16:14:36   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated global function SVPPQCameraThreadFunc to use the new image circle buffer methodology.  Updated FinishCamera, FinishInspection and IndexPPQ methods to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Jan 2003 13:56:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bugs in the Get(Product)ImageList and Get(Product)DataList in calls from the SIAC.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   16 Jan 2003 18:37:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added code to support image overlays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Jan 2003 11:03:30   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Fixed bug in RemoveInput and RemoveOutput.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Jan 2003 13:02:12   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  307
 * SCR Title:  Implement the ability to remotely change Tool parameters on the SVIM
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added new code to fully enable Remote Inputs in the system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Jan 2003 14:02:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to how the images are displayed while running and while doing a RunOnce operation to use the image that is specified in the SVProductInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   17 Dec 2002 15:21:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  299
 * SCR Title:  SIAC needs to be able to request Source Images
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   BOOL GetProductInfoStruct(long lProcessCount, SVProductInfoStruct** ppStruct);
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Dec 2002 19:57:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  295
 * SCR Title:  Remove Result Data from Configuration Printout
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   CreateObject method was modified to properly update the isCreated flag and to set the printability of the internal value objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Dec 2002 16:50:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  315
 * SCR Title:  In Time Delay & Data Complete, outputs are wrong under certain configurations
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated SVPPQOutputsDelayThreadFunc for use with SVPPQObject class.
 * Updated SetInspectionComplete, StartOutputs, and AddResultsToPPQ methods in SVPPQObject.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   09 Dec 2002 15:56:24   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated FinishInspection method of SVPPQObject class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   09 Dec 2002 15:03:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  298
 * SCR Title:  Inputs on the PPQ Bar after position 1 prevents acquisitions and inspections
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated Rebuild, Destroy, ReadInputs, RebuildInputList, and IndexPPQ in SVPPQObject class relating to oPPQInfo.ppInputData variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Dec 2002 14:17:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  297
 * SCR Title:  Extended Time Delay output mode does not always write outputs
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated SVPPQOutputsDelayThreadFunc to force the netx output delay to be processed when the current one expires.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   09 Dec 2002 14:03:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  296
 * SCR Title:  Fix PPQ when multiple inspections and cameras are used
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   SVCamerasAPCProc function Added.
 * 
 * SVPPQCamerasThreadFunc function Added.
 * 
 * Initialize thread handle in both SVPPQObject constructors and in the destructor.
 * 
 * SVPPQObject::Create
 * -Created Camera Processing Queue
 * -Creates Camera Processing Thread
 * 
 * SVPPQObject::Destroy
 * -Destroys Camera Processing Thread
 * -Destroys Camera Processing Queue
 * 
 * SVPPQObject::FinishCamera
 * -Moved logic to Camera Processing Thread
 * 
 * SVPPQObject::FinishTrigger
 * -Changed logic to check if cameras are available.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Dec 2002 13:32:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  304
 * SCR Title:  Fix crash after deleting a PPQ and its attached inspection(s)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added DetachAll method to SVPPQObject.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:51:54   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/