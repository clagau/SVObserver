//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVImageConvertorGDI
//* .File Name       : $Workfile:   SVImageConvertorGDI.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   14 May 2013 14:35:10  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVImageConvertorGDI.h"
#pragma endregion Includes

HRESULT SVImageConvertorGDI::GetBitmapFromIPictureDisp(IPictureDisp* pPictureDisp, HBITMAP& rBitmap, HPALETTE * rPalette)
{
	HRESULT hr = S_FALSE;
	
	if (nullptr != pPictureDisp)
	{
		IPicture* l_pPicture(NULL);
		hr = pPictureDisp->QueryInterface(IID_IPicture, (void **)&l_pPicture);

		if (S_OK == hr && l_pPicture)
		{
			hr = l_pPicture->get_Handle((OLE_HANDLE *)&rBitmap);
			if (S_OK == hr && rPalette)
			{
				hr = l_pPicture->get_hPal((OLE_HANDLE *)rPalette);
			}
			l_pPicture->Release();
		}
	}
	return hr;
}

HRESULT SVImageConvertorGDI::GetIPictureDispFromBitmap( HBITMAP Bitmap, IPictureDisp** ppPictureDisp )
{
	HRESULT hr = S_FALSE;
	
	if( nullptr != ppPictureDisp )
	{
		if( nullptr != *ppPictureDisp )
		{
			( *ppPictureDisp )->Release();

			*ppPictureDisp = nullptr;
		}

		// Create the IPictureDisp
		PICTDESC pictDesc;
		pictDesc.cbSizeofstruct = sizeof( PICTDESC ); 
		pictDesc.picType = PICTYPE_BITMAP;
		pictDesc.bmp.hbitmap = Bitmap;

		// 
		IPictureDisp* l_pPictureDisp = nullptr;
		hr = ::OleCreatePictureIndirect( &pictDesc, IID_IPictureDisp, true, (LPVOID *)&l_pPictureDisp );
		if( S_OK == hr )
		{
			*ppPictureDisp = l_pPictureDisp;
		}
	}
	return hr;
}

