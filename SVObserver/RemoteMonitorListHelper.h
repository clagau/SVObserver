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
#include "RemoteMonitorNamedList.h"
#include "SVSharedMemoryLibrary\MonitorEntry.h"
#include "SVSharedMemoryLibrary\MonitorListCpy.h"
#include "SVMatroxLibrary\MatroxImageProps.h"


class RemoteMonitorListHelper
{
public:
	static SvSml::MonitorListCpyPointer  RemoteMonitorListHelper::CreateMLcopy(const RemoteMonitorNamedList& remoteMonitorNamedlist);

	/// returns a monitorobject for the objectname 
	static MonitoredObject GetMonitoredObjectFromName(const std::string& name);
	/// GetTHe name of the Monitorobject
	static std::string GetNameFromMonitoredObject(const MonitoredObject& rMonitoredObject);
	
private:	
	
	static void AddMonitorObject2MonitorListcpy(const MonitoredObjectList& values, SvSml::ListType::typ listtype,SvSml::MonitorListCpy& molcpy );
	static DWORD GetTypeFromMonitoredObject(const MonitoredObject& rMonitoredObject);
	///retrieves MonitorEntryData for Monitorobject from the data manager 
	static void GetPropertiesFromMonitoredObject(const MonitoredObject& rMonitoredObject, SvSml::MonitorEntryData &data);


	
};


