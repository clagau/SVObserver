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

#include "SVOMFCLibrary/SVDataItemManagerTemplate.h"

#define INTERNAL_INSPECTION_NAME "Inspection_"

/////////////////////////////////////////////////////////////////////////////
// CSVOInspectionSourceDlg dialog

class CSVOConfigAssistantDlg;

class CSVOInspectionSourceDlg : public CPropertyPage
{
	DECLARE_DYNCREATE(CSVOInspectionSourceDlg)

// Construction
public:
	CSVOInspectionSourceDlg();
	virtual ~CSVOInspectionSourceDlg();
    
    void SetupList();
// Dialog Data  
	//{{AFX_DATA(CSVOInspectionSourceDlg)
	enum { IDD = IDD_DLG_PROPPAGE_AA_INSPECTION };
	CListBox	m_ctlIPDlist;
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
	//{{AFX_VIRTUAL(CSVOInspectionSourceDlg)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSVOInspectionSourceDlg)
	afx_msg void OnBtnAddIpd();
	afx_msg void OnBtnDeleteVi();
	afx_msg void OnBtnPropVi();
	afx_msg void OnBtnImportIpd();
	afx_msg void OnBtnExportIpd();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeLstIpdList();
	afx_msg void OnDblclkLstIpdList();
	afx_msg void OnDestroy();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


private:
	void ClearList();
    CString GetNextInspectionDisplayName(CString sLabeName);

    void LoadInspectionFileNameList(); 
    void LoadVInspectionList();
	void EnableDisableExport();

    int m_iVIHorzScroll;
    int m_iIPDHorzScroll;
    CSVOConfigAssistantDlg *m_pParent;
    int m_iInspectionNextCount;
    int m_iCursel;
    int m_iEditingSel;

	typedef SVDataItemManagerTemplate< CString > SVDataItemManager;

	SVDataItemManager m_Items;


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
