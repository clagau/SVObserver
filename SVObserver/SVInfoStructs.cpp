//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIOEntryStruct
//* .File Name       : $Workfile:   SVInfoStructs.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   16 Dec 2014 06:54:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVInfoStructs.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVSVIMStateClass.h"


SVIOEntryStruct::SVIOEntryStruct()
: m_IOEntryPtr()
, m_EntryValid( false )
, m_CombinedValue( false )
{
}

SVIOEntryStruct::SVIOEntryStruct( const SVIOEntryStruct& p_rsvObject )
: m_IOEntryPtr( p_rsvObject.m_IOEntryPtr )
, m_EntryValid( p_rsvObject.m_EntryValid )
, m_CombinedValue( p_rsvObject.m_CombinedValue )
{
}

SVIOEntryStruct::~SVIOEntryStruct()
{
	clear();
}

bool SVIOEntryStruct::operator<( const SVIOEntryStruct& p_rsvObject ) const
{
	return ( m_IOEntryPtr < p_rsvObject.m_IOEntryPtr );
}

bool SVIOEntryStruct::empty() const
{
	return m_IOEntryPtr.empty();
}

void SVIOEntryStruct::clear()
{
	m_IOEntryPtr.clear();

	m_EntryValid = false;
	m_CombinedValue = false;
}

void SVIOEntryStruct::InitEntry()
{
	m_EntryValid = false;
	m_CombinedValue = false;
}


SVInputsInfoStruct::SVInputsInfoStruct()
: pInputsList( NULL )
, m_BeginProcess( 0.0 )
, m_EndProcess( 0.0 )
{
}

SVInputsInfoStruct::SVInputsInfoStruct( const SVInputsInfoStruct& p_rsvObject )
: pInputsList( p_rsvObject.pInputsList )
, m_BeginProcess( p_rsvObject.m_BeginProcess )
, m_EndProcess( p_rsvObject.m_EndProcess )
{
}

SVInputsInfoStruct::~SVInputsInfoStruct()
{
	Reset();
}

const SVInputsInfoStruct& SVInputsInfoStruct::operator=( const SVInputsInfoStruct& p_rsvObject )
{
	if( this != &p_rsvObject )
	{
		pInputsList = p_rsvObject.pInputsList;
		m_BeginProcess = p_rsvObject.m_BeginProcess;
		m_EndProcess = p_rsvObject.m_EndProcess;
	}

	return *this;
}

void SVInputsInfoStruct::Reset()
{
	pInputsList		= NULL;
	m_BeginProcess	= 0;
	m_EndProcess	= 0;
}// end Reset

void SVInputsInfoStruct::Init()
{
	m_BeginProcess = 0;
	m_EndProcess = 0;
}// end Init


SVOutputsInfoStruct::SVOutputsInfoStruct()
: pOutputsList( NULL )
, lOutputDelay( 0 )
, lResetDelay( 0 )
, lDataValidDelay( 0 )
, DataValidResult( false )
, OutputToggleResult( false )
, m_EndOutputDelay( 0 )
, m_EndResetDelay( 0 )
, m_EndDataValidDelay( 0 )
, m_BeginProcess( 0 )
, m_EndProcess( 0 )
{
}

SVOutputsInfoStruct::SVOutputsInfoStruct( const SVOutputsInfoStruct& p_rsvObject )
: pOutputsList( p_rsvObject.pOutputsList )
, lOutputDelay( p_rsvObject.lOutputDelay )
, lResetDelay( p_rsvObject.lResetDelay )
, lDataValidDelay( p_rsvObject.lDataValidDelay )
, DataValidResult( p_rsvObject.DataValidResult )
, OutputToggleResult( p_rsvObject.OutputToggleResult )
, m_EndOutputDelay( p_rsvObject.m_EndOutputDelay )
, m_EndResetDelay( p_rsvObject.m_EndResetDelay )
, m_EndDataValidDelay( p_rsvObject.m_EndDataValidDelay )
, m_BeginProcess( p_rsvObject.m_BeginProcess )
, m_EndProcess( p_rsvObject.m_EndProcess )
{
}

SVOutputsInfoStruct::~SVOutputsInfoStruct()
{
	Reset();
}

const SVOutputsInfoStruct& SVOutputsInfoStruct::operator=( const SVOutputsInfoStruct& p_rsvObject )
{
	if( this != &p_rsvObject )
	{
		pOutputsList = p_rsvObject.pOutputsList;
		lOutputDelay = p_rsvObject.lOutputDelay;
		lResetDelay = p_rsvObject.lResetDelay;
		lDataValidDelay = p_rsvObject.lDataValidDelay;
		DataValidResult = p_rsvObject.DataValidResult;
		OutputToggleResult = p_rsvObject.OutputToggleResult;
		m_EndOutputDelay = p_rsvObject.m_EndOutputDelay;
		m_EndResetDelay = p_rsvObject.m_EndResetDelay;
		m_EndDataValidDelay = p_rsvObject.m_EndDataValidDelay;
		m_BeginProcess = p_rsvObject.m_BeginProcess;
		m_EndProcess = p_rsvObject.m_EndProcess;
	}

	return *this;
}

void SVOutputsInfoStruct::Reset()
{
	pOutputsList	= NULL;
	lOutputDelay	= 0;
	lResetDelay		= 0;
	lDataValidDelay	= 0;
	DataValidResult = false;
	OutputToggleResult = false;
	m_EndOutputDelay			= 0;
	m_EndResetDelay				= 0;
	m_EndDataValidDelay			= 0;
	m_BeginProcess				= 0;
	m_EndProcess				= 0;
}// end Reset

void SVOutputsInfoStruct::Init()
{
	//Init only initializes these values because the rest of the values are required for the next process cycle
	m_EndOutputDelay			= 0;
	m_EndResetDelay				= 0;
	m_EndDataValidDelay			= 0;
	m_BeginProcess				= 0;
	m_EndProcess				= 0;
}// end Init


SVPPQInfoStruct::SVPPQInfoStruct()
: pPPQ()
, m_ResultDataDMIndexHandle()
, m_ResultImagePublishedDMIndexHandle()
,	m_InputData()
{
}

