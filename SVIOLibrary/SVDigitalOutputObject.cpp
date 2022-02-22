//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVDigitalOutputObject.h"
#include "SVIOConfigurationInterfaceClass.h"
#include "ObjectInterfaces/IObjectWriter.h"
#include "SVObjectLibrary/SVClsids.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVXMLLibrary/SVConfigurationTags.h"
#pragma endregion Includes

SVDigitalOutputObject::SVDigitalOutputObject( LPCSTR strObjectName ) : SVOutputObject( strObjectName )
{
	LocalInitialize();
}

SVDigitalOutputObject::SVDigitalOutputObject( SVObjectClass *pOwner, int StringResourceID ) : SVOutputObject( pOwner, StringResourceID )
{
	LocalInitialize();
}

void SVDigitalOutputObject::Persist(SvOi::IObjectWriter& rWriter) const
{
	_variant_t svVariant;
	svVariant.SetString(GetName());
	rWriter.WriteAttribute(SvXml::CTAG_IO_ENTRY_NAME, svVariant);
	svVariant.Clear();

	svVariant = static_cast<int> (GetObjectSubType());
	rWriter.WriteAttribute(SvXml::CTAG_TYPE, svVariant);
	svVariant.Clear();

	svVariant = GetChannel();
	rWriter.WriteAttribute(SvXml::CTAG_CHANNEL, svVariant);
	svVariant.Clear();

	svVariant = true;
	rWriter.WriteAttribute(SvXml::CTAG_IS_OUTPUT, svVariant);
	svVariant.Clear();

	svVariant = IsInverted();
	rWriter.WriteAttribute(SvXml::CTAG_IS_INVERTED, svVariant);
	svVariant.Clear();

	svVariant = IsForced();
	rWriter.WriteAttribute(SvXml::CTAG_IS_FORCED, svVariant);
	svVariant.Clear();

	svVariant = GetForcedValue();
	rWriter.WriteAttribute(SvXml::CTAG_FORCED_VALUE, svVariant);
	svVariant.Clear();

	svVariant = isCombined();
	rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED, svVariant);
	svVariant.Clear();

	svVariant = isAndACK();
	rWriter.WriteAttribute(SvXml::CTAG_IS_COMBINED_ACK, svVariant);
	svVariant.Clear();
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
	m_ObjectTypeInfo.m_ObjectType = SvPb::SVIoObjectType;
	m_ObjectTypeInfo.m_SubType = SvPb::SVDigitalOutputObjectType;
	m_startID = ObjectIdEnum::DigitalOutputId;
}

