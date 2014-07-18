//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeCtrl
//* .File Name       : $Workfile:   ObjectTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:16:26  $
//* ----------------------------------------------------------------------------
//* This class is used to display the object tree control
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTreeLibrary\ObjectTreeItems.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectSelectorLibrary
	{
		#pragma region Declarations
		//Forward declarations
		class ObjectSelectorPpg;
		#pragma endregion Declarations

		class ObjectTreeCtrl : public CTreeCtrl
		{
		public:
		#pragma region Declarations
			typedef std::set< HTREEITEM > TreeItemSet;
		#pragma endregion Declarations

		public:
		#pragma region Constructor
			/**********
			 The class constructor
			 \param rParent <in> a reference to the parent property page
			 \param SingleSelect <in> true for single selection mode
			***********/
			ObjectTreeCtrl( ObjectSelectorPpg& rParent, bool SingleSelect );

			/**********
			 The class destructor
			***********/
			virtual ~ObjectTreeCtrl();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			The method loads the control with the tree items from the container
			***********/
			virtual void loadTree() = 0;

			/**********
			The method updates the tree control
			***********/
			virtual void updateTree() = 0;
		#pragma endregion Public Methods

		protected:
		#pragma region Protected Methods
			DECLARE_MESSAGE_MAP()
			//{{AFX_MSG(ObjectTreeCtrl)
			afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
			afx_msg void OnLButtonDown( UINT Flags, CPoint Point );
			afx_msg void OnRButtonDown( UINT Flags, CPoint Point );
			afx_msg void OnKeyDown( UINT Char, UINT RepCnt, UINT Flags );
			afx_msg void OnDestroy();
			//}}AFX_MSG

			/**********
			The method is called when tree item selection has changed
			\param Item <in> the newly selected item
			***********/
			virtual void changeSelectedItem( const HTREEITEM& rItem ) = 0;

			/**********
			The method displays the context menu
			\param ItemHit <in> true if item is hit 
			***********/
			virtual void showContextMenu( bool ItemHit ) = 0;

			/**********
			The method determines if item is checkable
			***********/
			virtual bool isCheckable() = 0;

			/**********
			The method gets the list of root items
			\param rRootItems <in> the reference to the list of root items
			***********/
			void getRootItems( TreeItemSet& rRootItems );

			/**********
			The method expands or collapses all items starting with the parent item
			\param rParentItems <in> the parent items to start expanding
			\param Code <in> the expand collapse code
			***********/
			void expandAll( const TreeItemSet& rParentItems, UINT Code );

			/**********
			The method selects the first tree item 
			\param rItems <in> a reference to the item set
			***********/
			void selectFirstItem( const TreeItemSet& rItems );

			/**********
			The method checks if at the given point a valid item is hit
			\param rPoint <in> a reference to the point to check if there is an item
			\param Flags <in> the flags for the hit test
			\return the tree item or NULL
			***********/
			const HTREEITEM checkItemHit( const CPoint& rPoint, UINT Flags ) const;

			/**********
			The method sets the item to checked
			\param rItem <in> reference to the item to be checked
			\return true if check item handled
			***********/
			bool setCheckItem( const HTREEITEM& rItem );

			/**********
			The method sets the check state of the item
			\param rParentItems <in> the respective parent items to change the state
			\param CheckedState <in> the state to change it to
			\return True if state has changed
			***********/
			bool setCheckState( const TreeItemSet& rParentItems, SVTreeLibrary::IObjectSelectorItem::CheckedStateEnum CheckedState = SVTreeLibrary::IObjectSelectorItem::CheckedStateNone );

			/**********
			The method sets the state of the children items
			\param rIter <in> a reference to the parent iterator
			\param rCheckedState <in> a reference to the checked state
			***********/
			void setChildrenState( SVTreeLibrary::ObjectTreeItems::iterator& rIter, SVTreeLibrary::IObjectSelectorItem::CheckedStateEnum& rCheckedState );

			/**********
			The method sets the state of the parent items
			\param rIter <in> a reference to the parent iterator
			\param rCheckedState <in> a reference to the checked state
			***********/
			void setParentState( SVTreeLibrary::ObjectTreeItems::iterator& rIter );

			/**********
			The method clears the last checked item for single select mode
			\param rItem <in> a reference to the item selected
			***********/
			void clearLastCheckedItem( const HTREEITEM& rItem );

			/**********
			The method gets the parent property page
			\return the property page reference
			***********/
			inline ObjectSelectorPpg& getParentPropPage();

			/**********
			The method gets the point where the context menu was clicked
			\return the point of the context menu
			***********/
			inline const CPoint& getContextPoint();

			/**********
			The method returns true if set to single select mode
			\return true if single select
			***********/
			inline bool isSingleSelect();

			/**********
			The method sets the current selection
			\param rCurrentSelection <in> a reference to the current selection
			***********/
			inline void setCurrentSelection( const SVString& rCurrentSelection );

			/**********
			The method gets a reference to the update items
			\return a reference to the list of updated items
			***********/
			inline SVStringSet& getUpdateItems();
		#pragma endregion Protected Methods

		private:
		#pragma region Member Variables
			ObjectSelectorPpg& m_rParent;					//The parent reference
			CPoint m_ContextPoint;							//Point where context menu called
			bool m_SingleSelect;							//True when tree has only single item selection
			static SVString m_CurrentSelection;				//The current selection
			static SVStringSet m_UpdateItems;				//The list of items to update
		#pragma endregion Member Variables
		};

		#pragma region Inline
		#include "ObjectTreeCtrl.inl"
		#pragma endregion Inline
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeCtrl.h_v  $
 * 
 *    Rev 1.0   17 Jul 2014 11:16:26   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
