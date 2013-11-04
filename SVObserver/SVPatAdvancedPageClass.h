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

#if !defined(SVPATADVANCEDPAGECLASS_H)
#define SVPATADVANCEDPAGECLASS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
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
	bool ValidateEditableParameters(UINT nMsgID);
	bool ValidateAdditionalCandidatesValue(UINT nMsgID);
	bool ValidatePreliminaryAcceptanceThreshold(UINT nMsgID);
	bool ValidateCandidatesSpacingXMinValue(UINT nMsgID);
	bool ValidateCandidatesSpacingYMinValue(UINT nMsgID);

	// Generated message map functions
	//{{AFX_MSG(SVPatAdvancedPageClass)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
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

#endif // !defined(SVPATADVANCEDPAGECLASS_H)

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPatAdvancedPageClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:18:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   31 Dec 2008 08:12:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
