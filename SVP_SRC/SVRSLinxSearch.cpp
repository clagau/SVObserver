// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRSLinxSearch
// * .File Name       : $Workfile:   SVRSLinxSearch.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   13 May 2013 11:21:04  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVRSLinxSearch.h"
#include "SVTimerLibrary/SVClock.h"
#include "PLCBaseClass.h"
#include "SVABSLC500Ethernet.h"
#include "SVABSLC500KTX.h"
#include "plc_interface.h"                 // IDC_FORCE_NEXT

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRSLinxSearchClass property page

IMPLEMENT_DYNCREATE(SVRSLinxSearchClass, CPropertyPage)


SVRSLinxSearchClass::SVRSLinxSearchClass() : CPropertyPage(SVRSLinxSearchClass::IDD)
{
	//{{AFX_DATA_INIT(SVRSLinxSearchClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


SVRSLinxSearchClass::~SVRSLinxSearchClass()
{
}


void SVRSLinxSearchClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVRSLinxSearchClass)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVRSLinxSearchClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVRSLinxSearchClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVRSLinxSearchClass message handlers


BOOL SVRSLinxSearchClass::OnInitDialog() 
{
   long  lErr;


	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here

   lErr = 0;

   while (1)
   {
      svmpParentSheet = (SetupDialogSheetClass*) GetParent ();

      break;
   }


	
	return TRUE;  // return TRUE unless you set the focus to a control
                 // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL SVRSLinxSearchClass::OnSetActive() 
{
   long           lErr;
	// TODO: Add your specialized code here and/or call the base class

   lErr = 0;

   while (1)
   {
      svmpParentSheet->SetWizardButtons (0); 
	
	  if( svmpParentSheet->svmhRSLinxLib == NULL )
	  {
         lErr = -1372;
         break;
	  }
	  svmhBuildRSLinxConnectionListThread = CreateThread (NULL, 
											  0, 
											  BuildRSLinxConnectionList, 
											  this, 
											  0, 
											  &svmdwBuildRSLinxConnectionListThreadId);
      if (!svmhBuildRSLinxConnectionListThread)
      {
         lErr = -1330;
         break;
      }

      if (!CPropertyPage::OnSetActive())
      {
         lErr = -1371;
         break;
      }

      break;
   }


   if (lErr < 0)
   {
	   svmpParentSheet->PostMessage (WM_COMMAND, IDC_PLACEBUTTON_FORCEBACK);
   }

   return TRUE;
}



DWORD WINAPI SVRSLinxSearchClass::
   BuildRSLinxConnectionList (LPVOID apCallingClass)
{
   long                    lCount;
   long                    lErr;
   long                    lDTLInitialized;

   SVRSLinxSearchClass*    pParentPage;
   SetupDialogSheetClass*  pParentSheet;

	// TODO: Add your specialized code here and/or call the base class

   lErr = 0;
   lDTLInitialized = FALSE;

   while (1)
   {
      pParentPage = (SVRSLinxSearchClass*) apCallingClass;
      pParentSheet = pParentPage->svmpParentSheet;

      pParentSheet->svmConnections.RemoveAll ();

      if (pParentSheet->svmhRSLinxLib)
      {
         lErr = -DTL_INIT (25);  // 641 = maximum number of data items

         if (lErr < 0)
            break;
         else
            lDTLInitialized = TRUE;
      }


      //--- 5000 ms timeout -------------------------------------------------------
      if (pParentSheet->svmhRSLinxLib)
      {
         lErr = -DTL_DRIVER_LIST (pParentPage->svmDtlDrivers, 
                                  (unsigned long *) &pParentPage->svmlNbrDrivers, 5000l);   
      }
      else
      {
//------ If RSLinx is not loaded, then there are no drivers available.
         pParentPage->svmlNbrDrivers = 0;
      }

      if (lErr < 0)
         break;

      for (lCount = 0; lCount < pParentPage->svmlNbrDrivers; lCount++)
      {
         if (pParentSheet->svmConnection == SV_ETHERNET)
         {
            if ((pParentPage->svmDtlDrivers [lCount].wNetworkType & DTL_NETTYPE_ENET) != 0)
            {
//------------ List only Ethernet connections. 
               pParentSheet->svmConnections.Add (A2T(pParentPage->svmDtlDrivers [lCount].szDriverName));
//               continue;
            }
         }
         else
         if (pParentSheet->svmConnection == SV_KTXDH)
         {
            if ((pParentPage->svmDtlDrivers [lCount].wNetworkType & DTL_NETTYPE_DHP) != 0)
            {
//------------ List only DataHighway connections. 
               pParentSheet->svmConnections.Add (A2T(pParentPage->svmDtlDrivers [lCount].szDriverName));
//               continue;
            }
         }
      } // for (lCount = 0; lCount < pParentPage->svmlNbrDrivers; lCount++)


      break;
   }

   if (lDTLInitialized == TRUE)
   {
      if (pParentSheet->svmhRSLinxLib)
      {
         DTL_UNINIT (DTL_E_FAIL);
      }
   }

   if (lErr < 0)
   {
      CString msg;
      msg.Format (_T("BuildRSLinxConnectionList error is %d"), lErr);
      ::MessageBox (HWND_DESKTOP, (LPCTSTR) msg, _T(""), MB_OK);
   }

   pParentSheet->PostMessage (WM_COMMAND, IDC_PLACEBUTTON_FORCENEXT);

   return lErr;
}

//LRESULT SVRSLinxSearchClass::OnWizardBack() 
//{
//   svmpParentSheet->SetActivePage (svmpParentSheet->svmpABPLCTypePage); 
//}

LRESULT SVRSLinxSearchClass::OnWizardNext() 
{
   long        lErr;

   DWORD       dwExitCode;



   lErr = 0;

   while (1)
   {
	   SVClock::SVTimeStamp l_Start = SVClock::GetTimeStamp();

      do
      {
         GetExitCodeThread (svmhBuildRSLinxConnectionListThread, &dwExitCode);
         if (10000 < SVClock::ConvertTo( SVClock::Milliseconds, ( SVClock::GetTimeStamp() - l_Start )))
         {
            lErr = -1332;
            break;
         }
      } while (dwExitCode == STILL_ACTIVE && lErr == 0);


//      if (lErr < 0)
//         break;

      if (svmpParentSheet->svmConnection == SV_ETHERNET)
      {
         svmpParentSheet->SetActivePage (svmpParentSheet->svmpABEthernetPage); 
      }
      else
      if (svmpParentSheet->svmConnection == SV_KTXDH)
      {
         svmpParentSheet->SetActivePage (svmpParentSheet->svmpABKTXPage); 
      }      

      break;
   }

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVRSLinxSearch.cpp_v  $
 * 
 *    Rev 1.2   13 May 2013 11:21:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 014.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   07 May 2013 13:02:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use new SVClock conversions functions, which will clarify timing tolerance checks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:36:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Apr 2013 13:29:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 16:05:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:59:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Nov 2001 15:15:16   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following function was modfied in order to integrate the Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs into the wizard dialog functioanllity.
 * 
 * 
 * 
 * SVRSLinxSearchClass::OnWizardBack ()
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Oct 2000 09:19:02   mike
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  55
 * SCR Title:  SVFocus Remote control from PLC
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Project would not build as originally checked-in. Corrected problems with missing files, incorrect dependencies/includes and removed unnecessary files from project.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Sep 2000 10:57:18   Jim
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  14
 * SCR Title:  Stage 2: Expand PLC Support
 * Checked in by:  Jim;  James A. Brown
 * Change Description:  
 *   These files were added to support dialog configuration of PLC connection.  
 * 
 * Supported connection types are:
 * Siemens S7 300 and 400 series via profibus.
 * AllenBradley SLC505 via Ethernet.
 * AllenBradley SLC504 via Data Highway.
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/