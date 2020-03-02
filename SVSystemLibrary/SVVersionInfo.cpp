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

namespace SvSyl
{
	std::string SVVersionInfo::GetVersion()
	{
		std::string Result;

		TCHAR moduleFilename[512];
		::GetModuleFileName(nullptr, moduleFilename, sizeof(moduleFilename));

		DWORD dwHandle;
		DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);
		unsigned char* lpData = new unsigned char[size];

		BOOL rc = ::GetFileVersionInfo(moduleFilename, 0, size, lpData);
		if (rc)
		{
			VS_FIXEDFILEINFO* pFileInfo = nullptr;
			UINT Len = 0;
			if (::VerQueryValue(lpData, _T("\\"), (LPVOID *)&pFileInfo, (PUINT)&Len)) 
			{
				std::stringstream buf;
				buf << HIWORD(pFileInfo->dwFileVersionMS) << _T(".") << std::setfill( '0' ) << std::setw( 2 ) << LOWORD(pFileInfo->dwFileVersionMS);

				//Patch number when between 1-99,  SVN revision number when >= 100
				auto patchOrSvnNumber = HIWORD(pFileInfo->dwFileVersionLS);
				if (patchOrSvnNumber > 0 && patchOrSvnNumber < 100)
				{
					buf << _T(".") << patchOrSvnNumber;
				}
				else if (patchOrSvnNumber >= 100)
				{
					buf << _T(", Revision ") << patchOrSvnNumber;
				}

				auto alphaOrBetaNumber = LOWORD(pFileInfo->dwFileVersionLS);
				if( alphaOrBetaNumber > 0 && alphaOrBetaNumber < 255)
				{
					buf << _T(" Beta ") << std::setfill( '0' ) << std::setw( 3 ) << alphaOrBetaNumber;
				}
				else if( alphaOrBetaNumber >1000)
				{
					buf << _T(" Alpha ") <<  std::setfill( '0' ) << std::setw( 3 ) << alphaOrBetaNumber;
				}

				Result = buf.str();
			}
		}
		delete [] lpData;

		#ifdef _DEBUG
			Result += _T("d");        // For debug builds.
		#endif

		return Result;
	}

	unsigned long SVVersionInfo::GetLongVersion()
	{
		unsigned long l_Version = 0;

		TCHAR moduleFilename[512];
		::GetModuleFileName(nullptr, moduleFilename, sizeof(moduleFilename));

		DWORD dwHandle;
		DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);
		unsigned char* lpData = new unsigned char[size];

		BOOL rc = ::GetFileVersionInfo(moduleFilename, 0, size, lpData);
		if (rc)
		{
			VS_FIXEDFILEINFO* pFileInfo = nullptr;
			UINT Len = 0;
			if (::VerQueryValue(lpData, _T("\\"), (LPVOID *)&pFileInfo, (PUINT)&Len)) 
			{
				SVVersionUnion l_TempVersion;
				l_TempVersion.m_VersionParts.m_Unused = 0;
				l_TempVersion.m_VersionParts.m_Major = static_cast< unsigned char >( std::min< WORD >( HIWORD( pFileInfo->dwFileVersionMS ), 255 ) );
				l_TempVersion.m_VersionParts.m_Minor = static_cast< unsigned char >( std::min< WORD >( LOWORD( pFileInfo->dwFileVersionMS ), 255 ) );
				l_TempVersion.m_VersionParts.m_Beta = static_cast< unsigned char >( std::min< WORD >( LOWORD( pFileInfo->dwFileVersionLS ), 255 ) );

				l_Version = l_TempVersion.m_Version;
			}
		}

		delete [] lpData;

		return l_Version;
	}

	std::string SVVersionInfo::GetShortTitleVersion()
	{
		std::string Result;

		TCHAR moduleFilename[512];
		::GetModuleFileName(nullptr, moduleFilename, sizeof(moduleFilename));

		DWORD dwHandle;
		DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);
		unsigned char* lpData = new unsigned char[size];

		BOOL rc = ::GetFileVersionInfo(moduleFilename, 0, size, lpData);
		if (rc)
		{
			VS_FIXEDFILEINFO* pFileInfo = nullptr;
			UINT Len = 0;
			if (::VerQueryValue(lpData, _T("\\"), (LPVOID *)&pFileInfo, (PUINT)&Len)) 
			{
				std::stringstream buf;

				buf << HIWORD(pFileInfo->dwFileVersionMS);
				buf << _T(".");
				buf << std::setfill('0') << std::setw(2) << LOWORD(pFileInfo->dwFileVersionMS);

				//Patch number when between 1-99,  SVN revision number when >= 100
				auto patchOrSvnNumber = HIWORD(pFileInfo->dwFileVersionLS);
				if (patchOrSvnNumber > 0 && patchOrSvnNumber < 100)
				{
					buf << _T(".") << patchOrSvnNumber;
				}
				else if (patchOrSvnNumber >= 100)
				{
					buf << _T("r") << patchOrSvnNumber;
				}

				auto alphaOrBetaNumber = LOWORD(pFileInfo->dwFileVersionLS);
				//Arvid this signifies a beta if nonzero and below 255 and an alpha if above 1001
				if( alphaOrBetaNumber > 0 && alphaOrBetaNumber < 255)
				{
					buf << _T("b") << alphaOrBetaNumber;
				}
				else if( alphaOrBetaNumber > 1000)
				{
					buf << _T("ALPHA") << alphaOrBetaNumber;
				}

				Result = buf.str();
			}
		}

		delete [] lpData;

		#ifdef _DEBUG
			Result += _T("d");        // For debug builds.
		#endif

		return Result;
	}
} //namespace SvSyl

