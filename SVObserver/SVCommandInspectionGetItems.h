//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionGetItems
//* .File Name       : $Workfile:   SVCommandInspectionGetItems.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   13 Nov 2014 10:09:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVStorageResult.h"
#include "SVInspectionProcess.h"
#pragma endregion Includes

class SVObjectReference;
class SVValueObjectReference;

struct SVCommandInspectionGetItems
{
	typedef std::pair<SVString, SVObjectReference> SVFullNameObjectPair;
	typedef std::set<SVFullNameObjectPair> SVNameObjectSet;

	SVCommandInspectionGetItems(const SVInspectionProcess& p_rInspection, const SVNameObjectSet& p_rItemNames);

	virtual ~SVCommandInspectionGetItems();

	HRESULT Execute();

	bool empty() const;

	const SVNameObjectSet& GetItemNames() const;
	const SVNameStorageResultMap& GetResultItems() const;

protected:
	HRESULT UpdateResultsWithImageData(const SVString& rItemName, const SVObjectReference& rImageRef, unsigned long TriggerCount);
	HRESULT UpdateResultsWithValueData(const SVString& rItemName, const SVValueObjectReference& rValueRef, unsigned long TriggerCount);
	HRESULT UpdateResultsWithErrorData(const SVString& rItemName, HRESULT errorStatus, unsigned long TriggerCount);

private:
	const SVInspectionProcess* m_Inspection;
	SVNameObjectSet m_ItemNames;
	SVNameStorageResultMap m_ResultItems;
};

typedef SVSharedPtr<SVCommandInspectionGetItems> SVCommandInspectionGetItemsPtr;

