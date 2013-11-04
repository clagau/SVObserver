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

#ifndef SVIMAGECONVERTORGDI_H
#define SVIMAGECONVERTORGDI_H

#include <comdef.h>
#include "SVUtilityLibrary/SVUtilityGlobals.h"

class SVImageConvertorGDI
{
private:
	// Non Constructable
	SVImageConvertorGDI();
	~SVImageConvertorGDI();

public:
	static HRESULT GetBitmapFromIPictureDisp(IPictureDisp* pPictureDisp, HBITMAP& rBitmap);
	static HRESULT GetIPictureDispFromBitmap(HBITMAP Bitmap, IPictureDisp** ppPictureDisp);

	static HRESULT BitmapToBStr( HBITMAP p_Bitmap, BSTR& p_rImage );
	static HRESULT BStrToBitmap( BSTR p_Image, HBITMAP& p_rBitmap );

	static HRESULT BitmapToByteVector( HBITMAP p_Bitmap, SVByteVector& p_rImage );
	static HRESULT ByteVectorToBitmap( const SVByteVector& p_Image, HBITMAP& p_rBitmap );

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

#endif

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

