//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPasswordDlg.cpp
//* .File Name       : $Workfile:   SVPasswordDlg.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   30 May 2014 10:47:12  $
//******************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "SVPasswordDlg.h"
#include "SVSecurity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSVSecurityApp theApp;

SVPasswordDlg::SVPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SVPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVPasswordDlg)
	m_strPassword = _T("");
	m_strUser = _T("");
	m_strStatus = _T("");
	//}}AFX_DATA_INIT

}

SVPasswordDlg::~SVPasswordDlg()
{
}

void SVPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVPasswordDlg)
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_strPassword);
	DDX_Text(pDX, IDC_USER_NAME_EDIT, m_strUser);
	DDX_Text(pDX, IDC_STATUS, m_strStatus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(SVPasswordDlg)
	ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL SVPasswordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	GetDlgItem( IDC_USER_NAME_EDIT )->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVPasswordDlg::OnHelp() 
{
	theApp.HtmlHelp(HH_HELP_CONTEXT,IDD_PASSWORD_DLG + 0x60000);
}

BOOL SVPasswordDlg::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// If the control ID comming thru is from static text or something invalid then
	// redirect it to the password dialog Id.
	if( ( pHelpInfo->iCtrlId & 0xffff ) == 0xffff || 
		pHelpInfo->iCtrlId < 500)
	{
		pHelpInfo->iCtrlId = IDD_PASSWORD_DLG + 0x60000;
	}
	else
	{
		pHelpInfo->iCtrlId += 0x70000;
	}	
	theApp.HtmlHelp(pHelpInfo->iCtrlId,HH_HELP_CONTEXT);
	return TRUE ; 
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSecurity\SVPasswordDlg.cpp_v  $
 * 
 *    Rev 1.2   30 May 2014 10:47:12   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  904
 * SCR Title:  Update SVObserver to use HtmlHelp instead of WinHelp
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed to use HtmlHelp instead of HtmlHelpA
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   20 May 2014 10:25:28   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  904
 * SCR Title:  Update SVObserver to use HtmlHelp instead of WinHelp
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   included SVSecurity.h so the dialogs would have access to theApp.  Changed OnHelp & OnHelpInfo to call HtmlHelp instead of WinHelp
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 17:00:52   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Sep 2012 18:43:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:38   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   10 Oct 2005 16:05:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified logic that checks for invalid ID in OnHelpInfo.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   10 Oct 2005 10:29:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnHelpInfo to call the appropriate help Id for basic types such as IDOK and IDCancel and Static_text with no ID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   10 Oct 2005 08:40:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo message handlers that take the help message Id, add an offset and then call into the SVObserver.hlp file.  The default operation would have tried to call into SVSecurity.hlp with normal IDs.  The offset was put in so there would be no conflict with SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Oct 2005 08:31:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo Message handlers for adding offsets to the resource IDs and calls the SVObserver Help file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   06 Jul 2005 13:49:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Status to password dialog, Cleaned up HRESULT return codes.  Added checkbox to SetupDlg for Auto Redirect o offline to edit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:06:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
