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


SVInputsInfoStruct::SVInputsInfoStruct()
{
}

SVInputsInfoStruct::SVInputsInfoStruct( const SVInputsInfoStruct& rRhs )
{
	m_Inputs = rRhs.m_Inputs;
}

SVInputsInfoStruct::~SVInputsInfoStruct()
{
	Reset();
}

const SVInputsInfoStruct& SVInputsInfoStruct::operator=( const SVInputsInfoStruct& rRhs )
{
	if( this != &rRhs )
	{
		m_Inputs = rRhs.m_Inputs;
	}

	return *this;
}

void SVInputsInfoStruct::Reset()
{
	m_Inputs.clear();
}// end Reset

void SVInputsInfoStruct::Init()
{
}// end Init


SVOutputsInfoStruct::SVOutputsInfoStruct()
: m_OutputDelay{0L}
, m_ResetDelay{0L}
, m_DataValidDelay{0L}
, m_DataValidResult{false}
, m_OutputToggleResult{false}
, m_NakResult{true}
, m_EndOutputDelay {0LL}
, m_EndResetDelay {0LL}
, m_EndDataValidDelay {0LL}
, m_BeginProcess {0LL}
, m_EndProcess {0LL}
{
}

SVOutputsInfoStruct::SVOutputsInfoStruct( const SVOutputsInfoStruct& rRhs )
: m_OutputDelay{rRhs.m_OutputDelay}
, m_ResetDelay{rRhs.m_ResetDelay}
, m_DataValidDelay{rRhs.m_DataValidDelay}
, m_DataValidResult{rRhs.m_DataValidResult}
, m_OutputToggleResult {rRhs.m_OutputToggleResult}
, m_NakResult {rRhs.m_NakResult}
, m_EndOutputDelay{rRhs.m_EndOutputDelay}
, m_EndResetDelay{rRhs.m_EndResetDelay}
, m_EndDataValidDelay{rRhs.m_EndDataValidDelay}
, m_BeginProcess{rRhs.m_BeginProcess}
, m_EndProcess{rRhs.m_EndProcess}
, m_Outputs{rRhs.m_Outputs}
{
}

SVOutputsInfoStruct::~SVOutputsInfoStruct()
{
}

const SVOutputsInfoStruct& SVOutputsInfoStruct::operator=( const SVOutputsInfoStruct& rRhs )
{
	if( this != &rRhs )
	{
		m_OutputDelay = rRhs.m_OutputDelay;
		m_ResetDelay = rRhs.m_ResetDelay;
		m_DataValidDelay = rRhs.m_DataValidDelay;
		m_DataValidResult = rRhs.m_DataValidResult;
		m_OutputToggleResult = rRhs.m_OutputToggleResult;
		m_NakResult = rRhs.m_NakResult;
		m_EndOutputDelay = rRhs.m_EndOutputDelay;
		m_EndResetDelay = rRhs.m_EndResetDelay;
		m_EndDataValidDelay = rRhs.m_EndDataValidDelay;
		m_BeginProcess = rRhs.m_BeginProcess;
		m_EndProcess = rRhs.m_EndProcess;
		m_Outputs = rRhs.m_Outputs;
	}

	return *this;
}

void SVOutputsInfoStruct::Reset()
{
	m_OutputDelay	= 0;
	m_ResetDelay		= 0;
	m_DataValidDelay	= 0;
	m_DataValidResult = false;
	m_OutputToggleResult = false;
	m_NakResult = true;
	m_EndOutputDelay			= 0;
	m_EndResetDelay				= 0;
	m_EndDataValidDelay			= 0;
	m_BeginProcess				= 0;
	m_EndProcess				= 0;
	m_Outputs.clear();
}// end Reset

void SVOutputsInfoStruct::Init()
{
	//Init only initializes these values because the rest of the values are required for the next process cycle
	m_EndOutputDelay			= 0;
	m_EndResetDelay				= 0;
	m_EndDataValidDelay			= 0;
	m_BeginProcess				= 0;
	m_EndProcess				= 0;
	m_NakResult = true;
	m_Outputs.clear();
}// end Init


SVPPQInfoStruct::SVPPQInfoStruct()
: pPPQ()
, m_ResultDataDMIndexHandle()
{
}

