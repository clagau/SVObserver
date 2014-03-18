//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionGetItems
//* .File Name       : $Workfile:   SVCommandInspectionGetItems.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   17 Mar 2014 15:19:06  $
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
SVCommandInspectionGetItems::SVCommandInspectionGetItems()
: m_InspectionId(), m_ItemNames(), m_ResultItems()
{
}

SVCommandInspectionGetItems::SVCommandInspectionGetItems(const SVCommandInspectionGetItems& p_rObject)
: m_InspectionId( p_rObject.m_InspectionId ), m_ItemNames( p_rObject.m_ItemNames ), m_ResultItems( p_rObject.m_ResultItems )
{
}

SVCommandInspectionGetItems::SVCommandInspectionGetItems(const SVGUID& p_rInspectionId, const SVItemNameSet& p_rItemNames)
: m_InspectionId( p_rInspectionId ), m_ItemNames( p_rItemNames ), m_ResultItems()
{
}

SVCommandInspectionGetItems::~SVCommandInspectionGetItems()
{
}
#pragma endregion Constructor

HRESULT SVCommandInspectionGetItems::Execute()
{
	HRESULT l_Status = S_OK;

	SVObjectClass* l_pObject = SVObjectManagerClass::Instance().GetObject( GetInspectionId() );
	SVInspectionProcess* l_pInspection = dynamic_cast< SVInspectionProcess* >( l_pObject );

	if( l_pInspection != NULL )
	{
		SVProductInfoStruct l_Product = l_pInspection->LastProductGet( SV_OTHER );
		unsigned long l_TriggerCount = l_Product.ProcessCount();

		for( SVItemNameSet::const_iterator l_Iter = m_ItemNames.begin(); SUCCEEDED( l_Status ) && l_Iter != m_ItemNames.end(); ++l_Iter )
		{
			SVObjectNameInfo l_Info;

			SVObjectNameInfo::ParseObjectName( l_Info, *l_Iter );

			if( l_Info.m_NameArray[ 0 ] == "Inspections" )
			{
				SVObjectReference ref;
	
				SVObjectManagerClass::Instance().GetObjectByDottedName( l_Info.GetObjectArrayName( 0 ), ref );

				if( ref.Object() != NULL )
				{
					HRESULT l_TempStatus = S_OK;

					if( dynamic_cast< SVValueObjectClass* >( ref.Object() ) != NULL )
					{
						l_TempStatus = UpdateResultsWithValueData( *l_Iter, ref, l_TriggerCount );
					}
					else
					{
						l_TempStatus = UpdateResultsWithImageData( *l_Iter, ref, l_TriggerCount );
					}

					if( l_Status == S_OK )
					{
						l_Status = l_TempStatus;
					}
				}
				else if( l_Status == S_OK )
				{
					l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
				}
			}
			else if( l_Status == S_OK )
			{
				l_Status = SVMSG_NOT_ALL_LIST_ITEMS_PROCESSED;
			}
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

bool SVCommandInspectionGetItems::empty() const
{
	bool l_Status = true;

	l_Status = l_Status && ( m_InspectionId.empty() );
	l_Status = l_Status && ( m_ItemNames.empty() );
	l_Status = l_Status && ( m_ResultItems.empty() );

	return l_Status;
}

void SVCommandInspectionGetItems::clear()
{
	m_InspectionId.clear();
	m_ItemNames.clear();
	m_ResultItems.clear();
}

const SVGUID& SVCommandInspectionGetItems::GetInspectionId() const
{
	return m_InspectionId;
}

const SVCommandInspectionGetItems::SVItemNameSet& SVCommandInspectionGetItems::GetItemNames() const
{
	return m_ItemNames;
}

HRESULT SVCommandInspectionGetItems::SetCommandData(const SVGUID& p_rInspectionId, const SVItemNameSet& p_rItemNames)
{
	HRESULT l_Status = S_OK;

	m_InspectionId = p_rInspectionId;
	m_ItemNames = p_rItemNames;

	m_ResultItems.clear();

	return l_Status;
}

const SVNameStorageResultMap& SVCommandInspectionGetItems::GetResultItems() const
{
	return m_ResultItems;
}

HRESULT SVCommandInspectionGetItems::UpdateResultsWithImageData( const SVString& p_rItemName, const SVObjectReference& p_rImageRef, unsigned long p_TriggerCount )
{
	HRESULT l_Status = S_OK;

	SVImageClass* l_pImage = dynamic_cast< SVImageClass* >( p_rImageRef.Object() );

	if( l_pImage != NULL )
	{
		HRESULT l_GetStatus = S_OK;

		SVStorage l_Storage;
		SVSmartHandlePointer l_ImageHandlePtr;
		unsigned long l_TriggerCount = p_TriggerCount;

		// Special check for Color Tool's RGBMainImage which is HSI
		if( dynamic_cast< SVRGBMainImageClass* >( l_pImage ) != NULL )
		{
			SVSmartHandlePointer l_TempHandlePtr;

			l_pImage->GetImageHandle( l_TempHandlePtr );

			if( !( l_TempHandlePtr.empty() ) )
			{
				SVImageProcessingClass::Instance().CreateImageBuffer( l_TempHandlePtr, SVImageHLSToRGB, l_ImageHandlePtr );
			}
		}
		else
		{
			l_pImage->GetImageHandle( l_ImageHandlePtr );
		}

		if( !( l_ImageHandlePtr.empty() ) )
		{
			SVString l_FileName;

			l_FileName.Format( _T( "V:\\%ld-%s.bmp" ), l_TriggerCount, l_pImage->GetUniqueObjectID().ToString().c_str() );

			l_GetStatus = SVImageProcessingClass::Instance().SaveImageBuffer( l_FileName.c_str(), l_ImageHandlePtr );

			if( l_GetStatus == S_OK )
			{
				l_Storage.m_StorageType = SVVisionProcessor::SVStorageImageFileName;
				l_Storage.m_Variant = l_FileName.ToVARIANT();
			}
			else
			{
				l_TriggerCount = 0;

				l_Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			}
		}
		else
		{
			l_TriggerCount = 0;

			l_GetStatus = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
			l_Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}

		m_ResultItems[ p_rItemName ] = SVStorageResult( l_Storage, l_GetStatus, l_TriggerCount );
	}
	else
	{
		l_Status = E_UNEXPECTED;
	}

	return l_Status;
}

HRESULT SVCommandInspectionGetItems::UpdateResultsWithValueData( const SVString& p_rItemName, const SVValueObjectReference& p_rValueRef, unsigned long p_TriggerCount )
{
	HRESULT l_Status = S_OK;
	HRESULT l_GetStatus = S_OK;

	SVStorage l_Storage;
	unsigned long l_TriggerCount = p_TriggerCount;

	if ( !p_rValueRef.IsEntireArray() )
	{
		l_GetStatus = p_rValueRef.GetValue( l_Storage.m_Variant.GetVARIANT() );

		if( l_GetStatus == S_OK )
		{
			l_Storage.m_StorageType = SVVisionProcessor::SVStorageValue;
		}
		else
		{
			l_TriggerCount = 0;

			l_Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
	}
	else
	{
		std::vector< _variant_t > l_Value;

		l_GetStatus = p_rValueRef.GetValues( l_Value ); 

		if( l_GetStatus == S_OK )
		{
			SVSAFEARRAY l_SafeArray;

			l_SafeArray.assign( l_Value.begin(), l_Value.end() );

			l_Storage.m_StorageType = SVVisionProcessor::SVStorageValue;
			l_Storage.m_Variant = l_SafeArray;
		}
		else 
		{
			l_TriggerCount = 0;

			l_Status = SVMSG_ONE_OR_MORE_REQUESTED_OBJECTS_DO_NOT_EXIST;
		}
	}

	m_ResultItems[ p_rItemName ] = SVStorageResult( l_Storage, l_GetStatus, l_TriggerCount );

	return l_Status;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCommandInspectionGetItems.cpp_v  $
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