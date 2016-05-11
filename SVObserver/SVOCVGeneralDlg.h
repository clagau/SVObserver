//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCVGeneralDlg
//* .File Name       : $Workfile:   SVOCVGeneralDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:03:10  $
//******************************************************************************
#pragma once

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVOCVGeneralDlg dialog
class SVOCVGeneralDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVOCVGeneralDlg)

// Construction
public:
	SVOCVGeneralDlg();
	virtual ~SVOCVGeneralDlg();

	virtual HRESULT SetInspectionData();

// Dialog Data
	//{{AFX_DATA(SVOCVGeneralDlg)
	enum { IDD = IDD_OCV_GENERAL_DIALOG };
	CString	m_fontFilename;
	CString	m_constraintsFilename;
	CString	m_controlsFilename;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCVGeneralDlg)
	public:
	//virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void UpdateOCVParameters();

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCVGeneralDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontBrowseCmd();
	afx_msg void OnConstraintsBrowseCmd();
	afx_msg void OnControlsBrowseCmd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
