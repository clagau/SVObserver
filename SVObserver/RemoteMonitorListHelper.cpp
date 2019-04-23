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
#include "SVUtilityLibrary/StringHelper.h"
#include "InspectionEngine/SVImageClass.h"
#include "SVSharedMemoryLibrary/MonitorEntry.h"
#include "SVSharedMemoryLibrary/MonitorListCpy.h"
#include "SVSharedMemoryLibrary/SVSharedMemorySettings.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"

#pragma endregion Includes

std::string RemoteMonitorListHelper::GetNameFromMonitoredObject(const MonitoredObject& rMonitoredObject, bool completeName /*=true*/)
{
	std::string Result;
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
		}
		Result = completeName ? ObjectRef.GetCompleteName(true) : ObjectRef.GetObjectNameToObjectType(SvPb::SVInspectionObjectType, true);
	}

	if(!Result.empty() && 0 != Result.find(SvDef::FqnInspections) && completeName)
	{
		std::string InspectionsPrefix(SvDef::FqnInspections);
		InspectionsPrefix += _T(".");
		Result = InspectionsPrefix + Result;
	}
	return Result;
}

DWORD RemoteMonitorListHelper::GetTypeFromMonitoredObject(const MonitoredObject& rMonitoredObject)
{
	SVObjectReference objectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	return  objectRef.getObject()->GetObjectType();
}


void RemoteMonitorListHelper::GetPropertiesFromMonitoredObject(const MonitoredObject& rMonitoredObject, SvSml::MonitorEntryData &data)
{
	SVObjectReference ObjectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	data.wholeArray = rMonitoredObject.wholeArray;
	data.isArray= rMonitoredObject.isArray;
	data.arrayIndex = rMonitoredObject.arrayIndex;
	data.ObjectType = ObjectRef.getObject()->GetObjectType();
	
	SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*>  (ObjectRef.getObject());
	if (pValueObject)
	{
		data.variant_type = pValueObject->GetType();
	}
	else
	{
		SvIe::SVImageClass* pImageObject = dynamic_cast<SvIe::SVImageClass*>(ObjectRef.getObject());
		if (nullptr != pImageObject)
		{
			SvTrc::IImagePtr pImageBuffer = pImageObject->getLastImage(true);
			if (nullptr != pImageBuffer && !pImageBuffer->isEmpty())
			{
				MatroxImageProps Imageprops;
				SVMatroxBufferInterface::InquireBufferProperties(pImageBuffer->getHandle()->GetBuffer(), Imageprops);
				data.SetMatroxImageProps(Imageprops);
			}
		}
	}
}


MonitoredObject RemoteMonitorListHelper::GetMonitoredObjectFromName(const std::string& name)
{
	MonitoredObject Result;

	std::string sObjectName;
	SVObjectNameInfo nameInfo;
	SVObjectNameInfo::ParseObjectName(nameInfo, name.c_str());
	//Check to see that first part of name is Inspections
	if (std::string(SvDef::FqnInspections) == nameInfo.m_NameArray[0])
	{
		SVObjectReference ObjectRef;
		SVObjectManagerClass::Instance().GetObjectByDottedName(nameInfo.GetObjectArrayName(0), ObjectRef);
		Result.guid = (nullptr != ObjectRef.getObject()) ? ObjectRef.getObject()->GetUniqueObjectID() : GUID_NULL;
		if (nullptr != ObjectRef.getValueObject())
		{
			Result.isArray = ObjectRef.getValueObject()->isArray();
			Result.wholeArray = ObjectRef.isEntireArray();
			if (Result.isArray)
			{
				Result.arrayIndex = ObjectRef.ArrayIndex();
			}
		}
	}
	return Result;
}

void RemoteMonitorListHelper::AddMonitorObject2MonitorListcpy(const MonitoredObjectList& values, SvSml::ListType::typ listtype,SvSml::MonitorListCpy& molcpy )
{
	MonitoredObjectList::const_iterator it;

	for(it = values.begin(); it != values.end() ; ++it)
	{
		std::string name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		SvSml::MonitorEntryPointer MeP = molcpy.AddEntry(listtype, name);
		RemoteMonitorListHelper::GetPropertiesFromMonitoredObject(*it, MeP->data);
		MeP->m_Guid = it->guid;
		if(listtype == SvSml::ListType::productItemsImage)
		{
			assert(MeP->data.ObjectType== SvPb::SVObjectTypeEnum::SVImageObjectType );
		}
	}
}


SvSml::MonitorListCpyPointer  RemoteMonitorListHelper::CreateMLcopy(const RemoteMonitorNamedList& remoteMonitorNamedlist)
{

	SVConfigurationObject* pConfig(nullptr);
	SVObjectManagerClass::Instance().GetConfigurationObject(pConfig);
	int len(0);
	if (pConfig)
	{
		SVPPQObject* pPPQ(nullptr);
		pConfig->GetPPQByName(remoteMonitorNamedlist.GetPPQName().c_str(), &pPPQ);
		if (pPPQ)
		{
			len = pPPQ->GetPPQLength();
		}
	}
	const SvSml::SVSharedMemorySettings& rSettings = SvSml::SharedMemWriter::Instance().GetSettings();
	int ProductDepth = len + rSettings.GetNumProductSlot();
	SvSml::MonitorListCpyPointer  MLCpPtr  = SvSml::MonitorListCpyPointer(new SvSml::MonitorListCpy);
	MLCpPtr->SetMonitorlistname(remoteMonitorNamedlist.GetName()); 
	MLCpPtr->SetRejectDepth(remoteMonitorNamedlist.GetRejectDepthQueue() + SvSml::MLPPQInfo::NumRejectSizeDelta);
	MLCpPtr->SetProductDepth(ProductDepth);
	MLCpPtr->SetIsActive(remoteMonitorNamedlist.IsActive());
	MLCpPtr->SetPPQname(remoteMonitorNamedlist.GetPPQName());
	MLCpPtr->SetProductFilter(remoteMonitorNamedlist.GetProductFilter());
	
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetProductValuesList(),SvSml::ListType::productItemsData,*(MLCpPtr.get()));
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetFailStatusList(),SvSml::ListType::failStatus,*(MLCpPtr.get()));
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetRejectConditionList(),SvSml::ListType::rejectCondition,*(MLCpPtr.get()));
	AddMonitorObject2MonitorListcpy(remoteMonitorNamedlist.GetProductImagesList(),SvSml::ListType::productItemsImage,*(MLCpPtr.get()));

	return MLCpPtr;
}

				  
