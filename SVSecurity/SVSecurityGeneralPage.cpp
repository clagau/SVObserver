//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
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

#include "SVAccessClass.h"
#include "SVSecuritySetupSheet.h"
#include "SVSecurityApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern SVSecurityApp theApp;

IMPLEMENT_DYNCREATE(SVSecurityGeneralPage, CPropertyPage)

SVSecurityGeneralPage::SVSecurityGeneralPage(UINT nIDTemplate, UINT nIDCaption)
: CPropertyPage(nIDTemplate, nIDCaption)
{
	//{{AFX_DATA_INIT(SVSecuritySetupPage)
	//}}AFX_DATA_INIT
}

SVSecurityGeneralPage::SVSecurityGeneralPage() : CPropertyPage(SVSecurityGeneralPage::IDD)
{
}

void SVSecurityGeneralPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVSecurityGeneralPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVSecurityGeneralPage, CPropertyPage)
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
	tcharCommand[0] = _T('\0');
	GetSystemDirectory(tcharSysDir,INFO_BUFFER_SIZE);
	_tcscat(tcharCommand, tcharSysDir);

	_tcscat(tcharCommand, _T("\\lusrmgr.msc"));

	m_pAccess->CreateProcess(_T("MMC.exe"), tcharSysDir, tcharCommand);
}

void SVSecurityGeneralPage::OnEventViewerBtn() 
{
	TCHAR tcharSysDir[INFO_BUFFER_SIZE];
	TCHAR tcharCommand[INFO_BUFFER_SIZE];
	tcharCommand[0] = _T('\0');
	GetSystemDirectory(tcharSysDir,INFO_BUFFER_SIZE);
	_tcscat(tcharCommand,tcharSysDir);

	_tcscat(tcharCommand,_T("\\eventvwr.msc"));

	m_pAccess->CreateProcess(_T("MMC.exe"), tcharSysDir, tcharCommand);
}

void SVSecurityGeneralPage::OnLoginBtn() 
{
	m_pAccess->Logon();
}

BOOL SVSecurityGeneralPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SVSecuritySetupSheet* pPropSheet = dynamic_cast<SVSecuritySetupSheet*> (GetParent());

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
	//m_pAccess->SetNTGroup( SECURITY_POINT_EDIT_MENU,                       _T("Everybody") );
	m_pAccess->SetNTGroup( SECURITY_POINT_EDIT_MENU_RESULT_PICKER,         _T("Everybody") );
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

	m_pAccess->SetForcedPrompt( SECURITY_POINT_EDIT_MENU_RESULT_PICKER,        FALSE);

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

	m_pAccess->SetNTGroup( SECURITY_POINT_EDIT_MENU_RESULT_PICKER        , _T("Supervisor Group,Vision Worker Group,Operator Group") );


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
	// If the control ID coming through is from static text or something invalid then
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

