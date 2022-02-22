//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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
					IterChild->second->m_LeafItem =  Item;
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
					IterChild->second->m_LeafItem =  nullptr;
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
				if( IterChild->second->isLeaf() && (nullptr != IterChild->second->m_LeafItem) )
				{
					SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState( SvCl::ObjectSelectorItem::EmptyEnabled );
					//The checked state is saved in the upper nibble of the item state (Filtered with 0xF000) and must be shifted by 12 to get the required value
					CheckedState = static_cast<SvCl::ObjectSelectorItem::CheckedStateEnum> (GetItemState(IterChild->second->m_LeafItem, TVIS_STATEIMAGEMASK)>>12);
					//Check if state has changed
					if( IterChild->second->m_CheckedState != CheckedState )
					{
						SetItemState( IterChild->second->m_LeafItem, INDEXTOSTATEIMAGEMASK( IterChild->second->m_CheckedState ),  TVIS_STATEIMAGEMASK );
					}
				}
				++IterChild;
			}
		}
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

	void LeafTreeCtrl::showContextMenu(bool)
	{
		//If single selection do not show context menu
		if (isSingleSelect())
		{
			return;
		}

		CMenu Menu;
		CMenu* pPopupMenu {nullptr};

		UINT MenuResource = 0;

		MenuResource = IDR_TREE_ALL_VALUES_MENU;

		if (0 != MenuResource && Menu.LoadMenu(MenuResource))
		{
			if (nullptr != (pPopupMenu = Menu.GetSubMenu(0)))
			{
				CPoint Point = getContextPoint();
				ClientToScreen(&Point);
				pPopupMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this);
			}
		}
	}

	bool LeafTreeCtrl::isCheckable() const
	{
		//Can always check item
		bool Result(true);

		return Result;
	}

	bool LeafTreeCtrl::setCheckState(const TreeItemSet& rParentItems, SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState)
	{
		TreeItemSet::iterator ParentIter = rParentItems.begin();

		while (rParentItems.end() != ParentIter)
		{
			if (nullptr == *ParentIter)
			{
				return false;
			}

			std::string* pLocation = reinterpret_cast<std::string*>(GetItemData(*ParentIter));
			if (nullptr == pLocation)
			{
				return false;
			}

			SvCl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem(*pLocation);
			if (getParentPropPage().getTreeContainer().end() != Iter)
			{
				//If no defined state then we want to toggle the state
				if (SvCl::ObjectSelectorItem::EmptyEnabled == CheckedState)
				{
					switch (Iter->second->m_CheckedState)
					{
						case SvCl::ObjectSelectorItem::UncheckedEnabled:
							CheckedState = SvCl::ObjectSelectorItem::CheckedEnabled;
							break;
						case SvCl::ObjectSelectorItem::CheckedEnabled:
						case SvCl::ObjectSelectorItem::TriStateEnabled:
							CheckedState = SvCl::ObjectSelectorItem::UncheckedEnabled;
							break;
						case SvCl::ObjectSelectorItem::UncheckedDisabled:
							CheckedState = SvCl::ObjectSelectorItem::CheckedDisabled;
							break;
						case SvCl::ObjectSelectorItem::CheckedDisabled:
						case SvCl::ObjectSelectorItem::TriStateDisabled:
							CheckedState = SvCl::ObjectSelectorItem::UncheckedDisabled;
							break;
						default:
							break;
					}
				}

				Iter->second->m_CheckedState = CheckedState;
				SetItemState(*ParentIter, INDEXTOSTATEIMAGEMASK(CheckedState), TVIS_STATEIMAGEMASK);
				getUpdateItems().insert(Iter->first);

				SvDef::StringSet updateItems = getParentPropPage().getTreeContainer().setParentState(Iter);
				getUpdateItems().insert(updateItems.begin(), updateItems.end());
			}
			++ParentIter;
		}

		getParentPropPage().updateData(this);

		return true;
	}
#pragma endregion Protected Methods

} //namespace SvOsl
