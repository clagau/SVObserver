//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalOutputObject.h"

#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
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
	LocalInitialize();
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
	LocalInitialize();
}

SVDigitalOutputObject::~SVDigitalOutputObject()
{
	if( m_isCreated )
	{
		Destroy();
		m_isCreated = false;
	}
}

bool SVDigitalOutputObject::Create()
{
	m_isCreated = true;
	
	return m_isCreated;
}

bool SVDigitalOutputObject::Destroy()
{
	m_isCreated = false;

	return !m_isCreated;
}

HRESULT SVDigitalOutputObject::Write( const _variant_t& rValue )
{
	HRESULT l_Status = S_OK;

	m_bLastValue = rValue;

	l_Status = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bLastValue );

	return l_Status;
}

HRESULT SVDigitalOutputObject::Reset()
{
	HRESULT l_Status = S_OK;

	l_Status = SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_lChannel, m_bDefaultValue );

	return l_Status;
}

bool SVDigitalOutputObject::Force( bool bForce, bool bForcedValue )
{
	m_bForced = bForce;
	m_bForcedValue = bForcedValue;

	bool l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced( m_lChannel, m_bForced );
	l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue( m_lChannel, m_bForcedValue ) && l_bOk;

	return l_bOk;
}

bool SVDigitalOutputObject::Invert( bool bInvert )
{
	m_bInverted = bInvert;

	bool l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted( m_lChannel, m_bInverted );

	return l_bOk;
}

void SVDigitalOutputObject::Combine( bool bCombine, bool bCombineACK )
{
	m_bCombined = bCombine;
	m_bCombinedACK = bCombineACK;
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

void SVDigitalOutputObject::SetChannel( long lChannel )
{
	m_lChannel = lChannel;
}

long SVDigitalOutputObject::GetChannel() const
{
	return m_lChannel;
}

void SVDigitalOutputObject::updateGuid(int position)
{
	if (0 <= position && 0x100 > position)
	{
		SVGUID ParameterUID(DigitalOutputUidGuid);
		ParameterUID.ToGUID().Data1 += position;
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ParameterUID);
	}
}

void SVDigitalOutputObject::LocalInitialize()
{
	m_isCreated = false;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SVDigitalOutputObjectType;
}

