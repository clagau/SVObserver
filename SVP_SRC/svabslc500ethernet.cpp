// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVABSLC500Ethernet
// * .File Name       : $Workfile:   svabslc500ethernet.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:44:08  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVABSLC500Ethernet.h"
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
// SVABSLC500EthernetClass property page

IMPLEMENT_DYNCREATE(SVABSLC500EthernetClass, CPropertyPage)

SVABSLC500EthernetClass::SVABSLC500EthernetClass() : CPropertyPage(SVABSLC500EthernetClass::IDD)
{
	//{{AFX_DATA_INIT(SVABSLC500EthernetClass)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

SVABSLC500EthernetClass::~SVABSLC500EthernetClass()
{
}

void SVABSLC500EthernetClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVABSLC500EthernetClass)
	DDX_Control(pDX, IDC_IPADDRESS_EDIT, svmIPAddressEdit);
	DDX_Control(pDX, IDC_RSLINXCONEXTION_COMBO, svmRSLinxCombo);
	DDX_Control(pDX, IDC_ABSLC500ETHERNET_PLCTYPE, svmPLCTypeEdit);
	DDX_Control(pDX, IDC_ABSLC500ETHERNET_MANUFACTURER, svmManufacturerEdit);
	DDX_Control(pDX, IDC_ABSLC500ETHERNET_CONNECTION, svmConnectionEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVABSLC500EthernetClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVABSLC500EthernetClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVABSLC500EthernetClass message handlers

BOOL SVABSLC500EthernetClass::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
   long  lErr;
   long  lCount;
   long  lNbrConnections;

   lErr = 0;

   svmpParentSheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_FINISH); 

   svmManufacturerEdit.SetWindowText (svmpParentSheet->svmManufacturer);
   svmPLCTypeEdit.SetWindowText (svmpParentSheet->svmPLCType);
   svmConnectionEdit.SetWindowText (svmpParentSheet->svmConnection);

// Clear any existing items from Combo box list.
   svmRSLinxCombo.ResetContent ();

   lNbrConnections = svmpParentSheet->svmConnections.GetSize ();

   for (lCount = 0; 
        lCount < lNbrConnections;
        lCount++)
   {
//AfxMessageBox( svmpParentSheet->svmConnections [lCount]);
      lErr = svmRSLinxCombo.AddString (svmpParentSheet->svmConnections [lCount]);
   }

   svmRSLinxCombo.SetWindowText (svmpParentSheet->svmDriverName);
   svmIPAddressEdit.SetWindowText (svmpParentSheet->svmIPAddress);
//   UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL SVABSLC500EthernetClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVABSLC500EthernetClass::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class

   long  lErr;


   lErr = 0;

   while (1)
   {
      svmRSLinxCombo.GetWindowText (svmpParentSheet->svmDriverName);
      svmIPAddressEdit.GetWindowText (svmpParentSheet->svmIPAddress);

   // The ABPLCType page will be skipped because there is only one 
   // AllenBradley PLC which is supported.
// (already done)      svmpParentSheet->svmPLCType = SV_SLC500;
      svmpParentSheet->BuildSetupParameterString ();
	   
      break;
   }

   return CPropertyPage::OnWizardFinish();
}

LRESULT SVABSLC500EthernetClass::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class

   if (svmpParentSheet->svmPLCType == SV_SLC500)
   {
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpABSLC500ConnectionPage); 
   }
   else
   if (svmpParentSheet->svmPLCType == SV_MICROLOGIX)
   {
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpABPLCTypePage); 
   }

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\svabslc500ethernet.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:44:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Nov 2001 10:14:36   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following functions were modfied in order to integrate the Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs into the wizard dialog functioanllity.
 * 
 * SVABSLC500EthernetClass::OnSetActive (), and
 * SVABSLC500EthernetClass::OnWizardBack ()
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Sep 2000 10:57:14   Jim
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