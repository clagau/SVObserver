//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
#pragma once

#pragma region Includes
#include <deque>
#include <map>
#include <Guiddef.h>
#include "SVString.h"
#include "SVUtilityLibrary\SVGUID.h"
#pragma endregion Includes
namespace Seidenader
{
	namespace SVUtilityLibrary
	{
		typedef std::pair<SVString, SVGUID> NameGuidPair;
		typedef std::deque<NameGuidPair> NameGuidList;
		typedef SVString InputName;
		typedef std::map<InputName, NameGuidPair> InputNameGuidPairList;
	}
}

namespace SvUl = Seidenader::SVUtilityLibrary;
