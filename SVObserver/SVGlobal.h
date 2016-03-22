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

extern SVObserverApp			TheSVObserverApp;
extern SVDataManager			TheSVDataManager;

enum SV_THRESHOLD_SELECTION_ENUM
{
	SV_USE_SLIDER                 = 0,
	SV_USE_MAX_MINUS_PERCENT_DIFF = 1,
	SV_USE_MAX_MINUS_OFFSET       = 2,
	SV_USE_MIN_PLUS_OFFSET        = 3,
};

const LPCSTR g_strThresholdSelectionEnums = 
				_T( "Selectable=0,Max - % Dif=1,Max - Offset=1,Min + Offset=2" );

enum SV_EDGECONTROL_POLARISATION_ENUM
{
	SV_UNDEFINED_POLARISATION	= 0x0000,	// 0000 0000 0000 0000
	SV_ANY_POLARISATION			= 0x0003,	// 0000 0000 0000 0011
	SV_NEGATIVE_POLARISATION	= 0x0002,	// 0000 0000 0000 0010
	SV_POSITIVE_POLARISATION	= 0x0001	// 0000 0000 0000 0001
};// end SV_EDGECONTROL_POLARISATION_ENUM

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strPolarisationEnums = 
				_T( "Any=3,Negative=2,Positive=1" );



enum SV_EDGECONTROL_EDGESELECT_ENUM
{
	SV_UNDEFINED_EDGE			= 0x0000,	// 0000 0000 0000 0000
	SV_FIRST_EDGE				= 0x8000,	// 1000 0000 0000 0000
	SV_LAST_EDGE				= 0x2000,	// 0010 0000 0000 0000
	SV_THIS_EDGE				= 0x1000	// 0001 0000 0000 0000
};// end SV_EDGECONTROL_EDGESELECT_ENUM

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strEdgeSelectEnums = 
				_T( "First=32768,Last=8192,This=4096" );



enum SV_EDGECONTROL_POSITION_ENUM
{
	SV_UNDEFINED_POSITION		= 0x0000,	// 0000 0000 0000 0000
	SV_START_POSITION			= 0x0080,	// 0000 0000 1000 0000
	SV_CENTER_POSITION			= 0x0040,	// 0000 0000 0100 0000
	SV_END_POSITION				= 0x0020,	// 0000 0000 0010 0000
	SV_OFFSET_POSITION			= 0x0010	// 0000 0000 0001 0000
};// end SV_EDGECONTROL_POSITION_ENUM

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strPositionEnums = 
				_T( "Start=128,Center=64,End=32,Offset=16" );



enum SV_EDGECONTROL_DIRECTION_ENUM
{
	SV_UNDEFINED_DIRECTION		= 0x0000,	// 0000 0000 0000 0000
	SV_TAIL_TO_HEAD_DIRECTION	= 0x0200,	// 0000 0010 0000 0000
	SV_HEAD_TO_TAIL_DIRECTION	= 0x0100	// 0000 0001 0000 0000
};// end SV_EDGECONTROL_DIRECTION_ENUM

// Add String for SVEnumerateValueObjectClass
const LPCSTR g_strDirectionEnums = 
				_T( "Undefined=0,Tail to Head=512,Head to Tail=256" );


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

//******************************************************************************
//* DEFINE(S):
//******************************************************************************

#define SV_PPQ_SUPPORT             // PPQ Support
#define SV_NO_MIL_ERROR_MESSAGES   // MIL Error Message Support

////////////////////////////////////////////////////////////////////////////////
// Global Helpers:
////////////////////////////////////////////////////////////////////////////////

#define SV_GET_BIT_NUMBER( X )						( sizeof( X ) * 8 )

