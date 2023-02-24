//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : LeafTreeCtrl
//* .File Name       : $Workfile:   LeafTreeCtrl.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Dec 2014 17:30:58  $
//* ----------------------------------------------------------------------------
//* This class is used to display only the leaves of a tree for selection purposes.
//* This is typically used in conjunction with the NodeTreeCtrl.
//******************************************************************************

#pragma once

#pragma region Includes
#include "ObjectTreeCtrl.h"

#pragma endregion Includes

namespace SvOsl
{
	//Forward declarations
	class ObjectSelectorPpg;

	class LeafTreeCtrl : public ObjectTreeCtrl
	{
	public:
	#pragma region Constructor
		/**********
			The class constructor
			\param rParent <in> a reference to the parent property page
			\param SingleSelect <in> true for single selection mode
		***********/
		explicit LeafTreeCtrl( ObjectSelectorPpg& rParent, bool SingleSelect );

		/**********
			The class destructor
		***********/
		virtual ~LeafTreeCtrl() = default;
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
		//{{AFX_MSG(LeafTreeCtrl)
		afx_msg void OnCheckAll();
		afx_msg void OnUncheckAll();
		//}}AFX_MSG

		/**********
		The method is called when the tree item selection has changed
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

		virtual bool setCheckState(const TreeItemSet& rParentItems, SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState = SvCl::ObjectSelectorItem::EmptyEnabled);
#pragma endregion Protected Methods

	private:
	#pragma region Member Variables
		std::string m_NodeLocation;						//The current node location
	#pragma endregion Member Variables
	};
} //namespace SvOsl

