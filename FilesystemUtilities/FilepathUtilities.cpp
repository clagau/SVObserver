///\copyright 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
///\file FilepathUtilities.cpp

#pragma once

#include "stdafx.h"
#include "FilepathUtilities.h"
#include "Definitions/StringTypeDef.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVStatusLibrary/MessageManagerHelper.h"

namespace
{
	bool isFilepathOnRegularPartition(const std::string& rFilePath)
	{
		size_t pos = 0;
		std::string delimiter = ":";

		pos = rFilePath.find(delimiter);

		if (1 == pos) //if we find one colon at the right position ...
		{
			if (std::string::npos == rFilePath.find(delimiter, 2)) // ... but, of course, not more than one ...
			{
				return true; // ... we consider this to be a valid path!
			}
		}

		// But otherwise, we do not:
		return false;
	}


	bool isFilepathOnNetwork(const std::string& rFilePath)
	{
		size_t pos = 0;
		std::string delimiter = "\\";

		pos = rFilePath.find(delimiter);

		if (0 == pos)
		{
			auto nextBackslashPosition = rFilePath.find("\\", pos + 2);
			if ((nextBackslashPosition > 3) &&
				(rFilePath.size() > nextBackslashPosition))
			{
				return true;
			}

			auto nextSlashPosition = rFilePath.find("/", pos + 2);
			if ((nextSlashPosition > 3) &&
				(rFilePath.size() > nextSlashPosition))
			{
				return true;
			}
		}

		return false;
	}

}

namespace SvFs
{
HRESULT checkDrive(const std::string& rDrive)
{
	HRESULT hr = S_OK;//@TODO [Arvid][10.30][11.11.2022] this is always S_OK regardless of problems. Why?
	// Check if exists
	if (!PathFileExists(rDrive.c_str()))
	{
		std::string Drive = SvUl::MakeUpper(SvUl::Left(rDrive, 1).c_str());

		SvDef::StringVector msgList;
		msgList.push_back(Drive);

		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_5051_DRIVEDOESNOTEXIST, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
	}
	TCHAR VolumeName[100];
	TCHAR FileSystemName[32];
	DWORD dwSerialNumber;
	DWORD dwMaxFileNameLength;
	DWORD dwFileSystemFlags;

	if (GetVolumeInformation(rDrive.c_str(),
		VolumeName,
		sizeof(VolumeName),
		&dwSerialNumber,
		&dwMaxFileNameLength,
		&dwFileSystemFlags,
		FileSystemName,
		sizeof(FileSystemName)))
	{
		std::string Name(FileSystemName);
		if (std::string::npos == Name.find(_T("NTFS")))
		{
			std::string Drive = SvUl::MakeUpper(SvUl::Left(rDrive, 1).c_str());

			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_5052_DRIVENOTNTFSFORMAT, Drive.c_str(), SvStl::SourceFileParams(StdMessageParams));

#ifndef _DEBUG
			Log_Assert(false);
#else
#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
			::OutputDebugString(Drive.c_str());
#endif
#endif
		}
	}

	return hr;
}


// PathName must be the full pathname of a file
// or a full pathname of a directory with a slash on the end!
// e.g.: c:/dir1/dir2/dir3/filename.ext
//       c:/dir1/dir2/dir3/
// strlen of PathName must not exceed _MAX_PATH length!
// If the drive and the directory exists, the function returns
// true. 
// If the directory not exits and createIfMissing is true,
// the return value is true if the directory is creatable.
//
// In all other cases it returns false.
bool ensureDirectoryExists(LPCTSTR PathName, bool createIfMissing)
{
	TCHAR  curPath[_MAX_PATH];
	// Save current settings...
	if (nullptr == _tgetcwd(curPath, _MAX_PATH))
	{
		curPath[0] = '\0';
	}
	// check path
	if (PathName && _tcslen(PathName) < _MAX_PATH)
	{
		TCHAR* pLast = const_cast<TCHAR*>  (PathName);
		TCHAR  drive[_MAX_DRIVE];
		TCHAR  dir[_MAX_DIR];
		TCHAR  fname[_MAX_FNAME];
		TCHAR  ext[_MAX_EXT];
		_tsplitpath(pLast, drive, dir, fname, ext);
		if (_tcslen(drive) == 0 && nullptr != _tcschr(pLast, _TCHAR(':')))
		{
			// Restore settings...
			_tchdir(curPath);
			return false;
		}

		while (nullptr != (pLast = _tcspbrk(pLast, _T("\\/"))))
		{
			TCHAR  path[_MAX_PATH];
			_tcsncpy(path, PathName, pLast++ - PathName + 1);
			path[pLast - PathName] = _TCHAR('\0');

			if (_tchdir(path) != 0)
			{
				// Check for creation...
				if (!createIfMissing)
				{
					// Restore settings...
					_tchdir(curPath);
					return false;
				}

				// We have to make a new directory
				if (_tmkdir(path) != 0 || _tchdir(path) != 0)
				{
					// Restore settings...
					_tchdir(curPath);
					return false;
				}
			}
		}

		// Restore settings...
		_tchdir(curPath);
		return true;
	}

	// Restore settings...
	_tchdir(curPath);
	return false;
}

