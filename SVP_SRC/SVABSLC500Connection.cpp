// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVABSLC500Connection
// * .File Name       : $Workfile:   SVABSLC500Connection.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:43:34  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVABSLC500Connection.h"
#include "PLCBaseClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVABSLC500ConnectionClass dialog

SVABSLC500ConnectionClass::SVABSLC500ConnectionClass()
	: CPropertyPage(SVABSLC500ConnectionClass::IDD)
{
	//{{AFX_DATA_INIT(SVABSLC500ConnectionClass)
	svmConnectionRadioIndex = -1;
	//}}AFX_DATA_INIT
}

void SVABSLC500ConnectionClass::DoDataExchange(CDataExchange* pDX)
{
//	DDX_Control(pDX, IDC_ASAB_ETH, svmPLCTypeEdit);
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVABSLC500ConnectionClass)
	DDX_Control(pDX, IDC_ABSLC500CONNECTION_PLCTYPE, svmPLCTypeEdit);
	DDX_Control(pDX, IDC_ABSLC500CONNECTION_MANUFACTURER, svmManufacturerEdit);
	DDX_Radio(pDX, IDC_ASAB_ETH, svmConnectionRadioIndex);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVABSLC500ConnectionClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVABSLC500ConnectionClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVABSLC500ConnectionClass message handlers

BOOL SVABSLC500ConnectionClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVABSLC500ConnectionClass::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

   svmpParentSheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT); 

   svmManufacturerEdit.SetWindowText (svmpParentSheet->svmManufacturer);

   svmPLCTypeEdit.SetWindowText (svmpParentSheet->svmManufacturer);

   if (svmpParentSheet->svmConnection == SV_ASABTCP)
   {
      svmConnectionRadioIndex = 0;
   }
   else
   if (svmpParentSheet->svmConnection == SV_KTXDH)
   {
      svmConnectionRadioIndex = 1;
   }
   else
   if (svmpParentSheet->svmConnection == SV_ETHERNET)
   {
      svmConnectionRadioIndex = 2;
   }
	
   UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

LRESULT SVABSLC500ConnectionClass::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
	
   svmpParentSheet->SetActivePage (svmpParentSheet->svmpABPLCTypePage); 

	return TRUE;
}

LRESULT SVABSLC500ConnectionClass::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class
	
   UpdateData(TRUE);

	if (svmConnectionRadioIndex == 0)
	{
		svmpParentSheet->svmConnection = SV_ASABTCP;
		svmpParentSheet->SetActivePage (svmpParentSheet->svmpASABtcpPage); 
	}
	else
	if (svmConnectionRadioIndex == 1)
	{
		svmpParentSheet->svmConnection = SV_KTXDH;
		svmpParentSheet->SetActivePage (svmpParentSheet->svmpRSLinxSearchPage); 
	}
	else
	if (svmConnectionRadioIndex == 2)
	{
		svmpParentSheet->svmConnection = SV_ETHERNET;
		svmpParentSheet->SetActivePage (svmpParentSheet->svmpRSLinxSearchPage); 
	}

	svmpParentSheet->svmlLastButtonNext = TRUE;

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVABSLC500Connection.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:43:34   bWalter
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
 *    Rev 1.1   14 Nov 2001 10:11:52   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  141
 * SCR Title:  Add support for Allen-Bradley PLC5 via the KT card (Data Highway)
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following functions were modfied in order to integrate the Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs into the wizard dialog functioanllity.
 * 
 * SVABSLC500Connection::OnSetActive (), and
 * SVABSLC500Connection::OnWizardBack ()
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