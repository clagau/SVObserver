// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : Utilities.h
// * .File Name       : $Workfile:   Utilities.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <errno.h>
//Utilities.h moved from SVCmnLib to SVOMFCLibrary 

BOOL CreateDirPath(CString szPath);
HRESULT GetSystemErrorText(DWORD dwError, CString & szMsg); ///< also in SVDriveInitDlg.cpp
DWORD GetLastSystemErrorText(CString & szMsg); ///< also in SVDriveInitDlg.cpp
bool CopyFilesInDirectory(LPCTSTR sourceDirectory, LPCTSTR destinationDirectory); ///< non-recursively copies all files in sourceDirectory to destinationDirectory

//************************************
/// moves (renames) one directory within its containing directory 
/// \param containingDirectoryPath [in] path of the containing directory 
/// \param sourceDirectory [in] old name of the directory to be moved 
/// \param destinationDirectory [in] new name of the directory to be moved 
//************************************
void moveContainedDirectory(const CString &containingDirectoryPath,const CString &sourceDirectory,const CString &destinationDirectory);


//************************************
/// removes a directory and all its content
/// \param path [in] path of the directory to be removed
//************************************
void deleteTree(const CString &path);

