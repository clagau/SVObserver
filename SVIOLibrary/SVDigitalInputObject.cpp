//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalInputObject.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#pragma endregion Includes

SVDigitalInputObject::SVDigitalInputObject( LPCSTR strObjectName )
:SVInputObject( strObjectName )
, m_Channel( -1 )
{
	LocalInitialize();
}

SVDigitalInputObject::SVDigitalInputObject( SVObjectClass *pOwner, int StringResourceID )
:SVInputObject( pOwner, StringResourceID )
, m_Channel( -1 )
{
	LocalInitialize();
}

SVDigitalInputObject::~SVDigitalInputObject()
{
	if( m_isCreated )
	{
		Destroy();
	}
	m_isCreated	= false;
}

bool SVDigitalInputObject::Create()
{
	m_isCreated = true;

	return true;
}

bool SVDigitalInputObject::Destroy()
{
	m_isCreated = false;

	return true;
}

HRESULT SVDigitalInputObject::Read( _variant_t& rValue )
{
	HRESULT result{S_OK};
	bool Value = false;

	result = SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue(m_Channel, Value);

	if(S_OK == result)
	{
		rValue = Value;
	}
	else
	{
		rValue.Clear();
	}

	return result;
}// end Read

bool SVDigitalInputObject::Force( bool bForce, bool bForcedValue )
{
	bool l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsForced(m_Channel, bForce);
	l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalInputForcedValue(m_Channel, bForcedValue) && l_bOk;

	return l_bOk;
}

bool SVDigitalInputObject::Invert(bool bInverted)
{
	return S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsInverted(m_Channel, bInverted);
}

bool SVDigitalInputObject::IsForced() const
{
	bool bIsForced{false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputIsForced(m_Channel, bIsForced);
	
	return bIsForced;
}

bool SVDigitalInputObject::GetForcedValue() const
{
	bool bForcedValue {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputForcedValue(m_Channel, bForcedValue);

	return bForcedValue;
}

bool SVDigitalInputObject::GetValue() const
{
	bool Value = false;
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue(m_Channel, Value);

	return Value;
}

bool SVDigitalInputObject::IsInverted() const
{
	bool bIsInverted {false};
	SVIOConfigurationInterfaceClass::Instance().GetDigitalInputIsInverted(m_Channel, bIsInverted);

	return bIsInverted;
}// end IsInverted

void SVDigitalInputObject::SetChannel( long lChannel )
{
	m_Channel = lChannel;
}

long SVDigitalInputObject::GetChannel() const
{
	return m_Channel;
}

void SVDigitalInputObject::updateGuid(int position)
{
	if (0 <= position && 0x100 > position)
	{
		SVGUID ParameterUID(DigitalInputUidGuid);
		ParameterUID.ToGUID().Data1 += position;
		SVObjectManagerClass::Instance().ChangeUniqueObjectID(this, ParameterUID);
	}
}

void SVDigitalInputObject::LocalInitialize()
{
	m_isCreated = false;
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVDigitalInputObjectType;
}
