//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTreeCtrl
//* .File Name       : $Workfile:   SVTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 08:24:42  $
//******************************************************************************

#include "stdafx.h"
#include "SVOResource\resource.h"
#include "SVOResource\ConstGlobalSvOr.h"
#include "SVTreeCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Seidenader
{
	namespace SVMFCControls
	{
		IMPLEMENT_DYNCREATE(SVTreeCtrlClass, CTreeCtrl)

			SVTreeCtrlClass::SVTreeCtrlClass()
			: CTreeCtrl()
		{
		}

		SVTreeCtrlClass::~SVTreeCtrlClass()
		{
		}

		/////////////////////////////////////////////////////////////////////////////
		//
		BOOL SVTreeCtrlClass::InitTreeCtrl()
		{
			StateImageList.Create( SvOr::IconSize, SvOr::IconSize, ILC_COLOR | ILC_MASK, 3, 1 );

			// State image index 0 is not used
			StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_NOTEXIST ) );

			// State image index 1 is 'Not Checked'
			StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_NOTEXIST ) );  

			// State image index 2 is 'Checked'
			StateImageList.Add( AfxGetApp()->LoadIcon( IDI_ICON_EXIST ) );

			SetImageList( &StateImageList, TVSIL_STATE );

			return TRUE;
		}

		/////////////////////////////////////////////////////////////////////////////
		//
		BOOL SVTreeCtrlClass::SetCheckState(HTREEITEM hItem, BOOL fCheck)
		{
			TVITEM tvItem;

			tvItem.mask = TVIF_HANDLE | TVIF_STATE;
			tvItem.hItem = hItem;
			tvItem.stateMask = TVIS_STATEIMAGEMASK;

			// Since state images are one-based, 
			// 1 in this macro turns the check off, and 2 turns it on.
			tvItem.state = INDEXTOSTATEIMAGEMASK((fCheck ? 2 : 1));

			return SetItem( &tvItem );
		}

		/////////////////////////////////////////////////////////////////////////////
		//
		BOOL SVTreeCtrlClass::GetCheckState(HTREEITEM hItem)
		{
			TVITEM tvItem;

			// Prepare to receive the desired information.
			tvItem.mask = TVIF_HANDLE | TVIF_STATE;
			tvItem.hItem = hItem;
			tvItem.stateMask = TVIS_STATEIMAGEMASK;

			// Request the information.
			GetItem( &tvItem );

			// Return zero if it's not checked, or nonzero otherwise.
			return ((BOOL)(tvItem.state >> 12) -1);
		}

		BEGIN_MESSAGE_MAP(SVTreeCtrlClass, CTreeCtrl)
			//{{AFX_MSG_MAP(SVTreeCtrlClass)
			ON_WM_LBUTTONDOWN()
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()

		/////////////////////////////////////////////////////////////////////////////
		// SVTreeCtrlClass message handlers

		/////////////////////////////////////////////////////////////////////////////
		//
		void SVTreeCtrlClass::OnLButtonDown(UINT nFlags, CPoint point) 
		{
			UINT uFlags;

			HTREEITEM hItem = HitTest(point,&uFlags);

			if (hItem)
			{
				DWORD dwItemData = static_cast<DWORD>(GetItemData(hItem));
				if( dwItemData )
				{
					BOOL bCheckState = GetCheckState(hItem);

					BOOL bNewCheckState = bCheckState ? FALSE : TRUE;

					SetCheckState(hItem,bNewCheckState);
				}
			}

			CTreeCtrl::OnLButtonDown(nFlags, point);
		}
	} //SVMFCControls
} //Seidenader

