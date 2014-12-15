// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionExitDlg
// * .File Name       : $Workfile:   SVRegressionExitDlg.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   12 Dec 2014 09:52:14  $
// ******************************************************************************

#include "stdafx.h"
#include "svobserver.h"
#include "SVRegressionExitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SVRegressionExitDlg dialog
extern HRESULT GlobalRCSetMode(unsigned long lSVIMNewMode);


SVRegressionExitDlg::SVRegressionExitDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVRegressionExitDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVRegressionExitDlg)
	m_csExitModes = _T("");
	//}}AFX_DATA_INIT
}


void SVRegressionExitDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVRegressionExitDlg)
	DDX_Control(pDX, IDC_COMBO_EXIT_MODE, m_ctlExitModes);
	DDX_CBString(pDX, IDC_COMBO_EXIT_MODE, m_csExitModes);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVRegressionExitDlg, CDialog)
	//{{AFX_MSG_MAP(SVRegressionExitDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SVRegressionExitDlg message handlers

BOOL SVRegressionExitDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_ctlExitModes.AddString("Run");
	m_ctlExitModes.AddString("Stop");
	m_ctlExitModes.AddString("Test");
	m_ctlExitModes.AddString("Edit");

	m_ctlExitModes.SelectString(0,"Edit");
	
	UpdateData(TRUE);
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVRegressionExitDlg::OnOK() 
{
	UpdateData(TRUE);
	
	//check to see what m_csExitModes is set to

	if ( m_csExitModes.CompareNoCase("Run") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_GO_ONLINE, 0L);
	}

	else if ( m_csExitModes.CompareNoCase("Stop") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_GO_OFFLINE, 0L);
	}

	else if ( m_csExitModes.CompareNoCase("Test") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_MODE_TEST, 0L);
	}

	else if ( m_csExitModes.CompareNoCase("Edit") == 0 )
	{
		::PostMessage (AfxGetMainWnd()->m_hWnd, WM_COMMAND, ID_MODE_EDIT, 0L);
	}

	CDialog::OnOK();
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRegressionExitDlg.cpp_v  $
 * 
 *    Rev 1.2   12 Dec 2014 09:52:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed text from mode selection drop list.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Dec 2014 12:02:20   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed Edit Move definitions.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:40:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Aug 2005 11:14:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/