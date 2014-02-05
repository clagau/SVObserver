//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGlobal
//* .File Name       : $Workfile:   SVGlobal.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   04 Feb 2014 15:18:50  $
//******************************************************************************

#ifndef SVGLOBAL_H
#define SVGLOBAL_H

#include <map>
#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVStatusLibrary/SVException.h"
#include "SVMessage/SVMessage.h"

typedef std::pair<CString, CString> StrStrPair;
typedef std::map<int, StrStrPair > IntStrPairMap;
typedef std::vector<StrStrPair> StringPairVect;

class SVImageClass;
class SVImageInfoClass;
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
//#define SV_IO_TEST_SUPPORT       // IO Test Support
#define SV_NO_MIL_ERROR_MESSAGES   // MIL Error Message Support
//#define SV_DEV_STATE             // Programming State

#define SV_DEFAULT_REFERENCETOOL_LEFT      10L
#define SV_DEFAULT_REFERENCETOOL_TOP       10L
#define SV_DEFAULT_REFERENCETOOL_WIDTH    100L
#define SV_DEFAULT_REFERENCETOOL_HEIGHT   100L
#define SV_DEFAULT_REFERENCETOOL_CONTRAST 20.0

//******************************************************************************
//* MACRO(S):
//******************************************************************************
#define SV_SERIALIZE_GUID_AND_INIT( XGUID, XARCHIVE )	\
		{	\
			if( ( XARCHIVE ).IsStoring() ) \
			{ \
				( XARCHIVE ) << ( XGUID ).Data1 << ( XGUID ).Data2 << ( XGUID ).Data3 \
							 << ( XGUID ).Data4[0] << ( XGUID ).Data4[1] << ( XGUID ).Data4[2] << ( XGUID ).Data4[3] \
							 << ( XGUID ).Data4[4] << ( XGUID ).Data4[5] << ( XGUID ).Data4[6] << ( XGUID ).Data4[7]; \
			} \
			else \
			{ \
				SVObjectManagerClass::Instance().CloseUniqueObjectID( this ); \
				\
				( XARCHIVE ) >> ( XGUID ).Data1 >> ( XGUID ).Data2 >> ( XGUID ).Data3 \
							 >> ( XGUID ).Data4[0] >> ( XGUID ).Data4[1] >> ( XGUID ).Data4[2] >> ( XGUID ).Data4[3] \
							 >> ( XGUID ).Data4[4] >> ( XGUID ).Data4[5] >> ( XGUID).Data4[6] >> ( XGUID ).Data4[7]; \
				\
				SVObjectManagerClass::Instance().OpenUniqueObjectID( this ); \
			} \
		}


#define SV_SERIALIZE_GUID( XGUID, XARCHIVE )	\
 		{	\
			if( ( XARCHIVE ).IsStoring() ) \
			{ \
				( XARCHIVE ) << ( XGUID ).Data1 << ( XGUID ).Data2 << ( XGUID ).Data3 \
							 << ( XGUID ).Data4[0] << ( XGUID ).Data4[1] << ( XGUID ).Data4[2] << ( XGUID ).Data4[3] \
							 << ( XGUID ).Data4[4] << ( XGUID ).Data4[5] << ( XGUID ).Data4[6] << ( XGUID ).Data4[7]; \
			} \
			else \
			{ \
				( XARCHIVE ) >> ( XGUID ).Data1 >> ( XGUID ).Data2 >> ( XGUID ).Data3 \
							 >> ( XGUID ).Data4[0] >> ( XGUID ).Data4[1] >> ( XGUID ).Data4[2] >> ( XGUID ).Data4[3] \
							 >> ( XGUID ).Data4[4] >> ( XGUID ).Data4[5] >> ( XGUID).Data4[6] >> ( XGUID ).Data4[7]; \
			} \
		}

////////////////////////////////////////////////////////////////////////////////
// Global Helpers:
////////////////////////////////////////////////////////////////////////////////

#define SV_TOLERANCE( IV, SV, OT, UT)				( ( IV ) >= ( SV ) + ( UT ) && ( IV ) <= ( SV ) + ( OT ) )

#define SV_IS_PQ_READY( X )							( ( ( X ) != NULL ) ? ( X )->IsStarted() : FALSE )

