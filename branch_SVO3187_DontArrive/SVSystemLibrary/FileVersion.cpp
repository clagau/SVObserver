//*****************************************************************************
// \copyright COPYRIGHT (c) 2020/07/22,2020/07/22 by Körber Pharma Inspection GmbH. All Rights Reserved 
/// \file FileVersion.cpp
/// DESCRIPTION
//******************************************************************************

#pragma once

#include "stdafx.h"
#include "FileVersion.h"




namespace SvSyl
{

	bool FileVersion::getFileInfo(VS_FIXEDFILEINFO& rFileInfo, LPCTSTR Filename)
	{

		bool ret{ false };
		DWORD dwHandle{ 0 };
		DWORD size = ::GetFileVersionInfoSize(Filename, &dwHandle);
		unsigned char* lpData = new unsigned char[size];

		BOOL rc = ::GetFileVersionInfo(Filename, 0, size, lpData);
		if (rc)
		{
			VS_FIXEDFILEINFO* pFileInfo = nullptr;
			UINT Len = 0;
			if (::VerQueryValue(lpData, _T("\\"), (LPVOID *)&pFileInfo, (PUINT)&Len))
			{
				ret = true;
				rFileInfo = *pFileInfo;
				//memcpy(pFileInfo, *rFileInfo, Len);
			}
		}
		delete[] lpData;
		return ret;
	}
	bool FileVersion::isEqual(const VS_FIXEDFILEINFO& rFileInfoA, const VS_FIXEDFILEINFO& rFileInfoB)
	{

		bool eq = rFileInfoA.dwFileVersionMS == rFileInfoB.dwFileVersionMS;
		eq = eq && rFileInfoA.dwFileVersionLS == rFileInfoB.dwFileVersionLS;
		eq = eq && rFileInfoA.dwProductVersionMS == rFileInfoB.dwProductVersionMS;
		eq = eq && rFileInfoA.dwProductVersionLS == rFileInfoB.dwProductVersionLS;
		eq = eq && rFileInfoA.dwFileFlags == rFileInfoB.dwFileFlags;
		eq = eq && rFileInfoA.dwFileOS == rFileInfoB.dwFileOS;
		eq = eq && rFileInfoA.dwFileType == rFileInfoB.dwFileType;
		return eq;
	}

	bool FileVersion::isEqual(const std::string& FilePathA, const std::string& FilePathB)
	{

		VS_FIXEDFILEINFO FileInfoA, FileInfoB;
		FileVersion::getFileInfo(FileInfoA, FilePathA.c_str());
		FileVersion::getFileInfo(FileInfoB, FilePathB.c_str());
		return FileVersion::isEqual(FileInfoA, FileInfoB);

	}
}


