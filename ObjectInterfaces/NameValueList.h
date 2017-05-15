//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Declaration of a Name Value Pair and List.
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <utility>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace SvOi
{
	typedef std::pair<SVString, long> NameValuePair;
	typedef std::deque<NameValuePair> NameValueList;
} //namespace SvOi