#define SV_POINT_TO_DWORD( P )						( ( ( ( DWORD ) ( P ).y ) << 16 ) | ( ( DWORD ) ( P ).x ) )

#define SV_VALIDATE_READ_WRITE_OBJECT( P, Type, BReadWrite )	( ( ( P ) != NULL ) && AfxIsValidAddress( ( P ), sizeof( Type ), ( BReadWrite ) ) )

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


// GUID Messagebox
#define SV_GUID_TO_MESSAGE( XGUID )	\
		{	\
			if( sizeof( GUID ) == sizeof( XGUID ) )	 \
			{	 \
				TCHAR szBuf[ _MAX_PATH ];	\
				_stprintf( szBuf, "{%x-%x-%x-%x %x %x %x %x %x %x %x}", \
						   ( XGUID ).Data1, ( XGUID ).Data2, ( XGUID ).Data3, \
						   ( XGUID ).Data4[0], ( XGUID ).Data4[1], ( XGUID ).Data4[2], ( XGUID ).Data4[3], \
						   ( XGUID ).Data4[4], ( XGUID ).Data4[5], ( XGUID ).Data4[6], ( XGUID ).Data4[7] ); \
				AfxMessageBox( szBuf );	\
			}	 \
		}
#define SV_GET_PIXELDEPTH( XMILPIXELDEPTH )		( 1 << ( ( XMILPIXELDEPTH ) & SVBufferSize ) )


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
// Special MessageBox Macros:
////////////////////////////////////////////////////////////////////////////////
#ifdef SV_DEV_STATE
	#define SV_MESSAGE_OK( x, t )			::MessageBox( NULL, x, t, MB_OK )
	#define SV_MESSAGE_WARNING( x, t )		::MessageBox( NULL, x, t, MB_ICONWARNING )
	#define SV_MESSAGE_STOP( x, t )			::MessageBox( NULL, x, t, MB_ICONSTOP )
#else
	#define SV_MESSAGE_OK( x, t )			
	#define SV_MESSAGE_WARNING( x, t )
	#define SV_MESSAGE_STOP( x, t )
#endif

// Formatted Messageboxes 
#define SV_FORMAT_MESSAGE( T, P )	\
{	\
	TCHAR text[ _MAX_PATH ];	\
	_stprintf( text, _T( T ), _T( P ) );	\
	AfxMessageBox( text );	\
};

#define SV_FORMAT_MESSAGE_5( T, P1, P2, P3, P4, P5 ) \
{	\
	TCHAR text[ _MAX_PATH ];	\
	_stprintf( text, _T( T ), _T( P1 ), _T( P2 ), _T( P3 ), _T( P4 ), _T( P5 ) );	\
	AfxMessageBox( text );	\
};

#define SV_FORMAT_DEBUG_OUTPUT( T, P )	\
{	\
	TCHAR text[ _MAX_PATH ];	\
	_stprintf( text, _T( T ), _T( P ) );	\
	OutputDebugString( text );	\
};

#define SV_FORMAT_MESSAGE_FRB( T, P )
#define SV_FORMAT_MESSAGE_5_FRB( T, P1, P2, P3, P4, P5 )

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
#define SVR_MUNICH
#define SVR_HARRISBURG

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

const long SV_DEFAULT_POLARTRANSFORM_CENTER_X     = 200L;
const long SV_DEFAULT_POLARTRANSFORM_CENTER_Y     =  50L;
const long SV_DEFAULT_POLARTRANSFORM_START_RADIUS = 200L;
const long SV_DEFAULT_POLARTRANSFORM_END_RADIUS   = 100L;
const long SV_DEFAULT_POLARTRANSFORM_START_ANGLE  = 210L;
const long SV_DEFAULT_POLARTRANSFORM_END_ANGLE    = 330L;

const long SV_DEFAULT_GAGETOOL_LEFT          =  10L;
const long SV_DEFAULT_GAGETOOL_TOP           =  10L;
const long SV_DEFAULT_GAGETOOL_WIDTH         = 150L;
const long SV_DEFAULT_GAGETOOL_HEIGHT        =  50L;

