// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Utilities.cpp
// * .File Name       : $Workfile:   Utilities.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
// ******************************************************************************
//DirectoryUtilities.h moved from SVCmnLib to SVLibrary 

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <direct.h>
//Moved to precompiled header: #include <tchar.h>
#include "DirectoryUtilities.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

bool CreateDirPath( LPCTSTR Path)
{
	int nBackCount = 0;
	size_t Pos = 0;

	if( nullptr == Path )
	{ 
		return false;
	}

	//Check if Path already exists
	if( 0 == _access( Path, 0 ) )
	{
		return true;
	}

	std::string PathToCreate( Path );
	//find the number of backslashes
	while( std::string::npos != (Pos = PathToCreate.find( _T("\\"), Pos )) )
	{
		Pos++;
		nBackCount++;
	}
	
	if(nBackCount > 1)
	{
		//check for all the directories, create dirs if they don't exist
		Pos = 0;
		for( int nIndex=0; nIndex < nBackCount; nIndex++ )
		{
			Pos = PathToCreate.find( _T("\\"), Pos );
			if(nIndex==0) 
			{
				Pos++;
				continue;
			}
			std::string Temp = SvUl::Left( PathToCreate, Pos );
			if( _tmkdir( Temp.c_str() ) )
			{
				if(errno != EEXIST) return false;
			}
			Pos++;
		}
	}
	else if( nBackCount == 1 )
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}

bool CopyFilesInDirectory(LPCTSTR sourceDirectory, LPCTSTR destinationDirectory)
{
	CreateDirPath(destinationDirectory);

	std::string SearchPattern = sourceDirectory;
	SearchPattern += _T("\\*.*");

	CFileFind		Finder;
	BOOL bWorking = Finder.FindFile ( SearchPattern.c_str() );

	while ( bWorking )
	{
		bWorking = Finder.FindNextFile ( );

		// copy only files, not directories ...
		if ( !Finder.IsDirectory ( ) )
		{
			std::string SourceFilepath = Finder.GetFilePath();
			std::string DestinationFilepath = destinationDirectory + std::string( _T("\\") ) + Finder.GetFileName().GetString();
			if( !CopyFile( SourceFilepath.c_str(),DestinationFilepath.c_str(), true ) )
			{
				return false;
			}
		}
	}

	return true;
}


std::vector<std::string> findFiles(LPCTSTR folder)
{
	std::vector<std::string> fileList;

	std::string searchFolder{folder};
	size_t Size = searchFolder.size();
	if(0 < Size && '\\' != searchFolder[Size-1])
	{
		searchFolder += '\\';
	}
	searchFolder += _T("*.*");

	CFileFind		Finder;
	BOOL bFileFound = Finder.FindFile(searchFolder.c_str());

	while (bFileFound)
	{
		bFileFound = Finder.FindNextFile();

		if (!Finder.IsDirectory())
		{
			fileList.emplace_back(Finder.GetFilePath());
		}
	}

	return fileList;
}


void deleteTree( LPCTSTR Path )
{
	//Arvid: this solution was taken (and modified somewhat) from here: http://www.codeproject.com/Articles/1862/Delete-folders-subfolders-and-files-easily
	CFileFind ff;
	std::string searchpath( Path );

	if( searchpath[searchpath.size()-1] != '\\' )
	{
		searchpath += _T("\\");
	}

	searchpath += _T("*.*");

	BOOL res = ff.FindFile( searchpath.c_str() );

	while(res)
	{
		res = ff.FindNextFile();
		if (!ff.IsDots() && !ff.IsDirectory())
		{
			DeleteFile(ff.GetFilePath());
		}
		else if (ff.IsDirectory())
		{
			if(!ff.IsDots()) //Arvid: do not remove the "." and ".." directories
			{
				deleteTree(ff.GetFilePath());
			}
		}
	}
	RemoveDirectory(Path);
}





