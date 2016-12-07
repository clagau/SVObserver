//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPatAdvancedPageClass
//* .File Name       : $Workfile:   SVPatAdvancedPageClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:18:40  $
//******************************************************************************

#pragma once

class SVToolClass;
class SVPatternAnalyzerClass;
class SVPatAnalyzeSetupDlgSheet;

/////////////////////////////////////////////////////////////////////////////
// SVPatAdvancedPageClass dialog

class SVPatAdvancedPageClass : public CPropertyPage
{
	DECLARE_DYNCREATE(SVPatAdvancedPageClass)

// Construction
public:
	SVPatAdvancedPageClass();
	virtual ~SVPatAdvancedPageClass();

// Dialog Data
	//{{AFX_DATA(SVPatAdvancedPageClass)
	enum { IDD = IDD_PAT_SETUP_ADVANCED };
	CComboBox m_FastFindComboBox;
	CComboBox m_ModelStepComboBox;
	CComboBox m_BeginningResolutionLevelComboBox;
	CComboBox m_FinalResolutionLevelComboBox;
	CButton m_AdditionalCandidatesAutoCheckBox;
	CButton m_CandidateSpacingXMinAutoCheckBox;
	CButton m_CandidateSpacingYMinAutoCheckBox;
	CButton m_PreliminaryAcceptanceThresholdAutoCheckBox;
	CSliderCtrl	m_PreliminaryAcceptanceThresholdSliderCtrl;
	CEdit m_AcceptanceThresholdEditCtrl;
	CEdit m_AdditionalCandidatesEditCtrl;
	CEdit m_CandidateSpacingXMinEditCtrl;
	CEdit m_CandidateSpacingYMinEditCtrl;
	
	// Editable Fields
	long m_lAdditionalCandidates;				// 1 – 100, or auto (M_DEFAULT)
	double m_dCandidateSpacingXMin;				// 1.0 – 100.0, or auto (M_DEFAULT)
	double m_dCandidateSpacingYMin;				// 1.0 – 100.0, or auto (M_DEFAULT
	double m_dPreliminaryAcceptanceThreshold;	// 1.0 – 100.0, or auto (M_DEFAULT)

	// Editable Fields - Display representation
	CString m_AdditionalCandidatesStr;
	CString m_CandidateSpacingXMinStr;
	CString m_CandidateSpacingYMinStr;
	CString m_PreliminaryAcceptanceThresholdStr;

	// Combobox dropdownlist selections...
	long m_lFastFind;						// M_ENABLE | M_DISABLE | M_DEFAULT (auto)
	long m_lModelStep;						// 1, 2, or Auto (M_DEFAULT)
	long m_lBeginningResolutionLevel;		// 0 - 7, or auto (M_DEFAULT)
	long m_lFinalResolutionLevel;			// 0 - 7, or auto (M_DEFAULT)
	
	//}}AFX_DATA
	
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVPatAdvancedPageClass)
	public:
	virtual void OnCancel() override;
	virtual void OnOK() override;
	virtual BOOL OnKillActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	SVPatAnalyzeSetupDlgSheet* m_pSheet;

	void InitializeControlValues();
	void SetSelection(CComboBox& rComboBox, long value);
	void SetControlTextLength();
	void SetInitialCheckBoxesState();

	void UpdateAdditionalCandidatesString();
	void UpdateCandidateSpacingXMinString();
	void UpdateCandidateSpacingYMinString();
	void UpdatePreliminaryAcceptanceThresholdString();

	void EnablePreliminaryAcceptanceThresholdEdit();
	void EnableAdditionalCandidatesEdit();
	void EnableCandidateSpacingXMinEdit();
	void EnableCandidateSpacingYMinEdit();

	void AdjustSliderPreliminaryAcceptanceThreshold();
	void SetSliderPosition(UINT nId);
	BOOL ProcessOnKillfocus(UINT nId);
	
	// Validation
	void ValidateEditableParameters();
	void ValidateAdditionalCandidatesValue();
	void ValidatePreliminaryAcceptanceThreshold();
	void ValidateCandidatesSpacingXMinValue();
	void ValidateCandidatesSpacingYMinValue();

	// Generated message map functions
	//{{AFX_MSG(SVPatAdvancedPageClass)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog() override;
	afx_msg void OnAcceptanceThresholdAutoClicked();
	afx_msg void OnAdditionalCandidatesAutoClicked();
	afx_msg void OnCandidateSpacingXMinAutoClicked();
	afx_msg void OnCandidateSpacingYMinAutoClicked();
	afx_msg void OnSelchangeFastFind();
	afx_msg void OnSelchangeModelStep();
	afx_msg void OnSelchangeBeginningResolutionLevel();
	afx_msg void OnSelchangeFinalResolutionLevel();
	afx_msg void OnKillAdditionalCandidates();
	afx_msg void OnKillCandidateSpacingXMin();
	afx_msg void OnKillCandidateSpacingYMin();
	afx_msg void OnKillPreliminaryAcceptanceThreshold();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
