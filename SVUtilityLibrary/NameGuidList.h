//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <guiddef.h>
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes
namespace SvUl
{
	typedef std::pair<std::string, SVGUID> NameGuidPair;
	typedef std::deque<NameGuidPair> NameGuidList;
	typedef std::map<std::string, NameGuidPair> InputNameGuidPairList;
} // namespace SvUl
