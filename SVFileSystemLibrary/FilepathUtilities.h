//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemLibrary
//* .File Name       : $Workfile:   FilepathUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:26  $
//******************************************************************************

#pragma once

bool SVCheckPathDir(LPCTSTR PathName, bool CreateIfDoesNotExist );
bool SVDeleteFiles(LPCTSTR PathName, bool IncludeSubDirectories );

bool pathCanProbablyBeCreatedOrExistsAlready(const std::string& rFilePath); //
bool isFilepathOnRegularPartition(const std::string& rFilePath);
bool isFilepathOnNetwork(const std::string& rFilePath);

