//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerSetupDlg
//* .File Name       : $Workfile:   SVSoftwareTriggerSetupDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 12:50:44  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "resource.h"
#pragma endregion Includes

class SVSoftwareTriggerSetupDlg : public CDialog
{
public:
	SVSoftwareTriggerSetupDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVSoftwareTriggerSetupDlg();

// Dialog Data
	enum { IDD = IDD_SOFTWARETRIGGER_SETUP_DLG };

protected:
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	long m_period1;
	long m_period2;
	long m_period3;
	long m_period4;
};

