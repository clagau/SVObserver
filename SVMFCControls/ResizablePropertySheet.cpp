//******************************************************************************
//* COPYRIGHT (c) 2008, 2009 by Magnus Egelberg
//* All Rights Reserved 
//* Author           : Magnus Egelberg (magnus.egelberg@lundalogik.se)
//* Licenses         : No known license
//* From             : http://www.codeguru.com/cpp/controls/propertysheet/sizing/article.php/c599/Resizable-CPropertySheet.htm
//******************************************************************************
//* .Module Name     : ResizablePropertySheet
//* .File Name       : $Workfile:   ResizablePropertySheet.cpp  $
//* .Description	 : The class resizes the registered controls in a dialog 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Aug 2014 07:49:04  $
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
	IMPLEMENT_DYNAMIC(ResizablePropertySheet, CPropertySheet)

	BEGIN_MESSAGE_MAP(ResizablePropertySheet, CPropertySheet)
		//{{AFX_MSG_MAP(ResizablePropertySheet)
		ON_WM_SIZE()
		ON_WM_GETMINMAXINFO()
		ON_WM_PAINT()
		//}}AFX_MSG_MAP
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

		return TRUE; 
	}

	void ResizablePropertySheet::GetPageRect(RECT *pRect)
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
	void ResizablePropertySheet::OnSize(UINT nType, int cx, int cy) 
	{
		CPropertySheet::OnSize(nType, cx, cy);

		// Make sure to erase the previous gripper position
		InvalidateRect(m_rcGripper, TRUE);

		if (GetTabControl())
		{
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

	LRESULT ResizablePropertySheet::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
	{
		// In wizard mode, it seems like the tab control does not send notification
		// messages (Spy++ says it is disabled) when a page is activated. 
		// Therefore this is done to resize the page when it becomes active.
		// Is there a better way?
		if (message == WM_COMMAND)
		{
			UINT nNotifyCode = HIWORD(wParam); // notification code 
			UINT nID = LOWORD(wParam);         // item, control, or accelerator identifier 

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

	void ResizablePropertySheet::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
	{
		CRect rc = m_rcClient;

		// Adjust it according to the window style
		AdjustWindowRect(rc, GetStyle(), FALSE);

		// Make sure it does not get smaller than the initial size
		lpMMI->ptMinTrackSize.x = rc.Width();
		lpMMI->ptMinTrackSize.y = rc.Height();

		CPropertySheet::OnGetMinMaxInfo(lpMMI);
	}

	void ResizablePropertySheet::OnPaint()
	{
		CPaintDC dc(this);

		CRect rect;
		GetClientRect(&rect);

		// Get the standard size of the gripper
		rect.left = rect.right - ::GetSystemMetrics(SM_CXHSCROLL);
		rect.top  = rect.bottom - ::GetSystemMetrics(SM_CYVSCROLL);

		// Draw it
		dc.DrawFrameControl(&rect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

		// Save the painted rect so we can invalidate the rect on next OnSize()
		m_rcGripper = rect;
	}

	INT_PTR ResizablePropertySheet::DoModal()
	{
		// Hook into property sheet creation code
		m_psh.dwFlags |= PSH_USECALLBACK;
		m_psh.pfnCallback = ResizePropSheetCallback;

		return CPropertySheet::DoModal();
	}

	int CALLBACK ResizablePropertySheet::ResizePropSheetCallback(HWND hWnd, UINT message, LPARAM lParam)
	{
		extern int CALLBACK AfxPropSheetCallback(HWND, UINT message, LPARAM lParam);
		// XMN: Call MFC's callback
		int nRes = AfxPropSheetCallback(hWnd, message, lParam);

		switch (message)
		{
		case PSCB_PRECREATE:
			( reinterpret_cast<LPDLGTEMPLATE> (lParam) )->style |= (DS_3DLOOK | DS_SETFONT | WS_THICKFRAME | WS_POPUP | WS_VISIBLE | WS_CAPTION);
			break;
		}

		return nRes;
	}
	#pragma endregion Protected Methods
} //namespace SvMc

