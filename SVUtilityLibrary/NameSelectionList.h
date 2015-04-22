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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\NameSelectionList.h_v  $
 * 
 *    Rev 1.0   17 Apr 2014 16:25:20   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
