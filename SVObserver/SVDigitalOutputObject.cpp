//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalOutputObject.h"

#include "TriggerHandling/SVIOConfigurationInterfaceClass.h"
#pragma endregion Includes

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
	m_isCreated	= false;
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
	m_isCreated = false;
}

SVDigitalOutputObject::~SVDigitalOutputObject()
{
	if( m_isCreated )
	{
		Destroy();
		m_isCreated = false;
	}
}

BOOL SVDigitalOutputObject::Create()
{
	m_isCreated = true;
	
	return m_isCreated;
}

BOOL SVDigitalOutputObject::Destroy()
{
	m_isCreated = false;

	return !m_isCreated;
}

HRESULT SVDigitalOutputObject::Write( const _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;

	m_bLastValue = p_rValue;

	l_Status = SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bLastValue );

	return l_Status;
}

HRESULT SVDigitalOutputObject::Reset()
{
	HRESULT l_Status = S_OK;

	l_Status = SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bDefaultValue );

	return l_Status;
}

BOOL SVDigitalOutputObject::Force( bool bForce, bool bForcedValue )
{
	BOOL l_bOk = FALSE;

	m_bForced = bForce;
	m_bForcedValue = bForcedValue;

	l_bOk = S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced( m_lChannel, m_bForced );
	l_bOk = S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue( m_lChannel, m_bForcedValue ) && l_bOk;

	return l_bOk;
}

BOOL SVDigitalOutputObject::Invert( bool bInvert )
{
	BOOL l_bOk = FALSE;

	m_bInverted = bInvert;

	l_bOk = S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted( m_lChannel, m_bInverted );

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

