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
#include <comdef.h>
#include "SVUtilityLibrary/SVUtilityGlobals.h"
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

	static HRESULT BitmapToBStr(HBITMAP Bitmap, BSTR& rImage);
	static HRESULT BStrToBitmap(BSTR Image, HBITMAP& rBitmap);

	static HRESULT BitmapToByteVector(HBITMAP Bitmap, SVByteVector& rImage);
	static HRESULT ByteVectorToBitmap(const SVByteVector& Image, HBITMAP& rBitmap);

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
	static HBITMAP CopyBitmap(HBITMAP SourceBitmap);

	static HBITMAP CreateDIB(BITMAPINFO* pbmInfo, void* pSrc);
	static HBITMAP CreateDIB(BITMAPINFO* pBmpInfo, void* pSrc, long srcPitch);
	static HBITMAP CreateDIB(BITMAPINFO* pBmpInfo, void* pSrc, long srcPitch, long pixelOffset, long bytesPerPixel);

	static HRESULT WriteBStrImageToFile(const _bstr_t& rFileName, BSTR Image);
};

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSystemLibrary\SVImageConvertorGDI.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:52:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Oct 2010 15:12:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to include new utility classes for bitmap management and conversion, and file minipulation.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Apr 2010 12:58:32   jspila
 * Project: SVSVIMInterface
 * Change Request (SCR) nbr: 62
 * SCR Title: Update Font Object with New Interface
 * Checked in by: Joe; Joe Spila
 * Change Description: Initial Check-in
 * \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

