//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVProtoBuf/SVRC.h"
#pragma endregion Includes

namespace SvCl
{
	struct ObjectSelectorItem
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
		ObjectSelectorItem() = default;

		explicit ObjectSelectorItem(const ObjectSelectorItem& rObject) = default;
		explicit ObjectSelectorItem(ObjectSelectorItem&& rObject) = default;
		explicit ObjectSelectorItem(const SvPb::TreeItem& rTreeItem);

		virtual ~ObjectSelectorItem() = default;
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		ObjectSelectorItem& operator=(const ObjectSelectorItem& rRhs) = default;
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
		std::string			m_Name;						//The name of the item
		std::string			m_Location;					//The location of the item
		std::string			m_ItemTypeName;				//The name of the data type of the item
		std::string			m_ItemKey;					//The item key
		HTREEITEM			m_NodeItem {nullptr};		//the node item handle
		HTREEITEM			m_LeafItem {nullptr};		//the Leaf item handle
		AttributeEnum		m_Attribute{AttributeNone};	//The item attribute
		int					m_IconNumber{0};			//The corresponding item image number
		bool				m_Modified{false};			//Modified flag
		CheckedStateEnum	m_NodeState {EmptyEnabled};	//The node checked state
		CheckedStateEnum	m_CheckedState{EmptyEnabled};		//The item checked state
		CheckedStateEnum	m_OrgCheckedState{EmptyEnabled};	//The item original checked state
	#pragma endregion Member Variables
	};

	typedef std::shared_ptr< ObjectSelectorItem > ObjectSelectorItemPtr;
} //namespace SvCl

