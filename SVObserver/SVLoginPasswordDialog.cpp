//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLoginPasswordDialogClass
//* .File Name       : $Workfile:   SVLoginPasswordDialog.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:17:58  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"
#include "SVLoginPasswordDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVLoginPasswordDialogClass 


SVLoginPasswordDialogClass::SVLoginPasswordDialogClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVLoginPasswordDialogClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVLoginPasswordDialogClass)
	BUseAutoLogin = FALSE;
	//}}AFX_DATA_INIT
	bOldUseAutoLogin = BUseAutoLogin;
}


void SVLoginPasswordDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVLoginPasswordDialogClass)
	DDX_Control(pDX, IDC_PASSWORD_EDIT, PasswordEdit);
	DDX_Control(pDX, IDC_LOGIN_EDIT, LoginEdit);
	DDX_Check(pDX, IDC_AUTO_LOGIN_CHECK, BUseAutoLogin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVLoginPasswordDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVLoginPasswordDialogClass)
	ON_BN_CLICKED(IDC_AUTO_LOGIN_CHECK, OnAutoLoginCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVLoginPasswordDialogClass 

void SVLoginPasswordDialogClass::OnCancel() 
{

	CDialog::OnCancel();
}

void SVLoginPasswordDialogClass::OnOK() 
{
	LoginEdit.GetWindowText( StrLogin );
	PasswordEdit.GetWindowText( StrPassword );
	TheSVObserverApp.WriteProfileInt( _T( "Settings" ), _T( "Use Auto Login" ), BUseAutoLogin );
	if( BUseAutoLogin )
	{
		// Write login and password out to registry...
		TheSVObserverApp.WriteProfileString( _T( "Settings" ), _T( "AutoLogin" ), StrLogin );
		TheSVObserverApp.WriteProfileString( _T( "Settings" ), _T( "AutoPassword" ), StrPassword );
	}
	else
	{
		// Flush login and password out of registry...
		TheSVObserverApp.WriteProfileString( _T( "Settings" ), _T( "AutoLogin" ), _T( "" ) );
		TheSVObserverApp.WriteProfileString( _T( "Settings" ), _T( "AutoPassword" ), _T( "" ) );
	}

	CDialog::OnOK();
}

BOOL SVLoginPasswordDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Read out registry info...
	bOldUseAutoLogin = BUseAutoLogin = ( BOOL ) TheSVObserverApp.GetProfileInt( _T( "Settings" ), _T( "Use Auto Login" ), 0 );

	LoginEdit.SetWindowText( StrDefaultLogin );

	UpdateData( FALSE );	// Send data to dialog...
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVLoginPasswordDialogClass::OnAutoLoginCheck() 
{
	BUseAutoLogin = ! BUseAutoLogin;

	UpdateData( FALSE );
}
















/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVNewLoginChangePasswordDialogClass 


SVNewLoginChangePasswordDialogClass::SVNewLoginChangePasswordDialogClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVNewLoginChangePasswordDialogClass::IDD, pParent)
{
	BEnterNewLogin = FALSE; // Default: No new login, only password changing!
	//{{AFX_DATA_INIT(SVNewLoginChangePasswordDialogClass)
		// HINWEIS: Der Klassen-Assistent fügt hier Elementinitialisierung ein
	//}}AFX_DATA_INIT
}


void SVNewLoginChangePasswordDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVNewLoginChangePasswordDialogClass)
	DDX_Control(pDX, IDC_REPEAT_PASSWORD_EDIT, RepeatPasswordEdit);
	DDX_Control(pDX, IDC_PASSWORD_EDIT, PasswordEdit);
	DDX_Control(pDX, IDC_LOGIN_EDIT, LoginEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SVNewLoginChangePasswordDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVNewLoginChangePasswordDialogClass)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVNewLoginChangePasswordDialogClass 

void SVNewLoginChangePasswordDialogClass::OnCancel() 
{
	CDialog::OnCancel();
}

void SVNewLoginChangePasswordDialogClass::OnOK() 
{
	LoginEdit.GetWindowText( StrLogin );
	PasswordEdit.GetWindowText( StrPassword );
	RepeatPasswordEdit.GetWindowText( StrRepeatPassword );
	
	if( StrLogin.IsEmpty() )
	{
		AfxMessageBox( IDS_USER_WARNING_NO_LOGIN_ENTERED );
		return;
	}

	if( StrPassword != StrRepeatPassword )
	{
		AfxMessageBox( IDS_USER_WARNING_PASSWORD_CHECK_FAILED );
		return;
	}

	CDialog::OnOK();
}

BOOL SVNewLoginChangePasswordDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if( StrLogin.IsEmpty() && ! BEnterNewLogin )
		SendMessage( WM_CLOSE );

	LoginEdit.SetWindowText( StrLogin );
	if(	! BEnterNewLogin )
		LoginEdit.EnableWindow( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVLoginPasswordDialog.cpp_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:17:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   27 Jan 2011 11:28:40   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  712
   SCR Title:  Fix issues with black images when using command interface (SIAC)
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Updated source code to fix issues with Matrox Buffer Management.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   22 Apr 2003 10:57:48   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/
