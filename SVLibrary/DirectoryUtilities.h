// ******************************************************************************
// * COPYRIGHT (c) 2001 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : DirectoryUtilities.h
// * .File Name       : $Workfile:   DirectoryUtilities.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 18:10:12  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <errno.h>
//DirectoryUtilities.h moved from SVCmnLib to SVLibrary 

BOOL CreateDirPath( LPCTSTR Path);
bool CopyFilesInDirectory(LPCTSTR sourceDirectory, LPCTSTR destinationDirectory); ///< non-recursively copies all files in sourceDirectory to destinationDirectory

//************************************
/// moves (renames) one directory within its containing directory 
/// \param containingDirectoryPath [in] path of the containing directory 
/// \param sourceDirectory [in] old name of the directory to be moved 
/// \param destinationDirectory [in] new name of the directory to be moved 
//************************************
void moveContainedDirectory( LPCTSTR containingDirectoryPath, LPCTSTR sourceDirectory, LPCTSTR destinationDirectory );


//************************************
/// removes a directory and all its content
/// \param Path [in] path of the directory to be removed
//************************************
void deleteTree( LPCTSTR Path);

