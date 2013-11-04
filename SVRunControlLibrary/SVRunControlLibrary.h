//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVRunControlLibrary
//* .File Name       : $Workfile:   SVRunControlLibrary.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 16:20:30  $
//******************************************************************************

#ifndef SVRUNCONTROLLIBRARY_H
#define SVRUNCONTROLLIBRARY_H

// Default drawing color settings...
const COLORREF SV_DEFAULT_TEXT_COLOR                        = RGB( 0, 0, 0 );       // Black
const COLORREF SV_DEFAULT_INACTIVE_COLOR                    = RGB( 0, 0, 255 );     // Blue
const COLORREF SV_DEFAULT_FAILED_COLOR                      = RGB( 255, 0, 0 );     // Red
const COLORREF SV_DEFAULT_WARNED_COLOR                      = RGB( 200, 200, 0 );   // Yellow
const COLORREF SV_DEFAULT_GOOD_COLOR                        = RGB( 0, 200, 0 );     // Green
const COLORREF SV_DEFAULT_SUB_FUNCTION_COLOR_1              = RGB( 255, 0, 255 );   // Magenta
const COLORREF SV_DEFAULT_SUB_FUNCTION_COLOR_2              = RGB( 0, 255, 255 );   // Cyan
const COLORREF SV_DEFAULT_WHITE_COLOR                       = RGB( 255, 255, 255 ); // White
const COLORREF SV_DEFAULT_BLACK_COLOR                       = RGB( 0, 0, 0 );       // Black
const COLORREF SV_DEFAULT_DISABLED_COLOR                    = RGB( 128, 128, 128 ); // Grey

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVRunControlLibrary\SVRunControlLibrary.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:20:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:38:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   File moved to here to allow for multiple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
