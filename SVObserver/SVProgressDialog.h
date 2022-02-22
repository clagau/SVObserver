//******************************************************************************
//* COPYRIGHT (c) 2003 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVProgressDialog
//* .File Name       : $Workfile:   SVProgressDialog.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 14:37:48  $
//******************************************************************************

#pragma once

#pragma region Inlcudes
class SVIProgress;
#pragma endregion Inlcudes

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
	explicit SVProgressDialog(LPCTSTR rTitle, CWnd* pParent = nullptr); // standard constructor
	virtual ~SVProgressDialog();

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
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

	
// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVProgressDialog)
	virtual void OnCancel() override;
	virtual void OnOK() override;
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	afx_msg LRESULT OnUpdateProgress( WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndProgressDialog( WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
