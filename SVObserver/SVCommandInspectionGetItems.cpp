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
				else if (nullptr != dynamic_cast<SVImageObjectClass*>(ref.Object()))
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
			SVString FileName;

			FileName.Format(_T("V:\\%ld-%s.bmp"), TriggerCount, pImage->GetUniqueObjectID().ToString().c_str());

			GetStatus = SVImageProcessingClass::Instance().SaveImageBuffer(FileName.c_str(), ImageHandlePtr);

			if (S_OK == GetStatus)
			{
				Storage.m_StorageType = SVVisionProcessor::SVStorageImageFileName;
				Storage.m_Variant = FileName.ToVARIANT();
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCommandInspectionGetItems.cpp_v  $
 * 
 *    Rev 1.4   13 Nov 2014 10:09:08   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  932
 * SCR Title:  Clean up GetInspectionItems and SVCommandInspectionGetItemsPtr (SVO-150)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use inspection-object instead of ID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Mar 2014 15:19:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  869
 * SCR Title:  Add PPQ and Environment Variables to Object Manager and Update Pickers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed the object name access (due to Object manager changes)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Mar 2014 18:09:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added regions.
 *   Added DEBUG_NEW.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Jun 2013 15:25:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 May 2013 12:57:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  824
 * SCR Title:  Add Remote Access to SVObserver Application Configuration Variables
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated execute funcitonality to include new name requirements found in IF00100.9401.003.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 09:56:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/