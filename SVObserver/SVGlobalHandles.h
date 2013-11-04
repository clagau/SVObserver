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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVGlobalHandles.h_v  $
 * 
 *    Rev 1.1   11 Jun 2013 15:26:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 017.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   21 May 2013 10:00:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  836
 * SCR Title:  Fix loading SEC bug
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved SVObserverEnums.h to SCConfigurationLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:43:38   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jan 2011 11:06:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   24 Jul 2007 13:06:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Upgrade Matrox Imaging Library to 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Jul 2003 09:16:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  322
 * SCR Title:  Add Additional Digital IO Resources to SVObserver
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved Coreco to DLL
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 Apr 2003 09:59:44   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jan 2003 10:50:18   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved revisions out of branch and back to main line.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.7   29 Jan 2003 09:22:30   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed SVImageBufferHandleStruct and associated enums from file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.6   19 Nov 2002 13:14:48   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Added bit enums
 *             Added to SVImageBufferHandleStruct
 *             Added SVBarCodeBufferHandleStruct
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.5   18 Sep 2001 12:57:58   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  195
 * SCR Title:  Fix Random NAKs
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed SVDigitizerHandleStruct and SVSaperaCamStruct.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.4   12 Jun 2001 14:42:18   ed
 * Project:  SVObserver
 * Change Request (SCR) nbr:  200
 * SCR Title:  Update code to implement SVIM COM Server
 * Checked in by:  Ed;  Ed Chobanoff
 * Change Description:  
 *   Added include for mil.h.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.3   19 May 2000 10:12:04   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  146
 * SCR Title:  Fix OCR Gray Scale Analyzer Configuration Dialog
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   This change was in response to SCR146.  These updates fix most of the problems with the Gray Scale OCR Dialogs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.2   16 May 2000 14:41:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.1   Sep 10 1999 17:04:56   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  34
 * SCR Title:  Add Build Reference Tool
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Added DataBufferhandle structure.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.0   Aug 18 1999 15:44:04   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  8
 * SCR Title:  Prepare for QUAD ( Multi-Dig. ) Systems and integrate VIPER RGB acquisition.
 * Checked in by:  Robert Massinger
 * Change Description:  
 *   Changes required for Viper (RGB and Quad) support.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
