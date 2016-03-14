//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HBitmapUtilities
//* .File Name       : $Workfile:   HBitmapUtilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:13:18  $
//******************************************************************************

#include "stdafx.h"
#include "HBitmapUtilities.h"

HRESULT SVGetDIBSection( HBITMAP hDIB, DIBSECTION& rDibSection )
{
	HRESULT hr = S_OK;
	if(::GetObject(hDIB, sizeof(DIBSECTION), &rDibSection) == 0)
		hr = S_FALSE;
	return hr;
}

HRESULT SVCopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo)
{
	BITMAPINFOHEADER* pbmhInfo = &(pbmInfo->bmiHeader);
	HBITMAP hbmOld = (HBITMAP) ::SelectObject( hDC, hDIB );

	// Set the bits of our new bitmap
	int iRet = ::StretchDIBits( hDC,
		                        0, 0,
		                        pbmhInfo->biWidth,   abs(pbmhInfo->biHeight),
		                        0,                   abs(pbmhInfo->biHeight),
		                        pbmhInfo->biWidth, -(abs(pbmhInfo->biHeight)),
		                        pSrcBits, pbmInfo,
		                        DIB_RGB_COLORS, SRCCOPY );
	::GdiFlush();
	//::SelectObject( hDC, hPalOld );
	::SelectObject( hDC, hbmOld );
	return S_OK;
}// end SVCopyDIBitsFlip


HRESULT SVCopyDIBitsFast(const SVDIBITMAPINFO svSrcDIB, SVDIBITMAPINFO& rsvDestDIB )
{
	HRESULT hr = S_FALSE;

	const BITMAPINFOHEADER* pSrcHeader = svSrcDIB.Header();
	BITMAPINFOHEADER* pDestHeader = rsvDestDIB.Header();
	
	ASSERT( pSrcHeader->biSizeImage == pDestHeader->biSizeImage );
	ASSERT( pSrcHeader->biWidth == pDestHeader->biWidth );
	ASSERT( abs(pSrcHeader->biHeight) == abs(pDestHeader->biHeight) );
	ASSERT( pSrcHeader->biClrUsed == pDestHeader->biClrUsed );
	ASSERT( pSrcHeader->biHeight != 0 );

	bool bFormatMatches =
		 (    abs(pSrcHeader->biHeight) == abs(pDestHeader->biHeight)
		   && pSrcHeader->biWidth == pDestHeader->biWidth
		   && pSrcHeader->biSizeImage == pDestHeader->biSizeImage
		   && pSrcHeader->biClrUsed == pDestHeader->biClrUsed
		   && pSrcHeader->biHeight != 0
		 );
	
	if ( bFormatMatches )
	{
		if ( pSrcHeader->biHeight == pDestHeader->biHeight )	// if sign matches
		{
			memcpy( rsvDestDIB.pBits, svSrcDIB.pBits, pSrcHeader->biSizeImage );
			hr = S_OK;
		}
		else
		{
			hr = SVRasterCopyFlip(svSrcDIB, rsvDestDIB);
		}
		//pDestHeader->biHeight = pSrcHeader->biHeight;	// ensure the sign is the same
	}

	return hr;
}// end SVCopyDIBitsFast

// don't call this directly; call SVCopyDIBitsFast
HRESULT SVRasterCopyFlip(const SVDIBITMAPINFO svSrcDIB, SVDIBITMAPINFO& rsvDestDIB )
{
	HRESULT hr = S_FALSE;

	const BITMAPINFOHEADER* pSrcHeader = svSrcDIB.Header();
	BITMAPINFOHEADER* pDestHeader = rsvDestDIB.Header();
	
	ASSERT( pSrcHeader->biSizeImage == pDestHeader->biSizeImage );
	ASSERT( pSrcHeader->biWidth == pDestHeader->biWidth );
	ASSERT( abs(pSrcHeader->biHeight) == abs(pDestHeader->biHeight) );
	ASSERT( pSrcHeader->biClrUsed == pDestHeader->biClrUsed );
	ASSERT( pSrcHeader->biHeight != 0 );

	// do a raster-line copy of the image, flipping it
	int iRasterLineSize = ( ( ( ( pSrcHeader->biWidth * pSrcHeader->biBitCount ) + 31 ) & ~31 ) >> 3 );

	const BYTE* pSrcLine = reinterpret_cast<BYTE *> (svSrcDIB.pBits);
	BYTE* pDestLine = (reinterpret_cast<BYTE *> (rsvDestDIB.pBits)) + (iRasterLineSize * (abs(pDestHeader->biHeight)-1));
	for ( int iLine = 0; iLine < pSrcHeader->biHeight; iLine++, pSrcLine += iRasterLineSize, pDestLine -= iRasterLineSize )
	{
		memcpy( pDestLine, pSrcLine, iRasterLineSize );
	}

	hr = S_OK;

	return hr;
}// end SVRasterCopyFlip

