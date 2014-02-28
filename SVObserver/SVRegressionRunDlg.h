// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionRunDlg
// * .File Name       : $Workfile:   SVRegressionRunDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   28 Feb 2014 09:22:56  $
// ******************************************************************************

#ifndef SVREGRESSIONRUNDLG_H
#define SVREGRESSIONRUNDLG_H

#include "SVRegressionTestStruct.h"
#include "SVLibrary/SVListCtrl.h"

class CSVRegressionRunDlg : public CDialog
{
// Construction
public:
	CSVRegressionRunDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSVRegressionRunDlg();
	void SetIPDocParent(SVIPDoc* pIPDocParent);

	int GetTimeoutPeriod();

// Dialog Data
	//{{AFX_DATA(CSVRegressionRunDlg)
	enum { IDD = IDD_DIALOG_REGRESSIONTEST_RUN };
	CButton	m_btnExit;
	CButton	m_btnSettings;
	CSpinButtonCtrl	m_spnDelayTime;
	CListBox	m_lstCameraImages;
	CButton	m_btnStopExit;
	CButton	m_btnPlayPause;
	CButton	m_btnMode;
	CButton	m_btnFrameUp;
	CButton	m_btnFrameBack;
	CButton	m_btnBeginning;
	int		m_iNumberSeconds;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVRegressionRunDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	afx_msg LRESULT SetNextFiles(WPARAM, LPARAM);
	afx_msg LRESULT SetPlayPauseBtn(WPARAM, LPARAM);
	afx_msg LRESULT SetPreviousFrameBtn(WPARAM, LPARAM);
	afx_msg LRESULT CloseRegressionTest(WPARAM, LPARAM);
	// Generated message map functions
	//{{AFX_MSG(CSVRegressionRunDlg)
	afx_msg void OnBtnBeginning();
	afx_msg void OnBtnFrameBack();
	afx_msg void OnBtnFrameUp();
	afx_msg void OnBtnMode();
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnStop();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDeltaposSpinDelayTime(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBtnSettings();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBtnExit();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void OnCancel();



private: // data members
	HICON m_iconPlay;
	HICON m_iconPause;
	HICON m_iconModeContinue;
	HICON m_iconModeRunToEnd;
	HICON m_iconFrameUp;
	HICON m_iconFrameBack;
	HICON m_iconBeginning;
	HICON m_iconStop;

	RegressionPlayPause m_enumPlayPause;
	RegressionPlayModeEnum m_enumMode;

	CToolTipCtrl* m_ctlToolTip;

	SVIPDoc* m_pIPDocParent;
	BOOL EnableButtons(BOOL p_bValue);
	void DestroyIcons();
	BOOL m_bFirst;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SVREGRESSIONRUNDLG_H__3AE6579A_1F58_4209_BAF7_84E524EF5D21__INCLUDED_)

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVObserver\SVRegressionRunDlg.h_v  $
 * 
 *    Rev 1.1   28 Feb 2014 09:22:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  890
 * SCR Title:  Fix SVObserver Memory Leaks
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Delete Icons. Add virtual destructor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Apr 2013 14:42:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Dec 2010 13:35:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes in notification functionality using the Observer Design Pattern.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   16 Dec 2009 12:34:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fix issues with includes and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   30 Aug 2005 11:21:02   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new message handle to Exit the regression.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   10 Aug 2005 10:33:18   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   fixed problems with regression test.  settings button will reselect files and will reset the files back to beginning of the list
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   15 Jul 2005 11:34:38   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  462
 * SCR Title:  Redo the fuctionality of the Regression Test
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   First iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/