//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListHelper
//* .File Name       : $Workfile:   RemoteMonitorListHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Dec 2014 13:06:44  $
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

	SVString sObjectName;
	size_t iLength = name.size();
	//Check to see if first part of name is Inspections. if so remove it.
	if ( name.Left(12) == "Inspections." )
	{
		sObjectName = name.Right(iLength - 12);
	}
	else
	{
		sObjectName = name;
	}

	obj.guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(sObjectName);
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
 *    Rev 1.1   12 Dec 2014 13:06:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  918
 * SCR Title:  Implement Method RegisterMonitorList for RemoteControl (SVO-369)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   changed GetMonitoredObjectFromName to remove "Inspections." if it exists.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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