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
#include "SVImageLibrary/SVImageBufferHandleImage.h"

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

DWORD RemoteMonitorListHelper::GetTypeFromMonitoredObject(const MonitoredObject& rMonitoredObject)
{
	SVObjectReference objectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	return  objectRef.getObject()->GetObjectType();
}

DWORD RemoteMonitorListHelper::GetSizeFromMonitoredObject(const MonitoredObject& rMonitoredObject)
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

void RemoteMonitorListHelper::GetImagePropertiesFromMonitoredObject(const MonitoredObject& rMonitoredObject,MatroxImageProps& Imageprops)
{
	SVObjectReference objectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	SVImageClass* pImageObject = dynamic_cast<SVImageClass*>(objectRef.getObject());
	if( nullptr !=  pImageObject)
	{
		SVSmartHandlePointer imageHandlePtr;
		// Special check for Color Tool's RGBMainImage which is HSI ???
		pImageObject->GetImageHandle(imageHandlePtr);
		if(!imageHandlePtr.empty())
		{
			SVImageBufferHandleImage MilHandle;
			imageHandlePtr->GetData( MilHandle );
			SVMatroxBufferInterface::InquireBufferProperties(MilHandle.GetBuffer(),Imageprops); 
		}
		
		
	}
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

void RemoteMonitorListHelper::AddMonitorObject2MonitorListcpy(const MonitoredObjectList& values, SvSml::ListType::typ listtype,SvSml::MonitorListCpy& molcpy )
{
	///TODO CALCULATE OFFSET and Image Store Index!!!!
	MonitoredObjectList::const_iterator it;
	SvSml::MonitorEntry Entry;
	for(it = values.begin(); it != values.end() ; ++it)
	{
		SVString name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		DWORD type =	RemoteMonitorListHelper::GetTypeFromMonitoredObject(*it);
		SvSml::MonitorEntryPointer MeP  = molcpy.AddEntry(listtype,name,type);
		if(listtype == SvSml::ListType::productItemsImage)
		{
			assert(type == SVObjectTypeEnum::SVImageObjectType );
			MatroxImageProps ImageProperties;
			RemoteMonitorListHelper::GetImagePropertiesFromMonitoredObject(*it,ImageProperties);
			MeP->SetMatroxImageProps(ImageProperties);
		}
		else
		{
			MeP->size =  RemoteMonitorListHelper::GetSizeFromMonitoredObject(*it);
		}
		
	}

}


SvSml::MonitorListCpyPointer  RemoteMonitorListHelper::CreateMLcopy(const RemoteMonitorNamedList& remoteMonitorNamedlist)
{
	SvSml::MonitorListCpyPointer  MLCpPtr  = SvSml::MonitorListCpyPointer(new SvSml::MonitorListCpy);
	MLCpPtr->SetMonitorlistname(remoteMonitorNamedlist.GetName()); 
	MLCpPtr->SetRejectDepth(remoteMonitorNamedlist.GetRejectDepthQueue()); 
	MLCpPtr->SetIsActive(remoteMonitorNamedlist.IsActive());
	MLCpPtr->SetPPQname(remoteMonitorNamedlist.GetPPQName());
	MLCpPtr->SetProductFilter(remoteMonitorNamedlist.GetProductFilter());
	
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetProductValuesList(),SvSml::ListType::productItemsData,*(MLCpPtr.get()));
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetFailStatusList(),SvSml::ListType::failStatus,*(MLCpPtr.get()));
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetRejectConditionList(),SvSml::ListType::rejectCondition,*(MLCpPtr.get()));
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetProductImagesList(),SvSml::ListType::productItemsImage,*(MLCpPtr.get()));

	return MLCpPtr;
}

				  
void RemoteMonitorListHelper::RemotMonitorNamedList2MonitorListcpy(const RemoteMonitorNamedList& remoteMonitorNamedlist,SvSml::MonitorListCpy&  monitorListCpy )
{
	monitorListCpy.ClearAll();
	monitorListCpy.SetMonitorlistname(remoteMonitorNamedlist.GetName()); 
	monitorListCpy.SetRejectDepth(remoteMonitorNamedlist.GetRejectDepthQueue()); 
	monitorListCpy.SetIsActive(remoteMonitorNamedlist.IsActive());
	monitorListCpy.SetPPQname(remoteMonitorNamedlist.GetPPQName());
	monitorListCpy.SetProductFilter(remoteMonitorNamedlist.GetProductFilter());

	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetProductValuesList(),SvSml::ListType::productItemsData,monitorListCpy);
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetFailStatusList(),SvSml::ListType::failStatus,monitorListCpy);
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetRejectConditionList(),SvSml::ListType::rejectCondition,monitorListCpy);
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetProductImagesList(),SvSml::ListType::productItemsImage,monitorListCpy);


}

