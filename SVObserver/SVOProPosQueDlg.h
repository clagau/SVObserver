//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOProPosQueDlg
//* .File Name       : $Workfile:   SVOProPosQueDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:12:06  $
//******************************************************************************

#pragma once

class CSVOConfigAssistantDlg;

class CSVOProPosQueDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSVOProPosQueDlg)

// Construction
public:
	CSVOProPosQueDlg();
	virtual ~CSVOProPosQueDlg();

    void SetupList();

// Dialog Data
	//{{AFX_DATA(CSVOProPosQueDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_PPQ };
	CEdit	m_ctlVTEdit;
	CListBox	m_ctlVIPList;
	CListBox	m_ctlVCList;
	CListBox	m_ctlPPQList;
	CButton	m_ctlRemoveVTbtn;
	CButton	m_ctlRemoveVIbtn;
	CButton	m_ctlRemoveVCbtn;
	CButton	m_ctlPropPPQbtn;
	CButton	m_ctlNewPPQbtn;
	CButton	m_ctlDeletePPQbtn;
	CButton	m_ctlAddVTbtn;
	CButton	m_ctlAddVIbtn;
	CButton	m_ctlAddVCbtn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSVOProPosQueDlg)
	public:
	virtual BOOL OnKillActive() override;
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSVOProPosQueDlg)
	afx_msg void OnBtnAddVc();
	afx_msg void OnBtnAddVi();
	afx_msg void OnBtnAddVt();
	afx_msg void OnBtnDeletePpq();
	afx_msg void OnBtnNewPpq();
	afx_msg void OnBtnPropPpq();
	afx_msg void OnBtnRemoveVc();
	afx_msg void OnBtnRemoveVi();
	afx_msg void OnBtnRemoveVt();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeLstPpqList();
	afx_msg void OnDblclkLstPpqList();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
    BOOL m_bIsListSetup;
	int m_iVCHorzScroll;

    CSVOConfigAssistantDlg *m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

