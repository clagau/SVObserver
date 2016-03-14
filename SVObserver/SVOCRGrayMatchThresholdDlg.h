//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCRGrayMatchThresholdDlg
//* .File Name       : $Workfile:   SVOCRGrayMatchThresholdDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:59:36  $
//******************************************************************************

#ifndef SVOCRGRAYMATCHTHRESHOLDDLG_H
#define SVOCRGRAYMATCHTHRESHOLDDLG_H

class SVOCRGrayMatchThresholdDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(SVOCRGrayMatchThresholdDlg)

// Construction
public:
	SVOCRGrayMatchThresholdDlg();
	~SVOCRGrayMatchThresholdDlg();

// Dialog Data
	//{{AFX_DATA(SVOCRGrayCharMatchDlg)
	enum { IDD = IDD_OCR_GRAY_THRESHOLD_DIALOG };
	CSliderCtrl	m_SliderAcceptThreshold;
	CSliderCtrl	m_SliderRelativeThreshold;
	CSliderCtrl	m_SliderRejectThreshold;
	float	m_acceptThresh;
	float	m_rejectThresh;
	float	m_relThresh;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOCRGrayCharMatchDlg)
	public:
	//virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

public:
	void	UpdateOCRParameters ();

	//  Pointer to working OCRParameters structure of property sheet.
	//SVOCRAnalyzeResultClass	*pOCRAnalyzerResult;


protected:
	// Generated message map functions
	//{{AFX_MSG(SVOCRGrayCharMatchDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEditAcceptThreshold();
	afx_msg void OnChangeEditRejectThreshold();
	afx_msg void OnChangeEditRelativeThreshold();
	afx_msg void OnCustomdrawSliderAcceptThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderRejectThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawSliderRelativeThreshold(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif

