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
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <map>
#include "CDSVPictureDisplay.h"
#include "SVLibrary\DrawObject.h" // for ParamMap typedefs
#include "ObjectInterfaces\ISVImage.h"
#include "ObjectInterfaces\IMatroxImageData.h"
#pragma endregion Includes

#pragma region Declarations

#pragma endregion Declarations

namespace SvOg
{
	class PictureDisplay : public CDSVPictureDisplay
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
		void setImageFromParent( SvOi::ISVImage* pImage, long tabNumber = 0 ); //@TODO - this needs to go!

		//************************************
		//! Set a image to a tab of the activeX-control.
		//! \param pImage [in] The image which will be set.
		//! \param tabNumber [in,out] The tab number of the activeX-control. Default value = 0.
		//! \returns void
		//************************************
		void setImage( SvOi::ISVImage* pImage, long tabNumber = 0 ); //@TODO - this needs to go!

		//************************************
		//! Set a image to a tab of the activeX-control.
		//! \param imageData [in] The data container of the image which will be set.
		//! \param tabNumber [in] The tab number of the activeX-control. Default value = 0.
		//! \returns void
		//************************************
		void setImage( const SvOi::IMatroxImageData *imageData, long tabNumber = 0 ); //@TODO - this needs to go!

		//************************************
		//! Set a image to a tab of the activeX-control.
		//! \param imageData [in] The Image Data.
		//! \param tabNumber [in] The tab number of the activeX-control. Default value = 0.
		//! \returns void
		//************************************
		void setImage(IPictureDisp* pImage, long tabNumber = 0);

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

		/// Add a overlay to a image tab
		/// \param Tab [in] handle to the tab
		/// \param ParMap [in] Parameter map for the overlay
		/// \param pHandle [out] Return the handle to the overlay.
		/// \returns SCODE return an error code.
		SCODE AddOverlay( long Tab, const LongParamMap& ParMap, long* pHandle );
		SCODE AddOverlay( long Tab, const VariantParamMap& ParMap, long* pHandle );

		/// Edit a existing overlay.
		/// \param Tab [in] handle to the tab
		/// \param handle [in] handle to the overlay
		/// \param ParameterList [in] list of the parameter names
		/// \param ParameterValues [in] list of the parameter values (same order than ParameterList)
		/// \returns SCODE
		SCODE EditOverlay( long Tab, long handle, VARIANT* ParameterList, VARIANT* ParameterValues );

		/// Edit a existing overlay
		/// \param Tab [in] handle to the tab
		/// \param handle [in] handle to the overlay
		/// \param ParMap [in] Parameter map for the overlay
		/// \returns SCODE return an error code
		SCODE EditOverlay( long Tab, long handle, const LongParamMap& ParMap );
		SCODE EditOverlay( long Tab, long handle, const VariantParamMap& ParMap );
#pragma endregion Public Methods
	};
} //namespace SvOg
