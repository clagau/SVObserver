//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the RangeController for the Range Value Objects.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
#include "DataController.h"
#include "LinkedValueWidgetHelper.h"
#include "Definitions/RangeEnum.h"

#pragma endregion Includes

namespace SvOg
{
	class RangeController
	{
	public:
		RangeController(uint32_t inspectionID, uint32_t taskID, uint32_t rangeID = SvDef::InvalidObjectId);
		virtual ~RangeController();

		void Init();
		HRESULT Commit();
	
		LinkedValueData Get(RangeEnum::ERange fieldId) const;
		void Set(RangeEnum::ERange fieldId, const LinkedValueData& rValue);

		std::string GetOwnerName() const;
		void IsFieldValid(RangeEnum::ERange fieldId) const;

		std::unique_ptr<LinkedValueWidgetHelper> createWidget(RangeEnum::ERange fieldId, CEdit& rValueEdit, CButton& rSelectButton);

	private:
		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		uint32_t m_RangeID;
		ValueController m_RangeValues;
	};
} //namespace SvOg
