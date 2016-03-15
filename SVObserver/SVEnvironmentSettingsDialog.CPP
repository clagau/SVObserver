//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnvironmentSettingsDialog
//* .File Name       : $Workfile:   SVEnvironmentSettingsDialog.CPP  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   31 Jan 2014 17:16:28  $
//******************************************************************************

#include "stdafx.h"
#include "SVObserver.h"

#include "SVEnvironmentSettingsDialog.h"
#include "ObjectInterfaces/SVUserMessage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Dialogfeld SVEnvironmentSettingsDialogClass 


SVEnvironmentSettingsDialogClass::SVEnvironmentSettingsDialogClass(CWnd* pParent /*=NULL*/)
	: CDialog(SVEnvironmentSettingsDialogClass::IDD, pParent)
{
	//{{AFX_DATA_INIT(SVEnvironmentSettingsDialogClass)
	StartLastAutomatically = FALSE;
	m_enableRemoteCommands = FALSE;
	//}}AFX_DATA_INIT
}

void SVEnvironmentSettingsDialogClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SVEnvironmentSettingsDialogClass)
	DDX_Control(pDX, IDC_CHK_UPDATE_FIRMWARE, m_ctlUpdateFirmware);
	DDX_Check(pDX, IDC_STARTLASTAUTOMATICALLY_CHECK, StartLastAutomatically);
	DDX_Check(pDX, IDC_ENABLE_REMOTE_COMMANDS, m_enableRemoteCommands);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(SVEnvironmentSettingsDialogClass, CDialog)
	//{{AFX_MSG_MAP(SVEnvironmentSettingsDialogClass)
	ON_BN_CLICKED(IDC_ENABLE_REMOTE_COMMANDS, OnEnableRemoteCommands)
	ON_BN_CLICKED(IDC_SETUP_SVIM_SERVER, OnSetupSvimServer)
	ON_BN_CLICKED(IDC_CHK_UPDATE_FIRMWARE, OnChkUpdateFirmware)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen für Nachrichten SVEnvironmentSettingsDialogClass 

BOOL SVEnvironmentSettingsDialogClass::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

	m_enableRemoteCommands = pApp->IsSVIMServerEnabled();

	if ( pApp->m_csFirmwareCommand.IsEmpty() )
	{
		m_ctlUpdateFirmware.EnableWindow(FALSE);
		m_ctlUpdateFirmware.ShowWindow(SW_HIDE);
	}
	else
	{
		if ( m_bUpdateFirmwareCheck )
		{
			m_ctlUpdateFirmware.SetCheck(1);
		}
		else
		{
			m_ctlUpdateFirmware.SetCheck(0);
		}
	}

	UpdateRemoteCommandSetupButton();

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX-Eigenschaftenseiten sollten FALSE zurückgeben
}

void SVEnvironmentSettingsDialogClass::OnOK() 
{
	UpdateData( TRUE );

	CDialog::OnOK();
}

void SVEnvironmentSettingsDialogClass::OnEnableRemoteCommands() 
{
	UpdateData( TRUE );	// Get Data...

	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

	// if Checked
	if( m_enableRemoteCommands )
	{
		// Init SVIMServer
		if( !pApp->IsSVIMServerEnabled() )
		{
			if( !pApp->InitSVIMServer() )
			{
				// Unable to Init SVIM server
				m_enableRemoteCommands = FALSE;
				UpdateData( FALSE );	// Set Data...
			}
		}
	}
	else
	{
		// Destroy SVIMServer
		pApp->DestroySVIMServer();
	}
	// Diasble the setup button
	UpdateRemoteCommandSetupButton();

	// Update the setting
	pApp->WriteProfileInt( _T( "Settings" ), _T( "Enable Remote Commands" ), (DWORD)m_enableRemoteCommands );
}

void SVEnvironmentSettingsDialogClass::UpdateRemoteCommandSetupButton()
{
	CWnd* pItem = GetDlgItem( IDC_SETUP_SVIM_SERVER );
	if( pItem )
		pItem->EnableWindow( m_enableRemoteCommands );
}

void SVEnvironmentSettingsDialogClass::OnSetupSvimServer() 
{
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();
	pApp->SetupSVIMServer();
}

void SVEnvironmentSettingsDialogClass::OnChkUpdateFirmware() 
{
	SVObserverApp* pApp = (SVObserverApp *)AfxGetApp();

	//get check state
	BOOL bChecked = FALSE;
	if ( m_ctlUpdateFirmware.GetCheck() == BST_CHECKED )
	{
		bChecked = TRUE;
	}
	pApp->setShowUpdateFirmwareInMenu( bChecked );
}

