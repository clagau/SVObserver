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
#include "SVOCore/SVImageClass.h"
#include "SVSharedMemoryLibrary/MonitorEntry.h"
#include "SVSharedMemoryLibrary/MonitorListCpy.h"

#pragma endregion Includes

SVString RemoteMonitorListHelper::GetNameFromMonitoredObject(const MonitoredObject& rMonitoredObject)
{
	SVString Result;

	SVObjectReference ObjectRef( SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid) );
	if( nullptr != ObjectRef.getObject() )
	{
		if( ObjectRef.getValueObject() )
		{
			if (rMonitoredObject.isArray)
			{
				if (rMonitoredObject.wholeArray)
				{
					ObjectRef.SetEntireArray();
				}
				else
				{
					ObjectRef.SetArrayIndex(rMonitoredObject.arrayIndex);
				}
			}
			Result = ObjectRef.GetCompleteOneBasedObjectName();
		}
		else
		{
			Result = ObjectRef.GetCompleteName();
		}
	}
	return Result;
}

DWORD RemoteMonitorListHelper::GetTypeFromFromMonitoredObject(const MonitoredObject& rMonitoredObject)
{
	SVObjectReference objectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	return  objectRef.getObject()->GetObjectType();
}

DWORD RemoteMonitorListHelper::GetSizeFromFromMonitoredObject(const MonitoredObject& rMonitoredObject)
{
	DWORD size(0);
	SVObjectReference ObjectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	SVImageClass* pImageObject = dynamic_cast<SVImageClass*> (ObjectRef.getObject());
	if( pImageObject)
	{
		long  height(0), width(0); 
		pImageObject->GetImageExtents().GetExtentProperty( SVExtentPropertyHeight, height );
		pImageObject->GetImageExtents().GetExtentProperty( SVExtentPropertyWidth, width );
		size = height * width;
	}
	return size;
	
}



MonitoredObject RemoteMonitorListHelper::GetMonitoredObjectFromName(const SVString& name)
{
	MonitoredObject Result;

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

	Result.guid = SVObjectManagerClass::Instance().GetObjectIdFromCompleteName(sObjectName.c_str());
	SVObjectNameInfo nameInfo;
	SVObjectNameInfo::ParseObjectName(nameInfo, name.c_str());
	
	SVObjectReference ObjectRef(  SVObjectManagerClass::Instance().GetObject( Result.guid ), nameInfo );
	if( nullptr != ObjectRef.getValueObject() )
	{
		Result.isArray = ObjectRef.getValueObject()->isArray();
		Result.wholeArray = ObjectRef.isEntireArray();
		if( Result.isArray )
		{
			Result.arrayIndex = ObjectRef.ArrayIndex();
		}
	}
	return Result;
}

void RemoteMonitorListHelper::AddMonitorObjects2MoListEntryVector(const MonitoredObjectList& values, SvSml::MonitorEntryVector  &ListEntries )
{
	MonitoredObjectList::const_iterator it;
	SvSml::MonitorEntry Entry;
	for(it = values.begin(); it != values.end() ; ++it)
	{
		Entry.name =  RemoteMonitorListHelper::GetNameFromMonitoredObject(*it).c_str();
		Entry.size = RemoteMonitorListHelper::GetSizeFromFromMonitoredObject(*it);
		Entry.type = RemoteMonitorListHelper::GetTypeFromFromMonitoredObject(*it);
		ListEntries.push_back(Entry);	
	}

}


void RemoteMonitorListHelper::InsertRemotMonitorNamedList2MonitorListcpy(const RemoteMonitorNamedList& remoteMonitorNamedlist,SvSml::MonitorListCpy  &monitorListCpy )
{

	monitorListCpy.m_name = remoteMonitorNamedlist.GetName(); 
	monitorListCpy.m_rejectDepth = remoteMonitorNamedlist.GetRejectDepthQueue(); 
	monitorListCpy.m_IsActive = remoteMonitorNamedlist.IsActive();
	monitorListCpy.m_ppq = remoteMonitorNamedlist.GetPPQName();;
	monitorListCpy.m_ProductFilter = remoteMonitorNamedlist.GetProductFilter();

	monitorListCpy.prodItems.clear();
	monitorListCpy.failStats.clear();
	monitorListCpy.rejctCond.clear();
	AddMonitorObjects2MoListEntryVector(remoteMonitorNamedlist.GetProductValuesList(),monitorListCpy.prodItems);
	AddMonitorObjects2MoListEntryVector(remoteMonitorNamedlist.GetProductImagesList(),monitorListCpy.prodItems);
	AddMonitorObjects2MoListEntryVector(remoteMonitorNamedlist.GetFailStatusList(),monitorListCpy.failStats);
	AddMonitorObjects2MoListEntryVector(remoteMonitorNamedlist.GetRejectConditionList(),monitorListCpy.rejctCond);

	

}
