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
//Moved to precompiled header: #include <io.h>
//Moved to precompiled header: #include <direct.h>
//Moved to precompiled header: #include <sys/stat.h>
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <exception>
#include "SVFileNameManagerClass.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "ObjectInterfaces/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#pragma endregion Includes

SVFileNameManagerClass& SVFileNameManagerClass::Instance()
{
	static SVFileNameManagerClass fileNameMgr;
	return fileNameMgr;
}

SVFileNameManagerClass::SVFileNameManagerClass()
{
	CString csRunPathName = GetRunPathName();

	m_iCurrentItem = 0;

	if ( csRunPathName.IsEmpty() )
	{
		SetRunPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str() );
	}
}

LPCTSTR SVFileNameManagerClass::GetConfigurationPathName()
{
	return Instance().m_ConfigurationPathName.c_str();
}

LPCTSTR SVFileNameManagerClass::GetRunPathName()
{
	return Instance().m_RunPathName.c_str();
}

BOOL SVFileNameManagerClass::SetRunPathName(LPCTSTR szPathName)
{
	BOOL bOk = false;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( rFileMgr.m_svFileNameList.Lock() )
	{
		bOk = true;

		if (szPathName)
		{
			rFileMgr.m_RunPathName = szPathName;
		}
		else
		{
			rFileMgr.m_RunPathName.clear();
		}

		if ( !rFileMgr.m_RunPathName.empty() )
		{
			bOk = CreatePath( rFileMgr.m_RunPathName.c_str() );
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return bOk;
}

BOOL SVFileNameManagerClass::SetConfigurationPathName(LPCTSTR szPathName)
{
	BOOL bOk = false;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( rFileMgr.m_svFileNameList.Lock() )
	{
		bOk = true;

		if (szPathName)
		{
			rFileMgr.m_ConfigurationPathName = szPathName;
		}
		else
		{
			rFileMgr.m_ConfigurationPathName.clear();
		}

		if ( !rFileMgr.m_ConfigurationPathName.empty() )
		{
			bOk = CreatePath( rFileMgr.m_ConfigurationPathName.c_str() );
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return bOk;
}

BOOL SVFileNameManagerClass::FindItem(SVFileNameClass* svpFileNameClass)
{
	BOOL bOk = false;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( 0 < rFileMgr.m_svFileNameList.GetSize() )
	{
		BOOL bDone = false;

		rFileMgr.m_iCurrentItem = static_cast<int>(rFileMgr.m_svFileNameList.GetSize());
		
		for ( int i = 0; !bDone && i < rFileMgr.m_svFileNameList.GetSize(); i++ )
		{
			bDone = svpFileNameClass <= rFileMgr.m_svFileNameList[i];
			if ( bDone )
			{
				bOk = rFileMgr.m_svFileNameList[i] == svpFileNameClass;
				rFileMgr.m_iCurrentItem = i;
			}
		}
	}
	else
	{
		rFileMgr.m_iCurrentItem = 0;
	}

	return bOk;
}

BOOL SVFileNameManagerClass::AddItem(SVFileNameClass* svpFileName)
{
	BOOL bOk = false;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( rFileMgr.m_svFileNameList.Lock() )
	{
		bOk = FindItem( svpFileName );

		if ( !bOk )
		{
			try
			{
				rFileMgr.m_svFileNameList.InsertAt( rFileMgr.m_iCurrentItem, svpFileName );

				bOk = LoadItem( rFileMgr.m_svFileNameList[rFileMgr.m_iCurrentItem] );
			}
			catch (std::bad_alloc& e)
			{
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( E_UNEXPECTED, e.what(), SvStl::SourceFileParams(StdMessageParams), SvOi::Err_15044_FileNameManagerAdd );
			}
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return bOk;
}

BOOL SVFileNameManagerClass::RemoveItem(SVFileNameClass * svpFileName)
{
	BOOL bOk = false;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( rFileMgr.m_svFileNameList.Lock() )
	{
		bOk = FindItem( svpFileName );

		if ( bOk )
		{
			rFileMgr.m_svFileNameList.RemoveAt( rFileMgr.m_iCurrentItem );

			if ( 0 < rFileMgr.m_svFileNameList.GetSize() )
			{
				if ( rFileMgr.m_svFileNameList.GetSize() <= rFileMgr.m_iCurrentItem )
				{
					rFileMgr.m_iCurrentItem = static_cast<int>(rFileMgr.m_svFileNameList.GetSize()) - 1;
				}
			}
			else
			{
				rFileMgr.m_iCurrentItem = 0;
			}
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return bOk;
}

SVFileNameClass * SVFileNameManagerClass::GetItem()
{
	SVFileNameClass *pItem = nullptr;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( rFileMgr.m_svFileNameList.Lock() )
	{
		if ( rFileMgr.m_iCurrentItem < rFileMgr.m_svFileNameList.GetSize() )
		{
			pItem = rFileMgr.m_svFileNameList[rFileMgr.m_iCurrentItem];
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return pItem;
}

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

		assert( iStart > 0 );

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

BOOL SVFileNameManagerClass::LoadItem(SVFileNameClass* svFileName)
{
	BOOL bOk = true;

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

BOOL SVFileNameManagerClass::SaveItem(SVFileNameClass * svFileName)
{
	BOOL bOk = true;

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
	BOOL bOk = true;

	if ( !CString( svFileName->GetFileName() ).IsEmpty() )
	{
		bOk = svFileName->SetPathName( GetRunPathName() );
	}

	return bOk;
}

BOOL SVFileNameManagerClass::SaveItems()
{
	BOOL bOk = false;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( rFileMgr.m_svFileNameList.Lock() )
	{
		bOk = true;

		for ( int i= 0; i < rFileMgr.m_svFileNameList.GetSize(); i++ )
		{
			bOk = SaveItem( rFileMgr.m_svFileNameList[i] ) && bOk;
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return bOk;
}

LPCTSTR SVFileNameManagerClass::GetFileNameList()
{
	return Instance().m_svFileNameList.GetFileNameList();
}

BOOL SVFileNameManagerClass::RemoveUnusedFiles(BOOL bCleanConfigDir)
{
	BOOL bOk = true;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	SVFileNameManagerClass& rFileMgr = Instance();

	CString csFileMask = GetRunPathName();

	csFileMask += _T( "\\*.*" );

	hFind = FindFirstFile( csFileMask, &FindFileData );
	if ( hFind != INVALID_HANDLE_VALUE )
	{
		BOOL bRun = true;

		while ( bRun )
		{
			BOOL bFound = false;
			CString csFileName = FindFileData.cFileName;

			for ( int i = 0; ! bFound && i < rFileMgr.m_svFileNameList.GetSize(); i++ )
			{
				bFound = csFileName.CompareNoCase( rFileMgr.m_svFileNameList[i]->GetFileName() ) == 0;
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
				BOOL bRun = true;

				while ( bRun )
				{
					BOOL bFound = false;
					CString csFileName = FindFileData.cFileName;

					for ( int i = 0; ! bFound && i < rFileMgr.m_svFileNameList.GetSize(); i++ )
					{
						bFound = csFileName.CompareNoCase( rFileMgr.m_svFileNameList[i]->GetFileName() ) == 0;
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