BITMAPINFO* CreateBitmapInfoStruct( ImageDefinitionStruct& rImageDef )
{ 
	BITMAPINFO* pbmi; 
	WORD wClrBits = 8;
	
	if ( rImageDef.eImageFormat == SVImageFormatRGB888 )
		wClrBits = 24;
	else if ( rImageDef.eImageFormat == SVImageFormatRGB8888 )
		wClrBits = 32;
	
	// Allocate memory for the BITMAPINFO structure. (This structure 
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
	// data structures.) 
	
	if (wClrBits == 8) 
	{
		pbmi = (PBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * (1<< wClrBits)]; 
	}
	else // There is no RGBQUAD array for the 24-bit-per-pixel format. 
	{
		pbmi = (PBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)]; 
	}
	
	// Initialize the fields in the BITMAPINFO structure. 
	
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = rImageDef.lWidth; 
	pbmi->bmiHeader.biHeight = -rImageDef.lHeight; 
	pbmi->bmiHeader.biPlanes = 1; //?
	pbmi->bmiHeader.biBitCount = wClrBits; 
	if (wClrBits < 24)
	{
		pbmi->bmiHeader.biClrUsed = (1<<wClrBits); 
	}
	
	// If the bitmap is not compressed, set the BI_RGB flag. 
	pbmi->bmiHeader.biCompression = BI_RGB;
	
	// Compute the number of bytes in the array of color 
	// indices and store the result in biSizeImage. 
	// For Windows NT, the width must be DWORD aligned unless 
	// the bitmap is RLE compressed. This example shows this. 
	// For Windows 95/98/Me, the width must be WORD aligned unless the 
	// bitmap is RLE compressed.
	pbmi->bmiHeader.biSizeImage = ((abs(pbmi->bmiHeader.biWidth) * wClrBits +31) & ~31) /8 * abs(pbmi->bmiHeader.biHeight); 

	// Set biClrImportant to 0, indicating that all of the 
	// device colors are important. 
	pbmi->bmiHeader.biClrImportant = 0; 

	ASSERT( pbmi );
	return pbmi; 
}// end CreateBitmapInfoStruct

HRESULT SVImageInfoToImageDefinitionStruct( const SVImageInfoClass& rInfo, ImageDefinitionStruct& rImageDef )
{
	long l_lBandNumber = 1;
	long l_lFormat = SVImageFormatUnknown;

	rInfo.GetProperty( SVExtentPropertyWidth, rImageDef.lWidth );
	rInfo.GetProperty( SVExtentPropertyHeight, rImageDef.lHeight );

	rInfo.GetProperty( SVImagePropertyFormat, l_lFormat ); 
	rInfo.GetProperty( SVImagePropertyBandNumber, l_lBandNumber ); 

	if( rImageDef.eImageFormat == SVImageFormatRGB888 &&
		rImageDef.eImageFormat == SVImageFormatRGB8888 &&
		l_lBandNumber == 1 )
	{
		rImageDef.eImageFormat = SVImageFormatMono8;
	}
	else
	{
		rImageDef.eImageFormat = (SVImageFormatEnum)l_lFormat;
	}

	return S_OK;
}// end SVImageInfoToImageDefinitionStruct

SVDIBITMAPINFO SVCreateHBitmap( ImageDefinitionStruct& rImageDef, RGBQUAD* pColorTable )
{
	BITMAPINFO* pbmInfo=CreateBitmapInfoStruct(rImageDef);
	ASSERT( pbmInfo );

	switch( rImageDef.eImageFormat )
	{
		case  SVImageFormatMono8:
		{
			memcpy(&(pbmInfo->bmiColors[0]), pColorTable, sizeof(RGBQUAD) * 256);
			break;
		}
		case SVImageFormatRGB888:
			break;
		case SVImageFormatRGB8888:
			break;
		case SVImageFormatUnknown:
			break;
		default:
			break;
	}

	void* pBits = NULL;
	HBITMAP hbmResult = ::CreateDIBSection( NULL, pbmInfo, DIB_RGB_COLORS, &pBits, NULL, 0 );
	return SVDIBITMAPINFO( hbmResult, pBits, pbmInfo );

}// end SVCreateHBitmap

