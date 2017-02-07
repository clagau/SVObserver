//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file VersionString.cpp 
/// All Rights Reserved 
//*****************************************************************************

/// Class to retrieve version string avoid unnecessary calls of BuildVersionString
//******************************************************************************
#include "StdAfx.h"
#include "RRSVersionString.h"


 SVString RRSVersionString::Get()
 {
	 static SVString versionString = BuildVersionString();
	 return versionString;
 }
 

SVString RRSVersionString::BuildVersionString()
 {
	 SVString Result;
	 TCHAR moduleFilename[512];
	 ::GetModuleFileName( nullptr, moduleFilename, 512);

	 DWORD dwHandle;
	 DWORD size = ::GetFileVersionInfoSize( moduleFilename, &dwHandle );
	 unsigned char* lpData = new unsigned char[size];

	 BOOL rc = ::GetFileVersionInfo(moduleFilename, 0, size, lpData);
	 if (rc)
	 {
		 VS_FIXEDFILEINFO* pFileInfo = nullptr;
		 UINT Len = 0;
		 if (::VerQueryValueA(lpData, "\\", (LPVOID *)&pFileInfo, (PUINT)&Len)) 
		 {
			 std::stringstream buf;

			 buf << HIWORD(pFileInfo->dwFileVersionMS);
			 buf << ".";
			 buf << std::setfill('0') << std::setw(2) << LOWORD(pFileInfo->dwFileVersionMS);

			 if( HIWORD(pFileInfo->dwFileVersionLS) < 255 )
			 {
				 buf << "b" << HIWORD(pFileInfo->dwFileVersionLS);
			 }
			 Result = buf.str();
		 }
	 }
	 delete [] lpData;

#ifdef _DEBUG
	 Result += "d";        // For debug builds.
#endif

	 return Result;
 }

