//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVExternalToolDlg
//* .File Name       : $Workfile:   SVExternalToolDlg.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:04  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOCore/SVTaskObject.h"
#include "ISVCancel.h"
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVMFCControls/SVActiveDisableButton.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
class SVExternalTool;
class SVExternalToolTask;

class SVExternalToolDlg : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
public:
	SVExternalToolDlg(const SVGUID& rInspectionID, const SVGUID& rTaskObjectID, SVToolAdjustmentDialogSheetClass* pSheet);   // standard constructor
	virtual ~SVExternalToolDlg();

	// ISVPropertyPageDialog
	bool QueryAllowExit();

	SVIPDoc* GetIPDoc() const;

// Dialog Data
	//{{AFX_DATA(SVExternalToolDlg)
	enum { IDD = IDD_TA_EXTERNAL_TOOL_DLG };
	CEdit	m_StatusEdit;
	CButton	m_btnDeleteAll;
	CButton	m_btnDelete;
	CButton	m_btnBrowse;
	CButton	m_btnAdd;
	CButton	m_btnTest;
	SvMc::SVActiveDisableButton m_btnDetails;
	CListBox m_lbDependentList;
	CString	m_strDLLPath;
	CString	m_strStatus;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVExternalToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HRESULT CleanUpOldToolInfo();
	HRESULT RestoreOriginalData();
	bool ShowDependentsDlg();
	bool m_bToolTipEnabled;
	CString m_sDetailBtnToolTipText;
	bool EnableDetailTip(  bool bEnable, const CString& strTip = CString(_T("")));
	void InitializeDll();
	void SetDependencies();
	void NotifyProgress(const CString& rstrMessage);


	SVToolAdjustmentDialogSheetClass* m_pSheet;
	SVExternalTool* m_pTool;
	SVExternalToolTask* m_pTask;
	CString m_strLastDllPath;
	CToolTipCtrl m_ToolTip;
	SVCancelData* m_pCancelData;
	SVGUID m_InspectionID;
	SVGUID m_TaskObjectID;

	// Generated message map functions
	//{{AFX_MSG(SVExternalToolDlg)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	afx_msg void OnDetails();
	afx_msg void OnDeleteAll();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	afx_msg void OnTest();
	//}}AFX_MSG
    afx_msg LRESULT OnUpdateStatus(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};

