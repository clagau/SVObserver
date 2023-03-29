///\copyright 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file FilepathUtilities.h

#pragma once

#include "ImageFileFormats.h"

namespace SvFs
{
bool ensureDirectoryExists(LPCTSTR PathName, bool createIfMissing);
bool deleteFiles(LPCTSTR PathName, bool IncludeSubDirectories);
std::string readContentFromFile(const std::string& rFileName, bool deleteFileAfterUse = true);
/// Write a string to a file. Throw exception if error happens.
/// \param rFileName [in] Filename
/// \param rFileData [in] The string to save.
/// \param Text [in] true is saved as text, false is saved binary.
void writeStringToFile(const std::string& rFileName, const std::string& rFileData, bool Text);

bool pathCanProbablyBeCreatedOrExistsAlready(const std::string& rFilePath);

HRESULT checkDrive(const std::string& p_strDrive);

std::vector<std::string> getFileList(LPCTSTR pPath, ImageFileFormat fileFormat, bool recursive);

///  Moved files to a destination path and if access for move denied it delete it. 
/// \param rFileList [in] List of files to move.
/// \param rDestinationPath [in] Destination path.
/// \param rIgnoreFilePart [in] Files which starts with this string will be ignored by moving/deleting.
void moveFilesToFolder(const std::vector<std::string>& rFileList, const std::string& rDestinationPath, const std::string& rIgnoreFilePart);

bool CreateDirPath(LPCTSTR Path);
bool ensureDirectoryExistsVar(std::string& rDirectoryPath);
}

