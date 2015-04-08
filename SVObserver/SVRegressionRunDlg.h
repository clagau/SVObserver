// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionRunDlg
// * .File Name       : $Workfile:   SVRegressionRunDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   25 Nov 2014 20:32:06  $
// ******************************************************************************

#ifndef SVREGRESSIONRUNDLG_H
#define SVREGRESSIONRUNDLG_H

#pragma region Includes
#include "SVMFCControls/SVSliderCtrl.h"
#include "SVRegressionTestStruct.h"
#include "SVLibrary/SVListCtrl.h"
#pragma endregion Includes

class CSVRegressionRunDlg : public CDialog
{
#pragma region Constructor
public:
	CSVRegressionRunDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CSVRegressionRunDlg();
#pragma endregion Constructor

#pragma region MFC Methods
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVRegressionRunDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CSVRegressionRunDlg)
	afx_msg void OnBtnBeginning();
	afx_msg void OnBtnFrameBack();
	afx_msg void OnBtnFrameUp();
	afx_msg void OnBtnMode();
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnStop();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBtnSettings();
	afx_msg void OnBtnExit();
	afx_msg LRESULT SetNextFiles(WPARAM, LPARAM);
	afx_msg LRESULT SetPlayPauseBtn(WPARAM, LPARAM);
	afx_msg LRESULT SetPreviousFrameBtn(WPARAM, LPARAM);
	afx_msg LRESULT CloseRegressionTest(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
#pragma endregion MFC Methods

#pragma region Public Methods
public:
	void SetIPDocParent(SVIPDoc* pIPDocParent);
#pragma endregion Public Methods

#pragma region Private Methods
private: 
	BOOL EnableButtons(BOOL p_bValue);
	void DestroyIcons();

	//************************************
	// Method:    setDelayTime
	// Description:  set the delay time depending of the position to edit control and system.
	// Parameter: int position
	// Returns:   void
	//************************************
	void setDelayTime( int position );
#pragma endregion Private Methods

#pragma region Member variables
private:
	// Dialog Data
	//{{AFX_DATA(CSVRegressionRunDlg)
	enum { IDD = IDD_DIALOG_REGRESSIONTEST_RUN };
	CButton	m_btnExit;
	CButton	m_btnSettings;
	SVSliderCtrl m_sliderDelayTime;
	CListBox	m_lstCameraImages;
	CButton	m_btnStopExit;
	CButton	m_btnPlayPause;
	CButton	m_btnMode;
	CButton	m_btnFrameUp;
	CButton	m_btnFrameBack;
	CButton	m_btnBeginning;
	CString	m_timeDelayText;
	//}}AFX_DATA

	 // data members
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
	BOOL m_bFirst;
	int	m_timeDelayInMS;
#pragma endregion Member variables
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
 *    Rev 1.3   25 Nov 2014 20:32:06   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  930
 * SCR Title:  Improve Regression Test Dialog (SVO-136)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Code review clean up
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   20 Nov 2014 08:14:38   mziegler
 * Project:  SVObserver
 * Change Request (SCR) nbr:  930
 * SCR Title:  Improve Regression Test Dialog (SVO-136)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   changes all over the RegressionRunDlg for new design (SVO-136)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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