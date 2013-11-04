//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameArrayClass
//* .File Name       : $Workfile:   SVFileNameArrayClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:36:20  $
//******************************************************************************

#include "stdafx.h"
#include "SVFileNameArrayClass.h"

//##ModelId=3A55CE1B0119
SVFileNameArrayClass::SVFileNameArrayClass()
{
}

//##ModelId=3A55CE1B0167
SVFileNameArrayClass::SVFileNameArrayClass(const SVFileNameArrayClass& orig)
{
	*this = orig;
}

//##ModelId=3A5A39C60203
SVFileNameArrayClass::~SVFileNameArrayClass()
{
}

//##ModelId=3A55CE1B01C5
const SVFileNameArrayClass& SVFileNameArrayClass::operator=(const SVFileNameArrayClass& rhs)
{
	Copy( rhs );

	return *this;
}

//##ModelId=3A55CCDA004E
LPCTSTR SVFileNameArrayClass::GetFileNameList()
{
	mcsFileList.Empty();

	for (int i = 0; i < GetSize(); i++)
	{
		if ( i )
		{
			mcsFileList += ";";
		}

		mcsFileList += GetAt(i).GetFullFileName();
	}

	return mcsFileList;
}

//##ModelId=3A55F7DF0399
BOOL SVFileNameArrayClass::SetFileNameList(LPCTSTR szFileNameList)
{
	BOOL bOk = TRUE;
	BOOL bDone = FALSE;

	int i = 0;

	CString csTemp;
	CString csFileNameList = szFileNameList;

	RemoveAll();

	do
	{
		bDone = csFileNameList.IsEmpty();

		if ( !bDone )
		{
			bOk = FALSE;

			i = csFileNameList.FindOneOf( ";," );

			if ( i < 0 )
			{
				csTemp = csFileNameList;
				csFileNameList.Empty();
			}
			else
			{
				csTemp = csFileNameList.Left( i );
				csFileNameList.Delete( 0 , i + 1 );
			}

      if ( !csTemp.IsEmpty() )
			{
				bOk = 0 <= Add( SVFileNameClass( csTemp ) );
			}
		}
	}
	while ( bOk && !bDone );

	return bOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVFileNameArrayClass.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 10:36:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 09:33:20   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Nov 2002 12:55:56   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added #include "SVConfigurationTags.h"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jun 2001 13:58:06   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions:
 * class SVFileNameArrayClass -- SVFileNameArrayClass, operator=.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Jan 2001 16:16:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Jan 2001 13:18:52   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This revision add the newly created classes for the file management added to SVObserver Project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/