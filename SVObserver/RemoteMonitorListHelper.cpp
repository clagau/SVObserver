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
#include "InspectionEngine/SVImageClass.h"
#include "SVSharedMemoryLibrary/MonitorEntry.h"
#include "SVSharedMemoryLibrary/MonitorListCpy.h"
#include "SVSharedMemoryLibrary/SVSharedMemorySettings.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"

#pragma endregion Includes

std::string RemoteMonitorListHelper::GetNameFromMonitoredObject(const MonitoredObject& rMonitoredObject, bool completeName /*=true*/)
{
	std::string Result;
	if( nullptr != rMonitoredObject.m_objectRef.getObject() )
	{
		Result = completeName ? rMonitoredObject.m_objectRef.GetCompleteName(true) : rMonitoredObject.m_objectRef.GetObjectNameToObjectType(SvPb::SVInspectionObjectType, true);
	}

	if(!Result.empty() && false == Result.starts_with(SvDef::FqnInspections) && completeName)
	{
		std::string InspectionsPrefix(SvDef::FqnInspections);
		InspectionsPrefix += _T(".");
		Result = InspectionsPrefix + Result;
	}
	return Result;
}

void RemoteMonitorListHelper::GetPropertiesFromMonitoredObject(const MonitoredObject& rMonitoredObject, SvSml::MonitorEntryData &data)
{
	if (auto* pFinalObj = rMonitoredObject.m_objectRef.getFinalObject(); nullptr != pFinalObj)
	{
		data.ObjectType = pFinalObj->GetObjectType();
	}
	
	auto* pValueObject = dynamic_cast<SvOi::IValueObject*>  (rMonitoredObject.m_objectRef.getObject());
	if (pValueObject)
	{
		data.variant_type = pValueObject->GetType();
	}
	else
	{
		auto* pImageObject = dynamic_cast<SvIe::SVImageClass*>(rMonitoredObject.m_objectRef.getObject());
		if (nullptr != pImageObject)
		{
			SvOi::SVImageBufferHandlePtr pImageBuffer = pImageObject->getLastImage(true);
			if (nullptr != pImageBuffer && !pImageBuffer->empty())
			{
				MatroxImageProps Imageprops;
				SVMatroxBufferInterface::InquireBufferProperties(pImageBuffer->GetBuffer(), Imageprops);
				data.SetMatroxImageProps(Imageprops);
			}
		}
	}
}


MonitoredObject RemoteMonitorListHelper::GetMonitoredObjectFromName(const std::string& name)
{
	MonitoredObject Result;
	SVObjectNameInfo nameInfo;
	SVObjectNameInfo::ParseObjectName(nameInfo, name);
	//Check to see that first part of name is Inspections
	if (std::string(SvDef::FqnInspections) == nameInfo.m_NameArray[0])
	{
		SVObjectManagerClass::Instance().GetObjectByDottedName(nameInfo.GetObjectArrayName(0), Result.m_objectRef);
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
		MeP->m_objectId = it->m_objectRef.getObjectId();
		if(listtype == SvSml::ListType::productItemsImage)
		{
			Log_Assert(MeP->data.ObjectType== SvPb::SVObjectTypeEnum::SVImageObjectType );
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
			len = pPPQ->getPPQLength();
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

				  
