// ******************************************************************************
// * COPYRIGHT (c) 2006 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVImageTestDlg.h
// * .File Name       : $Workfile:   SVImageTestDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:10:30  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "resource.h"
#include "SVCameraPage.h"
#pragma endregion Includes

class SVTestAcquisitionSubsystem;

class CSVImageTestDlg : public CDialog
{
	public:
	//{{AFX_DATA(CSVImageTestDlg)
	enum { IDD = IDD_SVIMAGETEST_DIALOG };
	CListBox	m_clbCameras;
	CString	m_modelNo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVImageTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

	protected:
	// Generated message map functions
	//{{AFX_MSG(CSVImageTestDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRefresh();
	afx_msg void OnSelchangeList1();
	afx_msg void OnUpdateModelNumber();
	virtual void OnCancel() override;
	afx_msg void OnStartAllCameras();
	afx_msg void OnStopAllCameras();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	CSVImageTestDlg(CWnd* pParent = nullptr);

	virtual ~CSVImageTestDlg();

	SVTestAcquisitionSubsystem* m_pSubsystem;

protected:
	void StartAllCameras();
	void StopAllCameras();
	void StopDevice();
	void ResetCameraList();

	HICON m_hIcon;

	bool m_bStarted;
	CPropertySheet m_CameraSheet;
	SVCameraPage m_Camera[ 4 ];

};


