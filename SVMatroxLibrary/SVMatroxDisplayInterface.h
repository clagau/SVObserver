// ******************************************************************************
// * COPYRIGHT (c) 2007 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplayInterface
// * .File Name       : $Workfile:   SVMatroxDisplayInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:54  $
// ******************************************************************************

#ifndef SV_MATROX_DISPLAY_INTERFACE_H
#define SV_MATROX_DISPLAY_INTERFACE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMatroxLibrary/SVMatroxEnums.h"
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"

#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxDisplay.h"
#include "SVMatroxLibrary/SVMatroxDisplayBuffer.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxBufferInterface.h"


/**
@SVObjectName Matrox Display Interface

@SVObjectOverview The interface class allows you to display images and manipulate their display. The interface class offers many display effects, such as LUTs, and panning and zooming

@SVObjectOperations The Create function calls the Matrox MdispAlloc to create a SVMatroxDisplay. This function allocates a display so that it can be used by subsequent display functions.
The Destroy function calls the Matrox MdispFree to free a SVMatroxDisplay that was previously created using the create function.
The Set functions use the Matrox MdispControl to set display settings. There are three overloads for the set functions with the value being double, long and SVMatroxString.
The Get functions use the Matrox MdispInquire to get display settings. There are three overloads for the get functions with the value being double, long and SVMatroxString.
The Lut function uses the Matrox MdispLut. This function associates a LUT buffer with the specified display. If and when the display is selected, the change required to produce the display (LUT) effect occurs. For windowed displays, MIL indirectly programs the physical output LUTs through the use of a Windows palette. MIL checks the target display to determine whether or not a LUT is supported. If not, an error is generated. 
The Pan function uses the Matrox MdispPan. This function associates pan and scroll values with the specified display. When an image buffer is selected for display, it will be panned and scrolled on the display according to these values. 
The Select function uses the Matrox MdispSelect. This function outputs the content of the specified image buffer to the specified MIL display. You can only display one buffer at a time on a specific display. 
The SelectWindow function uses the Matrox MdispSelectWindow. This function displays the specified image buffer contents in the specified user window, using the specified SVMatroxDisplay.
The Zoom function uses the Matrox MdispZoom. This function associates a zoom factor with the specified display. When an image buffer is selected for display, it will be zoomed according to this factor. The image buffer will be displayed starting from its top-left corner, unless it has been panned and/or scrolled, using Pan().
*/
class SVMatroxDisplayInterface
{
public:
	typedef SVMatroxApplicationInterface::SVStatusCode SVStatusCode;

	SVMatroxDisplayInterface();

	virtual ~SVMatroxDisplayInterface();

	// This function replaces MdispAlloc
	static SVStatusCode Create( SVMatroxDisplay& p_rDispId);
	// This function replaces MbufAlloc with display flag.
	static SVStatusCode Create( SVMatroxDisplayBuffer& p_rDispBufId, SVMatroxBufferCreateStruct p_CreateStruct);
	// This function creates a SVMatroxDisplayBuffer from the attributes from a SVMatroxBuffer.
	static SVStatusCode Create( SVMatroxDisplayBuffer& p_rNewBuffer, const SVMatroxBuffer& p_CreateFrom );
	// This function replaces MdispFree
	static SVStatusCode Destroy( SVMatroxDisplay& p_rDispId);
	static SVStatusCode Destroy( SVMatroxDisplayBuffer& p_rDispId );

	// This function replaces MdispInquire
	static SVStatusCode Get( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, double& p_rdData) ;
	static SVStatusCode Get( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, long& p_rlData) ;
	static SVStatusCode Get( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, SVMatroxBuffer& p_rBuffer) ;

	// This function replaces MdispControl
	static SVStatusCode Set( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const double& p_dValue);
	static SVStatusCode Set( const SVMatroxDisplay& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const long& p_lValue);

	// This function replaces MdispLut
	static SVStatusCode Lut( const SVMatroxDisplay& p_rDispId, const SVMatroxBuffer& p_rLutBufId);
	// This function clears the LUT
	static SVStatusCode LutClear( const SVMatroxDisplay& p_rDispId);

	// This function replaces MdispPan
	static SVStatusCode Pan( const SVMatroxDisplay& p_rDispId, const double& p_dXOffset, const double& p_dYOffset);
	// This function replaces MdispSelect
	static SVStatusCode Select( const SVMatroxDisplay& p_rDispId, const SVMatroxBuffer& p_rImageId);

	// This function replaces MdispSelect
	static SVStatusCode Deselect( const SVMatroxDisplay& p_rDispId);

	static SVStatusCode GetBitmapInfo( LPBITMAPINFO& p_rpBitmapInfo, const SVMatroxDisplayBuffer& p_rBuffer );
	static SVStatusCode GetHostAddress( LPVOID p_rpHostAddress, const SVMatroxDisplayBuffer& p_rBuffer );

	
	// This function replaces MdispSelectWindow
	static SVStatusCode SelectWindow( const SVMatroxDisplay& p_rDispId, const SVMatroxDisplayBuffer& p_rImageId, const HWND& p_hClientWindowHandle );
	// This function replaces MdispZoom
	static SVStatusCode Zoom( const SVMatroxDisplay& p_rDispId, const double& p_dXFactor, const double& p_dYFactor );
public:
	static long Convert2MatroxType( SVDisplayCntrlEnum p_eDisp) ;

};

#endif // SV_MATROX_DISPLAY_INTERFACE_H

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxDisplayInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:12:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:20:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
