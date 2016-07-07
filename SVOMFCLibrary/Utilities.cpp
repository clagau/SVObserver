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
//Utilities.h moved from SVCmnLib to SVOMFCLibrary 

#include "stdafx.h"
//Moved to precompiled header: #include <direct.h>
//Moved to precompiled header: #include <tchar.h>
#include "utilities.h"


BOOL CreateDirPath(CString szPath)
{
	int nBackCount = 0;
	int nBackIndex = 0;
	int nIndex;
	CString szTemp;

	if(szPath.IsEmpty())return FALSE;

	//find the number of backslashes
	while(nBackIndex = szPath.Find(_T("\\"),nBackIndex))
		{
		if(nBackIndex == -1)break;
		nBackIndex++;
		nBackCount++;
		}
	
	if(nBackCount > 1)
		{
		//check for all the directories, create dirs if they don't exist
		nBackIndex=0;
		for(nIndex=0; nIndex<nBackCount; nIndex++)
			{
			nBackIndex = szPath.Find(_T("\\"),nBackIndex);				
			nBackIndex++;
			if(nIndex==0)continue;
			szTemp = szPath.Left(nBackIndex-1);
			if(_tmkdir(szTemp.GetBuffer(szTemp.GetLength()))) 
				{
				if(errno != EEXIST)return FALSE;
				}
			}
		}
	else if (nBackCount == 1)
		{
		return TRUE;
		}
	else
		{
		return FALSE;
		}
	return TRUE;
}

HRESULT GetSystemErrorText(DWORD dwError, CString & szMsg)
{
	LPVOID lpMsgBuf;
	if(FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		nullptr 
	) == 0)return S_FALSE;
	szMsg.Format(_T("%s"),lpMsgBuf);
	LocalFree(lpMsgBuf);
	return S_OK;
}

DWORD GetLastSystemErrorText(CString & szMsg)
{
	DWORD dwError = GetLastError();
	GetSystemErrorText(dwError, szMsg);
	return dwError;
}

BOOL CopyDir(LPCTSTR szOrigPath, LPCTSTR szNewPath)
{

	CString			csFileToFind;
	CString			csNewFile;
	CString			csFile;
	CFileFind		cffFinder;
	BOOL			blReturn = TRUE;

	//create the temp folder
	CreateDirPath(szNewPath);

	csFileToFind	= szOrigPath;
	csFileToFind += _T("*.*");

	BOOL bWorking = cffFinder.FindFile ( csFileToFind );

	while ( bWorking )
		{
		bWorking = cffFinder.FindNextFile ( );

		// If file is directory ...
		if ( !cffFinder.IsDirectory ( ) )
			{
			csFile = (LPCTSTR) cffFinder.GetFilePath ( );
			csNewFile = szNewPath + cffFinder.GetFileName();
			if(!CopyFile(csFile,csNewFile,TRUE))
				{
				blReturn = FALSE;
				break;
				}
			}
		}

	return blReturn;
}


void moveContainedDirectory(const CString &containingDirectoryPath,const CString &sourceDirectory,const CString &destinationDirectory)
{
	CString sourcePath=containingDirectoryPath+"\\"+sourceDirectory;
	CString destinationPath=containingDirectoryPath+"\\"+destinationDirectory;

	deleteTree(destinationPath);

	MoveFile(sourcePath,destinationPath); //Arvid: this should work for directories: https://msdn.microsoft.com/en-us/library/aa365239(VS.85).aspx
}


void deleteTree(const CString &originalPath)
{
	//Arvid: this solution was taken (and modified somewhat) from here: http://www.codeproject.com/Articles/1862/Delete-folders-subfolders-and-files-easily
	CFileFind ff;
	CString searchpath = originalPath;

	if(searchpath.Right(1) != "\\")
		searchpath += "\\";

	searchpath += "*.*";

	BOOL res = ff.FindFile(searchpath);

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
	RemoveDirectory(originalPath);//Arvid: the directory path is removed only when empty (which should be the case at this stage)
}





