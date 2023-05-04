//******************************************************************************
//* COPYRIGHT (c) 2004 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTADlgExternalDllPage.h
//* .File Name       : $Workfile:   TADlgExternalSelectDllPage.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:32:04  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOGui/ISVPropertyPageDialog.h"
#include "SVOGuiUtility/DataController.h"
#pragma endregion Includes

namespace SvOgu
{
class ExternalToolTaskController;
}
class TADlgSheetClass;
struct SVCancelData;

class TADlgExternalSelectDllPage : public CPropertyPage, public SvOg::ISVPropertyPageDialog
{
public:
	TADlgExternalSelectDllPage(uint32_t inspectionID, uint32_t toolObjectID, CWnd& rParent, SvOgu::ExternalToolTaskController& rExternalToolTaskController);   // standard constructor
	virtual ~TADlgExternalSelectDllPage() {};

	// ISVPropertyPageDialog
	bool QueryAllowExit() override;

// Dialog Data
	//{{AFX_DATA(TADlgExternalSelectDllPage)
	enum { IDD = IDD_TA_EXTERNAL_DLL_SELECT };
	CEdit	m_StatusEdit;
	CButton	m_btnDeleteAll;
	CButton	m_btnDelete;
	CButton	m_btnBrowse;
	CButton	m_btnAdd;
	CListBox m_lbDependentList;
	CString	m_currentExternalDllFilepath;
	CString	m_strStatus;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(TADlgExternalSelectDllPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitializeDllAndDisplayResults(bool jumpToInputPage, bool setDefaultValues = false);

protected:
	void SetDependencies();
	void testExternalDll(bool setDefaultValues = false);
	void setDefaultValuesForInputs();

	std::string getStdStringFromValueController(SvPb::EmbeddedIdEnum objectId, int index = 0);
	CWnd& m_rParent;
	CString m_previousExternalDllDirectorypath;
	CToolTipCtrl m_ToolTip;
	const uint32_t m_ToolObjectID;
	const uint32_t m_InspectionID;

	// Generated message map functions
	//{{AFX_MSG(TADlgExternalSelectDllPage)
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
	bool InitializeDll(bool setDefaultValues);
	void showDllConfigurationPages(bool show);
	void displayInitializationProblem(std::string error);
	void displayInitializationFailure(const SvStl::MessageContainer& e);

	SvOgu::ExternalToolTaskController& m_rExternalToolTaskController;
	SvOgu::ValueController m_valueControllerToolTask;
};

