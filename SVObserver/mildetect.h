//******************************************************************************
//* COPYRIGHT (c) 2002 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : MilDetect
//* .File Name       : $Workfile:   mildetect.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:14:00  $
//******************************************************************************

#ifndef MILDETECT_H__
#define MILDETECT_H__
#pragma once

#define MILDETECT_DISPLAY_MESSAGE

#include <mil.h>
#include <milcode.h>

#ifdef M_MIL_CURRENT_VERSION
#ifdef MILDETECT_DISPLAY_MESSAGE
	#pragma message ( "--- MIL Detect ---" )
#endif
// MIL detection
#ifdef M_MIL_CURRENT_INT_VERSION	// 8.x
	#ifdef MILDETECT_DISPLAY_MESSAGE
		#pragma message ( "MIL 8.x" )
	#endif
	#define SV_CURRENT_MIL_VERSION   M_MIL_CURRENT_INT_VERSION	// defined as 0x0800, so use hex for other mil versions as well
	// assume 8.x has all 7.5 patches included?
	#define SV_CURRENT_MIL_1394_PATCH 21
	#define SV_CURRENT_MIL_BAR_CODE_PATCH 3746	// patch dll version 7.50.374.6
	#define SV_MIL_75_PP1 1
#else
	#ifdef M_LOCAL	// 7.x
		#ifdef M_BOARD_SUB_REVISION	// 7.5
			#ifdef MILDETECT_DISPLAY_MESSAGE
				#pragma message ( "MIL 7.5" )
			#endif
			#define SV_CURRENT_MIL_VERSION 0x0750

			// detect Processing Pack 1
			#ifdef M_LICENSE_STR
				#define SV_MIL_75_PP1 1
				#pragma message ( "MIL 7.5 Processing Pack 1 = yes" )
			#else
				#define SV_MIL_75_PP1 0
				#pragma message ( "MIL 7.5 Processing Pack 1 = no" )
			#endif

			// now detect 1394 patch level
			#ifdef M_1394_BANDWIDTH	// patch 21
				#define SV_CURRENT_MIL_1394_PATCH 22	// really 21 but we never used 21
				#ifdef MILDETECT_DISPLAY_MESSAGE
					#pragma message ( "MIL 7.5 1394 Patch = 22" )
				#endif
			#else
				#ifdef M_MODEL	// patch 14/17
					#define SV_CURRENT_MIL_1394_PATCH 17
					#ifdef MILDETECT_DISPLAY_MESSAGE
						#pragma message ( "MIL 7.5 1394 Patch = 17" )
					#endif
				#else
					#define SV_CURRENT_MIL_1394_PATCH 0
					#ifdef MILDETECT_DISPLAY_MESSAGE
						#pragma message ( "MIL 7.5 1394 Patch = 0" )
					#endif
				#endif
			#endif

			// detect bar code patch
			#ifdef M_RSSCODE
				#define SV_CURRENT_MIL_BAR_CODE_PATCH 3746	// patch dll version 7.50.374.6
				#ifdef MILDETECT_DISPLAY_MESSAGE
					#pragma message ( "MIL 7.5 Bar Code Patch = yes" )
				#endif
			#else
				#define SV_CURRENT_MIL_BAR_CODE_PATCH 0		// non patch dll version 7.50.384.1 (!!!)
				#ifdef MILDETECT_DISPLAY_MESSAGE
					#pragma message ( "MIL 7.5 Bar Code Patch = no" )
				#endif
			#endif
		#else	// assume 7.1
			#ifdef MILDETECT_DISPLAY_MESSAGE
				#pragma message ( "MIL 7.1" )
			#endif
			#define SV_CURRENT_MIL_VERSION 0x0710
			#define SV_CURRENT_MIL_1394_PATCH 0
			#define SV_CURRENT_MIL_BAR_CODE_PATCH 0
			#define SV_MIL_75_PP1 0
		#endif
	#else	// asssume 6.x
		#ifdef M_SYS_MD_ID	// 6.1
			#ifdef MILDETECT_DISPLAY_MESSAGE
				#pragma message ( "MIL 6.1" )
			#endif
			#define SV_CURRENT_MIL_VERSION 0x0610
		#else
			#ifdef M_CURRENT_APPLICATION	// 6.01
				#ifdef MILDETECT_DISPLAY_MESSAGE
					#pragma message ( "MIL 6.01" )
				#endif
				#define SV_CURRENT_MIL_VERSION 0x0601
			#else
				#ifdef MILDETECT_DISPLAY_MESSAGE
					#pragma message ( "MIL 6.00" )
				#endif
				#define SV_CURRENT_MIL_VERSION 0x0600	// default to 6.0
			#endif
		#endif

		#define SV_CURRENT_MIL_1394_PATCH 0
		#define SV_CURR	ENT_MIL_BAR_CODE_PATCH 0
		#define SV_MIL_75_PP1 0
#endif // end detect 7.x / 6.x

#endif	// end detect 8.x


	//const long SV_CURRENT_MIL_VERSION		= 5100L;	// MIL 5.1
#else
	#define SV_CURRENT_MIL_VERSION 0
#endif

/*
#define SV_CURRENT_MIL_VERSION_STRING SV_MAKE_STRING(SV_CURRENT_MIL_VERSION)
#define SV_CURRENT_MIL_1394_PATCH_STRING SV_MAKE_STRING(SV_CURRENT_MIL_1394_PATCH)
#define SV_CURRENT_MIL_BAR_CODE_PATCH_STRING SV_MAKE_STRING(SV_CURRENT_MIL_BAR_CODE_PATCH)
#define SV_MAKE_STRING(x) #x

#pragma message (  "SV_CURRENT_MIL_VERSION = " SV_CURRENT_MIL_VERSION_STRING )
#pragma message ( "SV_CURRENT_MIL_1394_PATCH = " SV_CURRENT_MIL_1394_PATCH_STRING )
#pragma message ( "SV_CURRENT_MIL_BAR_CODE_PATCH = " SV_CURRENT_MIL_BAR_CODE_PATCH_STRING )
*/

#ifdef MILDETECT_DISPLAY_MESSAGE
	#pragma message ( "--- End MIL Detect ---" )
#endif

#endif // #ifndef MILDETECT_H__

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\mildetect.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:14:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   28 Jun 2005 17:21:52   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added #pragma message PP1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Jun 2005 10:01:54   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  498
 * SCR Title:  Add individual timeout capability to Bar Code analyzers
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added check for MIL 7.5 Processing Pack 1
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   17 Feb 2005 09:49:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  467
 * SCR Title:  Fix missing bar code problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   changed patch 21 to patch 22 (same header)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Dec 2004 09:51:02   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  467
 * SCR Title:  Fix missing bar code problem
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration (fixed PVCS Log header)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/