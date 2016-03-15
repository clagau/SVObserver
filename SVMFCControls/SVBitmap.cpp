// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVBitmap
// * .File Name       : $Workfile:   SVBitmap.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 13:30:48  $
// ******************************************************************************

#include "stdafx.h"
#include "SVBitmap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SVBitmap::SVBitmap(): 
   CBitmap ()
{

}

SVBitmap::~SVBitmap()
{

}
   

HRESULT SVBitmap::LoadBitmap (unsigned int p_nIDResource)
{

   HRESULT  hr;

   hr = 0;

   if (!CBitmap::LoadBitmap (p_nIDResource))
   {
      hr = -1595;

   }

   return hr;
}


HRESULT SVBitmap::LoadBitmap (TCHAR* p_pszResourceName)
{

   HRESULT  hr;

   hr = 0;

   if (!CBitmap::LoadBitmap (p_pszResourceName))
   {
      hr = -1596;

   }

   return hr;
}


HRESULT SVBitmap::LoadOEMBitmap (unsigned int p_nIDResource)
{

   HRESULT  hr;

   hr = 0;

   if (!CBitmap::LoadOEMBitmap (p_nIDResource))
   {
      hr = -1597;

   }

   return hr;
}


HRESULT SVBitmap::LoadDIBitmap (void* p_pDIB)
{

   HRESULT           l_hr;
	BITMAPINFOHEADER* l_pbmhInfo;
   HBITMAP           l_bmhCreateDIBSectionBitmapHandle;
   HBITMAP           l_bmhTestExisting;

   long              l_lColorTableSize;
   long              l_lNumberOfColors;

   char*             l_pDIBSectionImage;
   char*             l_pImage;

   l_hr = 0;
   l_pbmhInfo = NULL;
   l_pDIBSectionImage = NULL;
   l_pImage = NULL;
   l_bmhCreateDIBSectionBitmapHandle = NULL;
   l_bmhTestExisting = NULL;

   while (1)
   {
      l_bmhTestExisting = (HBITMAP) m_hObject; // Typecast from void* to
															  //  HBITMAP.

      if (l_bmhTestExisting != NULL)			  // If there is already a 
      {													  //  Windows GDI object 
         l_hr = -1599;								  //  attached to this instance
         break;										  //  then an error code will be
      }													  //  returned.

      l_pbmhInfo = (BITMAPINFOHEADER*) p_pDIB; // Typecast from void* to
															  //  BITMAPINFOHEADER*.

      if (l_pbmhInfo->biSize == sizeof (BITMAPINFOHEADER))
      {
//------ Version 3 header


      }
      else
      if (l_pbmhInfo->biSize == sizeof (BITMAPV4HEADER))
      {
//------ Version 4 header


      }
      else
      if (l_pbmhInfo->biSize == sizeof (BITMAPV5HEADER))
      {
//------ Version 5 header


      }
      else
      {
         l_hr = -1598;
         break;
      }


      // Make sure color table size is calculated
		l_lNumberOfColors = l_pbmhInfo->biClrUsed;
		if(l_lNumberOfColors == 0)
		{
		   if (l_pbmhInfo->biBitCount != 24)
			  l_lNumberOfColors = 1 << l_pbmhInfo->biBitCount;
		}

      l_lColorTableSize = l_pbmhInfo->biClrUsed * sizeof( RGBQUAD );

      l_pImage = ((char *) p_pDIB) + l_lColorTableSize + l_pbmhInfo->biSize;

      l_bmhCreateDIBSectionBitmapHandle = 
		          ::CreateDIBSection (NULL,    // [in] Device Context; not
                                             //  required.
                                    (BITMAPINFO*)l_pbmhInfo, 
                                             // [in] BITMAPINFOHEADER is the 
                                             //  first element of BITMAPINFO.
                                    DIB_RGB_COLORS, 
                                    (void**) &l_pDIBSectionImage, 
                                             // [out] System allocated 
                                             //  location for image bits.
															// Typecast from char** to 
															//  void**.
                                    NULL,    // [in] File map handle; not
                                             //  required.
                                    NULL );  // [in] Used for filemapping;
                                             //  not required.

      ::GdiFlush (); // The GdiFlush function flushes the calling thread's 
                     //  current batch. Probably not needed but MSDN 
                     //  recommends.

      MemCopy (l_pImage, l_pDIBSectionImage, l_pbmhInfo->biSizeImage);

      Attach (l_bmhCreateDIBSectionBitmapHandle);
      break;
   }

   return l_hr;
}

