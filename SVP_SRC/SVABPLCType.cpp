// ******************************************************************************
// * COPYRIGHT (c) 2000 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVABPLCType
// * .File Name       : $Workfile:   SVABPLCType.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 15:38:40  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVABPLCType.h"
#include "PLCBaseClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVABPLCTypeClass property page

IMPLEMENT_DYNCREATE(SVABPLCTypeClass, CPropertyPage)

SVABPLCTypeClass::SVABPLCTypeClass() : CPropertyPage(SVABPLCTypeClass::IDD)
{
	//{{AFX_DATA_INIT(SVABPLCTypeClass)
	//}}AFX_DATA_INIT
}


SVABPLCTypeClass::~SVABPLCTypeClass()
{
}


void SVABPLCTypeClass::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVABPLCTypeClass)
	DDX_Control(pDX, IDC_SCL500, svmSLC500Radio);
	DDX_Control(pDX, IDC_MICROLOGIX, svmMicroLogixRadio);
	DDX_Control(pDX, IDC_ABPLCTYPE_MANUFACTURER, svmManufacturerEdit);
	//}}AFX_DATA_MAP

//	DDX_Control(pDX, IDC_PLC5, svmPLC5Radio);

}


BEGIN_MESSAGE_MAP(SVABPLCTypeClass, CPropertyPage)
	//{{AFX_MSG_MAP(SVABPLCTypeClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVABPLCTypeClass message handlers


BOOL SVABPLCTypeClass::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
//   svmpParentSheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT); 
   svmpParentSheet->SetWizardButtons (PSWIZB_NEXT); 

   svmManufacturerEdit.SetWindowText (svmpParentSheet->svmManufacturer);

   if (svmpParentSheet->svmPLCType == SV_SLC500)
   {
      svmSLC500Radio.SetCheck (1);
   }
/*   else
   if (svmpParentSheet->svmPLCType == SV_PLC5)
   {
      svmPLC5Radio.SetCheck (1);
   }*/
   else
//   if (svmpParentSheet->svmPLCType == SV_MICROLOGIX)
   {
      svmMicroLogixRadio.SetCheck (1);
   }

	return CPropertyPage::OnSetActive();
}


BOOL SVABPLCTypeClass::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT SVABPLCTypeClass::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class

   if (svmSLC500Radio.GetCheck ())
   {
      svmpParentSheet->svmPLCType = SV_SLC500;

//      svmpParentSheet->
//         SetActivePage (svmpParentSheet->svmpABSLC500ConnectionPage); 
// For Now Only Allow Automated Solutions(ASABtcp)
      svmpParentSheet->svmConnection =    SV_ASABTCP;
	  svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpASABtcpPage); 
   }
/*   else
   if (svmPLC5Radio.GetCheck ())
   {
      svmpParentSheet->svmPLCType =       SV_PLC5;
      svmpParentSheet->svmConnection =    SV_KTXDH;
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpRSLinxSearchPage); 
   }*/
   else
// Micrologix
	if( svmMicroLogixRadio.GetCheck () )
   {
      svmpParentSheet->svmPLCType =       SV_MICROLOGIX;

//	  svmpParentSheet->
//         SetActivePage (svmpParentSheet->svmpABSLC500ConnectionPage); 
// For Now Only Allow Automated Solutions(ASABtcp)
      svmpParentSheet->svmConnection =    SV_ASABTCP;
	  svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpASABtcpPage); 
   }

   svmpParentSheet->svmlLastButtonNext = TRUE;

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVABPLCType.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:38:40   bWalter
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
*/