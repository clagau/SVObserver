//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file SelectorItem.cpp
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SelectorItem.h"
#pragma endregion Includes

namespace SvCl
{

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

	const SelectorItem& SelectorItem::operator=( const SelectorItem& rRhs )
	{
		if( this != &rRhs )
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

	bool SelectorItem::operator==( const SelectorItem& rRhs ) const
	{
		bool Result = false;

		if( (m_Name == rRhs.m_Name) &&
			(m_Location == rRhs.m_Location) &&
			(m_DisplayLocation == rRhs.m_DisplayLocation) &&
			(m_ItemTypeName == rRhs.m_ItemTypeName) &&
			(m_ItemKey == rRhs.m_ItemKey) &&
			(m_Array == rRhs.m_Array) &&
			(m_ArrayIndex == rRhs.m_ArrayIndex) &&
			(m_Selected == rRhs.m_Selected))
		{
			Result = true;
		}

		return Result;
	}

	bool SelectorItem::operator!=( const SelectorItem& rRhs ) const
	{
		return !( *this == rRhs );
	}

	#pragma endregion Public Methods
} //namespace SvCl

