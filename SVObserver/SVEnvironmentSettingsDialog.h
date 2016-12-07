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

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#pragma endregion Includes

class SVEnvironmentSettingsDialogClass : public CDialog
{
	//{{AFX_VIRTUAL(SVEnvironmentSettingsDialogClass)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

	protected:
	//{{AFX_MSG(SVEnvironmentSettingsDialogClass)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	public:
	//{{AFX_DATA(SVEnvironmentSettingsDialogClass)
	enum { IDD = IDD_ENVIRONMENT_SETTINGS_DIALOG };
	BOOL	StartLastAutomatically;
	//}}AFX_DATA

public:
	SVEnvironmentSettingsDialogClass(CWnd* pParent = nullptr);   // Standardkonstruktor
	virtual ~SVEnvironmentSettingsDialogClass();
};

