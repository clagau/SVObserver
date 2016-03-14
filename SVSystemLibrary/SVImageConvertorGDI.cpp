//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageConvertorGDI
//* .File Name       : $Workfile:   SVImageConvertorGDI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:51:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVImageConvertorGDI.h"
#pragma endregion Includes

HRESULT SVImageConvertorGDI::GetBitmapFromIPictureDisp(IPictureDisp* pPictureDisp, HBITMAP& rBitmap)
{
	HRESULT hr = S_FALSE;
	
	if (nullptr != pPictureDisp)
	{
		IPicture* l_pPicture(nullptr);
		hr = pPictureDisp->QueryInterface(IID_IPicture, reinterpret_cast<void **>(&l_pPicture));

		if (S_OK == hr && l_pPicture)
		{
			hr = l_pPicture->get_Handle(reinterpret_cast<OLE_HANDLE *>(&rBitmap));
		}
		if (l_pPicture)
		{
			l_pPicture->Release();
		}
	}
	return hr;
}

HRESULT SVImageConvertorGDI::GetIPictureDispFromBitmap(HBITMAP Bitmap, IPictureDisp** ppPictureDisp)
{
	HRESULT hr = S_FALSE;
	
	if (nullptr != ppPictureDisp)
	{
		if (nullptr != *ppPictureDisp)
		{
			(*ppPictureDisp)->Release();

			*ppPictureDisp = nullptr;
		}

		// Create the IPictureDisp
		PICTDESC pictDesc;
		pictDesc.cbSizeofstruct = sizeof(PICTDESC);
		pictDesc.picType = PICTYPE_BITMAP;
		pictDesc.bmp.hbitmap = Bitmap;

		// 
		IPictureDisp* l_pPictureDisp = nullptr;
		hr = ::OleCreatePictureIndirect(&pictDesc, IID_IPictureDisp, true, reinterpret_cast<LPVOID *>(&l_pPictureDisp));
		if (S_OK == hr)
		{
			*ppPictureDisp = l_pPictureDisp;
		}
	}
	return hr;
}

