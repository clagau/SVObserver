//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : InitializeIOSubsystemDlg.h
//* .File Name       : $Workfile:   InitializeIOSubsystemDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Sep 2014 11:40:00  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "afxcmn.h"
#include "SVIOLibrary\SVIODigitalLoadLibraryClass.h"
#include <vector>
#pragma endregion Includes

// CInitializeIOSubsystemDlg dialog
class CInitializeIOSubsystemDlg : public CDialog
{
// Construction
public:
	CInitializeIOSubsystemDlg(CWnd* pParent = NULL);	// standard constructor
	~CInitializeIOSubsystemDlg();
// Dialog Data
	enum { IDD = IDD_INITIALIZEIOSUBSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CProgressCtrl m_Progress;
	afx_msg void OnDestroy();

	bool m_bShutdown;
	bool m_bToggle;

protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;
	std::vector<CString> m_strLog;
	time_t m_TimeElapse;
	time_t m_StartTime;
	long m_FailedCounts;
	UINT_PTR m_lTimer;

	bool m_bLptIOInitialized;
	long m_lGoodCounter;
	long m_lNewBehaviorCounter;
	long m_lProgressCounter;
	long m_lVer;
	CString m_strVer;
	CString m_FanSpeed;
	SVIODigitalLoadLibraryClass m_IOSystem;
	bool m_bOldBehavior;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
