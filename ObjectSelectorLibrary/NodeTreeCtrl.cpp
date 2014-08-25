//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : NodeTreeCtrl
//* .File Name       : $Workfile:   NodeTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   25 Aug 2014 07:40:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "NodeTreeCtrl.h"
#include "ObjectSelectorPpg.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::ObjectSelectorLibrary;
using namespace Seidenader::SVTreeLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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
NodeTreeCtrl::NodeTreeCtrl(  ObjectSelectorPpg& rParent, bool SingleSelect  )
	: ObjectTreeCtrl( rParent, SingleSelect )
{
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

	HTREEITEM ParentItem = NULL;
	TreeItemSet CurrentItems;

	ObjectTreeItems::SVTree_pre_order_iterator Iter( getParentPropPage().getTreeContainer().pre_order_begin() );

	while( getParentPropPage().getTreeContainer().pre_order_end() != Iter )
	{
		if( Iter.node()->is_root() )
		{
			++Iter;
			continue;
		}
		if(NULL != Iter.node()->parent())
		{
			ParentItem = Iter.node()->parent()->get()->second.getTreeItem();
		}
		if( Iter->second.isNode() )
		{
			HTREEITEM Item = NULL;
			Item = InsertItem( TVIF_TEXT | TVIF_STATE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM, 
				Iter->second.getName().c_str(),
				Iter->second.getIconNumber(),
				Iter->second.getIconNumber(),
				0,
				0,
				reinterpret_cast<LPARAM> (&Iter->first),
				ParentItem,
				TVI_LAST );

			SetItemState( Item, INDEXTOSTATEIMAGEMASK( Iter->second.getCheckedState() ),  TVIS_STATEIMAGEMASK );
			Iter->second.setTreeItem( Item );
		}
		//On initial load check if leaf value selected when in single object selection mode
		if( isSingleSelect() && Iter->second.isLeaf() && IObjectSelectorItem::CheckedEnabled == Iter->second.getCheckedState() )
		{
			setCurrentSelection( Iter->second.getLocation() );
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
	SVStringSet::const_iterator IterName = getUpdateItems().begin();

	while( getUpdateItems().end()  != IterName )
	{
		ObjectTreeItems::iterator Iter;

		Iter = getParentPropPage().getTreeContainer().findItem( *IterName );
		if( getParentPropPage().getTreeContainer().end() != Iter )
		{
			if( Iter->second.isNode() && (NULL != Iter->second.getTreeItem()) )
			{
				IObjectSelectorItem::CheckedStateEnum CheckedState( IObjectSelectorItem::EmptyEnabled );
				CheckedState = static_cast<IObjectSelectorItem::CheckedStateEnum> (GetItemState(Iter->second.getTreeItem(), TVIS_STATEIMAGEMASK)>>12);
				//Check if state has changed
				if( Iter->second.getCheckedState() != CheckedState )
				{
					SetItemState( Iter->second.getTreeItem(), INDEXTOSTATEIMAGEMASK( Iter->second.getCheckedState() ),  TVIS_STATEIMAGEMASK );
				}
			}
		}
		++IterName;
	}

	getUpdateItems().clear();
}

#pragma endregion Public Methods

#pragma region Protected Methods
void NodeTreeCtrl::OnExpandAll()
{
	HTREEITEM Item = checkItemHit( getContextPoint(), TVHT_ONITEM );
	TreeItemSet Items;

	if( NULL != Item )
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

	if( NULL != Item )
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

	if( NULL != Item )
	{
		Items.insert( Item );
	}
	else
	{
		//All root items
		getRootItems( Items );
	}

	setCheckState( Items, IObjectSelectorItem::CheckedEnabled );
}

void NodeTreeCtrl::OnUncheckAll()
{
	TreeItemSet Items;

	HTREEITEM Item = checkItemHit( getContextPoint(), TVHT_ONITEM );

	if( NULL != Item )
	{
		Items.insert( Item );
	}
	else
	{
		//All root items
		getRootItems( Items );
	}

	setCheckState( Items, IObjectSelectorItem::UncheckedEnabled );
}

void NodeTreeCtrl::changeSelectedItem( const HTREEITEM& rItem )
{
	if( NULL != rItem )
	{
		SVString* pLocation = reinterpret_cast<SVString*> ( GetItemData( rItem ) );
		if( NULL != pLocation )
		{
			getParentPropPage().setNodeLocation( *pLocation );
			getParentPropPage().updateData( this );

		}
	}
	else
	{
		//Set to no selection
		SVString Location;
		getParentPropPage().setNodeLocation( Location );
	}
}

void NodeTreeCtrl::showContextMenu( bool ItemHit )
{
	CMenu Menu;
	CMenu* pPopupMenu;

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
		if( pPopupMenu = Menu.GetSubMenu( 0 ) )
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

	ObjectTreeItems::SVTree_pre_order_iterator Iter( getParentPropPage().getTreeContainer().pre_order_begin() );

	while( getParentPropPage().getTreeContainer().pre_order_end() != Iter )
	{
		if( Iter->second.isNode() )
		{
			IObjectSelectorItem::CheckedStateEnum CheckedState = Iter->second.getCheckedState();
			bool Checked =  IObjectSelectorItem::CheckedEnabled == CheckedState || IObjectSelectorItem::CheckedDisabled == CheckedState;
			bool Tristate = IObjectSelectorItem::TriStateEnabled == CheckedState || IObjectSelectorItem::TriStateDisabled == CheckedState;
			if( Checked || Tristate )
			{
				Expand( Iter->second.getTreeItem(), TVE_EXPAND );
				Result = true;
			}
		}

		++Iter;
	}

	selectFirstItem( Items );

	return Result;
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\NodeTreeCtrl.cpp_v  $
 * 
 *    Rev 1.2   25 Aug 2014 07:40:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added disabled checked states
 * Object Selector displays nodes disabled when in single select mode
 * Changed methods: loadTree, updateTree, OnCheckAll, OnUnCheckAll, ExpandToCheckedItems
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Aug 2014 07:46:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Coding guidline changes using const object for get and is methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:12:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
