/// \copyright COPYRIGHT (c) 2011,2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file BlobAnalyzer2.cpp

#include "stdafx.h"
#include "SVVersionInfo.h"

#pragma comment (lib, "version.lib")

auto constexpr c_bullseyeVersionInfoString = _T(" (for bullseye)");
//do not change the line directly following this comment: it will be directly manipulated by the BuildController
auto constexpr c_instrumentedForBullseye = false;
//do not change the line directly following this comment: it will be directly manipulated by the BuildController
auto constexpr c_GitIdentifier = 0;

namespace SvSyl
{

std::string g_releasedVersionString = _T("");

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

std::string getSvoVersionAsString(bool showRcInformation)
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
			buf << _T("r") << patchOrSvnNumber;
		}
		if (0 != c_GitIdentifier)
		{
			buf << _T("[") << std::hex << c_GitIdentifier << std::dec << _T("]");
		}
		
		auto basicVersionString = buf.str(); // I.e., without alpha, beta or RC information

		auto alphaOrBetaOrRcIndex = LOWORD(fileInfo.dwFileVersionLS); // cf. Autobuild.h
		if (alphaOrBetaOrRcIndex > 0 && alphaOrBetaOrRcIndex < 255)
		{
			buf << _T(" beta ") << alphaOrBetaOrRcIndex;
		}
		else if (alphaOrBetaOrRcIndex > 1000)
		{
			buf << _T(" ALPHA ") << alphaOrBetaOrRcIndex;
		}
		else if (true == showRcInformation)
		{
			buf << _T(" Release Candidate ") << alphaOrBetaOrRcIndex;
		}

		if (buf.str() == g_releasedVersionString)
		{
			Result = basicVersionString;
		}
		else
		{
			Result = buf.str();
		}
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


void setReleasedSvoVersion(const std::string& rReleasedVersionString)
{
	g_releasedVersionString = rReleasedVersionString;
}


unsigned long getSvoVersionAsUnsignedLong()
{
	unsigned long Version = 0;

	VS_FIXEDFILEINFO fileInfo;
	bool isValid = getFileInfo(fileInfo);
	if (isValid)
	{
		SVVersionUnion TempVersion;
		TempVersion.m_VersionParts.m_Unused = 0;
		TempVersion.m_VersionParts.m_Major = static_cast<unsigned char>(std::min< WORD >(HIWORD(fileInfo.dwFileVersionMS), 255));
		TempVersion.m_VersionParts.m_Minor = static_cast<unsigned char>(std::min< WORD >(LOWORD(fileInfo.dwFileVersionMS), 255));
		TempVersion.m_VersionParts.m_Beta = static_cast<unsigned char>(std::min< WORD >(LOWORD(fileInfo.dwFileVersionLS), 255));

		Version = TempVersion.m_Version;
	}

	return Version;
}

} //namespace SvSyl