HRESULT SVImageInfoToNewDIB( const SVImageInfoClass& info, SVDIBITMAPINFO& rsvDIB )
{
	static SVColorTable<SVImageFormatMono8> l_MonoColorTable;

	HRESULT hr = S_OK;

	ASSERT( rsvDIB.hbm == NULL );

	ImageDefinitionStruct l_ImageDef;
	SVImageInfoToImageDefinitionStruct( info, l_ImageDef );
	
	SVDIBITMAPINFO l_Bitmap = SVCreateHBitmap( l_ImageDef, &(l_MonoColorTable.ColorTable[0]) );
	
	rsvDIB = l_Bitmap;

	//l_Bitmap.FreeBitmapInfo();

	return hr;

}// end SVImageInfoToNewDIB

HRESULT SVHBitmapToDC(const SVDIBITMAPINFO svSrcDIB, HDC hDC)
{
	HRESULT hr = S_OK;

	CDC* pdc = CDC::FromHandle( hDC );
	CSize sizeDest = pdc->GetWindowExt();

	CDC memDC;
	memDC.CreateCompatibleDC( pdc );
	::SelectObject(memDC.GetSafeHdc(), svSrcDIB.hbm );

	CSize sizeSrc(svSrcDIB.pBitmapInfo->bmiHeader.biWidth, svSrcDIB.pBitmapInfo->bmiHeader.biHeight);

	pdc->StretchBlt( 0, 0, sizeDest.cx, sizeDest.cy, 
				   &memDC, 0, 0, sizeSrc.cx, sizeSrc.cy, SRCCOPY );

	memDC.DeleteDC();
	CDC::DeleteTempMap();

	return hr;
}

BOOL SVHBitmapToBstr(HBITMAP hbm, BSTR& rbstr)
{
	ASSERT( rbstr == NULL );	// clean up and set to NULL before calling

	HRESULT hr = S_OK;

	if ( hbm != NULL )
	{
		BITMAPINFOHEADER* pbmhInfo;
		BYTE* pBytes;
		long lNumColor;
		long lHeight;
		long lBufSize;
		long lTabSize;

		DIBSECTION dib;

		if(::GetObject(hbm, sizeof(dib), &dib) == 0)
			return FALSE;

		pbmhInfo = &dib.dsBmih;

		// Source images seem to be flipped even though MIL is not supposed to flip them
		//if( pbmhInfo->biHeight > 0 )
		//	pbmhInfo->biHeight =  pbmhInfo->biHeight * -1;
		
		// Calculate the absolute height
		lHeight = abs( pbmhInfo->biHeight );

		// Make sure image size is calculated
		if( 0 == pbmhInfo->biSizeImage )
			pbmhInfo->biSizeImage = ( ( ( ( pbmhInfo->biWidth * pbmhInfo->biBitCount ) + 31 ) & ~31 ) >> 3 ) * lHeight;

		// Make sure color table size is calculated
		lNumColor = pbmhInfo->biClrUsed;
		if( 0 == lNumColor )
		{
		   if( pbmhInfo->biBitCount != 24 )
			  lNumColor = 1 << pbmhInfo->biBitCount;
		}
		lTabSize = lNumColor * sizeof( RGBQUAD );

		// Calculate total size buffer needed for image
		lBufSize = sizeof( BITMAPINFOHEADER ) + lTabSize + pbmhInfo->biSizeImage;

		// Alloc BSTR to store image : in this structure <Color Table Size - BITMAPINFO (incl Color Table) - bitmap bits>
		rbstr = ::SysAllocStringByteLen( NULL, lBufSize );
		pBytes = (BYTE*) rbstr;
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

			hOldBitmap = (HBITMAP) SelectObject( hMemDC, hbm);

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
		void* pDIBits=NULL;
		HDC hDC = CreateCompatibleDC(NULL);
		HBITMAP hFlipDIB = ::CreateDIBSection( hDC, pbmInfo, DIB_RGB_COLORS, &pDIBits, NULL, 0 );
		SVCopyDIBitsFlip( hDC, hFlipDIB, pBits, pbmInfo );
		pBits = pDIBits;
		//////////////////////////////////
#endif
		memcpy( pBytes + sizeof( BITMAPINFOHEADER ) + lTabSize, pBits, pbmhInfo->biSizeImage );

		::DeleteDC( hDC );
		::DeleteObject( hFlipDIB );
	}

	return TRUE;
}

