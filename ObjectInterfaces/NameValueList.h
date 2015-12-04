//******************************************************************************
/// COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
//******************************************************************************
/// Declaration of a Name Value Pair and List.
//******************************************************************************
#pragma once

#pragma region Includes
#include <deque>
#include <utility>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		typedef std::pair<SVString, long> NameValuePair;
		typedef std::deque<NameValuePair> NameValueList;
	}
}

namespace SvOi = Seidenader::ObjectInterfaces;