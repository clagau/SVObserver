// ******************************************************************************
// * COPYRIGHT (c) 2003 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : StaticImageCompressionPool.cpp
// * .File Name       : $Workfile:   StaticImageCompressionPool.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 10:19:40  $
// ******************************************************************************

#include "stdafx.h"
//Moved to precompiled header: #include <memory>
#include "SVImageCompression.h"
#include "StaticImageCompressionPool.h"
#include "SVLibrary/SVBStr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CWnd g_DesktopWindow;

#define ICOMPRESSIONPOOLMASTER

SVDataManager*       StaticImageCompressionPool::svmpICompressionPoolDataManager = NULL;
SVCompressionDlg*    StaticImageCompressionPool::svmpICompressionDlgPool = NULL;
long                 StaticImageCompressionPool::svmlICompressionPoolManagerHdl;
long                 StaticImageCompressionPool::svmlIsInitialized = FALSE;
long                 StaticImageCompressionPool::svmlICompressionPoolSize = 0;
CMutex               StaticImageCompressionPool::svmIsInitializedLock;
HGLOBAL*             StaticImageCompressionPool::svmghpDIBInputBufferPool = NULL;

//This is the LEAD Tools licence string variable.  It is used to validate the licence information for LEAD Tools to operate correctly.
static const WCHAR BASED_CODE _szLicString[] = L"LEADTOOLS OCX Copyright (c) 1991-2001 LEAD Technologies, Inc.";

/*
This object maintains a parameterized constructor for allocating the appropriate static resources is the resources are unavailable.  This object maintains the Create, Destroy, and Destroy Instance operators for handling start-up and shutdown of the resource pool.
*/
StaticImageCompressionPool
   ::StaticImageCompressionPool(long               alPoolSize,
                                SVCompressionDlg** appParentsDlg,
                                HRESULT*           ahrpResult)
{
   CRect    rcClient(0,0,1,1);

   long     lErr;

   lErr = 0;
   svmpICompressionDlg = NULL;
   svmghDIBInputBuffer = NULL;

   while (1)
   {
      if (alPoolSize == 0)
      {
         if (appParentsDlg == NULL)
         {
            lErr = -1592;
            break;
         }

//------ If the resource pool is not being used, then it is expected that 
//------ the resource life expectancy will be that of the parent
//------ (SVImageCompressionClass). If the parents dialog value is null, 
//------ then a new one must be allocated. If the parents dialog value is not
//------ null, then use it.
         if (*appParentsDlg == NULL)
         {
            svmpICompressionDlg = new SVCompressionDlg;
            if (svmpICompressionDlg == NULL)
            {
               lErr = -1569;
               break;
            }

            svmpICompressionDlg->Create (IDD_SVCOMPRESSION_DLG);
            if (svmpICompressionDlg-> m_hWnd == NULL)
            {
               lErr = -1572;
               break;
            }

            svmpICompressionDlg-> svmLeadControl.SetEnableMethodErrors (FALSE);
            svmpICompressionDlg-> svmLeadControl.SetScaleMode (3); // Use Pixels

            *appParentsDlg = svmpICompressionDlg;
         }
         else
         {
            svmpICompressionDlg = *appParentsDlg;
         }

         svmghDIBInputBuffer = GlobalAlloc (GPTR, 4000000);
         if (svmghDIBInputBuffer == NULL)
         {
            lErr = -1574;
            break;
         }
      }
      else
      {
         if (svmlICompressionPoolSize == 0)
         {
            svmlICompressionPoolSize = alPoolSize;
         }
         else
         if (svmlICompressionPoolSize != alPoolSize)
         {
            lErr = 1571;
         }

//------ We are allocating pool resources.
	        lErr = Create ();
      }

      break;
   }

   if (lErr == -1564)
   {
      delete [] svmpICompressionDlgPool;
      svmpICompressionDlgPool = NULL;
      svmlIsInitialized = FALSE;
   }

   if (ahrpResult != NULL)
   {
      *ahrpResult = lErr;
   }
}


