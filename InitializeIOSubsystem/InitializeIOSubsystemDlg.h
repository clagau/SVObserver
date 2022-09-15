//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "resource.h"
#include "SVIOLibrary\SVIODigitalLoadLibraryClass.h"
#include <vector>
#pragma endregion Includes

// InitializeIOSubsystemDlg dialog
class InitializeIOSubsystemDlg : public CDialog
{
// Construction
public:
	InitializeIOSubsystemDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~InitializeIOSubsystemDlg();
// Dialog Data
	enum { IDD = IDD_INITIALIZEIOSUBSYSTEM_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
// Implementation
protected:
	HICON m_hIcon {nullptr};

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

	bool m_bShutdown {false};
	bool m_bToggle {false};

protected:
	virtual void OnCancel() override;
	virtual void OnOK() override;
	std::vector<CString> m_strLog;
	time_t m_TimeElapse {0ULL};
	time_t m_StartTime{0ULL};
	long m_FailedCounts{0L};
	UINT_PTR m_lTimer{0ULL};

	bool m_bLptIOInitialized {false};
	long m_lGoodCounter {0L};
	long m_lNewBehaviorCounter {0L};
	long m_lProgressCounter {0L};
	long m_lVer {0L};
	CString m_strVer;
	CString m_FanSpeed;
	SVIODigitalLoadLibraryClass m_IOSystem;
	HMODULE m_dllHandle {nullptr};
	bool m_bOldBehavior {false};
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
