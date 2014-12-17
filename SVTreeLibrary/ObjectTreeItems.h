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
#include "ObjectSelectorItem.h"
#include "SVTreeLibrary\SVTree.h"
#include "SVTreeLibrary\SVTreeAdapter.h"
#include "SVUtilityLibrary\SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVTreeLibrary
	{
		class ObjectTreeItems : public SVTree< SVTreeAdapter<SVString, ObjectSelectorItem, SVString> >
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
			iterator insertLeaf(const SVString& rDisplayLocation, ObjectSelectorItem& rSelectorItem);

			/**********
			 The method sets the states of all the nodes
			***********/
			void setNodeCheckedStates();

			/**********
			 The method gets the required state of the node depending on its children
			 \param rIter <in> a reference to the item to get the state for
			 \return the checked state for the item
			***********/
			IObjectSelectorItem::CheckedStateEnum getNodeCheckedState( const iterator& rIter ) const;

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
			iterator findItem( const SVString& rDisplayLocation, bool CreateIfNone = false );

			/**********
			The method sets the state of the parent items
			\param rIter <in> a reference to the parent iterator
			\return string set (location names) of changed parent
			***********/
			SVStringSet setParentState( const SVTreeLibrary::ObjectTreeItems::iterator& rIter );

			/**********
			 The method clears (unchecked) a item at the specified location and update also the parents if necessary.
			 \param rLocation <in> a reference to the location of the item
			 \return string set (location names) of changed items.
			***********/
			SVStringSet clearItem(const SVString& itemLocation);
		#pragma endregion Public Methods
		
		private:
		#pragma region Private Methods
			/**********
			 The method creates a node in the tree container
			 \param rParentIter <in> the parent iterator where the node is to be created
			 \param rDisplayLocation <in> the display node location to create
			 \return the iterator that was created
			***********/
			iterator createNode( iterator& rParentIter, const SVString& rDisplayLocation);

			/**********
			 The method searches the location only in the given level
			 \param rStartIter <in> a reference to start iterator of the level
			 \param rEndIter <in> a reference to end iterator of the level
			 \param rDisplayLocation <in> a reference to the display location of the item
			 \return the iterator to the found item
			***********/
			iterator findLevelItem( const iterator& rStartIter, const iterator& rEnd, const SVString& rDisplayLocation );
		#pragma endregion Private Methods

		private:
		#pragma region Member Variables
			bool m_SingleSelect;
		#pragma endregion Member Variables
		};
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTreeLibrary\ObjectTreeItems.h_v  $
 * 
 *    Rev 1.3   16 Dec 2014 18:17:32   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added methods setParentState (moved from ObjectTreeCtrl) and clearItem.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Dec 2014 09:02:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Single object selection mode state for different icons
 * Added Methods getDisplayLocation;setDisplayLocation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Aug 2014 08:37:08   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added disabled checked states
 * Object Selector displays nodes disabled when in single select mode
 * Added method: setTreeType
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
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