// ******************************************************************************
// * COPYRIGHT (c) 2008 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVHeartBeatPage
// * .File Name       : $Workfile:   SVHeartBeatPage.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   :     $Date:   25 Apr 2013 15:48:16  $
// ******************************************************************************

#include "stdafx.h"
#include "PLCEnglish\resource.h"
#include "SVHeartBeatPage.h"
#include "PLCBaseClass.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVHeartBeatPage property page

IMPLEMENT_DYNCREATE(SVHeartBeatPage, CPropertyPage)


SVHeartBeatPage::SVHeartBeatPage() : CPropertyPage(SVHeartBeatPage::IDD)
{
	//{{AFX_DATA_INIT(SVHeartBeatPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


SVHeartBeatPage::~SVHeartBeatPage()
{
}


void SVHeartBeatPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVHeartBeatPage)
	DDX_Control(pDX, IDC_HEART_BEAT_ADDRESS, m_HeartBeatAddress);
	DDX_Control(pDX, IDC_HEART_BEAT_TIME, m_HeartBeatTimeOut);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVHeartBeatPage, CPropertyPage)
	//{{AFX_MSG_MAP(SVHeartBeatPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVHeartBeatPage message handlers

BOOL SVHeartBeatPage::OnSetActive() 
{
	// TODO: Add your specialized code here and/or call the base class

	long  lErr;

	lErr = 0;

	svmpParentSheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_FINISH); 

	m_HeartBeatAddress.SetWindowText ( svmpParentSheet->m_strHeartBeatAddress );
	CString l_strTmp;
	l_strTmp.Format( "%d", svmpParentSheet->m_lHeartBeatTime );
	m_HeartBeatTimeOut.SetWindowText ( l_strTmp );

	return CPropertyPage::OnSetActive();
}

BOOL SVHeartBeatPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	
   svmpParentSheet = (SetupDialogSheetClass*) GetParent ();


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL SVHeartBeatPage::OnWizardFinish() 
{

	long  lErr;


	lErr = 0;

	CString l_strHeartBeatAddr;
	m_HeartBeatAddress.GetWindowText (l_strHeartBeatAddr );
	if( l_strHeartBeatAddr.GetLength() < 4 
		|| (l_strHeartBeatAddr.Find(_T(':')) < 1) 
		|| (l_strHeartBeatAddr.Find(_T(':')) >= l_strHeartBeatAddr.GetLength()-1) 
		|| isalpha(l_strHeartBeatAddr[0]) == 0 )
	{
		AfxMessageBox("Enter a valid PLC Address for Heartbeat.");
		return FALSE;
	}

	svmpParentSheet->m_strHeartBeatAddress = l_strHeartBeatAddr;
	CString l_strTmp;
	m_HeartBeatTimeOut.GetWindowText ( l_strTmp );
	svmpParentSheet->m_lHeartBeatTime = atol( l_strTmp );

	svmpParentSheet->BuildSetupParameterString ();
	   

	return CPropertyPage::OnWizardFinish();
}


LRESULT SVHeartBeatPage::OnWizardBack() 
{
	svmpParentSheet->
	 SetActivePage (svmpParentSheet->svmpASABtcpPage); 

	return TRUE;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVP_SRC\SVHeartBeatPage.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 15:48:16   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 Sep 2012 11:08:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  783
 * SCR Title:  Update SVObserver to Validate PLC Sub-system Before Entering a Running Mode
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added validation to heartbeat address.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   14 Nov 2008 14:57:26   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  615
 * SCR Title:  Integrate PLC Classes into SVObserver Outputs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
