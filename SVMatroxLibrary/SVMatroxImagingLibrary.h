// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxImagingLibrary
// * .File Name       : $Workfile:   SVMatroxImagingLibrary.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:21:34  $
// ******************************************************************************

#ifndef SV_MATROX_IMAGING_LIBRARY_H
#define SV_MATROX_IMAGING_LIBRARY_H

#define SV_DESIRED_MIL_VERSION 0x0900

#if SV_DESIRED_MIL_VERSION == 0x0900
	#define M_MIL_USE_SAFE_TYPE 0
#endif

#include <mil.h>

//#define USE_TRY_BLOCKS

#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/mil" )
#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/milblob" )
#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/milcode" )
#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/milim" )
#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/milocr" )
#pragma comment( lib, "C:/Program Files/Matrox Imaging/Mil/LIB/milpat" )

#ifdef M_MIL_CURRENT_ASCII_VERSION
 	#pragma message ( "MIL Version " M_MIL_CURRENT_ASCII_VERSION )
#else
 	#pragma message ( "MIL Version UNKNOWN" )
#endif

#ifdef M_MIL_CURRENT_INT_VERSION
	#ifdef SV_CURRENT_MIL_VERSION
		#undef SV_CURRENT_MIL_VERSION
	#endif

	#define SV_CURRENT_MIL_VERSION M_MIL_CURRENT_INT_VERSION
#endif

#ifndef SV_CURRENT_MIL_VERSION
	#define SV_CURRENT_MIL_VERSION 0x0000
#endif

#if SV_CURRENT_MIL_VERSION != SV_DESIRED_MIL_VERSION
	#error ****** INVALID MIL VERSION : Current version does not match desired version.
#endif

#endif // SV_MATROX_IMAGING_LIBRARY_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxImagingLibrary.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:21:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Jul 2012 12:45:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow gige to be compiled with both MIL 8.0 and 9.0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jun 2012 14:06:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code needed to implement MIL 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Nov 2008 14:42:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  641
 * SCR Title:  BoundsChecker results
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Fixed bugs in Font Object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:20:02   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
