//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SelectorItem.h"
#pragma endregion Includes

#pragma region Declarations

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

namespace Seidenader { namespace ObjectSelectorLibrary
{

	#pragma region Constructor
	SelectorItem::SelectorItem() :
	  m_Array( false )
	, m_ArrayIndex(-1)
	, m_Selected( false )
	{
	}

	SelectorItem::SelectorItem( const SelectorItem& rItem ) :
	  m_Array( false )
	, m_ArrayIndex(-1)
	, m_Selected( false ) 
	{
		*this = rItem;
	}

	SelectorItem::~SelectorItem()
	{
	}
	#pragma endregion Constructor

	#pragma region Public Methods
	void SelectorItem::Clear()
	{
		m_Name = _T("");
		m_Location = _T("");
		m_DisplayLocation = _T("");
		m_ItemTypeName = _T("");
		m_ItemKey = _T("");
		m_Array = false;
		m_ArrayIndex = -1;
		m_Selected = false;
	}

	const SelectorItem& SelectorItem::operator=( const SelectorItem& rItem )
	{
		if( this != &rItem )
		{
			m_Name = rItem.m_Name;
			m_Location = rItem.m_Location;
			m_DisplayLocation = rItem.m_DisplayLocation;
			m_ItemTypeName = rItem.m_ItemTypeName;
			m_ItemKey = rItem.m_ItemKey;
			m_Array = rItem.m_Array;
			m_ArrayIndex = rItem.m_ArrayIndex;
			m_Selected = rItem.m_Selected;
		}
		return *this;
	}

	bool SelectorItem::operator==( const SelectorItem& rObject ) const
	{
		bool Result = false;

		if( (m_Name == rObject.m_Name) &&
			(m_Location == rObject.m_Location) &&
			(m_DisplayLocation == rObject.m_DisplayLocation) &&
			(m_ItemTypeName == rObject.m_ItemTypeName) &&
			(m_ItemKey == rObject.m_ItemKey) &&
			(m_Array == rObject.m_Array) &&
			(m_ArrayIndex == rObject.m_ArrayIndex) &&
			(m_Selected == rObject.m_Selected))
		{
			Result = true;
		}

		return Result;
	}

	bool SelectorItem::operator!=( const SelectorItem& rObject ) const
	{
		return !( *this == rObject );
	}

	void SelectorItem::setArrayIndex( int Index )
	{
		m_ArrayIndex = Index;
		//If index is not -1 then it is an array
		if( -1 != m_ArrayIndex )
		{
			m_Array  = true;
		}
	}
	#pragma endregion Public Methods
} /* namespace ObjectSelectorLibrary */ } /* namespace Seidenader */