const long SV_DEFAULT_PROFILETOOL_LEFT       =  10L;
const long SV_DEFAULT_PROFILETOOL_TOP        =  10L;
const long SV_DEFAULT_PROFILETOOL_WIDTH      = 150L;
const long SV_DEFAULT_PROFILETOOL_HEIGHT     = 100L;

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

extern __declspec( thread ) int tls_ObjectIndex;

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

HBITMAP SVMilBufferToBitmap2( SVImageClass *pImage );

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
// .Title       : SVGetPLCBaseAddress
// -----------------------------------------------------------------------------
// .Description : Separates and returns PLC Address in RPLCBase, if successfully
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.07.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
BOOL SVGetPLCBaseAddress( CString& RPLCBase, const CString& RItem );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVGetPLCOffset
// -----------------------------------------------------------------------------
// .Description : Returns Offset of PLC Address, if successfully;
//				: Otherwise -1 !
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.07.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
int SVGetPLCOffset( LPCTSTR TStrPLCAddress );

////////////////////////////////////////////////////////////////////////////////
// .Title       : SVComparePLCAddresses
// -----------------------------------------------------------------------------
// .Description : Compares two PLC Addresses without regarding the case.
//				: If a Address1 pointer is NULL, he is lesser. 
//				: If both Address pointer are NULL, 0 is returned!
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :19.07.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
int SVComparePLCAddresses( LPCTSTR TStrAddress1, LPCTSTR TStrAddress2 );


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


extern TCHAR	SVRCCurrentSVCPathName[];

enum svModeEnum
{
	SVIM_MODE_UNKNOWN = 0,
	SVIM_MODE_ONLINE,
	SVIM_MODE_OFFLINE,
	SVIM_MODE_REGRESSION,
	SVIM_MODE_TEST,
	SVIM_MODE_EDIT,
	SVIM_MODE_EDIT_MOVE,
	SVIM_MODE_CHANGING = 100,
	SVIM_MODE_REMOVE = 0x8000,
};

