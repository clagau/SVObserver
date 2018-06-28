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
		m_ItemKey = GUID_NULL;
		m_Attribute = AttributeNone;
		m_IconNumber = 0;
		m_ArrayIndex = -1;
		m_Modified = false;
		m_CheckedState = EmptyEnabled;
		m_OrgCheckedState = EmptyEnabled;
	}

	const ObjectSelectorItem& ObjectSelectorItem::operator=( const ObjectSelectorItem& rRhs )
	{
		if( this != &rRhs )
		{
			//SelectorItem values
			m_Name = rRhs.m_Name;
			m_Location = rRhs.m_Location;
			m_DisplayLocation = rRhs.m_DisplayLocation;
			m_ItemTypeName = rRhs.m_ItemTypeName;
			m_ItemKey = rRhs.m_ItemKey;
			m_Array = rRhs.m_Array;
			m_ArrayIndex = rRhs.m_ArrayIndex;
			//ObjectSelectorItem values
			m_TreeItem = rRhs.m_TreeItem;
			m_Attribute = rRhs.m_Attribute;
			m_IconNumber = rRhs.m_IconNumber;
			m_Modified = rRhs.m_Modified;
			m_CheckedState = rRhs.m_CheckedState;
			m_OrgCheckedState = rRhs.m_OrgCheckedState;
		}
		return *this;
	}

	const ObjectSelectorItem& ObjectSelectorItem::operator=(const SelectorItem& rRhs)
	{
		if (this != &rRhs)
		{
			m_Name = rRhs.m_Name;
			m_Location = rRhs.m_Location;
			m_DisplayLocation = rRhs.m_DisplayLocation;
			m_ItemTypeName = rRhs.m_ItemTypeName;
			m_ItemKey = rRhs.m_ItemKey;
			m_Array = rRhs.m_Array;
			m_ArrayIndex = rRhs.m_ArrayIndex;
			m_Selected = rRhs.m_Selected;
		}
		return *this;
	}

	bool ObjectSelectorItem::operator==( const ObjectSelectorItem& rRhs ) const
	{
		bool Result = false;

		if( (m_Name == rRhs.m_Name) &&
			(m_Location == rRhs.m_Location) &&
			(m_DisplayLocation == rRhs.m_DisplayLocation) &&
			(m_ItemTypeName == rRhs.m_ItemTypeName) &&
			(m_TreeItem == rRhs.m_TreeItem) &&
			(m_ItemKey == rRhs.m_ItemKey) &&
			(m_Attribute == rRhs.m_Attribute) &&
			(m_IconNumber == rRhs.m_IconNumber) &&
			(m_ArrayIndex == rRhs.m_ArrayIndex) &&
			(m_Modified == rRhs.m_Modified) &&
			(m_CheckedState == rRhs.m_CheckedState) &&
			(m_OrgCheckedState == rRhs.m_OrgCheckedState))
		{
			Result = true;
		}

		return Result;
	}

	bool ObjectSelectorItem::operator!=( const ObjectSelectorItem& rRhs ) const
	{
		return !( *this == rRhs );
	}

	bool ObjectSelectorItem::isNode() const
	{
		return ( m_Attribute & Node ) == Node;
	}

	bool ObjectSelectorItem::isLeaf() const
	{
		return ( m_Attribute & Leaf ) == Leaf;
	}
	#pragma endregion Public Methods

} //namespace SvCl