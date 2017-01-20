//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Range Enum for the Range Value Objects.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost\assign\list_of.hpp>
#include "SVOResource\resource.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

struct RangeEnum
{
	enum ERange { ER_FailHigh = 0, ER_WarnHigh, ER_FailLow, ER_WarnLow, ER_COUNT = 4 };

	//************************************
	// Description: Translate enum to string
	// Returns: string:  name of the range enum
	//************************************
	static SVString ERange2String(ERange range)
	{
		static std::map<ERange, SVString> enumNames = boost::assign::map_list_of<>
			(ER_FailHigh, SvUl_SF::LoadSVString(IDS_FAIL_HIGH))
			(ER_WarnHigh, SvUl_SF::LoadSVString(IDS_WARN_HIGH))
			(ER_FailLow, SvUl_SF::LoadSVString(IDS_FAIL_LOW))
			(ER_WarnLow, SvUl_SF::LoadSVString(IDS_WARN_LOW));
		
		return enumNames[range];
	}
};