HRESULT SVImageConvertorGDI::BitmapToBStr( HBITMAP p_Bitmap, BSTR& p_rImage )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_rImage )
	{
		::SysFreeString( p_rImage );

		p_rImage = nullptr;
	}

	DIBSECTION dib;

	if( nullptr != p_Bitmap )
	{
		if( 0 == ::GetObject( p_Bitmap, sizeof( dib ), &dib ) )
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( S_OK == l_Status )
	{
		BITMAPINFOHEADER* pbmhInfo;
		BYTE* pBytes;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;

		pbmhInfo = &dib.dsBmih;

		// Source images seem to be flipped even though MIL is not supposed to flip them
		//if( pbmhInfo->biHeight > 0 )
		//	pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
		
		// Calculate the absolute height
		lHeight = abs( pbmhInfo->biHeight );

		// Make sure image size is calculated
		if( 0 == pbmhInfo->biSizeImage )
		{
			pbmhInfo->biSizeImage = ( ( ( ( pbmhInfo->biWidth * pbmhInfo->biBitCount ) + 31 ) & ~31 ) >> 3 ) * lHeight;
		}
		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if( 0 == lNumColor )
		{
			if( 24 != pbmhInfo->biBitCount )
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		lTabSize = lNumColor * sizeof( RGBQUAD );

		// Calculate total size buffer needed for image
		lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;

		// Alloc BSTR to store image : in this structure <Color Table Size - BITMAPINFO (incl Color Table) - bitmap bits>
		p_rImage = ::SysAllocStringByteLen( NULL, lBufSize );
		pBytes = (BYTE*) p_rImage;
		BITMAPINFO* pbmInfo = (BITMAPINFO*) pBytes;
		
		// Copy data to BSTR memory locations
		memcpy( pBytes, pbmhInfo, sizeof( BITMAPINFOHEADER ) );

		//********************************************************
		// Get Color Table information.

		// If has color table 
		if(dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes <= 8)
		{
			HDC hMemDC;
			HBITMAP hOldBitmap;
			int nColors;

			// Find out how many colors are in the color table
			nColors = 1 << (dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes);
			// Create a memory DC and select the DIBSection into it
			hMemDC = CreateCompatibleDC(NULL);

			hOldBitmap = (HBITMAP) SelectObject( hMemDC, p_Bitmap );

			// Get the DIBSection's color table
			GetDIBColorTable(hMemDC, 0, nColors, (RGBQUAD *) (pBytes + sizeof(BITMAPINFOHEADER)) );

			// Cleanup
			SelectObject(hMemDC,hOldBitmap);
			DeleteDC(hMemDC);
		}

		void* pBits = dib.dsBm.bmBits;

#if 1
		//////////////////////////////////
		// flip the image for SVObserver
		void* pDIBits=nullptr;
		HDC hDC = CreateCompatibleDC(nullptr);
		HBITMAP hFlipDIB = ::CreateDIBSection( hDC, pbmInfo, DIB_RGB_COLORS, &pDIBits, nullptr, 0 );
		CopyDIBitsFlip( hDC, hFlipDIB, pBits, pbmInfo );
		pBits = pDIBits;
		//////////////////////////////////
#endif
		memcpy( pBytes + sizeof( BITMAPINFOHEADER ) + lTabSize, pBits, pbmhInfo->biSizeImage );

		::DeleteDC( hDC );
		::DeleteObject( hFlipDIB );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageConvertorGDI::BStrToBitmap( BSTR p_Image, HBITMAP& p_rBitmap )
{
	HRESULT hr = S_FALSE;

	BITMAPINFOHEADER* pbmhInfo=nullptr;
	BITMAPINFO* pbmInfo=nullptr;
	long lTabSize;
	long lNumColor;
	HBITMAP hBitmap=nullptr;
	HDC hDC=nullptr;
	BYTE* pBytes=nullptr;
	void* pBits=nullptr;

	if( p_Image && 0 != (SysStringByteLen (p_Image) ))
	{
		// Find out how big it is and get pointers to BITMAPINFO and the bitmap bits
		pBytes   = (BYTE*) p_Image;
		pbmInfo  = (BITMAPINFO*) pBytes;
		pbmhInfo = (BITMAPINFOHEADER*) &pbmInfo->bmiHeader;

		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if( 0 == lNumColor )
		{
			if( 24 != pbmhInfo->biBitCount )
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		lTabSize = lNumColor * sizeof( RGBQUAD );

		pBytes   = pBytes + sizeof( BITMAPINFOHEADER ) + ( lTabSize );

		// Create DC and a DIB bitmap
		hDC = CreateCompatibleDC(nullptr);
		pbmhInfo->biHeight = abs(pbmhInfo->biHeight);	// always make positive
		hBitmap = ::CreateDIBSection( hDC, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0 );
		
#if 1
		CopyDIBitsFlip( hDC, hBitmap, pBytes, pbmInfo);
#else
		// Set the bits of our new bitmap
		memcpy( pBits, pBytes, pbmhInfo->biSizeImage );
#endif
		::DeleteDC(hDC);

		p_rBitmap = hBitmap;

		hr = S_OK;	
	}
	return hr;
}

HRESULT SVImageConvertorGDI::BitmapToByteVector( HBITMAP p_Bitmap, SVByteVector& p_rImage )
{
	HRESULT l_Status = S_OK;

	p_rImage.clear();

	DIBSECTION dib;

	if( nullptr != p_Bitmap )
	{
		if( 0 == ::GetObject( p_Bitmap, sizeof( dib ), &dib ) )
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( S_OK == l_Status )
	{
		BITMAPINFOHEADER* pbmhInfo;
		BYTE* pBytes;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;

		pbmhInfo = &dib.dsBmih;

		// Source images seem to be flipped even though MIL is not supposed to flip them
		//if( pbmhInfo->biHeight > 0 )
		//	pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
		
		// Calculate the absolute height
		lHeight = abs( pbmhInfo->biHeight );

		// Make sure image size is calculated
		if( 0 == pbmhInfo->biSizeImage )
		{
			pbmhInfo->biSizeImage = ( ( ( ( pbmhInfo->biWidth * pbmhInfo->biBitCount ) + 31 ) & ~31 ) >> 3 ) * lHeight;
		}
		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if( 0 == lNumColor )
		{
			if( 24 != pbmhInfo->biBitCount )
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		lTabSize = lNumColor * sizeof( RGBQUAD );

		// Calculate total size buffer needed for image
		lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;

		// Alloc BSTR to store image : in this structure <Color Table Size - BITMAPINFO (incl Color Table) - bitmap bits>
		p_rImage.resize( lBufSize );
		pBytes = (BYTE*)( &( p_rImage[ 0 ] ) );
		BITMAPINFO* pbmInfo = (BITMAPINFO*) pBytes;
		
		// Copy data to BSTR memory locations
		memcpy( pBytes, pbmhInfo, sizeof( BITMAPINFOHEADER ) );

		//********************************************************
		// Get Color Table information.

		// If has color table 
		if(dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes <= 8)
		{
			HDC hMemDC;
			HBITMAP hOldBitmap;
			int nColors;

			// Find out how many colors are in the color table
			nColors = 1 << (dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes);
			// Create a memory DC and select the DIBSection into it
			hMemDC = CreateCompatibleDC(NULL);

			hOldBitmap = (HBITMAP) SelectObject( hMemDC, p_Bitmap );

			// Get the DIBSection's color table
			GetDIBColorTable(hMemDC, 0, nColors, (RGBQUAD *) (pBytes + sizeof(BITMAPINFOHEADER)) );

			// Cleanup
			SelectObject(hMemDC,hOldBitmap);
			DeleteDC(hMemDC);
		}

		void* pBits = dib.dsBm.bmBits;

#if 1
		//////////////////////////////////
		// flip the image for SVObserver
		void* pDIBits=nullptr;
		HDC hDC = CreateCompatibleDC(nullptr);
		HBITMAP hFlipDIB = ::CreateDIBSection( hDC, pbmInfo, DIB_RGB_COLORS, &pDIBits, nullptr, 0 );
		CopyDIBitsFlip( hDC, hFlipDIB, pBits, pbmInfo );
		pBits = pDIBits;
		//////////////////////////////////
#endif
		memcpy( pBytes + sizeof( BITMAPINFOHEADER ) + lTabSize, pBits, pbmhInfo->biSizeImage );

		::DeleteDC( hDC );
		::DeleteObject( hFlipDIB );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVImageConvertorGDI::ByteVectorToBitmap( const SVByteVector& p_Image, HBITMAP& p_rBitmap )
{
	HRESULT hr = S_FALSE;

	BITMAPINFOHEADER* pbmhInfo=nullptr;
	BITMAPINFO* pbmInfo=nullptr;
	long lTabSize;
	long lNumColor;
	HBITMAP hBitmap=nullptr;
	HDC hDC=nullptr;
	BYTE* pBytes=nullptr;
	void* pBits=nullptr;

	if( 0 < p_Image.size() )
	{
		// Find out how big it is and get pointers to BITMAPINFO and the bitmap bits
		pBytes   = (BYTE*)( &( p_Image[ 0 ] ) );
		pbmInfo  = (BITMAPINFO*) pBytes;
		pbmhInfo = (BITMAPINFOHEADER*) &pbmInfo->bmiHeader;

		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if( 0 == lNumColor )
		{
			if( 24 != pbmhInfo->biBitCount )
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		lTabSize = lNumColor * sizeof( RGBQUAD );

		pBytes   = pBytes + sizeof( BITMAPINFOHEADER ) + ( lTabSize );

		// Create DC and a DIB bitmap
		hDC = CreateCompatibleDC(nullptr);
		pbmhInfo->biHeight = abs(pbmhInfo->biHeight);	// always make positive
		hBitmap = ::CreateDIBSection( hDC, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0 );
		
#if 1
		CopyDIBitsFlip( hDC, hBitmap, pBytes, pbmInfo);
#else
		// Set the bits of our new bitmap
		memcpy( pBits, pBytes, pbmhInfo->biSizeImage );
#endif
		::DeleteDC(hDC);

		p_rBitmap = hBitmap;

		hr = S_OK;	
	}
	return hr;
}

HRESULT SVImageConvertorGDI::BitmapToBytes( HBITMAP p_Bitmap, bytes& p_rImage )
{
	HRESULT l_Status = S_OK;

	DIBSECTION dib;

	if( nullptr != p_Bitmap )
	{
		if( 0 == ::GetObject( p_Bitmap, sizeof( dib ), &dib ) )
		{
			l_Status = E_FAIL;
		}
	}
	else
	{
		l_Status = E_FAIL;
	}

	if( S_OK == l_Status )
	{
		BITMAPINFOHEADER* pbmhInfo;
		BYTE* pBytes;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;

		pbmhInfo = &dib.dsBmih;

		// Source images seem to be flipped even though MIL is not supposed to flip them
		//if( pbmhInfo->biHeight > 0 )
		//	pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
		
		// Calculate the absolute height
		lHeight = abs( pbmhInfo->biHeight );

		// Make sure image size is calculated
		if( 0 == pbmhInfo->biSizeImage )
		{
			pbmhInfo->biSizeImage = ( ( ( ( pbmhInfo->biWidth * pbmhInfo->biBitCount ) + 31 ) & ~31 ) >> 3 ) * lHeight;
		}
		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if( 0 == lNumColor )
		{
			if( 24 != pbmhInfo->biBitCount )
			{
				lNumColor = 1 << pbmhInfo->biBitCount;
			}
		}
		lTabSize = lNumColor * sizeof( RGBQUAD );

		// Calculate total size buffer needed for image
		lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;

		// Alloc BSTR to store image : in this structure <Color Table Size - BITMAPINFO (incl Color Table) - bitmap bits>
		pBytes = new BYTE[lBufSize];
		BITMAPINFO* pbmInfo = (BITMAPINFO*) pBytes;
		
		// Copy data to BSTR memory locations
		memcpy( pBytes, pbmhInfo, sizeof( BITMAPINFOHEADER ) );

		//********************************************************
		// Get Color Table information.

		// If has color table 
		if(dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes <= 8)
		{
			HDC hMemDC;
			HBITMAP hOldBitmap;
			int nColors;

			// Find out how many colors are in the color table
			nColors = 1 << (dib.dsBm.bmBitsPixel * dib.dsBm.bmPlanes);
			// Create a memory DC and select the DIBSection into it
			hMemDC = CreateCompatibleDC(nullptr);

			hOldBitmap = (HBITMAP) SelectObject( hMemDC, p_Bitmap );

			// Get the DIBSection's color table
			GetDIBColorTable(hMemDC, 0, nColors, (RGBQUAD *) (pBytes + sizeof(BITMAPINFOHEADER)) );

			// Cleanup
			SelectObject(hMemDC,hOldBitmap);
			DeleteDC(hMemDC);
		}

		void* pBits = dib.dsBm.bmBits;

#if 1
		//////////////////////////////////
		// flip the image for SVObserver
		void* pDIBits=nullptr;
		HDC hDC = CreateCompatibleDC(nullptr);
		HBITMAP hFlipDIB = ::CreateDIBSection( hDC, pbmInfo, DIB_RGB_COLORS, &pDIBits, nullptr, 0 );
		CopyDIBitsFlip( hDC, hFlipDIB, pBits, pbmInfo );
		pBits = pDIBits;
		//////////////////////////////////
#endif
		memcpy( pBytes + sizeof( BITMAPINFOHEADER ) + lTabSize, pBits, pbmhInfo->biSizeImage );

		::DeleteDC( hDC );
		::DeleteObject( hFlipDIB );

		bytes tmp(pBytes);
		p_rImage.swap(tmp);
	}
	else
	{
		l_Status = E_FAIL;
	}
	return l_Status;
}

HRESULT SVImageConvertorGDI::CopyDIBits( HBITMAP p_SrcBitmap, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_SrcBitmap && nullptr != p_pDestInfo && nullptr != p_pDestBits )
	{
		DIBSECTION dib;

		if( 0 != ::GetObject( p_SrcBitmap, sizeof( dib ), &dib ) )
		{
			l_Status = CopyDIBits( &( dib.dsBmih ), dib.dsBm.bmBits, p_pDestInfo, p_pDestBits );
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

HRESULT SVImageConvertorGDI::CopyDIBits( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, HBITMAP& p_rDestBitmap )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pSrcInfo && nullptr != p_pSrcBits && nullptr != p_rDestBitmap )
	{
		DIBSECTION dib;

		if( 0 != ::GetObject( p_rDestBitmap, sizeof( dib ), &dib ) )
		{
			l_Status = CopyDIBits( p_pSrcInfo, p_pSrcBits, &( dib.dsBmih ), dib.dsBm.bmBits );
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

HRESULT SVImageConvertorGDI::CopyDIBits( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pSrcInfo && nullptr != p_pSrcBits && nullptr != p_pDestInfo && nullptr != p_pDestBits )
	{
		if( p_pSrcInfo->biHeight == p_pDestInfo->biHeight )	// if sign matches
		{
			bool l_FormatMatches = ( 0 != p_pSrcInfo->biHeight );

			l_FormatMatches &= ( abs( p_pSrcInfo->biHeight ) == abs( p_pDestInfo->biHeight ) );
			l_FormatMatches &= ( p_pSrcInfo->biWidth == p_pDestInfo->biWidth );
			l_FormatMatches &= ( p_pSrcInfo->biSizeImage == p_pDestInfo->biSizeImage );
			l_FormatMatches &= ( p_pSrcInfo->biClrUsed == p_pDestInfo->biClrUsed );
			
			if( l_FormatMatches )
			{
				memcpy( p_pDestBits, p_pSrcBits, p_pSrcInfo->biSizeImage );
			}
			else
			{
				l_Status = E_INVALIDARG;
			}
		}
		else
		{
			l_Status = CopyDIBitsFlip( p_pSrcInfo, p_pSrcBits, p_pDestInfo, p_pDestBits );
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
	SetDIBits( hDC, hDIB, 0, abs( pbmhInfo->biHeight ), pSrcBits, pbmInfo, DIB_RGB_COLORS );

	::GdiFlush();

	return S_OK;
}

HRESULT SVImageConvertorGDI::CopyDIBitsFlip( HBITMAP p_SrcBitmap, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_SrcBitmap && nullptr != p_pDestInfo && nullptr != p_pDestBits )
	{
		DIBSECTION dib;

		if( 0 != ::GetObject( p_SrcBitmap, sizeof( dib ), &dib ) )
		{
			l_Status = CopyDIBitsFlip( &( dib.dsBmih ), dib.dsBm.bmBits, p_pDestInfo, p_pDestBits );
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

HRESULT SVImageConvertorGDI::CopyDIBitsFlip( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, HBITMAP& p_rDestBitmap )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pSrcInfo && nullptr != p_pSrcBits && nullptr != p_rDestBitmap )
	{
		DIBSECTION dib;

		if( 0 != ::GetObject( p_rDestBitmap, sizeof( dib ), &dib ) )
		{
			l_Status = CopyDIBitsFlip( p_pSrcInfo, p_pSrcBits, &( dib.dsBmih ), dib.dsBm.bmBits );
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

HRESULT SVImageConvertorGDI::CopyDIBitsFlip( BITMAPINFOHEADER* p_pSrcInfo, void* p_pSrcBits, BITMAPINFOHEADER* p_pDestInfo, void* p_pDestBits )
{
	HRESULT l_Status = S_OK;

	if( nullptr != p_pSrcInfo && nullptr != p_pSrcBits && nullptr != p_pDestInfo && nullptr != p_pDestBits )
	{
		bool l_FormatMatches = ( 0 != p_pSrcInfo->biHeight );

		l_FormatMatches &= ( abs( p_pSrcInfo->biHeight ) == abs( p_pDestInfo->biHeight ) );
		l_FormatMatches &= ( p_pSrcInfo->biWidth == p_pDestInfo->biWidth );
		l_FormatMatches &= ( p_pSrcInfo->biSizeImage == p_pDestInfo->biSizeImage );
		l_FormatMatches &= ( p_pSrcInfo->biClrUsed == p_pDestInfo->biClrUsed );
		
		if( l_FormatMatches )
		{
			int iRasterLineSize = ( ( ( ( p_pSrcInfo->biWidth * p_pSrcInfo->biBitCount ) + 31 ) & ~31 ) >> 3 );

			const BYTE* pSrcLine = reinterpret_cast< BYTE* >( p_pSrcBits );
			BYTE* pDestLine = ( reinterpret_cast< BYTE* >( p_pDestBits ) ) + ( iRasterLineSize * ( abs( p_pDestInfo->biHeight ) - 1 ) );
			for( int iLine = 0; iLine < p_pSrcInfo->biHeight; iLine++, pSrcLine += iRasterLineSize, pDestLine -= iRasterLineSize )
			{
				memcpy( pDestLine, pSrcLine, iRasterLineSize );
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

HBITMAP SVImageConvertorGDI::CreateBitmap( HBITMAP p_SourceBitmap, const RECT& p_rROI )
{
	HBITMAP l_Bitmap = nullptr;

	if( nullptr != p_SourceBitmap )
	{
		DIBSECTION l_DIBSection;
		GetObject( p_SourceBitmap, sizeof(DIBSECTION), &l_DIBSection );

		HDC l_DC = ::GetDC( nullptr );
		HDC l_SourceDC = ::CreateCompatibleDC( l_DC );
		HGDIOBJ l_OldSourceBitmap = ::SelectObject( l_SourceDC, p_SourceBitmap );

		long l_BitmapInfoSize = sizeof( BITMAPINFO ) + ( sizeof( RGBQUAD ) * l_DIBSection.dsBmih.biClrUsed );
		unsigned char* l_pBuffer = new unsigned char[ l_BitmapInfoSize ];
		BITMAPINFO* l_pInfo = reinterpret_cast< BITMAPINFO* >( l_pBuffer );

		void* l_pBits = nullptr;
		long l_Stride = ( ::labs( p_rROI.right - p_rROI.left + 1 ) * ( l_DIBSection.dsBmih.biBitCount / 8 ) + 3 ) & ~3;
		long l_ImageSize = l_Stride * ::labs( p_rROI.bottom - p_rROI.top + 1 );

		l_pInfo->bmiHeader.biBitCount = l_DIBSection.dsBmih.biBitCount;
		l_pInfo->bmiHeader.biClrImportant = 0;
		l_pInfo->bmiHeader.biClrUsed = 0;
		l_pInfo->bmiHeader.biCompression = BI_RGB;
		l_pInfo->bmiHeader.biHeight = ( p_rROI.bottom - p_rROI.top + 1 );
		l_pInfo->bmiHeader.biPlanes = l_DIBSection.dsBmih.biPlanes;
		l_pInfo->bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
		l_pInfo->bmiHeader.biSizeImage = l_ImageSize;
		l_pInfo->bmiHeader.biWidth = ( p_rROI.right - p_rROI.left + 1 );

		if( 0 < l_DIBSection.dsBmih.biClrUsed )
		{
			RGBQUAD* l_pTable = &( l_pInfo->bmiColors[0] );

			::GetDIBColorTable( l_SourceDC, 0, l_DIBSection.dsBmih.biClrUsed, l_pTable );
		}

		HDC l_DestDC = ::CreateCompatibleDC( l_DC );
		HBITMAP l_hRoiBitmap = ::CreateDIBSection( l_DestDC, l_pInfo, DIB_RGB_COLORS, &l_pBits, NULL, 0 );
		HGDIOBJ l_OldDestBitmap = ::SelectObject( l_DestDC, l_hRoiBitmap );

		if( ::BitBlt( l_DestDC, 0, 0, ( p_rROI.right - p_rROI.left + 1 ), ( p_rROI.bottom - p_rROI.top + 1 ), l_SourceDC, p_rROI.left, p_rROI.top, SRCCOPY ) )
		{
			l_Bitmap = l_hRoiBitmap;
		}

		::SelectObject( l_DestDC, l_OldDestBitmap );
		::SelectObject( l_SourceDC, l_OldSourceBitmap );
		::ReleaseDC( nullptr, l_DC );

		delete l_pBuffer;
	}

	return l_Bitmap;
}

HBITMAP SVImageConvertorGDI::CopyBitmap( HBITMAP p_SourceBitmap )
{
	BITMAP bmp;
	GetObject(p_SourceBitmap, sizeof(BITMAP), &bmp);

	HBITMAP hBitmap = (HBITMAP)CopyImage(p_SourceBitmap, IMAGE_BITMAP, bmp.bmWidth, bmp.bmHeight, LR_CREATEDIBSECTION);
	return hBitmap;
}

HRESULT SVImageConvertorGDI::WriteBStrImageToFile( const _bstr_t& p_rFileName, BSTR p_Image )
{
	HRESULT l_Status = S_OK;

	unsigned long l_TotalSize = ::SysStringByteLen( p_Image );

	if( sizeof( BITMAPINFO ) < l_TotalSize )
	{
		// Create the .BMP file.  
		HANDLE hf = CreateFile( static_cast< TCHAR* >( p_rFileName ), 
			GENERIC_READ | GENERIC_WRITE, 
			(DWORD) 0, 
			NULL, 
			CREATE_ALWAYS, 
			FILE_ATTRIBUTE_NORMAL, 
			(HANDLE) NULL); 

		if( INVALID_HANDLE_VALUE != hf ) 
		{
			unsigned long dwTmp;
			BITMAPFILEHEADER hdr;
			BITMAPINFO* p_pInfo = reinterpret_cast< BITMAPINFO* >( p_Image );

			hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
			hdr.bfSize = sizeof( BITMAPFILEHEADER ) + l_TotalSize; 
			hdr.bfReserved1 = 0; 
			hdr.bfReserved2 = 0; 
			hdr.bfOffBits = sizeof( BITMAPFILEHEADER ) + p_pInfo->bmiHeader.biSize + p_pInfo->bmiHeader.biClrUsed * sizeof( RGBQUAD );

			if( !WriteFile( hf, (LPVOID) &hdr, sizeof(hdr), (LPDWORD) &dwTmp,  NULL ) ) 
			{
				l_Status = E_FAIL;
			}

			if( S_OK == l_Status )
			{
				if( !WriteFile( hf, (LPVOID) p_Image, l_TotalSize, (LPDWORD) &dwTmp,  NULL ) ) 
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
