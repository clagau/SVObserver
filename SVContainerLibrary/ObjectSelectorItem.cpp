//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectItemData
//* .File Name       : $Workfile:   ObjectSelectorItem.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 Dec 2014 18:16:34  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "ObjectSelectorItem.h"
#pragma endregion Includes

namespace SvCl
{
	#pragma region Constructor
	ObjectSelectorItem::ObjectSelectorItem()
	{
		Clear();
	}

	ObjectSelectorItem::ObjectSelectorItem( const ObjectSelectorItem& rObject )
	{
		Clear();
		*this = rObject;
	}

	ObjectSelectorItem::~ObjectSelectorItem()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void ObjectSelectorItem::Clear()
	{
		m_Name = _T("");
		m_Location = _T("");
		m_DisplayLocation = _T("");
		m_ItemTypeName = _T("");
		m_TreeItem = nullptr;
		m_ItemKey = _T("");
		m_Attribute = AttributeNone;
		m_IconNumber = 0;
		m_ArrayIndex = -1;
		m_Modified = false;
		m_CheckedState = EmptyEnabled;
		m_OrgCheckedState = EmptyEnabled;
	}

	const ObjectSelectorItem& ObjectSelectorItem::operator=( const ObjectSelectorItem& rObject )
	{
		if( this != &rObject )
		{
			m_Name = rObject.m_Name;
			m_Location = rObject.m_Location;
			m_DisplayLocation = rObject.m_DisplayLocation;
			m_ItemTypeName = rObject.m_ItemTypeName;
			m_TreeItem = rObject.m_TreeItem;
			m_ItemKey = rObject.m_ItemKey;
			m_Attribute = rObject.m_Attribute;
			m_IconNumber = rObject.m_IconNumber;
			m_ArrayIndex = rObject.m_ArrayIndex;
			m_Modified = rObject.m_Modified;
			m_CheckedState = rObject.m_CheckedState;
			m_OrgCheckedState = rObject.m_OrgCheckedState;
		}
		return *this;
	}

	bool ObjectSelectorItem::operator==( const ObjectSelectorItem& rObject ) const
	{
		bool Result = false;

		if( (m_Name == rObject.m_Name) &&
			(m_Location == rObject.m_Location) &&
			(m_DisplayLocation == rObject.m_DisplayLocation) &&
			(m_ItemTypeName == rObject.m_ItemTypeName) &&
			(m_TreeItem == rObject.m_TreeItem) &&
			(m_ItemKey == rObject.m_ItemKey) &&
			(m_Attribute == rObject.m_Attribute) &&
			(m_IconNumber == rObject.m_IconNumber) &&
			(m_ArrayIndex == rObject.m_ArrayIndex) &&
			(m_Modified == rObject.m_Modified) &&
			(m_CheckedState == rObject.m_CheckedState) &&
			(m_OrgCheckedState == rObject.m_OrgCheckedState))
		{
			Result = true;
		}

		return Result;
	}

	bool ObjectSelectorItem::operator!=( const ObjectSelectorItem& rObject ) const
	{
		return !( *this == rObject );
	}

	const std::string& ObjectSelectorItem::getName() const
	{
		return m_Name;
	}

	void ObjectSelectorItem::setName( const std::string& rName )
	{
		m_Name = rName;
	}

	const std::string& ObjectSelectorItem::getLocation() const
	{
		return m_Location;
	}

	void ObjectSelectorItem::setLocation( const std::string& rLocation )
	{
		m_Location = rLocation;
	}

	const std::string& ObjectSelectorItem::getDisplayLocation() const
	{
		return m_DisplayLocation;
	}

	void ObjectSelectorItem::setDisplayLocation( const std::string& rDisplayLocation )
	{
		m_DisplayLocation = rDisplayLocation;
	}

	const std::string& ObjectSelectorItem::getItemTypeName() const
	{
		return m_ItemTypeName;
	}

	void ObjectSelectorItem::setItemTypeName( const std::string& ItemTypeName )
	{
		m_ItemTypeName = ItemTypeName;
	}

	const HTREEITEM& ObjectSelectorItem::getTreeItem() const
	{
		return m_TreeItem;
	}

	void ObjectSelectorItem::setTreeItem( const HTREEITEM& rTreeItem )
	{
		m_TreeItem = rTreeItem;
	}

	const _variant_t& ObjectSelectorItem::getItemKey() const
	{
		return m_ItemKey;
	}

	void ObjectSelectorItem::setItemKey( const _variant_t& rItemKey )
	{
		m_ItemKey = rItemKey;
	}

	const ObjectSelectorItem::AttributeEnum& ObjectSelectorItem::getAttribute() const
	{
		return m_Attribute;
	}

	void ObjectSelectorItem::setAttibute( const AttributeEnum& rAttribute )
	{
		m_Attribute = rAttribute;
	}

	int ObjectSelectorItem::getIconNumber() const
	{
		return m_IconNumber;
	}

	void ObjectSelectorItem::setIconNumber( const int IconNumber )
	{
		m_IconNumber = IconNumber;
	}

	const ObjectSelectorItem::CheckedStateEnum& ObjectSelectorItem::getCheckedState() const
	{
		return m_CheckedState;
	}

	void ObjectSelectorItem::setCheckedState( const CheckedStateEnum& rCheckedState )
	{
		m_CheckedState = rCheckedState;
	}

	const ObjectSelectorItem::CheckedStateEnum& ObjectSelectorItem::getOrgCheckedState() const
	{
		return m_OrgCheckedState;
	}

	void ObjectSelectorItem::setOrgCheckedState( const CheckedStateEnum& rCheckedState )
	{
		m_OrgCheckedState = rCheckedState;
	}

	const int& ObjectSelectorItem::getArrayIndex() const
	{
		return m_ArrayIndex;
	}

	void ObjectSelectorItem::setArrayIndex( const int& rIndex )
	{
		m_ArrayIndex = rIndex;
	}

	void ObjectSelectorItem::setModified( const bool Modified )
	{
		m_Modified = Modified;
	}

	bool ObjectSelectorItem::isNode() const
	{
		return ( m_Attribute & Node ) == Node;
	}

	bool ObjectSelectorItem::isLeaf() const
	{
		return ( m_Attribute & Leaf ) == Leaf;
	}

	bool ObjectSelectorItem::isArray() const
	{
		return ( m_Attribute & Array ) == Array;
	}

	bool ObjectSelectorItem::isModified() const
	{
		return m_Modified;
	}
	#pragma endregion Public Methods

} //namespace SvCl