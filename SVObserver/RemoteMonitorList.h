//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MonitorListObject
//* .File Name       : $Workfile:   RemoteMonitorList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Apr 2014 16:26:12  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <map>
#include <deque>
#include "RemoteMonitorNamedList.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

// Key is List name
typedef std::map<SVString, RemoteMonitorNamedList> RemoteMonitorList;

// These are for the GUI(s)
// Listname, reject Depth
typedef std::pair<SVString, int> NameDepthPair;
typedef std::deque<NameDepthPair> NameDepthPairList;
// 1st is PPQ name, 2nd is a list of MonitorListNames and their Reject Depth
typedef std::map<SVString, NameDepthPairList> PPQNameListNames;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorList.h_v  $
 * 
 *    Rev 1.0   17 Apr 2014 16:26:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