HRESULT SVImageConvertorGDI::BitmapToBStr(HBITMAP hBitmap, BSTR& rImage)
{
	HRESULT l_Status = S_OK;

	if (nullptr != rImage)
	{
		::SysFreeString(rImage);
		rImage = nullptr;
	}

	DIBSECTION dib;

	if (nullptr != hBitmap)
	{
		if (0 == ::GetObject(hBitmap, sizeof(dib), &dib))
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if (S_OK == l_Status)
	{
		BITMAPINFOHEADER* pbmhInfo = &dib.dsBmih;

		// Source images seem to be flipped even though MIL is not supposed to flip them
		//if (pbmhInfo->biHeight > 0)
		//	pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
		
		// Calculate the absolute height
		long lHeight = abs(pbmhInfo->biHeight);

		// Make sure image size is calculated
		if (0 == pbmhInfo->biSizeImage)
		{
			pbmhInfo->biSizeImage = ((((pbmhInfo->biWidth * pbmhInfo->biBitCount) + 31) & ~31) >> 3) * lHeight;
		}
		// Make sure color table size is calculated
		long lNumColor = pbmhInfo->biClrUsed;
		if (0 == lNumColor)
		{
			if (24 != pbmhInfo->biBitCount)
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		long lTabSize = lNumColor * sizeof(RGBQUAD);

		// Calculate total size buffer needed for image
		long lBufSize = sizeof(BITMAPINFOHEADER) + lTabSize + pbmhInfo->biSizeImage;

		// Alloc BSTR to store image : in this structure <Color Table Size - BITMAPINFO (incl Color Table) - bitmap bits>
		rImage = ::SysAllocStringByteLen(nullptr, lBufSize);
		BYTE* pBytes = reinterpret_cast<BYTE*>(rImage);
		BITMAPINFO* pbmInfo = reinterpret_cast<BITMAPINFO*>(pBytes);
		
		// Copy data to BSTR memory locations
		memcpy(pBytes, pbmhInfo, sizeof(BITMAPINFOHEADER));

		//********************************************************
		// Get Color Table information.

		// If has color table 
		if (dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes <= 8)
		{
			HDC hMemDC;
			HBITMAP hOldBitmap;
			int nColors;

			// Find out how many colors are in the color table
			nColors = 1 << (dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes);
			// Create a memory DC and select the DIBSection into it
			hMemDC = CreateCompatibleDC(nullptr);

			hOldBitmap = reinterpret_cast<HBITMAP>(SelectObject(hMemDC, hBitmap));

			// Get the DIBSection's color table
			GetDIBColorTable(hMemDC, 0, nColors, reinterpret_cast<RGBQUAD *>(pBytes + sizeof(BITMAPINFOHEADER)) );

			// Cleanup
			SelectObject(hMemDC,hOldBitmap);
			DeleteDC(hMemDC);
		}

		void* pBits = dib.dsBm.bmBits;

#if 1
		//////////////////////////////////
		// flip the image for SVObserver
		void* pDIBits = nullptr;
		HDC hDC = CreateCompatibleDC(nullptr);
		HBITMAP hFlipDIB = ::CreateDIBSection(hDC, pbmInfo, DIB_RGB_COLORS, &pDIBits, nullptr, 0);
		CopyDIBitsFlip(hDC, hFlipDIB, pBits, pbmInfo);
		pBits = pDIBits;
		//////////////////////////////////
#endif
		memcpy(pBytes + sizeof(BITMAPINFOHEADER) + lTabSize, pBits, pbmhInfo->biSizeImage);

		::DeleteDC(hDC);
		::DeleteObject(hFlipDIB);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageConvertorGDI::BStrToBitmap(BSTR Image, HBITMAP& rBitmap)
{
	HRESULT hr = S_FALSE;

	HBITMAP hBitmap = nullptr;
	HDC hDC = nullptr;
	void* pBits = nullptr;

	if (Image && (0 != SysStringByteLen(Image)))
	{
		// Find out how big it is and get pointers to BITMAPINFO and the bitmap bits
		BYTE* pBytes   = reinterpret_cast<BYTE*>(Image);
		BITMAPINFO* pbmInfo  = reinterpret_cast<BITMAPINFO*>(pBytes);
		BITMAPINFOHEADER* pbmhInfo = reinterpret_cast<BITMAPINFOHEADER*>(&pbmInfo->bmiHeader);

		// Make sure color table size is calculated
		long lNumColor = pbmhInfo->biClrUsed;
		if (0 == lNumColor)
		{
			if (24 != pbmhInfo->biBitCount)
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		long lTabSize = lNumColor * sizeof(RGBQUAD);

		pBytes  = pBytes + sizeof(BITMAPINFOHEADER) + (lTabSize);

		// Create DC and a DIB bitmap
		hDC = CreateCompatibleDC(nullptr);
		pbmhInfo->biHeight = abs(pbmhInfo->biHeight);	// always make positive
		hBitmap = ::CreateDIBSection(hDC, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0);
		
#if 1
		CopyDIBitsFlip(hDC, hBitmap, pBytes, pbmInfo);
#else
		// Set the bits of our new bitmap
		memcpy(pBits, pBytes, pbmhInfo->biSizeImage);
#endif
		::DeleteDC(hDC);

		rBitmap = hBitmap;

		hr = S_OK;	
	}
	return hr;
}

HRESULT SVImageConvertorGDI::BitmapToByteVector(HBITMAP hBitmap, SVByteVector& rImage)
{
	HRESULT l_Status = S_OK;

	rImage.clear();

	DIBSECTION dib;

	if (nullptr != hBitmap)
	{
		if( 0 == ::GetObject(hBitmap, sizeof(dib), &dib))
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if (S_OK == l_Status)
	{
		BITMAPINFOHEADER* pbmhInfo = &dib.dsBmih;

		// Source images seem to be flipped even though MIL is not supposed to flip them
		//if( pbmhInfo->biHeight > 0 )
		//	pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
		
		// Calculate the absolute height
		long lHeight = abs(pbmhInfo->biHeight);

		// Make sure image size is calculated
		if (0 == pbmhInfo->biSizeImage)
		{
			pbmhInfo->biSizeImage = ((((pbmhInfo->biWidth * pbmhInfo->biBitCount) + 31) & ~31) >> 3) * lHeight;
		}
		// Make sure color table size is calculated
		long lNumColor = pbmhInfo->biClrUsed;
		if (0 == lNumColor)
		{
			if (24 != pbmhInfo->biBitCount)
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		long lTabSize = lNumColor * sizeof(RGBQUAD);

		// Calculate total size buffer needed for image
		long lBufSize = sizeof(BITMAPINFOHEADER) + lTabSize + pbmhInfo->biSizeImage;

		// Alloc BSTR to store image : in this structure <Color Table Size - BITMAPINFO (incl Color Table) - bitmap bits>
		rImage.resize(lBufSize);
		BYTE* pBytes = reinterpret_cast<BYTE*>(&(rImage[0]));
		BITMAPINFO* pbmInfo = reinterpret_cast<BITMAPINFO*>(pBytes);
		
		// Copy data to BSTR memory locations
		memcpy( pBytes, pbmhInfo, sizeof( BITMAPINFOHEADER ) );

		//********************************************************
		// Get Color Table information.

		// If has color table 
		if (dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes <= 8)
		{
			HDC hMemDC;
			HBITMAP hOldBitmap;
			int nColors;

			// Find out how many colors are in the color table
			nColors = 1 << (dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes);
			// Create a memory DC and select the DIBSection into it
			hMemDC = CreateCompatibleDC(nullptr);

			hOldBitmap = reinterpret_cast<HBITMAP>(SelectObject(hMemDC, hBitmap));

			// Get the DIBSection's color table
			GetDIBColorTable(hMemDC, 0, nColors, reinterpret_cast<RGBQUAD *>(pBytes + sizeof(BITMAPINFOHEADER)));

			// Cleanup
			SelectObject(hMemDC,hOldBitmap);
			DeleteDC(hMemDC);
		}

		void* pBits = dib.dsBm.bmBits;

#if 1
		//////////////////////////////////
		// flip the image for SVObserver
		void* pDIBits = nullptr;
		HDC hDC = CreateCompatibleDC(nullptr);
		HBITMAP hFlipDIB = ::CreateDIBSection(hDC, pbmInfo, DIB_RGB_COLORS, &pDIBits, nullptr, 0);
		CopyDIBitsFlip(hDC, hFlipDIB, pBits, pbmInfo);
		pBits = pDIBits;
		//////////////////////////////////
#endif
		memcpy(pBytes + sizeof(BITMAPINFOHEADER) + lTabSize, pBits, pbmhInfo->biSizeImage);

		::DeleteDC(hDC);
		::DeleteObject(hFlipDIB);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageConvertorGDI::ByteVectorToBitmap(const SVByteVector& Image, HBITMAP& rBitmap)
{
	HRESULT hr = S_FALSE;

	HBITMAP hBitmap = nullptr;
	HDC hDC = nullptr;
	void* pBits = nullptr;

	if (0 < Image.size())
	{
		// Find out how big it is and get pointers to BITMAPINFO and the bitmap bits
		BYTE* pBytes   = (BYTE*)(&(Image[0]));
		BITMAPINFO* pbmInfo  = reinterpret_cast<BITMAPINFO*>(pBytes);
		BITMAPINFOHEADER* pbmhInfo = reinterpret_cast<BITMAPINFOHEADER*>(&pbmInfo->bmiHeader);

		// Make sure color table size is calculated
		long lNumColor = pbmhInfo->biClrUsed;
		if (0 == lNumColor)
		{
			if (24 != pbmhInfo->biBitCount)
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		long lTabSize = lNumColor * sizeof(RGBQUAD);

		pBytes = pBytes + sizeof(BITMAPINFOHEADER) + (lTabSize);

		// Create DC and a DIB bitmap
		hDC = CreateCompatibleDC(nullptr);
		pbmhInfo->biHeight = abs(pbmhInfo->biHeight);	// always make positive
		hBitmap = ::CreateDIBSection(hDC, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0);
		
#if 1
		CopyDIBitsFlip(hDC, hBitmap, pBytes, pbmInfo);
#else
		// Set the bits of our new bitmap
		memcpy(pBits, pBytes, pbmhInfo->biSizeImage);
#endif
		::DeleteDC(hDC);

		rBitmap = hBitmap;

		hr = S_OK;	
	}
	return hr;
}

HRESULT SVImageConvertorGDI::CopyDIBits(HBITMAP SrcBitmap, BITMAPINFOHEADER* pDestInfo, void* pDestBits)
{
	HRESULT l_Status = S_OK;

	if (nullptr != SrcBitmap && nullptr != pDestInfo && nullptr != pDestBits)
	{
		DIBSECTION dib;

		if (0 != ::GetObject(SrcBitmap, sizeof(dib), &dib))
		{
			l_Status = CopyDIBits(&(dib.dsBmih), dib.dsBm.bmBits, pDestInfo, pDestBits);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, HBITMAP& rDestBitmap)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != rDestBitmap)
	{
		DIBSECTION dib;

		if (0 != ::GetObject(rDestBitmap, sizeof(dib), &dib))
		{
			l_Status = CopyDIBits(pSrcInfo, pSrcBits, &(dib.dsBmih), dib.dsBm.bmBits);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, BITMAPINFOHEADER* pDestInfo, void* pDestBits)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != pDestInfo && nullptr != pDestBits)
	{
		if (pSrcInfo->biHeight == pDestInfo->biHeight)	// if sign matches
		{
			bool l_FormatMatches = (0 != pSrcInfo->biHeight);

			l_FormatMatches &= (abs(pSrcInfo->biHeight) == abs(pDestInfo->biHeight));
			l_FormatMatches &= (pSrcInfo->biWidth == pDestInfo->biWidth);
			l_FormatMatches &= (pSrcInfo->biSizeImage == pDestInfo->biSizeImage);
			l_FormatMatches &= (pSrcInfo->biClrUsed == pDestInfo->biClrUsed);
			
			if (l_FormatMatches)
			{
				memcpy(pDestBits, pSrcBits, pSrcInfo->biSizeImage);
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
		else
		{
			l_Status = CopyDIBitsFlip(pSrcInfo, pSrcBits, pDestInfo, pDestBits);
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}

	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPitch, HBITMAP& rDestBitmap)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != rDestBitmap)
	{
		DIBSECTION dib;

		if (0 != ::GetObject(rDestBitmap, sizeof(dib), &dib))
		{
			l_Status = CopyDIBits(pSrcInfo, pSrcBits, srcPitch, &(dib.dsBmih), dib.dsBm.bmBits);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPitch, BITMAPINFOHEADER* pDstInfo, void* pDstBits)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != pDstInfo && nullptr != pDstBits)
	{
		bool l_FormatMatches = (0 != pSrcInfo->biHeight);
		l_FormatMatches &= (abs(pSrcInfo->biHeight) == abs(pDstInfo->biHeight));
		l_FormatMatches &= (pSrcInfo->biWidth == pDstInfo->biWidth);
		
		if (l_FormatMatches)
		{
			long dstPitch = ((((pDstInfo->biWidth * pDstInfo->biBitCount) + 31) & ~31) >> 3);

			const BYTE* pSrc = reinterpret_cast<BYTE*>(pSrcBits);
			BYTE* pDst = reinterpret_cast<BYTE*>(pDstBits);

			for (int row = 0; row < abs(pSrcInfo->biHeight); row++)
			{
				memcpy(pDst, pSrc, srcPitch);
				pSrc += srcPitch;
				pDst += dstPitch;
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPitch, long pixelOffset, long bytesPerPixel, HBITMAP& rDestBitmap)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != rDestBitmap)
	{
		DIBSECTION dib;

		if (0 != ::GetObject(rDestBitmap, sizeof(dib), &dib))
		{
			l_Status = CopyDIBits(pSrcInfo, pSrcBits, srcPitch, pixelOffset, bytesPerPixel, &(dib.dsBmih), dib.dsBm.bmBits);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPitch, long pixelOffset, long bytesPerPixel, BITMAPINFOHEADER* pDstInfo, void* pDstBits)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != pDstInfo && nullptr != pDstBits)
	{
		bool l_FormatMatches = (0 != pSrcInfo->biHeight);
		l_FormatMatches &= (abs(pSrcInfo->biHeight) == abs(pDstInfo->biHeight));
		l_FormatMatches &= (pSrcInfo->biWidth == pDstInfo->biWidth);
		
		if (l_FormatMatches)
		{
			long dstPitch = ((((pDstInfo->biWidth * pDstInfo->biBitCount) + 31) & ~31) >> 3);

			const BYTE* pSrc = reinterpret_cast<BYTE*>(pSrcBits);
			BYTE* pDst = reinterpret_cast<BYTE*>(pDstBits);

			for (int row = 0; row < abs(pSrcInfo->biHeight); row++)
			{
				for (int col = 0; col < pSrcInfo->biWidth; col++)
				{
					*(pDst + col) = *(pSrc + ((col * bytesPerPixel) + pixelOffset));
				}
				pSrc += srcPitch;
				pDst += dstPitch;
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo)
{
	BITMAPINFOHEADER* pbmhInfo = &(pbmInfo->bmiHeader);

	pbmInfo->bmiHeader.biHeight = -pbmInfo->bmiHeader.biHeight;

	// Set the bits of our new bitmap
	int iRet = SetDIBits(hDC, hDIB, 0, abs(pbmhInfo->biHeight), pSrcBits, pbmInfo, DIB_RGB_COLORS);

	::GdiFlush();

	return S_OK;
}

HRESULT SVImageConvertorGDI::CopyDIBitsFlip(HBITMAP SrcBitmap, BITMAPINFOHEADER* pDestInfo, void* pDestBits)
{
	HRESULT l_Status = S_OK;

	if (nullptr != SrcBitmap && nullptr != pDestInfo && nullptr != pDestBits)
	{
		DIBSECTION dib;

		if (0 != ::GetObject(SrcBitmap, sizeof(dib), &dib))
		{
			l_Status = CopyDIBitsFlip(&(dib.dsBmih), dib.dsBm.bmBits, pDestInfo, pDestBits);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBitsFlip(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, HBITMAP& rDestBitmap)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != rDestBitmap)
	{
		DIBSECTION dib;

		if (0 != ::GetObject( rDestBitmap, sizeof(dib), &dib))
		{
			l_Status = CopyDIBitsFlip(pSrcInfo, pSrcBits, &(dib.dsBmih), dib.dsBm.bmBits);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBitsFlip(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, BITMAPINFOHEADER* pDestInfo, void* pDestBits)
{
	HRESULT l_Status = S_OK;

	if (nullptr != pSrcInfo && nullptr != pSrcBits && nullptr != pDestInfo && nullptr != pDestBits)
	{
		bool l_FormatMatches = (0 != pSrcInfo->biHeight);

		l_FormatMatches &= (abs(pSrcInfo->biHeight) == abs(pDestInfo->biHeight));
		l_FormatMatches &= (pSrcInfo->biWidth == pDestInfo->biWidth);
		l_FormatMatches &= (pSrcInfo->biSizeImage == pDestInfo->biSizeImage);
		l_FormatMatches &= (pSrcInfo->biClrUsed == pDestInfo->biClrUsed);
		
		if (l_FormatMatches)
		{
			int iRasterLineSize = ((((pSrcInfo->biWidth * pSrcInfo->biBitCount) + 31) & ~31) >> 3);

			const BYTE* pSrcLine = reinterpret_cast<BYTE*>(pSrcBits);
			BYTE* pDestLine = (reinterpret_cast<BYTE*>(pDestBits)) + (iRasterLineSize * (abs(pDestInfo->biHeight) - 1));
			for (int iLine = 0; iLine < pSrcInfo->biHeight; iLine++, pSrcLine += iRasterLineSize, pDestLine -= iRasterLineSize)
			{
				memcpy(pDestLine, pSrcLine, iRasterLineSize);
			}
		}
		else
		{
			l_Status = E_INVALIDARG;
		}
	}
	else
	{
		l_Status = E_INVALIDARG;
	}
	return l_Status;
}

HBITMAP SVImageConvertorGDI::CreateBitmap(HBITMAP SourceBitmap, const RECT& rROI)
{
	HBITMAP l_Bitmap = nullptr;

	if (nullptr != SourceBitmap)
	{
		DIBSECTION l_DIBSection;
		GetObject(SourceBitmap, sizeof(DIBSECTION), &l_DIBSection);

		HDC l_DC = ::GetDC(nullptr);
		HDC l_SourceDC = ::CreateCompatibleDC(l_DC);
		HGDIOBJ l_OldSourceBitmap = ::SelectObject(l_SourceDC, SourceBitmap);

		long l_BitmapInfoSize = sizeof(BITMAPINFO) + (sizeof(RGBQUAD) * l_DIBSection.dsBmih.biClrUsed);
		unsigned char* l_pBuffer = new unsigned char[l_BitmapInfoSize];
		BITMAPINFO* l_pInfo = reinterpret_cast<BITMAPINFO*>(l_pBuffer);

		void* l_pBits = nullptr;
		long l_Stride = (::labs(rROI.right - rROI.left + 1) * (l_DIBSection.dsBmih.biBitCount/ 8) + 3) & ~3;
		long l_ImageSize = l_Stride * ::labs(rROI.bottom - rROI.top + 1);

		l_pInfo->bmiHeader.biBitCount = l_DIBSection.dsBmih.biBitCount;
		l_pInfo->bmiHeader.biClrImportant = 0;
		l_pInfo->bmiHeader.biClrUsed = 0;
		l_pInfo->bmiHeader.biCompression = BI_RGB;
		l_pInfo->bmiHeader.biHeight = (rROI.bottom - rROI.top + 1);
		l_pInfo->bmiHeader.biPlanes = l_DIBSection.dsBmih.biPlanes;
		l_pInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		l_pInfo->bmiHeader.biSizeImage = l_ImageSize;
		l_pInfo->bmiHeader.biWidth = (rROI.right - rROI.left + 1);

		if (0 < l_DIBSection.dsBmih.biClrUsed)
		{
			RGBQUAD* l_pTable = &(l_pInfo->bmiColors[0]);

			::GetDIBColorTable(l_SourceDC, 0, l_DIBSection.dsBmih.biClrUsed, l_pTable);
		}

		HDC l_DestDC = ::CreateCompatibleDC(l_DC);
		HBITMAP l_hRoiBitmap = ::CreateDIBSection(l_DestDC, l_pInfo, DIB_RGB_COLORS, &l_pBits, nullptr, 0);
		HGDIOBJ l_OldDestBitmap = ::SelectObject(l_DestDC, l_hRoiBitmap );

		if (::BitBlt(l_DestDC, 0, 0, (rROI.right - rROI.left + 1), (rROI.bottom - rROI.top + 1), l_SourceDC, rROI.left, rROI.top, SRCCOPY))
		{
			l_Bitmap = l_hRoiBitmap;
		}

		::SelectObject(l_DestDC, l_OldDestBitmap);
		::SelectObject(l_SourceDC, l_OldSourceBitmap);
		::ReleaseDC(nullptr, l_DC);

		::DeleteDC(l_SourceDC);
		::DeleteDC(l_DestDC);

		delete [] l_pBuffer;
	}
	return l_Bitmap;
}

HBITMAP SVImageConvertorGDI::CopyBitmap(HBITMAP SourceBitmap)
{
	BITMAP bmp;
	GetObject(SourceBitmap, sizeof(BITMAP), &bmp);

	HBITMAP hBitmap = reinterpret_cast<HBITMAP>(CopyImage(SourceBitmap, IMAGE_BITMAP, bmp.bmWidth, bmp.bmHeight, LR_CREATEDIBSECTION));
	return hBitmap;
}

HBITMAP SVImageConvertorGDI::CreateDIB(BITMAPINFO* pbmInfo, void* pSrc)
{
	void* pBits = nullptr;
	HBITMAP hBitmap = nullptr;
	BITMAPINFOHEADER* pbmhInfo = &(pbmInfo->bmiHeader);
	// Create DC and a DIB bitmap
	HDC hDC = ::CreateCompatibleDC(nullptr);
	if (hDC)
	{
		pbmhInfo->biHeight = abs(pbmhInfo->biHeight);	// always make positive
		hBitmap = ::CreateDIBSection(hDC, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0);
		if (hBitmap)
		{
			SVImageConvertorGDI::CopyDIBitsFlip(hDC, hBitmap, pSrc, pbmInfo);
		}
		::DeleteDC(hDC);
	}
	return hBitmap;
}

HBITMAP SVImageConvertorGDI::CreateDIB(BITMAPINFO* pbmInfo, void* pSrc, long srcPitch)
{
	void* pBits = nullptr;
	HBITMAP hBitmap = nullptr;
	BITMAPINFOHEADER* pbmhInfo = &(pbmInfo->bmiHeader);
	// Create DC and a DIB bitmap
	HDC hDC = ::CreateCompatibleDC(nullptr);
	if (hDC)
	{
		hBitmap = ::CreateDIBSection(hDC, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0);
		::DeleteDC(hDC);
		if (hBitmap)
		{
			HRESULT hr = SVImageConvertorGDI::CopyDIBits(pbmhInfo, pSrc, srcPitch, hBitmap);
			if (S_OK != hr)
			{
				::DeleteObject(hBitmap);
				hBitmap = nullptr;
			}
		}
	}
	return hBitmap;
}

HBITMAP SVImageConvertorGDI::CreateDIB(BITMAPINFO* pbmInfo, void* pSrc, long srcPitch, long pixelOffset, long bytesPerPixel)
{
	void* pBits = nullptr;
	HBITMAP hBitmap = nullptr;
	BITMAPINFOHEADER* pbmhInfo = &(pbmInfo->bmiHeader);
	// Create DC and a DIB bitmap
	HDC hDC = ::CreateCompatibleDC(nullptr);
	if (hDC)
	{
		hBitmap = ::CreateDIBSection(hDC, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0);
		::DeleteDC(hDC);
		if (hBitmap)
		{
			HRESULT hr = SVImageConvertorGDI::CopyDIBits(pbmhInfo, pSrc, srcPitch, pixelOffset, bytesPerPixel, hBitmap);
			if (S_OK != hr)
			{
				::DeleteObject(hBitmap);
				hBitmap = nullptr;
			}
		}
	}
	return hBitmap;
}

HRESULT SVImageConvertorGDI::WriteBStrImageToFile(const _bstr_t& rFileName, BSTR Image)
{
	HRESULT l_Status = S_OK;

	unsigned long l_TotalSize = ::SysStringByteLen(Image);

	if (sizeof(BITMAPINFO) < l_TotalSize)
	{
		// Create the .BMP file.  
		HANDLE hf = CreateFile(static_cast<TCHAR*>(rFileName), 
			GENERIC_READ | GENERIC_WRITE, 
			0, 
			nullptr, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			nullptr); 

		if (INVALID_HANDLE_VALUE != hf) 
		{
			unsigned long dwTmp;
			BITMAPFILEHEADER hdr;
			BITMAPINFO* pInfo = reinterpret_cast<BITMAPINFO*>(Image);

			hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
			hdr.bfSize = sizeof(BITMAPFILEHEADER) + l_TotalSize; 
			hdr.bfReserved1 = 0; 
			hdr.bfReserved2 = 0; 
			hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + pInfo->bmiHeader.biSize + pInfo->bmiHeader.biClrUsed * sizeof(RGBQUAD);

			if (!WriteFile(hf, reinterpret_cast<LPVOID>(&hdr), sizeof(hdr), reinterpret_cast<LPDWORD>(&dwTmp), nullptr)) 
			{
				l_Status = E_FAIL;
			}

			if (S_OK == l_Status)
			{
				if (!WriteFile(hf, reinterpret_cast<LPVOID>(Image), l_TotalSize, reinterpret_cast<LPDWORD>(&dwTmp), nullptr)) 
				{
					l_Status = E_FAIL;
				}
			}
			// Close the .BMP file.  
			CloseHandle(hf);
		}
		else
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

