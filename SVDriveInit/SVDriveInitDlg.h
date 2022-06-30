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

// Dialog Data
	//{{AFX_DATA(SVDriveInitDlg)
	enum { IDD = IDD_SVDRIVEINIT_DIALOG };
	CButton m_SingleCamera;
	CEdit	m_model_number;
	CComboBox	m_type;
	CEdit	m_cdkey[cCdKeyBlockNr];
	CEdit	m_date;
	CEdit	m_serviced_by;
	CEdit	m_serial_number;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDriveInitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(SVDriveInitDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK() override;
	virtual void OnCancel() override;
	afx_msg void OnChangeCdkey(UINT);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool IsValidModelNumber() const;

	bool GetOEMInfo();
	bool GetSysPrepInfo();

	bool UpdateRegistryInfo();
	bool UpdateOEMInfo();
	bool UpdateSVIMInfo();
	bool UpdateSysPrepInfo();

	HRESULT BackupBootIni();

	HRESULT UpdateIOInterfaceDLL(LPCTSTR p_szIOBoard);

	HRESULT UpdateMatrox(LPCTSTR p_szDigitizer);
	HRESULT UpdateMatroxRegistryMaxMem(size_t& p_rMaxSize, size_t p_MILSize);
	HRESULT RemoveMatroxBootIniMaxMem();
	HRESULT AddCommandBeforeSVObserver(LPCTSTR p_strNewCommand, bool p_bRemove);

	SVProductId m_CDKey;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

