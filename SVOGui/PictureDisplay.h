//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : PictureDisplay
//* .File Name       : $Workfile:   PictureDisplay.h  $
//* .Description	 : The class specializes the wrapper for the ActiveX
//*					SVPictureDisplay.  One of the additional functionalities grab
//*					bitmaps from e.g. SVImageClass to set them to the control.
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 16:46:30  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <vector>
#include <map>
#include "CDSVPictureDisplay.h"
#include "SVLibrary\SVBitmap.h"
#include "SVPictureDisplay\DrawObject.h" // for ParamMap typedefs
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IMatroxImageData.h"
#pragma endregion Includes

#pragma region Declarations

namespace SVColor	// define standard color names
{  
	const COLORREF Black        = RGB(   0,   0,   0); //0x00000000;	//        0
	const COLORREF White        = RGB( 255, 255, 255); //0x00FFFFFF;	// 16777215
	const COLORREF DarkBlue     = RGB(   0,   0, 192); //0x00C00000;	// 12582912
	const COLORREF Blue         = RGB(   0,   0, 255); //0x00FF0000;	// 16711680
	const COLORREF MediumBlue   = RGB(   0, 128, 255); //0x00FF8000;	// 16760832
	const COLORREF LightBlue    = RGB(   0, 192, 255); //0x00FFC000;	// 16744448
	const COLORREF Aqua         = RGB(   0, 255, 255); //0x00FFFF00;	// 16776960
	const COLORREF DarkGreen    = RGB(   0, 128,   0); //0x00008000;	//    32668
	const COLORREF Green        = RGB(   0, 192,   0); //0x0000C000;	//    49152
	const COLORREF LightGreen   = RGB(   0, 255,   0); //0x0000FF00;	//    65280
	const COLORREF Cyan         = RGB(   0, 192, 192); //0x00C0C000;	// 12632064
	const COLORREF LightCyan    = RGB(   0, 255, 255); //0x00FFFF00;	// 16776960
	const COLORREF Red          = RGB( 192,   0,   0); //0x000000C0;	//      192
	const COLORREF LightRed     = RGB( 255,   0,   0); //0x000000FF;	//      255
	const COLORREF Magenta      = RGB( 192,   0, 192); //0x00C000C0;	// 12583104
	const COLORREF LightMagenta = RGB( 255,   0, 255); //0x00FF00FF;	// 16711935
	const COLORREF Brown        = RGB( 143,  71,  71); //0x0047478F;	//  4671375
	const COLORREF Yellow       = RGB( 255, 255,   0); //0x0000FFFF;	//    65535
	const COLORREF DarkGray     = RGB( 128, 128, 128); //0x00808080;	//  8421504
	const COLORREF LightGray    = RGB( 192, 192, 192); //0x00C0C0C0;	// 12632256
	const COLORREF WhiteSmoke   = RGB( 241, 241, 241);
	const COLORREF Orange       = RGB( 255, 112,   0); //0x000070FF;	//    28927
}
#pragma endregion Declarations

