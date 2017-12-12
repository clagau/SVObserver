//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorItem
//* .File Name       : $Workfile:   ObjectSelectorItem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Dec 2014 18:16:34  $
//* ----------------------------------------------------------------------------
//* This class is used to define a single selector item for the tree
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SelectorItem.h"
#pragma endregion Includes

namespace SvCl
{
	struct ObjectSelectorItem : public SelectorItem
	{
	public:
	#pragma region Declarations
		/**********
		The selector item state enumerations
		***********/
		enum AttributeEnum
		{
			AttributeNone = 0,
			Node = 1 << 1,
			Leaf = 1 << 2,
			Checkable = 1 << 3,
			NameEditable = 1 << 4
		};

		/**********
		The selector item checked state enumerations
		***********/
		enum CheckedStateEnum
		{
			EmptyEnabled,
			UncheckedEnabled,
			CheckedEnabled,
			TriStateEnabled,
			EmptyDisabled,
			UncheckedDisabled,
			CheckedDisabled,
			TriStateDisabled
		};
	#pragma endregion Declarations
	public:
	#pragma region Constructor
		ObjectSelectorItem();

		ObjectSelectorItem( const ObjectSelectorItem& rObject ) = default;

		virtual ~ObjectSelectorItem();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		void Clear();

		const ObjectSelectorItem& operator=(const ObjectSelectorItem& rRhs);
		const ObjectSelectorItem& operator=(const SelectorItem& rRhs);
		bool operator==( const ObjectSelectorItem& rRhs ) const;
		bool operator!=( const ObjectSelectorItem& rRhs ) const;

		/**********
			The method checks if the item is a node
			\return True if the item is a node
		***********/
		bool isNode() const;

		/**********
			The method checks if the item is a leaf
			\return True if the item is a leaf
		***********/
		bool isLeaf() const;
	#pragma endregion Public Methods

	#pragma region Member Variables
	public:
		HTREEITEM			m_TreeItem;					//the corresponding tree item handle
		AttributeEnum		m_Attribute;				//The item attribute
		int					m_IconNumber;				//The corresponding item image number
		bool				m_Modified;					//Modified flag
		CheckedStateEnum	m_CheckedState;				//The item checked state
		CheckedStateEnum	m_OrgCheckedState;			//The item original checked state
	#pragma endregion Member Variables
	};

	typedef std::shared_ptr< ObjectSelectorItem > ObjectSelectorItemPtr;
} //namespace SvCl

