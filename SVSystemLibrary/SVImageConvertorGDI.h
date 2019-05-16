//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageConvertorGDI
//* .File Name       : $Workfile:   SVImageConvertorGDI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:52:02  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#pragma endregion Includes

class SVImageConvertorGDI
{
private:
	// Non Constructable
	SVImageConvertorGDI();
	~SVImageConvertorGDI();

public:
	static HRESULT GetBitmapFromIPictureDisp(IPictureDisp* pPictureDisp, HBITMAP& rBitmap);
	static HRESULT GetIPictureDispFromBitmap(HBITMAP Bitmap, IPictureDisp** ppPictureDisp);

	static HRESULT BitmapToByteVector(HBITMAP Bitmap, std::vector<unsigned char>& rImage);
	static HRESULT ByteVectorToBitmap(const std::vector<unsigned char>& Image, HBITMAP& rBitmap);

	static HRESULT CopyDIBits(HBITMAP SrcBitmap, BITMAPINFOHEADER* pDestInfo, void* pDestBits);
	static HRESULT CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, HBITMAP& DestBitmap);
	static HRESULT CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, BITMAPINFOHEADER* pDestInfo, void* pDestBits);

	// For Copying Child Buffer
	static HRESULT CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPicth, HBITMAP& rDestBitmap);
	static HRESULT CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPitch, BITMAPINFOHEADER* pDstInfo, void* pDstBits);
	static HRESULT CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPitch, long pixelOffset, long bytesPerPixel, HBITMAP& rDestBitmap);
	static HRESULT CopyDIBits(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, long srcPitch, long pixelOffset, long bytesPerPixel, BITMAPINFOHEADER* pDstInfo, void* pDstBits);

	static HRESULT CopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo);
	static HRESULT CopyDIBitsFlip(HBITMAP SrcBitmap, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits);
	static HRESULT CopyDIBitsFlip(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, HBITMAP& DestBitmap);
	static HRESULT CopyDIBitsFlip(BITMAPINFOHEADER* pSrcInfo, void* pSrcBits, BITMAPINFOHEADER* pDestInfo, void* pDestBits);

	static HBITMAP CreateBitmap(HBITMAP SourceBitmap, const RECT& rROI);

	static HBITMAP CreateDIB(BITMAPINFO* pbmInfo, void* pSrc);
	static HBITMAP CreateDIB(BITMAPINFO* pBmpInfo, void* pSrc, long srcPitch);
	static HBITMAP CreateDIB(BITMAPINFO* pBmpInfo, void* pSrc, long srcPitch, long pixelOffset, long bytesPerPixel);
};