HRESULT SVBitmap::LoadDIBitmap ( BITMAPINFO* pDIBInfo, void* pDIBData )
{
	HRESULT           l_hr;
	BITMAPINFOHEADER* l_pbmhInfo;
	HBITMAP           l_bmhCreateDIBSectionBitmapHandle;
	HBITMAP           l_bmhTestExisting;
	
	long              l_lColorTableSize;
	long              l_lNumberOfColors;
	
	char*             l_pDIBSectionImage;
	char*             l_pImage;
	
	l_hr = 0;
	l_pbmhInfo = NULL;
	l_pDIBSectionImage = NULL;
	l_pImage = NULL;
	l_bmhCreateDIBSectionBitmapHandle = NULL;
	l_bmhTestExisting = NULL;
	
	do
	{
		l_bmhTestExisting = (HBITMAP) m_hObject; // Typecast from void* to HBITMAP.
		
		if (l_bmhTestExisting != NULL)   // If there is already a 
		{                                //  Windows GDI object 
			l_hr = -12336;               //  attached to this instance
			break;                       //  then an error code will be
		}                                //  returned.
		
		l_pbmhInfo = &(pDIBInfo->bmiHeader);
		
#ifdef _DEBUG
		int nSizeOfBitmapInfoHeader = sizeof (BITMAPINFOHEADER);
		int nSizeOfBitmapV4Header = sizeof (BITMAPV4HEADER);
		int nSizeOfBitmapV5Header = sizeof (BITMAPV5HEADER);
#endif
		if (l_pbmhInfo->biSize == sizeof (BITMAPINFOHEADER))
		{
			//------ Version 3 header
			
			
		}
		else if (l_pbmhInfo->biSize == sizeof (BITMAPV4HEADER))
		{
			//------ Version 4 header
			
			
		}
		else if (l_pbmhInfo->biSize == sizeof (BITMAPV5HEADER))
		{
			//------ Version 5 header
			
			
		}
		else
		{
			l_hr = -12337;
			break;
		}
				
				
		// Make sure color table size is calculated
		l_lNumberOfColors = l_pbmhInfo->biClrUsed;
		if(l_lNumberOfColors == 0)
		{
			if (l_pbmhInfo->biBitCount != 24)
				l_lNumberOfColors = 1 << l_pbmhInfo->biBitCount;
		}
		
		l_lColorTableSize = l_pbmhInfo->biClrUsed * sizeof( RGBQUAD );
		
		l_pImage = (char*) pDIBData;
		
		l_bmhCreateDIBSectionBitmapHandle = 
				::CreateDIBSection ( NULL,                          // [in] Device Context; not required.
						             pDIBInfo,                      // [in]
						             DIB_RGB_COLORS, 
						             (void**) &l_pDIBSectionImage,  // [out] System allocated  
						                                            //  location for image bits.
						                                            // Typecast from char** to 
						                                            //  void**.
						             NULL,                          // [in] File map handle; not
						                                            //  required.
						             NULL );                        // [in] Used for filemapping;
						                                            //  not required.
		
		::GdiFlush (); // The GdiFlush function flushes the calling thread's 
				       //  current batch. Probably not needed but MSDN 
				       //  recommends.
		
		MemCopy (l_pImage, l_pDIBSectionImage, l_pbmhInfo->biSizeImage);
		
		Attach (l_bmhCreateDIBSectionBitmapHandle);
		break;

	} while ( false );// end do
	
	return l_hr;
}

HRESULT SVBitmap::CreateBitmap (long            p_lWidth, 
                                long            p_lHeight, 
                                unsigned long   p_lPlanes, 
                                unsigned long   p_lBitCount, 
                                const void*     p_pBits)

{
	HRESULT	hr;

	hr = 0;

   if (!CBitmap::CreateBitmap (p_lWidth,
                               p_lHeight,
										 p_lPlanes,
										 p_lBitCount,
										 p_pBits))
   {
      hr = -1600;

   }

   return hr;
}


HRESULT SVBitmap::CreateBitmapIndirect (LPBITMAP p_pBitmap)
{

	HRESULT	hr;

	hr = 0;

   if (!CBitmap::CreateBitmapIndirect (p_pBitmap))
   {
      hr = -1601;
   }

   return hr;
}


HRESULT SVBitmap::CreateCompatibleBitmap (CDC* p_pDC, 
														long p_lWidth, 
														long p_lHeight)
{

	HRESULT	hr;

	hr = 0;

   if (!CBitmap::CreateCompatibleBitmap (p_pDC, 
													  p_lWidth, 
													  p_lHeight))
   {
      hr = -1602;
   }

   return hr;
}


HRESULT SVBitmap::CreateDiscardableBitmap (CDC* p_pDC, 
														 long p_lWidth, 
														 long p_lHeight)
{

	HRESULT	hr;

	hr = 0;

   if (!CBitmap::CreateDiscardableBitmap (p_pDC, 
													  p_lWidth, 
													  p_lHeight))
   {
      hr = -1603;
   }

   return hr;
}


HRESULT SVBitmap::MemCopy (void* p_pSource, 
									void* p_pDestination, 
									long p_lNbrOfBytes)
{
	HRESULT	l_hr;

	l_hr = 0;

	memcpy (p_pDestination, p_pSource, p_lNbrOfBytes);

	return l_hr;
}

