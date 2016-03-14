//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayMatchDlg
//* .File Name       : $Workfile:   SVOCRGrayMatchDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:59:16  $
//******************************************************************************

#if !defined(AFX_SV_OCR_GRAY_MATCH_DLG_H__0CA068C4_EDE1_11D2_846C_00403355EC6D__INCLUDED_)
#define AFX_SV_OCR_GRAY_MATCH_DLG_H__0CA068C4_EDE1_11D2_846C_00403355EC6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayMatchDlg dialog

class SVOCRGrayMatchDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVOCRGrayMatchDlg)

// Construction
public:
	SVOCRGrayMatchDlg();
	~SVOCRGrayMatchDlg();

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVOCRGrayMatchDlg)
	enum { IDD = IDD_OCR_GRAY_MATCH_DIALOG };
	CStatic	m_wndPixelsForSpace;
	CSpinButtonCtrl	m_spinPixelsForSpace;
	CButton	m_buttonBrowse;
	CEdit	m_editMatchString;
	CEdit	m_editMatchFilename;
	CString	m_MatchFilename;
	CString	m_MatchString;
	int		m_useMatchFile;
	int		m_nPixelsForSpace;
	int		m_nRadioSort;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCRGrayMatchDlg)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	void	UpdateOCRParameters ();

	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCRGrayMatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontBrowseCmd();
	afx_msg void OnUseMatchString();
	afx_msg void OnUseMatchFile();
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_OCR_GRAY_MATCH_DLG_H__0CA068C4_EDE1_11D2_846C_00403355EC6D__INCLUDED_)

