//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
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

namespace SvOsl
{
	#pragma region Declarations
	//Forward declarations
	class ObjectSelectorPpg;
	#pragma endregion Declarations

	class NodeTreeCtrl : public ObjectTreeCtrl
	{
	public:
	#pragma region Constructor
		explicit NodeTreeCtrl( ObjectSelectorPpg& rParent, bool SingleSelect );

		virtual ~NodeTreeCtrl() = default;
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		virtual void loadTree() override;
		virtual void updateTree() override;

		bool SelectNodeByName(const std::string& rDottedName);

	#pragma endregion Public Methods

	protected:
	#pragma region Protected Methods
		DECLARE_MESSAGE_MAP()
		afx_msg void OnExpandAll();
		afx_msg void OnExpandToCheckedItems();
		afx_msg void OnCollapseAll();
		afx_msg void OnCollapseTo2ndLevel();
		afx_msg void OnCheckAll();
		afx_msg void OnUncheckAll();

		virtual void changeSelectedItem( const HTREEITEM& rItem ) override;
		virtual void showContextMenu( bool ItemHit ) override;
		virtual bool isCheckable() const override;
		virtual bool setCheckState(const TreeItemSet& rParentItems, SvCl::ObjectSelectorItem::CheckedStateEnum CheckedState = SvCl::ObjectSelectorItem::EmptyEnabled);
		bool ExpandToCheckedItems();

		void UpdateNode(SvCl::ObjectTreeItems::iterator Iter);

		void filterItems();
	#pragma endregion Protected Methods
	};
} //namespace SvOsl
