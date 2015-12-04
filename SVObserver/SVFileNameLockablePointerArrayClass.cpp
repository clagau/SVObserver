//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLockableFileNameArrayClass
//* .File Name       : $Workfile:   SVFileNameLockablePointerArrayClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:54  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVFileNameLockablePointerArrayClass.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#pragma endregion Includes

//##ModelId=3A54D7EC0251
SVFileNameLockablePointerArrayClass::SVFileNameLockablePointerArrayClass()
:mcmArrayLock()
{
}

//##ModelId=3A54A0EB03D8
SVFileNameLockablePointerArrayClass::SVFileNameLockablePointerArrayClass(const SVFileNameLockablePointerArrayClass& orig)
{
	*this = orig;
}

SVFileNameLockablePointerArrayClass::~SVFileNameLockablePointerArrayClass()
{
}


//##ModelId=3A54A0EC003E
const SVFileNameLockablePointerArrayClass SVFileNameLockablePointerArrayClass::operator=(const SVFileNameLockablePointerArrayClass& rhs)
{
	Copy( rhs );

	return *this;
}

//##ModelId=3A54A6750280
LPCTSTR SVFileNameLockablePointerArrayClass::GetFileNameList()
{
	mcsFileList.Empty();

	for (int i = 0; i < GetSize(); i++)
	{
		if ( i )
		{
			mcsFileList += ";";
		}

		mcsFileList += GetAt(i)->GetFullFileName();
	}

	return mcsFileList;
}

//##ModelId=3A535EFE009C
BOOL SVFileNameLockablePointerArrayClass::Lock()
{
	return mcmArrayLock.Lock( 300000 );
}

//##ModelId=3A535F67000F
BOOL SVFileNameLockablePointerArrayClass::Unlock()
{
	return mcmArrayLock.Unlock();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileNameLockablePointerArrayClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:36:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Oct 2007 16:37:48   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added destructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 Apr 2003 09:45:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jun 2001 14:35:42   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified constructor & operator=.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jan 2001 16:16:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Jan 2001 13:18:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This revision add the newly created classes for the file management added to SVObserver Project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/