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

#pragma region Declarations
using namespace Seidenader::SVTreeLibrary;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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
	Clear();
}
#pragma endregion Constructor

#pragma region Public Methods
void ObjectSelectorItem::Clear()
{
	m_Name = _T("");
	m_Location = _T("");
	m_DisplayLocation = _T("");
	m_ItemTypeName = _T("");
	m_TreeItem = NULL;
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

const SVString& ObjectSelectorItem::getName() const
{
	return m_Name;
}

void ObjectSelectorItem::setName( const SVString& rName )
{
	m_Name = rName;
}

const SVString& ObjectSelectorItem::getLocation() const
{
	return m_Location;
}

void ObjectSelectorItem::setLocation( const SVString& rLocation )
{
	m_Location = rLocation;
}

const SVString& ObjectSelectorItem::getDisplayLocation() const
{
	return m_DisplayLocation;
}

void ObjectSelectorItem::setDisplayLocation( const SVString& rDisplayLocation )
{
	m_DisplayLocation = rDisplayLocation;
}

const SVString& ObjectSelectorItem::getItemTypeName() const
{
	return m_ItemTypeName;
}

void ObjectSelectorItem::setItemTypeName( const SVString& ItemTypeName )
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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTreeLibrary\ObjectSelectorItem.cpp_v  $
 * 
 *    Rev 1.3   16 Dec 2014 18:16:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed types from ItemTypeEnum to SVString.  Changed the method getItemType to getItemTypeName.  Changed the method setItemType to setItemTypeName.  Changed m_ItemType to m_ItemTypeName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   04 Dec 2014 09:02:54   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Single object selection mode state for different icons
 * Added Methods getDisplayLocation;setDisplayLocation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   25 Aug 2014 08:33:18   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added disabled checked states
 * Object Selector displays nodes disabled when in single select mode
 * Changed methods: Clear
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 17:04:46   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/