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
#include "SVMatroxLibrary\MatroxImageProps.h"


class RemoteMonitorListHelper
{
public:

	/// adds the content from RemoteMonitorNamedList to monitorlistcpy 
	static void RemotMonitorNamedList2MonitorListcpy(const RemoteMonitorNamedList& remoteMonitorNamedlist,SvSml::MonitorListCpy  &monitorListCpy ); 

	static SvSml::MonitorListCpyPointer  RemoteMonitorListHelper::CreateMLcopy(const RemoteMonitorNamedList& remoteMonitorNamedlist);

	/// returns a monitorobject for the objectname 
	static MonitoredObject GetMonitoredObjectFromName(const SVString& name);
	/// GetTHe name of the Monitorobject
	static SVString GetNameFromMonitoredObject(const MonitoredObject& rMonitoredObject);
	
private:	
	//static void AddMonitorObjects2MoListEntryVector(const MonitoredObjectList& values, SvSml::MonitorEntries  &ListEntries);
	static void AddMonitorObject2MonitorListcpy(const MonitoredObjectList& values, SvSml::ListType::typ listtype,SvSml::MonitorListCpy& molcpy );
	static DWORD GetTypeFromMonitoredObject(const MonitoredObject& rMonitoredObject);
	static DWORD GetSizeFromMonitoredObject(const MonitoredObject& rMonitoredObject); 
	static void GetImagePropertiesFromMonitoredObject(const MonitoredObject& rMonitoredObject,MatroxImageProps& props);
	
};


