//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBlobAnalyzerResultDlg
//* .File Name       : $Workfile:   SVBlobAnalyzerResultDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:00  $
//******************************************************************************

#pragma once

class SVBlobAnalyzerClass;

class SVBlobAnalyzerResultDlg : public CDialog
{
// Construction
public:
	SVBlobAnalyzerResultDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVBlobAnalyzerResultDlg)
	enum { IDD = IDD_BA_RESULT_DIALOG };
	CListBox	m_ResultList;
	//}}AFX_DATA

	SVBlobAnalyzerClass*      m_pBlobAnalyzer;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVBlobAnalyzerResultDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVBlobAnalyzerResultDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