SVPPQInfoStruct::SVPPQInfoStruct( const SVPPQInfoStruct& p_rsvObject )
: pPPQ( p_rsvObject.pPPQ )
, m_ResultDataDMIndexHandle()
, m_ResultImagePublishedDMIndexHandle()
,	m_InputData( p_rsvObject.m_InputData )
{
	m_ResultDataDMIndexHandle.Assign( p_rsvObject.m_ResultDataDMIndexHandle, p_rsvObject.m_ResultDataDMIndexHandle.GetLockType() );
	m_ResultImagePublishedDMIndexHandle.Assign( p_rsvObject.m_ResultImagePublishedDMIndexHandle, p_rsvObject.m_ResultImagePublishedDMIndexHandle.GetLockType() );
}

SVPPQInfoStruct::~SVPPQInfoStruct()
{
	Reset();
}

HRESULT SVPPQInfoStruct::Assign( const SVPPQInfoStruct &p_rsvObject, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( this != &p_rsvObject )
	{
		HRESULT l_Temp = S_OK;

		pPPQ = p_rsvObject.pPPQ;

		m_InputData = p_rsvObject.m_InputData;

		l_Temp = m_ResultDataDMIndexHandle.Assign( p_rsvObject.m_ResultDataDMIndexHandle, p_rsvObject.m_ResultDataDMIndexHandle.GetLockType() );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		l_Temp = m_ResultImagePublishedDMIndexHandle.Assign( p_rsvObject.m_ResultImagePublishedDMIndexHandle, p_rsvObject.m_ResultImagePublishedDMIndexHandle.GetLockType() );
	
		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}
	
	return l_Status;
}

void SVPPQInfoStruct::Reset()
{
	pPPQ = NULL;

	m_InputData.clear();

	ClearIndexes();
}

void SVPPQInfoStruct::InitPPQInfo()
{
	SVIOEntryStructVector::iterator l_svIter;
	
	for( l_svIter = m_InputData.begin(); l_svIter != m_InputData.end(); ++l_svIter )
	{
		( *l_svIter ).InitEntry();
	}

	ClearIndexes();
}

void SVPPQInfoStruct::ClearIndexes()
{
	m_ResultDataDMIndexHandle.clear();
	m_ResultImagePublishedDMIndexHandle.clear();
}

