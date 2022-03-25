//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file BitmapHelper.h
//*****************************************************************************
// Helper functions for bitmaps
//******************************************************************************

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvUl
{

	//************************************
	/// Function to create a DIB from an image buffer
	/// \param pString, rString or rVariant [in] The source object
	/// \returns std::string The created object.
	//************************************
	HBITMAP CreateDIBitmap(const BITMAPINFO& rDIBInfo, void* pDIBData);
} // namespace SvUl

