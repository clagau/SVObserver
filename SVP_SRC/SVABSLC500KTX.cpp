// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVABSLC500KTX
// * .File Name       : $Workfile:   SVABSLC500KTX.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:44:42  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVABSLC500KTX.h"
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
// SVABSLC500KTXClass property page

IMPLEMENT_DYNCREATE(SVABSLC500KTXClass, CPropertyPage)

SVABSLC500KTXClass::SVABSLC500KTXClass() : CPropertyPage(SVABSLC500KTXClass::IDD)
{
	//{{AFX_DATA_INIT(SVABSLC500KTXClass)
	//}}AFX_DATA_INIT
}

SVABSLC500KTXClass::~SVABSLC500KTXClass()
{
}

void SVABSLC500KTXClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVABSLC500KTXClass)
	DDX_Control(pDX, IDC_DHNODE_ADDRESS, svmDHNodeAddressEdit);
	DDX_Control(pDX, IDC_ABSLC500KTX_PLCTYPE, svmPLCTypeEdit);
	DDX_Control(pDX, IDC_ABSLC500KTX_MANUFACTURER, svmManufacturerEdit);
	DDX_Control(pDX, IDC_ABSLC500KTX_CONNECTION, svmConnectionEdit);
	DDX_Control(pDX, IDC_KTXRSLINXCONNECTION_COMBO, svmRSLinxCombo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVABSLC500KTXClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVABSLC500KTXClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVABSLC500KTXClass message handlers

BOOL SVABSLC500KTXClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVABSLC500KTXClass::OnSetActive() 
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
   svmDHNodeAddressEdit.SetWindowText (svmpParentSheet->svmDHNodeAddress);

   lNbrConnections = svmpParentSheet->svmConnections.GetSize ();

// Clear any existing items from Combo box list.
   svmRSLinxCombo.ResetContent ();

   for (lCount = 0; 
        lCount < lNbrConnections;
        lCount++)
   {
//AfxMessageBox( svmpParentSheet->svmConnections [lCount]);
      lErr = svmRSLinxCombo.AddString (svmpParentSheet->svmConnections [lCount]);
   }

   svmRSLinxCombo.GetWindowText (svmpParentSheet->svmDriverName);
   svmDHNodeAddressEdit.GetWindowText (svmpParentSheet->svmDHNodeAddress);
//   UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL SVABSLC500KTXClass::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
   long  lErr = 0;

   while (1)
   {
      svmRSLinxCombo.GetWindowText (svmpParentSheet->svmDriverName);
      svmDHNodeAddressEdit.GetWindowText (svmpParentSheet->svmDHNodeAddress);

      svmpParentSheet->BuildSetupParameterString ();
	   
      break;
   }

	return CPropertyPage::OnWizardFinish();
}

LRESULT SVABSLC500KTXClass::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
   if (svmpParentSheet->svmPLCType == SV_SLC500)
   {
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpABSLC500ConnectionPage); 
   }
   else
   if (svmpParentSheet->svmPLCType == SV_PLC5)
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVABSLC500KTX.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:44:42   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:56:46   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Nov 2001 14:57:54   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following functions were modfied in order to integrate the Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs into the wizard dialog functioanllity.
 * 
 * 
 * 
 * SVABSLC500KTXClass::OnSetActive (), and
 * 
 * SVABSLC500KTXClass::OnWizardBack ()
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Nov 2001 14:57:38   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following functions were modfied in order to integrate the Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs into the wizard dialog functioanllity.
 * 
 * 
 * 
 * SVABSLC500KTXClass::OnSetActive (), and
 * 
 * SVABSLC500KTXClass::OnWizardBack ()
 * 
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   12 Sep 2000 10:57:16   Jim
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