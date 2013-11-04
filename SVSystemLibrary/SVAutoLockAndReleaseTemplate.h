//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVAutoLockAndReleaseTemplate
//* .File Name       : $Workfile:   SVAutoLockAndReleaseTemplate.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:48:54  $
//******************************************************************************

#ifndef SVAUTOLOCKANDRELEASETEMPLATE_H
#define SVAUTOLOCKANDRELEASETEMPLATE_H

template< typename SVLockType >
class SVAutoLockAndReleaseTemplate
{
public:
	SVAutoLockAndReleaseTemplate();

	virtual ~SVAutoLockAndReleaseTemplate();

	BOOL Assign( SVLockType* p_pLock, DWORD p_WaitTime = INFINITE );

protected:
	SVLockType* m_pLock;

};

#include "SVAutoLockAndReleaseTemplate.inl"

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVAutoLockAndReleaseTemplate.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:48:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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