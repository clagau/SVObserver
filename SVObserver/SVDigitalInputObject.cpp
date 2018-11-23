//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalInputObject.h"
#include "SVIOLibrary/SVIOConfigurationInterfaceClass.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
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
	LocalInitialize();
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
	m_outObjectInfo.m_ObjectTypeInfo.ObjectType = SvPb::SVIoObjectType;
	m_outObjectInfo.m_ObjectTypeInfo.SubType = SvPb::SVDigitalInputObjectType;

	m_isCreated = true;

	return true;
}// end Create

bool SVDigitalInputObject::Destroy()
{
	m_isCreated = false;

	return true;
}// end Destroy

HRESULT SVDigitalInputObject::Read( _variant_t& p_rValue )
{
	HRESULT l_Status = S_OK;
	bool l_bValue = false;

	l_Status = SVIOConfigurationInterfaceClass::Instance().GetDigitalInputValue( m_lChannel, l_bValue );

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

bool SVDigitalInputObject::Force( bool bForce, bool bForcedValue )
{
	m_bForced = bForce;
	m_bForcedValue = bForcedValue;

	bool l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsForced( m_lChannel, m_bForced );
	l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalInputForcedValue( m_lChannel, m_bForcedValue ) && l_bOk;

	return l_bOk;
}// end Force

bool SVDigitalInputObject::Invert( bool bInvert )
{
	m_bInverted = bInvert;

	bool l_bOk = S_OK == SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsInverted( m_lChannel, m_bInverted );

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

void SVDigitalInputObject::SetChannel( long lChannel )
{
	if( m_lChannel != lChannel )
	{
		m_lChannel = lChannel;
		//! When the channel is set then we need to initialize the input values
		SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsInverted(m_lChannel, m_bInverted);
		SVIOConfigurationInterfaceClass::Instance().SetDigitalInputIsForced(m_lChannel, m_bForced);
		SVIOConfigurationInterfaceClass::Instance().SetDigitalInputForcedValue(m_lChannel, m_bForcedValue);
	}
}

long SVDigitalInputObject::GetChannel() const
{
	return m_lChannel;
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
