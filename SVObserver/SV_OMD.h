//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_OMD
//* .File Name       : $Workfile:   SV_OMD.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:23:08  $
//******************************************************************************

#if !defined(AFX_SV_OMD_H__6E7FD401_ECEA_11D2_846B_00403355EC6D__INCLUDED_)
#define AFX_SV_OMD_H__6E7FD401_ECEA_11D2_846B_00403355EC6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVTaskObjectInterfaceClass.h"

/////////////////////////////////////////////////////////////////////////////
// SVOCRMiscDlg dialog
class SVOCRMiscDlg : public CPropertyPage, public SVTaskObjectInterfaceClass
{
	DECLARE_DYNCREATE(SVOCRMiscDlg)

// Construction
public:
	SVOCRMiscDlg();
	~SVOCRMiscDlg();

	virtual HRESULT SetInspectionData();
// Dialog Data
	//{{AFX_DATA(SVOCRMiscDlg)
	enum { IDD = IDD_OCR_MISC_DIALOG };
	CEdit	m_fontFileNameEdit;
	CEdit	m_featureCntrl;
	CSliderCtrl	m_featuresSlider;
	CString	m_samplesEdit;
	int		m_featuresEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCRMiscDlg)
	public:
	//virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL	UpdateOCRParameters ();

//  samplesInt	0-5.
	int	m_samplesInt;
	
//  featuresInt 0-100.
	int m_featuresInt;

//  Pointer to working OCRParameters structure of property sheet.
	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;

protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCRMiscDlg)
	afx_msg void OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeFeatures();
	afx_msg void OnFontBrowseCmd();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_OMD_H__6E7FD401_ECEA_11D2_846B_00403355EC6D__INCLUDED_)

