// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVContainableCriticalSection
// * .File Name       : $Workfile:   SVContainableCriticalSection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 11:52:20  $
// ******************************************************************************

#include "stdafx.h"
#include "SVContainableCriticalSection.h"

// these functions are in the .CPP instead of the .INL
// because they have WINNT 0x0500 specific function calls.
// This will allow the code to be used in projects where WINNT 0x0500 is not defined.
SVContainableCriticalSection::SVContainableCriticalSection( DWORD p_dwSpinCount /*= 0*/ )
{
	m_plRefCount = new long;
	*m_plRefCount = 1;
	m_pCritSec = new CRITICAL_SECTION;
	m_dwSpinCount = p_dwSpinCount;
	::InitializeCriticalSectionAndSpinCount(m_pCritSec, m_dwSpinCount);
}


DWORD SVContainableCriticalSection::SetSpinCount( DWORD dwSpinCount )
{
	return ::SetCriticalSectionSpinCount( m_pCritSec, dwSpinCount );
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOLibrary\SVContainableCriticalSection.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 11:52:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Sep 2009 09:22:52   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Jun 2005 14:06:24   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved from SVOMFCLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   20 Jun 2005 09:28:28   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  450
 * SCR Title:  Add asynchronous functionality to the archive tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
