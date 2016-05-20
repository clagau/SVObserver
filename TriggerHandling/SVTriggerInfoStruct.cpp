//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file SVTriggerInfoStruct.cpp
/// All Rights Reserved
//*****************************************************************************
  
/// Contains the struct SVTriggerInfoStruct 
//******************************************************************************

#include "stdafx.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVTriggerInfoStruct.h"


namespace Seidenader { namespace TriggerHandling {
	SVTriggerInfoStruct::SVTriggerInfoStruct()
	: bValid( FALSE )
	, pTrigger( nullptr )
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
		pTrigger		= nullptr;
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


} /* namespace TriggerHandling */ } /* namespace Seidenader */

namespace SvTh = Seidenader::TriggerHandling;