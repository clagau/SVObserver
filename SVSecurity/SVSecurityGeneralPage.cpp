//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSecurityGeneralPage.cpp
//* .File Name       : $Workfile:   SVSecurityGeneralPage.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   10 Dec 2014 12:10:24  $
//******************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "SVSecurityGeneralPage.h"

#include "SVMessage/SVMessage.h"

#include "SVAccessPointNode.h"
#include "SVAccessClass.h"
#include "SVSecuritySetupSheet.h"
#include "SVSecurity.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSVSecurityApp theApp;

IMPLEMENT_DYNCREATE(SVSecurityGeneralPage, CPropertyPage)

SVSecurityGeneralPage::SVSecurityGeneralPage(UINT nIDTemplate, UINT nIDCaption)
: CPropertyPage(nIDTemplate, nIDCaption)
{
	//{{AFX_DATA_INIT(SVSecuritySetupPage)
	//}}AFX_DATA_INIT
}

SVSecurityGeneralPage::SVSecurityGeneralPage() 
: CPropertyPage(SVSecurityGeneralPage::IDD)
{
}

SVSecurityGeneralPage::~SVSecurityGeneralPage()
{
}

void SVSecurityGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVSecurityGeneralPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVSecurityGeneralPage, CDialog)
	//{{AFX_MSG_MAP(SVSecurityGeneralPage)
	ON_BN_CLICKED(IDC_USER_MGR_BTN, OnUserMgrBtn)
	ON_BN_CLICKED(IDC_EVENT_VIEWER_BTN, OnEventViewerBtn)
	ON_BN_CLICKED(IDC_LOGIN_BTN, OnLoginBtn)
	ON_BN_CLICKED(IDC_NO_SECURITY_BTN, OnNoSecurityBtn)
	ON_BN_CLICKED(IDC_TRADITIONAL_SECURITY_BTN, OnTraditionalSecurityBtn)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_HELP, OnHelp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp )
END_MESSAGE_MAP()

void SVSecurityGeneralPage::OnUserMgrBtn() 
{
	TCHAR tcharSysDir[INFO_BUFFER_SIZE];
	TCHAR tcharCommand[INFO_BUFFER_SIZE];
	tcharCommand[0] = NULL;
	GetSystemDirectory(tcharSysDir,INFO_BUFFER_SIZE);
	_tcscat(tcharCommand,tcharSysDir);

	_tcscat(tcharCommand,_T("\\lusrmgr.msc"));

	CString sStr1(tcharCommand);
	CString sStr2(tcharSysDir);
	//ShellExecute(m_hWnd, _T("open"),_T("c:\\winnt\\System32\\lusrmgr.msc"), _T(""), _T(""), SW_SHOW );
//	m_pAccess->CreateProcess(_T("MMC.exe"),_T("c:\\winnt\\System32"),_T("c:\\winnt\\System32\\lusrmgr.msc"));
	m_pAccess->CreateProcess(_T("MMC.exe"),tcharSysDir,tcharCommand);
}

void SVSecurityGeneralPage::OnEventViewerBtn() 
{
	TCHAR tcharSysDir[INFO_BUFFER_SIZE];
	TCHAR tcharCommand[INFO_BUFFER_SIZE];
	tcharCommand[0] = NULL;
	GetSystemDirectory(tcharSysDir,INFO_BUFFER_SIZE);
	_tcscat(tcharCommand,tcharSysDir);

	_tcscat(tcharCommand,_T("\\eventvwr.msc"));

	CString sStr1(tcharCommand);
	CString sStr2(tcharSysDir);

	//ShellExecute(m_hWnd, _T("open"),_T("c:\\winnt\\System32\\eventvwr.msc"), _T(""), _T(""), SW_SHOW );
	m_pAccess->CreateProcess(_T("MMC.exe"),tcharSysDir,tcharCommand);
}

void SVSecurityGeneralPage::OnLoginBtn() 
{
	m_pAccess->Logon();
}

