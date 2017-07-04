//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************
// This is the RangeController for the Range Value Objects.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <guiddef.h>
#include "ValuesAccessor.h"
#include "BoundValue.h"
#include "DataController.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace SvOg
{
	class RangeController
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
		typedef SvOg::DataController<Values, Values::value_type> ValueController;

	public:
		RangeController(const GUID& rInspectionID, const GUID& rTaskObjectID);
		virtual ~RangeController();

		void Init();
		void Commit();
	
		std::string Get(const std::string& rName) const;
		void Set(const std::string& rName, const std::string& rValue);

		SVString GetOwnerName() const;
		void IsFieldValid(SvStl::MessageTextEnum fieldName, const SVString& rValue);
		void Validate();

		static const std::string FailHigh;
		static const std::string FailLow;
		static const std::string WarnHigh;
		static const std::string WarnLow;

	private:
		std::string GetIndirectValue(const std::string& rName) const;
		std::string GetDirectValue(const std::string& rName) const;
		void SetIndirectValue(const std::string& rName, const std::string& rValue);
		void SetDirectValue(const std::string& rName, const std::string& rValue);

		SVString GetInspectionName() const;
		SVString GetPPQName() const;
		SVString GetToolName() const;

		GUID m_InspectionID;
		GUID m_TaskObjectID;
		ValueController m_directRangeValues;
		ValueController m_indirectRangeValues;
	};
} //namespace SvOg
