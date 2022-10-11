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
#include "SVMatroxLibrary\SVMatroxBufferInterface.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

HBITMAP CreateDIBitmap(const BITMAPINFO& rDIBInfo, void* pDIBData)
{
	HBITMAP Result{ nullptr };

	const BITMAPINFOHEADER& rBitmapInfoHeader = rDIBInfo.bmiHeader;

	if (sizeof(BITMAPINFOHEADER) != rBitmapInfoHeader.biSize &&
		sizeof(BITMAPV4HEADER) != rBitmapInfoHeader.biSize &&
		sizeof(BITMAPV5HEADER) != rBitmapInfoHeader.biSize)
	{
		return Result;
	}

	void* pDIBSectionImage{ nullptr };

	Result = ::CreateDIBSection(nullptr, &rDIBInfo, DIB_RGB_COLORS, (void**) &pDIBSectionImage, nullptr, 0);
		
	::GdiFlush(); // The GdiFlush function flushes the calling thread's which is recommended by MSDN

	memcpy(pDIBSectionImage, pDIBData, rBitmapInfoHeader.biSizeImage);

	return Result;
}

HBITMAP convertToBitmap(const SvOi::SVImageBufferHandlePtr imageData)
{
	if (nullptr != imageData && !(imageData->empty()))
	{
		SVBitmapInfo dibInfo = imageData->GetBitmapInfo();
		BYTE* pMilBuffer = static_cast<BYTE*>(imageData->GetBufferAddress());
		if (nullptr != pMilBuffer)
		{
			//copy the image buffer - because the some images does not support DIB!
			SVMatroxBuffer newBuffer;
			SVMatroxBuffer oldBuffer = imageData->GetBuffer();

			HRESULT l_Code = SVMatroxBufferInterface::Create(newBuffer, oldBuffer, true);
			if (S_OK == l_Code)
			{
				l_Code = SVMatroxBufferInterface::CopyBuffer(newBuffer, oldBuffer);
			}
			if (S_OK == l_Code)
			{
				l_Code = SVMatroxBufferInterface::GetBitmapInfo(dibInfo, newBuffer);
			}

			if (S_OK == l_Code)
			{
				l_Code = SVMatroxBufferInterface::GetHostAddress(&pMilBuffer, newBuffer);
			}

			if (S_OK == l_Code && nullptr != pMilBuffer && !dibInfo.empty())
			{
				return CreateDIBitmap(*dibInfo.GetBitmapInfo(), pMilBuffer);
			}
		}
	}
	return nullptr;
}