HRESULT SVPPQInfoStruct::GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( pPPQ != NULL )
	{
		l_Status = pPPQ->GetNextAvailableIndexes( *this, p_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}


SVTriggerInfoStruct::SVTriggerInfoStruct()
: bValid( FALSE )
, pTrigger( NULL )
, lTriggerCount( 0 )
, m_ToggleState( false )
, m_BeginProcess( 0 )
, m_ToggleTimeStamp( 0 )
, m_PushedOutputs( 0 )
, m_PreviousTrigger( 0 )
, m_CallbackReceived( 0 )
{
}

SVTriggerInfoStruct::SVTriggerInfoStruct( const SVTriggerInfoStruct& p_rsvObject )
: bValid( p_rsvObject.bValid )
, pTrigger( p_rsvObject.pTrigger )
, lTriggerCount( p_rsvObject.lTriggerCount )
, m_ToggleState( p_rsvObject.m_ToggleState )
, m_BeginProcess( p_rsvObject.m_BeginProcess )
, m_ToggleTimeStamp( p_rsvObject.m_ToggleTimeStamp )
, m_PushedOutputs( p_rsvObject.m_PushedOutputs )
, m_PreviousTrigger( p_rsvObject.m_PreviousTrigger )
, m_CallbackReceived( p_rsvObject.m_CallbackReceived )
, m_Data( p_rsvObject.m_Data )
{
}

SVTriggerInfoStruct::~SVTriggerInfoStruct()
{
	Reset();
}

const SVTriggerInfoStruct& SVTriggerInfoStruct::operator=( const SVTriggerInfoStruct& p_rsvObject )
{
	if( this != &p_rsvObject )
	{
		bValid = p_rsvObject.bValid;
		pTrigger = p_rsvObject.pTrigger;
		lTriggerCount	= p_rsvObject.lTriggerCount;
		m_ToggleState = p_rsvObject.m_ToggleState;
		m_BeginProcess = p_rsvObject.m_BeginProcess;
		m_ToggleTimeStamp = p_rsvObject.m_ToggleTimeStamp;
		// *** // ***
		// For Debugging Only
		m_PushedOutputs = p_rsvObject.m_PushedOutputs;
		m_PreviousTrigger = p_rsvObject.m_PreviousTrigger;
		m_CallbackReceived = p_rsvObject.m_CallbackReceived;
		// *** // ***

		m_Data = p_rsvObject.m_Data;
	}
	return *this;
}

void SVTriggerInfoStruct::Reset()
{
	bValid			= FALSE;
	pTrigger		= NULL;
	lTriggerCount	= 0;
	m_ToggleState = false;
	m_BeginProcess	= 0;
	m_ToggleTimeStamp = 0;
	// *** // ***
	// For Debugging Only
	m_PushedOutputs		= 0;
	m_PreviousTrigger	= 0;
	m_CallbackReceived	= 0;
	// *** // ***
	m_Data = boost::any();
}// end Reset

void SVTriggerInfoStruct::Init()
{
	bValid			= FALSE;
	lTriggerCount	= 0;
	m_ToggleState = false;
	m_BeginProcess	= 0;
	m_ToggleTimeStamp = 0;
	// *** // ***
	// For Debugging Only
	m_PushedOutputs		= 0;
	m_PreviousTrigger	= 0;
	m_CallbackReceived	= 0;
	// *** // ***
	m_Data = boost::any();
}// end Init


SVInspectionInfoStruct::SVInspectionInfoStruct()
:	pInspection( NULL ),
	oInspectedState(PRODUCT_NOT_INSPECTED),
	m_ResultImageDMIndexHandle(),
	m_CanProcess( false ),
	m_InProcess( false ),
	m_HasBeenQueued( false ),
	m_BeginInspection( 0 ),
	m_EndInspection( 0 ),
	m_BeginToolset( 0 ),
	m_EndToolset( 0 ),
	m_CallbackReceived( 0 ),
	m_ToolSetEndTime( 0.0 ),
	m_ToolSetAvgTime( 0.0 ),
	m_lastInspectedSlot(-1)
{
}

SVInspectionInfoStruct::SVInspectionInfoStruct( const SVInspectionInfoStruct &p_rsvData )
:	pInspection( NULL ),
	oInspectedState(),
	m_ResultImageDMIndexHandle(),
	m_CanProcess( false ),
	m_InProcess( false ),
	m_HasBeenQueued( false ),
	m_BeginInspection( 0 ),
	m_EndInspection( 0 ),
	m_BeginToolset( 0 ),
	m_EndToolset( 0 ),
	m_CallbackReceived( 0 ),
	m_ToolSetEndTime( 0.0 ),
	m_ToolSetAvgTime( 0.0 ),
	m_lastInspectedSlot(-1)
{
	Assign( p_rsvData, p_rsvData.m_ResultImageDMIndexHandle.GetLockType() );
}

SVInspectionInfoStruct::~SVInspectionInfoStruct()
{
	Reset();
}

const SVInspectionInfoStruct &SVInspectionInfoStruct::operator=( const SVInspectionInfoStruct &p_rsvData )
{
	if( this != &p_rsvData )
	{
		Assign( p_rsvData, p_rsvData.m_ResultImageDMIndexHandle.GetLockType() );
	}

	return *this;
}

HRESULT SVInspectionInfoStruct::Assign( const SVInspectionInfoStruct &p_rsvData, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( this != &p_rsvData )
	{
		HRESULT l_Temp = S_OK;

		pInspection = p_rsvData.pInspection;
		oInspectedState = p_rsvData.oInspectedState;
		m_CanProcess = p_rsvData.m_CanProcess;
		m_InProcess = p_rsvData.m_InProcess;
		m_HasBeenQueued = p_rsvData.m_HasBeenQueued;
		m_BeginInspection = p_rsvData.m_BeginInspection;
		m_EndInspection = p_rsvData.m_EndInspection;
		m_BeginToolset = p_rsvData.m_BeginToolset;
		m_EndToolset = p_rsvData.m_EndToolset;
		m_CallbackReceived = p_rsvData.m_CallbackReceived;

		m_ToolSetEndTime = p_rsvData.m_ToolSetEndTime;
		m_ToolSetAvgTime = p_rsvData.m_ToolSetAvgTime;
		m_lastInspectedSlot = p_rsvData.m_lastInspectedSlot;

		l_Temp = m_ResultImageDMIndexHandle.Assign( p_rsvData.m_ResultImageDMIndexHandle, p_LockType );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}
	
	return l_Status;
}

void SVInspectionInfoStruct::Reset()
{
	pInspection = NULL;
	oInspectedState = PRODUCT_NOT_INSPECTED;

	m_CanProcess = false;
	m_InProcess = false;
	m_HasBeenQueued = false;

	m_BeginInspection = 0;
	m_EndInspection = 0;
	m_BeginToolset = 0;
	m_EndToolset = 0;
	// *** // ***
	// For Debugging Only
	m_CallbackReceived = 0;
	// *** // ***

	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;

	m_lastInspectedSlot = -1;
	ClearIndexes();
}// end Reset

void SVInspectionInfoStruct::Init()
{
	oInspectedState = PRODUCT_NOT_INSPECTED;

	m_CanProcess = false;
	m_InProcess = false;
	m_HasBeenQueued = false;

	m_BeginInspection = 0;
	m_EndInspection = 0;
	m_BeginToolset = 0;
	m_EndToolset = 0;
	// *** // ***
	// For Debugging Only
	m_CallbackReceived = 0;
	// *** // ***

	m_ToolSetEndTime = 0.0;
	m_ToolSetAvgTime = 0.0;

	m_lastInspectedSlot = -1;

	ClearIndexes();
}// end Init

void SVInspectionInfoStruct::ClearIndexes()
{
	m_CanProcess = false;
	m_InProcess = false;

	m_ResultImageDMIndexHandle.clear();
}

HRESULT SVInspectionInfoStruct::GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( pInspection != NULL )
	{
		pInspection->GetNextAvailableIndexes( *this, p_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

SVCameraInfoStruct::SVCameraInfoStruct()
:	pCamera( nullptr ),
	m_SourceImageDMIndexHandle(),
	m_StartFrameTimeStamp( 0 ),
	m_EndFrameTimeStamp( 0 ),
	m_CallbackTimeStamp( 0 )
{
}

SVCameraInfoStruct::SVCameraInfoStruct( const SVCameraInfoStruct &p_rsvData )
:	pCamera( nullptr ),
	m_SourceImageDMIndexHandle(),
	m_StartFrameTimeStamp( 0 ),
	m_EndFrameTimeStamp( 0 ),
	m_CallbackTimeStamp( 0 )
{
	Assign( p_rsvData, p_rsvData.m_SourceImageDMIndexHandle.GetLockType() );
}

SVCameraInfoStruct::~SVCameraInfoStruct()
{
	Reset();
}

const SVCameraInfoStruct &SVCameraInfoStruct::operator=( const SVCameraInfoStruct &p_rsvData )
{
	if( this != &p_rsvData )
	{
		pCamera = p_rsvData.pCamera;
		m_StartFrameTimeStamp = p_rsvData.m_StartFrameTimeStamp;
		m_EndFrameTimeStamp = p_rsvData.m_EndFrameTimeStamp;
		m_CallbackTimeStamp = p_rsvData.m_CallbackTimeStamp;

		m_SourceImageDMIndexHandle.Assign( p_rsvData.m_SourceImageDMIndexHandle, p_rsvData.m_SourceImageDMIndexHandle.GetLockType() );
	}
	
	return *this;
}

HRESULT SVCameraInfoStruct::Assign( const SVCameraInfoStruct &p_rsvData, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( this != &p_rsvData )
	{
		pCamera = p_rsvData.pCamera;
		m_StartFrameTimeStamp = p_rsvData.m_StartFrameTimeStamp;
		m_EndFrameTimeStamp = p_rsvData.m_EndFrameTimeStamp;
		m_CallbackTimeStamp = p_rsvData.m_CallbackTimeStamp;

		l_Status = m_SourceImageDMIndexHandle.Assign( p_rsvData.m_SourceImageDMIndexHandle, p_LockType );

		if( m_SourceImageDMIndexHandle.GetIndex() != p_rsvData.m_SourceImageDMIndexHandle.GetIndex() )
		{
			l_Status = E_FAIL;
		}
	}
	
	return l_Status;
}

HRESULT SVCameraInfoStruct::Assign( SVClock::SVTimeStamp p_StartFrameTS, SVClock::SVTimeStamp p_EndFrameTS, const SVDataManagerHandle& p_rIndexHandle, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	assert( m_SourceImageDMIndexHandle.GetIndex() < 0 || ( 0 <= m_SourceImageDMIndexHandle.GetIndex() && 0 <= p_rIndexHandle.GetIndex() ) );

	m_StartFrameTimeStamp = p_StartFrameTS;
	m_EndFrameTimeStamp = p_EndFrameTS;

	m_CallbackTimeStamp = SVClock::GetTimeStamp();

	l_Status = m_SourceImageDMIndexHandle.Assign( p_rIndexHandle, p_LockType );

	if( m_SourceImageDMIndexHandle.GetIndex() != p_rIndexHandle.GetIndex() )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}


void SVCameraInfoStruct::Reset()
{
	pCamera = nullptr;
	m_StartFrameTimeStamp = 0;
	m_EndFrameTimeStamp = 0;
	// *** // ***
	// For Debugging Only
	m_CallbackTimeStamp	= 0;
	// *** // ***

	ClearIndexes();
}// end Reset

void SVCameraInfoStruct::ClearInfo()
{
	m_StartFrameTimeStamp = 0;
	m_EndFrameTimeStamp = 0;
	// *** // ***
	// For Debugging Only
	m_CallbackTimeStamp	= 0;
	// *** // ***

	ClearIndexes();
}

void SVCameraInfoStruct::ClearCameraInfo()
{
	m_StartFrameTimeStamp = 0;
	m_EndFrameTimeStamp = 0;

	ClearIndexes();
}// end Init

void SVCameraInfoStruct::ClearIndexes()
{
	m_SourceImageDMIndexHandle.clear();
}

HRESULT SVCameraInfoStruct::GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( nullptr != pCamera )
	{
		pCamera->ReserveNextImageHandleIndex( m_SourceImageDMIndexHandle, p_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

long SVCameraInfoStruct::GetIndex() const
{
	return m_SourceImageDMIndexHandle.GetIndex();
}

const SVDataManagerHandle& SVCameraInfoStruct::GetSourceImageDMIndexHandle() const
{
	return m_SourceImageDMIndexHandle;
}

SVProductInfoStruct::SVProductInfoStruct()
: m_ProductActive( 0 )
, m_lastInspectedSlot(-1)

{
	Reset();
}

SVProductInfoStruct::SVProductInfoStruct( const SVProductInfoStruct &p_rsvData )
: m_ProductState( p_rsvData.m_ProductState )
, bTriggered( p_rsvData.bTriggered )
, bStartAcquisition( p_rsvData.bStartAcquisition )
, bFinishAcquisition( p_rsvData.bFinishAcquisition )
, bDelayExpired( p_rsvData.bDelayExpired )
, bDataComplete( p_rsvData.bDataComplete )
, bStreamed( p_rsvData.bStreamed )
, hrPPQStatus( p_rsvData.hrPPQStatus )
, oTriggerInfo( p_rsvData.oTriggerInfo )
, oInputsInfo( p_rsvData.oInputsInfo )
, oOutputsInfo( p_rsvData.oOutputsInfo )
, oPPQInfo( p_rsvData.oPPQInfo )
, m_svCameraInfos( p_rsvData.m_svCameraInfos )
, m_svInspectionInfos( p_rsvData.m_svInspectionInfos )
, m_ProductActive( 0 )
, m_lastInspectedSlot(p_rsvData.m_lastInspectedSlot)
{
	if( p_rsvData.IsProductActive() )
	{
		SetProductActive();
	}
}

SVProductInfoStruct::~SVProductInfoStruct()
{
	SetProductComplete();
}

const SVProductInfoStruct &SVProductInfoStruct::operator=( const SVProductInfoStruct &p_rsvData )
{
	if( this != &p_rsvData )
	{
		SetProductComplete();

		m_ProductState = p_rsvData.m_ProductState;
		bTriggered = p_rsvData.bTriggered;
		bStartAcquisition = p_rsvData.bStartAcquisition;
		bFinishAcquisition = p_rsvData.bFinishAcquisition;
		bDelayExpired = p_rsvData.bDelayExpired;
		bDataComplete = p_rsvData.bDataComplete;
		bStreamed = p_rsvData.bStreamed;
		hrPPQStatus = p_rsvData.hrPPQStatus;

		oTriggerInfo = p_rsvData.oTriggerInfo;
		oInputsInfo = p_rsvData.oInputsInfo;
		oOutputsInfo = p_rsvData.oOutputsInfo;
		oPPQInfo = p_rsvData.oPPQInfo;

		m_svCameraInfos = p_rsvData.m_svCameraInfos;
		m_svInspectionInfos = p_rsvData.m_svInspectionInfos;
		m_lastInspectedSlot = p_rsvData.m_lastInspectedSlot;

		if( p_rsvData.IsProductActive() )
		{
			SetProductActive();
		}
	}

	return *this;
}

HRESULT SVProductInfoStruct::Assign( const SVProductInfoStruct &p_rsvData, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	if( this != &p_rsvData )
	{
		SetProductComplete();

		m_ProductState = p_rsvData.m_ProductState;
		bTriggered = p_rsvData.bTriggered;
		bStartAcquisition = p_rsvData.bStartAcquisition;
		bFinishAcquisition = p_rsvData.bFinishAcquisition;
		bDelayExpired = p_rsvData.bDelayExpired;
		bDataComplete = p_rsvData.bDataComplete;
		bStreamed = p_rsvData.bStreamed;
		hrPPQStatus = p_rsvData.hrPPQStatus;

		oTriggerInfo = p_rsvData.oTriggerInfo;
		oInputsInfo = p_rsvData.oInputsInfo;
		oOutputsInfo = p_rsvData.oOutputsInfo;

		l_Status = oPPQInfo.Assign( p_rsvData.oPPQInfo, p_LockType );

		m_lastInspectedSlot = p_rsvData.m_lastInspectedSlot;

		SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_Iter;
		SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::const_iterator l_RightIter;

		l_Iter = m_svCameraInfos.begin();

		while( l_Iter != m_svCameraInfos.end() )
		{
			l_RightIter = p_rsvData.m_svCameraInfos.find( l_Iter->first );

			if( l_RightIter == p_rsvData.m_svCameraInfos.end() )
			{
				l_Iter = m_svCameraInfos.erase( l_Iter );
			}
			else
			{
				++l_Iter;
			}
		}

		l_RightIter = p_rsvData.m_svCameraInfos.begin();

		while( l_RightIter != p_rsvData.m_svCameraInfos.end() )
		{
			HRESULT l_Temp = m_svCameraInfos[ l_RightIter->first ].Assign( l_RightIter->second, p_LockType );

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_RightIter;
		}

		m_svInspectionInfos.clear();

		SVGUIDSVInspectionInfoStructMap::const_iterator l_InspectIter = p_rsvData.m_svInspectionInfos.begin();

		while( l_InspectIter != p_rsvData.m_svInspectionInfos.end() )
		{
			HRESULT l_Temp = m_svInspectionInfos[ l_InspectIter->first ].Assign( l_InspectIter->second, p_LockType );

			if( l_Status == S_OK )
			{
				l_Status = l_Temp;
			}

			++l_InspectIter;
		}

		if( p_rsvData.IsProductActive() )
		{
			SetProductActive();
		}
	}

	return l_Status;
}

bool SVProductInfoStruct::empty() const
{
	return ( bTriggered == FALSE );
}

void SVProductInfoStruct::InitProductInfo()
{
	SetProductComplete();

	m_ProductState.clear();
	bTriggered			= FALSE;
	bStartAcquisition	= FALSE;
	bFinishAcquisition	= FALSE;
	bDelayExpired		= FALSE;
	bDataComplete		= FALSE;
	bStreamed           = FALSE;
	hrPPQStatus         = S_OK;
	m_lastInspectedSlot = -1;

	oInputsInfo.Init();
	oOutputsInfo.Init();
	oPPQInfo.InitPPQInfo();
	oTriggerInfo.Init();
	
	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svCameraIter = m_svCameraInfos.begin();

	while( l_svCameraIter != m_svCameraInfos.end() )
	{
		l_svCameraIter->second.ClearInfo();

		++l_svCameraIter;
	}

	SVGUIDSVInspectionInfoStructMap::iterator l_svInspectionIter = m_svInspectionInfos.begin();

	while( l_svInspectionIter != m_svInspectionInfos.end() )
	{
		l_svInspectionIter->second.Init();

		++l_svInspectionIter;
	}
}// end Init

void SVProductInfoStruct::Reset()
{
	SetProductComplete();

	m_ProductState.clear();
	bTriggered			= FALSE;
	bStartAcquisition	= FALSE;
	bFinishAcquisition	= FALSE;
	bDelayExpired		= FALSE;
	bDataComplete		= FALSE;
	bStreamed           = FALSE;
	hrPPQStatus         = S_OK;
	m_lastInspectedSlot = -1;

	oInputsInfo.Reset();
	oOutputsInfo.Reset();
	oPPQInfo.Reset();
	oTriggerInfo.Reset();
	
	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svCameraIter = m_svCameraInfos.begin();

	while( l_svCameraIter != m_svCameraInfos.end() )
	{
		l_svCameraIter->second.Reset();

		++l_svCameraIter;
	}

	SVGUIDSVInspectionInfoStructMap::iterator l_svInspectionIter = m_svInspectionInfos.begin();

	while( l_svInspectionIter != m_svInspectionInfos.end() )
	{
		l_svInspectionIter->second.Reset();

		++l_svInspectionIter;
	}
}// end Reset

void SVProductInfoStruct::ClearIndexes()
{
	oPPQInfo.ClearIndexes();

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svCameraIter = m_svCameraInfos.begin();

	while( l_svCameraIter != m_svCameraInfos.end() )
	{
		l_svCameraIter->second.ClearCameraInfo();

		++l_svCameraIter;
	}

	SVGUIDSVInspectionInfoStructMap::iterator l_svInspectionIter = m_svInspectionInfos.begin();

	while( l_svInspectionIter != m_svInspectionInfos.end() )
	{
		l_svInspectionIter->second.ClearIndexes();

		++l_svInspectionIter;
	}
}

HRESULT SVProductInfoStruct::GetNextAvailableIndexes( SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = oPPQInfo.GetNextAvailableIndexes( p_LockType );

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::iterator l_svCameraIter = m_svCameraInfos.begin();

	while( l_svCameraIter != m_svCameraInfos.end() )
	{
		HRESULT l_Temp = l_svCameraIter->second.GetNextAvailableIndexes( p_LockType );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_svCameraIter;
	}

	SVGUIDSVInspectionInfoStructMap::iterator l_svInspectionIter = m_svInspectionInfos.begin();

	while( l_svInspectionIter != m_svInspectionInfos.end() )
	{
		HRESULT l_Temp = l_svInspectionIter->second.GetNextAvailableIndexes( p_LockType );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}

		++l_svInspectionIter;
	}

	return l_Status;
}

bool SVProductInfoStruct::IsAlive() const
{
	bool l_Status = ! SVSVIMStateClass::CheckState( SV_STATE_RUNNING );

	if( ! l_Status && oPPQInfo.pPPQ != NULL )
	{
		l_Status = oPPQInfo.pPPQ->IsProductAlive( ProcessCount() );
	}

	return l_Status;
}

void SVProductInfoStruct::DumpIndexInfo( SVString& p_rData )
{
	p_rData.Format( _T( "TriggerCount=%ld-DataComplete=%s-ResultDataIndex=%ld-PublishedImageIndex=%ld" ),
		ProcessCount(),
		( bDataComplete ) ? _T( "T" ) : _T( "F" ),
		oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), 
		oPPQInfo.m_ResultImagePublishedDMIndexHandle.GetIndex() );

	SVStdMapSVVirtualCameraPtrSVCameraInfoStruct::const_iterator l_CamIter = m_svCameraInfos.begin();

	while( l_CamIter != m_svCameraInfos.end() )
	{
		SVString l_Temp;

		l_Temp.Format( _T( " : %s-Index=%ld" ), 
			( l_CamIter->first != NULL ) ? l_CamIter->first->GetName() : _T( "(null)" ), 
			l_CamIter->second.GetIndex() );

		p_rData += l_Temp;

		++l_CamIter;
	}

	SVGUIDSVInspectionInfoStructMap::const_iterator l_InspectIter = m_svInspectionInfos.begin();

	while( l_InspectIter != m_svInspectionInfos.end() )
	{
		SVString l_Temp;

		l_Temp.Format( _T( " : %s-State=0x%x-Index=%ld" ), 
			( l_InspectIter->second.pInspection != NULL ) ? l_InspectIter->second.pInspection->GetName() : _T( "(null)" ), 
			l_InspectIter->second.oInspectedState,
			l_InspectIter->second.m_ResultImageDMIndexHandle.GetIndex() );

		p_rData += l_Temp;

		++l_InspectIter;
	}
}

HRESULT SVProductInfoStruct::GetResultDataIndex( SVDataManagerHandle& p_rHandle ) const
{
	HRESULT l_Status = S_OK;

	const SVDataManagerHandle& l_rTemp = oPPQInfo.m_ResultDataDMIndexHandle;

	l_Status = p_rHandle.Assign( l_rTemp, l_rTemp.GetLockType() );

	return l_Status;
}

HRESULT SVProductInfoStruct::GetResultImageIndex( SVImageIndexStruct& p_rIndex, const SVGUID& p_rInspectionID ) const
{
	HRESULT l_Status = S_OK;

	SVGUIDSVInspectionInfoStructMap::const_iterator l_Iter = m_svInspectionInfos.find( p_rInspectionID );

	if( l_Iter != m_svInspectionInfos.end() )
	{
		HRESULT l_Temp = S_OK;

		l_Status = p_rIndex.m_ResultDMIndexHandle.Assign( l_Iter->second.m_ResultImageDMIndexHandle, 
			l_Iter->second.m_ResultImageDMIndexHandle.GetLockType() );

		l_Temp = p_rIndex.m_PublishedResultDMIndexHandle.Assign( oPPQInfo.m_ResultImagePublishedDMIndexHandle, 
			oPPQInfo.m_ResultImagePublishedDMIndexHandle.GetLockType() );

		if( l_Status == S_OK )
		{
			l_Status = l_Temp;
		}
	}

	return l_Status;
}

bool SVProductInfoStruct::IsProductActive() const
{
	return ( 0 < m_ProductActive );
}

void SVProductInfoStruct::SetProductActive()
{
	::InterlockedIncrement( &m_ProductActive );

	SVObjectManagerClass::Instance().IncrementProductIndicator();
}

void SVProductInfoStruct::SetProductComplete()
{
	if( 0 < m_ProductActive )
	{
		// Set ObjectManager product active to 0, then set local 
		// SVProductInfoStruct product active to 0.
		SVObjectManagerClass::Instance().AdjustProductIndicator( -m_ProductActive );

		::InterlockedExchange( &m_ProductActive, 0 );
	}

	SVGUIDSVInspectionInfoStructMap::iterator l_svInspectionIter = m_svInspectionInfos.begin();
	
	while( l_svInspectionIter != m_svInspectionInfos.end() )
	{
		l_svInspectionIter->second.m_CanProcess = false;
		l_svInspectionIter->second.m_InProcess = false;

		++l_svInspectionIter;
	}
}

SVInspectionCompleteInfoStruct::SVInspectionCompleteInfoStruct()
: m_InspectionID(), m_ProductInfo()
{
}

SVInspectionCompleteInfoStruct::SVInspectionCompleteInfoStruct( const SVGUID& p_rInspectionID, const SVProductInfoStruct& p_rProductInfo )
: m_InspectionID( p_rInspectionID ), m_ProductInfo( p_rProductInfo )
{
	ClearExtraInspectionIndexes();
}

SVInspectionCompleteInfoStruct::SVInspectionCompleteInfoStruct( const SVInspectionCompleteInfoStruct& p_rObject )
: m_InspectionID( p_rObject.m_InspectionID ), m_ProductInfo( p_rObject.m_ProductInfo )
{
	ClearExtraInspectionIndexes();
}

SVInspectionCompleteInfoStruct::~SVInspectionCompleteInfoStruct()
{
}

HRESULT SVInspectionCompleteInfoStruct::Assign( const SVGUID& p_rInspectionID, const SVProductInfoStruct& p_rProductInfo, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	m_InspectionID = p_rInspectionID;

	l_Status = m_ProductInfo.Assign( p_rProductInfo, p_LockType );

	ClearExtraInspectionIndexes();

	return l_Status;
}

HRESULT SVInspectionCompleteInfoStruct::Assign( const SVInspectionCompleteInfoStruct& p_rObject, SVDataManagerLockTypeEnum p_LockType )
{
	HRESULT l_Status = S_OK;

	l_Status = Assign( p_rObject.m_InspectionID, p_rObject.m_ProductInfo, p_LockType );

	ClearExtraInspectionIndexes();

	return l_Status;
}

bool SVInspectionCompleteInfoStruct::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && m_InspectionID.empty();
	l_Status = l_Status && m_ProductInfo.empty();

	return l_Status;
}

void SVInspectionCompleteInfoStruct::clear()
{
	m_InspectionID.clear();
	m_ProductInfo.Reset();
}

void SVInspectionCompleteInfoStruct::ClearExtraInspectionIndexes()
{
	SVGUIDSVInspectionInfoStructMap::iterator l_Iter( m_ProductInfo.m_svInspectionInfos.begin() );

	while( l_Iter != m_ProductInfo.m_svInspectionInfos.end() )
	{
		if( m_InspectionID != l_Iter->first )
		{
			l_Iter->second.m_ResultImageDMIndexHandle.clear();
		}

		++l_Iter;
	}
}

SVInspectionNameUpdate::SVInspectionNameUpdate()
: m_InspectionName()
{
}

SVInspectionNameUpdate::SVInspectionNameUpdate( const SVString& p_rName )
: m_InspectionName( p_rName )
{
}

SVInspectionNameUpdate::SVInspectionNameUpdate( const SVInspectionNameUpdate& p_rObject )
: m_InspectionName( p_rObject.m_InspectionName )
{
}

SVInspectionNameUpdate::~SVInspectionNameUpdate()
{
}

SVRenameObject::SVRenameObject()
: m_OldName(), m_NewName(), m_ObjectId()
{
}

SVRenameObject::SVRenameObject( const SVString& p_rOldName, const SVString& p_rNewName, const SVGUID& p_rObjectId )
: m_OldName( p_rOldName ), m_NewName( p_rNewName ), m_ObjectId( p_rObjectId )
{
}

SVRenameObject::SVRenameObject( const SVRenameObject& p_rObject )
: m_OldName( p_rObject.m_OldName ), m_NewName( p_rObject.m_NewName ), m_ObjectId( p_rObject.m_ObjectId )
{
}

SVRenameObject::~SVRenameObject()
{
}

SVAddTool::SVAddTool()
: m_pTool( NULL ), m_Index( -1 )
{
}

SVAddTool::SVAddTool( SVToolClass* p_pTool, int p_Index )
: m_pTool( p_pTool ), m_Index( p_Index )
{
}

SVAddTool::SVAddTool( const SVAddTool& p_rObject )
: m_pTool( p_rObject.m_pTool ), m_Index( p_rObject.m_Index )
{
}

SVAddTool::~SVAddTool()
{
}

SVDeleteTool::SVDeleteTool()
: m_pTool( NULL )
{
}

SVDeleteTool::SVDeleteTool( SVToolClass* p_pTool )
: m_pTool( p_pTool )
{
}

SVDeleteTool::SVDeleteTool( const SVDeleteTool& p_rObject )
: m_pTool( p_rObject.m_pTool )
{
}

SVDeleteTool::~SVDeleteTool()
{
}

SVRemoveImages::SVRemoveImages()
: m_Images()
{
}

SVRemoveImages::SVRemoveImages( const SVImageIdSet& p_rImages )
: m_Images( p_rImages )
{
}

SVRemoveImages::SVRemoveImages( const SVRemoveImages& p_rObject )
: m_Images( p_rObject.m_Images )
{
}

SVRemoveImages::~SVRemoveImages()
{
}

SVRemoveValues::SVRemoveValues()
: m_Values()
{
}

SVRemoveValues::SVRemoveValues( const SVValueIdSet& p_rValues )
: m_Values( p_rValues )
{
}

SVRemoveValues::SVRemoveValues( const SVRemoveValues& p_rObject )
: m_Values( p_rObject.m_Values )
{
}

SVRemoveValues::~SVRemoveValues()
{
}

SVRemoveSubjectStruct::SVRemoveSubjectStruct()
{
}

SVRemoveSubjectStruct::~SVRemoveSubjectStruct()
{
}

SVInputRequestInfoStruct::SVInputRequestInfoStruct()
: refValueObject(), m_Value()
{
	Reset();
}

SVInputRequestInfoStruct::SVInputRequestInfoStruct( const SVValueObjectReference& p_rValueObject, const _variant_t& p_rValue )
: refValueObject( p_rValueObject ), m_Value( p_rValue )
{
}

SVInputRequestInfoStruct::~SVInputRequestInfoStruct()
{
	Reset();
}

void SVInputRequestInfoStruct::Reset()
{
	refValueObject = SVValueObjectReference();
	m_Value.Clear();
}// end Reset

void SVInputRequestInfoStruct::Init()
{
}// end Init

SVInputImageRequestInfoStruct::SVInputImageRequestInfoStruct()
: m_strObjectName()
, m_ImageInfo()
, m_ImageHandlePtr()
, m_bUsingCameraName( false )
{
}

SVInputImageRequestInfoStruct::~SVInputImageRequestInfoStruct()
{
	Reset();
}

void SVInputImageRequestInfoStruct::Reset()
{
	m_strObjectName.Empty();
}// end Reset

void SVInputImageRequestInfoStruct::Init()
{
	m_strObjectName.Empty();
}// end Init

SVOutputRequestInfoStruct::SVOutputRequestInfoStruct()
{
	Reset();
}

SVOutputRequestInfoStruct::~SVOutputRequestInfoStruct()
{
	Reset();
}

void SVOutputRequestInfoStruct::Reset()
{
	m_ProductInfo.Reset();

	pCaller		 = NULL;
	pOwner		 = NULL;
	pCallback	 = NULL;
}// end Reset

void SVOutputRequestInfoStruct::Init()
{
	m_ProductInfo.Reset();

	pCaller		 = NULL;
	pOwner		 = NULL;
	pCallback	 = NULL;
}// end Init

SVProductInfoRequestStruct::SVProductInfoRequestStruct()
: m_pProduct( NULL ), m_WaitHandle( NULL )
{
	m_WaitHandle = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

SVProductInfoRequestStruct::SVProductInfoRequestStruct( SVProductInfoStruct* p_pProduct )
: m_pProduct( p_pProduct ), m_WaitHandle( NULL )
{
	m_WaitHandle = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

SVProductInfoRequestStruct::SVProductInfoRequestStruct( const SVProductInfoRequestStruct& p_rObject )
: m_pProduct( p_rObject.m_pProduct ), m_WaitHandle( NULL )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVProductInfoRequestStruct::~SVProductInfoRequestStruct()
{
	if( m_WaitHandle != NULL )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = NULL;
	}
}

const SVProductInfoRequestStruct& SVProductInfoRequestStruct::operator=( const SVProductInfoRequestStruct& p_rObject )
{
	if( this != &p_rObject )
	{
		m_pProduct = p_rObject.m_pProduct;

		p_rObject.GetWaitHandle( m_WaitHandle );
	}

	return *this;
}

bool SVProductInfoRequestStruct::empty() const
{
	return ( m_pProduct == NULL );
}

void SVProductInfoRequestStruct::clear()
{
	if( m_pProduct != NULL )
	{
		m_pProduct = NULL;
	}

	if( m_WaitHandle != NULL )
	{
		::ResetEvent( m_WaitHandle );
	}
}

HRESULT SVProductInfoRequestStruct::NotifyRequestComplete() const
{
	HRESULT l_Status = S_OK;

	if( m_WaitHandle != NULL )
	{
		::SetEvent( m_WaitHandle );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVProductInfoRequestStruct::WaitForRequest() const
{
	HRESULT l_Status = S_OK;

	if( m_WaitHandle != NULL )
	{
		if( ::ResetEvent( m_WaitHandle ) )
		{
			if( ::WaitForSingleObject( m_WaitHandle, INFINITE ) != WAIT_OBJECT_0 )
			{
				l_Status = E_FAIL;
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

HRESULT SVProductInfoRequestStruct::GetWaitHandle( HANDLE& p_rHandle ) const
{
	HRESULT l_Status = S_OK;

	if( p_rHandle != NULL )
	{
		::CloseHandle( p_rHandle );

		p_rHandle = NULL;
	}

	if( ! ::DuplicateHandle( GetCurrentProcess(), m_WaitHandle, GetCurrentProcess(), &p_rHandle, 0, FALSE, DUPLICATE_SAME_ACCESS ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVInfoStructs.cpp_v  $
 * 
 *    Rev 1.6   16 Dec 2014 06:54:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  977
 * SCR Title:  Fix Double Processing of Inspections with 2 Inspections on 1 PPQ
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Fixed merge error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   11 Dec 2014 06:45:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  977
 * SCR Title:  Fix Double Processing of Inspections with 2 Inspections on 1 PPQ
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initializer list for copy constructor SVInspectionInfoStruct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Dec 2014 06:44:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  977
 * SCR Title:  Fix Double Processing of Inspections with 2 Inspections on 1 PPQ
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added flag to InspectionInfoStruct to prevent additional queuing in Inspection::StartProcess from the PPQ::StartInspection. New Flag m_HasBeenQueued.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   14 Aug 2014 18:08:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added m_lastInspectedSlot to SVInspectionInfoStruct.
 * Revised SVInspectionInfoStruct.constructor, copy constructor,  Assign, Reset and Init methods for m_lastInspectedSlot.
 * Added m_lastInspectedSlot to SVProductInfoStruct.
 * Revised SVProductInfoStruct constructor, copy constructor, assignment operator, Assign, InitProdictInfo, Reset methods for m_lastInspectedSlot.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   13 May 2013 12:16:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.37   02 May 2013 09:54:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated variable name for the trigger toggle state variable from trigger toggle to toggle state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 16:16:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.36   29 Apr 2013 14:15:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:59:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.35   18 Feb 2013 13:21:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated inspection input info structs from using a CString as the data element to a variant.  Updated all methods that call the inspection interface from CString to variant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.34   21 Jan 2013 10:52:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised SVTriggerInfoStruct to have a member variable m_data of type boost::any
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.33   10 Jan 2013 16:15:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Re-arranged log comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.32   13 Dec 2012 10:42:42   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (Assign) Added #ifdef to avoid assert if a nak occurs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.31   11 Oct 2012 09:35:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged changes in to main line of code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.30   04 Oct 2012 10:45:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29.1.1   12 Dec 2012 19:31:40   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  802
 * SCR Title:  Add new product type GD1A
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   (Assign) Added #ifdef to avoid assert if a nak occurs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29.1.0   11 Oct 2012 08:53:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  795
 * SCR Title:  Fix the Inspection State Problem with the Product Information Structure
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a new start process state to the inspection info struct and update the variable when the inspection is queued and whe the product gets reset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.29   10 Jul 2012 12:15:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added operator= to class/structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.28   09 Jul 2012 14:03:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to change the include for SVIM State.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.27   02 Jul 2012 16:58:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  769
 * SCR Title:  Fix Problems and Crashes with Inspection Document Display Updates
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to promote new display functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.26   19 Jun 2012 12:50:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Optimized the application state functionality.  The application state class was altered to simplify its usage and elements were added to object manager to separate tracking states.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.25   16 Sep 2011 16:00:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated objects to use new resource management functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.24   14 Jul 2011 16:19:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to remove un-used product info struct attribute and inspection attribute.  Added logging attribute to product info struct for product state.  Remove extra Inspection Complete notification.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.23   14 Jul 2011 08:59:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code to better track resource management, and clear un-used resources.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.22   05 Jul 2011 14:45:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  724
 * SCR Title:  Add Resource Logging to SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to for Last Inspected to defined reference catagories.  Added new logging based on low availiable resources.  Added offline Data Manager dumps of reference counts.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.21   18 Apr 2011 10:15:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to move the shared data for the IO Entry data to a different class to fix issues with PPQ and Inspection shared data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.20   15 Dec 2010 10:00:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix connections and synchronization issues with the IO Sub-system.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.19   08 Dec 2010 12:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.18   16 Feb 2010 10:16:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Undated Trigger Information structure to include the trigger toggle state.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.17   19 Jan 2010 15:56:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated inspection queueing functionality to handle floundering inspections.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   18 Aug 2009 08:42:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix problem with trigger toggle and output delays.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   14 Jul 2009 14:00:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to use new data manager objects.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   02 Nov 2005 08:30:32   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  512
 * SCR Title:  Add Published Result Images functionality
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added published result image variables
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Aug 2005 10:16:26   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  504
 * SCR Title:  Fix lock-up problem when pending product count is larger than PPQ size
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to include changes to SVProductInfoStruct and supporting classes to handle proper assignment and copy construction.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   10 Aug 2005 12:32:30   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  500
 * SCR Title:  Reduce delay when adjusting tool parameters with a large toolset
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated methods to improve performance and reduce system delay.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Jul 2005 11:29:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added m_bUsingCameraName to use when looking up for main image
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   27 Jun 2005 14:54:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   use SVValueObjectReference
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   02 Nov 2004 15:07:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  459
 * SCR Title:  Fix a problem with inspection lock-up
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added hrPPQStatus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Sep 2003 14:34:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  381
 * SCR Title:  Combine PPQ Data to Inspection Data for a Digital Output
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Add code to support the combining of digital outputs with PPQ values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   17 Sep 2003 13:02:50   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  384
 * SCR Title:  Add new Matrox OCR/OCV analyzer to SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Created and integrated a new Matrox based OCR/OCV analyzer to SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   17 Jun 2003 15:18:06   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  374
 * SCR Title:  Add check before changing tool parameters when processing input list
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added prototype code for using a map in the inspection to lookup objects that are used in the input request queue. It will be finalized after further testing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Apr 2003 10:25:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   19 Mar 2003 13:27:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  330
 * SCR Title:  Add SetImageList to ActiveX Interface
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new struct - SVInputImageRequestInfoStruct
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   20 Dec 2002 19:59:30   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  305
 * SCR Title:  Implement the ability to perform RunOnce from a SIAC client
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified SVInputRequestInfoStruct to use a CString instead of a char*
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Dec 2002 14:40:10   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  300
 * SCR Title:  Create an interface for SVObserver to Push Output data to a SIAC Client
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added the bStreamed flag
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   09 Dec 2002 19:29:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  314
 * SCR Title:  SVIM stops inspecting when triggered very fast
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Updated to fix problems and track issues with result data indexes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:46:04   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  272
 * SCR Title:  Outputs do not always get set correctly.
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
