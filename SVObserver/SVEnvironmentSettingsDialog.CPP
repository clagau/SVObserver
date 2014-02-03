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
#include "SVUserMessage.h"

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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVEnvironmentSettingsDialog.CPP_v  $
 * 
 *    Rev 1.2   31 Jan 2014 17:16:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  884
 * SCR Title:  Update Source Code Files to Follow New Programming Standards and Guidelines
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed to follow guidelines more closely.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 May 2013 20:06:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
      Rev 3.8   16 Apr 2013 14:24:14   jspila
   Project:  SVObserver
   Change Request (SCR) nbr:  822
   SCR Title:  Remove DDE Input and Output functionality from SVObserver
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Removed DDE functionality from the source code.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
 *    Rev 1.0   23 Apr 2013 10:27:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.7   02 Mar 2007 06:24:16   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  595
   SCR Title:  Fix issues with Utility Menu when switching products
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     added show update firmware checkbox
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.6   18 Aug 2003 15:45:26   Joe
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     Updated OnOK method to use updated I/O Configuration Interface method.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.5   08 Jul 2003 12:00:08   ebeyeler
   Project:  SVObserver
   Change Request (SCR) nbr:  322
   SCR Title:  Add Additional Digital IO Resources to SVObserver
   Checked in by:  eBeyeler;  Eric Beyeler
   Change Description:  
     changed DDE handling for new interface to DLL
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.4   25 Jun 2003 09:11:08   ryoho
   Project:  SVObserver
   Change Request (SCR) nbr:  371
   SCR Title:  Remove DOS Focus COM support
   Checked in by:  rYoho;  Rob Yoho
   Change Description:  
     removed the CommPort support for DOS Focus
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.3   22 Apr 2003 09:25:16   rschock
   Project:  SVObserver
   Change Request (SCR) nbr:  346
   SCR Title:  Update SVObserver to Version 4.21 Release
   Checked in by:  Joe;  Joe Spila
   Change Description:  
     Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.2   05 May 2000 14:48:44   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  119
   SCR Title:  SVFocusNT Support
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Revised OnInitDialog to check if remote commands are
   enabled.
   Revised OnEnableRemoteCommands handler.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.1   05 May 2000 14:10:36   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  119
   SCR Title:  SVFocusNT Support
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     Added Enable Remote Command Checkboxhandler.
   Added Setup SVIM Server handler.
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 3.0   23 Nov 1999 11:29:46   mike
   Project:  SVObserver
   Change Request (SCR) nbr:  61
   SCR Title:  Update PVCS versioning to version 3.0.
   Checked in by:  Mike;  Mike McCarl
   Change Description:  
     
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.3   Aug 23 1999 10:01:12   sjones
   Project:  SVObserver
   Change Request (SCR) nbr:  22
   SCR Title:  Port Multiple Position Queue code from program version 2.4.2
   Checked in by:  Steve Jones
   Change Description:  
     Revised for new PPQ Architecture (removed old PPQ code).
   
   /////////////////////////////////////////////////////////////////////////////////////
   
      Rev 1.1   01 Mar 1999 20:24:20   robert
   Project:  SVObserver
   Change Request (SCR) nbr:  7
   SCR Title:  Clarify the include list links, regard. have diff. versions of same project
   Checked in by:  Robert Massinger
   Change Description:  
     Changed include directive for SVCommRC.h.
   
   /////////////////////////////////////////////////////////////////////////////////////
*/