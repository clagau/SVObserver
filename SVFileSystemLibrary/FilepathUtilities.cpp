//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemLibrary
//* .File Name       : $Workfile:   FilepathUtilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:08  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "FilepathUtilities.h"
#include "Definitions/StringTypeDef.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"


#pragma endregion Includes

bool isFilepathOnRegularPartition(const std::string& rFilePath);
bool isFilepathOnNetwork(const std::string& rFilePath);

HRESULT CheckDrive(const std::string& rDrive)
{
	HRESULT l_hr = S_OK;
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
			std::string Drive = SvUl::MakeUpper(SvUl::Left(	rDrive, 1).c_str());

			SvStl::MessageManager Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_5052_DRIVENOTNTFSFORMAT, Drive.c_str(), SvStl::SourceFileParams(StdMessageParams));

#ifndef _DEBUG
			assert(false);
#else
#if defined (TRACE_THEM_ALL) || defined (TRACE_SVO)
			::OutputDebugString(Drive.c_str());
#endif
#endif
		}
	}

	return l_hr;
}




void KeepPrevError( HRESULT& p_rhrPrev, HRESULT p_hrNew )
{
	if( S_OK == p_rhrPrev )
	{
		p_rhrPrev = p_hrNew;
	}
}

// TStrPathName must be the full pathname of a file
// or a full pathname of a directory with a slash on the end!
// e.g.: c:/dir1/dir2/dir3/filename.ext
//       c:/dir1/dir2/dir3/
// strlen of TStrPathName must not exceed _MAX_PATH length!
// If the drive and the directory exists, the function returns
// true. 
// If the directory not exits and BCreateIfNotExists is true,
// the return value is true if the directory is creatible.
//
// In all other cases it returns false.
bool SVCheckPathDir( LPCTSTR PathName, bool CreateIfDoesNotExist )
{
	TCHAR  curPath[ _MAX_PATH ];
	// Save current settings...
	if( nullptr == _tgetcwd( curPath, _MAX_PATH )  )
	{
		curPath[0] = '\0';
	}
	// check path
	if( PathName && _tcslen( PathName ) < _MAX_PATH )
	{
		TCHAR* pLast = (TCHAR*)PathName;
		TCHAR  drive[_MAX_DRIVE];
		TCHAR  dir[_MAX_DIR];
		TCHAR  fname[_MAX_FNAME];
		TCHAR  ext[_MAX_EXT];
		_tsplitpath( pLast, drive, dir, fname, ext );
		if( _tcslen( drive ) == 0 && nullptr != _tcschr( pLast, _TCHAR( ':' ) ) )
		{
			// Restore settings...
			_tchdir( curPath );
			return false;
		}

		while(nullptr != (pLast = _tcspbrk( pLast, _T( "\\/" ))))
		{
			TCHAR  path[_MAX_PATH];
			_tcsncpy( path, PathName, pLast++ - PathName + 1 );
			path[ pLast - PathName ] = _TCHAR( '\0' );

			if( _tchdir( path ) != 0 )
			{
				// Check for creation...
				if( ! CreateIfDoesNotExist )
				{
					// Restore settings...
					_tchdir( curPath );
					return false;
				}

				// We have to make a new directory
				if( _tmkdir( path ) != 0 || _tchdir( path ) != 0 )
				{
					// Restore settings...
					_tchdir( curPath );
					return false;
				}
			}
		}

		// Restore settings...
		_tchdir( curPath );
		return true;
	}

	// Restore settings...
	_tchdir( curPath );
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
	if (isFilepathOnRegularPartition(rFilePath)|| isFilepathOnNetwork(rFilePath))
	{
		return true;
	}

	return false;
}


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



bool SVDeleteFiles( LPCTSTR PathName, bool IncludeSubDirectories )
{
	// TStrPathName must be the full pathname of a file,
	// that also includes * for the filename or the file extension!
	// e.g.: c:/dir1/dir2/dir3/filename.ext
	//       c:/dir1/dir2/*.*
	// strlen of TStrPathName must not exceed _MAX_PATH length!

	// If all deleting is done well, the function returns
	// true. 
	// In all other cases it returns false.

	if( PathName && _tcslen( PathName ) < _MAX_PATH )
	{
		TCHAR  drive[_MAX_DRIVE];
		TCHAR  dir[_MAX_DIR];
		TCHAR  fname[_MAX_FNAME];
		TCHAR  ext[_MAX_EXT];
		_tsplitpath( PathName, drive, dir, fname, ext );

		// Search for file(s)...
		WIN32_FIND_DATA findData;
		bool Result( true );
		HANDLE hFindFile = FindFirstFile( PathName, &findData );
		do
		{
			if( hFindFile )
			{
				TCHAR  path[_MAX_PATH];
				_tmakepath( path, drive, dir, findData.cFileName, _T( "" ) );
				if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
				{
					if( _tcscmp( findData.cFileName, _T( "." ) ) && _tcscmp( findData.cFileName, _T( ".." ) ) &&
						IncludeSubDirectories 
					  )
					{
						// Delete sub directory contents...
						_tcscat( path, _T( "\\*.*" ) );
						Result = SVDeleteFiles( path, IncludeSubDirectories ) && Result;

						// Prepare path for deleting sub directory...
						_tmakepath( path, drive, dir, findData.cFileName, _T( "" ) );
						// Check for current directory...
						TCHAR curDir[ _MAX_PATH ];
						_tgetcwd( curDir, _MAX_PATH );
						if( _tcscmp( curDir, path ) == 0 )
						{
							_tmakepath( curDir, drive, dir, _T( "" ), _T( "" ) );
							_tchdir( curDir );
						}
						// Delete sub directory...
						Result = _trmdir( path ) && Result;
					}
				}
				else
				{
					// Delete file...
					Result = DeleteFile( path ) && Result;
				}
			}
		} while( FindNextFile( hFindFile, &findData ) );

		FindClose( hFindFile );
		return Result;
	}

	return false;
}


std::string readContentFromFileAndDelete(const std::string& rFileName)
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
	::DeleteFile(rFileName.c_str());
	content.append(_T("\0"));

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
}

