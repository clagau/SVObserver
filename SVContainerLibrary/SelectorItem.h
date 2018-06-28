//*****************************************************************************
/// \copyright (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
/// \file SelectorItem.h
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <iterator>
//Moved to precompiled header: #include <comdef.h>
#include "SVUtilityLibrary/SVGUID.h"
#pragma endregion Includes

namespace SvCl
{
	struct SelectorItem
	{
	#pragma region Constructor
	public:
		SelectorItem() {};
		SelectorItem( const SelectorItem& rItem ) = default;

		virtual ~SelectorItem() {};
	#pragma endregion Constructor

	#pragma region Public Methods
	public:
		//************************************
		//! The method clears all values stored in the item
		//************************************
		void Clear();

		//************************************
		//! The assignment operator
		//! \param rItem <in> a reference to another item
		//! \return a reference to itself
		//************************************
		const SelectorItem& operator=( const SelectorItem& rItem );

		//************************************
		//! The method checks if the items are the same
		//! \param rItem <in> a reference to another item
		//! \returns true if the same
		//************************************
		bool operator==( const SelectorItem& rObject ) const;

		//************************************
		//! The method checks if the items are different
		//! \param rItem <in> a reference to another item
		//! \return True if different
		//************************************
		bool operator!=( const SelectorItem& rItem ) const;

	#pragma endregion Public Methods

	#pragma region Member Variables
	public:
		std::string			m_Name;						//The name of the item
		std::string			m_Location;					//The location of the item
		std::string			m_DisplayLocation;			//The display location of the item
		std::string			m_ItemTypeName;				//The name of the data type of the item
		SVGUID				m_ItemKey;					//The item key
		bool				m_Selected{ false };		//Item selected
		bool				m_Array{ false };			//Is array
		int					m_ArrayIndex{ -1 };			//The array index
	#pragma endregion Member Variables
	};
	typedef std::vector<SelectorItem> SelectorItemVector;
	typedef std::back_insert_iterator<SelectorItemVector> SelectorItemInserter;
} //namespace SvCl