namespace Seidenader
{
	namespace SVOGui
	{
		class PictureDisplay :
			public CDSVPictureDisplay
		{
#pragma region Contructor
		public:
			virtual ~PictureDisplay();
#pragma endregion Contructor

#pragma region Public Methods
		public:
			//************************************
			// Method:    setImageFromParent
			// Description: Set image to the activeX-control. Take the parent image from pImage.
			// Access:    public
			// Returns:   void
			// Parameter: SVImageClass* pImage: The child image of the image which should be set.
			// Parameter: long tabNumber = 0: The tab number of the activeX-control.
			//************************************
			void setImageFromParent( SvOi::ISVImage* pImage, long tabNumber = 0 );

			//************************************
			//! Set a image to a tab of the activeX-control.
			//! \param pImage [in] The image which will be set.
			//! \param tabNumber [in,out] The tab number of the activeX-control. Default value = 0.
			//! \returns void
			//************************************
			void setImage( SvOi::ISVImage* pImage, long tabNumber = 0 );

			//************************************
			//! Set a image to a tab of the activeX-control.
			//! \param imageData [in] The data container of the image which will be set.
			//! \param tabNumber [in] The tab number of the activeX-control. Default value = 0.
			//! \returns void
			//************************************
			void setImage( const SvOi::IMatroxImageData *imageData, long tabNumber = 0 );

			//************************************
			// Method:    AddTab
			// Description: Add a tab to the activeX-control.
			// Access:    public
			// Returns:   SCODE return an error code.
			// Parameter: LPCTSTR Text: The text for the tab.
			//************************************
			SCODE AddTab( LPCTSTR Text );

			//************************************
			// Method:    AddTab
			// Description: Add a tab to the activeX-control.
			// Access:    public
			// Returns:   SCODE return an error code.
			// Parameter: LPCTSTR Text: The text for the tab.
			//Parameter: long * pHandle: Return the handle to the tab.
			//************************************
			SCODE AddTab( LPCTSTR Text , LONG* Handle );

			//************************************
			// Method:    AddOverlay
			// Description: Add an overlay to an image tab.
			// Access:    public
			// Returns:   SCODE return an error code.
			// Parameter: long Tab: handle to the tab
			// Parameter: VARIANT* ParameterList: list of the parameter names
			// Parameter: VARIANT* ParameterValues: list of the parameter values (same order than ParameterList)
			// Parameter: long * pHandle: Return the handle to the overlay.
			//************************************
			SCODE AddOverlay( long Tab, VARIANT* ParameterList, VARIANT* ParameterValues, long* pHandle );

			//************************************
			// Method:    AddOverlay
			// Description: Add a overlay to a image tab
			// Access:    public
			// Returns:   SCODE return an error code.
			// Parameter: long Tab: handle to the tab
			// Parameter: LongParamMap Parmap: Parameter map for the overlay.
			// Parameter: long * pHandle: Return the handle to the overlay.
			//************************************
			SCODE AddOverlay( long Tab, const LongParamMap& ParMap, long* pHandle );

			//************************************
			// Method:    AddOverlay
			// Description: Add a overlay to a image tab
			// Returns:   SCODE return an error code.
			// Parameter: long Tab: handle to the tab
			// Parameter: const VariantParamMap& Parmap: Parameter map for the overlay.
			// Parameter: long * pHandle: Return the handle to the overlay.
			//************************************
			SCODE AddOverlay( long Tab, const VariantParamMap& ParMap, long* pHandle );

			//************************************
			// Method:    EditShape
			// Description: Edit a existing overlay.
			// Access:    public
			// Returns:   SCODE return an error code.
			// Parameter: long Tab: handle to the tab
			// Parameter: long handle: handle to the overlay
			// Parameter: VARIANT* ParameterList: list of the parameter names
			// Parameter: VARIANT* ParameterValues: list of the parameter values (same order than ParameterList)
			//************************************
			SCODE EditOverlay( long Tab, long handle, VARIANT* ParameterList, VARIANT* ParameterValues );

			//************************************
			// Method:    EditShape
			// Description: Edit a existing overlay.
			// Access:    public
			// Returns:   SCODE return an error code.
			// Parameter: long Tab: handle to the tab
			// Parameter: long handle: handle to the overlay
			// Parameter: LongParamMap Parmap: Parameter map for the overlay.
			//************************************
			SCODE EditOverlay( long Tab, long handle, LongParamMap ParMap );
#pragma endregion Public Methods

#pragma region Member Variables
		private:
			std::vector< SVBitmap* > m_dibVector;
#pragma endregion Member Variables
		};
	}
}

namespace SvOg = Seidenader::SVOGui;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\PictureDisplay.h_v  $
 * 
 *    Rev 1.0   26 Jun 2014 16:46:30   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  885
 * SCR Title:  Replace image display in TA-dialogs with activeX SVPictureDisplay
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/