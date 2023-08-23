//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDriveInitDlg
//* .File Name       : $Workfile:   SVDriveInitDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Sep 2014 15:03:26  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"
#include "SVProductId.h"
#pragma endregion Includes

constexpr int cCdKeyBlockNr = 5;

class SVDriveInitDlg : public CDialog
{
public:
	SVDriveInitDlg(CWnd* pParent = nullptr);	// standard constructor

	enum { IDD = IDD_SVDRIVEINIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support

protected:
	HICON m_hIcon {NULL};

	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnChangeCdkey(UINT);
	DECLARE_MESSAGE_MAP()

private:
	bool IsModelNumberValidAndAreBoardNamesPresent() const;

	bool GetOEMInfo();
	bool GetSysPrepInfo();

	bool UpdateOEMInfo();
	bool UpdateSysPrepInfo();

	CEdit	m_modelNumber {};
	CEdit	m_cdKey[cCdKeyBlockNr] {};
	CEdit	m_serialNumber {};

	SVProductId m_CDKey {};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

