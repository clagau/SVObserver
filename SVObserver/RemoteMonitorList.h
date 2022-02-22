//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <deque>
#include "RemoteMonitorNamedList.h"

#pragma endregion Includes

typedef std::map<std::string, RemoteMonitorNamedList> RemoteMonitorListMap;

// These are for the GUI(s)
// Listname, reject Depth
typedef std::pair<std::string, int> NameDepthPair;
typedef std::deque<NameDepthPair> NameDepthPairList;
// 1st is PPQ name, 2nd is a list of MonitorListNames and their Reject Depth
typedef std::map<std::string, NameDepthPairList> PPQNameListNames;

