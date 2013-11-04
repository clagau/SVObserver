//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision, Inc. Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImportedInputList
//* .File Name       : $Workfile:   SVImportedInputList.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 20:06:08  $
//******************************************************************************

#ifndef INCL_SVIMPORTEDINPUTLIST_H
#define INCL_SVIMPORTEDINPUTLIST_H

#include <boost/config.hpp>
#include <boost/any.hpp>
#include <deque>

#include "SVUtilityLibrary/SVString.h"

typedef std::deque<boost::any> SVImportedInputList;

struct SVImportedInput
{
	const SVString name;
	const long ppqPosition;
	
	SVImportedInput(const SVString& rName, long ppqPos)
	: name(rName), ppqPosition(ppqPos) {}
	virtual ~SVImportedInput() {}
};

struct SVImportedRemoteInput : SVImportedInput
{
	const _variant_t value;
	const long index;
	SVImportedRemoteInput(const SVString& rName, long ppqPos, const _variant_t& rValue, long idx)
	: SVImportedInput(rName, ppqPos)
	, index(idx)
	, value(rValue)
	{
	}
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVImportedInputList.h_v  $
 * 
 *    Rev 1.1   06 May 2013 20:06:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Apr 2013 14:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:56:48   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   05 Mar 2013 15:30:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
