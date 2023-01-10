//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HBitmapUtilities
//* .File Name       : $Workfile:   SVHBitmapUtilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   12 Jun 2014 13:19:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVHBitmapUtilities.h"
#include "SVStatusLibrary/MessageManagerHelper.h"
//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <math.h>
#pragma endregion Includes

/**
@SVOperationName SVCopyDIBitsFlip

@SVOperationDescription This function copies a DIB from an HBITMAP into an BITMAPINFO. The resulting copy is flipped top to botton.

*/
HRESULT SVIHBitmapUtilities::SVCopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo)
{
	BITMAPINFOHEADER* pbmhInfo = &(pbmInfo->bmiHeader);
	HBITMAP hbmOld = (HBITMAP) ::SelectObject( hDC, hDIB );

	// Set the bits of our new bitmap
	/*int iRet = */::StretchDIBits( hDC,
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

/**
@SVOperationName CreateBitmapInfoStruct

@SVOperationDescription This function creats a BITMAPINFO from a SVImageDefinitionStruct.

*/
BITMAPINFO* SVIHBitmapUtilities::CreateBitmapInfoStruct( SVImageDefinitionStruct& rImageDef )
{ 
	BITMAPINFO* pbmi; 
	WORD wClrBits = 8;
	
	if ( rImageDef.eImageFormat == SvDef::SVImageFormatBGR888 )
		wClrBits = 24;
	else if ( rImageDef.eImageFormat == SvDef::SVImageFormatBGR888X )
		wClrBits = 32;
	
	// Allocate memory for the BITMAPINFO structure. (This structure 
	// contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
	// data structures.) 
	
	if (wClrBits == 8) 
	{
		pbmi = ( PBITMAPINFO ) new BYTE[ sizeof( BITMAPINFOHEADER ) + sizeof( RGBQUAD ) * static_cast< DWORD >( 1 << wClrBits ) ]; 
	}
	else // There is no RGBQUAD array for the 24-bit-per-pixel format. 
	{
		pbmi = (PBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)]; 
	}
	
	// Initialize the fields in the BITMAPINFO structure. 
	
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
	pbmi->bmiHeader.biWidth = rImageDef.lWidth; 
	pbmi->bmiHeader.biHeight = -( abs( rImageDef.lHeight ) ); 
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

	Log_Assert(pbmi );
	return pbmi; 
}// end CreateBitmapInfoStruct


/**
@SVOperationName SVImageInfoToImageDefinitionStruct

@SVOperationDescription This function fills a SVImageDefinitionStruct from a BITMAPINFOHEADER.

*/
HRESULT SVIHBitmapUtilities::SVImageInfoToImageDefinitionStruct( const BITMAPINFOHEADER& rInfo, SVImageDefinitionStruct& rImageDef )
{
	rImageDef.lWidth = rInfo.biWidth ;
	rImageDef.lHeight = rInfo.biHeight ;

	if( rInfo.biBitCount == 8 )
	{
		rImageDef.eImageFormat = SvDef::SVImageFormatMono8;
	}
	else
	if( rInfo.biBitCount == 32 )
	{
		rImageDef.eImageFormat = SvDef::SVImageFormatBGR888X;
	}
	else
	{
		rImageDef.eImageFormat = SvDef::SVImageFormatUnknown;
	}
	return S_OK;
}// end SVImageInfoToImageDefinitionStruct


/**
@SVOperationName SVCreateHBitmap

@SVOperationDescription This function creates a SVDIBITMAPINFO with a SVImageDefinitionStruct and a color table as inputs.

*/
SVDIBITMAPINFO SVIHBitmapUtilities::SVCreateHBitmap( SVImageDefinitionStruct& rImageDef, RGBQUAD* pColorTable )
{
	BITMAPINFO* pbmInfo=CreateBitmapInfoStruct(rImageDef);
	Log_Assert( pbmInfo );

	switch( rImageDef.eImageFormat )
	{
		case  SvDef::SVImageFormatMono8:
		{
			memcpy(&(pbmInfo->bmiColors[0]), pColorTable, sizeof(RGBQUAD) * 256);
			break;
		}
		case SvDef::SVImageFormatBGR888:
			break;
		case SvDef::SVImageFormatBGR888X:
			break;
		case SvDef::SVImageFormatUnknown:
			break;
		default:
			break;
	}

	void* pBits = nullptr;
	HBITMAP hbmResult = ::CreateDIBSection( nullptr, pbmInfo, DIB_RGB_COLORS, &pBits, nullptr, 0 );
	if(nullptr == hbmResult)
	{
		delete [] pbmInfo;
		pbmInfo = nullptr;
		Log_Assert(0);
	}
	return SVDIBITMAPINFO( hbmResult, pBits, pbmInfo );

}// end SVCreateHBitmap


/**
@SVOperationName SVImageInfoToNewDIB

@SVOperationDescription This function creates a new SVDIBITMAPINFO with a BITMAPINFOHEADER.

*/
HRESULT SVIHBitmapUtilities::SVImageInfoToNewDIB( const BITMAPINFOHEADER& info, SVDIBITMAPINFO& rsvDIB )
{
	static SVColorTable<SvDef::SVImageFormatMono8> l_MonoColorTable;

	HRESULT hr = S_OK;

	Log_Assert( nullptr == rsvDIB.hbm );

	SVImageDefinitionStruct l_ImageDef;
	SVImageInfoToImageDefinitionStruct( info, l_ImageDef );
	
	SVDIBITMAPINFO l_Bitmap = SVCreateHBitmap( l_ImageDef, &(l_MonoColorTable.ColorTable[0]) );
	
	rsvDIB = l_Bitmap;

	//l_Bitmap.FreeBitmapInfo();

	return hr;

}// end SVImageInfoToNewDIB

/**
@SVOperationName SVHBitmapToDC

@SVOperationDescription This function copies a SVDIBITMAPINFO to a HDC.

*/
HRESULT SVIHBitmapUtilities::SVHBitmapToDC(const SVDIBITMAPINFO& rSrcDIB, HDC hDC)
{
	HRESULT hr = S_OK;

	SIZE sizeDest;
	::GetWindowExtEx(hDC, &sizeDest);

	HDC memDC = ::CreateCompatibleDC( hDC );
	::SelectObject(memDC, rSrcDIB.hbm );

	SIZE sizeSrc = {rSrcDIB.pBitmapInfo->bmiHeader.biWidth, rSrcDIB.pBitmapInfo->bmiHeader.biHeight };

	::StretchBlt( hDC, 0, 0, sizeDest.cx, sizeDest.cy, 
				   memDC, 0, 0, sizeSrc.cx, sizeSrc.cy, SRCCOPY );

	::DeleteDC(memDC);

	return hr;
}