/*
The Create operator is used to start-up and initializes the static image compression pool if it is not already created.
*/
HRESULT StaticImageCompressionPool::Create ()
{
	HRESULT hr;
    long    lIndex;
    SVBStr  bstrManagedIndexArrayName;
    SVBStr  bstrLicense;



	hr = 0;


	while (1)
	{
		svmIsInitializedLock.Lock ();

		if (svmlIsInitialized == FALSE)
		{
			svmlIsInitialized = TRUE;
			svmIsInitializedLock.Unlock ();


			// Used to hold a cwnd that is attached to the desktop.
			if( g_DesktopWindow.GetSafeHwnd() == NULL )
			{
				g_DesktopWindow.Attach( ::GetDesktopWindow() );
			}

			svmpICompressionPoolDataManager = new SVDataManager;

			svmghpDIBInputBufferPool = new HGLOBAL [svmlICompressionPoolSize];

			for (lIndex = 0; lIndex < svmlICompressionPoolSize; lIndex++)
			{
				//------------ Initialize the GlobalAlloc buffers to NULL.
				svmghpDIBInputBufferPool [lIndex] = NULL;
			}

			bstrManagedIndexArrayName = "ImageCompressionPool";
			bstrLicense = _szLicString;
			svmpICompressionPoolDataManager->
				CreateManagedIndexArray (bstrManagedIndexArrayName, 
				svmlICompressionPoolSize,
				&svmlICompressionPoolManagerHdl);

			svmpICompressionDlgPool = new SVCompressionDlg [svmlICompressionPoolSize];

			if (svmpICompressionDlgPool == NULL)
			{
				hr = -1570;
				break;
			}


			for (lIndex = 0; lIndex < svmlICompressionPoolSize; lIndex++)
			{
				svmpICompressionDlgPool [lIndex].Create( IDD_SVCOMPRESSION_DLG, &g_DesktopWindow );

				if (svmpICompressionDlgPool [lIndex].m_hWnd == NULL)
				{
					//--------------- The ExitInstance () function of SVImageCompression instantiates 
					//--------------- StaticImageCompressionPool for the purpose of calling the 
					//--------------- Destroy () function.  When doing the SVObserver server 
					//--------------- registration, InitInstance () and ExitInstance () are called 
					//--------------- without actually executing the rest of SVObserver.  In this case
					//--------------- the framework does not exist to instantiate windows.
					if (lIndex == 0)
					{
						hr = -1564;
					}
					else
					{
						hr = -1565;
					}
					break;
				}


				svmpICompressionDlgPool [lIndex].svmLeadControl.SetEnableMethodErrors (FALSE);
				svmpICompressionDlgPool [lIndex].svmLeadControl.SetScaleMode (3); // Use Pixels

				svmghpDIBInputBufferPool [lIndex] = GlobalAlloc (GPTR, 4000000);

			}
		} // if (svmlIsInitialized == FALSE)
		else
		{
			svmIsInitializedLock.Unlock ();
		}
		break;
	} // while (1)

	return hr;
}

/*
This method will clean up the local resources to this object upon destruction.
*/
StaticImageCompressionPool::~StaticImageCompressionPool()
{
   if (svmghDIBInputBuffer != NULL)
   {
      GlobalFree (svmghDIBInputBuffer);
      svmghDIBInputBuffer = NULL;
   }
}


/*
The Destroy operator is used to shutdown and destroys the static image compression pool if it was created.
*/
HRESULT StaticImageCompressionPool::Destroy ()
{
   HRESULT  hr;
   long     lIndex;

   hr = 0;

   while (1)
   {
      svmIsInitializedLock.Lock ();
//--- This is to destroy the instance window if a pool was not used.
      if (svmpICompressionDlg != NULL)
      {
         svmpICompressionDlg->DestroyWindow ();
      }

      delete svmpICompressionDlg;
      svmpICompressionDlg = NULL;

//--- If the array of dialogs was not allocated, do not attempt to destroy
//--- them. 
      if (svmpICompressionDlgPool != NULL)
      {
         for (lIndex = 0; lIndex < svmlICompressionPoolSize; lIndex++)
         {
            svmpICompressionDlgPool [lIndex].DestroyWindow ();
         }

         delete [] svmpICompressionDlgPool;
         svmpICompressionDlgPool = NULL;
      }


      for (lIndex = 0; lIndex < svmlICompressionPoolSize; lIndex++)
      {
         svmghpDIBInputBufferPool [lIndex] = GlobalFree (svmghpDIBInputBufferPool [lIndex]);

         if (svmghpDIBInputBufferPool [lIndex] != NULL)
         {
            hr = -1562;
         }
      }

      delete svmghpDIBInputBufferPool;
      svmghpDIBInputBufferPool = NULL;

	  if(svmpICompressionPoolDataManager != NULL)
	  {
		hr = svmpICompressionPoolDataManager->DestroyIndexArray (svmlICompressionPoolManagerHdl);
	    delete svmpICompressionPoolDataManager;
	    svmpICompressionPoolDataManager = NULL;
	  }

		if( g_DesktopWindow.GetSafeHwnd() != NULL )
		{
			g_DesktopWindow.Detach();
		}

      svmIsInitializedLock.Unlock ();
      break;
   } // while (1)

   return hr;
}


/*
The Destroy Instance operator is used to destroy the single image compression instance that was created for this single image compression operation.
*/
HRESULT StaticImageCompressionPool::DestroyInstance ()
{
   HRESULT  hr;

   hr = 0;

   while (1)
   {
//--- This is to destroy the instance window if a pool was not used.
      if (svmpICompressionDlg != NULL)
      {
         svmpICompressionDlg->DestroyWindow ();
      }

      delete svmpICompressionDlg;
      svmpICompressionDlg = NULL;

      break;
   }

   return hr;
}

