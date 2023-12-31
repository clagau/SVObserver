//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// \file RangeEnum.h
// All Rights Reserved
//*****************************************************************************

// This is the Range Enum for the Range Value Objects.
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource\resource.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

struct RangeEnum
{
	enum ERange { ER_FailHigh = 0, ER_WarnHigh, ER_WarnLow, ER_FailLow, ER_COUNT = 4 };

	//************************************
	// Description: Translate enum to string
	// Returns: string:  name of the range enum
	//************************************
	static std::string ERange2String(ERange range)
	{
		static std::map<ERange, std::string> enumNames
		{
			{ER_FailHigh, SvUl::LoadStdString(IDS_FAIL_HIGH)},
			{ER_WarnHigh, SvUl::LoadStdString(IDS_WARN_HIGH)},
			{ER_FailLow, SvUl::LoadStdString(IDS_FAIL_LOW)},
			{ER_WarnLow, SvUl::LoadStdString(IDS_WARN_LOW)}
		};
		
		return enumNames[range];
	}
};