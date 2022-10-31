//******************************************************************************
//* COPYRIGHT (c) 2013 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVInspectionProcess.h"
#include "InspectionEngine/SVImageProcessingClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/SVSafeArray.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

#pragma region Constructor


CommandInspectionGetItems::CommandInspectionGetItems(const SVInspectionProcess& rInspection, const SVNameObjectSet& rItemNames)
	: m_Inspection(&rInspection)
	, m_ItemNames(rItemNames)
{
	m_pResultItems = std::make_shared<SVNameStorageResultMap>();

}

CommandInspectionGetItems::~CommandInspectionGetItems()
{
	;
}

std::pair<HRESULT, std::shared_ptr<SVNameStorageResultMap>> CommandInspectionGetItems::operator()()
{
	HRESULT Status = S_OK;

	if (nullptr != m_Inspection)
	{
		auto product = m_Inspection->getLastProductData();
		unsigned long TriggerCount = product.first.lTriggerCount;

		for (SVNameObjectSet::const_iterator Iter = m_ItemNames.begin(); SUCCEEDED(Status) && Iter != m_ItemNames.end(); ++Iter)
		{
			const SVObjectReference& rObjRef = Iter->second;

			if (nullptr != rObjRef.getObject())
			{
				HRESULT TempStatus = S_OK;

				// Check if it's a ValueObject or an ImageObject
				// as these are the only items that can be gotten from the inspection remotely, currently
				SVObjectClass* pObject = rObjRef.getObject();
				SvIe::SVImageClass* pImage = nullptr;
				if (nullptr != rObjRef.getValueObject())
				{
					TempStatus = UpdateResultsWithValueData(Iter->first, rObjRef, TriggerCount);
				}
				else if (nullptr != (pImage = dynamic_cast<SvIe::SVImageClass*> (pObject)))
				{
					TempStatus = UpdateResultsWithImageData(Iter->first, rObjRef, TriggerCount, product.second.m_triggerRecordComplete);
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

	return std::make_pair(Status, m_pResultItems);
}



HRESULT CommandInspectionGetItems::UpdateResultsWithImageData(const std::string& rItemName, const SVObjectReference& rImageRef, unsigned long TriggerCnt, const SvOi::ITriggerRecordRPtr pTriggerRecord)
{
	HRESULT Status = S_OK;

	SvIe::SVImageClass* pImage = dynamic_cast<SvIe::SVImageClass*> (rImageRef.getObject());

	if (nullptr != pImage)
	{
		HRESULT GetStatus = S_OK;

		SVStorage Storage;
		unsigned long TriggerCount = TriggerCnt;
		SvOi::ITRCImagePtr pImageBuffer = pImage->getImageReadOnly(pTriggerRecord.get());

		if (nullptr != pImageBuffer && !pImageBuffer->isEmpty())
		{
			std::string Temp = SvUl::Format(_T("%ld-%u.bmp"), TriggerCount, pImage->getObjectId());
			std::string FileName = SvStl::GlobalPath::Inst().GetPathOnRamDrive(Temp.c_str());

			GetStatus = SvIe::SVImageProcessingClass::SaveImageBuffer(FileName.c_str(), ImageFileFormat::bmp, pImageBuffer->getHandle());

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

		(*m_pResultItems)[rItemName] = SVStorageResult(Storage, GetStatus, TriggerCount);
	}
	else
	{
		Status = E_UNEXPECTED;
	}

	return Status;
}

HRESULT CommandInspectionGetItems::UpdateResultsWithValueData(const std::string& rItemName, const SVObjectReference& rValueRef, unsigned long TriggerCnt)
{
	HRESULT Status = S_OK;
	HRESULT GetStatus = S_OK;

	SVStorage Storage;
	unsigned long TriggerCount = TriggerCnt;

	SvOi::IValueObject* pValueObject = rValueRef.getValueObject();
	if (nullptr != pValueObject)
	{
		int index = rValueRef.isEntireArray() ? -1 : rValueRef.getValidArrayIndex();
		//Enumeration Value objects need to return the text and not the value
		if (SvPb::SVEnumValueObjectType == rValueRef.getObject()->GetObjectSubType())
		{
			std::string Value;
			GetStatus = pValueObject->getValue(Value, index);
			Storage.m_Variant.SetString(Value.c_str());
		}
		else
		{
			///if this is an Array this is Zero based!!!!
			GetStatus = pValueObject->getValue(Storage.m_Variant, index);
		}

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

	(*m_pResultItems)[rItemName] = SVStorageResult(Storage, GetStatus, TriggerCount);

	return Status;
}

HRESULT CommandInspectionGetItems::UpdateResultsWithErrorData(const std::string& rItemName, HRESULT errorStatus, unsigned long triggerCount)
{
	HRESULT Status = S_OK;
	SVStorage storage;

	(*m_pResultItems)[rItemName] = SVStorageResult(storage, errorStatus, triggerCount);

	return Status;
}
