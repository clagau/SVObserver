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
#include "SVUtilityLibrary/SVSAFEARRAY.h"

#include "SVImageProcessingClass.h"
#include "SVInfoStructs.h"
#include "SVInspectionProcess.h"
#include "SVValueObjectReference.h"
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
			SVObjectReference ref = Iter->second;

			if (nullptr != ref.Object())
			{
				HRESULT TempStatus = S_OK;

				// Check if it's a ValueObject or an ImageObject
				// as these are the only items that can be gotten from the inspection remotely, currently
				if (nullptr != dynamic_cast<SVValueObjectClass*>(ref.Object()))
				{
					TempStatus = UpdateResultsWithValueData(Iter->first, ref, TriggerCount);
				}
				else if (nullptr != dynamic_cast<SVImageClass*>(ref.Object()))
				{
					TempStatus = UpdateResultsWithImageData(Iter->first, ref, TriggerCount);
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

HRESULT SVCommandInspectionGetItems::UpdateResultsWithImageData(const SVString& rItemName, const SVObjectReference& rImageRef, unsigned long TriggerCnt)
{
	HRESULT Status = S_OK;

	SVImageClass* pImage = dynamic_cast<SVImageClass*>(rImageRef.Object());

	if (nullptr != pImage)
	{
		HRESULT GetStatus = S_OK;

		SVStorage Storage;
		SVSmartHandlePointer ImageHandlePtr;
		unsigned long TriggerCount = TriggerCnt;

		// Special check for Color Tool's RGBMainImage which is HSI
		if (nullptr != dynamic_cast<SVRGBMainImageClass*>(pImage))
		{
			SVSmartHandlePointer TempHandlePtr;

			pImage->GetImageHandle(TempHandlePtr);

			if (!(TempHandlePtr.empty()))
			{
				SVImageProcessingClass::Instance().CreateImageBuffer(TempHandlePtr, SVImageHLSToRGB, ImageHandlePtr);
			}
		}
		else
		{
			pImage->GetImageHandle(ImageHandlePtr);
		}

		if (!(ImageHandlePtr.empty()))
		{
			SVString FileName = SvUl_SF::Format(_T("V:\\%ld-%s.bmp"), TriggerCount, pImage->GetUniqueObjectID().ToString().c_str());

			GetStatus = SVImageProcessingClass::Instance().SaveImageBuffer(FileName.c_str(), ImageHandlePtr);

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

HRESULT SVCommandInspectionGetItems::UpdateResultsWithValueData(const SVString& rItemName, const SVValueObjectReference& rValueRef, unsigned long TriggerCnt)
{
	HRESULT Status = S_OK;
	HRESULT GetStatus = S_OK;

	SVStorage Storage;
	unsigned long TriggerCount = TriggerCnt;
	
	if (!rValueRef.IsEntireArray())
	{
		///if this is an Array this is Zerro based!!!!
		GetStatus = rValueRef.GetValue(Storage.m_Variant.GetVARIANT());

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

		GetStatus = rValueRef.GetValues(Value); 

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

HRESULT SVCommandInspectionGetItems::UpdateResultsWithErrorData(const SVString& rItemName, HRESULT errorStatus, unsigned long triggerCount)
{
	HRESULT Status = S_OK;
	SVStorage storage;
		
	m_ResultItems[rItemName] = SVStorageResult(storage, errorStatus, triggerCount);

	return Status;
}

