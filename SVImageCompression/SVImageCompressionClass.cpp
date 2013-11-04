// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageCompressionClass.cpp
// * .File Name       : $Workfile:   SVImageCompressionClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:22:28  $
// ******************************************************************************

#include "stdafx.h"

#define OEMRESOURCE
#include "winuser.h"


#define ImageCompressionMaster

#include "SVImageCompression.h"
#include "SVImageCompressionClass.h"
#include "SVCompressionDlg.h"       // SVCompressionDlg
#include "StaticImageCompressionPool.h"
#include "SVMessage/SVMessage.h"

#include "SVLibrary/SVBStr.h"       // SVBstr
#include "SVLibrary/SVBitmap.h"       // SVBstr
#include "picture.h"                   // CPicture

#include <L_OCX40.H>                // Format defines

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// The following code represents the abstraction of the compression 
// parameters.  The first index represents SVR's compression number. The 
// second index represents the compression parameters.  
//
// SVRCompression numbers 0-99 will represent lossless compressions.
// SVRCompression numbers 100-999 will represent lossy compressions.
// SVRCompression numbers 1000 and higher are for "special" compressions 
//  which may use algorithms to determine compression level vs. image 
//  quality level.
//
// Parameter 1 represents the image format.
//    An image format of -1 indicates that that index is not defined.
// Parameter 2 represents the image quality.
//
// SVRNumber   Description
//    0        RAW DIB (no lead tools)
//    1        LEAD Proprietary Compression Format. Bits per pixel: 8 for 
//              grayscale, 24 for color.
//    2        JPEG File Interchange Format with YUV 4:4:4 color space and 
//              Lossless. Bits per pixel: 8, 12, and 16 for grayscale, 24 for
//              color.
//    3        Tagged Image File with JPEG compression and YUV 4:4:4 color 
//              space and Lossless. Bits per pixel: 8, 12, and 16 for 
//              grayscale, 24 for color.
//    4        JPEG 2000 Format.  This file format contains image data and 
//              extra information about the contents and organization of the
//              file.  Bits per pixel: 8 for grayscale, 24 for color.
//    5        JPEG 2000 Format.  This file format contains only a stream of 
//              image data.  Bits per pixel: 8, 12, and 16 for grayscale, 24 
//              for color.  (12 and 16-bit grayscale support available in 
//              Medical toolkits only).  SOMETHING ABOUT A PLUG-IN; THIS ONE
//              MIGHT NOT BE AVAILABLE.
//    6        JPEG File Interchange Format with CieLAB 4:4:4 color space and
//              Lossless.
//    7 - 99   Unused.
//  100        
// 1000        LEAD Proprietary Compression Format. Size more important that 
//              quality (sharp)This option is recommended for images that 
//              have sharp edges. For example, an image that has a text 
//              overlay or an image with many straight lines. While it 
//              creates a larger file than the next option, it retains better
//              edges.
// 1001        JPEG File Interchange Format with YUV 4:4:4 color space and 
//              Lossless. Size more important that 
//              quality (sharp)This option is recommended for images that 
//              have sharp edges. For example, an image that has a text 
//              overlay or an image with many straight lines. While it 
//              creates a larger file than the next option, it retains better
//              edges.
//
//
//

long lHardICompAbstraction [1100][2] =
{
	/* 0000 Native DIB*/   0,0,
	/* 0001 */  FILE_CMP,2,  
	/* 0002 */  FILE_JFIF,2,
	/* 0003 */  FILE_JTIF,2,
	/* 0004 */  FILE_JP2,2,
	/* 0005 */  FILE_J2K,2,
	/* 0006 */  FILE_JFIF_LAB,2,
	/* 0007 - 0099 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	     
	/* 0100 - 0199 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0200 - 0299 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0300 - 0399 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0400 - 0499 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0500 - 0599 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0600 - 0699 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0700 - 0799 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0800 - 0899 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
	/* 0900 - 0999 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

	/* 1000 */  FILE_CMP, QFACTOR_SQS,
	/* 1001 */  FILE_JFIF, QFACTOR_SQS,
	/* 1002 - 1099 */ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
};

/*
This constructor initializes the attributes to empty defaults and sets the pool size to the provided alPoolSize parameter.
*/
SVImageCompressionClass::SVImageCompressionClass(long alPoolSize)
{
	svmlICompressionPoolSize = alPoolSize;
	svmpICompressionDlg = NULL;
	m_DMIndex = -1;
	m_DMTxnID = 0;
}

