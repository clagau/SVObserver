//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVOInspectionSourceDlg
//* .File Name       : $Workfile:   SVOInspectionSourceDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   26 Jul 2013 10:16:06  $
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

#define INTERNAL_INSPECTION_NAME "Inspection_"

/////////////////////////////////////////////////////////////////////////////
// SVOInspectionSourceDlg dialog

class SVOConfigAssistantDlg;

class SVOInspectionSourceDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(SVOInspectionSourceDlg)

// Construction
public:
	SVOInspectionSourceDlg();
	virtual ~SVOInspectionSourceDlg() = default;
    
    void SetupList();
// Dialog Data  
	//{{AFX_DATA(SVOInspectionSourceDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_INSPECTION };
	CListBox m_ctlIPDlist;
	CEdit	m_ctlInspectionEdt;
	CButton	m_ctlPropVIbtn;
	CButton	m_ctlNewVIbtn;
	CButton	m_ctlDeleteVIbtn;
	CButton	m_ctlAttachVIbtn;
	CButton	m_ctlAddIPDbtn;
	CString	m_sDeviceName;
	CButton m_ctlImportIPDbtn;
	CButton m_ctlExportIPDbtn;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(SVOInspectionSourceDlg)
	public:
	virtual BOOL OnKillActive() override;
	virtual BOOL OnSetActive() override;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(SVOInspectionSourceDlg)
	afx_msg void OnBtnAddIpd();
	afx_msg void OnBtnDeleteVi();
	afx_msg void OnBtnPropVi();
	afx_msg void OnBtnImportIpd();
	afx_msg void OnBtnExportIpd();
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSelchangeLstIpdList();
	afx_msg void OnDblclkLstIpdList();
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	void ClearList();

	void EnableDisableExport();

	int m_iVIHorzScroll{0};
	int m_iIPDHorzScroll{0};
    SVOConfigAssistantDlg *m_pParent;
	int m_iInspectionNextCount{1};
	int m_iCursel{-1};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
