//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListHelper
//* .File Name       : $Workfile:   RemoteMonitorListHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   08 Jul 2014 08:47:10  $
//******************************************************************************
#include "stdafx.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "RemoteMonitorListHelper.h"
#include "SVValueObjectReference.h"

SVString RemoteMonitorListHelper::GetNameFromMonitoredObject(const MonitoredObject& rMonitoredObject)
{
	SVString name;
	SVObjectReference objectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	if (objectRef.Object())
	{
		if (SV_IS_KIND_OF(objectRef.Object(), SVValueObjectClass))
		{
			SVValueObjectReference valRef(objectRef);
			if (rMonitoredObject.isArray)
			{
				if (rMonitoredObject.wholeArray)
				{
					valRef.SetEntireArray();
				}
				else
				{
					valRef.SetArrayIndex(rMonitoredObject.arrayIndex);
				}
			}
			name = valRef.GetCompleteOneBasedObjectName();
		}
		else
		{
			name = objectRef.GetCompleteObjectName();
		}
	}
	return name;
}

MonitoredObject RemoteMonitorListHelper::GetMonitoredObjectFromName(const SVString& name)
{
	MonitoredObject obj;
	obj.guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(name);
	SVObjectNameInfo nameInfo;
	SVObjectNameInfo::ParseObjectName(nameInfo, name.c_str());
	
	SVObjectReference ref(SVObjectManagerClass::Instance().GetObject(obj.guid), nameInfo);
	if (ref.Object() && SV_IS_KIND_OF(ref.Object(), SVValueObjectClass))
	{
		SVValueObjectReference valRef(ref);
		obj.isArray = valRef.Object()->IsArray();
		obj.wholeArray = valRef.IsEntireArray();
		if (obj.isArray)
		{
			obj.arrayIndex = valRef.ArrayIndex();
		}
	}
	return obj;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\RemoteMonitorListHelper.cpp_v  $
 * 
 *    Rev 1.0   08 Jul 2014 08:47:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  886
 * SCR Title:  Add RunReject Server Support to SVObserver
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/