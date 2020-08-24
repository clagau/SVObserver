//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOCameraDlg
//* .File Name       : $Workfile:   SVOCameraDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Dec 2013 11:03:16  $
//******************************************************************************

#pragma once

class SVOConfigAssistantDlg;

class SVOCameraDlg : public CPropertyPage
{
// Construction
public:
	SVOCameraDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~SVOCameraDlg() = default;

    void SetupList();
// Dialog Data
	//{{AFX_DATA(SVOCameraDlg)
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
	//{{AFX_VIRTUAL(SVOCameraDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(SVOCameraDlg)
	afx_msg void OnSelchangeLstCamera();
	afx_msg void OnBtnAdvanced();
	afx_msg void OnBtnDeleteVc();
	afx_msg void OnBtnNewVc();
	afx_msg void OnBtnPropVc();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnDblclkLstCamera();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnCameraManager();
	afx_msg void OnUpdateAdvancedBtn( CCmdUI* PCmdUI );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    SVOConfigAssistantDlg *m_pParent;
	bool m_bModified{false};
	bool m_bNewConfig{false};
    
    void SetCameraPropForAll( LPCTSTR CurrentCamera );
};