bool pathCanProbablyBeCreatedOrExistsAlready(const std::string& rFilePath)
{
	//unfortunately, std::filesystem::exists() currently seems to have a serious problem with network paths 
	//that don't exist so we use _access() instead.

	if (0 == _access(rFilePath.c_str(), 0))
	{
		return true; // rFilePath exists already
	}

	//taken from: https ://docs.microsoft.com/de-de/windows/win32/api/shlobj_core/nf-shlobj_core-pathcleanupspec?redirectedfrom=MSDN
	constexpr auto c_illegalCharacters = R"(*?"<>|')";

	if (std::string::npos != rFilePath.find_first_of(c_illegalCharacters))
	{
		return false;
	}

	//since it is very hard to reliably determine whether a directory path can be created:
	//we just do some basic checks

	std::filesystem::path filepath {rFilePath};
	auto parentPath = filepath.parent_path();

	if (exists(parentPath) && !is_directory(parentPath))
	{
		// if there already is something at the location of the parent directory it must be a directory - otherwise the path cannot be created!

		return false;
	}

	if (isFilepathOnRegularPartition(rFilePath) || isFilepathOnNetwork(rFilePath))
	{
		return true;
	}

	return false;
}


bool deleteFiles(LPCTSTR PathName, bool IncludeSubDirectories)
{
	// TStrPathName must be the full pathname of a file,
	// that also includes * for the filename or the file extension!
	// e.g.: c:/dir1/dir2/dir3/filename.ext
	//       c:/dir1/dir2/*.*
	// strlen of TStrPathName must not exceed _MAX_PATH length!

	// If all deleting is done well, the function returns
	// true. 
	// In all other cases it returns false.

	if (PathName && _tcslen(PathName) < _MAX_PATH)
	{
		TCHAR  drive[_MAX_DRIVE];
		TCHAR  dir[_MAX_DIR];
		TCHAR  fname[_MAX_FNAME];
		TCHAR  ext[_MAX_EXT];
		_tsplitpath(PathName, drive, dir, fname, ext);

		// Search for file(s)...
		WIN32_FIND_DATA findData;
		bool Result(true);
		HANDLE hFindFile = FindFirstFile(PathName, &findData);
		if (hFindFile)
		{
			do
			{
				TCHAR  path[_MAX_PATH];
				_tmakepath(path, drive, dir, findData.cFileName, _T(""));
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					if (_tcscmp(findData.cFileName, _T(".")) && _tcscmp(findData.cFileName, _T("..")) &&
						IncludeSubDirectories
					  )
					{
						// Delete sub directory contents...
						_tcscat(path, _T("\\*.*"));
						Result = deleteFiles(path, IncludeSubDirectories) && Result;

						// Prepare path for deleting sub directory...
						_tmakepath(path, drive, dir, findData.cFileName, _T(""));
						// Check for current directory...
						TCHAR curDir[_MAX_PATH];
						_tgetcwd(curDir, _MAX_PATH);
						if (_tcscmp(curDir, path) == 0)
						{
							_tmakepath(curDir, drive, dir, _T(""), _T(""));
							_tchdir(curDir);
						}
						// Delete sub directory...
						Result = _trmdir(path) && Result;
					}
				}
				else
				{
					// Delete file...
					Result = DeleteFile(path) && Result;
				}
			} while (FindNextFile(hFindFile, &findData));
			FindClose(hFindFile);
		}

		return Result;
	}

	return false;
}


std::string readContentFromFile(const std::string& rFileName, bool deleteFileAfterUse)
{
	std::string content;
	std::ifstream FileStream;

	FileStream.open(rFileName.c_str(), std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
	if (FileStream.is_open())
	{
		size_t FileSize(0);
		FileSize = static_cast<size_t> (FileStream.tellg());
		content.resize(FileSize);
		FileStream.seekg(0, std::ios::beg);
		FileStream.read(&content.at(0), FileSize);
		FileStream.close();
	}
	content.append(_T("\0"));

	if (deleteFileAfterUse)
	{
		::DeleteFile(rFileName.c_str());
	}

	return content;
}


void writeStringToFile(const std::string& rFileName, const std::string& rFileData, bool Text)
{
	std::ofstream FileStream;

	if (Text)
	{
		FileStream.open(rFileName.c_str());
	}
	else
	{
		FileStream.open(rFileName.c_str(), std::ofstream::binary);
	}
	if (FileStream.is_open())
	{
		FileStream.write(&rFileData.at(0), rFileData.size());
		FileStream.close();
	}
	else
	{
		SvStl::MessageManager e(SvStl::MsgType::Data);
		e.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_WriteFileFailed, {rFileName}, SvStl::SourceFileParams(StdMessageParams));
		e.Throw();
	}
}

