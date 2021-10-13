//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// \file SMViewDlg.h 
/// All Rights Reserved 
//*****************************************************************************

/// These files contain your CShareViewDlg class.  This class defines
//the behavior of your application's main dialog.  The dialog's template is
//in ShareView.rc, which can be edited in Microsoft Visual C++. 
//******************************************************************************

#pragma once

#include "SVSharedMemoryLibrary\SharedMemReader.h"
#include "resource.h"

class MonitorListMap;

//! CShareViewDlg dialog application's main dialog
class SMViewDlg : public CDialogEx
{
// Construction
public:
	SMViewDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SMView_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void UpdateControls(bool ready);


	enum ECOL {eName=0, ePPQ, eIsActive, eRejectDepth, eProductDepth,eProductFilter,eImageCount,ePcount};
	static LPCTSTR ColHeader[ePcount+1];


	// Implementation
protected:

	SvSml::SharedMemReader m_MemReader;
	HICON m_hIcon;

	bool	 PostRefresh(DWORD par);
	LRESULT  OnRefresh(WPARAM wParam, LPARAM lParam);
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	friend class MonitorListDlg;
	CStatic m_ReadyValueCtrl;
	CListCtrl m_MonListsCtrl;
	void DisplayProduct(bool isreject);
	afx_msg void OnBnClickedButtonShow();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButGetproduct();
	CEdit m_EditTrigger;
	afx_msg void OnBnClickedButGetfailstatus();
	afx_msg void OnBnClickedButReject();
};