/*
The destructor cleans up the created pool and properly shuts down the image compression system.
*/
SVImageCompressionClass::~SVImageCompressionClass()
{
   StaticImageCompressionPool staticPool (svmlICompressionPoolSize, 
                                          &svmpICompressionDlg);

   if (svmlICompressionPoolSize != 0)
   {
	   if (staticPool.svmpICompressionPoolDataManager != 0)
	   {
		  staticPool.svmpICompressionPoolDataManager->
         ReleaseBufferIndex (staticPool.svmlICompressionPoolManagerHdl,
                             m_DMIndex, m_DMTxnID, SV_OTHER );
	   }
   }
   else
   {
      staticPool.DestroyInstance ();
   }
}

/*
This method allocated the next available resource to use for image compression.
*/
HRESULT SVImageCompressionClass::GetAvailableResource ()
{
   HRESULT  hr;
   StaticImageCompressionPool staticPool (svmlICompressionPoolSize,
                                          &svmpICompressionDlg);

   hr = 0;

   while (1)
   {
      if (svmlICompressionPoolSize == 0)
      {
         hr = 1575;
         break;
      }

      hr = staticPool.svmpICompressionPoolDataManager->
            GetNextAvailableBufferIndex (staticPool.svmlICompressionPoolManagerHdl,
                                         SV_OTHER, true, 
                                         &m_DMIndex, &m_DMTxnID);

      if (hr == SVDM_1502NO_INDEXESAVAILABLE_ERROR)
      {
         break;
      }

      break;
   }

   return hr;
}

/*
This method takes the input buffer and creates a image compression processing buffer.  If creation is successful, the image is imported into the processing buffer.
*/
HRESULT SVImageCompressionClass::ImportGlobalAllocDIBData ()
{
   long     hr;
   long     rValue;

   StaticImageCompressionPool staticPool (svmlICompressionPoolSize,
                                          &svmpICompressionDlg);

   hr = 0;

   while (1)
   {
      if (svmlICompressionPoolSize == 0)
      {
         rValue = staticPool.svmpICompressionDlg->
            svmLeadControl.SetDIB ((OLE_HANDLE) staticPool.svmghDIBInputBuffer);
      }
      else
      {
         rValue = staticPool.svmpICompressionDlgPool[m_DMIndex].
            svmLeadControl.SetDIB ((OLE_HANDLE) staticPool.
                                    svmghpDIBInputBufferPool [m_DMIndex]);
      }
     

      if (rValue != 0)
      {
         hr = -1563;
         break;
      }

      break;
   }

   return hr;
}

/*
This function does not destroy the original BSTR.
*/
HRESULT  SVImageCompressionClass::ImportBSTR (BSTR abstrInputImage)
{

   long     hr;
   long     rValue;

   SVCompressionDlg*    pDlg;

   StaticImageCompressionPool staticPool (svmlICompressionPoolSize,
                                          &svmpICompressionDlg);

   hr = 0;

   while (1)
   {
      if (svmlICompressionPoolSize == 0)
      {
         pDlg = staticPool.svmpICompressionDlg;
      }
      else
      {
         pDlg = &staticPool.svmpICompressionDlgPool [m_DMIndex];
      }


      rValue = pDlg->
         svmLeadControl.LoadBuffer ((long) abstrInputImage, 
                                    0,    // Use current image pixel 
                                          //  depth.
                                    0,    // Not multi-page.
                                    1,    // Load first page (not 
                                          //  multi-page).
                                    *(((long *) abstrInputImage) - 1) //
                                          // BSTR manipulation to get 
                                          // the BSTR buffer size.
                                    );

      if (rValue != 0)
      {
         hr = -1576;
         break;
      }

      break;
   }

   return hr;

}


/*
apbstrOutputImage gets allocated within the SVCompressionDlg::OnSaveBufferLeadctrl1 callback. This is mentioned here since it is not readily apparent.
*/
HRESULT SVImageCompressionClass::ExportBSTR (BSTR* apbstrOutputImage, 
                                             long  alColorDepth, 
                                             long  alCompression)
{
   long  hr;
   long  rValue;
   StaticImageCompressionPool staticPool (svmlICompressionPoolSize, &svmpICompressionDlg);

   SVCompressionDlg*    pDlg;

   hr = 0;

   while (1)
   {
      if (alCompression == -1)
      {
         hr = -1552;
         break;
      }

      if (alCompression > 1100)
      {
         hr = -1553;
         break;
      }

      if (alCompression == 0)
      {
//------ No compression.  This should be a pass through for the DIB.
      }

      if (svmlICompressionPoolSize == 0)
      {
         pDlg = staticPool.svmpICompressionDlg;
      }
      else
      {
         pDlg = &staticPool.svmpICompressionDlgPool [m_DMIndex];
      }


//--- The message/call back will use this address for allocating the 
//--- required BSTR.
      pDlg->svmpbstrOutputImage = apbstrOutputImage;

//--- Lead control will do a callback/event to 
//--- SVCompressionDlg::OnSaveBufferLeadctrl1(long uRequiredSize) which will 
//--- re-set the buffer size and address.
//--- We have an update!!!! Lead tools does not accept a NULL or 0 size 
//--- buffer.
      pDlg->AllocateSaveBuffer (100);

      pDlg->
         svmLeadControl.SetEnableSaveBufferEvent(TRUE); 

      rValue = pDlg->
         svmLeadControl.SaveBuffer ((short)( lHardICompAbstraction [alCompression][0] ), 
                                    (short)( alColorDepth ),         // May need to be dynamic.
                                    (short)( lHardICompAbstraction [alCompression][1] ), 
                                    SAVE_OVERWRITE);
      if (rValue != 0)
      {
         hr = -1554;
         break;
      }


      break;
   }

   return hr;
}

