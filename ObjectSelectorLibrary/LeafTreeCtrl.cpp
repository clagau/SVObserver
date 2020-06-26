//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LeafTreeCtrl
//* .File Name       : $Workfile:   LeafTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   04 Dec 2014 03:10:58  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LeafTreeCtrl.h"
#include "ObjectSelectorPpg.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace SvOsl
{
	BEGIN_MESSAGE_MAP(LeafTreeCtrl, ObjectTreeCtrl)
		//{{AFX_MSG_MAP(NodeTreeCtrl)
		ON_COMMAND( ID_TREE_CHECKALL, LeafTreeCtrl::OnCheckAll )
		ON_COMMAND( ID_TREE_UNCHECKALL, LeafTreeCtrl::OnUncheckAll )
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	#pragma region Constructor
	LeafTreeCtrl::LeafTreeCtrl(  ObjectSelectorPpg& rParent, bool SingleSelect )
		: ObjectTreeCtrl( rParent, SingleSelect )
	{
		setLeftButtonCheckFlag( TVHT_ONITEM );
	}

	LeafTreeCtrl::~LeafTreeCtrl()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void LeafTreeCtrl::changeSelectedItem( const HTREEITEM&  )
	{
	}

	void LeafTreeCtrl::loadTree()
	{
		SetRedraw(FALSE);

		DeleteAllItems();
	
		m_NodeLocation = getParentPropPage().getHighlightedNode();
		if( m_NodeLocation.empty() )
		{
			SetRedraw(TRUE);
			return;
		}

		SvCl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

		if( getParentPropPage().getTreeContainer().end() != Iter )
		{
			SvCl::ObjectTreeItems::iterator IterChild = Iter.node()->begin();
			while( Iter.node()->end() != IterChild )
			{
				if( IterChild->second->isLeaf() )
				{

					HTREEITEM Item;

					Item = InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_STATE | TVIF_PARAM, 
						IterChild->second->m_Name.c_str(),
						IterChild->second->m_IconNumber,
						IterChild->second->m_IconNumber,
						0,
						0,
						reinterpret_cast<LPARAM> (&IterChild->first),
						nullptr,
						TVI_LAST);

					SetItemState( Item, INDEXTOSTATEIMAGEMASK( IterChild->second->m_CheckedState ),  TVIS_STATEIMAGEMASK );
					IterChild->second->m_TreeItem =  Item;
				}
				++IterChild;
			}
		}

		SetRedraw(TRUE);
	}

	void LeafTreeCtrl::updateTree()
	{
		std::string Location = getParentPropPage().getHighlightedNode();

		if( m_NodeLocation != Location  )
		{
			//First reset the tree items
			SvCl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

			if( getParentPropPage().getTreeContainer().end() != Iter )
			{
				SvCl::ObjectTreeItems::iterator IterChild = Iter.node()->begin();
				while( Iter.node()->end() != IterChild )
				{
					if( IterChild->second->isLeaf() )
					{
						IterChild->second->m_TreeItem =  nullptr;
					}

					++IterChild;
				}
			}

			loadTree();

			return;
		}
		else if( Location.empty() )
		{
			return;
		}

		SvCl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

		if( getParentPropPage().getTreeContainer().end() != Iter )
		{
			SvCl::ObjectTreeItems::iterator IterChild = Iter.node()->begin();
			while( Iter.node()->end() != IterChild )
			{
				if( IterChild->second->isLeaf() && (nullptr != IterChild->second->m_TreeItem) )
				{
					SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState( SvCl::ObjectSelectorItem::EmptyEnabled );
					//The checked state is saved in the upper nibble of the item state (Filtered with 0xF000) and must be shifted by 12 to get the required value
					CheckedState = static_cast<SvCl::ObjectSelectorItem::CheckedStateEnum> (GetItemState(IterChild->second->m_TreeItem, TVIS_STATEIMAGEMASK)>>12);
					//Check if state has changed
					if( IterChild->second->m_CheckedState != CheckedState )
					{
						SetItemState( IterChild->second->m_TreeItem, INDEXTOSTATEIMAGEMASK( IterChild->second->m_CheckedState ),  TVIS_STATEIMAGEMASK );
					}
				}
				++IterChild;
			}
		}
	}

	void LeafTreeCtrl::showContextMenu( bool)
	{
		//If single selection do not show context menu
		if ( isSingleSelect() )
		{
			return;
		}

		CMenu Menu;
		CMenu* pPopupMenu{nullptr};

		UINT MenuResource = 0;

		MenuResource = IDR_TREE_ALL_VALUES_MENU;

		if( 0 != MenuResource && Menu.LoadMenu( MenuResource ) )
		{
			if(nullptr != (pPopupMenu = Menu.GetSubMenu(0)))
			{
				CPoint Point = getContextPoint();
				ClientToScreen( &Point );
				pPopupMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this );
			}
		}
	}

	bool LeafTreeCtrl::isCheckable() const
	{
		//Can always check item
		bool Result(true);

		return Result;
	}
	#pragma endregion Public Methods

	#pragma region Protected Methods
	void LeafTreeCtrl::OnCheckAll()
	{
		TreeItemSet Items;

		getRootItems( Items );

		setCheckState( Items, SvCl::ObjectSelectorItem::CheckedEnabled );
	}

	void LeafTreeCtrl::OnUncheckAll()
	{
		TreeItemSet Items;

		getRootItems( Items );

		setCheckState( Items, SvCl::ObjectSelectorItem::UncheckedEnabled );
	}
	#pragma endregion Protected Methods

} //namespace SvOsl
