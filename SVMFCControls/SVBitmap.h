// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBitmap
// * .File Name       : $Workfile:   SVBitmap.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:30:56  $
// ******************************************************************************

#pragma once

// Bitmap documentation.
// In the end the SVBitmap class is created and used (by me) to deal with
// windows images, DIBs, and conversions between the two.  
//
// Random thoughts.....
// An HBITMAP refers to a "Windows graphics device interface (GDI) bitmap".
//  At a little deeper down, the  "Windows graphics device interface (GDI)
//  bitmap" is a DIB.
// A "Windows graphics device interface (GDI) bitmap" can be created by 
//  CreateBitmap (). The CreateBitmap () function creates a "windows graphics
//  object" and allocates an associated DIB. There is no way (that I know of)
//  for an externally allocated DIB to be assocuated with the "Windows 
//  graphics device interface (GDI) bitmap". 
// DeleteObject (HBITMAP) then frees the memory and resources allocated by 
//  CreateBitmap ();
// CBitmap encapsultes the "Windows graphics device interface (GDI) bitmap"
//  and many of the image API functions that would also be associated with
//  windows images.  

class SVBitmap : public CBitmap  
{
public:
// Base class functions
// Attach () - This function will attach a Windows GDI object (in our case 
//  this should be an HBITMAP returned by (resources allocated by) a Windows
//  API function (as in CreateDIBSection ()).


// Constructor - Although this instantiates SVBitMap (and therefore CBitmap),
//  it does not allocate the "Windows graphics device interface (GDI) bitmap"
//  or DIB data yet.  
// One of the following initialization functions must be called prior to 
//  using this class.
//    LoadBitmap (),
//    LoadOEMBitmap (),
//    LoadDIBBitmap (),
//    CreateBitmap (),
//    CreateBitmapIndirect (), 
//    CreateCompatibleBitmap (),
//    CreateDiscardableBitmap ()

	SVBitmap();

   virtual ~SVBitmap();

// Initialization routines --------------------------------------------------
// Added functionallity.

// LoadDIBitmap () - Unlike the other initialization routines, 
//	LoadDIBitmap () does not simply call the base class function. This 
//	function loads a DIB memory block (DIB header, color table, image bits)
// into our SVBitmap (which wraps a Windows GDI object) object.
	HRESULT	LoadDIBitmap (void* p_pDIB);

	HRESULT LoadDIBitmap ( BITMAPINFO* pDIBInfo, void* pDIBData );

// Duplicates current CBitmap functionallity.

	HRESULT	LoadBitmap (unsigned int p_nIDResource);

	HRESULT	LoadBitmap (TCHAR* p_pszResourceName);

	HRESULT	LoadOEMBitmap (unsigned int p_nIDResource);

	HRESULT	CreateBitmap (long            p_lWidth, 
                          long            p_lHeight, 
                          unsigned long   p_lPlanes, 
                          unsigned long   p_lBitcount, 
                          const void*     p_pBits);

	HRESULT	CreateBitmapIndirect (LPBITMAP p_pBitmap);

	HRESULT	CreateCompatibleBitmap (CDC* p_pDC, long p_lWidth, long p_lHeight);
	HRESULT	CreateDiscardableBitmap (CDC* p_pDC, long p_lWidth, long p_lHeight);

   static HRESULT CopyDIB (void* apSourceDIB, void* apDestinationDIB);
   static HRESULT MemCopy (void* p_pSource, void* p_pDestination, long p_lNbrOfBytes);
};

