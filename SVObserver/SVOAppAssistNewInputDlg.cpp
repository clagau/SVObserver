//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOAppAssistNewInputDlg
//* .File Name       : $Workfile:   SVOAppAssistNewInputDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:38:12  $
//******************************************************************************

#include "stdafx.h"
#include "SVOAppAssistNewInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSVOAppAssistNewInputDlg dialog


CSVOAppAssistNewInputDlg::CSVOAppAssistNewInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSVOAppAssistNewInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSVOAppAssistNewInputDlg)
	m_sEditData = _T("");
	//}}AFX_DATA_INIT
}


void CSVOAppAssistNewInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSVOAppAssistNewInputDlg)
	DDX_Control(pDX, IDC_EDT_NEWINPUT, m_ctlEditData);
	DDX_Text(pDX, IDC_EDT_NEWINPUT, m_sEditData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSVOAppAssistNewInputDlg, CDialog)
	//{{AFX_MSG_MAP(CSVOAppAssistNewInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSVOAppAssistNewInputDlg message handlers

void CSVOAppAssistNewInputDlg::SetDialogTitle(CString sDialogTxt)
{
	m_sDialogTitle = sDialogTxt;
}

void CSVOAppAssistNewInputDlg::SetStaticTxtMessage(CString sMsg)
{
	m_sStaticTxt = sMsg;
}

BOOL CSVOAppAssistNewInputDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(m_sDialogTitle);
	GetDlgItem( IDC_STATIC_TXT_MSG )->SetWindowText( m_sStaticTxt );
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSVOAppAssistNewInputDlg::OnOK() 
{
    m_ctlEditData.UpdateData(TRUE);
    m_ctlEditData.GetWindowText(m_sEditData);
	// TODO: Add extra validation here
    m_sEditData.TrimRight(" ");
    m_sEditData.TrimLeft(" ");
    m_ctlEditData.SetWindowText(m_sEditData);
	
	CDialog::OnOK();
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOAppAssistNewInputDlg.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:38:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 11:25:00   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Nov 2002 13:33:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   This is the first revision of this file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/