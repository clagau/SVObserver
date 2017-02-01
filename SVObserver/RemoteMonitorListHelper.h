//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListHelper
//* .File Name       : $Workfile:   RemoteMonitorListHelper.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   08 Jul 2014 08:47:10  $
//******************************************************************************
#pragma once
#include "SVUtilityLibrary\SVString.h"
#include "RemoteMonitorNamedList.h"
#include "SVSharedMemoryLibrary\MonitorEntry.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"

class RemoteMonitorListHelper
{
public:
	static MonitoredObject GetMonitoredObjectFromName(const SVString& name);
	static SVString GetNameFromMonitoredObject(const MonitoredObject& rMonitoredObject);
	static DWORD GetTypeFromFromMonitoredObject(const MonitoredObject& rMonitoredObject);
	static DWORD GetSizeFromFromMonitoredObject(const MonitoredObject& rMonitoredObject); 
	static void AddMonitorObjects2MoListEntryVector(const MonitoredObjectList& values, SvSml::MonitorEntryVector  &ListEntries);
	static void InsertRemotMonitorNamedList2MonitorListcpy(const RemoteMonitorNamedList& remoteMonitorNamedlist,SvSml::MonitorListCpy  &monitorListCpy ); 
};


