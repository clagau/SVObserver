//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// NoSelector is the function object to get the empty selection list
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

class NoSelector
{
public:
	SVStringArray operator()(const GUID& rGuid)
	{
		return SVStringArray();
	}
	SVStringArray operator()(UINT attribute)
	{
		return SVStringArray();
	}
};