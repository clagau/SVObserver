//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
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
#include "SVProductId.h"
#pragma endregion Includes

class SVDriveInitDlg : public CDialog
{
public:
	SVDriveInitDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	//{{AFX_DATA(SVDriveInitDlg)
	enum { IDD = IDD_SVDRIVEINIT_DIALOG };
	CButton	m_cb1394Hub;
	CButton m_SingleCamera;
	CEdit	m_model_number;
	CComboBox	m_type;
	CEdit	m_cdkey5;
	CEdit	m_cdkey4;
	CEdit	m_cdkey3;
	CEdit	m_cdkey2;
	CEdit	m_cdkey1;
	CEdit	m_date;
	CEdit	m_serviced_by;
	CEdit	m_serial_number;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVDriveInitDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(SVDriveInitDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnChangeCdkey1();
	afx_msg void OnChangeCdkey2();
	afx_msg void OnChangeCdkey3();
	afx_msg void OnChangeCdkey4();
	afx_msg void OnChangeCdkey5();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void OnChangeCdkey(CEdit& rEdit);
	bool IsValidModelNumber() const;

	bool GetProductId() const;
	
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
	HRESULT AddMatroxBootIniMaxMem(size_t p_MILSize);
	HRESULT RemoveMatroxBootIniMaxMem();
	HRESULT AddCommandBeforeSVObserver(LPCTSTR p_strNewCommand, bool p_bRemove);

	SVProductId m_CDKey;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

