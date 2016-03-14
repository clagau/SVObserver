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

