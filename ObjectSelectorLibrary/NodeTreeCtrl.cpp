//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : NodeTreeCtrl
//* .File Name       : $Workfile:   NodeTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   18 Dec 2014 01:48:38  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "NodeTreeCtrl.h"
#include "ObjectSelectorPpg.h"
#include "Definitions/StringTypeDef.h"
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
	BEGIN_MESSAGE_MAP(NodeTreeCtrl, ObjectTreeCtrl)
		//{{AFX_MSG_MAP(NodeTreeCtrl)
		ON_COMMAND( ID_TREE_EXPANDALL, NodeTreeCtrl::OnExpandAll )
		ON_COMMAND( ID_TREE_EXPANDTOCHECKEDITEMS, NodeTreeCtrl::OnExpandToCheckedItems )
		ON_COMMAND( ID_TREE_COLLAPSEALL, NodeTreeCtrl::OnCollapseAll )
		ON_COMMAND( ID_TREE_COLLAPSETO2NDLEVEL, NodeTreeCtrl::OnCollapseTo2ndLevel )
		ON_COMMAND( ID_TREE_CHECKALL, NodeTreeCtrl::OnCheckAll )
		ON_COMMAND( ID_TREE_UNCHECKALL, NodeTreeCtrl::OnUncheckAll )
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	#pragma region Constructor
	NodeTreeCtrl::NodeTreeCtrl( ObjectSelectorPpg& rParent, bool SingleSelect )
		: ObjectTreeCtrl( rParent, SingleSelect )
	{
		if( SingleSelect )
		{
			setLeftButtonCheckFlag( TVHT_ONITEM );
		}
	}

	NodeTreeCtrl::~NodeTreeCtrl()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void NodeTreeCtrl::loadTree()
	{
		SetRedraw(FALSE);

		DeleteAllItems();

		HTREEITEM ParentItem = nullptr;
		TreeItemSet CurrentItems;

		SvCl::ObjectTreeItems::pre_order_iterator Iter( getParentPropPage().getTreeContainer().pre_order_begin() );

		while( getParentPropPage().getTreeContainer().pre_order_end() != Iter )
		{
			if( Iter.node()->is_root() )
			{
				++Iter;
				continue;
			}
			if(nullptr != Iter.node()->parent())
			{
				ParentItem = Iter.node()->parent()->get()->second->m_TreeItem;
			}
			if( Iter->second->isNode() )
			{
				HTREEITEM Item = nullptr;
				Item = InsertItem( TVIF_TEXT | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM, 
					Iter->second->m_Name.c_str(),
					Iter->second->m_IconNumber,
					Iter->second->m_IconNumber,
					0,
					0,
					reinterpret_cast<LPARAM> (&Iter->first),
					ParentItem,
					TVI_LAST );

				SetItemState( Item, INDEXTOSTATEIMAGEMASK( Iter->second->m_CheckedState ), TVIS_STATEIMAGEMASK );
				Iter->second->m_TreeItem =  Item;
			}
			//On initial load check if leaf value selected when in single object selection mode
			if( isSingleSelect() && Iter->second->isLeaf() && SvCl::ObjectSelectorItem::CheckedEnabled == Iter->second->m_CheckedState)
			{
				setCurrentSelection( Iter->second->m_Location );
				CurrentItems.insert( ParentItem );
			}
			++Iter;
		}
		//Expand to checked items if none then expand to 2nd Level items
		if( !ExpandToCheckedItems() )
		{
			OnCollapseTo2ndLevel();
		}

		selectFirstItem( CurrentItems );

		SetRedraw(TRUE);
	}

	void NodeTreeCtrl::updateTree()
	{
		SvDef::StringSet& rUpdateItems = getUpdateItems();
		SvCl::ObjectTreeItems& rTreeItems = getParentPropPage().getTreeContainer();

		SvDef::StringSet::const_iterator IterName = rUpdateItems.begin();

		while( rUpdateItems.end() != IterName )
		{
			SvCl::ObjectTreeItems::iterator Iter( rTreeItems.findItem( *IterName ) );

			if( rTreeItems.end() != Iter )
			{
				UpdateNode(*Iter->second);
			}
			++IterName;
		}

		rUpdateItems.clear();
	}

	void NodeTreeCtrl::UpdateAllNodes()
	{
		SvCl::ObjectTreeItems::pre_order_iterator Iter( getParentPropPage().getTreeContainer().pre_order_begin() );

		while( getParentPropPage().getTreeContainer().pre_order_end() != Iter )
		{
			if( Iter.node()->is_root() )
			{
				++Iter;
				continue;
			}

			UpdateNode(*Iter->second);

			if( isSingleSelect() && Iter->second->isLeaf() && SvCl::ObjectSelectorItem::CheckedEnabled == Iter->second->m_CheckedState )
			{
				setCurrentSelection( Iter->second->m_Location );
			}
			++Iter;
		}
	}
	#pragma endregion Public Methods

	#pragma region Protected Methods
	void NodeTreeCtrl::OnExpandAll()
	{
		HTREEITEM Item = checkItemHit( getContextPoint(), TVHT_ONITEM );
		TreeItemSet Items;

		if( nullptr != Item )
		{
			Items.insert( Item );
		}
		else
		{
			//All root items
			getRootItems( Items );
		}

		expandAll( Items, TVE_EXPAND );

		selectFirstItem( Items );
	}

	void NodeTreeCtrl::OnExpandToCheckedItems()
	{
		ExpandToCheckedItems();
	}

	void NodeTreeCtrl::OnCollapseAll()
	{
		HTREEITEM Item = checkItemHit( getContextPoint(), TVHT_ONITEM );
		TreeItemSet Items;

		if( nullptr != Item )
		{
			Items.insert( Item );
		}
		else
		{
			//All root items
			getRootItems( Items );
		}

		expandAll( Items, TVE_COLLAPSE );

		selectFirstItem( Items );
	}

	void NodeTreeCtrl::OnCollapseTo2ndLevel()
	{
		//First collapse all
		TreeItemSet Items;
		getRootItems( Items );
		expandAll( Items, TVE_COLLAPSE );

		TreeItemSet::iterator Iter = Items.begin();

		while( Items.end() != Iter )
		{
			Expand( *Iter, TVE_EXPAND );

			++Iter;
		}

		selectFirstItem( Items );
	}

	void NodeTreeCtrl::OnCheckAll()
	{
		TreeItemSet Items;

		HTREEITEM Item = checkItemHit( getContextPoint(), TVHT_ONITEM );

		if( nullptr != Item )
		{
			Items.insert( Item );
		}
		else
		{
			//All root items
			getRootItems( Items );
		}

		setCheckState( Items, SvCl::ObjectSelectorItem::CheckedEnabled );
	}

	void NodeTreeCtrl::OnUncheckAll()
	{
		TreeItemSet Items;

		HTREEITEM Item = checkItemHit( getContextPoint(), TVHT_ONITEM );

		if( nullptr != Item )
		{
			Items.insert( Item );
		}
		else
		{
			//All root items
			getRootItems( Items );
		}

		setCheckState( Items, SvCl::ObjectSelectorItem::UncheckedEnabled );
	}

	void NodeTreeCtrl::changeSelectedItem( const HTREEITEM& rItem )
	{
		if( nullptr != rItem )
		{
			std::string* pLocation = reinterpret_cast<std::string*> ( GetItemData( rItem ) );
			if( nullptr != pLocation )
			{
				getParentPropPage().setHighlightedNode( *pLocation );
				getParentPropPage().updateData( this );

			}
		}
		else
		{
			//Set to no selection
			std::string Location;
			getParentPropPage().setHighlightedNode( Location );
		}
	}

	void NodeTreeCtrl::showContextMenu( bool ItemHit )
	{
		CMenu Menu;
		CMenu* pPopupMenu{nullptr};

		UINT MenuResource = 0;

		if( ItemHit )
		{
			MenuResource = IDR_TREE_ALL_SUB_NODES_MENU;
		}
		else
		{
			MenuResource = IDR_TREE_ALL_TREE_MENU;
		}

		if( 0 != MenuResource && Menu.LoadMenu( MenuResource ) )
		{
			if(nullptr != (pPopupMenu = Menu.GetSubMenu(0)))
			{
				//Single select remove the check all command
				if( isSingleSelect() )
				{
					for (int i = 0; i < pPopupMenu->GetMenuItemCount(); i++)
					{
						if ( ID_TREE_CHECKALL == pPopupMenu->GetMenuItemID(i) )
						{
							//Check all menu
							pPopupMenu->RemoveMenu( i, MF_BYPOSITION );
							//Remove separator it is at the previous check all position
							if( i < pPopupMenu->GetMenuItemCount() )
							{
								pPopupMenu->RemoveMenu( i, MF_BYPOSITION );
							}
							break;
						}
					}
				}

				CPoint Point = getContextPoint();
				ClientToScreen( &Point );
				pPopupMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON, Point.x, Point.y, this );
			}
		}
	}

	bool NodeTreeCtrl::isCheckable() const
	{
		bool Result = !isSingleSelect();
		return Result;
	}

	bool NodeTreeCtrl::ExpandToCheckedItems()
	{
		bool Result( false );
		//First collapse all
		TreeItemSet Items;
		getRootItems( Items );
		expandAll( Items, TVE_COLLAPSE );

		SvCl::ObjectTreeItems::pre_order_iterator Iter( getParentPropPage().getTreeContainer().pre_order_begin() );

		while( getParentPropPage().getTreeContainer().pre_order_end() != Iter )
		{
			if( Iter->second->isNode() )
			{
				SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState = Iter->second->m_CheckedState;
				bool Checked =  SvCl::ObjectSelectorItem::CheckedEnabled == CheckedState || SvCl::ObjectSelectorItem::CheckedDisabled == CheckedState;
				bool Tristate = SvCl::ObjectSelectorItem::TriStateEnabled == CheckedState || SvCl::ObjectSelectorItem::TriStateDisabled == CheckedState;
				if( Checked || Tristate )
				{
					Expand( Iter->second->m_TreeItem, TVE_EXPAND );
					Result = true;
				}
			}

			++Iter;
		}

		selectFirstItem( Items );

		return Result;
	}

	void NodeTreeCtrl::UpdateNode( SvCl::ObjectSelectorItem& rItem )
	{
		bool isNode = rItem.isNode();
		const HTREEITEM treeItem = rItem.m_TreeItem;

		if( isNode && ( nullptr != treeItem ) )
		{
			SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState = static_cast<SvCl::ObjectSelectorItem::CheckedStateEnum>(GetItemState(treeItem, TVIS_STATEIMAGEMASK)>>12);
			SvCl::ObjectSelectorItem::CheckedStateEnum itemCheckedState = rItem.m_CheckedState;
			//Check if state has changed
			if( itemCheckedState != CheckedState )
			{
				SetItemState( treeItem, INDEXTOSTATEIMAGEMASK( itemCheckedState ), TVIS_STATEIMAGEMASK );
			}
		}
	}
	#pragma endregion Protected Methods

} //namespace SvOsl
