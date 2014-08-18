//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LeafTreeCtrl
//* .File Name       : $Workfile:   LeafTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   18 Aug 2014 07:46:16  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "LeafTreeCtrl.h"
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
	
	m_NodeLocation = getParentPropPage().getNodeLocation();
	if( m_NodeLocation.empty() )
	{
		SetRedraw(TRUE);
		return;
	}

	ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

	if( getParentPropPage().getTreeContainer().end() != Iter )
	{
		ObjectTreeItems::iterator IterChild = Iter.GetChildTree()->begin();
		while( Iter.GetChildTree()->end() != IterChild )
		{
			if( IterChild->second.isLeaf() )
			{

				HTREEITEM Item;

				Item = InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_STATE | TVIF_PARAM, 
					IterChild->second.getName().c_str(),
					IterChild->second.getIconNumber(),
					IterChild->second.getIconNumber(),
					0,
					0,
					reinterpret_cast<LPARAM> (&IterChild->first),
					NULL,
					TVI_LAST);

				SetItemState( Item, INDEXTOSTATEIMAGEMASK( IterChild->second.getCheckedState() ),  TVIS_STATEIMAGEMASK );
				IterChild->second.setTreeItem( Item );
			}
			++IterChild;
		}
	}

	SetRedraw(TRUE);
}

void LeafTreeCtrl::updateTree()
{
	SVString Location = getParentPropPage().getNodeLocation();

	if( m_NodeLocation != Location  )
	{
		//First reset the tree items
		ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

		if( getParentPropPage().getTreeContainer().end() != Iter )
		{
			ObjectTreeItems::iterator IterChild = Iter.GetChildTree()->begin();
			while( Iter.GetChildTree()->end() != IterChild )
			{
				if( IterChild->second.isLeaf() )
				{
					IterChild->second.setTreeItem( NULL );
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

	ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( m_NodeLocation );

	if( getParentPropPage().getTreeContainer().end() != Iter )
	{
		ObjectTreeItems::iterator IterChild = Iter.GetChildTree()->begin();
		while( Iter.GetChildTree()->end() != IterChild )
		{
			if( IterChild->second.isLeaf() && (NULL != IterChild->second.getTreeItem()) )
			{
				IObjectSelectorItem::CheckedStateEnum CheckedState( IObjectSelectorItem::CheckedStateNone );
				CheckedState = static_cast<IObjectSelectorItem::CheckedStateEnum> (GetItemState(IterChild->second.getTreeItem(), TVIS_STATEIMAGEMASK)>>12);
				//Check if state has changed
				if( IterChild->second.getCheckedState() != CheckedState )
				{
					SetItemState( IterChild->second.getTreeItem(), INDEXTOSTATEIMAGEMASK( IterChild->second.getCheckedState() ),  TVIS_STATEIMAGEMASK );
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

	setCheckState( Items, IObjectSelectorItem::Checked );
}

void LeafTreeCtrl::OnUncheckAll()
{
	TreeItemSet Items;

	getRootItems( Items );

	setCheckState( Items, IObjectSelectorItem::Unchecked );
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\LeafTreeCtrl.cpp_v  $
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
