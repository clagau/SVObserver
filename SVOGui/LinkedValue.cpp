//*****************************************************************************
// \copyright COPYRIGHT (c) 2021 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the Linked Value class for use by the GUI.
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LinkedValue.h"
#include "SVProtoBuf/ConverterHelper.h"
#pragma endregion Includes

namespace SvOg
{
LinkedValueData convertLinkedValue(const SvPb::LinkedValue& rValue)
{
	LinkedValueData data;
	SvPb::ConvertProtobufToVariant(rValue.value(), data.m_Value);
	SvPb::ConvertProtobufToVariant(rValue.defaultvalue(), data.m_defaultValue);
	SvPb::ConvertProtobufToVariant(rValue.directvalue(), data.m_directValue);
	data.m_indirectDotName = rValue.indirectdotname();
	data.m_type = rValue.type();
	data.m_formula = rValue.formula();
	data.m_equationId = rValue.equationid();
	return data;
}

SvPb::LinkedValue convertLinkedValue(const LinkedValueData& rValue)
{
	SvPb::LinkedValue data;
	data.set_type(static_cast<SvPb::LinkedSelectedType>(rValue.m_type));
	SvPb::ConvertVariantToProtobuf(rValue.m_directValue, data.mutable_directvalue());
	data.set_indirectdotname(rValue.m_indirectDotName);
	data.set_formula(rValue.m_formula);
	SvPb::ConvertVariantToProtobuf(rValue.m_defaultValue, data.mutable_defaultvalue());
	SvPb::ConvertVariantToProtobuf(rValue.m_Value, data.mutable_value());
	data.set_equationid(rValue.m_equationId);
	return data;
}

LinkedValue::LinkedValue(uint32_t instanceID, LinkedValueData&& data)
	: m_instanceID(instanceID)
	, m_data(data)
{}

void LinkedValue::setLinkedData(const LinkedValueData& data)
{
	if (m_data != data)
	{
		m_data = std::move(data);
		m_bModified = true;
	}
}

} //namespace SvOg