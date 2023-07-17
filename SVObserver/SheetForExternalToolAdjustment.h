//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SheetForExternalToolAdjustment.h
/// The property sheet for the tool adjustment dialog for the external tool
//*****************************************************************************

#pragma once

#pragma region Includes
#include "SVOGuiUtility/ExternalToolTaskController.h"
#pragma endregion Includes

class SheetForExternalToolAdjustment :public TADlgSheetClass
{
public:
	SheetForExternalToolAdjustment(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd = nullptr, UINT iSelectPage = 0);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(SheetForExternalToolAdjustment)
public:
	virtual BOOL OnInitDialog() override;
	//}}AFX_VIRTUAL

	void OnRunOnce();
	void OnReInitialize();
	CButton m_runOnceButton;
	CButton m_reInitializeButton;

protected:
	virtual void addPages() override;
	//{{AFX_MSG(SheetForExternalToolAdjustment)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void ExternalToolRetainOnlySelectDllPage();

	LRESULT AdaptToTestedDll(WPARAM, LPARAM);
	LRESULT AdaptToUntestedDll(WPARAM, LPARAM);
	LRESULT ExternalToolSelectInputValuePage(WPARAM, LPARAM);
	LRESULT UpdateIpDocViews(WPARAM, LPARAM);

private:
	SvOgu::ExternalToolTaskController m_externalToolTaskController;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

