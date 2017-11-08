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
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvCl
{
	#pragma region Constructor
	ObjectTreeItems::ObjectTreeItems() : SVTree<std::string, ObjectSelectorItem>()
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

	ObjectTreeItems::iterator ObjectTreeItems::insertLeaf( const std::string& rDisplayLocation, ObjectSelectorItem& rSelectorItem )
	{
		iterator Iter(end());
		iterator ParentIter(end());
		std::string Branch(rDisplayLocation);
		std::string LeafName;

		std::string::size_type Pos = rDisplayLocation.rfind('.');
		if (std::string::npos != Pos)
		{
			Branch = SvUl::Left(rDisplayLocation, Pos);
			LeafName = SvUl::Mid(rDisplayLocation, Pos + 1);
		}
		BranchIteratorMap::iterator BranchMapIter = m_BranchMap.find(Branch);
		if (m_BranchMap.end() != BranchMapIter)
		{
			ParentIter = BranchMapIter->second;
		}
		else
		{
			ParentIter = findItem(Branch, true);
		}

		if (end() != ParentIter)
		{
			Branch += '.';
			Iter = createNode(ParentIter, Branch, LeafName);
			if (end() != Iter)
			{
				rSelectorItem.setName(Iter->second->getName());
				rSelectorItem.setDisplayLocation(Iter->second->getDisplayLocation());
				*Iter->second = rSelectorItem;
			}
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

	ObjectTreeItems::iterator ObjectTreeItems::findItem(const std::string& rDisplayLocation, bool CreateIfNone)
	{
		iterator Iter(end());
		iterator IterParent(end());
		iterator IterStart(begin());
		iterator IterEnd(end());
		std::string::size_type StartPos(std::string::npos);
		std::string::size_type EndPos;

		//! Check if sub branch is available
		while ( std::string::npos != (EndPos = rDisplayLocation.rfind('.', StartPos)))
		{
			std::string Branch = rDisplayLocation.substr(0, EndPos);
			BranchIteratorMap::iterator BranchMapIter = m_BranchMap.find(Branch);
			if (m_BranchMap.end() != BranchMapIter)
			{
				IterParent = BranchMapIter->second;
				IterStart = IterParent.node()->begin();
				IterEnd = IterParent.node()->end();
				StartPos = EndPos + 1;
				break;
			}
			StartPos = EndPos - 1;
		}
		if (std::string::npos == EndPos)
		{
			StartPos = 0;
		}
		while (true)
		{
			std::string Branch;
			std::string NodeName;
			EndPos = rDisplayLocation.find(_T("."), StartPos);
			if (std::string::npos == EndPos)
			{
				NodeName = SvUl::Mid(rDisplayLocation, StartPos);
				Branch = rDisplayLocation.substr(0, StartPos);
			}
			else
			{
				NodeName = rDisplayLocation.substr(StartPos, EndPos - StartPos);
				Branch = rDisplayLocation.substr(0, StartPos);
			}
			std::string Item = Branch + NodeName;
			Iter = std::find_if(IterStart, IterEnd, SVCompareKeys<std::string, SVSharedPtr<ObjectSelectorItem>>(Item));;
			//Branch not found so create or exit
			if (IterEnd == Iter)
			{
				if (CreateIfNone)
				{
					Iter = createNode(IterParent, Branch, NodeName);
					m_BranchMap[Iter->first] = Iter;
				}
				else
				{
					Iter = end();
					break;
				}
			}
			//Found the search item 
			if (rDisplayLocation == Iter->first)
			{
				break;
			}
			//Set the next level
			IterParent = Iter;
			IterStart = Iter.node()->begin();
			IterEnd = Iter.node()->end();
			StartPos = EndPos + 1;
		}

		return Iter;
	}

	SvDef::StringSet ObjectTreeItems::setParentState( const ObjectTreeItems::iterator& rIter )
	{
		SvDef::StringSet retValue;
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
					SvDef::StringSet tmpValue = setParentState( ParentIter );
					retValue.insert( tmpValue.begin(), tmpValue.end() );
				}
			}
		}
		return retValue;
	}

	SvDef::StringSet ObjectTreeItems::clearItem(const std::string &itemLocation)
	{
		SvDef::StringSet updateItems;
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
	ObjectTreeItems::iterator ObjectTreeItems::createNode(iterator& rParentIter, const std::string& rBranchName, const std::string& rNodeName)
	{
		iterator Iter( end() );
		ObjectSelectorItemPtr pSelectorItem =  new ObjectSelectorItem;

		IObjectSelectorItem::CheckedStateEnum CheckedState;

		if( m_SingleSelect )
		{
			CheckedState = IObjectSelectorItem::EmptyEnabled;
		}
		else
		{
			CheckedState = IObjectSelectorItem::UncheckedEnabled;
		}
		std::string DisplayLocation(rBranchName);
		DisplayLocation += rNodeName;
		pSelectorItem->setName( rNodeName );
		pSelectorItem->setDisplayLocation(DisplayLocation);
		pSelectorItem->setAttibute( IObjectSelectorItem::Node );
		pSelectorItem->setCheckedState( CheckedState );
		pSelectorItem->setOrgCheckedState( CheckedState );
		try
		{
			if( end() == rParentIter )
			{
				Iter = insert( SVTreeElement(DisplayLocation, pSelectorItem) );
			}
			else
			{
				Iter = rParentIter.node()->insert( SVTreeElement(DisplayLocation, pSelectorItem) );
			}
		}
		catch( const std::exception& e )
		{
			//We only want to log otherwise it would need MFC 
			SvStl::MessageMgrStd Exception( SvStl::LogOnly );
			Exception.setMessage( SVMSG_SVO_98_TREE_ITEM, e.what(), SvStl::SourceFileParams(StdMessageParams) );
		}

		return Iter;
	}

	#pragma endregion Private Methods
} //namespace SvCl

