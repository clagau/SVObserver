//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeCtrl
//* .File Name       : $Workfile:   ObjectTreeCtrl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 17:40:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectTreeCtrl.h"
#include "ObjectSelectorPpg.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//ObjectTreeCtrl static variables 
std::string SvOsl::ObjectTreeCtrl::m_CurrentSelection( _T("") );
SvDef::StringSet SvOsl::ObjectTreeCtrl::m_UpdateItems;
#pragma endregion Declarations

namespace SvOsl
{
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
	void ObjectTreeCtrl::OnTvnSelChanged(NMHDR*, LRESULT* pResult)
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
	
		bool ItemHit = nullptr != Item;
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
				if( isSingleSelect() )
				{
					clearLastCheckedItem( rItem );
				}

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

	bool ObjectTreeCtrl::setCheckState( const TreeItemSet& rParentItems, SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState )
	{
		TreeItemSet::iterator ParentIter = rParentItems.begin();

		while( rParentItems.end() != ParentIter )
		{
			if( nullptr == *ParentIter )
			{
				return false;
			}

			std::string* pLocation = reinterpret_cast<std::string*>( GetItemData( *ParentIter ));
			if( nullptr == pLocation )
			{
				return false;
			}

			SvCl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( *pLocation );
			if( m_rParent.getTreeContainer().end() != Iter )
			{
				//If no defined state then we want to toggle the state
				if( SvCl::ObjectSelectorItem::EmptyEnabled == CheckedState )
				{
					switch(Iter->second->m_CheckedState)
					{
					case SvCl::ObjectSelectorItem::UncheckedEnabled:
						CheckedState =  SvCl::ObjectSelectorItem::CheckedEnabled;
						break;
					case SvCl::ObjectSelectorItem::CheckedEnabled:
					case SvCl::ObjectSelectorItem::TriStateEnabled: // fall through...
						CheckedState = SvCl::ObjectSelectorItem::UncheckedEnabled;
						break;
					case SvCl::ObjectSelectorItem::UncheckedDisabled:
						CheckedState = SvCl::ObjectSelectorItem::CheckedDisabled;
						break;
					case SvCl::ObjectSelectorItem::CheckedDisabled:
					case SvCl::ObjectSelectorItem::TriStateDisabled: // fall through...
						CheckedState = SvCl::ObjectSelectorItem::UncheckedDisabled;
						break;
					default:
						break;
					}
				}

				Iter->second->m_CheckedState =  CheckedState;
				SetItemState( *ParentIter, INDEXTOSTATEIMAGEMASK( CheckedState ), TVIS_STATEIMAGEMASK );
				m_UpdateItems.insert( Iter->first );

				SvDef::StringSet updateItems = getParentPropPage().getTreeContainer().setParentState( Iter );
				m_UpdateItems.insert(updateItems.begin(), updateItems.end());
				if( Iter->second->isNode() )
				{
					setChildrenState( Iter, CheckedState );
				}
			}
			++ParentIter;
		}

		m_rParent.updateData( this );

		return true;
	}

	void ObjectTreeCtrl::setChildrenState( SvCl::ObjectTreeItems::iterator& rIter, SvCl::ObjectSelectorItem::CheckedStateEnum& rCheckedState )
	{
		SvCl::ObjectTreeItems::iterator ChildIter = rIter.node()->begin();
		while( rIter.node()->end() != ChildIter )
		{
			bool TriState = SvCl::ObjectSelectorItem::TriStateEnabled == rCheckedState || SvCl::ObjectSelectorItem::TriStateDisabled == rCheckedState;
			if( !TriState || ChildIter->second->isNode() )
			{
				ChildIter->second->m_CheckedState =  rCheckedState;
				m_UpdateItems.insert( ChildIter->first );
				setChildrenState(ChildIter, rCheckedState);
			}
			++ChildIter;
		}
	}

	void ObjectTreeCtrl::clearLastCheckedItem( const HTREEITEM& rItem )
	{
		if( isSingleSelect() )
		{
			std::string* pLocation = reinterpret_cast<std::string*> ( GetItemData( rItem ) );
			if( nullptr != pLocation )
			{
				if( m_CurrentSelection != *pLocation )
				{
					SvDef::StringSet updateItems = getParentPropPage().getTreeContainer().clearItem (m_CurrentSelection);
					m_UpdateItems.insert(updateItems.begin(), updateItems.end());

					SvCl::ObjectTreeItems::iterator Iter = getParentPropPage().getTreeContainer().findItem( *pLocation );

					if( getParentPropPage().getTreeContainer().end() != Iter )
					{
						//If it is unchecked then it will be checked after this method
						if( SvCl::ObjectSelectorItem::UncheckedEnabled == Iter->second->m_CheckedState )
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

} //namespace SvOsl
