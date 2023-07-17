//*****************************************************************************
/// \copyright COPYRIGHT (c) 2023, 2023 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file ResizablePropertySheet.cpp
/// \brief Make CPropertySheet resizable using dynamic layout
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ResizablePropertySheet.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace SvMc
{

BEGIN_MESSAGE_MAP(ResizablePropertySheet, CPropertySheet)
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
ResizablePropertySheet::ResizablePropertySheet()
{
}

ResizablePropertySheet::ResizablePropertySheet(UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage) : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

ResizablePropertySheet::ResizablePropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd, UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

ResizablePropertySheet::~ResizablePropertySheet()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL ResizablePropertySheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();

	ModifyStyle(WS_SYSMENU, WS_THICKFRAME, SWP_FRAMECHANGED);
	SetupDynamicLayout();
	return TRUE;
}

#pragma endregion Public Methods

#pragma region Protected Methods
void ResizablePropertySheet::SetupDynamicLayout()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	constexpr int gripperSize = 12;
	constexpr UINT cResizeGripper {61501};
	CRect gripperRect(clientRect.right - gripperSize, clientRect.bottom - gripperSize, clientRect.right, clientRect.bottom);
	m_resizeGripper.Create(SBS_SIZEGRIP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, gripperRect, this, cResizeGripper);

	EnableDynamicLayout(TRUE);
	auto pManager = GetDynamicLayout();
	if (pManager != nullptr)
	{
		pManager->Create(this);

		for (CWnd* child = GetWindow(GW_CHILD); child; child = child->GetWindow(GW_HWNDNEXT))
		{
			// All buttons need to be moved 100% in all directions
			if (child->SendMessage(WM_GETDLGCODE) & DLGC_BUTTON)
			{
				pManager->AddItem(child->GetSafeHwnd(), CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());
			}
			else // This will be the main tab control which needs to be stretched in both directions
			{
				pManager->AddItem(child->GetSafeHwnd(), CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
			}

		}
	}
}
#pragma endregion Protected Methods
} //namespace SvMc

