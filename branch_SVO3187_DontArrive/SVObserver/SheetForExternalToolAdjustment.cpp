//*****************************************************************************
// \copyright COPYRIGHT (c) 2021,2021 by Körber Pharma Inspection GmbH. All Rights Reserved
/// \file SheetForExternalToolAdjustment.cpp
/// The property sheet for the tool adjustment dialog for the external tool (plus the helper class GreyOutHelper)
//*****************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVToolAdjustmentDialogSheetClass.h"
#include "SheetForExternalToolAdjustment.h"
#include "SVIPDoc.h"
#include "SVTADlgExternalSelectDllPage.h"
#include "SVTADlgExternalInputSelectPage.h"
#include "SVTADlgExternalResultPage.h"
#include "Definitions/SVUserMessage.h"
#include "SVOGui/SVTADlgExternalToolImageSelectPage.h"


#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

constexpr int c_indexOfSelectDllPage = 0;/// this is the position at which the "Select DLL" page will be found
constexpr int c_indexOfInputValuePage = 2;/// this is the position at which the "Input Values" page will be found - if it is present at all!
constexpr int c_indexOfOutputValuePage = 3;/// this is the position at which the "Output Values" page will be found - if it is present at all!

class GreyOutHelper //uses RAII to facilitate temporary renaming and greying out of MFC buttons
{
public:
	GreyOutHelper(CButton& rButton, const TCHAR* pTemporaryCaption) :
		m_rButton(rButton)
	{
		m_rButton.GetWindowText(m_originalDialogCaption);
		m_rButton.SetWindowText(pTemporaryCaption);
		m_rButton.EnableWindow(false);
	};

	~GreyOutHelper()
	{
		m_rButton.SetWindowText(m_originalDialogCaption);
		m_rButton.EnableWindow(true);
	};

private:
	CButton& m_rButton;
	CString m_originalDialogCaption;

};

IMPLEMENT_DYNAMIC(SheetForExternalToolAdjustment, SVToolAdjustmentDialogSheetClass)

BEGIN_MESSAGE_MAP(SheetForExternalToolAdjustment, SVToolAdjustmentDialogSheetClass)
	//{{AFX_MSG_MAP(SheetForExternalToolAdjustment)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_RUN_ONCE, OnRunOnce)
	ON_BN_CLICKED(IDC_RE_INITIALIZE, OnReInitialize)
	ON_MESSAGE(SV_ADAPT_TO_UNTESTED_DLL, AdaptToUntestedDll)
	ON_MESSAGE(SV_ADAPT_TO_TESTED_DLL, AdaptToTestedDll)
	ON_MESSAGE(SV_SELECT_INPUT_VALUE_PAGE, ExternalToolSelectInputValuePage)
	ON_MESSAGE(SV_UPDATE_IPDOC_VIEWS, UpdateIpDocViews)
END_MESSAGE_MAP()

