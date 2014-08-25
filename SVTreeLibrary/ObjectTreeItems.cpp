//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeItems
//* .File Name       : $Workfile:   ObjectTreeItems.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   25 Aug 2014 08:34:42  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectTreeItems.h"
#pragma endregion Includes

#pragma region Declarations
using namespace Seidenader::SVTreeLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma endregion Declarations

#pragma region Constructor
ObjectTreeItems::ObjectTreeItems()
	: SVTreeBase()
	, m_SingleSelect( false )
{
}

ObjectTreeItems::ObjectTreeItems( const SVTreeContainer& l_rTree )
	: SVTreeBase( l_rTree )
	, m_SingleSelect( false )
{
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

ObjectTreeItems::iterator ObjectTreeItems::insertLeaf( const SVString& rLocation, ObjectSelectorItem& rSelectorItem )
{
	ObjectTreeItems::iterator Iter = findItem( rLocation, true );

	if( end() != Iter )
	{
		rSelectorItem.setName( Iter->second.getName() );
		rSelectorItem.setLocation( Iter->second.getLocation() );
		Iter->second = rSelectorItem;
	}
	return Iter;
}

void ObjectTreeItems::setNodeCheckedStates()
{
	SVTree_post_order_iterator Iter( post_order_begin() );
	iterator IterBase( begin() );


	while( post_order_end() != Iter )
	{
		if( Iter->second.isNode() )
		{
			IObjectSelectorItem::CheckedStateEnum CheckedState = getNodeCheckedState( Iter.base() );
			if( IObjectSelectorItem::EmptyEnabled != CheckedState && Iter->second.getCheckedState() != CheckedState )
			{
				Iter->second.setCheckedState( CheckedState );
			}
		}
		Iter++;
	}
}

IObjectSelectorItem::CheckedStateEnum ObjectTreeItems::getNodeCheckedState( const iterator& rIter ) const
{
	IObjectSelectorItem::CheckedStateEnum CheckedState = IObjectSelectorItem::EmptyEnabled;

	if( rIter->second.isNode() )
	{
		bool LoopChildren = true;
		bool AllChecked = true;
		bool SomeChecked = false;

		if( m_SingleSelect )
		{
			CheckedState = IObjectSelectorItem::UncheckedDisabled;
		}
		else
		{
			CheckedState = IObjectSelectorItem::UncheckedEnabled;
		}

		const_iterator IterChild( rIter.GetChildTree()->begin() );
		while( rIter.GetChildTree()->end() != IterChild && LoopChildren )
		{
			switch( IterChild->second.getCheckedState() )
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
				CheckedState = IObjectSelectorItem::CheckedDisabled;
			}
			else
			{
				CheckedState = IObjectSelectorItem::CheckedEnabled;
			}
		}
		else if( SomeChecked )
		{
			if( m_SingleSelect )
			{
				CheckedState = IObjectSelectorItem::TriStateDisabled;
			}
			else
			{
				CheckedState = IObjectSelectorItem::TriStateEnabled;
			}
		}
	}

	return CheckedState;
}

void ObjectTreeItems::synchronizeCheckedStates()
{
	ObjectTreeItems::SVTree_pre_order_iterator Iter( pre_order_begin() );

	while( pre_order_end() != Iter )
	{
		Iter->second.setOrgCheckedState( Iter->second.getCheckedState() );
		Iter++;
	}
}

ObjectTreeItems::iterator ObjectTreeItems::findItem( const SVString& rLocation, bool CreateIfNone )
{
	ObjectTreeItems::iterator Iter( end() );
	ObjectTreeItems::iterator IterParent( end() );
	ObjectTreeItems::iterator IterStart( begin() );
	ObjectTreeItems::iterator IterEnd( end() );
	SVString::size_type Pos( 0 );

	while( true )
	{
		SVString Branch;
		Pos = rLocation.find( _T("."), Pos );
		if( SVString::npos == Pos )
		{
			Branch = rLocation;
		}
		else
		{
			Branch = rLocation.Left( Pos );
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
		if( rLocation == Iter->first )
		{
			break;
		}
		//Set the next level
		IterParent = Iter;
		IterStart = Iter.GetChildTree()->begin();
		IterEnd = Iter.GetChildTree()->end();
		Pos++;
	}

	return Iter;
}
#pragma endregion Public Methods

#pragma region Private Methods
ObjectTreeItems::iterator ObjectTreeItems::createNode( ObjectTreeItems::iterator& rParentIter, const SVString& rLocation )
{
	iterator Iter( end() );
	ObjectSelectorItem SelectorItem;
	SVString Name;

	SVString::size_type Pos = rLocation.rfind( _T(".") );
	if( SVString::npos == Pos )
	{
		Name = rLocation;
	}
	else
	{
		Name = rLocation.Mid( Pos + 1 );
	}

	IObjectSelectorItem::CheckedStateEnum CheckedState;

	if( m_SingleSelect )
	{
		CheckedState = IObjectSelectorItem::UncheckedDisabled;
	}
	else
	{
		CheckedState = IObjectSelectorItem::UncheckedEnabled;
	}
	SelectorItem.setName( Name );
	SelectorItem.setLocation( rLocation );
	SelectorItem.setAttibute( IObjectSelectorItem::Node );
	SelectorItem.setCheckedState( CheckedState );
	SelectorItem.setOrgCheckedState( CheckedState );
	try
	{
		if( end() == rParentIter )
		{
			Iter = insert( ObjectTreeItems::ObjectItemsElement( rLocation, SelectorItem ), rParentIter );
		}
		else
		{
			Iter = rParentIter.GetChildTree()->insert( ObjectTreeItems::ObjectItemsElement( rLocation, SelectorItem ) );
		}
	}
	catch( ... )
	{
		::AfxMessageBox( "EXCEPTION: Error creating node" );
	}

	return Iter;
}

ObjectTreeItems::iterator ObjectTreeItems::findLevelItem(const iterator& rIter, const iterator& rEnd, const SVString& rLocation )
{
	iterator Iter( rIter );
	
	while( rEnd != Iter )
	{
		if( rLocation == Iter->first )
		{
			break;
		}

		Iter++;
	}

	return Iter;
}
#pragma endregion Private Methods

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSARCH65\PROJECTFILES\ARCHIVES\SVOBSERVER_SRC\SVTreeLibrary\ObjectTreeItems.cpp_v  $
 * 
 *    Rev 1.1   25 Aug 2014 08:34:42   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added disabled checked states
 * Object Selector displays nodes disabled when in single select mode
 * Added Method: setTreeType
 * Changed methods: setNodeCheckedStates, getNodeCheckedState, createNode
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 17:04:48   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
