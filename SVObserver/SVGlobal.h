//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGlobal
//* .File Name       : $Workfile:   SVGlobal.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.7  $
//* .Check In Date   : $Date:   10 Dec 2014 12:02:22  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

typedef std::map<int, SvDef::StringPair > IntStrPairMap;

enum SVGetPathInfo
{
	SVDRIVE				= 0x81,			// 0000 0000 1000 0001
	SVDIR				= 0x82,			// 0000 0000 1000 0010
	SVDIRECTORY			= 0x82,
	SVFILE				= 0x84,			// 0000 0000 1000 0100
	SVFILENAME			= 0x84,
	SVEXT				= 0x88,			// 0000 0000 1000 1000
	SVEXTENSION			= 0x88,
	SVFULL				= 0x8f,			// 0000 0000 1000 1111
	SVPATH				= 0x8f,
	SVPATHNAME			= 0x8f
};

enum ConfigFileType
{
	SvzStandard,
	SvzFormatPutConfig,
	SvzFormatDefaultName,
	PackedFormat,
};


////////////////////////////////////////////////////////////////////////////////
// Global Helpers:
////////////////////////////////////////////////////////////////////////////////

#define SV_GET_BIT_NUMBER( X )						( sizeof( X ) * 8 )

////////////////////////////////////////////////////////////////////////////////
// DLL Handling:
////////////////////////////////////////////////////////////////////////////////
#ifndef _AFXDLL
	// If you choose to use MFC as a static library link and 
	//	using the Multithread Runtime Library switch /MT
	#define SVLoadLibrary( X )		LoadLibrary( X )
	#define SVFreeLibrary( X )		FreeLibrary( X )
#else
	// If you choose to link a common used DLL version of MFC and 
	//	using the Multithread DLL Runtime Library switch /MD
	#define SVLoadLibrary( X )		AfxLoadLibrary( X )
	#define SVFreeLibrary( X )		AfxFreeLibrary( X )
#endif

////////////////////////////////////////////////////////////////////////////////
//	Global Application Defines
////////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4308)

////////////////////////////////////////////////////////////////////////////
// IO DLL Function Types

//	SVIOSetModuleOutput
//	SVIOSetResultOutput
//  SVIOInvertModuleInput
//  SVIOInvertModuleOutput
//  SVIOInvertResultInput
//  SVIOInvertResultOutput
typedef BOOL ( CALLBACK* SVIODLLBOOLDWORDBOOLFUNCTION ) ( DWORD, BOOL );
#define SVDIGITALIOOUTPUTFUNC SVIODLLBOOLDWORDBOOLFUNCTION
#define SVDIGITALIOINVERTFUNC SVIODLLBOOLDWORDBOOLFUNCTION

//	SVIOGetModuleInput
//	SVIOGetResultInput
typedef BOOL ( CALLBACK* SVIODLLBOOLDWORDBOOLPTRFUNCTION ) ( DWORD, BOOL* );
#define SVDIGITALIOINPUTFUNC SVIODLLBOOLDWORDBOOLPTRFUNCTION

//	SVIOForceModuleInput
//	SVIOForceModuleOutput
//	SVIOForceResultInput
//	SVIOForceResultOutput
typedef BOOL ( CALLBACK* SVIODLLBOOLDWORDBOOLBOOLFUNCTION ) ( DWORD, BOOL, BOOL );
#define SVDIGITALIOFORCEFUNC SVIODLLBOOLDWORDBOOLBOOLFUNCTION

//	SVIOGetBoardType
//	SVIOGetBoardManufacturer
typedef TCHAR* ( CALLBACK* SVIODLLTCHARPTRFUNCTION ) ();

//	SVIOGetBoardMaxModuleInputChannels
//	SVIOGetBoardMaxModuleOutputChannels
//	SVIOGetBoardMaxResultInputChannels
//	SVIOGetBoardMaxResultOutputChannels
typedef DWORD ( CALLBACK* SVIODLLDWORDFUNCTION ) ();

//	SVIOInitIOEventFunctionPointer
typedef void ( CALLBACK* SVIODLLVOIDLPVOIDFUNCTION ) ( LPVOID );

//	SVIOTestOutputs
typedef void ( CALLBACK* SVIODLLVOIDFUNCTION ) ();

typedef BOOL ( CALLBACK* SVDLL_BOOL_BYTEPTR_DWORDPTR_FUNCTION ) ( BYTE*, DWORD* );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVSearchForMDIChildWnd
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
CMDIChildWnd* SVSearchForMDIChildWnd( CWnd* PStartWnd );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetPathInformation
// -----------------------------------------------------------------------------
// .Description : Uses SVGetPathInfo enumeration...
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetPathInformation( std::string& rOutput, LPCTSTR TStrFileInputPath, DWORD DwMask );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetVersionString
// -----------------------------------------------------------------------------
// .Description : Use this function to convert an SVObserver Version to a string.
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetVersionString( std::string& rCurrentVersion, DWORD dwVersion );

CMenu* SVFindMenuByCommand( CMenu *pMenu, int nID, BOOL bIncludeSubMenues, int& rMenuPos );
CMenu* SVFindMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues, int& rMenuPos );
CMenu* SVFindSubMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues );

//Methods that were once in SVCommRC
LPCTSTR SVRCGetSVCPathName();
void 	SVRCSetSVCPathName( LPCTSTR TStrPathName );

extern TCHAR	SVRCCurrentSVCPathName[];

