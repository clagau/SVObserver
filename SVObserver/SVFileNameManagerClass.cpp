//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameManagerClass
//* .File Name       : $Workfile:   SVFileNameManagerClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 14:19:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVFileNameManagerClass.h"
#include "SVFileNameClass.h"
#include "SVFileNameLockablePointerArrayClass.h"

#include <io.h>
#include <direct.h>
#include <sys/stat.h>

// Global Configuration Path Name variable for DLL
CString gcsConfigurationPathName;

// Global Run Path Name variable for DLL
CString gcsRunPathName;

// Global File Name array for DLL
SVFileNameLockablePointerArrayClass gsvFileNameArray;

//##ModelId=3A4B960A0280
SVFileNameManagerClass::SVFileNameManagerClass()
{
	CString csRunPathName = GetRunPathName();

	miCurrentItem = 0;

	if ( csRunPathName.IsEmpty() )
	{
		SetRunPathName( "C:\\RUN" );
	}
}

//##ModelId=3A4A3E72038A
LPCTSTR SVFileNameManagerClass::GetConfigurationPathName()
{
	return gcsConfigurationPathName;
}

//##ModelId=3A4A41370251
LPCTSTR SVFileNameManagerClass::GetRunPathName()
{
	return gcsRunPathName;
}

//##ModelId=3A4A4E0B0186
BOOL SVFileNameManagerClass::SetRunPathName(LPCTSTR szPathName)
{
	BOOL bOk = FALSE;

	if ( gsvFileNameArray.Lock() )
	{
		bOk = TRUE;

		gcsRunPathName = szPathName;

		if ( !gcsRunPathName.IsEmpty() )
		{
			bOk = CreatePath( gcsRunPathName );
		}

		gsvFileNameArray.Unlock();
	}

	return bOk;
}

//##ModelId=3A4A4F2200FA
BOOL SVFileNameManagerClass::SetConfigurationPathName(LPCTSTR szPathName)
{
	BOOL bOk = FALSE;

	if ( gsvFileNameArray.Lock() )
	{
		bOk = TRUE;

		gcsConfigurationPathName = szPathName;

		if ( !gcsConfigurationPathName.IsEmpty() )
		{
			bOk = CreatePath( gcsConfigurationPathName );
		}

		gsvFileNameArray.Unlock();
	}

	return bOk;
}

//##ModelId=3A522D32005D
BOOL SVFileNameManagerClass::FindItem(SVFileNameClass * svpFileNameClass)
{
	BOOL bOk = FALSE;

	if ( 0 < gsvFileNameArray.GetSize() )
	{
		BOOL bDone = FALSE;

		miCurrentItem = static_cast<int>(gsvFileNameArray.GetSize());
		
		for ( int i = 0; !bDone && i < gsvFileNameArray.GetSize(); i++ )
		{
			bDone = svpFileNameClass <= gsvFileNameArray[i];
			if ( bDone )
			{
				bOk = gsvFileNameArray[i] == svpFileNameClass;
				miCurrentItem = i;
			}
		}
	}
	else
	{
		miCurrentItem = 0;
	}

	return bOk;
}

//##ModelId=3A4B786E01C5
BOOL SVFileNameManagerClass::AddItem(SVFileNameClass * svpFileName)
{
	BOOL bOk = FALSE;

	if ( gsvFileNameArray.Lock() )
	{
		bOk = FindItem( svpFileName );

		if ( !bOk )
		{
			try
			{
				gsvFileNameArray.InsertAt( miCurrentItem, svpFileName );

				bOk = LoadItem( gsvFileNameArray[miCurrentItem] );
			}
			catch (CMemoryException * e)
			{
				e->Delete();
			}
		}

		gsvFileNameArray.Unlock();
	}

	return bOk;
}

//##ModelId=3A52217C034B
BOOL SVFileNameManagerClass::RemoveItem(SVFileNameClass * svpFileName)
{
	BOOL bOk = FALSE;

	if ( gsvFileNameArray.Lock() )
	{
		bOk = FindItem( svpFileName );

		if ( bOk )
		{
			gsvFileNameArray.RemoveAt( miCurrentItem );

			if ( 0 < gsvFileNameArray.GetSize() )
			{
				if ( gsvFileNameArray.GetSize() <= miCurrentItem )
				{
					miCurrentItem = static_cast<int>(gsvFileNameArray.GetSize()) - 1;
				}
			}
			else
			{
				miCurrentItem = 0;
			}
		}

		gsvFileNameArray.Unlock();
	}

	return bOk;
}

