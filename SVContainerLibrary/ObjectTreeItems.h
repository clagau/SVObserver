//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeItems
//* .File Name       : $Workfile:   ObjectTreeItems.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 Dec 2014 18:17:32  $
//* ----------------------------------------------------------------------------
//* This class is used to define the object selector tree
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "ObjectSelectorItem.h"
#include "SVTree.h"
#include "Definitions/StringTypeDef.h"
#pragma endregion Includes

namespace SvCl
{
	class ObjectTreeItems : public SVTree<std::string, ObjectSelectorItem>
	{
		typedef std::unordered_map<std::string, iterator> BranchIteratorMap;
	public:
	#pragma region Constructor
		/**********
			The class constructor
		***********/
		ObjectTreeItems();

		/**********
			The class destructor
		***********/
		virtual ~ObjectTreeItems();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		/**********
			The method sets if the tree type is single select
			\param SingleSelect <in> true if the tree is in single select mode
		***********/
		void setTreeType( bool SingleSelect );

		/**********
			The method inserts a leaf into the tree including required nodes
			\param rDisplayLocation <in> the dotted path of the item in the tree
			\param rSelectorItem <in> a reference to the selector item to insert
			\return the iterator to the inserted item
		***********/
		iterator insertLeaf(const std::string& rDisplayLocation, ObjectSelectorItem& rSelectorItem);

		/**********
			The method sets the states of all the nodes
		***********/
		void setNodeCheckedStates();

		/**********
			The method gets the required state of the node depending on its children
			\param rIter <in> a reference to the item to get the state for
			\return the checked state for the item
		***********/
		ObjectSelectorItem::CheckedStateEnum getNodeCheckedState( const iterator& rIter ) const;

		/**********
			The method synchronizes the checked states
		***********/
		void synchronizeCheckedStates();

		/**********
			The method finds the item at the specified location
			\param rDisplayLocation <in> a reference to the display location of the item
			\param CreateIfNone <in> creates the location if not available
			\return the iterator to the found item
		***********/
		iterator findItem( const std::string& rDisplayLocation, bool CreateIfNone = false );

		/**********
		The method sets the state of the parent items
		\param rIter <in> a reference to the parent iterator
		\return string set (location names) of changed parent
		***********/
		SvDef::StringSet setParentState( const ObjectTreeItems::iterator& rIter );

		/**********
			The method clears (unchecked) a item at the specified location and update also the parents if necessary.
			\param rLocation <in> a reference to the location of the item
			\return string set (location names) of changed items.
		***********/
		SvDef::StringSet clearItem(const std::string& itemLocation);

		void clearBranchMap() { m_BranchMap.clear(); };
#pragma endregion Public Methods
		
	private:
	#pragma region Private Methods
		/**********
			The method creates a node in the tree container
			\param rParentIter <in> the parent iterator where the node is to be created
			\param rBranch <in> reference to the branch name
			\param rNodeName <in> the name of the node
			\return the iterator that was created
		***********/
		iterator createNode( iterator& rParentIter, const std::string& rBranchName, const std::string& rNodeName);

	#pragma endregion Private Methods

	private:
	#pragma region Member Variables
		bool m_SingleSelect;
		BranchIteratorMap m_BranchMap;

	#pragma endregion Member Variables
	};
} //namespace SvCl

