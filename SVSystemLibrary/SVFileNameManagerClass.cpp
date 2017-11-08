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
#include "SVLibrary/SVFileNameClass.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVStatusLibrary/ErrorNumbers.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

SVFileNameManagerClass& SVFileNameManagerClass::Instance()
{
	static SVFileNameManagerClass fileNameMgr;
	return fileNameMgr;
}

SVFileNameManagerClass::SVFileNameManagerClass()
{
	std::string csRunPathName = GetRunPathName();

	m_iCurrentItem = 0;

	if ( csRunPathName.empty() )
	{
		SetRunPathName( SvStl::GlobalPath::Inst().GetRunPath().c_str() );
	}
}

bool SVFileNameManagerClass::SetRunPathName(LPCTSTR PathName)
{
	bool Result( false );

	if ( m_svFileNameList.Lock() )
	{
		Result = true;

		if( nullptr != PathName)
		{
			m_RunPathName = PathName;
		}
		else
		{
			m_RunPathName.clear();
		}

		if ( !m_RunPathName.empty() )
		{
			Result = CreatePath( m_RunPathName.c_str() );
		}

		m_svFileNameList.Unlock();
	}

	return Result;
}

bool SVFileNameManagerClass::SetConfigurationPathName(LPCTSTR szPathName)
{
	bool Result( false );

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( rFileMgr.m_svFileNameList.Lock() )
	{
		Result = true;

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
			Result = CreatePath( rFileMgr.m_ConfigurationPathName.c_str() );
		}

		rFileMgr.m_svFileNameList.Unlock();
	}

	return Result;
}

