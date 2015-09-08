//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// GlobalSelector is the function object to get the list of selectable 
/// Global Constants and Environment variables
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces\IRootObject.h"
#pragma endregion Includes

class GlobalSelector
{
public:
	SVStringArray operator()(UINT attribute)
	{
		SVStringArray ObjectNameList;
		SvOi::getRootChildNameList(ObjectNameList, _T(""), attribute);
		return ObjectNameList;
	}
};