SVPPQInfoStruct::SVPPQInfoStruct( const SVPPQInfoStruct& p_rsvObject )
: pPPQ( p_rsvObject.pPPQ )
, m_ResultDataDMIndexHandle()
{
	m_ResultDataDMIndexHandle.Assign( p_rsvObject.m_ResultDataDMIndexHandle, p_rsvObject.m_ResultDataDMIndexHandle.GetLockType() );
}

SVPPQInfoStruct::~SVPPQInfoStruct()
{
	Reset();
}

HRESULT SVPPQInfoStruct::Assign( const SVPPQInfoStruct &p_rsvObject )
{
	HRESULT l_Status = S_OK;

	if( this != &p_rsvObject )
	{
		pPPQ = p_rsvObject.pPPQ;

		l_Status = m_ResultDataDMIndexHandle.Assign( p_rsvObject.m_ResultDataDMIndexHandle, p_rsvObject.m_ResultDataDMIndexHandle.GetLockType() );
	}
	
	return l_Status;
}

void SVPPQInfoStruct::Reset()
{
	pPPQ = nullptr;

	ClearIndexes();
}

void SVPPQInfoStruct::InitPPQInfo()
{
	ClearIndexes();
}

void SVPPQInfoStruct::ClearIndexes()
{
	m_ResultDataDMIndexHandle.clear();
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
	Assign( p_rsvData );
}

SVInspectionInfoStruct::~SVInspectionInfoStruct()
{
	Reset();
}

const SVInspectionInfoStruct &SVInspectionInfoStruct::operator=( const SVInspectionInfoStruct &p_rsvData )
{
	Assign( p_rsvData );
	return *this;
}

