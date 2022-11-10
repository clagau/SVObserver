//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemLibrary
//* .File Name       : $Workfile:   FilepathUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:26  $
//******************************************************************************

#pragma once

#include "SVFileSystemLibrary/ImageFileFormats.h"

bool SVCheckPathDir(LPCTSTR PathName, bool CreateIfDoesNotExist );
bool SVDeleteFiles(LPCTSTR PathName, bool IncludeSubDirectories );
std::string readContentFromFileAndDelete(const std::string& rFileName);
void writeStringToFile(const std::string& rFileName, const std::string& rFileData, bool Text);

bool pathCanProbablyBeCreatedOrExistsAlready(const std::string& rFilePath);

HRESULT CheckDrive(const std::string& p_strDrive);

std::vector<std::string> getFileList(LPCTSTR pPath, ImageFileFormat fileFormat, bool recursive);


