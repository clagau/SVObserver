//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeItems
//* .File Name       : $Workfile:   ObjectTreeItems.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.4  $
//* .Check In Date   : $Date:   18 Dec 2014 02:07:12  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectTreeItems.h"
#pragma endregion Includes

#pragma region Declarations
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

namespace Seidenader { namespace SVTreeLibrary
{
	#pragma region Constructor
	ObjectTreeItems::ObjectTreeItems() : SVTree<SVString, ObjectSelectorItem>()
		, m_SingleSelect( false )
	{
		SVTreeElement* pElement( get() );
		if( nullptr != pElement && pElement->second.empty() )
		{
			pElement->second = new ObjectSelectorItem;
		}
	}

	ObjectTreeItems::~ObjectTreeItems()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void ObjectTreeItems::setTreeType( bool SingleSelect )
	{
		m_SingleSelect = SingleSelect;
	}

	ObjectTreeItems::iterator ObjectTreeItems::insertLeaf( const SVString& rDisplayLocation, ObjectSelectorItem& rSelectorItem )
	{
		iterator Iter = findItem( rDisplayLocation, true );

		if( end() != Iter )
		{
			rSelectorItem.setName( Iter->second->getName() );
			rSelectorItem.setDisplayLocation( Iter->second->getDisplayLocation() );
			*Iter->second = rSelectorItem;
		}
		return Iter;
	}

	void ObjectTreeItems::setNodeCheckedStates()
	{
		post_order_iterator Iter( post_order_begin() );
		iterator IterBase( begin() );

		while( post_order_end() != Iter )
		{
			if( Iter->second->isNode() )
			{
				IObjectSelectorItem::CheckedStateEnum CheckedState = getNodeCheckedState( Iter.base() );
				if( Iter->second->getCheckedState() != CheckedState )
				{
					Iter->second->setCheckedState( CheckedState );
				}
			}
			Iter++;
		}
	}

	IObjectSelectorItem::CheckedStateEnum ObjectTreeItems::getNodeCheckedState( const iterator& rIter ) const
	{
		IObjectSelectorItem::CheckedStateEnum CheckedState = IObjectSelectorItem::EmptyEnabled;

		if( rIter->second->isNode() )
		{
			bool LoopChildren = true;
			bool AllChecked = true;
			bool SomeChecked = false;

			if( m_SingleSelect )
			{
				CheckedState = IObjectSelectorItem::EmptyEnabled;
			}
			else
			{
				CheckedState = IObjectSelectorItem::UncheckedEnabled;
			}

			const_iterator IterChild( rIter.node()->begin() );
			while( rIter.node()->end() != IterChild && LoopChildren )
			{
				switch( IterChild->second->getCheckedState() )
				{
				case IObjectSelectorItem::UncheckedEnabled:
				case IObjectSelectorItem::UncheckedDisabled:
					AllChecked = false;
					break;
				case IObjectSelectorItem::CheckedEnabled:
				case IObjectSelectorItem::CheckedDisabled:
					SomeChecked = true;
					break;
				case IObjectSelectorItem::TriStateEnabled:
				case IObjectSelectorItem::TriStateDisabled:
					AllChecked = false;
					SomeChecked = true;
					break;
				default:
					AllChecked = false;
					break;
				}
				//If Tristate then don't need to check further
				if( !AllChecked && SomeChecked )
				{
					LoopChildren = false;
				}

				IterChild++;
			}

			if( AllChecked )
			{
				if( m_SingleSelect )
				{
					//For single select only use the tristate symbol
					CheckedState = IObjectSelectorItem::TriStateEnabled;
				}
				else
				{
					CheckedState = IObjectSelectorItem::CheckedEnabled;
				}
			}
			else if( SomeChecked )
			{
				CheckedState = IObjectSelectorItem::TriStateEnabled;
			}
		}

		return CheckedState;
	}

	void ObjectTreeItems::synchronizeCheckedStates()
	{
		pre_order_iterator Iter( pre_order_begin() );

		while( pre_order_end() != Iter )
		{
			Iter->second->setOrgCheckedState( Iter->second->getCheckedState() );
			Iter++;
		}
	}

