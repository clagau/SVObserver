//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the RangeController for the Range Value Objects.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
#include "ValuesAccessor.h"
#include "BoundValue.h"
#include "DataController.h"
#include "SVStatusLibrary/MessageTextEnum.h"

#pragma endregion Includes

namespace SvOg
{
	class RangeController
	{
		typedef SvOg::DataController<SvOg::ValuesAccessor, SvOg::ValuesAccessor::value_type> Controller;

	public:
		RangeController(uint32_t inspectionID, uint32_t taskID, uint32_t rangeID = SvDef::InvalidObjectId);
		virtual ~RangeController();

		void Init();
		HRESULT Commit();
	
		std::string Get(const std::string& rName) const;
		void Set(const std::string& rName, const std::string& rValue);

		std::string GetOwnerName() const;
		void IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& rValue);

		static const std::string FailHigh;
		static const std::string FailLow;
		static const std::string WarnHigh;
		static const std::string WarnLow;

	private:
		std::string GetIndirectValue(SvPb::EmbeddedIdEnum embeddedID) const;
		std::string GetDirectValue(SvPb::EmbeddedIdEnum embeddedID) const;

		const uint32_t m_InspectionID;
		const uint32_t m_TaskObjectID;
		uint32_t m_RangeID;
		Controller m_RangeValues;
	};
} //namespace SvOg
