//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageConvertorGDI
//* .File Name       : $Workfile:   SVImageConvertorGDI.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 14:35:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include <comdef.h>
#include <vector>
#include <boost/shared_array.hpp>
#pragma endregion Includes

class SVImageConvertorGDI
{
private:
	// Non Constructable
	SVImageConvertorGDI();
	~SVImageConvertorGDI();

public:
	typedef std::vector< unsigned char > SVByteVector;
	typedef boost::shared_array< BYTE > bytes;

	static HRESULT GetBitmapFromIPictureDisp(IPictureDisp* pPictureDisp, HBITMAP& rBitmap, HPALETTE * rPalette = NULL);
	static HRESULT GetIPictureDispFromBitmap(HBITMAP Bitmap, IPictureDisp** ppPictureDisp);

	static HRESULT BitmapToBStr( HBITMAP p_Bitmap, BSTR& p_rImage );
	static HRESULT BStrToBitmap( BSTR p_Image, HBITMAP& p_rBitmap );

	static HRESULT BitmapToByteVector( HBITMAP p_Bitmap, SVByteVector& p_rImage );
	static HRESULT ByteVectorToBitmap( const SVByteVector& p_Image, HBITMAP& p_rBitmap );

	static HRESULT BitmapToBytes( HBITMAP p_Bitmap, bytes& p_rImage );

	static HRESULT CopyDIBits( HBITMAP p_SrcBitmap, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits );
	static HRESULT CopyDIBits( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, HBITMAP& p_DestBitmap );
	static HRESULT CopyDIBits( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits );

	static HRESULT CopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo);
	static HRESULT CopyDIBitsFlip( HBITMAP p_SrcBitmap, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits );
	static HRESULT CopyDIBitsFlip( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, HBITMAP& p_DestBitmap );
	static HRESULT CopyDIBitsFlip( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits );

	static HBITMAP CreateBitmap( HBITMAP p_SourceBitmap, const RECT& p_rROI );
	static HBITMAP CopyBitmap(HBITMAP p_SourceBitmap); 

	static HRESULT WriteBStrImageToFile( const _bstr_t& p_rFileName, BSTR p_Image );

};
