//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoLockAndReleaseTemplate
//* .File Name       : $Workfile:   SVAutoLockAndReleaseTemplate.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:49:08  $
//******************************************************************************

#ifndef SVAUTOLOCKANDRELEASETEMPLATE_INL
#define SVAUTOLOCKANDRELEASETEMPLATE_INL

#include "SVAutoLockAndReleaseTemplate.h"

template< typename SVLockType >
SVAutoLockAndReleaseTemplate< SVLockType >::SVAutoLockAndReleaseTemplate()
: m_pLock( NULL )
{
}

template< typename SVLockType >
BOOL SVAutoLockAndReleaseTemplate< SVLockType >::Assign( SVLockType* p_pLock, DWORD p_WaitTime )
{
	BOOL l_Status = true;

	if( m_pLock != NULL )
	{
		m_pLock->Unlock();
	}

	m_pLock = p_pLock;

	if( m_pLock != NULL )
	{
		l_Status = m_pLock->Lock( p_WaitTime );

		if( !l_Status )
		{
			m_pLock = NULL;
		}
	}

	return l_Status;
}

template< typename SVLockType >
SVAutoLockAndReleaseTemplate< SVLockType >::~SVAutoLockAndReleaseTemplate()
{
	if( m_pLock != NULL )
	{
		m_pLock->Unlock();
	}
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVAutoLockAndReleaseTemplate.inl_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:49:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Feb 2011 12:17:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new Critical Section object and fixed bug in Auto Lock and Release object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Feb 2011 15:06:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated error path on Assign method to clear the lock if un-successful in establishing a lock.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Feb 2011 14:45:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new locking class to make sure locking and unlocking is working correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/