std::vector<std::string> getFileList(LPCTSTR pPath, ImageFileFormat fileFormat, bool recursive)
{
	std::vector<std::string> result;

	if (std::filesystem::is_directory(pPath))
	{
		try
		{
			for (const auto& rExtension : allImageFileNameExtensions(fileFormat))
			{
				if (nullptr != pPath && strlen(pPath) > 0)
				{
					std::vector<std::filesystem::directory_entry>  filteredList {};

					if (recursive)
					{
						std::filesystem::recursive_directory_iterator  dirList {pPath};
						if (fileFormat == ImageFileFormat::invalid)
						{
							std::copy(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList));
						}
						else
						{
							std::copy_if(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList), [rExtension](const auto& rEntry)
							{
								return (rEntry.is_regular_file() && SvUl::MakeLower(rEntry.path().extension().string().c_str()) == rExtension);
							});
						}
					}
					else
					{
						std::filesystem::directory_iterator  dirList {pPath};

						if (fileFormat == ImageFileFormat::invalid)
						{
							std::copy(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList));
						}
						else
						{
							std::copy_if(std::filesystem::begin(dirList), std::filesystem::end(dirList), std::back_inserter(filteredList), [rExtension](const auto& rEntry)
							{
								return (rEntry.is_regular_file() && SvUl::MakeLower(rEntry.path().extension().string().c_str()) == rExtension);
							});
						}
					}
					result.reserve(filteredList.size());
					for (const auto& rEntry : filteredList)
					{
						// cppcheck-suppress useStlAlgorithm
						result.push_back(rEntry.path().string());
					}
					//StrCmpLogicalW is the sorting function used by Windows Explorer
					auto FolderCompare = [](const std::string& rLhs, const std::string& rRhs) { return ::StrCmpLogicalW(_bstr_t {rLhs.c_str()}, _bstr_t {rRhs.c_str()}) < 0; };
					std::sort(result.begin(), result.end(), FolderCompare);
				}
			}
		}
		catch (std::exception& e)
		{
			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			SvDef::StringVector msgList;
			msgList.push_back(e.what());
			Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_Default, msgList, SvStl::SourceFileParams(StdMessageParams));
			Log_Assert(false);
		}
	}

	return result;
}

void moveFilesToFolder(const std::vector<std::string>& rFileList, const std::string& rDestinationPath, const std::string& rIgnoreFilePart)
{
	for (const auto& rEntry : rFileList)
	{
		if (false == rEntry.starts_with(rIgnoreFilePart))
		{
			_TCHAR Name[_MAX_FNAME];
			_TCHAR Extension[_MAX_EXT];
			_splitpath(rEntry.c_str(), nullptr, nullptr, Name, Extension);

			std::string DestinationFile {rDestinationPath + _T("\\") + Name += Extension};
			if (0 == ::_access_s(DestinationFile.c_str(), 0))
			{
				::DeleteFile(rEntry.c_str());
			}
			else
			{
				::MoveFile(rEntry.c_str(), DestinationFile.c_str());
			}
		}
	}
}


bool CreateDirPath(LPCTSTR Path)
{
	int nBackCount = 0;
	size_t Pos = 0;

	if (nullptr == Path)
	{
		return false;
	}

	//Check if Path already exists
	if (0 == _access(Path, 0))
	{
		return true;
	}

	std::string PathToCreate(Path);
	//find the number of backslashes
	while (std::string::npos != (Pos = PathToCreate.find(_T("\\"), Pos)))
	{
		Pos++;
		nBackCount++;
	}

	if (nBackCount > 1)
	{
		//check for all the directories, create dirs if they don't exist
		Pos = 0;
		for (int nIndex = 0; nIndex < nBackCount; nIndex++)
		{
			Pos = PathToCreate.find(_T("\\"), Pos);
			if (nIndex == 0)
			{
				Pos++;
				continue;
			}
			std::string Temp = SvUl::Left(PathToCreate, Pos);
			if (_tmkdir(Temp.c_str()))
			{
				if (errno != EEXIST) return false;
			}
			Pos++;
		}
	}
	else if (nBackCount == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
	return true;
}


bool ensureDirectoryExistsVar(std::string & rDirectoryPath)
//@TODO [Arvid][10.30][11.11.2022]: this function works similarly to CreateDirPath() and ensureDirectoryExists()
//probably just one of them is needed
{
	try
	{
		if (std::filesystem::is_directory(rDirectoryPath))
		{
			return true;
		}

		return std::filesystem::create_directories(rDirectoryPath);
	}
	catch (std::exception& e)
	{
		SvDef::StringVector msgList;
		msgList.push_back(e.what());
		SvStl::MessageManager Exception(SvStl::MsgType::Log | SvStl::MsgType::Display);
		Exception.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidPath, msgList, SvStl::SourceFileParams(StdMessageParams));
		return false;
	}
}

}