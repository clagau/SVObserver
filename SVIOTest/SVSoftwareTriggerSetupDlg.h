//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerSetupDlg
//* .File Name       : $Workfile:   SVSoftwareTriggerSetupDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 12:50:44  $
//******************************************************************************
#pragma once

// SVSoftwareTriggerSetupDlg dialog

class SVSoftwareTriggerSetupDlg : public CDialog
{
	DECLARE_DYNAMIC(SVSoftwareTriggerSetupDlg)

public:
	SVSoftwareTriggerSetupDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~SVSoftwareTriggerSetupDlg();

// Dialog Data
	enum { IDD = IDD_SOFTWARETRIGGER_SETUP_DLG };

protected:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	long m_period1;
	long m_period2;
	long m_period3;
	long m_period4;
};

