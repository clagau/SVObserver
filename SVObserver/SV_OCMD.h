//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_OCMD
//* .File Name       : $Workfile:   SV_OCMD.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:18:58  $
//******************************************************************************

#if !defined(AFX_SV_OCMD_H__B779623E_F037_11D2_A778_00016F0207D9__INCLUDED_)
#define AFX_SV_OCMD_H__B779623E_F037_11D2_A778_00016F0207D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// SVOCRCharMatchDlg dialog

class SVOCRCharMatchDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(SVOCRCharMatchDlg)

// Construction
public:
	SVOCRCharMatchDlg();
	~SVOCRCharMatchDlg();

// Dialog Data
	//{{AFX_DATA(SVOCRCharMatchDlg)
	enum { IDD = IDD_OCR_CHARMATCH_DIALOG };
	CEdit	m_noMatchCharLabel;
	CEdit	m_thresholdCntrl;
	CSliderCtrl	m_thresholdSlider;
	int		m_thresholdEdit;
	BOOL	m_ignoreNoMatch;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCRCharMatchDlg)
	public:
	//virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

public:
	BOOL	UpdateOCRParameters ();

	//  Pointer to working OCRParameters structure of property sheet.
	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCRCharMatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeThreshold();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_OCMD_H__B779623E_F037_11D2_A778_00016F0207D9__INCLUDED_)

