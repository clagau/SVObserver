//******************************************************************************
//* COPYRIGHT (c) 2008, 2009 by Magnus Egelberg
//* All Rights Reserved 
//* Author           : Magnus Egelberg (magnus.egelberg@lundalogik.se)
//* Licenses         : No known license
//* From             : http://www.codeguru.com/cpp/controls/propertysheet/sizing/article.php/c599/Resizable-CPropertySheet.htm
//******************************************************************************
//* .Module Name     : CResizablePropertySheet
//* .File Name       : $Workfile:   ResizablePropertySheet.cpp  $
//* .Description	 : The class resizes the registered controls in a dialog 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:26  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ResizablePropertySheet.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::ObjectSelectorLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CResizablePropertySheet, CPropertySheet)

BEGIN_MESSAGE_MAP(CResizablePropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CResizablePropertySheet)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#pragma endregion Declarations

#pragma region Constructor
CResizablePropertySheet::CResizablePropertySheet()
{
}

CResizablePropertySheet::CResizablePropertySheet(UINT nIDCaption, CWnd *pParentWnd, UINT iSelectPage) : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CResizablePropertySheet::CResizablePropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd, UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CResizablePropertySheet::~CResizablePropertySheet()
{
}
#pragma endregion Constructor

#pragma region Public Methods
BOOL CResizablePropertySheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();

	ASSERT(GetActivePage() != 0);

	// Get the original client rect
	GetClientRect(m_rcClient);

	// Get the original page size 
	GetActivePage()->GetWindowRect(m_rcPage);
	ScreenToClient(m_rcPage);

	// Add all the controls to the resizer
	if (GetDlgItem(ID_APPLY_NOW) != 0)
	{
		m_Resize.Add(this, ID_APPLY_NOW, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);
	}

	if (GetDlgItem(ID_WIZBACK) != 0)
	{
		m_Resize.Add(this, ID_WIZBACK, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);
	}

	if (GetDlgItem(ID_WIZNEXT) != 0)
	{
		m_Resize.Add(this, ID_WIZNEXT, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);
	}

	if (GetDlgItem(ID_WIZFINISH) != 0)
	{
		m_Resize.Add(this, ID_WIZFINISH, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);
	}

	if (GetDlgItem(IDOK) != 0)
	{
		m_Resize.Add(this, IDOK, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);
	}

	if (GetDlgItem(IDHELP) != 0)
	{
		m_Resize.Add(this, IDHELP, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);
	}

	if (GetDlgItem(IDCANCEL) != 0)
	{
		m_Resize.Add(this, IDCANCEL, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT);
	}

	if (GetDlgItem(AFX_IDC_TAB_CONTROL) != 0)
	{
		m_Resize.Add(this, AFX_IDC_TAB_CONTROL, RESIZE_LOCKALL);
	}

	// Undocumented stuff, resize the two lines in wizard mode
	// Found the IDs by using the Spy++ utility
	if (GetDlgItem(0x3026) != 0)
	{
		m_Resize.Add(this, 0x3026, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT|RESIZE_LOCKLEFT);
	}
	if (GetDlgItem(0x3027) != 0)
	{
		m_Resize.Add(this, 0x3027, RESIZE_LOCKBOTTOM|RESIZE_LOCKRIGHT|RESIZE_LOCKLEFT);
	}

	// Set the resizable border
	ModifyStyle(0, WS_THICKFRAME);
	return TRUE; 
}

void CResizablePropertySheet::GetPageRect(RECT *pRect)
{
	CRect rcClient;
	GetClientRect(rcClient);

	// Set the page rect and adjust using the original page size
	pRect->left   = m_rcPage.left;
	pRect->top    = m_rcPage.top;
	pRect->right  = rcClient.right - (m_rcClient.right - m_rcPage.right);
	pRect->bottom = rcClient.bottom - (m_rcClient.bottom - m_rcPage.bottom);
}
#pragma endregion Public Methods

#pragma region Protected Methods
void CResizablePropertySheet::OnSize(UINT nType, int cx, int cy) 
{
	// Must repeatedly do this to keep the frame from restoring itself
	ModifyStyle(0, WS_THICKFRAME);

	CPropertySheet::OnSize(nType, cx, cy);

	// Make sure to erase the previous gripper position
	InvalidateRect(m_rcGripper, TRUE);

	if (GetTabControl())
	{
		CRect rcClient;
		GetClientRect(rcClient);

		CRect rcPage;
		GetPageRect(rcPage);

		// Resize all pages created 
		for (INT i = 0; i < GetPageCount(); i++)
		{
			CPropertyPage *page = GetPage(i);

			if (IsWindow(page->GetSafeHwnd()))
			{
				page->MoveWindow(rcPage);
			}
		}

		// Resize other controls
		m_Resize.Resize(this);
	}
	
}

LRESULT CResizablePropertySheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// In wizard mode, it seems like the tab control does not send notification
	// messages (Spy++ says it is disabled) when a page is activated. 
	// Therefore this is done to resize the page when it becomes active.
	// Is there a better way?
	if (message == WM_COMMAND)
	{
		UINT nNotifyCode = HIWORD(wParam); // notification code 
		UINT nID = LOWORD(wParam);         // item, control, or accelerator identifier 
		HWND hwndCtl = (HWND)lParam;       // handle of control 

		if (nNotifyCode == BN_CLICKED && (nID == ID_WIZNEXT || nID == ID_WIZBACK || nID == ID_WIZFINISH)) 
		{
			LRESULT result = CPropertySheet::WindowProc(message, wParam, lParam);

			// Make sure to resize it
			CRect rcPage;
			GetPageRect(rcPage);
			GetActivePage()->MoveWindow(rcPage);

			return result;
		}
	}
	
	return CPropertySheet::WindowProc(message, wParam, lParam);
}

void CResizablePropertySheet::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	CRect rc = m_rcClient;

	// Adjust it according to the window style
	AdjustWindowRect(rc, GetStyle(), FALSE);

	// Make sure it does not get smaller than the initial size
	lpMMI->ptMinTrackSize.x = rc.Width();
	lpMMI->ptMinTrackSize.y = rc.Height();

	CPropertySheet::OnGetMinMaxInfo(lpMMI);
}

void CResizablePropertySheet::OnPaint()
{
	CPaintDC dc(this);

	CRect rect;
	GetClientRect(&rect);

	// Get the standard size of the gripper
	rect.left = rect.right-GetSystemMetrics(SM_CXHSCROLL);
	rect.top  = rect.bottom-GetSystemMetrics(SM_CYVSCROLL);

	// Draw it
	dc.DrawFrameControl(&rect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

	// Save the painted rect so we can invalidate the rect on next OnSize()
	m_rcGripper = rect;
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ResizablePropertySheet.cpp_v  $
 * 
 *    Rev 1.0   17 Jul 2014 11:16:26   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