/**************************************************************************/
/* INCLUDE CONTROL:
/**************************************************************************/
#endif	// SVGLOBAL_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVGlobal.h_v  $
 * 
 *    Rev 1.5   04 Feb 2014 15:18:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  880
 * SCR Title:  Remove .SEC
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Removed dead code and unused SV_DEFAULT_VIEW_REFRESH_RATE and NUM_INTENSITIES.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   11 Nov 2013 07:12:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  872
 * SCR Title:  Add Archive Tool Headers to Archive File
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Typedef for string pair vector
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   01 Oct 2013 14:19:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   23 Jul 2013 17:53:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Added include for std map
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Jun 2013 15:20:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.28   22 May 2013 09:18:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed dead code (SVReadTimeStamp)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:43:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.27   18 Sep 2012 18:35:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  781
 * SCR Title:  Update SVObserver Version Number for Esok Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the creation and management for the version information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.26   26 Sep 2011 19:21:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed unused defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.25   01 Sep 2011 10:56:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  737
 * SCR Title:  Fix serialization bug associated with control characters in strings
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified SVAddEscapeCharacters and SVRemoveEscapeCharacters to convert control characters.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.24   27 Jan 2011 11:06:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.23   08 Dec 2010 08:05:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.22   04 Nov 2010 13:35:34   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with image processing and display image processing classes and associated includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.21   01 Jun 2010 10:34:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to remove redundent methodologies and fix missing or incorrect calling functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.20   03 Sep 2009 10:07:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.19   30 Jul 2009 11:39:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.18   24 Jul 2007 13:06:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17.1.0   14 Jul 2009 13:27:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to fix data manager issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.17   28 Jul 2005 16:11:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated GUI Editing Flag to block mode changes from SIAC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.16   19 Jul 2005 14:32:00   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  464
 * SCR Title:  Add array indexing for value objects
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   SVAddEscapeSpecialCharacters and SVRemoveEscapeSpecialCharacters now return bool indicating if the string had been modified
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.15   21 Jun 2005 13:09:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Enum for new SVIM Modes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.14   16 Feb 2005 14:29:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  440
 * SCR Title:  Create Internal Tool Object to Managing Tool Extents and Result Extents
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added enums
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.13   28 Dec 2004 08:29:14   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  467
 * SCR Title:  Fix missing bar code problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   removed SV_MIL_CURRENT_VERSION define to mildetect.h
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.12   25 Jun 2003 09:12:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  371
 * SCR Title:  Remove DOS Focus COM support
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the CommPort support for DOS Focus.  Moved methods from the SVCommRC project to the Globals
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.11   22 Apr 2003 09:59:40   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.10   29 Jan 2003 16:08:26   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated global function SVMilBufferToBitmap2 to use the new image circle buffer methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.9   14 Jan 2003 09:07:16   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  306
 * SCR Title:  Implement the ability for the SIAC to remotely setup the RGB LUT for a camera
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Cosmetic changes
 * #define -> const typename
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.8   19 Nov 2002 13:12:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed System calls to SVImageProcessingClass calls
 * 
 * Cosmetic change: TheObjectManager -> TheSVObjectManager
 * 
 * Added SVDIGITALIOxxx macros
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.7   08 Nov 2001 13:16:36   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  255
 * SCR Title:  Add Sub-pixel resolution to Profile and Line Gage Tool Analyzers
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added two new global SVCalcLineDXValue and SVCalcLineDYValue with two new overloads for those functions to calculate sub-pixel resolution.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.6   30 Oct 2001 10:27:42   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  248
 * SCR Title:  Notify user when loading a configuration from a newer version of SVObserver
 * Checked in by:  rSchock;  Rosco Schock
 * Change Description:  
 *   Added new function SVGetVersionString that returns a text representation of a SVObserver version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   19 Apr 2001 14:16:18   Steve
 * Project:  SVObserver
 * Change Request (SCR) nbr:  196
 * SCR Title:  Restructure Scripted Load/Save Procedures Functional Requirement
 * Checked in by:  Steve;  Stephen E. Steffan
 * Change Description:  
 *   Header file changes due to include file restructuring.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   06 Feb 2001 13:26:56   cSchmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  191
 * SCR Title:  Restructure File Management
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified source code to use new file management classes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   Mar 01 2000 12:03:26   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  113
 * SCR Title:  Integrate new Polar Unwrap Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added new global functions:
 * SVPolarToCartesian, SVCartesianToPolar
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   Feb 24 2000 11:38:20   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  111
 * SCR Title:  Integrate Conditional Tool Set/Tool Drawing
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added SVFindMenuByCommand, SVFindMenuByName, and SVFindSubMenuByName functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   Jan 03 2000 10:37:34   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  73
 * SCR Title:  Mask Editor Mil 5.0 to Mil 6.0 repairs.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changing from MIL 5.0 to MIL 6.0 library requires some changes to accomodate the requirement for Windows HDC (device context) for an image buffer.  This is required for image masking functions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:46:06   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   Nov 02 1999 08:59:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  55
 * SCR Title:  Version 3.00 Beta 16 Versioning
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes required for v3.00 Beta 15 to v3.00 Beta 16 change.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   Nov 01 1999 17:04:38   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Remove sv_strncpy() function. Not needed by OCR anymore.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   Oct 15 1999 13:41:52   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  15
 * SCR Title:  Integrate archive tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Repair buts in image saving part of archives.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   Oct 04 1999 10:11:06   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  21
 * SCR Title:  Port Gage Tool code from program version 2.42.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changed CalcX/Y Functions to support 360 Degree mesurement.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   Sep 24 1999 09:47:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  4
 * SCR Title:  Add "Load image" option
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   No Change - Checked out in error.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   16 Sep 1999 11:01:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added two methods:
 * SVConvertToHexString and
 * SVConvertFromHexString
 * for support of converting a serialize buffer to an Object Script
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Sep 1999 17:30:24   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  9
 * SCR Title:  Integr. a common object interface to handle and to identify obj. in-/outputs
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added 2 new functions:
 * SVEscapeDoubleQuotes and
 * SVRemoveEscapedDoubleQuotes
 * to support strings with embedded quoptes in the Object
 * Script Parser.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 23 1999 10:08:38   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised to support Beta Versioning.
 * Added 3 new methods, SVCompareNoCase, SVGetPLCBaseAddress, and SVGetPLCOffset.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/