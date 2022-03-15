//******************************************************************************
//* COPYRIGHT (c) 2010 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : StringToEnum
//* .File Name       : $Workfile:   StringToEnum.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:19:54  $
//******************************************************************************
#pragma once

namespace StringToEnum
{
template<typename LookupTable, typename EnumType>
bool GetEnum(const LookupTable& rtable, LPCTSTR value, EnumType& enumValue)
{
	bool  bRetVal = false;
	std::string compareText {value};
	auto iter = std::find_if(rtable.cbegin(), rtable.cend(), [compareText](const auto& rEntry) { return compareText == rEntry.second; });
	if (iter != rtable.cend())
	{
		enumValue = iter->first;
		bRetVal = true;
	}
	return bRetVal;
}
}

