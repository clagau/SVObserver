//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalOutputObject.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

SVDigitalOutputObject::SVDigitalOutputObject( LPCSTR strObjectName )
:SVOutputObject( strObjectName )
, m_Channel( -1 )
, m_bCombined( false )
, m_bLastValue( false )
, m_bDefaultValue( false )
, m_bCombinedACK( true )
{
	LocalInitialize();
}

SVDigitalOutputObject::SVDigitalOutputObject( SVObjectClass *pOwner, int StringResourceID )
:SVOutputObject( pOwner, StringResourceID )
, m_Channel( -1 )
, m_bCombined( false )
, m_bLastValue( false )
, m_bDefaultValue( false )
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
	m_bLastValue = rValue;
	return SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_Channel, m_bLastValue );
}

HRESULT SVDigitalOutputObject::Reset()
{
	return SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue( m_Channel, m_bDefaultValue );
}

bool SVDigitalOutputObject::Force(bool bForced, bool bForcedValue)
{
	bool l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced(m_Channel, bForced);
	l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue(m_Channel, bForcedValue) && l_bOk;

	return l_bOk;
}

bool SVDigitalOutputObject::Invert(bool bInverted)
{
	return S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted( m_Channel, bInverted );
}

void SVDigitalOutputObject::Combine( bool bCombine, bool bCombineACK )
{
	m_bCombined = bCombine;
	m_bCombinedACK = bCombineACK;
}

bool SVDigitalOutputObject::IsForced() const
{
	bool bIsForced {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputIsForced(m_Channel, bIsForced);

	return bIsForced;
}

bool SVDigitalOutputObject::GetForcedValue() const
{
	bool bForcedValue {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputForcedValue(m_Channel, bForcedValue);

	return bForcedValue;
}

bool SVDigitalOutputObject::IsInverted() const
{
	bool bIsInverted {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputIsInverted(m_Channel, bIsInverted);

	return bIsInverted;
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
	m_Channel = lChannel;
}

long SVDigitalOutputObject::GetChannel() const
{
	return m_Channel;
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVDigitalOutputObjectType;
}

