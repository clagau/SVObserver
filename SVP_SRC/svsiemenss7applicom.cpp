// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVSiemensS7Applicom
// * .File Name       : $Workfile:   svsiemenss7applicom.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 16:06:18  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVSiemensS7Applicom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVSiemensS7ApplicomClass property page

IMPLEMENT_DYNCREATE(SVSiemensS7ApplicomClass, CPropertyPage)

SVSiemensS7ApplicomClass::SVSiemensS7ApplicomClass() : CPropertyPage(SVSiemensS7ApplicomClass::IDD)
{
	//{{AFX_DATA_INIT(SVSiemensS7ApplicomClass)
	//}}AFX_DATA_INIT
}

SVSiemensS7ApplicomClass::~SVSiemensS7ApplicomClass()
{
}

void SVSiemensS7ApplicomClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVSiemensS7ApplicomClass)
	DDX_Control(pDX, IDC_S7EQUIPMENT_NBR, svmEquipmentNbrEdit);
	DDX_Control(pDX, IDC_S7CHANNEL_NBR, svmChannelNbrEdit);
	DDX_Control(pDX, IDC_S7_CONNECTION, svmConnectionEdit);
	DDX_Control(pDX, IDC_S7_PLCTYPE, svmPLCTypeEdit);
	DDX_Control(pDX, IDC_S7_MANUFACTURER, svmManufacturerEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVSiemensS7ApplicomClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVSiemensS7ApplicomClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVSiemensS7ApplicomClass message handlers

BOOL SVSiemensS7ApplicomClass::OnWizardFinish() 
{
	// TODO: Add your specialized code here and/or call the base class
   long lErr = 0;

   while (1)
   {
      svmEquipmentNbrEdit.GetWindowText (svmpParentSheet->svmEquipmentNbr);
      svmChannelNbrEdit.GetWindowText (svmpParentSheet->svmChannelNbr);

      svmpParentSheet->BuildSetupParameterString ();
	   
      break;
   }

   return CPropertyPage::OnWizardFinish();
}

BOOL SVSiemensS7ApplicomClass::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
   long lErr = 0;

   svmpParentSheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_FINISH); 

   svmManufacturerEdit.SetWindowText (svmpParentSheet->svmManufacturer);
   svmPLCTypeEdit.SetWindowText (svmpParentSheet->svmPLCType);
   svmConnectionEdit.SetWindowText (svmpParentSheet->svmConnection);

	return CPropertyPage::OnSetActive();
}

BOOL SVSiemensS7ApplicomClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT SVSiemensS7ApplicomClass::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class
   svmpParentSheet->SetActivePage (svmpParentSheet->svmpManufacturerPage); 

   return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\svsiemenss7applicom.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 16:06:18   bWalter
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
 *    Rev 1.0   12 Sep 2000 10:57:20   Jim
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