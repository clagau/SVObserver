//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeCtrl
//* .File Name       : $Workfile:   ObjectTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   04 Dec 2014 03:20:50  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVOResource\resource.h"
#include "ObjectTreeCtrl.h"
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

//ObjectTreeCtrl static variables 
SVString ObjectTreeCtrl::m_CurrentSelection( _T("") );
SVStringSet ObjectTreeCtrl::m_UpdateItems;
#pragma endregion Declarations

BEGIN_MESSAGE_MAP(ObjectTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(ObjectTreeCtrl)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_DESTROY()
	ON_NOTIFY_REFLECT( TVN_SELCHANGED, ObjectTreeCtrl::OnTvnSelChanged )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#pragma region Constructor
ObjectTreeCtrl::ObjectTreeCtrl( ObjectSelectorPpg& rParent, bool SingleSelect )
	: CTreeCtrl()
	, m_rParent( rParent )
	, m_SingleSelect( SingleSelect)
	, m_ContextPoint(0, 0)
	, m_LeftButtonCheckFlag( TVHT_ONITEMSTATEICON )
{
}

ObjectTreeCtrl::~ObjectTreeCtrl()
{
}
#pragma endregion Constructor

#pragma region Protected Methods
void ObjectTreeCtrl::OnTvnSelChanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	HTREEITEM Item = GetSelectedItem();

	changeSelectedItem( Item );
}

void ObjectTreeCtrl::OnLButtonDown( UINT Flags, CPoint Point )
{
	//When left button clicked then reset context menu point
	m_ContextPoint = CPoint(0, 0);

	HTREEITEM Item = checkItemHit( Point, m_LeftButtonCheckFlag );

	if( setCheckItem( Item ) )
	{
		SelectItem( Item );
		SetFocus();
	}
	else
	{
		CTreeCtrl::OnLButtonDown( Flags, Point );
	}
}

void ObjectTreeCtrl::OnRButtonDown( UINT Flags, CPoint Point )
{
	//Save the context point to be able to check which item the context menu action is destined for
	m_ContextPoint = Point;

	HTREEITEM Item = checkItemHit( getContextPoint(), TVHT_ONITEM );
	
	bool ItemHit = NULL != Item;
	if( ItemHit )
	{
		SelectItem( Item );
	}

	showContextMenu( ItemHit );

	CTreeCtrl::OnRButtonDown( Flags, Point );
}

void ObjectTreeCtrl::OnKeyDown( UINT Char, UINT RepCnt, UINT Flags )
{
	bool KeyHandled( false );

	if( VK_SPACE == Char )
	{
		HTREEITEM Item = GetSelectedItem();

		KeyHandled = setCheckItem( Item );
	}

	if( !KeyHandled )
	{
		CTreeCtrl::OnKeyDown( Char, RepCnt, Flags );
	}
}

void ObjectTreeCtrl::OnDestroy()
{
	CTreeCtrl::OnDestroy();

	DeleteAllItems();
}

void ObjectTreeCtrl::getRootItems( TreeItemSet& rRootItems ) const
{
	HTREEITEM Item = GetRootItem();

	while( nullptr != Item )
	{
		rRootItems.insert( Item );
		Item = GetNextSiblingItem( Item );
	}
}

void ObjectTreeCtrl::expandAll( const TreeItemSet& rParentItems, UINT Code )
{
	TreeItemSet::iterator Iter = rParentItems.begin();

	while( rParentItems.end() != Iter )
	{
		if( (nullptr != *Iter) && ItemHasChildren( *Iter ) )
		{
			Expand( *Iter, Code );

			HTREEITEM ChildItem;
			ChildItem = GetChildItem( *Iter );
			while( nullptr != ChildItem )
			{
				TreeItemSet ChildItems;
				ChildItems.insert(ChildItem);
				expandAll( ChildItems, Code );
				ChildItem = GetNextItem( ChildItem, TVGN_NEXT );
			}
		}
		++Iter;
	}
}

void ObjectTreeCtrl::selectFirstItem( const TreeItemSet& rItems )
{
	TreeItemSet::const_iterator IterItem = rItems.begin();
	if( rItems.end() != IterItem )
	{
		SelectItem( *IterItem );
		Select( *IterItem, TVGN_FIRSTVISIBLE );
	}
}

const HTREEITEM ObjectTreeCtrl::checkItemHit( const CPoint& rPoint, const UINT Flags ) const
{
	TVHITTESTINFO HitTestInfo;
	HitTestInfo.pt = rPoint;

	HTREEITEM Item = HitTest( &HitTestInfo );
	
	if( nullptr != Item && 0 == (HitTestInfo.flags & Flags) )
	{
		Item = nullptr;
	}
	
	return Item;
}

bool ObjectTreeCtrl::setCheckItem( const HTREEITEM& rItem )
{
	bool Result( false );

	if( nullptr != rItem )
	{
		//Check if item is checkable
		if( isCheckable() )
		{
			clearLastCheckedItem( rItem );
			TreeItemSet Items;

			Items.insert( rItem );
			if( setCheckState( Items ) )
			{
				Result = true;
			}
		}
		else
		{
			Result = true;
		}
	}

	return Result;
}

