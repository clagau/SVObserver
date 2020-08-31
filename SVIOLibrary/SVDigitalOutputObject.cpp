//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalOutputObject.h"
#include "SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

SVDigitalOutputObject::SVDigitalOutputObject( LPCSTR strObjectName ) : SVOutputObject( strObjectName )
{
	LocalInitialize();
}

SVDigitalOutputObject::SVDigitalOutputObject( SVObjectClass *pOwner, int StringResourceID ) : SVOutputObject( pOwner, StringResourceID )
{
	LocalInitialize();
}

HRESULT SVDigitalOutputObject::Write( const _variant_t& rValue )
{
	m_bLastValue = rValue;
	return SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(m_channel, m_bLastValue);
}

HRESULT SVDigitalOutputObject::Reset()
{
	return SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputValue(m_channel, m_bDefaultValue);
}

bool SVDigitalOutputObject::Force(bool bForced, bool bForcedValue)
{
	bool l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsForced(m_channel, bForced);
	l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputForcedValue(m_channel, bForcedValue) && l_bOk;

	return l_bOk;
}

bool SVDigitalOutputObject::Invert(bool bInverted)
{
	return S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalOutputIsInverted( m_channel, bInverted );
}

void SVDigitalOutputObject::Combine( bool bCombine, bool isAndACK )
{
	m_bCombined = bCombine;
	m_isAndACK = isAndACK;
}

bool SVDigitalOutputObject::IsForced() const
{
	bool bIsForced {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputIsForced(m_channel, bIsForced);

	return bIsForced;
}

bool SVDigitalOutputObject::GetForcedValue() const
{
	bool bForcedValue {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputForcedValue(m_channel, bForcedValue);

	return bForcedValue;
}

bool SVDigitalOutputObject::IsInverted() const
{
	bool bIsInverted {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalOutputIsInverted(m_channel, bIsInverted);

	return bIsInverted;
}

bool SVDigitalOutputObject::isCombined() const
{
	return m_bCombined;
}

bool SVDigitalOutputObject::isAndACK() const
{
	return m_isAndACK;
}

bool SVDigitalOutputObject::GetValue() const
{
	return m_bLastValue;
}

void SVDigitalOutputObject::LocalInitialize()
{
	m_outObjectInfo.m_ObjectTypeInfo.m_ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.m_SubType = SvPb::SVDigitalOutputObjectType;
	m_startID = ObjectIdEnum::DigitalOutputId;
}

