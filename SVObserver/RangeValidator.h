//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// RangeValidator is the function object used to Validate the range variable(s)
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

class RangeValidator
{
public:
	static HRESULT IsFieldValid(SVString& msg, const SVString& fieldName, const SVString& value);

	static HRESULT Validate(SVString& msg, 
					HINSTANCE resHandle,
					const SVString& InspectionName, 
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