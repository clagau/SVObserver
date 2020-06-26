//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileSystemScanner
//* .File Name       : $Workfile:   SVFileSystemScanner.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:37:36  $
//******************************************************************************

#pragma region Includes
#include "SVFileInfo.h"
#pragma endregion Includes

template<typename Insertor>
HRESULT SVFileSystemScanner<Insertor>::ScanForFiles(LPCTSTR dirName, LPCTSTR filter, Insertor insertor, bool bScanSubDirs)
{
	HRESULT hr = S_FALSE;

	WIN32_FIND_DATA fileData;
	std::string fileSpec;
	std::string dirSpec = (nullptr != dirName) ? dirName : std::string();
	
	// check for trailing slash
	size_t pos = dirSpec.rfind(_T("\\"));
	if (pos == std::string::npos || pos != dirSpec.size() - 1)
	{
		dirSpec += _T("\\");
	}

	fileSpec = dirSpec;
	fileSpec += (nullptr != filter) ? filter : std::string();

	HANDLE fHandle = FindFirstFile(fileSpec.c_str(), &fileData);

	if (fHandle != INVALID_HANDLE_VALUE)
	{
		hr = S_OK;

		BOOL rc = true;
		do
		{
			if (!(fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				ULARGE_INTEGER val;
				SVFileInfo fileInfo;
				fileInfo.filename = dirSpec;
				fileInfo.filename += fileData.cFileName;
				val.HighPart = fileData.nFileSizeHigh;
				val.LowPart = fileData.nFileSizeLow;
				fileInfo.fileSize = val.QuadPart;
				fileInfo.creationDate = fileData.ftCreationTime;
				fileInfo.modifiedDate = fileData.ftLastWriteTime;

				insertor = fileInfo;
			}
			else if (bScanSubDirs)
			{
				// future - recurse into sub dir
			}
			rc = FindNextFile(fHandle, &fileData);
		} while (rc);
		FindClose(fHandle);
	}
	return hr;
}

template<typename Insertor>
HRESULT SVFileSystemScanner<Insertor>::ScanForDirectories(LPCTSTR dirName, LPCTSTR filter, Insertor insertor, bool bScanSubDirs)
{
	HRESULT hr = S_FALSE;

	WIN32_FIND_DATA fileData;
	std::string fileSpec;
	
	std::string dirSpec = (nullptr != dirName) ? dirName : std::string();
	// check for trailing slash
	size_t pos = dirSpec.rfind(_T("\\"));
	if (pos == std::string::npos || pos != dirSpec.size() - 1)
	{
		dirSpec += _T("\\");
	}

	fileSpec = dirSpec;
	fileSpec += (nullptr != filter) ? filter : std::string();

	HANDLE fHandle = FindFirstFile(fileSpec, &fileData);

	if (fHandle != INVALID_HANDLE_VALUE)
	{
		hr = S_OK;

		BOOL rc = true;
		do while (fHandle)
		{
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				ULARGE_INTEGER val;
				SVFileInfo fileInfo;
				fileInfo.filename = dirSpec;
				fileInfo.filename += fileData.cFileName;
				val.HighPart = fileData.nFileSizeHigh;
				val.LowPart = fileData.nFileSizeLow;
				fileInfo.fileSize = val.QuadPart;
				fileInfo.creationDate = fileData.ftCreationTime;
				fileInfo.modifiedDate = fileData.ftLastWriteTime;

				insertor = fileInfo;
			}
			rc = FindNextFile(fHandle, &fileData);
		} while (rc);
		FindClose(fHandle);
	}
	return hr;
}