/*
ahpBitmap needs to be the address of a handle. The returned DIB is actually allocated using GlobalAlloc, and therefore must follow the associated access rules. This memory must be freed with GlobalFree (). After calling GlobalLock () the resulting pointer should be able to be directly type cast into an HBITMAP.
*/
HRESULT  SVImageCompressionClass::ExportGlobalAllocDIBData (HGLOBAL* ahpBitmap)
{
   StaticImageCompressionPool staticPool (svmlICompressionPoolSize, 
                                          &svmpICompressionDlg);

   SVCompressionDlg*    pDlg;

   long hr;

   hr = 0;

   while (1)
   {
      if (ahpBitmap == NULL)
      {
         hr = -1586;
         break;
      }

      *ahpBitmap = 0;

      if (svmlICompressionPoolSize == 0)
      {
         pDlg = staticPool.svmpICompressionDlg;
      }
      else
      {
         pDlg = &staticPool.svmpICompressionDlgPool [m_DMIndex];
      }

       pDlg->svmLeadControl.SetEnableMethodErrors(FALSE);

      try 
      {
         *ahpBitmap = (HGLOBAL) pDlg->svmLeadControl.GetDIB ();
      }


      catch (COleException coleException)
      {
         hr = -1593;
         break;
      }

      catch (COleDispatchException coleDException)
      {
         hr = -1594;
         break;
      }

      if (*ahpBitmap == 0)
      {
         hr = -1577;
         break;
      }

      break;
   }


   return hr;
}

/*
This method exports an image into an IPicture object.
*/
HRESULT SVImageCompressionClass::ExportIPicture (LPPICTUREDISP* appIPictureDispatch)
{
   long  hr;

	CPictureHolder    oPictureMaker;
   HGLOBAL           hGlobalBitmap;
   HBITMAP           hSourceBitmap;
   HBITMAP           l_hWindowsBitmap;
	SVBitmap				l_oBitmap;
   SVCompressionDlg* l_pDlg;

   StaticImageCompressionPool staticPool (svmlICompressionPoolSize, 
                                          &svmpICompressionDlg);
   hr = 0;
   hGlobalBitmap = NULL;
   hSourceBitmap = NULL;
   l_hWindowsBitmap = NULL;

   while (1)
   {
      if (appIPictureDispatch == NULL)
      {
         hr = -1589;
         break;
      }

      *appIPictureDispatch = NULL;

      if (svmlICompressionPoolSize == 0)
      {
         l_pDlg = staticPool.svmpICompressionDlg;
      }
      else
      {
         l_pDlg = &staticPool.svmpICompressionDlgPool [m_DMIndex];
      }

      hr = ExportGlobalAllocDIBData (&hGlobalBitmap);
      if (hr < 0)
      {
         break;
      }

      if (hGlobalBitmap == NULL)
      {
         hr = -1587;
         break;
      }

      hSourceBitmap = (HBITMAP) GlobalLock (hGlobalBitmap);
      if (hSourceBitmap == NULL)
      {
         hr = -1588;
         break;
      }

		hr = l_oBitmap.LoadDIBitmap (hSourceBitmap);

      if (hr < 0)
      {
         break;
      }

		l_hWindowsBitmap = (HBITMAP) l_oBitmap.Detach ();
		if (l_hWindowsBitmap == NULL)
		{
			hr = -1604;
			break;
		}

		if (!oPictureMaker.CreateFromBitmap (l_hWindowsBitmap, NULL, TRUE ))  // let the IPictureDisp take ownership of the HBITMAP
      {
         hr = -1592;
         break;
      }

      if (GlobalUnlock (hGlobalBitmap) != 0)
      {
         hr = -1591;
         break;
      }

      hSourceBitmap = NULL;

	   // Add this bitmap to the appropriate ImageObject of the ImageObjectList
	   *appIPictureDispatch = oPictureMaker.GetPictureDispatch();
 
      if (*appIPictureDispatch == NULL)
      {
         hr = -1590;
         break;
      }

      break;
   }

   if (hSourceBitmap)
   {
      GlobalUnlock (hGlobalBitmap);
      hSourceBitmap = NULL;
   }

   if (hGlobalBitmap)
   {
      GlobalFree (hGlobalBitmap);
      hGlobalBitmap = NULL;
   }

   return hr;
}

