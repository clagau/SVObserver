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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVOCRGrayMatchThresholdDlg.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 12:59:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jul 2012 10:00:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  744
 * SCR Title:  Add Shared Memory and Socket Functionality for Run Page Web Server
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed header inclusion issue.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   22 Apr 2003 12:53:18   rschock
 * Project:  SVObserver
 * Change Request (SCR) nbr:  346
 * SCR Title:  Update SVObserver to Version 4.21 Release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Redid the #include defines and standardized the Tracker log headers and removed warning from release mode builds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 May 2000 14:44:44   Joe
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
