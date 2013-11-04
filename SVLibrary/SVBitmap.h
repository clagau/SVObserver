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

#if !defined(AFX_SVBITMAP_H__05A4A540_F213_4EBA_AD69_9EAEE3193BC6__INCLUDED_)
#define AFX_SVBITMAP_H__05A4A540_F213_4EBA_AD69_9EAEE3193BC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


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

	HRESULT	CreateCompatibleBitmap (CDC* p_pDC, 
												long p_lWidth, 
												long p_lHeight);

	HRESULT	CreateDiscardableBitmap (CDC* p_pDC, 
												 long p_lWidth, 
												 long p_lHeight);

// End of initialization routines -------------------------------------------


   static HRESULT CopyDIB (void* apSourceDIB, void* apDestinationDIB);
   static HRESULT MemCopy (void* p_pSource, void* p_pDestination, long p_lNbrOfBytes);




};

#endif // !defined(AFX_SVBITMAP_H__05A4A540_F213_4EBA_AD69_9EAEE3193BC6__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVLibrary\SVBitmap.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 13:30:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Mar 2003 08:13:26   ebeyeler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  328
 * SCR Title:  Upgrade SVObserver to MIL 7.1
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Added CreateDIBitmap overload to allow for specifying BITMAPINFO and data bits
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   26 Feb 2003 17:06:22   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   For the purpose of converting various formats of images into something that Windows can use, SVBitmap was created.  SVBitmap is derived from CBitmap and provides a fairly simple method of creating a windows graphic object from the bitmaps we manipulate.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/