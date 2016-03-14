//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : NodeTreeCtrl
//* .File Name       : $Workfile:   NodeTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Dec 2014 17:33:32  $
//* ----------------------------------------------------------------------------
//* This class is used to display only the branch nodes of a tree.
//* This is typically used in conjunction with the LeafTreeCtrl.
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
			The method updates the changed nodes of the tree control.
			***********/
			virtual void updateTree() override;

			/**********
			This method updates all nodes of the tree control.
			***********/
			void UpdateAllNodes();
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
			\param rItem <in> reference to the newly selected item
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
			virtual bool isCheckable() const override;

			/**********
			The method expands all checked items
			\return true if there is a checked item
			***********/
			bool ExpandToCheckedItems();

			/**********
			The method is called to update the state of a node.
			\param rItem <in> reference to the updating item
			***********/
			void UpdateNode( SvTrl::ObjectSelectorItem& rItem );
#pragma endregion Protected Methods
		};
	} //namespace ObjectSelectorLibrary
} //namespace Seidenader

namespace SvOsl = Seidenader::ObjectSelectorLibrary;

