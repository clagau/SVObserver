//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
	ObjectSelectorItem::ObjectSelectorItem(const SvPb::TreeItem& rTreeItem)
	: m_Name {rTreeItem.name()}
	, m_Location {rTreeItem.location()}
	, m_CheckedState {SvCl::ObjectSelectorItem::UncheckedEnabled}
	, m_NodeState {SvCl::ObjectSelectorItem::UncheckedEnabled}
	, m_ItemTypeName {rTreeItem.type()}
	, m_ItemKey {rTreeItem.objectidindex()}
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	bool ObjectSelectorItem::operator==( const ObjectSelectorItem& rRhs ) const
	{
		bool Result = false;

		if( (m_Name == rRhs.m_Name) &&
			(m_Location == rRhs.m_Location) &&
			(m_ItemTypeName == rRhs.m_ItemTypeName) &&
			(m_ItemKey == rRhs.m_ItemKey) &&
			(m_NodeItem == rRhs.m_NodeItem) &&
			(m_LeafItem == rRhs.m_LeafItem) &&
			(m_Attribute == rRhs.m_Attribute) &&
			(m_IconNumber == rRhs.m_IconNumber) &&
			(m_Modified == rRhs.m_Modified) &&
			(m_NodeState == rRhs.m_NodeState) &&
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