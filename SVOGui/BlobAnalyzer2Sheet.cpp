//*****************************************************************************
// \copyright COPYRIGHT (c) 2020,2020 by Seidenader Maschinenbau GmbH. All Rights Reserved
/// \file BlobAnalyzer2Sheet.cpp
/// This is the class for the Sheet of Blob Analyzer 2
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "BlobAnalyzer2Sheet.h"
#include "BlobAnalyzer2General.h"
#include "BlobAnalyzer2Feature.h"
#include "BlobAnalyzer2Range.h"
#include "BlobAnalyzer2Draw.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOg
{

	IMPLEMENT_DYNAMIC(BlobAnalyzer2Sheet, CPropertySheet)

	BEGIN_MESSAGE_MAP(BlobAnalyzer2Sheet, CPropertySheet)
		//{{AFX_MSG_MAP(BlobAnalyzer2Sheet)
		ON_COMMAND(IDOK, OnOK)
		ON_WM_SYSCOMMAND()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	BlobAnalyzer2Sheet::BlobAnalyzer2Sheet(uint32_t inspectionID, uint32_t toolId, uint32_t taskObjectID, UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
		: CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
		, m_InspectionID(inspectionID)
		, m_toolId(toolId)
		, m_TaskObjectID(taskObjectID)
	{
		init();
	}

	BlobAnalyzer2Sheet::BlobAnalyzer2Sheet(uint32_t inspectionID, uint32_t toolId, uint32_t taskObjectID, LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
		: CPropertySheet(pszCaption, pParentWnd, iSelectPage)
		, m_InspectionID(inspectionID)
		, m_toolId(toolId)
		, m_TaskObjectID(taskObjectID)
	{
		init();
	}

	void BlobAnalyzer2Sheet::init()
	{
		//This will remove the APPLY Button from the Tool Adjustment Dialogs.
		m_psh.dwFlags |= PSH_NOAPPLYNOW;

		addPages();
	}

	BlobAnalyzer2Sheet::~BlobAnalyzer2Sheet()
	{
		// Remove and delete all pages...
		for (int i = GetPageCount() - 1; i >= 0; --i)
		{
			CPropertyPage* pPage = GetPage(i);
			RemovePage(i);
			if (pPage)
			{
				delete pPage;
			}
		}
	}


	void BlobAnalyzer2Sheet::addPages()
	{
		AddPage(new BlobAnalyzer2General(m_InspectionID, m_toolId, m_TaskObjectID));
		AddPage(new BlobAnalyzer2Feature(m_InspectionID, m_toolId, m_TaskObjectID));
		AddPage(new BlobAnalyzer2Range(m_InspectionID, m_toolId, m_TaskObjectID));
		AddPage(new BlobAnalyzer2Draw(m_InspectionID, m_TaskObjectID));
	}


	BOOL BlobAnalyzer2Sheet::OnInitDialog()
	{
		BOOL bResult = CPropertySheet::OnInitDialog();

		// Disable and Hide Cancel Button
		HWND hWnd = ::GetDlgItem(m_hWnd, IDCANCEL);
		if (nullptr != hWnd)
		{
			::EnableWindow(hWnd, FALSE);
			::ShowWindow(hWnd, SW_HIDE);
		}
		// Remove Close Button
		ModifyStyle(WS_SYSMENU, 0, SWP_FRAMECHANGED);

		// Send Errors to the display
		SetWindowText("Blob Analyzer 2");
		
		return bResult;
	}

	void BlobAnalyzer2Sheet::OnOK()
	{
		CWaitCursor l_cwcMouse;

		// Try to validate the Equations
		int cnt = GetPageCount();

		for (int i = 0; i < cnt; i++)
		{
			CPropertyPage* pPage = GetPage(i);
			ISVPropertyPageDialog* pIDlg = dynamic_cast <ISVPropertyPageDialog*> (pPage);
			if (pIDlg && pPage->GetSafeHwnd())
			{
				if (pIDlg->QueryAllowExit() == false)	// exit not allowed
				{
					return;
				}
				pPage->OnOK();
			}
		}

		EndDialog(IDOK);
	}


	void BlobAnalyzer2Sheet::OnCancel()
	{
		EndDialog(IDCANCEL);
	}


	void BlobAnalyzer2Sheet::OnSysCommand(UINT nID, LPARAM lParam)
	{
		switch (nID & 0xFFF0)
		{
			// Ignore the close command
		case SC_CLOSE:
			return;
		}

		CPropertySheet::OnSysCommand(nID, lParam);
	}
}