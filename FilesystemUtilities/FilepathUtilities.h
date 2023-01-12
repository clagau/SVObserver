///\copyright 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file FilepathUtilities.h

#pragma once

#include "ImageFileFormats.h"

namespace SvFs
{
bool ensureDirectoryExists(LPCTSTR PathName, bool createIfMissing);
bool deleteFiles(LPCTSTR PathName, bool IncludeSubDirectories);
std::string readContentFromFile(const std::string& rFileName, bool deleteFileAfterUse = true);
void writeStringToFile(const std::string& rFileName, const std::string& rFileData, bool Text);

bool pathCanProbablyBeCreatedOrExistsAlready(const std::string& rFilePath);

HRESULT checkDrive(const std::string& p_strDrive);

std::vector<std::string> getFileList(LPCTSTR pPath, ImageFileFormat fileFormat, bool recursive);

bool CreateDirPath(LPCTSTR Path);
bool ensureDirectoryExistsVar(std::string& rDirectoryPath);
}

