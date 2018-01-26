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
#include "SVMessage/SVMessage.h"
#include "SVMatroxLibrary\SVMatroxEnums.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

typedef std::map<int, SvDef::StringPair > IntStrPairMap;

class SVObserverApp;
class SVDataManager;

extern SVObserverApp TheSVObserverApp;
extern SVDataManager TheSVDataManager;

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

// Default Data Type ...
const DWORD SV_1BIT                     = 1;
const DWORD SV_8BIT_INT                 = ( SV_GET_BIT_NUMBER( __int8 ) );
const DWORD SV_BYTE                     = SV_8BIT_INT;
const DWORD SV_SIGNED_8BIT_INT          = ( SV_GET_BIT_NUMBER( __int8 ) + static_cast< DWORD >( SVBufferSigned ) );
const DWORD SV_CHAR                     = SV_SIGNED_8BIT_INT;
const DWORD SV_16BIT_INT                = ( SV_GET_BIT_NUMBER( __int16 ) + SVBufferUnsigned );
const DWORD SV_SIGNED_16BIT_INT         = ( SV_GET_BIT_NUMBER( __int16 ) + static_cast< DWORD >( SVBufferSigned ) );
const DWORD SV_32BIT_INT                = ( SV_GET_BIT_NUMBER( __int32 ) + SVBufferUnsigned );
const DWORD SV_DWORD                    = SV_32BIT_INT;
const DWORD SV_SIGNED_32BIT_INT         = ( SV_GET_BIT_NUMBER( __int32 ) + static_cast< DWORD >( SVBufferSigned ) );
const DWORD SV_LONG                     = SV_SIGNED_32BIT_INT;
const DWORD SV_64BIT_INT                = ( SV_GET_BIT_NUMBER( __int64 ) + SVBufferUnsigned );
const DWORD SV_SIGNED_64BIT_INT         = ( SV_GET_BIT_NUMBER( __int64 ) + static_cast< DWORD >( SVBufferSigned ) );
const DWORD SV_FLOAT                    = ( SV_GET_BIT_NUMBER( float )  + static_cast< DWORD >( SVBufferFloat ) );

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
// .Title       : SVCalcLinePixelNumber
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
long SVCalcLinePixelNumber( long Width, long Height );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetDataTypeMin
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
double SVGetDataTypeMin( DWORD DataType );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetDataTypeMax
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
double SVGetDataTypeMax( DWORD DataType );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetDataTypeRange
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
double SVGetDataTypeRange( DWORD DataType );

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

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConvertToHexString
// -----------------------------------------------------------------------------
// .Description : Convert hex binary data to a hex Dump String
////////////////////////////////////////////////////////////////////////////////
void SVConvertToHexString( DWORD len, LPBYTE buff, std::string& rHexString );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConvertFromHexString
// -----------------------------------------------------------------------------
// .Description : Convert hex Dump String to hex binary data
////////////////////////////////////////////////////////////////////////////////
BOOL SVConvertFromHexString( DWORD &len, LPBYTE *buff, const std::string& rHexString );

CMenu* SVFindMenuByCommand( CMenu *pMenu, int nID, BOOL bIncludeSubMenues, int& rMenuPos );
CMenu* SVFindMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues, int& rMenuPos );
CMenu* SVFindSubMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues );

//Methods that were once in SVCommRC
LPCTSTR SVRCGetSVCPathName();
void 	SVRCSetSVCPathName( LPCTSTR TStrPathName );

//************************************
// Method:    EnableParentMenu
// Description:  Find the ParentMenu entry  of an Entry and enable or disable it 
// Parameter: CMenu * pMenu 
// Parameter: UINT ID Command Id of the entry 
// Parameter: bool Enable true is enable false is disable 
// Parameter: int  start index of submenu where search for the command entry is started.
// Returns:   bool true if sucessfull 
//************************************
bool EnableParentMenu(CMenu* pMenu, UINT ID, bool Enable, int start =0);

extern TCHAR	SVRCCurrentSVCPathName[];


#define SV_ERROR_CONDITION     0xc0000000