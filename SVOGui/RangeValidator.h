//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeValidator is the function object used to Validate the range variable(s)
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#pragma endregion Includes

namespace SvOg
{
	class RangeValidator
	{
	public:
		static void IsFieldValid(SvStl::MessageTextEnum fieldName, const SVString& value);

		static void Validate( const SVString& InspectionName, 
						const SVString& FailHighIndirectValue, 
						const SVString& FailLowIndirectValue, 
						const SVString& WarnHighIndirectValue, 
						const SVString& WarnLowIndirectValue, 
						double FailHighValue, 
						double FailLowValue, 
						double WarnHighValue, 
						double WarnLowValue, 
						const GUID& rInspectionID);

	};
} //namespace SvOg
