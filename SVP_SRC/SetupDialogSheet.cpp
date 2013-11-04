// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SetupDialogSheet
// * .File Name       : $Workfile:   SetupDialogSheet.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:35:22  $
// ******************************************************************************

#include "stdafx.h"
#include "plc_interface.h"
#include "SetupDialogSheet.h"
#include "PLCEnglish\resource.h"
#include "SVManufacturer.h"
#include "SVABPLCType.h"
#include "SVABSLC500Connection.h"
#include "SVRSLinxSearch.h"
#include "SVABSLC500Ethernet.h"
#include "SVASABtcpParametersPage.h"
#include "SVABSLC500KTX.h"
#include "SVSiemensS7Applicom.h"
#include "SVHeartBeatPage.h"
#include "PLCBaseClass.h"                    // SV_MANUFACTURER, 
                                             // SV_PLC_TYPE, 
                                             // SV_CONNECTION_TYPE 
                                             // SV_DRIVER_NAME
                                             // SV_STATION_NBR
                                             // SV_IP_ADDRESS

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetupDialogSheetClass

IMPLEMENT_DYNAMIC(SetupDialogSheetClass, CPropertySheet)


SetupDialogSheetClass::SetupDialogSheetClass(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   Init ();
}


SetupDialogSheetClass::SetupDialogSheetClass(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   Init ();
}


long SetupDialogSheetClass::Init ()
{
   long           lErr;
//   CPlc_interfaceApp plcInterface;

   lErr = 0;
   while (1)
   {

      svmManufacturer = SV_ALLENBRADLEY;
      svmPLCType = SV_MICROLOGIX;

      svmpManufacturerPage = new SVManufacturerClass ();
      if (svmpManufacturerPage == NULL)
      {
         lErr = -1323;
         break;
      }
      
      AddPage (svmpManufacturerPage);

      svmpABPLCTypePage = new SVABPLCTypeClass ();
      if (svmpABPLCTypePage == NULL)
      {
         lErr = -1324;
         break;
      }
      
      AddPage (svmpABPLCTypePage);

      svmpABSLC500ConnectionPage = new SVABSLC500ConnectionClass ();
      if (svmpABSLC500ConnectionPage == NULL)
      {
         lErr = -1326;
         break;
      }
      
      AddPage (svmpABSLC500ConnectionPage);

      svmpRSLinxSearchPage = new SVRSLinxSearchClass ();
      if (svmpRSLinxSearchPage == NULL)
      {
         lErr = -1327;
         break;
      }
      
      AddPage (svmpRSLinxSearchPage);

      svmpABKTXPage = new SVABSLC500KTXClass ();
      if (svmpABKTXPage == NULL)
      {
         lErr = -1335;
         break;
      }
      
      AddPage (svmpABKTXPage);
//      svmpABSLC500KTXPage->Construct (IDD_ABSLC500KTX);

      svmpABEthernetPage = new SVABSLC500EthernetClass ();
      if (svmpABEthernetPage == NULL)
      {
         lErr = -1339;
         break;
      }
      AddPage (svmpABEthernetPage);

      svmpASABtcpPage = new SVASABtcpParametersPage ();
      if (svmpASABtcpPage == NULL)
      {
         lErr = -1338;
         break;
      }
      AddPage (svmpASABtcpPage);



      svmpSiemensS7ApplicomPage = new SVSiemensS7ApplicomClass ();
      if (svmpSiemensS7ApplicomPage == NULL)
      {
         lErr = -1336;
         break;
      }
      
      AddPage (svmpSiemensS7ApplicomPage);

      svmpHeartBeatPage = new SVHeartBeatPage();
      if (svmpHeartBeatPage == NULL)
      {
         lErr = -1337;
         break;
      }
      AddPage (svmpHeartBeatPage);


// For Now Allow only AllenBradley...
//      SetActivePage (svmpManufacturerPage);
	  SetActivePage(svmpABPLCTypePage);
      
      break;
   }

   return lErr;
}



SetupDialogSheetClass::~SetupDialogSheetClass()
{
      if (svmpManufacturerPage != NULL)
      {
         delete svmpManufacturerPage;
      }
      
      if (svmpABPLCTypePage != NULL)
      {
         delete svmpABPLCTypePage;
      }
      
      if (svmpABSLC500ConnectionPage != NULL)
      {
         delete svmpABSLC500ConnectionPage;
      }
      
      if (svmpRSLinxSearchPage != NULL)
      {
         delete svmpRSLinxSearchPage;
      }
      
      if (svmpABKTXPage != NULL)
      {
         delete svmpABKTXPage;
      }
      
      if (svmpABEthernetPage != NULL)
      {
         delete svmpABEthernetPage;
      }
      if (svmpASABtcpPage != NULL)
      {
         delete svmpASABtcpPage;
      }
      if (svmpSiemensS7ApplicomPage != NULL)
      {
         delete svmpSiemensS7ApplicomPage;
      }
      if (svmpHeartBeatPage != NULL)
      {
         delete svmpHeartBeatPage;
      }
}



