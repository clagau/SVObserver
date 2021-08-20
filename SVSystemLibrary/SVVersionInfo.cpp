//******************************************************************************
//* COPYRIGHT (c) 2011 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVersionInfo
//* .File Name       : $Workfile:   SVVersionInfo.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2014 06:08:38  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <iomanip>
//Moved to precompiled header: #include <sstream>
#include "SVVersionInfo.h"

#pragma comment (lib, "version.lib")

//do not change the directly following line: it will be directly manipulated by the BuildController
auto constexpr c_instrumentedForBullseye = false;
auto constexpr c_bullseyeVersionInfoString = _T(" (for bullseye)");

namespace SvSyl
{
bool getFileInfo(VS_FIXEDFILEINFO& rFileInfo)
{
	bool isValid = false;
	TCHAR moduleFilename[512];
	::GetModuleFileName(nullptr, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);

	std::vector<unsigned char> data;
	data.resize(size);

	BOOL rc = ::GetFileVersionInfo(moduleFilename, 0, size, &data.at(0));
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = nullptr;
		UINT Len = 0;
		if (::VerQueryValue(&data.at(0), _T("\\"), (LPVOID*)&pFileInfo, (PUINT)&Len))
		{
			isValid = true;
			rFileInfo = *pFileInfo;
			//memcpy(pFileInfo, *rFileInfo, Len);
		}
	}
	return isValid;
}

std::string SVVersionInfo::GetVersion()
{
	std::string Result;

	VS_FIXEDFILEINFO fileInfo;
	bool isValid = getFileInfo(fileInfo);
	if (isValid)
	{
		std::stringstream buf;
		buf << HIWORD(fileInfo.dwFileVersionMS) << _T(".") << std::setfill('0') << std::setw(2) << LOWORD(fileInfo.dwFileVersionMS);

		//Patch number when between 1-99,  SVN revision number when >= 100
		auto patchOrSvnNumber = HIWORD(fileInfo.dwFileVersionLS);
		if (patchOrSvnNumber > 0 && patchOrSvnNumber < 100)
		{
			buf << _T(".") << patchOrSvnNumber;
		}
		else if (patchOrSvnNumber >= 100)
		{
			buf << _T(", Revision ") << patchOrSvnNumber;
		}

		auto alphaOrBetaNumber = LOWORD(fileInfo.dwFileVersionLS);
		if (alphaOrBetaNumber > 0 && alphaOrBetaNumber < 255)
		{
			buf << _T(" Beta ") << std::setfill('0') << std::setw(3) << alphaOrBetaNumber;
		}
		else if (alphaOrBetaNumber > 1000)
		{
			buf << _T(" Alpha ") << std::setfill('0') << std::setw(3) << alphaOrBetaNumber;
		}

		Result = buf.str();
	}


#ifdef _DEBUG
	Result += _T("d");        // For debug builds.
#endif

	if (c_instrumentedForBullseye)
	{
		Result += c_bullseyeVersionInfoString;
	}

	return Result;
}

unsigned long SVVersionInfo::GetLongVersion()
{
	unsigned long l_Version = 0;

	VS_FIXEDFILEINFO fileInfo;
	bool isValid = getFileInfo(fileInfo);
	if (isValid)
	{
		SVVersionUnion l_TempVersion;
		l_TempVersion.m_VersionParts.m_Unused = 0;
		l_TempVersion.m_VersionParts.m_Major = static_cast<unsigned char>(std::min< WORD >(HIWORD(fileInfo.dwFileVersionMS), 255));
		l_TempVersion.m_VersionParts.m_Minor = static_cast<unsigned char>(std::min< WORD >(LOWORD(fileInfo.dwFileVersionMS), 255));
		l_TempVersion.m_VersionParts.m_Beta = static_cast<unsigned char>(std::min< WORD >(LOWORD(fileInfo.dwFileVersionLS), 255));

		l_Version = l_TempVersion.m_Version;
	}

	return l_Version;
}

std::string SVVersionInfo::GetShortTitleVersion()
{
	std::string Result;

	VS_FIXEDFILEINFO fileInfo;
	bool isValid = getFileInfo(fileInfo);
	if (isValid)
	{
		std::stringstream buf;

		buf << HIWORD(fileInfo.dwFileVersionMS);
		buf << _T(".");
		buf << std::setfill('0') << std::setw(2) << LOWORD(fileInfo.dwFileVersionMS);

		//Patch number when between 1-99,  SVN revision number when >= 100
		auto patchOrSvnNumber = HIWORD(fileInfo.dwFileVersionLS);
		if (patchOrSvnNumber > 0 && patchOrSvnNumber < 100)
		{
			buf << _T(".") << patchOrSvnNumber;
		}
		else if (patchOrSvnNumber >= 100)
		{
			buf << _T("r") << patchOrSvnNumber;
		}

		auto alphaOrBetaNumber = LOWORD(fileInfo.dwFileVersionLS);
		// this signifies a beta if nonzero and below 255 and an alpha if above 1001
		if (alphaOrBetaNumber > 0 && alphaOrBetaNumber < 255)
		{
			buf << _T("b") << alphaOrBetaNumber;
		}
		else if (alphaOrBetaNumber > 1000)
		{
			buf << _T("ALPHA") << alphaOrBetaNumber;
		}

		Result = buf.str();
	}

#ifdef _DEBUG
	Result += _T("d");        // For debug builds.
#endif

	if (c_instrumentedForBullseye)
	{
		Result += c_bullseyeVersionInfoString;
	}

	return Result;
}
} //namespace SvSyl

