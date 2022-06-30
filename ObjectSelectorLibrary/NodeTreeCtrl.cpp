//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
				ParentItem = Iter.node()->parent()->get()->second->m_NodeItem;
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

				SetItemState( Item, INDEXTOSTATEIMAGEMASK(Iter->second->m_NodeState), TVIS_STATEIMAGEMASK );
				Iter->second->m_NodeItem =  Item;
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
			CMenu* pPopupMenu {Menu.GetSubMenu(0)};
			if(nullptr != pPopupMenu)
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

	bool NodeTreeCtrl::setCheckState(const TreeItemSet& rParentItems, SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState)
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
					switch (Iter->second->m_NodeState)
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

				Iter->second->m_NodeState = CheckedState;
				SetItemState(*ParentIter, INDEXTOSTATEIMAGEMASK(CheckedState), TVIS_STATEIMAGEMASK);
				getUpdateItems().insert(Iter->first);

				SvDef::StringSet updateItems = getParentPropPage().getTreeContainer().setParentState(Iter);
				getUpdateItems().insert(updateItems.begin(), updateItems.end());
				if (Iter->second->isNode())
				{
					setChildrenState(Iter, CheckedState);
				}
			}
			++ParentIter;
		}

		getParentPropPage().updateData(this);

		return true;
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
				SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState = Iter->second->m_NodeState;
				bool Checked =  SvCl::ObjectSelectorItem::CheckedEnabled == CheckedState || SvCl::ObjectSelectorItem::CheckedDisabled == CheckedState;
				bool Tristate = SvCl::ObjectSelectorItem::TriStateEnabled == CheckedState || SvCl::ObjectSelectorItem::TriStateDisabled == CheckedState;
				if( Checked || Tristate )
				{
					Expand( Iter->second->m_NodeItem, TVE_EXPAND );
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
		const HTREEITEM treeItem = rItem.m_NodeItem;

		if( isNode && ( nullptr != treeItem ) )
		{
			SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState = static_cast<SvCl::ObjectSelectorItem::CheckedStateEnum>(GetItemState(treeItem, TVIS_STATEIMAGEMASK)>>12);
			//Check if state has changed
			if(rItem.m_NodeState != CheckedState )
			{
				SetItemState( treeItem, INDEXTOSTATEIMAGEMASK(rItem.m_NodeState), TVIS_STATEIMAGEMASK );
			}
		}
	}
	#pragma endregion Protected Methods

} //namespace SvOsl
