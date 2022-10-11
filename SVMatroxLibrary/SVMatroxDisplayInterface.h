// ******************************************************************************
// * COPYRIGHT (c) 2007 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDisplayInterface
// * .File Name       : $Workfile:   SVMatroxDisplayInterface.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 15:12:54  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "Definitions\SVMatroxSimpleEnums.h"
#include "SVMatroxBufferCreateStruct.h"
#include "SVMatroxBuffer.h"
#include "SVMatroxBufferInterface.h"
#pragma endregion Includes


/**
@SVObjectName Matrox Display Interface

@SVObjectOverview The interface class allows you to display images and manipulate their display. The interface class offers many display effects, such as LUTs, and panning and zooming

@SVObjectOperations The Create function calls the Matrox MdispAlloc to create a SVMatroxIdentifier. This function allocates a display so that it can be used by subsequent display functions.
The Destroy function calls the Matrox MdispFree to free a SVMatroxIdentifier that was previously created using the create function.
The Set functions use the Matrox MdispControl to set display settings. There are three overloads for the set functions with the value being double, long and SVMatroxString.
The Get functions use the Matrox MdispInquire to get display settings. There are three overloads for the get functions with the value being double, long and SVMatroxString.
The Lut function uses the Matrox MdispLut. This function associates a LUT buffer with the specified display. If and when the display is selected, the change required to produce the display (LUT) effect occurs. For windowed displays, MIL indirectly programs the physical output LUTs through the use of a Windows palette. MIL checks the target display to determine whether or not a LUT is supported. If not, an error is generated. 
The Pan function uses the Matrox MdispPan. This function associates pan and scroll values with the specified display. When an image buffer is selected for display, it will be panned and scrolled on the display according to these values. 
The Select function uses the Matrox MdispSelect. This function outputs the content of the specified image buffer to the specified MIL display. You can only display one buffer at a time on a specific display. 
The SelectWindow function uses the Matrox MdispSelectWindow. This function displays the specified image buffer contents in the specified user window, using the specified SVMatroxIdentifier.
The Zoom function uses the Matrox MdispZoom. This function associates a zoom factor with the specified display. When an image buffer is selected for display, it will be zoomed according to this factor. The image buffer will be displayed starting from its top-left corner, unless it has been panned and/or scrolled, using Pan().
*/
class SVMatroxDisplayInterface
{
public:
	SVMatroxDisplayInterface();

	virtual ~SVMatroxDisplayInterface();

	// This function replaces MdispAlloc
	static HRESULT CreateDisplay(__int64& p_rDispId);
	// This function replaces MbufAlloc with display flag.
	static HRESULT Create( SVMatroxBuffer& p_rDispBufId, SVMatroxBufferCreateStruct p_CreateStruct);
	// This function creates a SVMatroxBuffer from the attributes from a SVMatroxBuffer.
	static HRESULT Create( SVMatroxBuffer& p_rNewBuffer, const SVMatroxBuffer& p_CreateFrom );
	// This function replaces MdispFree
	static HRESULT DestroyDisplay(__int64& p_rDispId);
	static HRESULT Destroy( SVMatroxBuffer& p_rDispId );

	// This function replaces MdispInquire
	static HRESULT Get( const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, double& p_rdData) ;
	static HRESULT Get( const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, long& p_rlData) ;

	// This function replaces MdispControl
	static HRESULT Set( const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const double& p_dValue);
	static HRESULT Set( const __int64& p_rDispId, SVDisplayCntrlEnum p_eDispFlag, const long& p_lValue);

	// This function replaces MdispLut
	static HRESULT Lut( const __int64& p_rDispId, const SVMatroxBuffer& p_rLutBufId);

	// This function replaces MdispPan
	static HRESULT Pan( const __int64& p_rDispId, const double& p_dXOffset, const double& p_dYOffset);
	// This function replaces MdispSelect
	static HRESULT Select( const __int64& p_rDispId, const SVMatroxBuffer& p_rImageId);

	// This function replaces MdispSelect
	static HRESULT Deselect( const __int64& p_rDispId);

	static HRESULT GetBitmapInfo( LPBITMAPINFO& p_rpBitmapInfo, const SVMatroxBuffer& p_rBuffer );
	
	// This function replaces MdispSelectWindow
	static HRESULT SelectWindow( const __int64& p_rDispId, const SVMatroxBuffer& p_rImageId, const HWND& p_hClientWindowHandle );
	// This function replaces MdispZoom
	static HRESULT Zoom( const __int64& p_rDispId, const double& p_dXFactor, const double& p_dYFactor );
public:
	static long Convert2MatroxType( SVDisplayCntrlEnum p_eDisp) ;

};