BOOL SVSecurityGeneralPage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SVSecuritySetupSheet* pPropSheet = (SVSecuritySetupSheet*)GetParent();

	m_pAccess = pPropSheet->m_pAccess;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void SVSecurityGeneralPage::OnNoSecurityBtn() 
{
	int l_iOk = AfxMessageBox(_T("Current Security settings will be Lost"),MB_OKCANCEL );
	if( l_iOk == IDCANCEL )
		return;
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_NEW,                    _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION,   _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION,    _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS,  _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION,     _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_PRINT,                  _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_PRINT_SETUP,            _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SAVE_IMAGE,             _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS,  _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_EXIT,                   _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_UNRESTRICTED_FILE_ACCESS,         _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_PPQ_BAR,                _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE,   _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE,  _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT,   _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL,       _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_RUN,                    _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_STOP,                   _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_REGRESSION_TEST,        _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_TEST,                   _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET,           _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE,          _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON,   _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS,         _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP,      _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN,        _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION,_T("Everybody") );

	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_NEW,                   FALSE );
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION,  FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION,   FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS, FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION,    FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_PRINT,                 FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_PRINT_SETUP,           FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_SAVE_IMAGE,            FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS, FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_FILE_MENU_EXIT,                  FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_UNRESTRICTED_FILE_ACCESS,        FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_VIEW_MENU_PPQ_BAR,               FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE,  FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE, FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT,  FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL,      FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_RUN,                   FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_STOP,                  FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_REGRESSION_TEST,       FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_TEST,                  FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET,          FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE,         FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON,  FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS,        FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP,     FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN,       FALSE);
	m_pAccess->SetForcedPrompt( SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION,FALSE);
}

