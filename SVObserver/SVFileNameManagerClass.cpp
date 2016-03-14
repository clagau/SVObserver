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

#pragma region Includes
#include "stdafx.h"
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include "SVFileNameManagerClass.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVFileNameLockablePointerArrayClass.h"
#pragma endregion Includes

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
