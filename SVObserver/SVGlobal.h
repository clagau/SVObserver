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
#pragma endregion Includes

typedef std::pair<CString, CString> StrStrPair;
typedef std::map<int, StrStrPair > IntStrPairMap;
typedef std::vector<StrStrPair> StringPairVect;

class SVObserverApp;
class SVDataManager;

extern SVObserverApp TheSVObserverApp;
extern SVDataManager TheSVDataManager;

enum SV_THRESHOLD_SELECTION_ENUM
{
	SV_USE_SLIDER                 = 0,
	SV_USE_MAX_MINUS_PERCENT_DIFF = 1,
	SV_USE_MAX_MINUS_OFFSET       = 2,
	SV_USE_MIN_PLUS_OFFSET        = 3,
};

const TCHAR* const cThresholdSelectionEnums = _T( "Selectable=0,Max - % Dif=1,Max - Offset=1,Min + Offset=2" );

enum SV_EDGECONTROL_POLARISATION_ENUM
{
	SV_UNDEFINED_POLARISATION	= 0x0000,	// 0000 0000 0000 0000
	SV_ANY_POLARISATION			= 0x0003,	// 0000 0000 0000 0011
	SV_NEGATIVE_POLARISATION	= 0x0002,	// 0000 0000 0000 0010
	SV_POSITIVE_POLARISATION	= 0x0001	// 0000 0000 0000 0001
};// end SV_EDGECONTROL_POLARISATION_ENUM

// Add String for SVEnumerateValueObjectClass
const TCHAR* const cPolarisationEnums = _T( "Any=3,Negative=2,Positive=1" );

enum SV_EDGECONTROL_EDGESELECT_ENUM
{
	SV_UNDEFINED_EDGE			= 0x0000,	// 0000 0000 0000 0000
	SV_FIRST_EDGE				= 0x8000,	// 1000 0000 0000 0000
	SV_LAST_EDGE				= 0x2000,	// 0010 0000 0000 0000
	SV_THIS_EDGE				= 0x1000	// 0001 0000 0000 0000
};// end SV_EDGECONTROL_EDGESELECT_ENUM

// Add String for SVEnumerateValueObjectClass
const TCHAR* const cEdgeSelectEnums = _T( "First=32768,Last=8192,This=4096" );

enum SV_EDGECONTROL_POSITION_ENUM
{
	SV_UNDEFINED_POSITION		= 0x0000,	// 0000 0000 0000 0000
	SV_START_POSITION			= 0x0080,	// 0000 0000 1000 0000
	SV_CENTER_POSITION			= 0x0040,	// 0000 0000 0100 0000
	SV_END_POSITION				= 0x0020,	// 0000 0000 0010 0000
	SV_OFFSET_POSITION			= 0x0010	// 0000 0000 0001 0000
};// end SV_EDGECONTROL_POSITION_ENUM

// Add String for SVEnumerateValueObjectClass
const TCHAR* const cPositionEnums = _T( "Start=128,Center=64,End=32,Offset=16" );

enum SV_EDGECONTROL_DIRECTION_ENUM
{
	SV_UNDEFINED_DIRECTION		= 0x0000,	// 0000 0000 0000 0000
	SV_TAIL_TO_HEAD_DIRECTION	= 0x0200,	// 0000 0010 0000 0000
	SV_HEAD_TO_TAIL_DIRECTION	= 0x0100	// 0000 0001 0000 0000
};// end SV_EDGECONTROL_DIRECTION_ENUM

// Add String for SVEnumerateValueObjectClass
const TCHAR* const cDirectionEnums = _T( "Undefined=0,Tail to Head=512,Head to Tail=256" );

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

// Predefined TCHAR String Constants...
const CString SV_TSTR_COMMA                 ( _T( ", " ) );
const CString SV_TSTR_DOLLAR                ( _T( "$" ) );
const CString SV_TSTR_SPACE                 ( _T( " " ) );
const CString SV_TSTR_NEWLINE               ( _T( "\n" ) );
const CString SV_TSTR_TAB                   ( _T( "\t" ) );

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
BOOL SVGetPathInformation( CString& RStrOutput, LPCTSTR TStrFileInputPath, DWORD DwMask );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetVersionString
// -----------------------------------------------------------------------------
// .Description : Use this function to convert an SVObserver Version to a string.
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetVersionString( CString& RSTRCurrentVersion, DWORD dwVersion );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConvertToHexString
// -----------------------------------------------------------------------------
// .Description : Convert hex binary data to a hex Dump String
////////////////////////////////////////////////////////////////////////////////
void SVConvertToHexString( DWORD len, LPBYTE buff, CString& hexString );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConvertFromHexString
// -----------------------------------------------------------------------------
// .Description : Convert hex Dump String to hex binary data
////////////////////////////////////////////////////////////////////////////////
BOOL SVConvertFromHexString( DWORD &len, LPBYTE *buff, CString& hexString );

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

enum svModeEnum
{
	SVIM_MODE_UNKNOWN = 0,
	SVIM_MODE_ONLINE,
	SVIM_MODE_OFFLINE,
	SVIM_MODE_REGRESSION,
	SVIM_MODE_TEST,
	SVIM_MODE_EDIT,
	SVIM_MODE_CHANGING = 100,
	SVIM_MODE_REMOVE = 0x8000,
};


#define SV_ERROR_CONDITION     0xc0000000