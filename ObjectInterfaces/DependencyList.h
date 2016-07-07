//*****************************************************************************
// \copyright COPYRIGHT (c) 2015 by Seidenader Maschinenbau GmbH
// All Rights Reserved
//*****************************************************************************

// This is the typedef for the DependencyList
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <utility>
//Moved to precompiled header: #include <deque>
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectInterfaces
	{
		typedef std::pair<SVString, SVString> Relation;
		typedef std::deque<Relation> DependencyList;
	}
}
namespace SvOi = Seidenader::ObjectInterfaces;