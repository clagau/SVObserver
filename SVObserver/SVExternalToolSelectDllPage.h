//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSelectExternalDllPage.h
//* .File Name       : $Workfile:   SVExternalToolSelectDllPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:04  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOGui\ISVPropertyPageDialog.h"
#pragma endregion Includes

namespace SvOp
{
class SVExternalToolTask;
}

class SVToolAdjustmentDialogSheetClass;
struct SVCancelData;

class SVSelectExternalDllPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
public:
	SVSelectExternalDllPage(uint32_t inspectionID, uint32_t toolObjectID, SVToolAdjustmentDialogSheetClass* pSheet);   // standard constructor
	virtual ~SVSelectExternalDllPage();

	// ISVPropertyPageDialog
	bool QueryAllowExit();

	SVIPDoc* GetIPDoc() const;

// Dialog Data
	//{{AFX_DATA(SVSelectExternalDllPage)
	enum { IDD = IDD_TA_EXTERNAL_DLL_SELECT };
	CEdit	m_StatusEdit;
	CButton	m_btnDeleteAll;
	CButton	m_btnDelete;
	CButton	m_btnBrowse;
	CButton	m_btnAdd;
	CButton	m_btnUndoChanges;
	CListBox m_lbDependentList;
	CString	m_strDLLPath;
	CString	m_strStatus;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVSelectExternalDllPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HRESULT CleanUpOldToolInfo();
	HRESULT RestoreOriginalData();
	void InitializeDll(bool jumpToInputPage);
	void SetDependencies();
	void NotifyProgress(LPCTSTR Message);
	void AddPagesForTestedExternalTool(bool jumpToInputPage);
	void RemovePagesForTestedExternalTool();
	void testExternalDll();

	SVToolAdjustmentDialogSheetClass* m_pSheet;
	SvOp::SVExternalToolTask* m_pTask;
	CString m_strLastDllPath;
	CToolTipCtrl m_ToolTip;
	SVCancelData* m_pCancelData;
	const uint32_t m_ToolObjectID;
	const uint32_t m_InspectionID;
	uint32_t m_TaskObjectID = SvDef::InvalidObjectId;

	// Generated message map functions
	//{{AFX_MSG(SVSelectExternalDllPage)
	virtual BOOL OnInitDialog() override;
	virtual void OnOK() override;
	afx_msg void OnDeleteAll();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	afx_msg void OnUndoChanges();

	//}}AFX_MSG
    afx_msg LRESULT OnUpdateStatus(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};

