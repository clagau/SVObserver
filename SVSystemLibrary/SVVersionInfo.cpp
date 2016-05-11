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

using namespace SeidenaderVision;

std::string SVVersionInfo::GetVersion()
{
	std::string verStr;

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
		if (::VerQueryValue(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&Len)) 
		{
			std::stringstream buf;
			buf << HIWORD(pFileInfo->dwFileVersionMS) << "." << std::setfill( '0' ) << std::setw( 2 ) << LOWORD(pFileInfo->dwFileVersionMS);

			auto alphaOrBetaNumber=HIWORD(pFileInfo->dwFileVersionLS);
			if( alphaOrBetaNumber > 0 && alphaOrBetaNumber < 255)
			{
				buf << " Beta " << std::setfill( '0' ) << std::setw( 3 ) << alphaOrBetaNumber;
			}
			else
				if( alphaOrBetaNumber >1000)
				{
					buf << " Alpha " <<  std::setfill( '0' ) << std::setw( 3 ) << alphaOrBetaNumber;
				}

			if( LOWORD(pFileInfo->dwFileVersionLS) > 0 )
			{
				buf << ", Revision " << LOWORD(pFileInfo->dwFileVersionLS);
			}

			verStr = buf.str();
		}
	}
	delete [] lpData;

	#ifdef _DEBUG
		verStr += _T("d");        // For debug builds.
	#endif

	return verStr;
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
		if (::VerQueryValue(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&Len)) 
		{
			// Version numbering code:
			//
			// DwCurrentVersion, Type DWORD
			// Meaning:		0x00mmssbb => Version m.s  - Beta b 
			// EXCEPTION:	if b = 255 is the release indicator 
			//					No Beta number in released Versions!!!
			//	e.g.
			//				0x000100FF => Version 1.0
			//		        0x00011219 => Version 1.18 Beta 25
			//				0x00020001 => Version 2.0  Beta 1
			union SVVersionUnion
			{
				struct SVVersionParts
				{
					unsigned long m_Beta : 8;
					unsigned long m_Minor : 8;
					unsigned long m_Major : 8;
					unsigned long m_Unused : 8;
				} m_VersionParts;

				unsigned long m_Version;

			} l_TempVersion;

			l_TempVersion.m_VersionParts.m_Unused = 0;
			l_TempVersion.m_VersionParts.m_Major = static_cast< unsigned char >( std::min< WORD >( HIWORD( pFileInfo->dwFileVersionMS ), 255 ) );
			l_TempVersion.m_VersionParts.m_Minor = static_cast< unsigned char >( std::min< WORD >( LOWORD( pFileInfo->dwFileVersionMS ), 255 ) );
			l_TempVersion.m_VersionParts.m_Beta = static_cast< unsigned char >( std::min< WORD >( HIWORD( pFileInfo->dwFileVersionLS ), 255 ) );

			l_Version = l_TempVersion.m_Version;
		}
	}

	delete [] lpData;

	return l_Version;
}

std::string SVVersionInfo::GetTitleVersion()
{
	std::string verStr;

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
		if (::VerQueryValue(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&Len)) 
		{
			std::stringstream buf;

			buf << HIWORD(pFileInfo->dwFileVersionMS);
			buf << ".";
			buf << std::setfill('0') << std::setw(2) << LOWORD(pFileInfo->dwFileVersionMS);
			buf << ".";
			buf << HIWORD(pFileInfo->dwFileVersionLS);

			verStr = buf.str();
		}
	}
	delete [] lpData;

	#ifdef _DEBUG
		verStr += _T("d");        // For debug builds.
	#endif

	return verStr;
}

std::string SVVersionInfo::GetShortTitleVersion()
{
	std::string verStr;

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
		if (::VerQueryValue(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&Len)) 
		{
			std::stringstream buf;

			buf << HIWORD(pFileInfo->dwFileVersionMS);
			buf << ".";
			buf << std::setfill('0') << std::setw(2) << LOWORD(pFileInfo->dwFileVersionMS);

			auto alphaOrBetaNumber=HIWORD(pFileInfo->dwFileVersionLS);
			//Arvid this signifies a beta if nonzero and below 255 and an alpha if above 1001

			if( alphaOrBetaNumber > 0 && alphaOrBetaNumber < 255)
			{
				buf << "b" << alphaOrBetaNumber;
			}
			else
				if( alphaOrBetaNumber >1000)
				{
					buf << "ALPHA" << alphaOrBetaNumber;
				}

			if( LOWORD(pFileInfo->dwFileVersionLS) > 0 )
			{
				buf << "r" << LOWORD(pFileInfo->dwFileVersionLS);
			}

			verStr = buf.str();
		}
	}

	delete [] lpData;

	#ifdef _DEBUG
		verStr += _T("d");        // For debug builds.
	#endif

	return verStr;
}

