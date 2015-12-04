//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the Range Enum for the Range Value Objects.
//******************************************************************************
#pragma once

#pragma region Includes
#include <string>
#include "SVOResource\resource.h"
#include <boost\assign\list_of.hpp>
#include "SVUtilityLibrary\SVString.h"
#include "SVUtilityLibrary\SVStringLoader.h"
#pragma endregion Includes

struct RangeEnum
{
	enum ERange { ER_FailHigh = 0, ER_WarnHigh, ER_FailLow, ER_WarnLow, ER_COUNT = 4 };

	//************************************
	// Description: Translate enum to string
	// Returns: string:  name of the range enum
	//************************************
	static SVString ERange2String(HINSTANCE resHandle, ERange range)
	{
		static std::map<ERange, SVString> enumNames = boost::assign::map_list_of<>
			(ER_FailHigh, SvU::SVStringLoader(resHandle, IDS_FAIL_HIGH))
			(ER_WarnHigh, SvU::SVStringLoader(resHandle, IDS_WARN_HIGH))
			(ER_FailLow, SvU::SVStringLoader(resHandle, IDS_FAIL_LOW))
			(ER_WarnLow, SvU::SVStringLoader(resHandle, IDS_WARN_LOW));
		
		return enumNames[range];
	}
};