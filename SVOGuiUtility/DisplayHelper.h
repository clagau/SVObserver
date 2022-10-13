//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : DisplayHelper
//* .File Name       : $Workfile:   DisplayHelper.h  $
//* .Description	 : Helper class for the SVPictureDisplay to convert between std::map and Variants
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   26 Jun 2014 11:38:32  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <comutil.h>
#include "GridCtrlLibrary/GridCtrl.h"
#pragma endregion Includes

namespace SvPb
{
class Image;
}
typedef std::map< long, long > LongParamMap;
typedef std::map< long, _variant_t > VariantParamMap;

namespace SvOgu
{
class DisplayHelper
{
#pragma region Public Methods
public:
	//************************************
	// Method:    CreateSafeArrayFromMap
	// Description:  Create two COleSafeArray with the values from the map.
	//				 Caller HAS TO destroy spar and val after use.
	// Parameter: const LongParamMap map
	// Parameter: COleSafeArray & spar SafeArray with long's
	// Parameter: COleSafeArray & val SafeArray with VARIANT (type VT_I4)
	// Returns:  long - number of entries in the array or -1 when an error occurs
	//************************************
	static long CreateSafeArrayFromMap(const LongParamMap& map, COleSafeArray& spar, COleSafeArray& val);

	//************************************
	// Method:    CreateSafeArrayFromMap
	// Description:  Create two COleSafeArray with the values from the map.
	//				 Caller HAS TO destroy spar and val after use.
	// Parameter: const VariantParamMap& map
	// Parameter: COleSafeArray & spar SafeArray with long's
	// Parameter: COleSafeArray & val SafeArray with VARIANT
	// Returns:  long - numbers of entries in the array or -1 when an error occurs
	//************************************
	static long CreateSafeArrayFromMap(const VariantParamMap& map, COleSafeArray& sPar, COleSafeArray& saVal);

	//************************************
	// Method:    FillParameterMap
	// Description: fill the parameter map with pairs from pParameterList and pParameterValue
	// Parameter: VariantParamMap& pamap
	// Parameter: VARIANT * pParameterList expect smartarray of long
	// Parameter: VARIANT * pParameterValue  expect smartarray of VARIANT
	// Returns:   long - number of entries in map -1 when an error occurs 
	//************************************
	static long FillParameterMap(VariantParamMap& pamap, VARIANT* ParameterList, VARIANT* ParameterValue);

	/// Convert a protobuf-Image to a IPictureDisp
	/// \param rImage [in] The protobuf-Message.
	/// \param rWidth [out] Width of the image.
	/// \param rHeight [out] Height of the image.
	/// \param idisp [out] The image in an IPictureDisp.
	/// \returns HRESULT S_OK if convert was successful.
	static HRESULT convertPBImageToIPictureDisp(const SvPb::Image& rImage, long &rWidth, long &rHeight, IPictureDisp** idisp);

	/// Load the arrow Icon to the imageList and set it to the Grid
	/// \param rImageList [in,out]
	/// \param rDownArrowBitmap [in,out]
	/// \param rGrid [in,out]
	static void setIconListToGrid(CImageList& rImageList, CBitmap& rDownArrowBitmap, SvGcl::GridCtrl& rGrid);
#pragma endregion Public Methods
};
} //namespace SvOgu
