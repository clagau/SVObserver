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

#pragma endregion Includes

namespace SvOg
{
	class RangeController
	{
		typedef SvOg::ValuesAccessor<SvOg::BoundValues> Values;
		typedef SvOg::DataController<Values, Values::value_type> Controller;

	public:
		RangeController(const GUID& rInspectionID, const GUID& rTaskObjectID);
		virtual ~RangeController();

		void Init();
		void Commit();
	
		std::string Get(const std::string& rName) const;
		void Set(const std::string& rName, const std::string& rValue);

		std::string GetOwnerName() const;
		void IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& rValue);
		void Validate();

		static const std::string FailHigh;
		static const std::string FailLow;
		static const std::string WarnHigh;
		static const std::string WarnLow;

	private:
		std::string GetIndirectValue(const GUID& rEmbeddedID) const;
		std::string GetDirectValue(const GUID& rEmbeddedID) const;
		void SetIndirectValue(const GUID& rEmbeddedID, const std::string& rValue);
		void SetDirectValue(const GUID& rEmbeddedID, const std::string& rValue);

		std::string GetInspectionName() const;
		std::string GetPPQName() const;
		std::string GetToolName() const;

		const GUID& m_rInspectionID;
		const GUID& m_rTaskObjectID;
		std::unique_ptr<Controller> m_pRangeValues{ nullptr };
	};
} //namespace SvOg
