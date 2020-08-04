//******************************************************************************
//* COPYRIGHT (c) 2008, 2009 by Magnus Egelberg
//* All Rights Reserved 
//* Author           : Magnus Egelberg (magnus.egelberg@lundalogik.se) Much of this code comes from Eli Vingot
//* Licenses         : No known license
//* From             : http://www.codeguru.com/cpp/controls/propertysheet/sizing/article.php/c599/Resizable-CPropertySheet.htm
//******************************************************************************
//* .Module Name     : CDlgItemResizer
//* .File Name       : $Workfile:   DlgItemResizer.cpp  $
//* .Description	 : The class resizes the registered controls in a dialog 
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:12:18  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "DlgItemResizer.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This struct is used to hold information
// about the added controls
struct DLGITEMINFO
{
	UINT m_uFlags;
	HWND m_hWnd;
	CRect m_rectPosition;
};
#pragma endregion Declarations

namespace SvMc
{
#pragma region Constructor
	DlgItemResizer::DlgItemResizer() :
		m_szInitial(CSize(0, 0))
	{
	}

	DlgItemResizer::~DlgItemResizer()
	{
		// Make sure to delete all allocated items
		for (INT i = 0; i < m_Controls.GetSize(); i++)
		{
			delete (static_cast<DLGITEMINFO*>  (m_Controls[i]));
		}
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void DlgItemResizer::Add(CWnd *pCtrl, UINT uFlags)
	{
		// Make sure params are valid
		assert(pCtrl != 0);
		assert(pCtrl->GetParent() != 0);

		CWnd *pParent = pCtrl->GetParent();

		if (uFlags == 0)
		{
			return;
		}

		CRect rect;
		pCtrl->GetWindowRect(rect);
		pParent->ScreenToClient(rect);

		DLGITEMINFO *item    = new DLGITEMINFO;
		item->m_uFlags       = uFlags;
		item->m_hWnd         = pCtrl->m_hWnd;
		item->m_rectPosition = rect;

		// Set initial size if not set
		if (0 == m_szInitial.cx || 0 == m_szInitial.cy) 
		{
			pParent->GetClientRect(rect);
			m_szInitial = rect.Size();
		}

		// Add it to the array
		m_Controls.Add(item);
	}

	void DlgItemResizer::Resize(CWnd *pWnd)
	{
		// Just return if no initial size yet
		if (0 == m_szInitial.cx || 0 == m_szInitial.cy) return;

		// Don't bother to resize minimized windows
		if (GetWindowLong(pWnd->m_hWnd, GWL_STYLE) & WS_MINIMIZE) return;

		CRect client;
		pWnd->GetClientRect(client);

		for (INT i = 0; i < m_Controls.GetSize(); i++)
		{
			DLGITEMINFO *item = (DLGITEMINFO *)m_Controls[i];
			CWnd *ctrl = CWnd::FromHandle(item->m_hWnd);

			// Invalidate the old position
			CRect rect;
			ctrl->GetWindowRect(rect);  
			pWnd->ScreenToClient(rect);
			pWnd->InvalidateRect(rect);

			// Get the current size of the control
			CSize size = rect.Size();

			// Set the new position according to the flags specified
			if ((item->m_uFlags & (RESIZE_LOCKLEFT|RESIZE_LOCKRIGHT)) == (RESIZE_LOCKLEFT|RESIZE_LOCKRIGHT))
			{
				rect.left = item->m_rectPosition.left;
				rect.right = client.right - (m_szInitial.cx - item->m_rectPosition.right);
			}
			else if (item->m_uFlags & RESIZE_LOCKRIGHT)
			{
				rect.right = client.right - (m_szInitial.cx - item->m_rectPosition.right);
				rect.left  = rect.right - size.cx;
			}

			else if (item->m_uFlags & RESIZE_LOCKLEFT)
			{
				rect.left  = item->m_rectPosition.left;
				rect.right = rect.left + size.cx;
			}

			if ((item->m_uFlags & (RESIZE_LOCKTOP|RESIZE_LOCKBOTTOM)) == (RESIZE_LOCKTOP|RESIZE_LOCKBOTTOM))
			{
				rect.top = item->m_rectPosition.top;
				rect.bottom = client.bottom - (m_szInitial.cy - item->m_rectPosition.bottom);
			}
			else if (item->m_uFlags & RESIZE_LOCKTOP)
			{
				rect.top = item->m_rectPosition.top;
				rect.bottom = rect.top + size.cy;
			}
			else if (item->m_uFlags & RESIZE_LOCKBOTTOM)
			{
				rect.bottom = client.bottom - (m_szInitial.cy - item->m_rectPosition.bottom);
				rect.top = rect.bottom - size.cy;
			}

			// Check if control is completely inside client, hide if not
			// Do this only when the RESIZE_SHOWHIDE flag is set.
			if (item->m_uFlags & RESIZE_SHOWHIDE) 
			{
				CRect unionrect;
				unionrect.UnionRect(rect, client);

				if (unionrect != client)
				{
					ctrl->ShowWindow(SW_HIDE);
				}
				else
				{
					// Make sure it is visible
					if (!(ctrl->GetStyle() & WS_VISIBLE))
					{
						ctrl->ShowWindow(SW_SHOWNORMAL);
					}
					ctrl->MoveWindow(rect);
				}
			}
			else
			{
				ctrl->MoveWindow(rect);
			}
		}
	}
	#pragma endregion Public Methods
} //namespace SvMc
