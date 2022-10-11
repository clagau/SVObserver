//*****************************************************************************
// \copyright COPYRIGHT (c) 2021 by Körber Pharma Inspection GmbH. All Rights Reserved
// All Rights Reserved
//*****************************************************************************
// This is the Linked Value class for use by the GUI.
//******************************************************************************
#pragma once
#include "Definitions/ObjectDefines.h"
#include "SVProtoBuf/InspectionCommands.h"
#include "SVUtilityLibrary/SafeArrayHelper.h"

namespace SvOg
{
	struct LinkedValueData
	{
		SvPb::LinkedSelectedOption m_selectedOption{ SvPb::LinkedSelectedOption::DirectValue };
		variant_t m_Value;
		variant_t m_defaultValue;
		std::string m_comment;
		variant_t m_directValue;
		std::string m_indirectIdName;
		std::string m_formula;
		uint32_t m_equationId = SvDef::InvalidObjectId;
		variant_t m_minValue;
		variant_t m_maxValue;

		bool operator==(const LinkedValueData& rValue) const //= default;
		{
			return m_selectedOption == rValue.m_selectedOption
				&& SvUl::isSameVar(m_Value, rValue.m_Value)
				&& SvUl::isSameVar(m_defaultValue, rValue.m_defaultValue)
				&& m_comment == rValue.m_comment
				&& SvUl::isSameVar(m_directValue, rValue.m_directValue)
				&& m_indirectIdName == rValue.m_indirectIdName
				&& m_formula == rValue.m_formula;
		}

		bool operator!=(const LinkedValueData& rValue) const
		{
			return !(*this == rValue);
		}
	};

	LinkedValueData convertLinkedValue(const SvPb::LinkedValue& rValue);
	SvPb::LinkedValue convertLinkedValue(const LinkedValueData& rValue);

	class LinkedValue
	{
#pragma region Constructor
	public:
		LinkedValue() = delete;// {};
		LinkedValue(uint32_t instanceID, LinkedValueData&& data);

		~LinkedValue() {}
#pragma endregion Constructor

#pragma region Public Methods
	public:
		variant_t GetValue() const
		{
			assert(false);
			return {};
		}

		variant_t GetMinValue() const
		{
			assert(false);
			return {};
		}

		variant_t GetMaxValue() const
		{
			assert(false);
			return {};
		}

		LinkedValueData getLinkedData() const { return m_data; };
		void setLinkedData(const LinkedValueData& data);
		bool setValue(const variant_t& rValue);
		int GetArrayIndex() const { return -1; }

		uint32_t GetObjectID() const { return m_instanceID; }

		bool isModified() const { return m_bModified; }
		bool isDefaultModified() const { return false; }
		//Need to make this const to be able to clear the flag in other const methods
		void ClearModified() const { m_bModified = false; }
		void ClearArrayIndex() const { m_ArrayIndex = -1; }
		void ClearDefaultModified() const { }
#pragma endregion Public Methods

#pragma region Member Variables
	private:
		uint32_t m_instanceID{ SvDef::InvalidObjectId };
		LinkedValueData m_data;
		mutable bool m_bModified{ false };
		mutable int m_ArrayIndex{ -1 };
#pragma endregion Member Variables
	};
}