//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIHBitmapUtilities
//* .File Name       : $Workfile:   SVHBitmapUtilities.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 10:10:58  $
//******************************************************************************

#ifndef SVIHBITMAPUTILITIES_H_
#define SVIHBITMAPUTILITIES_H_

#include "SVImageDefinitionStruct.h"

/**
@SVObjectName SVDIBITMAPINFO

@SVObjectOverview This structure is used to hold a BitmapInfo.

@SVObjectOperations 
*/
struct SVDIBITMAPINFO
{
	HBITMAP hbm;
	void* pBits;
	BITMAPINFO* pBitmapInfo;

	SVDIBITMAPINFO() {hbm=NULL; pBits=NULL; pBitmapInfo=NULL; pBitmapHeader=NULL;}
	SVDIBITMAPINFO(HBITMAP hbm_, void* pBits_, BITMAPINFO* pBitmapInfo_, BITMAPINFOHEADER* pBitmapHeader_=NULL) { hbm = hbm_; pBits = pBits_; pBitmapInfo = pBitmapInfo_; pBitmapHeader = pBitmapHeader_ ? pBitmapHeader_ : pBitmapInfo_ ? &(pBitmapInfo_->bmiHeader) : NULL;}
	
	BITMAPINFOHEADER* Header() const { return pBitmapHeader ? pBitmapHeader : pBitmapInfo ? &(pBitmapInfo->bmiHeader) : NULL; }

	void FreeBitmapInfo() { if ( pBitmapInfo ) { delete [] pBitmapInfo; pBitmapInfo = NULL; } }
	void Clear() { pBitmapInfo = NULL; pBits = NULL; hbm = NULL; pBitmapHeader = NULL; }

private:
	BITMAPINFOHEADER* pBitmapHeader;
};


/**
@SVObjectName SVIHBitmapUtilities

@SVObjectOverview This interface class is used to do work on functions dealing with HBITMAPs.

@SVObjectOperations This class includes the following functions: SVGetDIBSection, SVCopyDIBitsFlip, SVCopyDIBitsFast, CreateBitmapInfoStruct, SVCreateHBitmap, SVImageInfoToNewDIB, SVHBitmapToDC, SVHBitmapToBstr and SVImageInfoToImageDefinitionStruct.
The SVGetDIBSection(HBITMAP, DIBSECTION& ) function extracts a dib section from a HBitmap. The SVCopyDIBitsFlip function copies a DIB from an HBITMAP into an BITMAPINFO. The resulting copy is flipped top to botton. The SVCopyDIBitsFast copies a SVDIBITMAPINFO into a SVDIBITMAPINFO.  This function uses a standard memcpy if the sizes and formats match which results in a fast copy. The CreateBitmapInfoStruct creats a BITMAPINFO from a SVImageDefinitionStruct. The SVImageInfoToImageDefinitionStruct function fills a SVImageDefinitionStruct from a BITMAPINFOHEADER. The SVCreateHBitmap function creates a SVDIBITMAPINFO with a SVImageDefinitionStruct and a color table as inputs. The SVImageInfoToNewDIB function creates a new SVDIBITMAPINFO with a BITMAPINFOHEADER. The SVHBitmapToDC function copies a SVDIBITMAPINFO to a HDC. The SVHBitmapToBstr function copies a HBITMAP to a BSTR.

*/
class SVIHBitmapUtilities
{
public:
	static HRESULT SVGetDIBSection( HBITMAP hDIB, DIBSECTION& rDibSection );

	static HRESULT SVCopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo);

	static HRESULT SVCopyDIBitsFast(const SVDIBITMAPINFO svSrcDIB, SVDIBITMAPINFO& svDestDIB );

	static BITMAPINFO* CreateBitmapInfoStruct( SVImageDefinitionStruct& rImageDef );

	static SVDIBITMAPINFO SVCreateHBitmap( SVImageDefinitionStruct& rImageDef, RGBQUAD* pColorTable );

	static HRESULT SVImageInfoToNewDIB( const BITMAPINFOHEADER& info, SVDIBITMAPINFO& rsvDIB );

	static HRESULT SVHBitmapToDC(const SVDIBITMAPINFO svSrcDIB, HDC hDC);

	static HRESULT SVImageInfoToImageDefinitionStruct( const BITMAPINFOHEADER& rInfo, SVImageDefinitionStruct& rImageDef );

private:
	// don't call this directly; call SVCopyDIBitsFast
	static HRESULT SVRasterCopyFlip(const SVDIBITMAPINFO svSrcDIB, SVDIBITMAPINFO& rsvDestDIB );
};

template <int>
struct SVColorTable;

/**
@SVObjectName SVColorTable<SVImageFormatMono8>

@SVObjectOverview This structure is used to store a 8 bit mono color table.

@SVObjectOperations There are no operations.
*/
template <>
struct SVColorTable <SVImageFormatMono8>
{
	enum { table_size = 256};

	RGBQUAD ColorTable[table_size];

	SVColorTable()
	{
		for ( int i=0; i < table_size; i++ )
		{
			RGBQUAD rgb={(BYTE)i,(BYTE)i,(BYTE)i,0};
			ColorTable[i]=rgb;
		}
	}
	int size() { return table_size; }
};


/**
@SVObjectName SVColorTable<SVImageFormatRGB888>

@SVObjectOverview This structure is used hold a non-existent RGB888 color table.  It is non-existant because all the color information is stored with each pixel.

@SVObjectOperations There are no operations.
*/
template <>
struct SVColorTable <SVImageFormatRGB888>
{
	enum { table_size = 0};

	SVColorTable() {}
};

/**
@SVObjectName SVColorTable<SVImageFormatRGB8888>

@SVObjectOverview This structure is used hold a non-existent RGB8888 color table.  It is non-existant because all the color information is stored with each pixel.

@SVObjectOperations There are no operations.
*/
template <>
struct SVColorTable <SVImageFormatRGB8888>
{
	enum { table_size = 0};

	SVColorTable() {}
};




#endif	//#ifndef SVIHBITMAPUTILITIES_H_

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVHBitmapUtilitiesLibrary\SVHBitmapUtilities.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:10:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 May 2010 13:48:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  693
 * SCR Title:  Fix Performance Issue with Inspection Process
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include file due to changes in the data manager include path update.
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
 *    Rev 1.0   23 Jul 2007 15:50:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   08 Nov 2005 15:32:46   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  516
 * SCR Title:  Allow SVObserver to use the Intek 1394 driver for camera acquisition
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated conversion functions to handle new RGB888 format.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   15 Jul 2005 11:28:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added method SVBitmapToBSTR
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 May 2005 14:44:40   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  482
 * SCR Title:  New vector based mask
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added SVHBitmapToDC
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Mar 2005 15:15:56   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   moved code to cpp file
 * added SVCopyDIBitsFast
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Mar 2005 15:19:10   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  476
 * SCR Title:  Optimize performance for External Tool DLL HBITMAP operations
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   added ability to generate HBITMAP from a SVImageInfoClass
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   08 Jul 2004 11:54:42   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  406
 * SCR Title:  Implement External Tool
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   first iteration
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
