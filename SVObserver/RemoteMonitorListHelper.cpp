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
#include "SVSharedMemoryLibrary/SVSharedMemorySettings.h"
#include "SVSharedMemoryLibrary/SharedMemWriter.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "ObjectInterfaces/IValueObject.h"
#include "SVPPQObject.h"
#include "SVConfigurationObject.h"

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


void RemoteMonitorListHelper::GetPropertiesFromMonitoredObject(const MonitoredObject& rMonitoredObject, SvSml::MonitorEntryData &data)
{
	SVObjectReference ObjectRef(SVObjectManagerClass::Instance().GetObject(rMonitoredObject.guid));
	data.wholeArray = rMonitoredObject.wholeArray;
	data.isArray= rMonitoredObject.isArray;
	data.arrayIndex = rMonitoredObject.arrayIndex;
	data.ObjectType = ObjectRef.getObject()->GetObjectType();
	
	SVImageClass* pImageObject(nullptr);
	SvOi::IValueObject* pValueObject = dynamic_cast<SvOi::IValueObject*>  (ObjectRef.getObject());
	if (pValueObject)
	{
		data.ByteSize = pValueObject->GetByteSize();
		data.variant_type = pValueObject->GetType();
	}
	else
	{
		pImageObject = dynamic_cast<SVImageClass*>(ObjectRef.getObject());
	}
	
	if (nullptr != pImageObject)
	{
		SVSmartHandlePointer imageHandlePtr;
		// Special check for Color Tool's RGBMainImage which is HSI ???
		pImageObject->GetImageHandle(imageHandlePtr);
		if (!imageHandlePtr.empty())
		{
			SVImageBufferHandleImage MilHandle;
			imageHandlePtr->GetData(MilHandle);
			MatroxImageProps Imageprops;
			SVMatroxBufferInterface::InquireBufferProperties(MilHandle.GetBuffer(), Imageprops);
			data.sizeX = Imageprops.sizeX;
			data.sizeY = Imageprops.sizeY;
			data.PitchByte = Imageprops.PitchByte;
			data.Pitch = Imageprops.Pitch;
			data.Matrox_type = Imageprops.Matrox_type;
			data.Attrib = Imageprops.Attrib;
			data.BandSize = Imageprops.Bandsize;
			data.ByteSize = Imageprops.Bytesize;

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
	MonitoredObjectList::const_iterator it;

	for(it = values.begin(); it != values.end() ; ++it)
	{
		SVString name = RemoteMonitorListHelper::GetNameFromMonitoredObject(*it);
		SvSml::MonitorEntryPointer MeP = molcpy.AddEntry(listtype, name);
		RemoteMonitorListHelper::GetPropertiesFromMonitoredObject(*it, MeP->data);
		MeP->m_Guid = it->guid;
		if(listtype == SvSml::ListType::productItemsImage)
		{
			assert(MeP->data.ObjectType== SVObjectTypeEnum::SVImageObjectType );
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

				  
