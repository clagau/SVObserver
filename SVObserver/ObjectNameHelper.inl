//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved 
//******************************************************************************
//* .Module Name     : ObjectNameHelper
//* .File Name       : $Workfile:   ObjectNameHelper.inl  $
//* .Description     : Implement ObjectNameHelper::BuildObjectNameList with template inserter
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   19 Dec 2014 13:53:40  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectNameHelper.h"
#include "ObjectInterfaces/SVObjectTypeInfoStruct.h"
#include <vector>
#include <set>
#pragma endregion Includes

#include <boost/assign/list_of.hpp> // for 'list_of()'

template<typename Inserter>
int ObjectNameHelper::BuildObjectNameList( SVTaskObjectListClass* pTaskObjectList, Inserter inserter, const CString& ExcludedPath, ObjectNameFilter efilter )
{
	int ret(0);
	std::set<SVObjectTypeEnum> Filter;
	switch(efilter)
	{
	case ENF_RANGE:
		Filter = boost::assign::list_of(SVDWordValueObjectType)(SVLongValueObjectType)(SVDoubleValueObjectType)(SVBoolValueObjectType)(SVPointValueObjectType)(SVByteValueObjectType);
		break;
	default:
		// Do nothing.
		break;
	}

	SVOutputInfoListClass OutputList;
	pTaskObjectList->GetOutputList( OutputList );
	SVObjectReferenceVector objectList;

	int nCount = OutputList.GetSize();
	for(int i = 0; i < nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoItem = nullptr;

		pInfoItem = OutputList.GetAt(i);
		//check pInfoItem
		if( nullptr == pInfoItem )
		{
			continue;
		}

		bool bFilterExclude = ( Filter.size() > 0 && Filter.end() == Filter.find(pInfoItem->ObjectTypeInfo.ObjectType) );

		if( bFilterExclude )
		{
			continue;
		}

		SVObjectReference ref = pInfoItem->GetObjectReference();

		if(ref.Object() == nullptr)
		{
			continue;
		}
		
		bool bViewable = (ref->ObjectAttributesAllowed() & SV_VIEWABLE);
		if(bViewable == false)
		{
			continue;
		}
		//getCompleteObjectName returns a CString
		CString CompleteName(ref->GetCompleteObjectName()); 
		
		int EPLen = ExcludedPath.GetLength();
		if( EPLen >0 )
		{
			bool bIsExcludePath = (0 ==  CompleteName.Left(EPLen).Compare(ExcludedPath));
			if( bIsExcludePath )
			{
				continue;
			}
		}

		++ret;
		inserter = CompleteName;
	}
	return ret;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\ObjectNameHelper.inl_v  $
 * 
 *    Rev 1.0   19 Dec 2014 13:53:40   mEichengruen
 * Project:  SVObserver
 * Change Request (SCR) nbr:  979
 * SCR Title:  Provide additional options to input the feature range for the blob analyzer.
 * Checked in by:  mEichengruen;  Marcus Eichengruen
 * Change Description:  
 *   Initial Check In for SCR 979 Indirect Range Objects 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/