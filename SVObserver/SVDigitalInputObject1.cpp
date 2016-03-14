//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDigitalInputObject
//* .File Name       : $Workfile:   SVDigitalInputObject1.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:11:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVDigitalInputObject1.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"

SVDigitalInputObject::SVDigitalInputObject( LPCSTR strObjectName )
:SVInputObject( strObjectName )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
{
	m_bCreated		= FALSE;
}

SVDigitalInputObject::SVDigitalInputObject( SVObjectClass *pOwner, int StringResourceID )
:SVInputObject( pOwner, StringResourceID )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
{
	m_bCreated		= FALSE;
}

SVDigitalInputObject::~SVDigitalInputObject()
{
	if( m_bCreated )
		Destroy();

	m_bCreated		= FALSE;
}

BOOL SVDigitalInputObject::Create()
{
	m_bCreated = TRUE;

	return TRUE;
}// end Create

BOOL SVDigitalInputObject::Destroy()
{
	m_bCreated = FALSE;

	return TRUE;
}// end Destroy

BOOL SVDigitalInputObject::IsCreated()
{
	return m_bCreated;
}// end IsCreated

HRESULT SVDigitalInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;
	bool l_bValue = false;

	l_Status = SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue( m_lChannel, l_bValue );

	if( l_Status == S_OK )
	{
		p_rValue = l_bValue;
	}
	else
	{
		p_rValue.Clear();
	}

	return l_Status;
}// end Read

BOOL SVDigitalInputObject::Force( bool bForce, bool bForcedValue )
{
	BOOL l_bOk = FALSE;

	m_bForced = bForce;
	m_bForcedValue = bForcedValue;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsForced( m_lChannel, m_bForced ) == S_OK;
	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalInputForcedValue( m_lChannel, m_bForcedValue ) == S_OK && l_bOk;

	return l_bOk;
}// end Force

BOOL SVDigitalInputObject::Invert( bool bInvert )
{
	BOOL l_bOk = FALSE;

	m_bInverted = bInvert;

	l_bOk = SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsInverted( m_lChannel, m_bInverted ) == S_OK;

	return l_bOk;
}// end Invert

bool SVDigitalInputObject::IsForced() const
{
	return m_bForced;
}// end IsForced

bool SVDigitalInputObject::GetForcedValue() const
{
	return m_bForcedValue;
}// end GetForcedValue

bool SVDigitalInputObject::GetValue() const
{
	return m_bLastValue;
}// end GetValue

bool SVDigitalInputObject::IsInverted() const
{
	return m_bInverted;
}// end IsInverted

BOOL SVDigitalInputObject::SetChannel( long lChannel )
{
	m_lChannel = lChannel;
	return TRUE;
}// end SetChannel

long SVDigitalInputObject::GetChannel() const
{
	return m_lChannel;
}

