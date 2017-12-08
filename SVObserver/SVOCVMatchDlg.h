//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVMatchDlg
//* .File Name       : $Workfile:   SVOCVMatchDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:03:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "InspectionEngine/SVTaskObjectValueInterface.h"
#pragma endregion Includes

/////////////////////////////////////////////////////////////////////////////
// SVOCVMatchDlg dialog

class SVOCVMatchDlg : public CPropertyPage, public SVTaskObjectValueInterface
{
	DECLARE_DYNCREATE(SVOCVMatchDlg)

// Construction
public:
	SVOCVMatchDlg();
	virtual ~SVOCVMatchDlg();

	HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVOCVMatchDlg)
	enum { IDD = IDD_OCV_MATCH_DIALOG };
	CButton	m_buttonBrowse;
	CEdit	m_editMatchString;
	CEdit	m_editMatchFilename;
	CString	m_MatchFilename;
	CString	m_MatchString;
	int		m_useMatchFile;
	int		m_nRadioOperation;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCVMatchDlg)
	public:
	virtual BOOL OnKillActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	void UpdateOCVParameters();

	//SVOCVAnalyzeResultClass	*pOCVAnalyzerResult;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCVMatchDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnFontBrowseCmd();
	afx_msg void OnUseMatchString();
	afx_msg void OnUseMatchFile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