// Creates new string using malloc and fills it with the GUID, use free() to deallocate the string memory...
#define SV_GUID_TO_NEW_STRING( XGUID, XLPTSTR )	\
 		{ \
			if( ( XLPTSTR ) = ( LPTSTR ) malloc( sizeof( TCHAR ) * 256 ) )	\
			{	\
				_stprintf( ( XLPTSTR ), "{ 0x%x, 0x%x, 0x%x, { 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x } }", \
				( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
				( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
				( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] );	\
			}	\
		}

// GUID String
// XLPSZSTRING must be a LPTSTR !!!
#define SV_GUID_TO_STRING( XGUID, XLPSZSTRING, XLENGTH ) \
		{	\
			if( ( XLPSZSTRING ) && ( XLENGTH ) > 0 ) \
			{ \
				TCHAR szBuf[ 100 ]; \
				_stprintf( szBuf, "{%x-%x-%x-%x %x %x %x %x %x %x %x}", \
						   ( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
						   ( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
						   ( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] ); \
				_tcsncpy( XLPSZSTRING, szBuf, min( ( XLENGTH ), 100 ) - 1 ); \
			} \
		}


// GUID String
// AFXSTRING must be a CString type !!!
#define SV_GUID_TO_AFXSTRING( XGUID, XAFXSTRING ) \
		{	\
			XAFXSTRING.Format( _T("\"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}\""), \
							  ( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
							  ( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
							  ( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] ); \
		}


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

// May be obsolete...
// #define SVR_MUNICH
// #define SVR_HARRISBURG

const long SV_DEFAULT_TOOL_UPPER_TRESH              = 255L;
const long SV_DEFAULT_TOOL_LOWER_TRESH              = 40L;
const BOOL SV_DEFAULT_TOOL_TRESH_ACTIVATE           = TRUE;
const BOOL SV_DEFAULT_TOOL_UPPER_TRESH_ACTIVATE     = TRUE;
const BOOL SV_DEFAULT_TOOL_LOWER_TRESH_ACTIVATE	    = TRUE;			 

// Default view color settings...
const COLORREF SV_DEFAULT_RESULT_VIEW_BACKGROUND_COLOR      = RGB( 240, 255, 255 );
const COLORREF SV_DEFAULT_IMAGE_VIEW_BACKGROUND_COLOR       = RGB( 250, 255, 250 );

// Default image settings...
const long SV_DEFAULT_IMAGE_DEPTH            =   8L;
const long SV_DEFAULT_CAMERA_IMAGE_WIDTH     = 640L;
const long SV_DEFAULT_CAMERA_IMAGE_HEIGHT    = 480L;
const long SV_DEFAULT_IMAGE_VIEW_WIDTH       = SV_DEFAULT_CAMERA_IMAGE_WIDTH;
const long SV_DEFAULT_IMAGE_VIEW_HEIGHT      = SV_DEFAULT_CAMERA_IMAGE_HEIGHT;
const long SV_DEFAULT_TOOL_SET_VIEW_WIDTH    = 200L;
const long SV_DEFAULT_TOOL_SET_VIEW_HEIGHT   = 400L;
const long SV_DEFAULT_RESULT_VIEW_WIDTH      = 840L;
const long SV_DEFAULT_RESULT_VIEW_HEIGHT     = 200L;
const long SV_DEFAULT_FILTER_USER_ARRAY_SIZE = ( ( SV_DEFAULT_CAMERA_IMAGE_WIDTH ) * ( SV_DEFAULT_CAMERA_IMAGE_HEIGHT ) );

// Default tool extents
const long SV_DEFAULT_WINDOWTOOL_LEFT   =  10L;
const long SV_DEFAULT_WINDOWTOOL_TOP    =  10L;
const long SV_DEFAULT_WINDOWTOOL_WIDTH  = 100L;
const long SV_DEFAULT_WINDOWTOOL_HEIGHT = 100L;
const double SV_DEFAULT_WINDOWTOOL_WIDTHSCALEFACTOR  = 1.0;
const double SV_DEFAULT_WINDOWTOOL_HEIGHTSCALEFACTOR = 1.0;

const long SV_DEFAULT_POLARTRANSFORM_CENTER_X     = 200L;
const long SV_DEFAULT_POLARTRANSFORM_CENTER_Y     =  50L;
const long SV_DEFAULT_POLARTRANSFORM_START_RADIUS = 200L;
const long SV_DEFAULT_POLARTRANSFORM_END_RADIUS   = 100L;
const long SV_DEFAULT_POLARTRANSFORM_START_ANGLE  = 210L;
const long SV_DEFAULT_POLARTRANSFORM_END_ANGLE    = 330L;

// Acquisition Overlapping
const long SV_DEFAULT_MAX_OVERLAPPING           = 10;


const long SV_DEFAULT_CURRENT_RESULT_OUTPUT_TABLE_SIZE   = 4;

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
const DWORD SV_DOUBLE                   = ( SV_GET_BIT_NUMBER( double ) + static_cast< DWORD >( SVBufferDouble ) );



const double SV_PI						= 3.141592654;


const CString SV_DEFAULT_SIMPLE_ENCRYPT_KEY_STRING			( _T("PixelDepth") );

//******************************************************************************
//* TYPEDEF(S):
//******************************************************************************

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

// DLLEXPORT BOOL WINAPI SVIOGetResultOutputState( BYTE* PStateBuffer, DWORD* PDwNumber );
typedef BOOL ( CALLBACK* SVDLL_BOOL_BYTEPTR_DWORDPTR_FUNCTION ) ( BYTE*, DWORD* );


//******************************************************************************
//* CONSTANT VARIABLE(S):
//******************************************************************************

//******************************************************************************
//* VARIABLE(S):
//******************************************************************************

//******************************************************************************
//* EXTERN(S):
//******************************************************************************

//******************************************************************************
//* FUNCTION DECLARATION(S):
//******************************************************************************

////////////////////////////////////////////////////////////////////////////////
// .Title       : FormatLongerString2
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.11.1997 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void FormatLongerString2( CString& RString, UINT IDS, LPCTSTR LPSZFirst, LPCTSTR LPSZSecond );


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVSearchForMDIChildWnd
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :25.05.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
CMDIChildWnd* SVSearchForMDIChildWnd( CWnd* PStartWnd );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCalcLinePixelNumber
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
long SVCalcLinePixelNumber( long Width, long Height );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCalcLineXValue
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
long SVCalcLineXValue( long Pixel, long Width, long Height );

double SVCalcLineDXValue( double Pixel, double Width, double Height );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCalcLineXValue
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.06.1998 RO			First Implementation
//  :04.10.1999 RO			Changed to work with all Quadrants.
////////////////////////////////////////////////////////////////////////////////
long SVCalcLineXValue( long Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q4 );

double SVCalcLineDXValue( double Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q4 );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCalcLineYValue
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
long SVCalcLineYValue( long Pixel, long Width, long Height );

double SVCalcLineDYValue( double Pixel, double Width, double Height );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCalcLineYValue
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.06.1998 RO			First Implementation
//  :04.10.1999 RO			Changed to work with all Quadrants.
////////////////////////////////////////////////////////////////////////////////
long SVCalcLineYValue( long Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q2 );

double SVCalcLineDYValue( double Pixel, double Gradient, BOOL IsXGradient, BOOL IsQ1Q2 );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetDataTypeMin
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
double SVGetDataTypeMin( DWORD DataType );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetDataTypeMax
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
double SVGetDataTypeMax( DWORD DataType );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetDataTypeRange
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :30.06.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
double SVGetDataTypeRange( DWORD DataType );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVSimpleEncrypt
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :14.07.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVSimpleEncrypt( LPTSTR LPCHCode, int CodeLength, LPCTSTR LPCHKey );




////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCheckPathDir
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :13.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVCheckPathDir( LPCTSTR TStrPathName, BOOL BCreateIfNotExists );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDeleteFiles
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :13.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVDeleteFiles( LPCTSTR TStrPathName, BOOL BIncludeSubDirectories );




////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetAbsoluteFilePathName
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :14.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetAbsoluteFilePathName( CString& RStrOutputPath, LPCTSTR TStrAbsoluteInputPath, LPCTSTR TStrRelativeFileInputPath );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetRelativeFilePathName
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :14.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetRelativeFilePathName( CString& RStrOutputPath, LPCTSTR TStrAbsoluteInputPath, LPCTSTR TStrAbsoluteFileInputPath );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVFileExists
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :14.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVFileExists( LPCTSTR TStrFilePath );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetPathInformation
// -----------------------------------------------------------------------------
// .Description : Uses SVGetPathInfo enumeration...
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :17.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetPathInformation( CString& RStrOutput, LPCTSTR TStrFileInputPath, DWORD DwMask );



////////////////////////////////////////////////////////////////////////////////
// .Title       : SVWaitXTimesForSingleObject
// -----------------------------------------------------------------------------
// .Description : 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :28.10.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVWaitXTimesForSingleObject( HANDLE HWaitObject, DWORD DwWaitTime, int XTimes );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVSelectListCtrlItem
// -----------------------------------------------------------------------------
// .Description : Select an item of a List Ctrl.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :07.11.1998 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVSelectListCtrlItem( CListCtrl& RListControl, int Item );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetVersionString
// -----------------------------------------------------------------------------
// .Description : Use this function to convert an SVObserver Version to a string.
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.03.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetVersionString( CString& RSTRCurrentVersion, DWORD dwVersion );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVCompareNoCase
// -----------------------------------------------------------------------------
// .Description : Compares two T-Strings without regarding the case.
//				: If a T-String pointer is NULL, he is lesser. 
//				: If both T-String pointer are NULL, 0 is returned!
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.07.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
int SVCompareNoCase( LPCTSTR TStrString1, LPCTSTR TStrString2 );


////////////////////////////////////////////////////////////////////////////////
// .Title       : SVEscapeDoubleQuotes
// -----------------------------------------------------------------------------
// .Description : Places escape character before double quotes in a CString
//				: 
//				: 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :10.09.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
bool SVAddEscapeSpecialCharacters( CString& RString, bool bConvertCtrl );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVRemoveEscapedDoubleQuotes
// -----------------------------------------------------------------------------
// .Description : Removes any escape characters before double quotes in a CString
//				: 
//				: 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :10.09.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
bool SVRemoveEscapedSpecialCharacters( CString& RString, bool bConvertCtrl );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConvertToHexString
// -----------------------------------------------------------------------------
// .Description : Convert hex binary data to a hex Dump String
//				: 
//				: 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.09.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
void SVConvertToHexString( DWORD len, LPBYTE buff, CString& hexString );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVConvertFromHexString
// -----------------------------------------------------------------------------
// .Description : Convert hex Dump String to hex binary data
//				: 
//				: 
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :16.09.1999 SEJ			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVConvertFromHexString( DWORD &len, LPBYTE *buff, CString& hexString );


CMenu* SVFindMenuByCommand( CMenu *pMenu, int nID, BOOL bIncludeSubMenues, int& rMenuPos );
CMenu* SVFindMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues, int& rMenuPos );
CMenu* SVFindSubMenuByName( CMenu *pMenu, LPCTSTR szMenuString, BOOL bIncludeSubMenues );

BOOL SVCartesianToPolar( const POINT& RPoint, double& RRadius, double& RAngle );
BOOL SVPolarToCartesian( double Radius, double Angle, POINT& RPoint );
POINT SVPolarToCartesian( double Radius, double Angle );

//Methods that were once in SVCommRC
LPCTSTR SVRCGetSVCPathName();
void 	SVRCSetSVCPathName( LPCTSTR TStrPathName );
void	SVSetupLoadSECCommand( HWND HWindow, UINT Msg, WPARAM WParam, LPARAM LParam );



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


