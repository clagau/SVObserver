//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : NodeTreeCtrl
//* .File Name       : $Workfile:   NodeTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 11:12:20  $
//* ----------------------------------------------------------------------------
//* This class is used to display the node tree control
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectTreeCtrl.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectSelectorLibrary
	{
		#pragma region Declarations
		//Forward declarations
		class ObjectSelectorPpg;
		#pragma endregion Declarations

		class NodeTreeCtrl : public ObjectTreeCtrl
		{
		public:
		#pragma region Constructor
			/**********
			 The class constructor
			 \param rParent <in> a reference to the parent property page
			 \param SingleSelect <in> true for single selection mode
			***********/
			NodeTreeCtrl( ObjectSelectorPpg& rParent, bool SingleSelect );

			/**********
			 The class destructor
			***********/
			virtual ~NodeTreeCtrl();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			The method loads the control with the tree items from the container
			***********/
			virtual void loadTree() override;

			/**********
			The method updates the tree control 
			***********/
			virtual void updateTree() override;
		#pragma endregion Public Methods

		protected:
		#pragma region Protected Methods
			DECLARE_MESSAGE_MAP()
			//{{AFX_MSG(NodeTreeCtrl)
			afx_msg void OnExpandAll();
			afx_msg void OnExpandToCheckedItems();
			afx_msg void OnCollapseAll();
			afx_msg void OnCollapseTo2ndLevel();
			afx_msg void OnCheckAll();
			afx_msg void OnUncheckAll();
			//}}AFX_MSG

			/**********
			The method is called when tree item selection has changed
			\param Item <in> reference to the newly selected item
			***********/
			virtual void changeSelectedItem( const HTREEITEM& rItem ) override;

			/**********
			The method displays the context menu
			\param ItemHit <in> true if item is hit 
			***********/
			virtual void showContextMenu( bool ItemHit ) override;

			/**********
			The method determines if item is checkable
			***********/
			virtual bool isCheckable() override;

			/**********
			The method expands all checked items
			\return true if there is a checked item
			***********/
			bool ExpandToCheckedItems();
		#pragma endregion Protected Methods
		};
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\NodeTreeCtrl.h_v  $
 * 
 *    Rev 1.0   17 Jul 2014 11:12:20   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
