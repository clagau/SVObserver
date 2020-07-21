//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVUtilityGlobals
//* .File Name       : $Workfile:   SVUtilityGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 19:39:26  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

typedef unsigned long SVHANDLE;

#define SV_SEVERITY( p_Status ) \
	( ( ( ( unsigned long ) p_Status ) >> 30 ) & 0x00000003 )

HRESULT SafeArrayPutElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
HRESULT SafeArrayGetElementNoCopy(SAFEARRAY* psa, long* rgIndices, void* pv);
HRESULT SafeArrayGetElementPointer(SAFEARRAY* psa, long* rgIndices, void** ppv);

void KeepPrevError( HRESULT& p_rhrPrev, HRESULT p_hrNew );

//@TODO[Arvid][10.00][20.07.2020] Die folgenden Funktionen wären besser in der SVVFileSystemLibrary aufgehoben:
bool SVCheckPathDir( LPCTSTR PathName, bool CreateIfDoesNotExist );
bool SVDeleteFiles( LPCTSTR PathName, bool IncludeSubDirectories );

//************************************
/// this function checks the existence of a 'proper' drive letter in rFilePath
/// \returns bool – true if valid
//************************************
bool ValidateDrive(const std::string& rFilePath);