void SVSecurityGeneralPage::OnTraditionalSecurityBtn() 
{
	int l_iOk = AfxMessageBox(_T("Current Security settings will be Lost"),MB_OKCANCEL );
	if( l_iOk == IDCANCEL )
		return;
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_NEW, _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SELECT_CONFIGURATION, _T("Supervisor Group,Vision Worker Group,Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_CLOSE_CONFIGURATION, _T("Supervisor Group,Vision Worker Group,Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION_AS, _T("Supervisor Group,Vision Worker Group,Move Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SAVE_CONFIGURATION, _T("Supervisor Group,Vision Worker Group,Move Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_PRINT, _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_PRINT_SETUP           , _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_SAVE_IMAGE            , _T("Supervisor Group,Vision Worker Group,Move Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_RECENT_CONFIGURATIONS , _T("Supervisor Group,Vision Worker Group,Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_FILE_MENU_EXIT                  , _T("Supervisor Group,Vision Worker Group,Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_UNRESTRICTED_FILE_ACCESS        , _T("Supervisor Group,Vision Worker Group,Operator Group") );

	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_PPQ_BAR               , _T("Supervisor Group,Vision Worker Group,Operator Group,Move Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_IMAGE_DISPLAY_UPDATE  , _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_RESULT_DISPLAY_UPDATE , _T("Supervisor Group,Vision Worker Group") );
	
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_CURRENT, _T("Supervisor Group,Vision Worker Group,Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_VIEW_MENU_RESET_COUNTS_ALL, _T("Supervisor Group,Vision Worker Group,Operator Group") );

	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_RUN,        _T("Supervisor Group,Vision Worker Group,Operator Group,Move Operator Group,Restricted Operator Group") );
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_RUN, TRUE );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_STOP,       _T("Supervisor Group,Vision Worker Group,Operator Group,Move Operator Group,Restricted Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_REGRESSION_TEST,  _T("Supervisor Group,Vision Worker Group,Operator Group,Move Operator Group,Restricted Operator Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_TEST,             _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_EDIT_TOOLSET,     _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE,        _T("Supervisor Group,Vision Worker Group,Operator Group,Move Operator Group,Restricted Operator Group") );
	m_pAccess->SetForcedPrompt( SECURITY_POINT_MODE_MENU_EXIT_RUN_MODE, TRUE );

	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_ADDITIONAL_ENVIRON, _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_TEST_OUTPUTS,       _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_UTILITIES_SETUP,    _T("Supervisor Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_UTILITIES_RUN,      _T("Supervisor Group,Vision Worker Group") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EXTRAS_MENU_AUTOSAVE_CONFIGURATION, _T("Supervisor Group,Vision Worker Group") );

	m_pAccess->SetUseLogon( true );
}

BOOL SVSecurityGeneralPage::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	// If the control ID comming thru is from static text or something invalid then
	// redirect it to the password dialog Id.
	if(( pHelpInfo->iCtrlId & 0xffff ) == 0xffff || 
		pHelpInfo->iCtrlId < 500)
	{
		pHelpInfo->iCtrlId = IDD_GENERAL_SECURITY_PAGE + 0x60000;
	}
	else
	{
		pHelpInfo->iCtrlId += 0x70000;
	}
	theApp.HtmlHelp(pHelpInfo->iCtrlId,HH_HELP_CONTEXT);
	return TRUE ; 
}

void SVSecurityGeneralPage::OnHelp() 
{
	theApp.HtmlHelp(IDD_PASSWORD_DLG + 0x60000,HH_HELP_CONTEXT);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSecurity\SVSecurityGeneralPage.cpp_v  $
 * 
 *    Rev 1.4   10 Dec 2014 12:10:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  908
 * SCR Title:  Remove option for Operator Move (SVO 101)
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed default security for Mode Operator Move
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   07 Jul 2014 17:06:00   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  900
 * SCR Title:  Separate View Image Update, View Result Update flags; remote access E55,E92
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Split the Security access from Image Online Display to Image Display Update and Result Display Update
 * Methods changed OnTraditionalSecurityBtn, OnNoSecurityBtn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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
 *    Rev 1.0   25 Apr 2013 17:02:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   18 Sep 2012 18:43:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library and to the Configuration Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Sep 2012 17:35:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the templated XML objects that moved from the SVLibrary to the new XML Library.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   15 Dec 2009 10:55:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   06 Jun 2008 11:43:44   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  603
 * SCR Title:  Update SVObserver to run under Windows XP
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problem with the system path.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   26 Mar 2008 09:33:24   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  603
 * SCR Title:  Update SVObserver to run under Windows XP
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   Changed the code to use the API to get the System32 path, instead of using the hard coded path.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   10 Oct 2005 16:05:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified logic that checks for invalid ID in OnHelpInfo.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   10 Oct 2005 10:29:34   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified OnHelpInfo to call the appropriate help Id for basic types such as IDOK and IDCancel and Static_text with no ID.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Oct 2005 08:40:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo message handlers that take the help message Id, add an offset and then call into the SVObserver.hlp file.  The default operation would have tried to call into SVSecurity.hlp with normal IDs.  The offset was put in so there would be no conflict with SVObserver.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Oct 2005 08:31:54   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added OnHelp and OnHelpInfo Message handlers for adding offsets to the resource IDs and calls the SVObserver Help file.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   31 Aug 2005 07:07:06   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Changed wording in messgebox to "Current Security Settings  will be lost"
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   23 Aug 2005 14:54:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Removed High Security Button from General Page
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   22 Aug 2005 10:49:08   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added ForcedPrompt Defaults, and a new accesspoint for Exit Run Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   17 Aug 2005 12:01:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Traditional Security to include new Exit Run Mode.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   09 Aug 2005 11:48:36   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified Traditiional Securitys standard NT Group names.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Aug 2005 09:52:28   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Default Security Buttons in the General Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Aug 2005 09:52:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Default Security Buttons in the General Page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   21 Jun 2005 15:08:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  455
 * SCR Title:  New Security for SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added PVCS Header
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
