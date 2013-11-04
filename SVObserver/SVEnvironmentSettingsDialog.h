//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnvironmentSettingsDialog
//* .File Name       : $Workfile:   SVEnvironmentSettingsDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   06 May 2013 20:06:06  $
//******************************************************************************

#ifndef SVENVIRONMENTSETTINGSDIALOG_H
#define SVENVIRONMENTSETTINGSDIALOG_H

class SVEnvironmentSettingsDialogClass : public CDialog
{
	//{{AFX_VIRTUAL(SVEnvironmentSettingsDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

	protected:
	//{{AFX_MSG(SVEnvironmentSettingsDialogClass)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnEnableRemoteCommands();
	afx_msg void OnSetupSvimServer();
	afx_msg void OnChkUpdateFirmware();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	public:
	//{{AFX_DATA(SVEnvironmentSettingsDialogClass)
	enum { IDD = IDD_ENVIRONMENT_SETTINGS_DIALOG };
	CButton	m_ctlUpdateFirmware;
	BOOL	StartLastAutomatically;
	BOOL	m_enableRemoteCommands;
	//}}AFX_DATA

public:
	SVEnvironmentSettingsDialogClass(CWnd* pParent = NULL);   // Standardkonstruktor

	BOOL m_bUpdateFirmwareCheck;

protected:
	void UpdateRemoteCommandSetupButton();

};

#endif	// SVENVIRONMENTSETTINGSDIALOG_H

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVEnvironmentSettingsDialog.h_v  $
 * 
 *    Rev 1.1   06 May 2013 20:06:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 010.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.5   16 Apr 2013 14:24:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  822
 * SCR Title:  Remove DDE Input and Output functionality from SVObserver
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Removed DDE functionality from the source code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 10:27:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.4   02 Mar 2007 06:24:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  595
 * SCR Title:  Fix issues with Utility Menu when switching products
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added show update firmware checkbox
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.3   25 Jun 2003 09:11:08   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  371
 * SCR Title:  Remove DOS Focus COM support
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   removed the CommPort support for DOS Focus
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   22 Apr 2003 09:25:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   05 May 2000 14:10:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  119
 * SCR Title:  SVFocusNT Support
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   Added Enable Remote Command Checkboxhandler.
 * Added Setup SVIM Server handler.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   23 Nov 1999 11:29:48   mike
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  Mike;  Mike McCarl
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   Aug 23 1999 10:01:10   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  22
 * SCR Title:  Port Multiple Position Queue code from program version 2.4.2
 * Checked in by:  Steve Jones
 * Change Description:  
 *   Revised for new PPQ Architecture (removed old PPQ code).
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/