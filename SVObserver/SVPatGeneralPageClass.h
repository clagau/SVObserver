//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatGeneralPageClass
//* .File Name       : $Workfile:   SVPatGeneralPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:19:24  $
//******************************************************************************

#ifndef SVPATGENERALPAGECLASS_H
#define SVPATGENERALPAGECLASS_H

#include "SVMatroxLibrary/SVMatroxLibrary.h"

class SVPatternAnalyzerClass;
class SVPatAnalyzeSetupDlgSheet;

class SVPatGeneralPageClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVPatGeneralPageClass)

public:
	SVPatGeneralPageClass();
	virtual ~SVPatGeneralPageClass();

// Dialog Data
	//{{AFX_DATA(SVPatGeneralPageClass)
	enum { IDD = IDD_PAT_SETUP_GENERAL };
	CSliderCtrl	m_AngularAccuracySliderCtrl;
	CSliderCtrl	m_AngleToleranceSliderCtrl;
	CSliderCtrl	m_AngleDeltaPositiveSliderCtrl;
	CSliderCtrl	m_AngleDeltaNegativeSliderCtrl;
	CSliderCtrl	m_SearchAngleSliderCtrl;
	BOOL	m_bAngleSearch;
	double	m_dSearchAngle;
	double	m_dAngleDeltaNegative;
	double	m_dAngleDeltaPositive;
	double	m_dAngleTolerance;
	double	m_dAngularAccuracy;
	long	m_lAcceptance;
	long	m_lCertainty;
	int		m_nOccurances;
	BOOL	m_bAccuracy;
	//}}AFX_DATA
	SVImageOperationTypeEnum	m_dInterpolationMode;
	int		m_nPosAccuracyIndex;
	int		m_nSpeedIndex;

	SVPatternAnalyzerClass* m_pPatAnalyzer;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVPatGeneralPageClass)
	public:
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SVPatAnalyzeSetupDlgSheet	*m_pSheet;

	void SetControlTextLength();
	void EnableAngleControls(BOOL bEnable); 
	void EnableAngleAccuracyControls(BOOL bEnable); 
	void SetSliderPosition(UINT nId);
	BOOL ProcessOnKillfocus(UINT nId); 

	// Generated message map functions
	//{{AFX_MSG(SVPatGeneralPageClass)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnKillOccuranceNumber();
	afx_msg void OnKillCertainty();
	afx_msg void OnKillAcceptance();
	afx_msg void OnKillValue1();
	afx_msg void OnKillValue2();
	afx_msg void OnKillValue3();
	afx_msg void OnKillValue4();
	afx_msg void OnKillValue5();
	afx_msg void OnAngularSearch();
	afx_msg void OnAngAccuracy();
	afx_msg void OnInterpoleMode1();
	afx_msg void OnInterpoleMode2();
	afx_msg void OnInterpoleMode3();
	afx_msg void OnSelchangePosAccuracy();
	afx_msg void OnSelchangeSpeed();
	afx_msg void OnSetRangeClicked();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVPATGENERALPAGECLASS_H__CFE15012_F9AB_11D3_A89C_00106F021742__INCLUDED_)

