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

#pragma region Includes
#include "stdafx.h"
#include "SVInfoStructs.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVInspectionProcess.h"
#include "SVPPQObject.h"
#include "SVStatusLibrary/SVSVIMStateClass.h"
#pragma endregion Includes

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
	return (nullptr == m_IOEntryPtr);
}

void SVIOEntryStruct::clear()
{
	m_IOEntryPtr.reset();

	m_EntryValid = false;
	m_CombinedValue = false;
}

void SVIOEntryStruct::InitEntry()
{
	m_EntryValid = false;
	m_CombinedValue = false;
}


SVInputsInfoStruct::SVInputsInfoStruct()
: pInputsList( nullptr )
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
	pInputsList		= nullptr;
	m_BeginProcess	= 0;
	m_EndProcess	= 0;
}// end Reset

void SVInputsInfoStruct::Init()
{
	m_BeginProcess = 0;
	m_EndProcess = 0;
}// end Init


SVOutputsInfoStruct::SVOutputsInfoStruct()
: pOutputsList( nullptr )
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
	pOutputsList	= nullptr;
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

		if( S_OK == l_Status )
		{
			l_Status = l_Temp;
		}

		l_Temp = m_ResultImagePublishedDMIndexHandle.Assign( p_rsvObject.m_ResultImagePublishedDMIndexHandle, p_rsvObject.m_ResultImagePublishedDMIndexHandle.GetLockType() );
	
		if( S_OK == l_Status )
		{
			l_Status = l_Temp;
		}
	}
	
	return l_Status;
}

void SVPPQInfoStruct::Reset()
{
	pPPQ = nullptr;

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

	if( nullptr != pPPQ )
	{
		l_Status = pPPQ->GetNextAvailableIndexes( *this, p_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}


SVInspectionInfoStruct::SVInspectionInfoStruct()
:	pInspection( nullptr ),
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
:	pInspection( nullptr ),
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

		if( S_OK == l_Status )
		{
			l_Status = l_Temp;
		}
	}
	
	return l_Status;
}

void SVInspectionInfoStruct::Reset()
{
	pInspection = nullptr;
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

	if( nullptr != pInspection )
	{
		pInspection->GetNextAvailableIndexes( *this, p_LockType );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
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
		SVGuidSVCameraInfoStructMap::iterator l_Iter;
		SVGuidSVCameraInfoStructMap::const_iterator l_RightIter;

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

			if( S_OK == l_Status )
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

			if( S_OK == l_Status )
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
	
	SVGuidSVCameraInfoStructMap::iterator l_svCameraIter = m_svCameraInfos.begin();

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
	
	SVGuidSVCameraInfoStructMap::iterator l_svCameraIter = m_svCameraInfos.begin();

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

	SVGuidSVCameraInfoStructMap::iterator l_svCameraIter = m_svCameraInfos.begin();

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

	SVGuidSVCameraInfoStructMap::iterator CameraIter( m_svCameraInfos.begin() );

	for( ;m_svCameraInfos.end() != CameraIter; ++CameraIter )
	{
		HRESULT l_Temp = CameraIter->second.GetNextAvailableIndexes( p_LockType );
		if( S_OK == l_Status )
		{
			l_Status = l_Temp;
		}
	}

	SVGUIDSVInspectionInfoStructMap::iterator l_svInspectionIter = m_svInspectionInfos.begin();

	while( l_svInspectionIter != m_svInspectionInfos.end() )
	{
		HRESULT l_Temp = l_svInspectionIter->second.GetNextAvailableIndexes( p_LockType );

		if( S_OK == l_Status )
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

	if( ! l_Status && nullptr != oPPQInfo.pPPQ )
	{
		l_Status = oPPQInfo.pPPQ->IsProductAlive( ProcessCount() );
	}

	return l_Status;
}

void SVProductInfoStruct::DumpIndexInfo( std::string& rData )
{
	rData = SvUl::Format( _T( "TriggerCount=%ld-DataComplete=%s-ResultDataIndex=%ld-PublishedImageIndex=%ld" ),
		ProcessCount(),
		( bDataComplete ) ? _T( "T" ) : _T( "F" ),
		oPPQInfo.m_ResultDataDMIndexHandle.GetIndex(), 
		oPPQInfo.m_ResultImagePublishedDMIndexHandle.GetIndex() );

	SVGuidSVCameraInfoStructMap::const_iterator CamIter = m_svCameraInfos.begin();

	for( ; CamIter != m_svCameraInfos.end(); ++CamIter )
	{
		if( GUID_NULL != CamIter->first )
		{
			std::string CameraName( _T( "(null)" ) );

			SvOi::IObjectClass* pCamera =  SvOi::getObject( CamIter->first );
			if( nullptr != pCamera )
			{
				CameraName = pCamera->GetName();
			}

			rData += SvUl::Format( _T( " : %s-Index=%ld" ), CameraName.c_str(), CamIter->second.GetIndex() );
		}
	}

	SVGUIDSVInspectionInfoStructMap::const_iterator l_InspectIter = m_svInspectionInfos.begin();

	while( l_InspectIter != m_svInspectionInfos.end() )
	{
		std::string l_Temp = SvUl::Format( _T( " : %s-State=0x%x-Index=%ld" ), 
			( nullptr != l_InspectIter->second.pInspection ) ? l_InspectIter->second.pInspection->GetName() : _T( "(null)" ), 
			l_InspectIter->second.oInspectedState,
			l_InspectIter->second.m_ResultImageDMIndexHandle.GetIndex() );

		rData += l_Temp;

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

		if( S_OK == l_Status )
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

SVInspectionNameUpdate::SVInspectionNameUpdate( const std::string& p_rName )
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

SVAddTool::SVAddTool()
: m_pTool( nullptr ), m_Index( -1 )
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
: m_pTool( nullptr )
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

SVRemoveImages::SVRemoveImages( const SVGuidSet& p_rImages )
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

SVRemoveValues::SVRemoveValues( const SVGuidSet& p_rValues )
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
: m_ValueObjectRef(), m_Value()
{
	Reset();
}

SVInputRequestInfoStruct::SVInputRequestInfoStruct( const SVObjectReference& rValueObject, const _variant_t& rValue )
: m_ValueObjectRef( rValueObject ), m_Value( rValue )
{
}

SVInputRequestInfoStruct::~SVInputRequestInfoStruct()
{
	Reset();
}

void SVInputRequestInfoStruct::Reset()
{
	m_ValueObjectRef = SVObjectReference();
	m_Value.Clear();
}// end Reset

void SVInputRequestInfoStruct::Init()
{
}// end Init

SVInputImageRequestInfoStruct::SVInputImageRequestInfoStruct()
: m_ObjectName()
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
	m_ObjectName.clear();
}// end Reset

void SVInputImageRequestInfoStruct::Init()
{
	m_ObjectName.clear();
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

	pCaller		 = nullptr;
	pOwner		 = nullptr;
	pCallback	 = nullptr;
}// end Reset

void SVOutputRequestInfoStruct::Init()
{
	m_ProductInfo.Reset();

	pCaller		 = nullptr;
	pOwner		 = nullptr;
	pCallback	 = nullptr;
}// end Init

SVProductInfoRequestStruct::SVProductInfoRequestStruct()
: m_pProduct( nullptr ), m_WaitHandle( nullptr )
{
	m_WaitHandle = ::CreateEvent( nullptr, TRUE, FALSE, nullptr );
}

SVProductInfoRequestStruct::SVProductInfoRequestStruct( SVProductInfoStruct* p_pProduct )
: m_pProduct( p_pProduct ), m_WaitHandle( nullptr )
{
	m_WaitHandle = ::CreateEvent( nullptr, TRUE, FALSE, nullptr );
}

SVProductInfoRequestStruct::SVProductInfoRequestStruct( const SVProductInfoRequestStruct& p_rObject )
: m_pProduct( p_rObject.m_pProduct ), m_WaitHandle( nullptr )
{
	p_rObject.GetWaitHandle( m_WaitHandle );
}

SVProductInfoRequestStruct::~SVProductInfoRequestStruct()
{
	if( nullptr != m_WaitHandle )
	{
		::CloseHandle( m_WaitHandle );

		m_WaitHandle = nullptr;
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
	return ( nullptr == m_pProduct );
}

void SVProductInfoRequestStruct::clear()
{
	if( nullptr != m_pProduct )
	{
		m_pProduct = nullptr;
	}

	if( nullptr != m_WaitHandle )
	{
		::ResetEvent( m_WaitHandle );
	}
}

HRESULT SVProductInfoRequestStruct::NotifyRequestComplete() const
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_WaitHandle )
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

	if( nullptr != m_WaitHandle )
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

	if( nullptr != p_rHandle )
	{
		::CloseHandle( p_rHandle );

		p_rHandle = nullptr;
	}

	if( ! ::DuplicateHandle( GetCurrentProcess(), m_WaitHandle, GetCurrentProcess(), &p_rHandle, 0, FALSE, DUPLICATE_SAME_ACCESS ) )
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

