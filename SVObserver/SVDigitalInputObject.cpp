//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalInputObject.h"

#include "TriggerHandling/SVIOConfigurationInterfaceClass.h"
#pragma endregion Includes

SVDigitalInputObject::SVDigitalInputObject( LPCSTR strObjectName )
:SVInputObject( strObjectName )
, m_lChannel( -1 )
, m_bForced( false )
, m_bInverted( true )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bForcedValue( false )
{
	m_isCreated = false;
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
	m_isCreated = false;
}

SVDigitalInputObject::~SVDigitalInputObject()
{
	if( m_isCreated )
	{
		Destroy();
	}
	m_isCreated	= false;
}

BOOL SVDigitalInputObject::Create()
{
	m_isCreated = true;

	return true;
}// end Create

BOOL SVDigitalInputObject::Destroy()
{
	m_isCreated = false;

	return true;
}// end Destroy

HRESULT SVDigitalInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;
	bool l_bValue = false;

	l_Status = SvTh::SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue( m_lChannel, l_bValue );

	if( S_OK == l_Status )
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

	l_bOk = S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsForced( m_lChannel, m_bForced );
	l_bOk = S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalInputForcedValue( m_lChannel, m_bForcedValue ) && l_bOk;

	return l_bOk;
}// end Force

BOOL SVDigitalInputObject::Invert( bool bInvert )
{
	BOOL l_bOk = FALSE;

	m_bInverted = bInvert;

	l_bOk = S_OK == SvTh::SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsInverted( m_lChannel, m_bInverted );

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

