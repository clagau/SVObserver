//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeValidator is the function object used to Validate the range variable(s)
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStatusLibrary/MessageTextEnum.h"
#pragma endregion Includes

namespace SvOg
{
	class RangeValidator
	{
	public:
		static void IsFieldValid(SvStl::MessageTextEnum fieldName, const std::string& value);

		static void Validate( const std::string& InspectionName, 
						const std::string& FailHighIndirectValue, 
						const std::string& FailLowIndirectValue, 
						const std::string& WarnHighIndirectValue, 
						const std::string& WarnLowIndirectValue, 
						double FailHighValue, 
						double FailLowValue, 
						double WarnHighValue, 
						double WarnLowValue, 
						const GUID& rInspectionID);

	};
} //namespace SvOg
