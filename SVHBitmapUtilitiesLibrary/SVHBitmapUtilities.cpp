//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : HBitmapUtilities
//* .File Name       : $Workfile:   SVHBitmapUtilities.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   12 Jun 2014 13:19:44  $
//******************************************************************************


#include "stdafx.h"
#include "SVHBitmapUtilities.h"
#include <assert.h>
#include <math.h>

/**
@SVOperationName SVGetDIBSection

@SVOperationDescription This function extracts a dib section from a HBitmap. 

*/
HRESULT SVIHBitmapUtilities::SVGetDIBSection( HBITMAP hDIB, DIBSECTION& rDibSection )
{
	HRESULT hr = S_OK;
	if(::GetObject(hDIB, sizeof(DIBSECTION), &rDibSection) == 0)
		hr = S_FALSE;
	return hr;
}

/**
@SVOperationName SVCopyDIBitsFlip

@SVOperationDescription This function copies a DIB from an HBITMAP into an BITMAPINFO. The resulting copy is flipped top to botton.

*/
HRESULT SVIHBitmapUtilities::SVCopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo)
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


/**
@SVOperationName SVCopyDIBitsFast

@SVOperationDescription This function copies a SVDIBITMAPINFO into a SVDIBITMAPINFO.  The function uses a standard memcpy if the sizes and formats match.

*/
HRESULT SVIHBitmapUtilities::SVCopyDIBitsFast(const SVDIBITMAPINFO svSrcDIB, SVDIBITMAPINFO& rsvDestDIB )
{
	HRESULT hr = S_FALSE;

	const BITMAPINFOHEADER* pSrcHeader = svSrcDIB.Header();
	BITMAPINFOHEADER* pDestHeader = rsvDestDIB.Header();
	
	assert( pSrcHeader->biSizeImage == pDestHeader->biSizeImage );
	assert( pSrcHeader->biWidth == pDestHeader->biWidth );
	assert( abs(pSrcHeader->biHeight) == abs(pDestHeader->biHeight) );
	assert( pSrcHeader->biClrUsed == pDestHeader->biClrUsed );
	assert( pSrcHeader->biHeight != 0 );

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
HRESULT SVIHBitmapUtilities::SVRasterCopyFlip(const SVDIBITMAPINFO svSrcDIB, SVDIBITMAPINFO& rsvDestDIB )
{
	HRESULT hr = S_FALSE;

	const BITMAPINFOHEADER* pSrcHeader = svSrcDIB.Header();
	BITMAPINFOHEADER* pDestHeader = rsvDestDIB.Header();
	
	assert( pSrcHeader->biSizeImage == pDestHeader->biSizeImage );
	assert( pSrcHeader->biWidth == pDestHeader->biWidth );
	assert( abs(pSrcHeader->biHeight) == abs(pDestHeader->biHeight) );
	assert( pSrcHeader->biClrUsed == pDestHeader->biClrUsed );
	assert( pSrcHeader->biHeight != 0 );

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

/**
@SVOperationName CreateBitmapInfoStruct

@SVOperationDescription This function creats a BITMAPINFO from a SVImageDefinitionStruct.

*/
BITMAPINFO* SVIHBitmapUtilities::CreateBitmapInfoStruct( SVImageDefinitionStruct& rImageDef )
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

	assert( pbmi );
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
		rImageDef.eImageFormat = SVImageFormatMono8;
	}
	else
	if( rInfo.biBitCount == 32 )
	{
		rImageDef.eImageFormat = SVImageFormatRGB8888;
	}
	else
	{
		rImageDef.eImageFormat = SVImageFormatUnknown;
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
	assert( pbmInfo );

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
	if(hbmResult == NULL)
	{
		delete [] pbmInfo;
		pbmInfo = nullptr;
		assert(0);
	}
	return SVDIBITMAPINFO( hbmResult, pBits, pbmInfo );

}// end SVCreateHBitmap


/**
@SVOperationName SVImageInfoToNewDIB

@SVOperationDescription This function creates a new SVDIBITMAPINFO with a BITMAPINFOHEADER.

*/
HRESULT SVIHBitmapUtilities::SVImageInfoToNewDIB( const BITMAPINFOHEADER& info, SVDIBITMAPINFO& rsvDIB )
{
	static SVColorTable<SVImageFormatMono8> l_MonoColorTable;

	HRESULT hr = S_OK;

	assert( rsvDIB.hbm == NULL );

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
HRESULT SVIHBitmapUtilities::SVHBitmapToDC(const SVDIBITMAPINFO svSrcDIB, HDC hDC)
{
	HRESULT hr = S_OK;

	SIZE sizeDest;
	::GetWindowExtEx(hDC, &sizeDest);

	HDC memDC = ::CreateCompatibleDC( hDC );
	::SelectObject(memDC, svSrcDIB.hbm );

	SIZE sizeSrc = { svSrcDIB.pBitmapInfo->bmiHeader.biWidth, svSrcDIB.pBitmapInfo->bmiHeader.biHeight };

	::StretchBlt( hDC, 0, 0, sizeDest.cx, sizeDest.cy, 
				   memDC, 0, 0, sizeSrc.cx, sizeSrc.cy, SRCCOPY );

	::DeleteDC(memDC);

	return hr;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.cpp_v  $
 * 
 *    Rev 1.3   12 Jun 2014 13:19:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Cleanup pointer after delete in SVCreateHBitmap.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Feb 2014 09:02:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   delete bitmapInfo if SVCreateDIBSection fails in SVCreateHBitmap.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2013 06:32:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 10:10:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   08 Nov 2010 12:51:18   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  701
 * SCR Title:  Add New Font Functionality to the Existing Command Interface
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Bitmap creatation functionality to take into consideration the negative height issue for windows.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Mar 2010 11:01:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated to remove MFC from the project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   17 Jan 2008 09:09:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed unused dead code HBitmapToBstr
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2007 15:50:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Nov 2005 15:32:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated conversion functions to handle new RGB888 format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Jul 2005 11:28:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method SVBitmapToBSTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 May 2005 14:44:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVHBitmapToDC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Mar 2005 08:33:20   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   fixed _DEBUG mode asserts
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Mar 2005 15:04:36   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved code from header file
 * added CopyDIBitsFast etc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/