//##ModelId=3A4B78E60138
BOOL SVFileNameManagerClass::TopItem()
{
	miCurrentItem = 0;

	return TRUE;
}

//##ModelId=3A4B79090138
BOOL SVFileNameManagerClass::NextItem()
{
	BOOL bOk = FALSE;

	bOk = miCurrentItem < gsvFileNameArray.GetSize();

	if ( bOk )
	{
		miCurrentItem++;
	}

	return bOk;
}

//##ModelId=3A4B7924000F
BOOL SVFileNameManagerClass::EndOfItems()
{
	return gsvFileNameArray.GetSize() <= miCurrentItem;
}

//##ModelId=3A4B796B01B5
SVFileNameClass * SVFileNameManagerClass::GetItem()
{
	SVFileNameClass *pItem = NULL;

	if ( gsvFileNameArray.Lock() )
	{
		if ( miCurrentItem < gsvFileNameArray.GetSize() )
		{
			pItem = gsvFileNameArray[miCurrentItem];
		}

		gsvFileNameArray.Unlock();
	}

	return pItem;
}

//##ModelId=3A4B7C70008C
BOOL SVFileNameManagerClass::CreatePath(LPCTSTR szPathName)
{
	CString csPathName = szPathName;

	BOOL bOk = _access( csPathName, 0 ) == 0;

	if ( !bOk )
	{
		BOOL bDone = FALSE;

		int iStart=-1;
		if (csPathName.Left(2) == _T("\\\\"))	// UNC path
		{
			iStart = csPathName.Mid(2).Find('\\') + 2 - 1;
		}
		else	// drive letter
		{
			iStart = csPathName.Find(':');
		}

		ASSERT( iStart > 0 );

		do
		{
			bDone = csPathName.GetLength() <= iStart + 1;

			if (!bDone)
			{
				if ( csPathName[iStart + 1] == '\\' )
				{
					iStart++;

					bDone = csPathName.GetLength() <= iStart + 1;

					if ( !bDone )
					{
						bDone = csPathName[iStart + 1] == '\\';
						bOk = !bDone;
					}
				}

				if ( !bDone )
				{
					int iNextBS = csPathName.Find( '\\', iStart + 1 );
					int iNextFS = csPathName.Find( '/', iStart + 1 );

					CString csPathMake;

					if ( iNextBS == -1 && iNextFS == -1 )
					{
						csPathMake = csPathName;
					}
					else
					{
						if ( iNextFS == -1 || iNextBS != -1 && iNextBS < iNextFS )
						{
							csPathMake = csPathName.Left( iNextBS );
						}
						else
						{
							csPathMake = csPathName.Left( iNextFS );
						}
					}

					bOk = _access( csPathMake, 0 ) == 0;

					if ( !bOk )
					{
						bOk = _mkdir( csPathMake ) == 0;
					}

					iStart = csPathMake.GetLength();
				}
			}
		}
		while ( bOk && !bDone );
	}

	return bOk;
}

//##ModelId=3A560FF50000
BOOL SVFileNameManagerClass::LoadItem(SVFileNameClass * svFileName)
{
	BOOL bOk = TRUE;

	if ( !CString( svFileName->GetFileName() ).IsEmpty() )
	{
		CString csFullName = svFileName->GetFullFileName();
		CString csConfigFullName = svFileName->GetFullFileName();
		CString csRunFullName = CString( GetRunPathName() ) + 
		                        "\\" + svFileName->GetFileName();
		 
		if ( !CString( GetConfigurationPathName() ).IsEmpty() )
		{
			csConfigFullName = CString( GetConfigurationPathName() ) + 
			                   "\\" + svFileName->GetFileName();
		}

		csRunFullName.Replace( '/', '\\' );
		csConfigFullName.Replace ( '/', '\\' );
		csFullName.Replace( '/', '\\' );

		bOk = csRunFullName.CompareNoCase( csFullName ) == 0 &&
		      _access( csRunFullName, 0 ) == 0;

		if ( !bOk )
		{
			if ( csConfigFullName.CompareNoCase( csFullName ) != 0 )
			{
				if ( _access( csFullName, 0 ) == 0 )
				{
					bOk = CopyFile( csFullName, csRunFullName, FALSE );
					_chmod(csRunFullName,_S_IREAD | _S_IWRITE);
				}
			}

			if ( !bOk )
			{
				if ( _access( csConfigFullName, 0 ) == 0 )
				{
					bOk = CopyFile( csConfigFullName, csRunFullName, FALSE );
					_chmod(csRunFullName,_S_IREAD | _S_IWRITE);
				}
			}

			if ( !bOk )
			{
				bOk = _access( csRunFullName, 0 ) == 0;
			}

			if ( bOk )
			{
				bOk = svFileName->SetPathName( GetRunPathName() );
			}
		}
	}

	return bOk;
}

