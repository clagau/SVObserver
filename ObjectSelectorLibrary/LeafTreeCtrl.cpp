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
using namespace Seidenader::ObjectSelectorLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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
void LeafTreeCtrl::changeSelectedItem( const HTREEITEM& rItem )
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

	SvTrl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

	if( getParentPropPage().getTreeContainer().end() != Iter )
	{
		SvTrl::ObjectTreeItems::iterator IterChild = Iter.node()->begin();
		while( Iter.node()->end() != IterChild )
		{
			if( IterChild->second->isLeaf() )
			{

				HTREEITEM Item;

				Item = InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_STATE | TVIF_PARAM, 
					IterChild->second->getName().c_str(),
					IterChild->second->getIconNumber(),
					IterChild->second->getIconNumber(),
					0,
					0,
					reinterpret_cast<LPARAM> (&IterChild->first),
					NULL,
					TVI_LAST);

				SetItemState( Item, INDEXTOSTATEIMAGEMASK( IterChild->second->getCheckedState() ),  TVIS_STATEIMAGEMASK );
				IterChild->second->setTreeItem( Item );
			}
			++IterChild;
		}
	}

	SetRedraw(TRUE);
}

void LeafTreeCtrl::updateTree()
{
	SVString Location = getParentPropPage().getHighlightedNode();

	if( m_NodeLocation != Location  )
	{
		//First reset the tree items
		SvTrl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

		if( getParentPropPage().getTreeContainer().end() != Iter )
		{
			SvTrl::ObjectTreeItems::iterator IterChild = Iter.node()->begin();
			while( Iter.node()->end() != IterChild )
			{
				if( IterChild->second->isLeaf() )
				{
					IterChild->second->setTreeItem( NULL );
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

	SvTrl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

	if( getParentPropPage().getTreeContainer().end() != Iter )
	{
		SvTrl::ObjectTreeItems::iterator IterChild = Iter.node()->begin();
		while( Iter.node()->end() != IterChild )
		{
			if( IterChild->second->isLeaf() && (NULL != IterChild->second->getTreeItem()) )
			{
				SvTrl::IObjectSelectorItem::CheckedStateEnum CheckedState( SvTrl::IObjectSelectorItem::EmptyEnabled );
				//The checked state is saved in the upper nibble of the item state (Filtered with 0xF000) and must be shifted by 12 to get the required value
				CheckedState = static_cast<SvTrl::IObjectSelectorItem::CheckedStateEnum> (GetItemState(IterChild->second->getTreeItem(), TVIS_STATEIMAGEMASK)>>12);
				//Check if state has changed
				if( IterChild->second->getCheckedState() != CheckedState )
				{
					SetItemState( IterChild->second->getTreeItem(), INDEXTOSTATEIMAGEMASK( IterChild->second->getCheckedState() ),  TVIS_STATEIMAGEMASK );
				}
			}
			++IterChild;
		}
	}
}

void LeafTreeCtrl::showContextMenu( bool ItemHit )
{
	//If single selection do not show context menu
	if ( isSingleSelect() )
	{
		return;
	}

	CMenu Menu;
	CMenu* pPopupMenu;

	UINT MenuResource = 0;

	MenuResource = IDR_TREE_ALL_VALUES_MENU;

	if( 0 != MenuResource && Menu.LoadMenu( MenuResource ) )
	{
		if( pPopupMenu = Menu.GetSubMenu( 0 ) )
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

	setCheckState( Items, SvTrl::IObjectSelectorItem::CheckedEnabled );
}

void LeafTreeCtrl::OnUncheckAll()
{
	TreeItemSet Items;

	getRootItems( Items );

	setCheckState( Items, SvTrl::IObjectSelectorItem::UncheckedEnabled );
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\LeafTreeCtrl.cpp_v  $
 * 
 *    Rev 1.4   04 Dec 2014 03:10:58   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Changed field name from Location to Highlighted Node
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Aug 2014 07:37:44   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added disabled checked states
 * Object Selector displays nodes disabled when in single select mode
 * Changed methods: UpdateTree, OnCheckAll, OnUnCheckAll
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Aug 2014 07:46:16   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Coding guidline changes using const object for get and is methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Aug 2014 12:24:12   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Toggle leaf value when the label is clicked
 * Changed methods: Constructor
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:12:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
