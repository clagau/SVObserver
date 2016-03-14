//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGlobalHandles
//* .File Name       : $Workfile:   SVGlobalHandles.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   11 Jun 2013 15:26:10  $
//******************************************************************************

#ifndef SVGLOBALHANDLES_H
#define SVGLOBALHANDLES_H

#ifndef SV_INLINE
#define SV_INLINE __forceinline
#endif

// These should go in svglobal.h but had compilation problems
  // Bit masking constants

#include "SVMatroxLibrary/SVMatroxLibrary.h"
#include "SVConfigurationLibrary/SVObserverEnums.h"

////////////////////////////////////////////////////////////////////////////////
// NOTE:
//		 Change from SAPERA 1.20 and earlier, 1.30 Beta 1
//		 and earlier 
//
//		 to 2.00 Beta ? ( and later ):
//	
//		 FILE: capidef.h
//		 Coreco's Log Note:	   Revision 1.6  1999/05/25 14:45:08  Corjea
//								Added CORDATA_XXX format definitions
//								Added CORHANDLE_XXX macros.
//						---->	Changed CORHANDLE definition
//
//	CORHANDLE definition before:
//
//	typedef struct
//	{
//		UINT32	  location;
//		CORADDRESS address;
//		UINT32     type;
//	} CORHANDLE, *PCORHANDLE;
//
//
//	CORHANDLE definition after:
//
//  typedef struct _CORHANDLE
//	{
//		UINT32	  location;
//		CORADDRESS address;
//		UINT32     type;
//		struct _CORHANDLE * localAddress;
//	} _CORHANDLE, *CORHANDLE, **PCORHANDLE;
//
//
////////////////////////////////////////////////////////////////////////////////
//
//	Therefore we have to change some structures and the way we handled these
//	CORHANDLEs.
//	To identify the kind of CORHANDLE, structure or pointer to structure, we
//	define SV_SAPERA_VERSION 0x0001ffff as the last one which uses the old
//	CORHANDLE structure version. Our SAPERA version numbering system works in
//	the same way as the SVObserver version numbering system:
//	Copied from SVObserver.h...
	// Version numbering code:
	//
	// DwCurrentVersion, Type DWORD
	// Meaning:		0x00mmssbb => Version m.s  - Subversion b 
	// EXCEPTION:	if m == 0 then s = b and b = 0 !!! 
	//					No Subversion number in not releasable
	//					versions!
	//	e.g.
	//				0x00000002 => Version 0.2  - Subversion 00 ( Subversion 'a' )
	//				0x00010000 => Version 1.0  - Subversion 00 ( Subversion 'a' )
	//		        0x00011219 => Version 1.18 - Subversion 25 ( Subversion 'z' )
	//				0x00020001 => Version 2.0  - Subversion 01 ( Subversion 'b' )
//
//
////////////////////////////////////////////////////////////////////////////////

//#define SV_SAPERA_VERSION		0x00011405	// 1.20 Beta 5
//#define SV_SAPERA_VERSION		0x00011e01	// 1.30 Beta 1
#define SV_SAPERA_VERSION		0x00020001	// 2.00 Beta 1

#if ( SV_SAPERA_VERSION >= 0x00020000 )
#define SV_COR_CALLBACK				CCONV
#define SV_CORHANDLE_TYPE( X )		( ( X ) ? CORHANDLE_TYPE( X ) : 0 )
#define SV_CORHANDLE_INFO( X )		( ( X ) ? CORHANDLE_INFO( X ) : 0 )
#define SV_IS_VALID_CORHANDLE( X )	( SV_CORHANDLE_INFO( X ) != 0 )

#else

#pragma message ( "Warning! Using SV_SAPERA_VERSION <= 0x00020000" )
#define SV_COR_CALLBACK				
#define SV_CORHANDLE_TYPE( X )		CORHANDLE_TYPE( X )
#define SV_CORHANDLE_INFO( X )		CORHANDLE_INFO( X )
#define SV_IS_VALID_CORHANDLE( X )	( SV_CORHANDLE_INFO( X ) != 0 )

#endif


















////////////////////////////////////////////////////////////////////////////////
// .Title       : SVDataBufferHandleStruct 
// -----------------------------------------------------------------------------
// .Description : Is used as a multi system data buffer handle container
////////////////////////////////////////////////////////////////////////////////
// .History
//	 Date		Author		Comment                                       
//  :09.09.1999 RO			First Implementation
////////////////////////////////////////////////////////////////////////////////
struct SVDataBufferHandleStruct
{
	SVDataBufferHandleStruct()
	{
		DwSize		= sizeof( SVDataBufferHandleStruct );
		DwMask		= SV_NOT_SET;
		DW			= 0;			
		COR         = NULL;
		PHostBuffer = NULL;
	};

	DWORD		DwSize;			// Size of this structure...
	DWORD		DwMask;			// Not yet used
	DWORD		DW;				// Not yet used
	SVMatroxImageResult	milResult;	// MIL Buffer Handle, if available...
	SVHANDLE	COR;			// Digitizer Buffer Handle, if available...
	LPVOID		PHostBuffer;	// Buffer Pointer ( Process Address Space ), if available...
};


struct SVBarCodeBufferHandleStruct
{
	SVBarCodeBufferHandleStruct()
	{
		DwSize		= sizeof( SVBarCodeBufferHandleStruct );
		DwMask		= SV_NOT_SET;
		DW			= 0;			
		COR         = NULL;
	};

	DWORD		DwSize;			// Size of this structure...
	DWORD		DwMask;			// Not yet used
	DWORD		DW;				// Not yet used
	SVMatroxBarCode	milBarCode;		// MIL Buffer Handle, if available...
	SVHANDLE	COR;			// Digitizer Buffer Handle, if available...
};














//******************************************************************************
//* INCLUDE CONTROL:
//******************************************************************************
#endif //SVGLOBALHANDLES_H

//** EOF **

