// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVRegressionRunDlg
// * .File Name       : $Workfile:   SVRegressionRunDlg.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   25 Nov 2014 20:32:06  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVMFCControls/SVSliderCtrl.h"
#include "SVRegressionTestStruct.h"
#include "ObjectInterfaces/IFormulaController.h"
#pragma endregion Includes

class SVIPDoc;
class RegressionTestController;

class SVRegressionRunDlg : public CDialog
{
#pragma region Constructor
public:
	explicit SVRegressionRunDlg(RegressionTestController& rRegessionController, uint32_t inspectionID, CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVRegressionRunDlg();
#pragma endregion Constructor

#pragma region MFC Methods
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVRegressionRunDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(SVRegressionRunDlg)
	afx_msg void OnBtnBeginning();
	afx_msg void OnBtnFrameBack();
	afx_msg void OnBtnFrameUp();
	afx_msg void OnBtnMode();
	afx_msg void OnBtnPlay();
	afx_msg void OnBtnStop();
	afx_msg void OnCheckPlayCond();
	afx_msg void OnValidateMode();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBtnSettings();
	afx_msg void OnBtnExit();
	afx_msg void OnBnClickedFormula();
	afx_msg void OnBnClickedResultFile();
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

	/// Set the Equation text to the screen
	void setEquationText();

	void SortSelectedFiles(std::vector<RegressionTestStruct>& rRegTestList);
#pragma endregion Private Methods

#pragma region Member variables
private:
	// Dialog Data
	//{{AFX_DATA(SVRegressionRunDlg)
	enum { IDD = IDD_DIALOG_REGRESSIONTEST_RUN };
	CButton	m_btnExit;
	CButton	m_btnSettings;
	SvMc::SVSliderCtrl m_sliderDelayTime;
	CListBox	m_lstCameraImages;
	CButton	m_btnStopExit;
	CButton	m_btnPlayPause;
	CButton	m_btnMode;
	CButton	m_btnFrameUp;
	CButton	m_btnFrameBack;
	CButton	m_btnBeginning;
	CString	m_timeDelayText;
	CString m_validateResultFile;
	CString m_equationString;
	//}}AFX_DATA

	 // data members
	HICON m_iconPlay {nullptr};
	HICON m_iconPause {nullptr};
	HICON m_iconModeContinue {nullptr};
	HICON m_iconModeRunToEnd {nullptr};
	HICON m_iconFrameUp {nullptr};
	HICON m_iconFrameBack {nullptr};
	HICON m_iconBeginning {nullptr};
	HICON m_iconStop {nullptr};

	RegressionPlayPause m_enumPlayPause {RegressionPlayPause::Pause};
	RegressionPlayModeEnum m_enumMode {RegressionPlayModeEnum::RunToEnd};

	CToolTipCtrl* m_ctlToolTip {nullptr};

	SVIPDoc* m_pIPDocParent {nullptr};
	BOOL m_bFirst {false};
	BOOL m_bPlayByEquation {false};
	BOOL m_isValidationMode {false};
	int	m_timeDelayInMS {0};
	uint32_t m_InspectionID {0UL};

	RegressionTestController& m_rRegressionController;
	SvOi::IFormulaControllerPtr m_pFormulaController;
#pragma endregion Member variables
};

