//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProgressDialog
//* .File Name       : $Workfile:   SVProgressDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:37:48  $
//******************************************************************************

#if !defined(AFX_SVPROGRESSDIALOG_H__99F49D44_91F0_11D3_A83A_00106F000C7D__INCLUDED_)
#define AFX_SVPROGRESSDIALOG_H__99F49D44_91F0_11D3_A83A_00106F000C7D__INCLUDED_

#include "SVIProgress.h"

class SVProgressDialog : public CDialog
{
public:
	SVIProgress* m_pTask;

protected:
	int m_TotalSize;
	int m_AmountProcessed;
	CString m_Text;
	CString m_Title;

public:
	SVProgressDialog(LPCTSTR rTitle, CWnd* pParent = NULL); // standard constructor

// Dialog Data
	//{{AFX_DATA(SVProgressDialog)
	enum { IDD = IDD_PROGRESS_DIALOG };
	CStatic	m_staticTextCtrl;
	CProgressCtrl	m_progressCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVProgressDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVProgressDialog)
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateProgress( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndProgressDialog( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

