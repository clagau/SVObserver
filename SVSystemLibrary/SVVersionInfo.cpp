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
#include <iomanip>
#include <sstream>
#include "SVVersionInfo.h"

#pragma comment (lib, "version.lib")

using namespace SeidenaderVision;

std::string SVVersionInfo::GetVersion()
{
	std::string verStr;

	TCHAR moduleFilename[512];
	::GetModuleFileName(NULL, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfo(moduleFilename, NULL, size, lpData);
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = NULL;
		//::ZeroMemory(&version, sizeof(VS_FIXEDFILEINFO));
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
	::GetModuleFileName(NULL, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfo(moduleFilename, NULL, size, lpData);
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = NULL;
		//::ZeroMemory(&version, sizeof(VS_FIXEDFILEINFO));
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
	::GetModuleFileName(NULL, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfo(moduleFilename, NULL, size, lpData);
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = NULL;
		//::ZeroMemory(&version, sizeof(VS_FIXEDFILEINFO));
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
	::GetModuleFileName(NULL, moduleFilename, sizeof(moduleFilename));

	DWORD dwHandle;
	DWORD size = ::GetFileVersionInfoSize(moduleFilename, &dwHandle);
	unsigned char* lpData = new unsigned char[size];

	BOOL rc = ::GetFileVersionInfo(moduleFilename, NULL, size, lpData);
	if (rc)
	{
		VS_FIXEDFILEINFO* pFileInfo = NULL;
		//::ZeroMemory(&version, sizeof(VS_FIXEDFILEINFO));
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSystemLibrary\SVVersionInfo.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2014 06:08:38   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  940
 * SCR Title:  Update SVObserver Version Number for the 7.00 Release
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added leading zeros for second digit of the version number and beta.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 18:03:46   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   05 Mar 2013 17:06:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  817
 * SCR Title:  Import/Export of Inspections
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to make minor part of the version display as 2 digits
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Oct 2012 11:04:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add beta information to title version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Sep 2012 18:10:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added additional string representation of the version number.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Jul 2012 12:05:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  770
 * SCR Title:  Upgrade SVObserver to 6.00 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new functionality to use the resource version information instead of data found in SVObserver.cpp.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/