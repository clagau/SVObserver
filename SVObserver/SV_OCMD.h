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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SV_OCMD.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:18:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.2   25 Jun 2003 15:00:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  349
 * SCR Title:  Fix Bug I0001101 Enter text in minimum scale field
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   changed method UpdateOCRParameters to return a BOOL value
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 3.1   17 Apr 2003 16:14:28   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
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
 *    Rev 1.3   Oct 20 1999 07:21:16   Nick
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