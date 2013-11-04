//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCriticalSection
//* .File Name       : $Workfile:   SVCriticalSection.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:50:06  $
//******************************************************************************

#ifndef SVCRITICALSECTION_H
#define SVCRITICALSECTION_H

#include "SVUtilityLibrary/SVSharedPtr.h"

class SVCriticalSection
{
public:
	SVCriticalSection( DWORD p_SpinCount = 4000 );

	virtual ~SVCriticalSection();

	BOOL Lock( DWORD p_TimeOutMilliseconds = INFINITE ) const;
	BOOL Unlock() const;

protected:
	BOOL EnterLock() const;
	BOOL TryEnterLock() const;

	DWORD m_SpinCount;

	// This attribute denotes whether the lock is created.
	mutable bool m_IsLockCreated;

	// This attribute manages the lock for multi-threaded access to this container.
	mutable CRITICAL_SECTION m_Lock;

private:
	// Do Not Implement
	SVCriticalSection( const SVCriticalSection& p_rObject );

	// Do Not Implement
	const SVCriticalSection& operator=( const SVCriticalSection& p_rObject );

};

typedef SVSharedPtr< SVCriticalSection > SVCriticalSectionPtr;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVCriticalSection.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:50:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Feb 2011 12:17:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Critical Section object and fixed bug in Auto Lock and Release object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
