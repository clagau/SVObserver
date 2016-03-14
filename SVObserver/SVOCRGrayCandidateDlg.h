//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayCandidateDlg
//* .File Name       : $Workfile:   SVOCRGrayCandidateDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:56:14  $
//******************************************************************************

#if !defined(AFX_SV_OCR_GRAY_GEOMETRY_DLG_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_)
#define AFX_SV_OCR_GRAY_GEOMETRY_DLG_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// SVOCRGrayCandidateDlg dialog

class SVOCRGrayCandidateDlg : public CPropertyPage
{
// Construction
public:
	SVOCRGrayCandidateDlg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(SVOCRGrayCandidateDlg)
	enum { IDD = IDD_OCR_GRAY_CANDIDATES_DIALOG };
	CEdit	m_editBuddy2;
	CEdit	m_editBuddy1;
	CSliderCtrl	m_sliderRelThreshold;
	CSliderCtrl	m_sliderThreshold;
	CComboBox	m_cbScale;
	int		m_maxCandidates;
	int		m_candsInVicinity;
	float	m_candidateThresh;
	float	m_candidateRelThresh;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVOCRGrayCandidateDlg)
	public:
	//virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
	public:
	void	UpdateOCRParameters ();

//  Pointer to working OCRParameters structure of property sheet.
	CString				m_rotaionModeComboContents [3];

	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;

protected:

	// Generated message map functions
	//{{AFX_MSG(SVOCRGrayCandidateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditCandidateRelthresh();
	afx_msg void OnChangeEditCandidateThresh();
	afx_msg void OnCustomdrawSliderCandidateRelthresh(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderCandidateThresh(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SV_OCR_GRAY_GEOMETRY_DLG_H__CF24C2DC_EEA8_11D2_A774_00016F0207D9__INCLUDED_)