	ObjectTreeItems::iterator ObjectTreeItems::findItem( const SVString& rDisplayLocation, bool CreateIfNone )
	{
		iterator Iter( end() );
		iterator IterParent( end() );
		iterator IterStart( begin() );
		iterator IterEnd( end() );
		SVString::size_type Pos( 0 );

		while( true )
		{
			SVString Branch;
			Pos = rDisplayLocation.find( _T("."), Pos );
			if( SVString::npos == Pos )
			{
				Branch = rDisplayLocation;
			}
			else
			{
				Branch = rDisplayLocation.substr( 0, Pos );
			}
			Iter = findLevelItem( IterStart, IterEnd, Branch);
			//Branch not found so create or exit
			if(IterEnd  == Iter )
			{
				if( CreateIfNone )
				{
					Iter = createNode( IterParent, Branch );
				}
				else
				{
					Iter = end();
					break;
				}
			}
			//Found the search item 
			if( rDisplayLocation == Iter->first )
			{
				break;
			}
			//Set the next level
			IterParent = Iter;
			IterStart = Iter.node()->begin();
			IterEnd = Iter.node()->end();
			Pos++;
		}

		return Iter;
	}

	SVStringSet ObjectTreeItems::setParentState( const ObjectTreeItems::iterator& rIter )
	{
		SVStringSet retValue;
		if( !rIter.node()->parent()->is_root() )
		{
			ObjectTreeItems::iterator ParentIter = findItem( rIter.node()->parent()->get()->first );

			if( end() != ParentIter )
			{
				IObjectSelectorItem::CheckedStateEnum CheckedState = getNodeCheckedState( ParentIter );
				if( ParentIter->second->getCheckedState() != CheckedState )
				{
					ParentIter->second->setCheckedState( CheckedState );
					retValue.insert( ParentIter->first );
					SVStringSet tmpValue = setParentState( ParentIter );
					retValue.insert( tmpValue.begin(), tmpValue.end() );
				}
			}
		}
		return retValue;
	}

	SVStringSet ObjectTreeItems::clearItem(const SVString &itemLocation)
	{
		SVStringSet updateItems;
		iterator Iter = findItem( itemLocation );
		if( end() != Iter )
		{
			Iter->second->setCheckedState( IObjectSelectorItem::UncheckedEnabled );

			updateItems = setParentState( Iter );
			updateItems.insert( Iter->first );
		}
		return updateItems;
	}
	#pragma endregion Public Methods

	#pragma region Private Methods
	ObjectTreeItems::iterator ObjectTreeItems::createNode( ObjectTreeItems::iterator& rParentIter, const SVString& rDisplayLocation )
	{
		iterator Iter( end() );
		ObjectSelectorItemPtr pSelectorItem =  new ObjectSelectorItem;
		SVString Name;

		SVString::size_type Pos = rDisplayLocation.rfind( _T(".") );
		if( SVString::npos == Pos )
		{
			Name = rDisplayLocation;
		}
		else
		{
			Name = SvUl_SF::Mid( rDisplayLocation, Pos + 1 );
		}

		IObjectSelectorItem::CheckedStateEnum CheckedState;

		if( m_SingleSelect )
		{
			CheckedState = IObjectSelectorItem::EmptyEnabled;
		}
		else
		{
			CheckedState = IObjectSelectorItem::UncheckedEnabled;
		}
		pSelectorItem->setName( Name );
		pSelectorItem->setDisplayLocation( rDisplayLocation );
		pSelectorItem->setAttibute( IObjectSelectorItem::Node );
		pSelectorItem->setCheckedState( CheckedState );
		pSelectorItem->setOrgCheckedState( CheckedState );
		try
		{
			if( end() == rParentIter )
			{
				Iter = insert( SVTreeElement( rDisplayLocation, pSelectorItem ) );
			}
			else
			{
				Iter = rParentIter.node()->insert( SVTreeElement( rDisplayLocation, pSelectorItem ) );
			}
		}
		catch( ... )
		{
			::AfxMessageBox( "EXCEPTION: Error creating node" );
		}

		return Iter;
	}

	ObjectTreeItems::iterator ObjectTreeItems::findLevelItem(const iterator& rIter, const iterator& rEnd, const SVString& rDisplayLocation )
	{
		iterator Iter( rIter );
	
		while( rEnd != Iter )
		{
			if( rDisplayLocation == Iter->first )
			{
				break;
			}

			Iter++;
		}

		return Iter;
	}
	#pragma endregion Private Methods
} /* namespace SVTreeLibrary */ } /* namespace Seidenader */

