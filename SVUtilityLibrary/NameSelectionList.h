//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : NameSelectionList
//* .File Name       : $Workfile:   NameSelectionList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:25:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <utility>
#include <deque>
#include "SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVUtilityLibrary
	{
		// first in the pair is the inspection object (image/value) name, second is a boolean for whether it's selected or not
		typedef std::pair<SVString, bool> NameSelection;
		typedef std::deque<NameSelection> NameSelectionList;
	} //SVUtilityLibrary
} //Seidenader

namespace SvUl = Seidenader::SVUtilityLibrary;

