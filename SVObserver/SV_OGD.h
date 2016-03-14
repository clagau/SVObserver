//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV_OGD
//* .File Name       : $Workfile:   SV_OGD.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:17:02  $
//******************************************************************************

#if !defined(AFX_SV_OGD_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_)
#define AFX_SV_OGD_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SVMFCControls\SVNumericEdit.h"

/////////////////////////////////////////////////////////////////////////////
// SVOCRGeometryDlg dialog

class SVOCRGeometryDlg : public CPropertyPage
{
// Construction
public:
	SVOCRGeometryDlg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVOCRGeometryDlg)
	enum { IDD = IDD_OCR_GEOM_DIALOG };
	CEdit	m_editMaxBlobPixels;
	CEdit	m_editMinBlobPixels;
	CEdit	m_sensitivityEdit;
	CComboBox	m_rotationModeCombo;
	CEdit	m_minWidthEdit;
	SvMc::CSVNumericEdit	m_minScaleEdit;
	CEdit	m_minHeightEdit;
	CEdit	m_minAngleEdit;
	CEdit	m_maxWidthEdit;
	CEdit	m_maxScaleEdit;
	CEdit	m_maxHeightEdit;
	CEdit	m_maxAngleEdit;
	BOOL	m_scaleInvarientButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOCRGeometryDlg)
	public:
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
	public:
	BOOL	UpdateOCRParameters ();

//  Pointer to working OCRParameters structure of property sheet.
	CString				m_rotaionModeComboContents [3];

	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;

protected:

	// Generated message map functions
	//{{AFX_MSG(SVOCRGeometryDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	float m_fMinScaleValue;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_OGD_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_)