//##ModelId=3A55F7A9000F
BOOL SVFileNameManagerClass::LoadItems()
{
	BOOL bOk = FALSE;

	if ( gsvFileNameArray.Lock() )
	{
		bOk = TRUE;

		for ( int i = 0; i < gsvFileNameArray.GetSize() ; i++ )
		{
			bOk = LoadItem( gsvFileNameArray[i] ) && bOk;
		}

		gsvFileNameArray.Unlock();
	}

	return bOk;
}

//##ModelId=3A560337007D
BOOL SVFileNameManagerClass::SaveItem(SVFileNameClass * svFileName)
{
	BOOL bOk = TRUE;

	if ( !CString( svFileName->GetFileName() ).IsEmpty() )
	{
		CString csRunFullName = CString( GetRunPathName() ) + 
		                        "\\" + svFileName->GetFileName();

		csRunFullName.Replace( '/', '\\' );

		if ( csRunFullName.CompareNoCase( svFileName->GetFullFileName() ) )
		{
			bOk = _access( svFileName->GetFullFileName(), 0 ) != 0;

			if ( !bOk )
			{
				bOk = CopyFile( svFileName->GetFullFileName(), csRunFullName, FALSE );
				_chmod(csRunFullName,_S_IREAD | _S_IWRITE);
			}

			if ( bOk )
			{
				bOk = svFileName->SetPathName( GetRunPathName() );
			}
		}

		if ( bOk )
		{
			if ( !CString( GetConfigurationPathName() ).IsEmpty() )
			{
				CString csFullName = CString( GetConfigurationPathName() ) + 
				                     "\\" + CString( svFileName->GetFileName() );

				csFullName.Replace ( '/', '\\' );

				if ( csFullName.CompareNoCase( svFileName->GetFullFileName() ) )
				{
					bOk = _access( csRunFullName, 0 ) != 0;

					if ( !bOk )
					{
						bOk = CopyFile( csRunFullName, csFullName, FALSE );
						_chmod(csRunFullName,_S_IREAD | _S_IWRITE);
					}
				}
			}
		}
	}

	return bOk;
}

BOOL SVFileNameManagerClass::RenameItem(SVFileNameClass * svFileName)
{
	BOOL bOk = TRUE;

	if ( !CString( svFileName->GetFileName() ).IsEmpty() )
	{
		bOk = svFileName->SetPathName( GetRunPathName() );
	}

	return bOk;
}

//##ModelId=3A4B7AF10251
BOOL SVFileNameManagerClass::SaveItems()
{
	BOOL bOk = FALSE;

	if ( gsvFileNameArray.Lock() )
	{
		bOk = TRUE;

		for ( int i= 0; i < gsvFileNameArray.GetSize(); i++ )
		{
			bOk = SaveItem( gsvFileNameArray[i] ) && bOk;
		}

		gsvFileNameArray.Unlock();
	}

	return bOk;
}

//##ModelId=3A59E8470399
LPCTSTR SVFileNameManagerClass::GetFileNameList()
{
	return gsvFileNameArray.GetFileNameList();
}



