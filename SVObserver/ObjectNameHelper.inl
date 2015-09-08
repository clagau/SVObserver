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
#include <set>
#include <boost/assign/list_of.hpp> // for 'list_of()'
#include "ObjectNameHelper.h"
#include "ObjectInterfaces\SVObjectTypeInfoStruct.h"
#include "ObjectInterfaces\ObjectDefines.h"
#pragma endregion Includes

typedef std::set<SVObjectTypeEnum> SVObjectTypeEnumSet;

bool IsExcluded(SVObjectTypeEnum type, const SVObjectTypeEnumSet& rFilter)
{
	return (rFilter.size() > 0 && rFilter.end() == rFilter.find(type));
}

bool IsViewable(UINT attributesAllowed)
{
	return (attributesAllowed & SV_VIEWABLE);
}

bool IsSameLinage(const SVString& name, const SVString& excludedPath)
{
	bool bSame = false; 
	size_t len = excludedPath.size();
	if (len > 0)
	{
		bSame = (0 ==  name.Left(len).Compare(excludedPath));
	}
	return bSame;
}

bool IsAllowed(SVObjectTypeEnum type, UINT attributesAllowed, const CString& name, const SVObjectTypeEnumSet& filter, const SVString& excludePath)
{
	return (IsViewable(attributesAllowed) && !IsSameLinage(name, excludePath) && !IsExcluded(type, filter));
}

template<typename Inserter>
int ObjectNameHelper::BuildObjectNameList( SVTaskObjectListClass* pTaskObjectList, Inserter inserter, const CString& ExcludedPath, ObjectNameFilter efilter)
{
	int ret(0);
	SVObjectTypeEnumSet Filter;
	switch (efilter)
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

	int nCount = OutputList.GetSize();
	for (int i = 0; i < nCount; i++)
	{
		SVOutObjectInfoStruct* pInfoItem = OutputList.GetAt(i);
		if (pInfoItem)
		{
			SVObjectReference ref = pInfoItem->GetObjectReference();

			if (ref.Object())
			{
				CString CompleteName(ref->GetCompleteObjectName());
				const SVObjectTypeInfoStruct& typeInfo = pInfoItem->ObjectTypeInfo;
				if (IsAllowed(typeInfo.ObjectType, ref->ObjectAttributesAllowed(), CompleteName, Filter, ExcludedPath))
				{
					++ret;
					inserter = CompleteName;
				}
			}
		}
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