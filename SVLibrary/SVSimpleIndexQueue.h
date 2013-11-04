//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSimpleIndexQueue
//* .File Name       : $Workfile:   SVSimpleIndexQueue.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 13:51:30  $
//******************************************************************************
#ifndef SVSIMPLEINDEXQUEUE_H_
#define SVSIMPLEINDEXQUEUE_H_
#pragma once

#include <deque>
#include <vector>
#include "SVUtilityLibrary/SVUtilityGlobals.h"

class SVSimpleIndexQueue
{
public:
	SVSimpleIndexQueue();

	HRESULT reset( const std::vector<long>& vecIndexes );	// specify which indexes are available
	HRESULT reset( long lNumIndexes );	// initialized to [0, lNumIndexes)
	HRESULT clear();

	bool push(long& rlIndex);  // index is an [out] param; return value is success
	bool pop(long& rlIndex);   // index is an [out] param; return value is success
	bool is_used( long lIndex ) const;
	bool is_empty() const;

	void get_used( std::vector<long>& rlUsedList ) const;
	bool get_next_available_index( long& rlNextAvailable ) const;	// returns false if none available

	long get_used_count() const;
	long get_available_count() const;

private:
	std::deque<long> m_Available;
	std::deque<long> m_Used;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLibrary\SVSimpleIndexQueue.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:51:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Oct 2010 13:31:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to remove duplicate definition of HRESULT.  The SVHRESULT definition will be removed in place of the Microsoft defined HRESULT.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jan 2006 09:08:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  529
 * SCR Title:  Add Conditional Product History
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   initial revision
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
