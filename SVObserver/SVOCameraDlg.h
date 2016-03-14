//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : CSVOCameraDlg
//* .File Name       : $Workfile:   SVOCameraDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Dec 2013 11:03:16  $
//******************************************************************************

#pragma once

class CSVOConfigAssistantDlg;

class CSVOCameraDlg : public CPropertyPage
{
// Construction
public:
	CSVOCameraDlg(CWnd* pParent = NULL);   // standard constructor

    void SetupList();
	void DisableAdvanceBtn();
	void ShowCameraDlgBtn( int iShow );
// Dialog Data
	//{{AFX_DATA(CSVOCameraDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_CAMERA };
	CButton	m_btnCameraManager;
	CListBox	m_ctlCameraList;
	CButton	m_btnVCProp;
	CButton	m_btnNew;
	CButton	m_btnDelete;
	CButton	m_btnAdvancedProp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVOCameraDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSVOCameraDlg)
	afx_msg void OnSelchangeLstCamera();
	afx_msg void OnBtnAdvanced();
	afx_msg void OnBtnDeleteVc();
	afx_msg void OnBtnNewVc();
	afx_msg void OnBtnPropVc();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkLstCamera();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCameraManager();
	afx_msg void OnUpdateAdvancedBtn( CCmdUI* PCmdUI );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    CSVOConfigAssistantDlg *m_pParent;
    BOOL m_bModified;
    BOOL m_bNewConfig;
    
    void SetCameraPropForAll(CString sCurrentCamera);
};

