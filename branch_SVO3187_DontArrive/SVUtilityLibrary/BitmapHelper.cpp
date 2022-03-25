//*****************************************************************************
/// \copyright (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file BitmapHelper.h
//*****************************************************************************
// Helper functions for bitmaps
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BitmapHelper.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvUl
{
	HBITMAP CreateDIBitmap(const BITMAPINFO& rDIBInfo, void* pDIBData)
	{
		HBITMAP Result{ nullptr };

		const BITMAPINFOHEADER& rBitmapInfoHeader = rDIBInfo.bmiHeader;

		if (sizeof(BITMAPINFOHEADER) == rBitmapInfoHeader.biSize)
		{
			//------ Version 3 header
		}
		else if (sizeof(BITMAPV4HEADER) == rBitmapInfoHeader.biSize)
		{
			//------ Version 4 header
		}
		else if (sizeof(BITMAPV5HEADER) ==  rBitmapInfoHeader.biSize)
		{
			//------ Version 5 header
		}
		else
		{
			return Result;
		}

		void* pDIBSectionImage{ nullptr };

		Result = ::CreateDIBSection(nullptr, &rDIBInfo, DIB_RGB_COLORS, (void**) &pDIBSectionImage, nullptr, 0);
		
		::GdiFlush(); // The GdiFlush function flushes the calling thread's which is recommended by MSDN

		memcpy(pDIBSectionImage, pDIBData, rBitmapInfoHeader.biSizeImage);

		return Result;
	}

} // namespace SvUl
