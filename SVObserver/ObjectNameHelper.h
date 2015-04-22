//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : ObjectNameHelper
//* .File Name       : $Workfile:   ObjectNameHelper.h  $
//* .Description     : Functionality to create Name Selection List from TaskObject
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Dec 2014 13:53:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <vector>
#include "SVUtilityLibrary/NameSelectionList.h"
#pragma endregion Includes

#pragma region Declarations
class SVTaskObjectListClass;
#pragma endregion Declarations

class ObjectNameHelper
{
public:
	enum ObjectNameFilter {ENF_NONE, ENF_RANGE};

	//************************************
	// Description:  Fills a container of SVstring via inserter with objects from pTaskObjectList
	// Parameter:  pTaskObjectList <in>:  Find the outputs of this tasklist
	// Parameter:  inserter <in/out>:  inserter for the container for the list
	// Parameter:  ExcludePath <in>:  Objects from this path are excluded.
 	// Parameter:  filter <in>:  defines filter of objects
	// Returns:  int:  number of new objects.
	//************************************
	template<typename Inserter>
	static int BuildObjectNameList(SVTaskObjectListClass* pTaskObjectList, Inserter inserter, const CString& ExcludePath, ObjectNameFilter efilter = ENF_RANGE);
};

#pragma region Inline
#include "ObjectNameHelper.inl"
#pragma endregion Inline

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ObjectNameHelper.h_v  $
 * 
 *    Rev 1.0   19 Dec 2014 13:53:40   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check In for SCR 979 Indirect Range Objects 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/