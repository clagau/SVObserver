//******************************************************************************
//* COPYRIGHT (c) 2004 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalDllPage.h
//* .File Name       : $Workfile:   SVTADlgExternalSelectDllPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:04  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOGui\ISVPropertyPageDialog.h"
#include "SVOGui/ExternalToolTaskController.h"
#include "SVOGui/DataController.h"
#pragma endregion Includes

class SVToolAdjustmentDialogSheetClass;
struct SVCancelData;

class SVTADlgExternalSelectDllPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
public:
	SVTADlgExternalSelectDllPage(uint32_t inspectionID, uint32_t toolObjectID, SVToolAdjustmentDialogSheetClass* pSheet);   // standard constructor
	virtual ~SVTADlgExternalSelectDllPage() {};

	// ISVPropertyPageDialog
	bool QueryAllowExit() override;

	SVIPDoc* GetIPDoc() const;

// Dialog Data
	//{{AFX_DATA(SVTADlgExternalSelectDllPage)
	enum { IDD = IDD_TA_EXTERNAL_DLL_SELECT };
	CEdit	m_StatusEdit;
	CButton	m_btnDeleteAll;
	CButton	m_btnDelete;
	CButton	m_btnBrowse;
	CButton	m_btnAdd;
	CListBox m_lbDependentList;
	CString	m_strDLLPath;
	CString	m_strStatus;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SVTADlgExternalSelectDllPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitializeDll(bool jumpToInputPage, bool setDefaultValues = false);
	void SetDependencies();
	void testExternalDll(bool setDefaultValues = false);
	void setDefaultValuesForInputs();

	std::string getStdStringFromValueController(SvPb::EmbeddedIdEnum objectId, int index = 0);
	SVToolAdjustmentDialogSheetClass* m_pSheet;
	CString m_strLastDllPath;
	CToolTipCtrl m_ToolTip;
	const uint32_t m_ToolObjectID;
	const uint32_t m_InspectionID;

	// Generated message map functions
	//{{AFX_MSG(SVTADlgExternalSelectDllPage)
	virtual BOOL OnInitDialog() override;
	virtual BOOL OnKillActive() override;
	virtual void OnOK() override;
	afx_msg void OnDeleteAll();
	afx_msg void OnDelete();
	afx_msg void OnAdd();
	afx_msg void OnBrowse();
	
	//}}AFX_MSG
    afx_msg LRESULT OnUpdateStatus(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	BOOL m_ResetInput{true};
	bool m_preserveStatus{ false };

private:
	ExternalToolTaskController m_externalToolTaskController;
	SvOg::ValueController m_valueController;
};