/*
GetAvailableResource must be called prior to calling this function.
*/
HRESULT SVImageCompressionClass::GetAndLockInputDIBBuffer (char** appInputDIBBuffer)
{
   HRESULT  hrResult;
   StaticImageCompressionPool staticPool (svmlICompressionPoolSize, 
                                          &svmpICompressionDlg);

   hrResult = 0;
   while (1)
   {
      if (svmlICompressionPoolSize == 0)
      {
         *appInputDIBBuffer = (char *) GlobalLock (staticPool.svmghDIBInputBuffer);

      }
      else
      {
         *appInputDIBBuffer = (char *) GlobalLock (staticPool.svmghpDIBInputBufferPool [m_DMIndex]);
      }


      break;
   }

   return hrResult;
}

/*
The apInputDIBBuffer value returned from GetAndLockInputDIBBuffer () should be set to NULL after calling this function.  GetAndLockInputDIBBuffer () must be called prior to calling this function.
*/
HRESULT SVImageCompressionClass::UnLockInputDIBBuffer ()
{
   HRESULT  hrResult;
   StaticImageCompressionPool staticPool (svmlICompressionPoolSize, 
                                          &svmpICompressionDlg);

   hrResult = 0;

   while (1)
   {
      if (svmlICompressionPoolSize == 0)
      {
         GlobalUnlock (staticPool.svmghDIBInputBuffer);
      }
      else
      {
         GlobalUnlock (staticPool.svmghpDIBInputBufferPool [m_DMIndex]);
      }


      break;
   }

   return hrResult;

}

/*
This method created the resource pool.
*/
HRESULT SVImageCompressionClass::CreateResourcePool ()
{
	HRESULT hr;
	StaticImageCompressionPool staticPool (svmlICompressionPoolSize, 
                                           &svmpICompressionDlg);

	hr = staticPool.Create ();

	return hr;

}

/*
This method destroys the resource pool.  Compression pool size is automatically set to zero by the constructor.  If pool size is greater than 0, DestroyResourcePool () will attempt to re-create.
*/
HRESULT SVImageCompressionClass::DestroyResourcePool ()
{
	HRESULT	hr;

	StaticImageCompressionPool staticPool (0, 
                                           &svmpICompressionDlg);

	hr = staticPool.Destroy ();
	return hr;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageCompression\SVImageCompressionClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 10:22:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   22 Oct 2010 08:05:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  704
 * SCR Title:  Upgrade SVObserver Version for 5.33 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix include problems and compilier efficiency.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   15 Feb 2010 08:19:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Dec 2009 10:26:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   13 Jul 2009 12:17:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new transaction id variable used to track the data manager index.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   30 Oct 2007 12:38:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initialized resource handle in constructor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   19 Jun 2007 13:06:22   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * This project includes a change to fix a problem with the attachment of the Lead Tools ActiveX to an SVObserver Dialog.  All Lead Tools includes have been moved from the header files to the cpp files and the appropriate externs were generated to get access to thedesired data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   22 Aug 2005 09:45:00   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  492
 * SCR Title:  Upgrade SVObserver to version 4.60
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed compiler warnings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   09 Apr 2003 15:47:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  344
 * SCR Title:  Fix Bug in SVImageCompressionClass so shutdown is clean
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added CreateResourcePool and DestroyResourcePool.  These functions are used to control when resources are created and destroyed.  This is because when they were left to automatically destroy, they crashed.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   26 Mar 2003 15:38:42   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   Fixed bug in ExportBSTR ().
 * 
 * This bug should only show itself in client side code, not in the server side code (which should be working from resource pool).
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   28 Feb 2003 08:46:28   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated include information.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   26 Feb 2003 17:37:02   jBrown
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   The following functions were modified to implement Image Compression:
 * 
 * SVImageCompressionClass() - constructor
 * ~SVImageCompressionClass() - destructor
 * GetAvailableResource ()
 * ImportGlobalAllocDIBData ()
 * ImportBSTR ()
 * ExportBSTR ()
 * ExportGlobalAllocDIBData ()
 * ExportIPicture ()
 * GetAndLockInputDIBBuffer ()
 * UnLockInputDIBBuffer ()
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   28 Jan 2003 15:19:26   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include definition to remove direct pathing information and added it to the project settings.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Jan 2003 12:55:30   cschmittinger
 * Project:  SVObserver
 * Change Request (SCR) nbr:  318
 * SCR Title:  Image Transfer Compression
 * Checked in by:  cSchmittinger;  Carl Schmittinger
 * Change Description:  
 *   Modified due to changes in SVLibrary and SVOLibrary
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