HRESULT SVInspectionInfoStruct::Assign( const SVInspectionInfoStruct &p_rsvData )
{
	HRESULT l_Status = S_OK;

	if( this != &p_rsvData )
	{
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
		m_triggerRecordWrite = p_rsvData.m_triggerRecordWrite;
		m_triggerRecordComplete = p_rsvData.m_triggerRecordComplete;
		m_inspectionPosInTrc = p_rsvData.m_inspectionPosInTrc;
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

	m_triggerRecordWrite = nullptr;
	m_triggerRecordComplete = nullptr;
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

	m_triggerRecordWrite = nullptr;
	m_triggerRecordComplete = nullptr;
	m_lastInspectedSlot = -1;

	ClearIndexes();
}// end Init

void SVInspectionInfoStruct::ClearIndexes()
{
	m_CanProcess = false;
	m_InProcess = false;
}

HRESULT SVInspectionInfoStruct::GetNextAvailableIndexes( )
{
	HRESULT l_Status = S_OK;

	if( nullptr != pInspection )
	{
		setNextTriggerRecord();
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

void SVInspectionInfoStruct::setNextTriggerRecord()
{
	m_triggerRecordWrite = SvTrc::getTriggerRecordControllerRWInstance().createTriggerRecordObjectToWrite(m_inspectionPosInTrc);
	m_triggerRecordComplete = nullptr;
}

void SVInspectionInfoStruct::setTriggerRecordCompleted()
{
	if (nullptr != m_triggerRecordWrite)
	{
		m_triggerRecordComplete = SvTrc::getTriggerRecordControllerRWInstance().closeWriteAndOpenReadTriggerRecordObject(m_triggerRecordWrite);
	}
}

SVProductInfoStruct::SVProductInfoStruct()
: m_ProductActive( 0 )
, m_lastInspectedSlot(-1)

{
	Reset();
}

SVProductInfoStruct::SVProductInfoStruct( const SVProductInfoStruct &rRhs )
: m_ProductState( rRhs.m_ProductState )
, bTriggered( rRhs.bTriggered )
, bDelayExpired( rRhs.bDelayExpired )
, bDataComplete( rRhs.bDataComplete )
, bStreamed( rRhs.bStreamed )
, hrPPQStatus( rRhs.hrPPQStatus )
, oTriggerInfo( rRhs.oTriggerInfo )
, oInputsInfo( rRhs.oInputsInfo )
, oOutputsInfo( rRhs.oOutputsInfo )
, oPPQInfo( rRhs.oPPQInfo )
, m_svCameraInfos( rRhs.m_svCameraInfos )
, m_svInspectionInfos( rRhs.m_svInspectionInfos )
, m_ProductActive( 0 )
, m_lastInspectedSlot(rRhs.m_lastInspectedSlot)
{
	for(int i=0; i < SvDef::cMaximumCameras; ++i)
	{
		bhasCameraImage[i] = rRhs.bhasCameraImage[i];
	}
	if( rRhs.IsProductActive() )
	{
		SetProductActive();
	}
}

SVProductInfoStruct::~SVProductInfoStruct()
{
	SetProductComplete();
	Reset();
}

const SVProductInfoStruct &SVProductInfoStruct::operator=( const SVProductInfoStruct &rRhs )
{
	if( this != &rRhs )
	{
		SetProductComplete();

		m_ProductState = rRhs.m_ProductState;
		bTriggered = rRhs.bTriggered;

		for (int i = 0; i < SvDef::cMaximumCameras; ++i)
		{
			bhasCameraImage[i] = rRhs.bhasCameraImage[i];
		}
		bDelayExpired = rRhs.bDelayExpired;
		bDataComplete = rRhs.bDataComplete;
		bStreamed = rRhs.bStreamed;
		hrPPQStatus = rRhs.hrPPQStatus;

		oTriggerInfo = rRhs.oTriggerInfo;
		oInputsInfo = rRhs.oInputsInfo;
		oOutputsInfo = rRhs.oOutputsInfo;
		oPPQInfo = rRhs.oPPQInfo;

		m_svCameraInfos = rRhs.m_svCameraInfos;
		m_svInspectionInfos = rRhs.m_svInspectionInfos;
		m_lastInspectedSlot = rRhs.m_lastInspectedSlot;
		if( rRhs.IsProductActive() )
		{
			SetProductActive();
		}
	}

	return *this;
}

HRESULT SVProductInfoStruct::Assign( const SVProductInfoStruct &rData )
{
	HRESULT l_Status = S_OK;

	if( this != &rData )
	{
		SetProductComplete();

		m_ProductState = rData.m_ProductState;
		bTriggered = rData.bTriggered;

		for (int i = 0; i < SvDef::cMaximumCameras; ++i)
		{
			bhasCameraImage[i] = rData.bhasCameraImage[i];
		}
		bDelayExpired = rData.bDelayExpired;
		bDataComplete = rData.bDataComplete;
		bStreamed = rData.bStreamed;
		hrPPQStatus = rData.hrPPQStatus;

		oTriggerInfo = rData.oTriggerInfo;
		oInputsInfo = rData.oInputsInfo;
		oOutputsInfo = rData.oOutputsInfo;

		l_Status = oPPQInfo.Assign( rData.oPPQInfo );

		m_lastInspectedSlot = rData.m_lastInspectedSlot;
		SVGuidSVCameraInfoStructMap::iterator l_Iter;
		SVGuidSVCameraInfoStructMap::const_iterator l_RightIter;

		l_Iter = m_svCameraInfos.begin();

		while( l_Iter != m_svCameraInfos.end() )
		{
			l_RightIter = rData.m_svCameraInfos.find( l_Iter->first );

			if( l_RightIter == rData.m_svCameraInfos.end() )
			{
				l_Iter = m_svCameraInfos.erase( l_Iter );
			}
			else
			{
				++l_Iter;
			}
		}

		l_RightIter = rData.m_svCameraInfos.begin();

		while( l_RightIter != rData.m_svCameraInfos.end() )
		{
			HRESULT l_Temp = m_svCameraInfos[ l_RightIter->first ].Assign( l_RightIter->second );

			if( S_OK == l_Status )
			{
				l_Status = l_Temp;
			}

			++l_RightIter;
		}

		m_svInspectionInfos.clear();

		SVGUIDSVInspectionInfoStructMap::const_iterator l_InspectIter = rData.m_svInspectionInfos.begin();

		while( l_InspectIter != rData.m_svInspectionInfos.end() )
		{
			HRESULT l_Temp = m_svInspectionInfos[ l_InspectIter->first ].Assign( l_InspectIter->second );

			if( S_OK == l_Status )
			{
				l_Status = l_Temp;
			}

			++l_InspectIter;
		}

		if( rData.IsProductActive() )
		{
			SetProductActive();
		}
	}

	return l_Status;
}

bool SVProductInfoStruct::empty() const
{
	return !bTriggered;
}

void SVProductInfoStruct::InitProductInfo()
{
	SetProductComplete();

	m_ProductState.clear();
	for (int i = 0; i < SvDef::cMaximumCameras; ++i)
	{
		bhasCameraImage[i] = false;
	}
	bTriggered			= false;
	bDelayExpired		= false;
	bDataComplete		= false;
	bStreamed           = false;
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
	for (int i = 0; i < SvDef::cMaximumCameras; ++i)
	{
		bhasCameraImage[i] = false;
	}
	bTriggered			= false;
	bDelayExpired		= false;
	bDataComplete		= false;
	bStreamed           = false;
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

void SVProductInfoStruct::resetIPInfos(const GUID& rExceptIPGuid)
{
	for (auto& rIPPair : m_svInspectionInfos)
	{
		if (rExceptIPGuid != rIPPair.first)
		{
			rIPPair.second.Reset();
		}
	}
}

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
		SvTrc::IImagePtr pImage = CameraIter->second.GetNextImage();
		if( S_OK == l_Status && nullptr == pImage )
		{
			l_Status = E_FAIL;
		}
	}

	SVGUIDSVInspectionInfoStructMap::iterator l_svInspectionIter = m_svInspectionInfos.begin();

	while( l_svInspectionIter != m_svInspectionInfos.end() )
	{
		HRESULT l_Temp = l_svInspectionIter->second.GetNextAvailableIndexes( );

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
	rData = SvUl::Format( _T( "TriggerCount=%ld-DataComplete=%s-ResultDataIndex=%ld" ),
		ProcessCount(),
		( bDataComplete ) ? _T( "T" ) : _T( "F" ),
		oPPQInfo.m_ResultDataDMIndexHandle.GetIndex() );

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

			SvTrc::IImagePtr pImage = CamIter->second.getImage();
			rData += SvUl::Format( _T( " : %s-Index=%ld" ), CameraName.c_str(), (nullptr != pImage) ? pImage->getBufferPos() : -1);
		}
	}

	SVGUIDSVInspectionInfoStructMap::const_iterator l_InspectIter = m_svInspectionInfos.begin();

	while( l_InspectIter != m_svInspectionInfos.end() )
	{
		std::string l_Temp = SvUl::Format( _T( " : %s-State=0x%x" ), 
			( nullptr != l_InspectIter->second.pInspection ) ? l_InspectIter->second.pInspection->GetName() : _T( "(null)" ), 
			l_InspectIter->second.oInspectedState );

		rData += l_Temp;

		++l_InspectIter;
	}
}

HRESULT SVProductInfoStruct::GetResultDataIndex( SVDataManagerHandle& p_rHandle ) const
{
	const SVDataManagerHandle& l_rTemp = oPPQInfo.m_ResultDataDMIndexHandle;

	HRESULT l_Status = p_rHandle.Assign( l_rTemp, l_rTemp.GetLockType() );

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

	for(auto& rIpInfoPair: m_svInspectionInfos)
	{
		rIpInfoPair.second.m_CanProcess = false;
		rIpInfoPair.second.m_InProcess = false;
	}
}

void SVProductInfoStruct::setInspectionTriggerRecordComplete(const SVGUID& rIPGuid)
{
	if (GUID_NULL != rIPGuid)
	{
		auto iter = m_svInspectionInfos.find(rIPGuid);
		if (m_svInspectionInfos.end() != iter)
		{
			iter->second.setTriggerRecordCompleted();
		}
	}
	else
	{
		for (auto& rIPInfoPair : m_svInspectionInfos)
		{
			rIPInfoPair.second.setTriggerRecordCompleted();
		}
	}
}

SVInspectionCompleteInfoStruct::SVInspectionCompleteInfoStruct()
: m_InspectionID(), m_ProductInfo()
{
}

SVInspectionCompleteInfoStruct::SVInspectionCompleteInfoStruct( const SVGUID& p_rInspectionID, const SVProductInfoStruct& p_rProductInfo )
: m_InspectionID( p_rInspectionID ), m_ProductInfo( p_rProductInfo )
{
}

SVInspectionCompleteInfoStruct::SVInspectionCompleteInfoStruct( const SVInspectionCompleteInfoStruct& p_rObject )
: m_InspectionID( p_rObject.m_InspectionID ), m_ProductInfo( p_rObject.m_ProductInfo )
{
}

SVInspectionCompleteInfoStruct::~SVInspectionCompleteInfoStruct()
{
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

