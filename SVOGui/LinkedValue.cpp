//*****************************************************************************
// \copyright COPYRIGHT (c) 2021 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	data.m_indirectIdName = rValue.indirectidstring();
	data.m_selectedOption = rValue.option();
	data.m_formula = rValue.formula();
	data.m_equationId = rValue.equationid();
	data.m_comment = rValue.comment();
	if (rValue.has_min_max())
	{
		SvPb::ConvertProtobufToVariant(rValue.min_max().minvalue(), data.m_minValue);
		SvPb::ConvertProtobufToVariant(rValue.min_max().maxvalue(), data.m_maxValue);
	}
	return data;
}

SvPb::LinkedValue convertLinkedValue(const LinkedValueData& rValue)
{
	SvPb::LinkedValue data;
	data.set_option(static_cast<SvPb::LinkedSelectedOption>(rValue.m_selectedOption));
	SvPb::ConvertVariantToProtobuf(rValue.m_directValue, data.mutable_directvalue());
	data.set_indirectidstring(rValue.m_indirectIdName);
	data.set_formula(rValue.m_formula);
	SvPb::ConvertVariantToProtobuf(rValue.m_defaultValue, data.mutable_defaultvalue());
	SvPb::ConvertVariantToProtobuf(rValue.m_Value, data.mutable_value());
	data.set_equationid(rValue.m_equationId);
	data.set_comment(rValue.m_comment);
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

bool LinkedValue::setValue(const variant_t& rValue)
{
	if (SvPb::LinkedSelectedOption::DirectValue == m_data.m_selectedOption)
	{
		m_data.m_directValue = rValue;
		m_data.m_Value = rValue;
		m_bModified = true;
		return true;
	}
	return false;
}

} //namespace SvOg