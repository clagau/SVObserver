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

#pragma once

#pragma region Includes
#include "SVImageDefinitionStruct.h"
#pragma endregion Includes

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

	SVDIBITMAPINFO() {hbm=nullptr; pBits=nullptr; pBitmapInfo=nullptr; pBitmapHeader=nullptr;}
	SVDIBITMAPINFO(HBITMAP hbm_, void* pBits_, BITMAPINFO* pBitmapInfo_, BITMAPINFOHEADER* pBitmapHeader_=nullptr) 
	{ 
		hbm = hbm_; 
		pBits = pBits_; 
		pBitmapInfo = pBitmapInfo_;
		pBitmapHeader = pBitmapHeader_ ? pBitmapHeader_ : pBitmapInfo_ ? &(pBitmapInfo_->bmiHeader) : nullptr;
	}
	
	BITMAPINFOHEADER* Header() const { return pBitmapHeader ? pBitmapHeader : pBitmapInfo ? &(pBitmapInfo->bmiHeader) : nullptr; }

	void FreeBitmapInfo() { if ( pBitmapInfo ) { delete [] pBitmapInfo; pBitmapInfo = nullptr; } }
	void Clear() { pBitmapInfo = nullptr; pBits = nullptr; hbm = nullptr; pBitmapHeader = nullptr; }

private:
	BITMAPINFOHEADER* pBitmapHeader;
};


/**
@SVObjectName SVIHBitmapUtilities

@SVObjectOverview This interface class is used to do work on functions dealing with HBITMAPs.

@SVObjectOperations This class includes the following functions: 
 SVCopyDIBitsFlip, 
  CreateBitmapInfoStruct, 
 SVCreateHBitmap, 
 SVImageInfoToNewDIB, 
 SVHBitmapToDC, 
  SVImageInfoToImageDefinitionStruct.
The SVCopyDIBitsFlip function copies a DIB from an HBITMAP into an BITMAPINFO. The resulting copy is flipped top to botton. 
The CreateBitmapInfoStruct creats a BITMAPINFO from a SVImageDefinitionStruct. 
The SVImageInfoToImageDefinitionStruct function fills a SVImageDefinitionStruct from a BITMAPINFOHEADER. 
The SVCreateHBitmap function creates a SVDIBITMAPINFO with a SVImageDefinitionStruct and a color table as inputs. 
The SVImageInfoToNewDIB function creates a new SVDIBITMAPINFO with a BITMAPINFOHEADER. 
The SVHBitmapToDC function copies a SVDIBITMAPINFO to a HDC. 
*/
class SVIHBitmapUtilities
{
public:
	static HRESULT SVCopyDIBitsFlip(HDC hDC, HBITMAP hDIB, void* pSrcBits, BITMAPINFO* pbmInfo);

	static BITMAPINFO* CreateBitmapInfoStruct( SVImageDefinitionStruct& rImageDef );

	static SVDIBITMAPINFO SVCreateHBitmap( SVImageDefinitionStruct& rImageDef, RGBQUAD* pColorTable );

	static HRESULT SVImageInfoToNewDIB( const BITMAPINFOHEADER& info, SVDIBITMAPINFO& rsvDIB );

	static HRESULT SVHBitmapToDC(const SVDIBITMAPINFO svSrcDIB, HDC hDC);

	static HRESULT SVImageInfoToImageDefinitionStruct( const BITMAPINFOHEADER& rInfo, SVImageDefinitionStruct& rImageDef );
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


