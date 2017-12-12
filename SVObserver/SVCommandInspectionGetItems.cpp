//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionGetItems
//* .File Name       : $Workfile:   SVCommandInspectionGetItems.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   13 Nov 2014 10:09:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVCommandInspectionGetItems.h"

#include "SVObjectLibrary/SVObjectManagerClass.h"
#include "SVUtilityLibrary/SVSafeArray.h"

#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

#pragma region Constructor
SVCommandInspectionGetItems::SVCommandInspectionGetItems(const SVInspectionProcess& rInspection, const SVNameObjectSet& rItemNames)
: m_Inspection(&rInspection)
, m_ItemNames(rItemNames )
, m_ResultItems()
{
}

SVCommandInspectionGetItems::~SVCommandInspectionGetItems()
{
}
#pragma endregion Constructor

HRESULT SVCommandInspectionGetItems::Execute()
{
	HRESULT Status = S_OK;

	if (nullptr != m_Inspection)
	{
		SVProductInfoStruct Product = m_Inspection->LastProductGet(SV_OTHER);
		unsigned long TriggerCount = Product.ProcessCount();

		for (SVNameObjectSet::const_iterator Iter = m_ItemNames.begin(); SUCCEEDED(Status) && Iter != m_ItemNames.end(); ++Iter)
		{
			const SVObjectReference& rObjRef = Iter->second;

			if (nullptr != rObjRef.getObject())
			{
				HRESULT TempStatus = S_OK;

				// Check if it's a ValueObject or an ImageObject
				// as these are the only items that can be gotten from the inspection remotely, currently
				SVObjectClass* pObject = rObjRef.getObject();
				if (nullptr != rObjRef.getValueObject())
				{
					TempStatus = UpdateResultsWithValueData(Iter->first, rObjRef, TriggerCount);
				}
				else if( nullptr != dynamic_cast<SVImageClass*> (pObject) )
				{
					TempStatus = UpdateResultsWithImageData(Iter->first, rObjRef, TriggerCount);
				}
				else
				{
					 TempStatus = UpdateResultsWithErrorData(Iter->first, SVMSG_OBJECT_WRONG_TYPE, TriggerCount);
				}

				if (S_OK == Status)
				{
					Status = TempStatus;
				}
			}
			else if (S_OK == Status)
			{
				Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}
	else
	{
		Status = E_FAIL;
	}

	return Status;
}

bool SVCommandInspectionGetItems::empty() const
{
	bool Status = true;

	Status = Status && (nullptr == m_Inspection);
	Status = Status && (m_ItemNames.empty());
	Status = Status && (m_ResultItems.empty());

	return Status;
}

const SVCommandInspectionGetItems::SVNameObjectSet& SVCommandInspectionGetItems::GetItemNames() const
{
	return m_ItemNames;
}

const SVNameStorageResultMap& SVCommandInspectionGetItems::GetResultItems() const
{
	return m_ResultItems;
}

HRESULT SVCommandInspectionGetItems::UpdateResultsWithImageData(const std::string& rItemName, const SVObjectReference& rImageRef, unsigned long TriggerCnt)
{
	HRESULT Status = S_OK;

	SVImageClass* pImage = dynamic_cast<SVImageClass*> (rImageRef.getObject());

	if (nullptr != pImage)
	{
		HRESULT GetStatus = S_OK;

		SVStorage Storage;
		SVImageBufferHandlePtr ImageHandlePtr;
		unsigned long TriggerCount = TriggerCnt;

		pImage->GetImageHandle(ImageHandlePtr);

		if (nullptr != ImageHandlePtr)
		{
			std::string FileName = SvUl::Format(_T("V:\\%ld-%s.bmp"), TriggerCount, pImage->GetUniqueObjectID().ToString().c_str());

			GetStatus = SVImageProcessingClass::SaveImageBuffer(FileName.c_str(),SVMatroxFileTypeEnum::SVFileBitmap , ImageHandlePtr);

			if (S_OK == GetStatus)
			{
				Storage.m_StorageType = SVVisionProcessor::SVStorageImageFileName;
				Storage.m_Variant = _variant_t(FileName.c_str());
			}
			else
			{
				TriggerCount = 0;
				Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			}
		}
		else
		{
			TriggerCount = 0;
			GetStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}

		m_ResultItems[rItemName] = SVStorageResult(Storage, GetStatus, TriggerCount);
	}
	else
	{
		Status = E_UNEXPECTED;
	}

	return Status;
}

HRESULT SVCommandInspectionGetItems::UpdateResultsWithValueData(const std::string& rItemName, const SVObjectReference& rValueRef, unsigned long TriggerCnt)
{
	HRESULT Status = S_OK;
	HRESULT GetStatus = S_OK;

	SVStorage Storage;
	unsigned long TriggerCount = TriggerCnt;
	
	SvOi::IValueObject* pValueObject = rValueRef.getValueObject();
	if( nullptr != pValueObject && !rValueRef.isEntireArray())
	{
		///if this is an Array this is Zero based!!!!
		GetStatus = pValueObject->getValue( Storage.m_Variant, rValueRef.getValidArrayIndex() );

		if (S_OK == GetStatus)
		{
			Storage.m_StorageType = SVVisionProcessor::SVStorageValue;
		}
		else
		{
			TriggerCount = 0;
			Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
	}
	else
	{
		std::vector<_variant_t> Value;

		if( nullptr != pValueObject )
		GetStatus = pValueObject->getValues( Value );

		if (S_OK == GetStatus)
		{
			SVSAFEARRAY SafeArray;

			SafeArray.assign(Value.begin(), Value.end());

			Storage.m_StorageType = SVVisionProcessor::SVStorageValue;
			Storage.m_Variant = SafeArray;
		}
		else 
		{
			TriggerCount = 0;
			Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
	}

	m_ResultItems[rItemName] = SVStorageResult(Storage, GetStatus, TriggerCount);

	return Status;
}

HRESULT SVCommandInspectionGetItems::UpdateResultsWithErrorData(const std::string& rItemName, HRESULT errorStatus, unsigned long triggerCount)
{
	HRESULT Status = S_OK;
	SVStorage storage;
		
	m_ResultItems[rItemName] = SVStorageResult(storage, errorStatus, triggerCount);

	return Status;
}