bool ObjectTreeCtrl::setCheckState( const TreeItemSet& rParentItems, IObjectSelectorItem::CheckedStateEnum CheckedState )
{
	TreeItemSet::iterator ParentIter = rParentItems.begin();

	while( rParentItems.end() != ParentIter )
	{
		if( nullptr == *ParentIter )
		{
			return false;
		}

		SVString* pLocation = reinterpret_cast<SVString*>( GetItemData( *ParentIter ));
		if( nullptr == pLocation )
		{
			return false;
		}

		ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( *pLocation );
		if( m_rParent.getTreeContainer().end() != Iter )
		{
			//If no defined state then we want to toggle the state
			if( IObjectSelectorItem::EmptyEnabled == CheckedState )
			{
				switch( Iter->second.getCheckedState() )
				{
				case IObjectSelectorItem::UncheckedEnabled:
					CheckedState =  IObjectSelectorItem::CheckedEnabled;
					break;
				case IObjectSelectorItem::CheckedEnabled:
				case IObjectSelectorItem::TriStateEnabled: // fall through...
					CheckedState = IObjectSelectorItem::UncheckedEnabled;
					break;
				case IObjectSelectorItem::UncheckedDisabled:
					CheckedState = IObjectSelectorItem::CheckedDisabled;
					break;
				case IObjectSelectorItem::CheckedDisabled:
				case IObjectSelectorItem::TriStateDisabled: // fall through...
					CheckedState = IObjectSelectorItem::UncheckedDisabled;
					break;
				default:
					break;
				}
			}

			Iter->second.setCheckedState( CheckedState );
			SetItemState( *ParentIter, INDEXTOSTATEIMAGEMASK( CheckedState ),  TVIS_STATEIMAGEMASK);
			m_UpdateItems.insert( Iter->first );

			setParentState( Iter );
			if( Iter->second.isNode() )
			{
				setChildrenState( Iter, CheckedState );
			}
		}
		++ParentIter;
	}

	m_rParent.updateData( this );

	return true;
}

void ObjectTreeCtrl::setChildrenState( ObjectTreeItems::iterator& rIter, IObjectSelectorItem::CheckedStateEnum& rCheckedState )
{
	ObjectTreeItems::iterator ChildIter = rIter.GetChildTree()->begin();
	while( rIter.GetChildTree()->end() != ChildIter )
	{
		bool TriState = IObjectSelectorItem::TriStateEnabled == rCheckedState || IObjectSelectorItem::TriStateDisabled == rCheckedState;
		if( !TriState || ChildIter->second.isNode() )
		{
			ChildIter->second.setCheckedState( rCheckedState );
			m_UpdateItems.insert( ChildIter->first );
			setChildrenState(ChildIter, rCheckedState);
		}
		++ChildIter;
	}
}

void ObjectTreeCtrl::setParentState( ObjectTreeItems::iterator& rIter )
{
	if( !rIter.parent()->is_root() )
	{
		ObjectTreeItems::iterator ParentIter = getParentPropPage().getTreeContainer().findItem( rIter.parent()->get()->first );

		if( getParentPropPage().getTreeContainer().end() != ParentIter )
		{
			IObjectSelectorItem::CheckedStateEnum CheckedState = IObjectSelectorItem::UncheckedEnabled;

			CheckedState = getParentPropPage().getTreeContainer().getNodeCheckedState( ParentIter );
			if( ParentIter->second.getCheckedState() != CheckedState )
			{
				ParentIter->second.setCheckedState( CheckedState );
				m_UpdateItems.insert( ParentIter->first );
				setParentState( ParentIter );
			}
		}
	}
}

void ObjectTreeCtrl::clearLastCheckedItem( const HTREEITEM& rItem )
{
	if( isSingleSelect() )
	{
		SVString* pLocation = reinterpret_cast<SVString*> ( GetItemData( rItem ) );
		if( nullptr != pLocation )
		{
			if( m_CurrentSelection != *pLocation )
			{
				ObjectTreeItems::iterator Iter( getParentPropPage().getTreeContainer().end() );

				Iter = getParentPropPage().getTreeContainer().findItem( m_CurrentSelection );
				if( getParentPropPage().getTreeContainer().end() != Iter )
				{
					Iter->second.setCheckedState( IObjectSelectorItem::UncheckedEnabled );

					m_UpdateItems.insert( Iter->first );
					setParentState( Iter );
				}

				Iter = getParentPropPage().getTreeContainer().findItem( *pLocation );

				if( getParentPropPage().getTreeContainer().end() != Iter )
				{
					//If it is unchecked then it will be checked after this method
					if( IObjectSelectorItem::UncheckedEnabled == Iter->second.getCheckedState() )
					{
						m_CurrentSelection = *pLocation;
					}
					else
					{
						m_CurrentSelection.clear();
					}
				}
			}
		}
	}
}
#pragma endregion Protected Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeCtrl.cpp_v  $
 * 
 *    Rev 1.4   04 Dec 2014 03:20:50   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   In Single Selection mode icons are dotted for highlighted nodes otherwise no icons are displayed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   25 Aug 2014 07:42:44   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added disabled checked states
 * Object Selector displays nodes disabled when in single select mode
 * Changed methods: setCheckState, setChildrenState, setParentState, clearLastCheckedItem
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Aug 2014 07:46:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Coding guidline changes using const object for get and is methods
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   12 Aug 2014 12:30:46   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Optional what part of the tree item is clicked for it to toggle
 * Coding guidline changes
 * Changed methods: OnLButtonDown
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:16:24   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/