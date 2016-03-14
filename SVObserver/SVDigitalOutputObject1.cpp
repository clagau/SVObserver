//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitalOutputObject
//* .File Name       : $Workfile:   SVDigitalOutputObject1.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:11:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVDigitalOutputObject1.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"

SVDigitalOutputObject::SVDigitalOutputObject( LPCSTR strObjectName )
:SVOutputObject( strObjectName )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bCombined( false )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
, m_bCombinedACK( true )
{
	m_bCreated		= false;
}

SVDigitalOutputObject::SVDigitalOutputObject( SVObjectClass *pOwner, int StringResourceID )
:SVOutputObject( pOwner, StringResourceID )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bCombined( false )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
, m_bCombinedACK( true )
{
	m_bCreated		= false;
}

SVDigitalOutputObject::~SVDigitalOutputObject()
{
	if( m_bCreated )
		Destroy();

	m_bCreated		= false;
}

BOOL SVDigitalOutputObject::Create()
{
	m_bCreated = true;
	
	return TRUE;
}

BOOL SVDigitalOutputObject::Destroy()
{
	m_bCreated = false;

	return TRUE;
}

HRESULT SVDigitalOutputObject::Write( const _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;

	m_bLastValue = p_rValue;

	l_Status = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bLastValue );

	return l_Status;
}

HRESULT SVDigitalOutputObject::Reset()
{
	HRESULT l_Status = S_OK;

	l_Status = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bDefaultValue );

	return l_Status;
}

BOOL SVDigitalOutputObject::Force( bool bForce, bool bForcedValue )
{
	BOOL l_bOk = FALSE;

	m_bForced = bForce;
	m_bForcedValue = bForcedValue;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced( m_lChannel, m_bForced ) == S_OK;
	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue( m_lChannel, m_bForcedValue ) == S_OK && l_bOk;

	return l_bOk;
}

BOOL SVDigitalOutputObject::Invert( bool bInvert )
{
	BOOL l_bOk = FALSE;

	m_bInverted = bInvert;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted( m_lChannel, m_bInverted ) == S_OK;

	return l_bOk;
}

BOOL SVDigitalOutputObject::Combine( bool bCombine, bool bCombineACK )
{
	BOOL l_bOk = TRUE;

	m_bCombined = bCombine;
	m_bCombinedACK = bCombineACK;

	return l_bOk;
}

bool SVDigitalOutputObject::IsForced() const
{
	return m_bForced;
}

bool SVDigitalOutputObject::GetForcedValue() const
{
	return m_bForcedValue;
}

bool SVDigitalOutputObject::IsInverted() const
{
	return m_bInverted;
}

bool SVDigitalOutputObject::IsCombined() const
{
	return m_bCombined;
}

bool SVDigitalOutputObject::GetCombinedValue() const
{
	return m_bCombinedACK;
}

bool SVDigitalOutputObject::GetValue() const
{
	return m_bLastValue;
}

BOOL SVDigitalOutputObject::SetChannel( long lChannel )
{
	m_lChannel = lChannel;
	return TRUE;
}

long SVDigitalOutputObject::GetChannel() const
{
	return m_lChannel;
}

