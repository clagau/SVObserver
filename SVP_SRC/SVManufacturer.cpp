// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVManufacturer
// * .File Name       : $Workfile:   SVManufacturer.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:49:22  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVManufacturer.h"
#include "PLCBaseClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVManufacturerClass property page

IMPLEMENT_DYNCREATE(SVManufacturerClass, CPropertyPage)

SVManufacturerClass::SVManufacturerClass() : CPropertyPage(SVManufacturerClass::IDD)
{
	//{{AFX_DATA_INIT(SVManufacturerClass)
	//}}AFX_DATA_INIT
}

SVManufacturerClass::~SVManufacturerClass()
{
}

void SVManufacturerClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVManufacturerClass)
	DDX_Control(pDX, IDC_MANUFACTURER_SIEMENS, svmSiemensRadio);
	DDX_Control(pDX, IDC_MANUFACTURER_AB, svmABRadio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVManufacturerClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVManufacturerClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVManufacturerClass message handlers

BOOL SVManufacturerClass::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
   svmpParentSheet->SetWizardButtons (PSWIZB_NEXT); 

   if (svmpParentSheet->svmManufacturer == SV_ALLENBRADLEY)
   {
      svmABRadio.SetCheck (1);
   }
   else
   {
      svmSiemensRadio.SetCheck (1);
   }

	return CPropertyPage::OnSetActive();
}

LRESULT SVManufacturerClass::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class

   if (svmABRadio.GetCheck ())
   {
      svmpParentSheet->svmManufacturer = SV_ALLENBRADLEY;
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpABPLCTypePage ); 
   }
   else
   if (svmSiemensRadio.GetCheck ())
   {
      svmpParentSheet->svmManufacturer =  SV_SIEMENS;
      svmpParentSheet->svmPLCType =       SV_S7;
      svmpParentSheet->svmConnection =    SV_APPLICOM;
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpSiemensS7ApplicomPage); 
   }

	return TRUE;
      //CPropertyPage::OnWizardNext();
}

BOOL SVManufacturerClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVManufacturer.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:49:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:57:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   14 Nov 2001 15:05:16   jBrown
 * Project:  SVFocusNT
 * Change Request (SCR) nbr:  131, 141
 * SCR Title:  Add support for Allen-Bradley Micrologix PLC's via the Ethernet
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   The following function was modfied in order to integrate the Allen-Bradley MicroLogix and Allen-Bradley PLC5 series PLCs into the wizard dialog functioanllity.
 * 
 * 
 * 
 * SVASABtcpParametersPage::OnWizardNext ()
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