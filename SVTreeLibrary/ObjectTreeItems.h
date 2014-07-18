//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeItems
//* .File Name       : $Workfile:   ObjectTreeItems.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 17:04:46  $
//* ----------------------------------------------------------------------------
//* This class is used to define the object selector tree
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectSelectorItem.h"
#include "SVTreeLibrary\SVTree.h"
#include "SVTreeLibrary\SVTreeAdapter.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVTreeLibrary
	{
		class ObjectTreeItems :	public SVTree< SVTreeAdapter<SVString, ObjectSelectorItem, SVString> >
		{
		public:
		#pragma region Declarations
			typedef SVTreeAdapter<SVString, ObjectSelectorItem, SVString> SVTreeAdapterBase;
			typedef SVTree<SVTreeAdapterBase> SVTreeBase;
			typedef SVTreeBase::sv_tree_iterator iterator;
			typedef SVTreeBase::sv_tree_const_iterator const_iterator;
			typedef SVTreeBase::SVTreeElement ObjectItemsElement;
			typedef SVTreeBase::SVTreeContainer SVTreeContainer;
		#pragma endregion Declarations

		public:
		#pragma region Constructor
			/**********
			 The class constructor
			***********/
			ObjectTreeItems();

			/**********
			 The class constructor
			 \param rTree <in> a reference to a tree object
			***********/
			ObjectTreeItems( const SVTreeContainer& rTree );

			/**********
			 The class destructor
			***********/
			~ObjectTreeItems();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			 The method inserts a leaf into the tree including required nodes
			 \param rLocation <in> the dotted path of the item in the tree
			 \param rSelectorItem <in> a reference to the selector item to insert
			 \return the iterator to the inserted item
			***********/
			iterator insertLeaf(const SVString& rLocation, ObjectSelectorItem& rSelectorItem);

			/**********
			 The method sets the states of all the nodes
			***********/
			void setNodeCheckedStates();

			/**********
			 The method gets the required state of the node depending on its children
			 \param rIter <in> a reference to the item to get the state for
			 \return the checked state for the item
			***********/
			IObjectSelectorItem::CheckedStateEnum getNodeCheckedState( const iterator& rIter );

			/**********
			 The method synchronizes the checked states
			***********/
			void synchronizeCheckedStates();

			/**********
			 The method finds the item at the specified location
			 \param rLocation <in> a reference to the location of the item
			 \param CreateIfNone <in> creates the location if not available
			 \return the iterator to the found item
			***********/
			iterator findItem( const SVString& rLocation, bool CreateIfNone = false );
		#pragma endregion Public Methods
		
		private:
		#pragma region Private Methods
			/**********
			 The method creates a node in the tree container
			 \param rParentIter <in> the parent iterator where the node is to be created
			 \param rLocation <in> the node location to create
			 \return the iterator that was created
			***********/
			iterator createNode( iterator& rParentIter, const SVString& rLocation);

			/**********
			 The method searches the location only in the given level
			 \param rStartIter <in> a reference to start iterator of the level
			 \param rEndIter <in> a reference to end iterator of the level
			 \param rLocation <in> a reference to the location of the item
			 \return the iterator to the found item
			***********/
			iterator findLevelItem( const iterator& rStartIter, const iterator& rEnd, const SVString& rLocation );
		#pragma endregion Private Methods
		};
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTreeLibrary\ObjectTreeItems.h_v  $
 * 
 *    Rev 1.0   17 Jul 2014 17:04:46   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/