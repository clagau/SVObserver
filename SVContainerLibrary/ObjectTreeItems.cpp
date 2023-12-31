//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
		SVTreeElement* pTreeElement( get() );
		if( nullptr != pTreeElement && nullptr == pTreeElement->second )
		{
			pTreeElement->second = ObjectSelectorItemPtr{ new ObjectSelectorItem };
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
				m_BranchMap[Iter->first] = Iter;
				rSelectorItem.m_Name = Iter->second->m_Name;
				rSelectorItem.m_Location = Iter->second->m_Location;
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
				ObjectSelectorItem::CheckedStateEnum CheckedState = getNodeCheckedState(Iter.base());
				Iter->second->m_NodeState = CheckedState;
			}
			Iter++;
		}
	}

	ObjectSelectorItem::CheckedStateEnum ObjectTreeItems::getNodeCheckedState( const iterator& rIter ) const
	{
		ObjectSelectorItem::CheckedStateEnum result {ObjectSelectorItem::EmptyEnabled};

		if( rIter->second->isNode() )
		{
			bool LoopChildren = true;
			bool AllChecked = true;
			bool SomeChecked = false;

			if( m_SingleSelect )
			{
				result = ObjectSelectorItem::EmptyEnabled;
			}
			else
			{
				result = ObjectSelectorItem::UncheckedEnabled;
			}

			const_iterator IterChild( rIter.node()->begin() );
			while(rIter.node()->end() != IterChild && LoopChildren)
			{
				if (IterChild->second->m_displayItem == false)
				{
					++IterChild;
					continue;
				}

				ObjectSelectorItem::CheckedStateEnum checkedState = IterChild->second->m_NodeItem != nullptr ? IterChild->second->m_NodeState : IterChild->second->m_CheckedState;
				switch(checkedState)
				{
				case ObjectSelectorItem::UncheckedEnabled:
				case ObjectSelectorItem::UncheckedDisabled:
					AllChecked = false;
					break;
				case ObjectSelectorItem::CheckedEnabled:
				case ObjectSelectorItem::CheckedDisabled:
					SomeChecked = true;
					break;
				case ObjectSelectorItem::TriStateEnabled:
				case ObjectSelectorItem::TriStateDisabled:
					AllChecked = false;
					SomeChecked = true;
					break;
				default:
					AllChecked = false;
					break;
				}
				if (IterChild->second->isLeaf())
				{
					if (AllChecked && ObjectSelectorItem::CheckedEnabled != IterChild->second->m_CheckedState)
					{
						AllChecked = false;
						SomeChecked = true;
					}
					else if(false == AllChecked && false == SomeChecked && ObjectSelectorItem::CheckedEnabled == IterChild->second->m_CheckedState)
					{
						SomeChecked = true;
					}
				}
				//If Tristate then don't need to check further
				if(AllChecked == false && SomeChecked == true)
				{
					LoopChildren = false;
				}

				++IterChild;
			}

			if(AllChecked == true)
			{
				if( m_SingleSelect == true )
				{
					//For single select only use the tristate symbol
					result = ObjectSelectorItem::TriStateEnabled;
				}
				else
				{
					result = ObjectSelectorItem::CheckedEnabled;
				}
			}
			else if(SomeChecked == true)
			{
				result = ObjectSelectorItem::TriStateEnabled;
			}
		}

		return result;
	}

	void ObjectTreeItems::synchronizeCheckedStates()
	{
		pre_order_iterator Iter( pre_order_begin() );

		while( pre_order_end() != Iter )
		{
			Iter->second->m_OrgCheckedState = Iter->second->m_CheckedState;
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
			Iter = std::find_if(IterStart, IterEnd, [&Item](const auto& rEntry) {return rEntry.first == Item; });
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

	void ObjectTreeItems::clearItem(const std::string &itemLocation)
	{
		iterator Iter = findItem( itemLocation );
		if( end() != Iter )
		{
			Iter->second->m_CheckedState = ObjectSelectorItem::UncheckedEnabled;
			Iter->second->m_NodeState = m_SingleSelect ? ObjectSelectorItem::EmptyEnabled : ObjectSelectorItem::UncheckedEnabled;
		}
	}
	#pragma endregion Public Methods

	#pragma region Private Methods
	ObjectTreeItems::iterator ObjectTreeItems::createNode(iterator& rParentIter, const std::string& rBranchName, const std::string& rNodeName)
	{
		iterator Iter( end() );
		ObjectSelectorItemPtr pSelectorItem{ new ObjectSelectorItem };

		ObjectSelectorItem::CheckedStateEnum checkedState;

		if( m_SingleSelect )
		{
			checkedState = ObjectSelectorItem::EmptyEnabled;
		}
		else
		{
			checkedState = ObjectSelectorItem::UncheckedEnabled;
		}
		std::string location(rBranchName);
		location += rNodeName;
		pSelectorItem->m_Name =  rNodeName;
		pSelectorItem->m_Location = location;
		pSelectorItem->m_Attribute = ObjectSelectorItem::Node;
		pSelectorItem->m_CheckedState = checkedState;
		pSelectorItem->m_OrgCheckedState = checkedState;
		try
		{
			if( end() == rParentIter )
			{
				Iter = insert( SVTreeElement(location, pSelectorItem) );
			}
			else
			{
				Iter = rParentIter.node()->insert( SVTreeElement(location, pSelectorItem) );
			}
		}
		catch( const std::exception& e )
		{
			//We only want to log otherwise it would need MFC 
			SvStl::MessageManager Exception(SvStl::MsgType::Log );
			Exception.setMessage( SVMSG_SVO_98_TREE_ITEM, e.what(), SvStl::SourceFileParams(StdMessageParams) );
		}

		return Iter;
	}

	#pragma endregion Private Methods
} //namespace SvCl

