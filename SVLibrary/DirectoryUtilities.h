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

//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <string>

bool CreateDirPath( LPCTSTR Path);
bool CopyFilesInDirectory(LPCTSTR sourceDirectory, LPCTSTR destinationDirectory); ///< non-recursively copies all files in sourceDirectory to destinationDirectory

/// Finds the files in a certain folder
/// \param folder [in] The folder to search in
/// \returns List of file names
std::vector<std::string> findFiles(LPCTSTR folder);


/// removes a directory and all its content
/// \param Path [in] path of the directory to be removed
void deleteTree( LPCTSTR Path);

