// ******************************************************************************
// * COPYRIGHT (c) 2008 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVASABtcpParametersPage
// * .File Name       : $Workfile:   SVASABtcpParametersPage.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   :     $Date:   25 Apr 2013 15:46:20  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVASABtcpParametersPage.h"
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
// SVASABtcpParametersPage property page
//
// ASABtcp setup page allows setup of ip address and ENI or * EIP/CIP options.

IMPLEMENT_DYNCREATE(SVASABtcpParametersPage, CPropertyPage)

SVASABtcpParametersPage::SVASABtcpParametersPage() : CPropertyPage(SVASABtcpParametersPage::IDD)
{
	//{{AFX_DATA_INIT(SVASABtcpParametersPage)
		// NOTE: the ClassWizard will add member initialization here
	m_dwIPAddress=0;
	//}}AFX_DATA_INIT
}

SVASABtcpParametersPage::~SVASABtcpParametersPage()
{
}

void SVASABtcpParametersPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVASABtcpParametersPage)
	DDX_IPAddress(pDX, IDC_IPADDRESS_EDIT, m_dwIPAddress);
	DDX_Control(pDX, IDC_ABSLC500ETHERNET_PLCTYPE, svmPLCTypeEdit);
	DDX_Control(pDX, IDC_ABSLC500ETHERNET_MANUFACTURER, svmManufacturerEdit);
	DDX_Control(pDX, IDC_ABSLC500ETHERNET_CONNECTION, svmConnectionEdit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVASABtcpParametersPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVASABtcpParametersPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVASABtcpParametersPage message handlers

DWORD SVASABtcpParametersPage::ExtractIPAddress( CString p_strIPAddress )
{
	int start = 0;
	DWORD l_Result = 0;
	for( int i = 3 ; i >= 0 ; i-- )
	{
		if( start >=0)
		{
			CString part = p_strIPAddress.Tokenize(".", start);
			if( part.SpanExcluding(_T("0123456789")).IsEmpty() )
			{
				l_Result += atol(part) << (i * 8);
			}
		}
	}
	return l_Result;
}

BOOL SVASABtcpParametersPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class
	
   long  lErr;

   lErr = 0;

   svmpParentSheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT); 

   svmManufacturerEdit.SetWindowText (svmpParentSheet->svmManufacturer);
   svmPLCTypeEdit.SetWindowText (svmpParentSheet->svmPLCType);
   svmConnectionEdit.SetWindowText (svmpParentSheet->svmConnection);

   m_dwIPAddress = ExtractIPAddress( svmpParentSheet->svmIPAddress );
  
   // svmIPAddressEdit.SetWindowText ();
   UpdateData(FALSE);

	return CPropertyPage::OnSetActive();
}

BOOL SVASABtcpParametersPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT SVASABtcpParametersPage::OnWizardNext() 
{
	// TODO: Add your specialized code here and/or call the base class

   long  lErr;


   lErr = 0;
	UpdateData();
   while (1)
   {
      CString l_strIPAddress;
      //svmIPAddressEdit.GetWindowText (l_strIPAddress);
	  l_strIPAddress.Format(_T("%d.%d.%d.%d"), ((m_dwIPAddress >> 24)& 255),((m_dwIPAddress >> 16)& 255),((m_dwIPAddress >> 8)& 255),(m_dwIPAddress & 255));	  
	  svmpParentSheet->svmIPAddress = l_strIPAddress;
	  svmpParentSheet->
		 SetActivePage (svmpParentSheet->svmpHeartBeatPage ); 
      break;
   }

	return CPropertyPage::OnWizardNext();
}

LRESULT SVASABtcpParametersPage::OnWizardBack() 
{
	// TODO: Add your specialized code here and/or call the base class

   if (svmpParentSheet->svmPLCType == SV_SLC500)
   {
//      svmpParentSheet->
//         SetActivePage (svmpParentSheet->svmpABSLC500ConnectionPage); 
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpABPLCTypePage); 
   }
   else
   if (svmpParentSheet->svmPLCType == SV_MICROLOGIX)
   {
//      svmpParentSheet->
//         SetActivePage (svmpParentSheet->svmpABSLC500ConnectionPage); 
      svmpParentSheet->
         SetActivePage (svmpParentSheet->svmpABPLCTypePage); 
   }

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVASABtcpParametersPage.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:46:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Sep 2012 11:02:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed IP edit to IP control.
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
*/