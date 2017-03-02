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
#include "SVUtilityLibrary/SVString.h"
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

	SVString PathToCreate( Path );
	//find the number of backslashes
	while( SVString::npos != (Pos = PathToCreate.find( _T("\\"), Pos )) )
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
			SVString Temp = SvUl_SF::Left( PathToCreate, Pos );
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

	SVString SearchPattern = sourceDirectory;
	SearchPattern += _T("\\*.*");

	CFileFind		Finder;
	BOOL bWorking = Finder.FindFile ( SearchPattern.c_str() );

	while ( bWorking )
	{
		bWorking = Finder.FindNextFile ( );

		// copy only files, not directories ...
		if ( !Finder.IsDirectory ( ) )
		{
			SVString SourceFilepath = Finder.GetFilePath();
			SVString DestinationFilepath = destinationDirectory + SVString( _T("\\") ) + Finder.GetFileName().GetString();
			if( !CopyFile( SourceFilepath.c_str(),DestinationFilepath.c_str(), true ) )
			{
				return false;
			}
		}
	}

	return true;
}


void moveContainedDirectory( LPCTSTR containingDirectoryPath, LPCTSTR sourceDirectory, LPCTSTR destinationDirectory)
{
	SVString sourcePath = containingDirectoryPath + SVString( _T("\\") ) + sourceDirectory;
	SVString destinationPath=containingDirectoryPath + SVString( _T("\\") ) +destinationDirectory;

	deleteTree( destinationPath.c_str() );

	MoveFile( sourcePath.c_str(), destinationPath.c_str() ); //Arvid: this should work for directories: https://msdn.microsoft.com/en-us/library/aa365239(VS.85).aspx
}


void deleteTree( LPCTSTR Path )
{
	//Arvid: this solution was taken (and modified somewhat) from here: http://www.codeproject.com/Articles/1862/Delete-folders-subfolders-and-files-easily
	CFileFind ff;
	SVString searchpath( Path );

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





