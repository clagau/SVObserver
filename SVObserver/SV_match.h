//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_Match.h
//* .File Name       : $Workfile:   SV_match.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:44  $
//******************************************************************************

#if !defined(AFX_SV_MATCH_H__0CA068C4_EDE1_11D2_846C_00403355EC6D__INCLUDED_)
#define AFX_SV_MATCH_H__0CA068C4_EDE1_11D2_846C_00403355EC6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVOCRMatchDlg dialog

class SVOCRMatchDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVOCRMatchDlg)

// Construction
public:
	SVOCRMatchDlg();
	~SVOCRMatchDlg();

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVOCRMatchDlg)
	enum { IDD = IDD_OCR_MATCH_DIALOG };
	CScrollBar	m_scrollPixelsForSpace;
	CEdit	m_matchString1Edit;
	CEdit	m_matchFilenameEdit;
	CButton	m_browseButton;
	int		m_nRadioSort;
	CString	m_csPixelsForSpace;
	int		m_nRadio1;
	//}}AFX_DATA

	int nPixelsForSpace;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCRMatchDlg)
	public:
	//virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void UpdatePixelsForSpace();

// Implementation
public:

	BOOL	UpdateOCRParameters ();

	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCRMatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	afx_msg void OnMatchStringFileBrowseCmd();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_MATCH_H__0CA068C4_EDE1_11D2_846C_00403355EC6D__INCLUDED_)

