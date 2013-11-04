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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVPatGeneralPageClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 13:19:24   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   27 Jan 2011 12:22:06   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   31 Dec 2008 11:22:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  644
 * SCR Title:  Pattern Analyzer Enhancements and Bug Fixes
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Results range button to allow setup of result range values.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   24 Jul 2007 15:43:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  597
 * SCR Title:  Upgrade Matrox Imaging Library to version 8.0
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Update MIL 8.0
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 13:37:56   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