SheetForExternalToolAdjustment::SheetForExternalToolAdjustment(SVIPDoc* p_pIPDoc, uint32_t inspectionID, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	: 
	SVToolAdjustmentDialogSheetClass(p_pIPDoc, inspectionID, taskObjectID, pszCaption, pParentWnd, iSelectPage), 
	m_externalToolTaskController(inspectionID, taskObjectID)
{
}

BOOL SheetForExternalToolAdjustment::OnInitDialog()
{
	BOOL bResult = SVToolAdjustmentDialogSheetClass::OnInitDialog();

	CRect rect, tabrect;

	//Get button sizes and positions
	GetDlgItem(IDOK)->GetWindowRect(rect);
	GetTabControl()->GetWindowRect(tabrect);
	ScreenToClient(rect); ScreenToClient(tabrect);

	//New button - width, height and Y-coordiate of IDOK
	//           - X-coordinate of tab control
	auto width = rect.Width();

	rect.left = tabrect.left; rect.right = tabrect.left + width;

	m_reInitializeButton.Create("Re-Initialize",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			rect, this, IDC_RE_INITIALIZE);

	m_reInitializeButton.SetFont(GetFont());

	rect.left += width + 10; rect.right += width + 10;

	m_runOnceButton.Create("Run Once",
			BS_PUSHBUTTON | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			rect, this, IDC_RUN_ONCE);

	m_runOnceButton.SetFont(GetFont());

	return bResult;
}

void SheetForExternalToolAdjustment::OnRunOnce()
{
	GreyOutHelper goh(m_runOnceButton, _T("<Running>"));

	SvOi::IObjectClass* pObject = GetTaskObject();
	ResetTools(pObject);

	auto activeIndex = GetActiveIndex();

	if (c_indexOfOutputValuePage == activeIndex)
	{
		auto pOutputValuePage = dynamic_cast<SVTADlgExternalResultPage*>(GetActivePage());

		if (nullptr != pOutputValuePage)
		{
			pOutputValuePage->OnSetActive();
		}
	}
}

void SheetForExternalToolAdjustment::OnReInitialize()
{
	GreyOutHelper goh(m_reInitializeButton, _T("<Initializing>"));

	SetActivePage(c_indexOfSelectDllPage);

	auto pSelectDllPage = dynamic_cast<SVTADlgExternalSelectDllPage*>(GetPage(c_indexOfSelectDllPage));

	if (nullptr != pSelectDllPage)
	{
		pSelectDllPage->InitializeDllAndDisplayResults(false);
	}
}

void SheetForExternalToolAdjustment::addPages()
{
	SvDef::SVObjectTypeInfoStruct ObjectInfo;

	AddPage(new SVTADlgExternalSelectDllPage(GetInspectionID(), GetTaskObjectID(), *this, m_externalToolTaskController));
	AddPage(createToolAdjustmentDialogCommentPage());
};


void SheetForExternalToolAdjustment::ExternalToolRetainOnlySelectDllPage()
{
	while (GetPageCount() > 1) // if the external DLL is uninitialized only the first page ("Select External DLL")
							   // plus the "additional pages" "Conditional", "General" (which will be added elsewhere) should be displayed
	{
		RemovePage(1);
	}
}

LRESULT SheetForExternalToolAdjustment::AdaptToTestedDll(WPARAM, LPARAM)
{
	ExternalToolRetainOnlySelectDllPage();

	m_runOnceButton.ShowWindow(SW_SHOW);
	m_reInitializeButton.ShowWindow(SW_SHOW);

	AddPage(new SvOg::SVTADlgExternalToolImageSelectPage(GetInspectionID(), GetTaskObjectID()));
	AddPage(new SVTADlgExternalInputSelectPage(_T("Input Values"), GetInspectionID(), GetTaskObjectID(), m_externalToolTaskController));
	AddPage(new SVTADlgExternalResultPage(_T("Result Values"), GetInspectionID(), m_externalToolTaskController.getExternalToolTaskObjectId(), m_externalToolTaskController));

	AddAdditionalPagesForExternalTool();
	return S_OK;
}

LRESULT SheetForExternalToolAdjustment::ExternalToolSelectInputValuePage(WPARAM, LPARAM)
{
	return PostMessage(PSM_SETCURSEL, c_indexOfInputValuePage, 0);
}

LRESULT SheetForExternalToolAdjustment::AdaptToUntestedDll(WPARAM, LPARAM)
{
	ExternalToolRetainOnlySelectDllPage();

	m_runOnceButton.ShowWindow(SW_HIDE);
	m_reInitializeButton.ShowWindow(SW_HIDE);

	AddAdditionalPagesForExternalTool();
	return S_OK;
}


LRESULT SheetForExternalToolAdjustment::UpdateIpDocViews(WPARAM, LPARAM)
{
	SVIPDoc* pIPDoc = GetIPDoc();

	if (nullptr != pIPDoc)
	{
		pIPDoc->UpdateAllViews(nullptr);
		return S_OK;
	}

	return E_POINTER;
}