bool SVFileNameManagerClass::FindItem(SVFileNameClass* svpFileNameClass)
{
	bool bOk = false;

	SVFileNameManagerClass& rFileMgr = Instance();

	if ( 0 < rFileMgr.m_svFileNameList.GetSize() )
	{
		bool bDone = false;

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

bool SVFileNameManagerClass::AddItem(SVFileNameClass* pFileName)
{
	bool Result( false );

	if ( m_svFileNameList.Lock() )
	{
		Result = FindItem( pFileName );

		if( !Result )
		{
			try
			{
				m_svFileNameList.InsertAt( m_iCurrentItem, pFileName );

				Result = LoadItem( m_svFileNameList[m_iCurrentItem] );
			}
			catch (std::bad_alloc& e)
			{
				SvStl::MessageMgrStd Exception( SvStl::LogOnly );
				Exception.setMessage( E_UNEXPECTED, e.what(), SvStl::SourceFileParams(StdMessageParams), SvStl::Err_15044_FileNameManagerAdd );
			}
		}

		m_svFileNameList.Unlock();
	}

	return Result;
}

bool SVFileNameManagerClass::RemoveItem(SVFileNameClass* pFileName)
{
	bool Result = false;

	if ( m_svFileNameList.Lock() )
	{
		Result = FindItem( pFileName );

		if ( Result )
		{
			m_svFileNameList.RemoveAt( m_iCurrentItem );

			if ( 0 < m_svFileNameList.GetSize() )
			{
				if ( m_svFileNameList.GetSize() <= m_iCurrentItem )
				{
					m_iCurrentItem = static_cast<int>(m_svFileNameList.GetSize()) - 1;
				}
			}
			else
			{
				m_iCurrentItem = 0;
			}
		}

		m_svFileNameList.Unlock();
	}

	return Result;
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

bool SVFileNameManagerClass::CreatePath(LPCTSTR PathName)
{
	std::string Path( PathName );

	bool Result = _access( Path.c_str(), 0 ) == 0;

	if ( !Result )
	{
		bool bDone = false;

		size_t Start=-1;
		if( SvUl::Left(Path, 2) == _T("\\\\"))	// UNC path
		{
			Start = SvUl::Mid(Path, 2).find('\\') + 1;
		}
		else	// drive letter
		{
			Start = Path.find(':');
		}

		assert( Start > 0 );

		do
		{
			bDone = Path.size() <= Start + 1;

			if (!bDone)
			{
				if ( Path[Start + 1] == '\\' )
				{
					Start++;

					bDone = Path.size() <= Start + 1;

					if ( !bDone )
					{
						bDone = Path[Start + 1] == '\\';
						Result = !bDone;
					}
				}

				if ( !bDone )
				{
					size_t NextBS = Path.find( '\\', Start + 1 );
					size_t NextFS = Path.find( '/', Start + 1 );

					std::string MakePath;

					if ( std::string::npos == NextBS && std::string::npos == NextFS )
					{
						MakePath = Path;
					}
					else
					{
						if ( std::string::npos == NextFS || std::string::npos != NextBS && NextBS < NextFS )
						{
							MakePath = SvUl::Left( Path,  NextBS );
						}
						else
						{
							MakePath = SvUl::Left( Path, NextFS );
						}
					}

					Result = _access( MakePath.c_str(), 0 ) == 0;

					if ( !Result )
					{
						Result = _mkdir( MakePath.c_str() ) == 0;
					}

					Start = MakePath.size();
				}
			}
		}
		while ( Result && !bDone );
	}

	return Result;
}

bool SVFileNameManagerClass::LoadItem(SVFileNameClass* pFileName)
{
	bool Result = true;

	if ( !std::string( pFileName->GetFileName() ).empty() )
	{
		std::string FullName = pFileName->GetFullFileName();
		std::string ConfigFullName = pFileName->GetFullFileName();
		std::string RunFullName = std::string( GetRunPathName() ) + _T("\\") + pFileName->GetFileName();
		 
		if ( !GetConfigurationPathName().empty() )
		{
			ConfigFullName = GetConfigurationPathName() + _T("\\") + pFileName->GetFileName();
		}

		SvUl::searchAndReplace( RunFullName, _T("/"), _T("\\") );
		SvUl::searchAndReplace( ConfigFullName, _T("/"), _T("\\") );
		SvUl::searchAndReplace( FullName,  _T("/"), _T("\\") );

		Result = SvUl::CompareNoCase( RunFullName, FullName ) == 0 && _access( RunFullName.c_str(), 0 ) == 0;

		if ( !Result )
		{
			if ( 0 != SvUl::CompareNoCase( ConfigFullName, FullName ) )
			{
				if ( _access( FullName.c_str(), 0 ) == 0 )
				{
					Result = CopyFile( FullName.c_str(), RunFullName.c_str(), false ) ? true : false;
					_chmod( RunFullName.c_str(), _S_IREAD | _S_IWRITE);
				}
			}

			if ( !Result )
			{
				if ( _access( ConfigFullName.c_str(), 0 ) == 0 )
				{
					Result = CopyFile( ConfigFullName.c_str(), RunFullName.c_str(), false ) ? true : false;
					_chmod( RunFullName.c_str(), _S_IREAD | _S_IWRITE);
				}
			}

			if ( !Result )
			{
				Result = _access( RunFullName.c_str(), 0 ) == 0;
			}

			if ( Result )
			{
				pFileName->SetPathName( GetRunPathName().c_str() );
			}
		}
	}

	return Result;
}

bool SVFileNameManagerClass::SaveItem( SVFileNameClass* pFileName )
{
	bool Result( true );

	if ( !std::string( pFileName->GetFileName() ).empty() )
	{
		Result = CopyFileToPath( GetRunPathName(), pFileName );
		if ( Result )
		{
			pFileName->SetPathName( GetRunPathName().c_str() );
		}

		if ( Result )
		{
			Result = CopyFileToPath( GetConfigurationPathName(), pFileName );
		}
	}

	return Result;
}

bool SVFileNameManagerClass::RenameItem(SVFileNameClass* pFileName)
{
	if ( !std::string( pFileName->GetFileName() ).empty() )
	{
		pFileName->SetPathName( GetRunPathName().c_str() );
	}

	return true;
}

bool SVFileNameManagerClass::SaveItems()
{
	bool Result = false;

	if ( m_svFileNameList.Lock() )
	{
		Result = true;

		for ( int i= 0; i < m_svFileNameList.GetSize(); i++ )
		{
			Result = SaveItem( m_svFileNameList[i] ) && Result;
		}

		m_svFileNameList.Unlock();
	}

	return Result;
}

LPCTSTR SVFileNameManagerClass::GetFileNameList()
{
	return m_svFileNameList.GetFileNameList();
}

bool SVFileNameManagerClass::RemoveUnusedFiles(bool bCleanConfigDir)
{
	bool Result( true );

	Result = RemoveUnusedFiles( GetRunPathName() );
	if ( bCleanConfigDir && Result )
	{
		Result = RemoveUnusedFiles( GetConfigurationPathName() );

	}

	return Result;
}

bool SVFileNameManagerClass::RemoveUnusedFiles( const std::string& rPath )
{
	bool Result( true );

	if ( !rPath.empty() )
	{
		std::string FileMask = rPath + _T( "\\*.*" );

		WIN32_FIND_DATA FindFileData;
		HANDLE hFind;

		hFind = FindFirstFile( FileMask.c_str(), &FindFileData );
		if ( hFind != INVALID_HANDLE_VALUE )
		{
			BOOL bRun = true;

			while ( bRun )
			{
				bool bFound = false;
				std::string FileName = FindFileData.cFileName;

				for ( int i = 0; ! bFound && i < m_svFileNameList.GetSize(); i++ )
				{
					bFound = (0 ==SvUl::CompareNoCase( FileName, m_svFileNameList[i]->GetFileName() ));
				}

				if ( ! bFound )
				{
					std::string FullFileName = rPath;

					FullFileName += _T( "\\" );
					FullFileName += FileName;

					_chmod(FullFileName.c_str(),_S_IREAD | _S_IWRITE);
					Result = DeleteFile( FullFileName.c_str() ) ? true : false;
				}

				bRun = FindNextFile( hFind, &FindFileData );
			}

			FindClose( hFind );
		}
	}
	
	return Result;
}

bool SVFileNameManagerClass::CopyFileToPath( const std::string& rPath, SVFileNameClass* pFileName ) const
{
	bool Result( true );
	
	if( !rPath.empty() )
	{
		std::string RunFullName = rPath + _T("\\") + pFileName->GetFileName();

		SvUl::searchAndReplace( RunFullName, _T("/"), _T("\\") );

		if ( SvUl::CompareNoCase( RunFullName, pFileName->GetFullFileName() ) )
		{
			Result = _access( pFileName->GetFullFileName().c_str(), 0 ) != 0;

			if( !Result )
			{
				Result = CopyFile( pFileName->GetFullFileName().c_str(), RunFullName.c_str(), false ) ? true : false;
				_chmod( RunFullName.c_str(), _S_IREAD | _S_IWRITE );
			}
		}
	}

	return Result;
}