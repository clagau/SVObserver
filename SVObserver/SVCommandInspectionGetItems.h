//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCommandInspectionGetItems
//* .File Name       : $Workfile:   SVCommandInspectionGetItems.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:56:26  $
//******************************************************************************

#ifndef SVCOMMANDINSPECTIONGETITEMS_H
#define SVCOMMANDINSPECTIONGETITEMS_H

#include <set>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVStorageResult.h"

class SVObjectReference;
class SVValueObjectReference;

struct SVCommandInspectionGetItems
{
	typedef std::set< SVString > SVItemNameSet;

	SVCommandInspectionGetItems();
	SVCommandInspectionGetItems(const SVCommandInspectionGetItems& p_rObject);
	SVCommandInspectionGetItems(const SVGUID& p_rInspectionId, const SVItemNameSet& p_rItemNames);

	virtual ~SVCommandInspectionGetItems();

	HRESULT Execute();

	bool empty() const;
	void clear();

	const SVGUID& GetInspectionId() const;
	const SVItemNameSet& GetItemNames() const;

	HRESULT SetCommandData(const SVGUID& p_rInspectionId, const SVItemNameSet& p_rItemNames);

	const SVNameStorageResultMap& GetResultItems() const;

protected:
	HRESULT UpdateResultsWithImageData( const SVString& p_rItemName, const SVObjectReference& p_rImageRef, unsigned long p_TriggerCount );
	HRESULT UpdateResultsWithValueData( const SVString& p_rItemName, const SVValueObjectReference& p_rValueRef, unsigned long p_TriggerCount );

private:
	SVGUID m_InspectionId;
	SVItemNameSet m_ItemNames;
	SVNameStorageResultMap m_ResultItems;

};

typedef SVSharedPtr< SVCommandInspectionGetItems > SVCommandInspectionGetItemsPtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCommandInspectionGetItems.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:56:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   11 Feb 2013 12:50:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  812
 * SCR Title:  Add New Remote Command Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Initial check-in for new framework objects used for new remote socket interface.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/