//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidendader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RemoteMonitorListHelper
//* .File Name       : $Workfile:   RemoteMonitorListHelper.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Dec 2014 10:01:22  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "RemoteMonitorListHelper.h"
#include "SVValueObjectLibrary/SVValueObjectReference.h"
#pragma endregion Includes

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
			name = objectRef.GetCompleteName();
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
	if ( iLength >= 12 && name.substr(0, 12) == "Inspections." )
	{
		sObjectName = SvUl_SF::Right(name, iLength - 12);
	}
	else
	{
		sObjectName = name;
	}

	obj.guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(sObjectName.c_str());
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