long SetupDialogSheetClass::BuildSetupParameterString ()
{
   long  lErr;
   TCHAR czConnectionString [215];


   lErr = 0;

   while (1)
   {

	  
      lErr = BuildSetupParameterString( svmManufacturer,
                                        svmPLCType,
                                        svmConnection,
                                        svmDriverName,
                                        svmIPAddress,
                                        svmDHNodeAddress,
                                        svmChannelNbr,
                                        svmEquipmentNbr,
                                        czConnectionString,
                                        SV_NBRCHARS (czConnectionString));


      svmConnectionParameters.Format (czConnectionString);

      break;
   }


   return lErr;
}


long SetupDialogSheetClass::BuildSetupParameterString ( 
	                                                   const TCHAR* apManufacturer,
                                                       const TCHAR* apPLCType,
                                                       const TCHAR* apConnectionType,
                                                       const TCHAR* apDriverName,
                                                       const TCHAR* apIPAddress,
                                                       const TCHAR* apDHNodeAddress,
                                                       const TCHAR* apChannelNbr,
                                                       const TCHAR* apEquipmentNbr,
                                                       TCHAR* apParameterString,
                                                       long   alParameterStringSize)
													   
{
   long     lErr;

// Since I have had problems passing CStrings to DLLs (freeing and 
// re-allocating to change size is an issue), the passed in string is a 
// TCHAR and the local variable is a CString.
   CString  localParameterString;

   lErr = 0;

   while (1)
   {

      localParameterString.Empty ();
      if (apManufacturer [0] != 0)
      {
         localParameterString+= SV_MANUFACTURER;
         localParameterString+= _T(" = ");
         localParameterString+= apManufacturer;
         localParameterString+= _T("\n");
      }
    
      if (apPLCType [0] != 0)
      {
         localParameterString+= SV_PLC_TYPE;
         localParameterString+= _T(" = ");
         localParameterString+= apPLCType;
         localParameterString+= _T("\n");
      }

      if (apConnectionType [0] != 0)
      {
         localParameterString+= SV_CONNECTION_TYPE;
         localParameterString+= _T(" = ");
         localParameterString+= apConnectionType;
         localParameterString+= _T("\n");
      }

      if (_tcscmp (apManufacturer, SV_ALLENBRADLEY) == 0)
      {
         if (apDriverName [0] != 0)
         {
            localParameterString+= SV_DRIVER_NAME;
            localParameterString+= _T(" = ");
            localParameterString+= apDriverName;
            localParameterString+= _T("\n");
         }

         if (_tcscmp (apConnectionType, SV_ETHERNET) == 0)
         {
            if (apIPAddress [0] != 0)
            {
               localParameterString += SV_IP_ADDRESS;
               localParameterString += _T(" = ");
               localParameterString += apIPAddress;
               localParameterString += _T("\n");
            }
         }
         else
         if (_tcscmp (apConnectionType, SV_ASABTCP) == 0)
         {
            if (apIPAddress [0] != 0)
            {
               localParameterString += SV_IP_ADDRESS;
               localParameterString += _T(" = ");
               localParameterString += apIPAddress;
               localParameterString += _T("\n");
            }
         }
         else
         if (_tcscmp (apConnectionType, SV_KTXDH) == 0)
         {
            if (apDHNodeAddress [0] != 0)
            {
               localParameterString += SV_DHNODE_ADDRESS;
               localParameterString += _T(" = ");
               localParameterString += apDHNodeAddress;
               localParameterString += _T("\n");
            }

         }
         
      }
      else
      if (_tcscmp (apManufacturer, SV_SIEMENS) == 0)
      {
         if (apChannelNbr [0] != 0)
         {
            localParameterString += SV_CHANNEL_NBR;
            localParameterString += _T(" = ");
            localParameterString += apChannelNbr;
            localParameterString += _T("\n");
         }

         if (apEquipmentNbr [0] != 0)
         {
            localParameterString += SV_EQUIPMENT_NBR;
            localParameterString += _T(" = ");
            localParameterString += apEquipmentNbr;
            localParameterString += _T("\n");
         }
      }

      sv_tstrncpy (apParameterString,
                   localParameterString, 
                   alParameterStringSize);

      if (localParameterString.GetLength () >= alParameterStringSize)
      {
         lErr = -1337;
      }

      break;
   }
   return lErr;

}

BEGIN_MESSAGE_MAP(SetupDialogSheetClass, CPropertySheet)
	//{{AFX_MSG_MAP(SetupDialogSheetClass)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetupDialogSheetClass message handlers


BOOL SetupDialogSheetClass::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
   if (wParam == IDC_PLACEBUTTON_FORCENEXT)
   {
      SetWizardButtons (PSWIZB_NEXT);
      PostMessage (WM_COMMAND, IDC_FORCE_NEXT);
      return TRUE;
   }
   else
	if (wParam == IDC_FORCE_NEXT)
   {
      PressButton (PSBTN_NEXT);
      return TRUE;
   }
	else
	if( wParam == IDC_PLACEBUTTON_FORCEBACK)
	{
      SetWizardButtons (PSWIZB_NEXT);
      PostMessage (WM_COMMAND, IDC_FORCE_BACK);
      return TRUE;
	}
   else
	if (wParam == IDC_FORCE_BACK)
   {
      PressButton (PSBTN_BACK);
      return TRUE;
   }

	return CPropertySheet::OnCommand(wParam, lParam);
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SetupDialogSheet.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:35:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   14 Nov 2001 09:50:50   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   SetupDialogSheetClass::Init () was modified in order to support Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/