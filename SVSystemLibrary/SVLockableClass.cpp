//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableClass
//* .File Name       : $Workfile:   SVLockableClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 16:37:54  $
//******************************************************************************

#include "stdafx.h"
#include "SVLockableClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace Seidenader
{
	namespace SVSystemLibrary
	{
		SVLockableClass::SVLockableClass()
			: hProtectionMutex( NULL )
		{
			hProtectionMutex = CreateMutex( NULL, FALSE, NULL );
		}

		SVLockableClass::~SVLockableClass()
		{
			if( hProtectionMutex != NULL )
			{
				CloseHandle( hProtectionMutex );
			}
		}


		BOOL SVLockableClass::Lock( DWORD DWWaitTime /* = INFINITE */ ) const
		{
			BOOL l_Status = ( hProtectionMutex != NULL );

			if( l_Status )
			{
				l_Status = ( WaitForSingleObject( hProtectionMutex, DWWaitTime ) == WAIT_OBJECT_0 );
			}

			return l_Status;
		}

		BOOL SVLockableClass::Unlock() const
		{
			BOOL l_Status = ( hProtectionMutex != NULL );

			if( l_Status )
			{
				l_Status = ReleaseMutex( hProtectionMutex );
			}

			return l_Status;
		}

		HANDLE SVLockableClass::GetLockHandle()
		{
			return hProtectionMutex;
		}
	} //SVSystemLibrary
} //Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObjectLibrary\SVLockableClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 16:37:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Feb 2011 11:59:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Object manager to use new Critical Section class and updated Lockable Class to remove unused attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 Dec 2010 15:51:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Oct 2007 16:14:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed WaitForSingleObject in destructor so the handle will be freed under all conditions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Feb 2005 14:52:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First version of file for SVObjectLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 10:57:46   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/

