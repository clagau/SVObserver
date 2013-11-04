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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayCandidateDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:56:14   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:52:58   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:50   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  133, 132
 * SCR Title:  Add OCR Gray Scale to SVObserver and Create OCR Gray Scale Font Training
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Upgrade Logical Vision WiT v5.3 to Logical Vision FastOCR v1.2.  Completely changed headers, libraries, and variable name types.  Functionality still the same as prior version.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.0   22 Nov 1999 17:13:28   jim
 * Project:  SVObserver
 * Change Request (SCR) nbr:  61
 * SCR Title:  Update PVCS versioning to version 3.0.
 * Checked in by:  JimAdmin;  James A. Brown
 * Change Description:  
 *   
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   Oct 20 1999 07:21:18   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   Changes for character x and y axis sort work.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   Aug 31 1999 19:35:16   Nick
 * Project:  SVObserver
 * Change Request (SCR) nbr:  12
 * SCR Title:  Port OCR functionallity from custom 2.xx version.
 * Checked in by:  Nick;  F Roland "Nick" Bjorklund
 * Change Description:  
 *   SVObserver 3.0 architecture changes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/