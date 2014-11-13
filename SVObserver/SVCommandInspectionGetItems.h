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

#ifndef SVCOMMANDINSPECTIONGETITEMS_H
#define SVCOMMANDINSPECTIONGETITEMS_H

#include <set>
#include "SVUtilityLibrary/SVGUID.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVStorageResult.h"
#include "SVInspectionProcess.h"

class SVObjectReference;
class SVValueObjectReference;

struct SVCommandInspectionGetItems
{
	typedef std::pair<SVString, SVObjectReference> SVFullNameObjectPair;
	typedef std::set< SVFullNameObjectPair > SVNameObjectSet;

	SVCommandInspectionGetItems();
	SVCommandInspectionGetItems(const SVInspectionProcess& p_rInspection, const SVNameObjectSet& p_rItemNames);

	virtual ~SVCommandInspectionGetItems();

	HRESULT Execute();

	bool empty() const;
	void clear();

	const SVNameObjectSet& GetItemNames() const;

	HRESULT SetCommandData(const SVInspectionProcess& p_rInspection, const SVNameObjectSet& p_rItemNames);

	const SVNameStorageResultMap& GetResultItems() const;

protected:
	HRESULT UpdateResultsWithImageData( const SVString& p_rItemName, const SVObjectReference& p_rImageRef, unsigned long p_TriggerCount );
	HRESULT UpdateResultsWithValueData( const SVString& p_rItemName, const SVValueObjectReference& p_rValueRef, unsigned long p_TriggerCount );

private:
	const SVInspectionProcess *m_Inspection;
	SVNameObjectSet m_ItemNames;
	SVNameStorageResultMap m_ResultItems;

};

typedef SVSharedPtr< SVCommandInspectionGetItems > SVCommandInspectionGetItemsPtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVCommandInspectionGetItems.h_v  $
 * 
 *    Rev 1.1   13 Nov 2014 10:09:08   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  932
 * SCR Title:  Clean up GetInspectionItems and SVCommandInspectionGetItemsPtr (SVO-150)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   use inspection-object instead of ID
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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