BOOL SVFileNameManagerClass::RemoveUnusedFiles(BOOL bCleanConfigDir)
{
	BOOL bOk = TRUE;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	CString csFileMask = GetRunPathName();

	csFileMask += _T( "\\*.*" );

	hFind = FindFirstFile( csFileMask, &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		BOOL bRun = TRUE;

		while ( bRun )
		{
			BOOL bFound = FALSE;
			CString csFileName = FindFileData.cFileName;

			for ( int i = 0; ! bFound && i < gsvFileNameArray.GetSize(); i++ )
			{
				bFound = csFileName.CompareNoCase( gsvFileNameArray[i]->GetFileName() ) == 0;
			}

			if ( ! bFound )
			{
				CString csFullFileName = GetRunPathName();

				csFullFileName += _T( "\\" );
				csFullFileName += csFileName;

				_chmod(csFullFileName,_S_IREAD | _S_IWRITE);
				bOk = DeleteFile( csFullFileName );
			}

			bRun = FindNextFile( hFind, &FindFileData );
		}

    FindClose( hFind );
  }

	if ( bCleanConfigDir && bOk )
	{
		csFileMask = GetConfigurationPathName();

		if ( !csFileMask.IsEmpty() )
		{
			csFileMask += _T( "\\*.*" );

			hFind = FindFirstFile( csFileMask, &FindFileData );
			if ( hFind != INVALID_HANDLE_VALUE )
			{
				BOOL bRun = TRUE;

				while ( bRun )
				{
					BOOL bFound = FALSE;
					CString csFileName = FindFileData.cFileName;

					for ( int i = 0; ! bFound && i < gsvFileNameArray.GetSize(); i++ )
					{
						bFound = csFileName.CompareNoCase( gsvFileNameArray[i]->GetFileName() ) == 0;
					}

					if ( ! bFound )
					{
						CString csFullFileName = GetConfigurationPathName();

						csFullFileName += _T( "\\" );
						csFullFileName += csFileName;

						_chmod(csFullFileName,_S_IREAD | _S_IWRITE);
						bOk = DeleteFile( csFullFileName );
					}

					bRun = FindNextFile( hFind, &FindFileData );
				}

				FindClose( hFind );
			}
		}
	}

	return bOk;
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVFileNameManagerClass.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 14:19:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:35:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   21 Jun 2007 12:50:48   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   08 Jul 2003 11:10:22   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  352
 * SCR Title:  Update SVObserver to Version 4.30 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upped beta 4.22 to 7 and fixed problem with .sec files being deleted from the run directory on a load.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 May 2003 11:59:46   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  366
 * SCR Title:  Read only files are staying around in C:\Run
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   before CopyFile or DeleteFile it will make sure the file attributes are Read/Write
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   22 Apr 2003 09:45:26   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   02 Dec 2002 12:00:56   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  303
 * SCR Title:  Unable to save a configuration to a UNC path
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::CreatePath method to be able to correctly parse UNC paths.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Nov 2002 13:04:34   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   SaveItems
 *                        Modified to fix array-out-of-bounds error
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:19:14   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:17:16   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:17:08   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:16:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:16:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:16:12   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:16:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:15:54   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   11 Dec 2001 15:15:44   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  259
 * SCR Title:  File Management Problem with OCR Files and Pattern Match Model File
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated SVFileNameManagerClass::LoadItem to include new copy methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   12 Nov 2001 10:17:38   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Reverted to old code in ::LoadItem & ::SaveItem except for
 * one copy in ::LoadItem removed prior to saving
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   09 Nov 2001 13:17:10   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Made changes to fix configuration save as problem by adding function ::RenameItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   05 Nov 2001 09:17:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Change LoadItem to have default return of TRUE
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Nov 2001 16:51:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Restructured LoadItem and SaveItem to correct file loading/saving issues
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   31 Oct 2001 15:42:32   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  251
 * SCR Title:  Inconsistent/Incorrect display of OCR file names
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Removed code that copied files from the Run directory to the configuration directory during loading in function LoadItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   12 Jun 2001 14:39:06   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Modified the following functions:  FindItem, LoadItem.  Added the following functions: RemoveUnusedFiles.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   25 Jan 2001 16:16:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes fixed the SVObserverApp file handling and associated systems.  These systems include the .SEC, .IOD, and .IPD files as well as the camera files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Jan 2001 10:32:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixes full file name return value, and initial path and titles for dialog methods.
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
