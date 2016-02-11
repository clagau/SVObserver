//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is the data items to be used for the object selector
//******************************************************************************
#pragma once

#pragma region Includes
#include <comdef.h>
#include "ObjectInterfaces/ISelectorItem.h"
#pragma endregion Includes

namespace Seidenader { namespace ObjectSelectorLibrary
{
	class SelectorItem : public SvOi::ISelectorItem
	{
	#pragma region Constructor
	public:
		SelectorItem();
		SelectorItem( const SelectorItem& rItem );

		virtual ~SelectorItem();
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

		//************************************
		//! The method gets the name of the item
		//! \return the name of the item
		//************************************
		virtual const SVString& getName() const { return m_Name; };

		//************************************
		//! The method sets the name of the item
		//! \param rName <in> string name of the item
		//************************************
		virtual void setName( LPCTSTR Name ) { m_Name = Name; };

		//************************************
		//! The method gets the location of the item
		//! \return the location of the item
		//************************************
		virtual const SVString& getLocation() const { return m_Location; };

		//************************************
		//! The method sets the location of the item
		//! \param rLocation <in> location of the item as a string
		//************************************
		virtual void setLocation( LPCTSTR Location ) { m_Location = Location; };

		//************************************
		//! The method gets the display location of the item
		//! \return the display location of the item
		//************************************
		virtual const SVString& getDisplayLocation() const { return m_DisplayLocation; };

		//************************************
		//! The method sets the location of the item
		//! \param rDisplayLocation <in> display location of the item as a string
		//************************************
		virtual void setDisplayLocation( LPCTSTR DisplayLocation ) { m_DisplayLocation = DisplayLocation; };

		//************************************
		//! The method gets the name of the item type
		//! \return the type of the item as a string
		//************************************
		virtual const SVString& getItemTypeName() const { return m_ItemTypeName; };

		//************************************
		//! The method sets the name of the item type
		//! \param rItemType <in> string indicating the item's type
		//************************************
		virtual void setItemTypeName( LPCTSTR ItemTypeName ) { m_ItemTypeName = ItemTypeName; };

		//************************************
		//! The method gets the item key (unique identifier)
		//! \return the item key as a variant
		//************************************
		virtual const _variant_t& getItemKey() const {return m_ItemKey; };

		//************************************
		//! The method sets the item key (unique identifier) 
		//! \param rItemKey <in> a reference to the key for the item
		//************************************
		virtual void setItemKey( const _variant_t& rItemKey ) { m_ItemKey = rItemKey; };

		//************************************
		//! The method returns the selected state the state
		//! \return the checked state
		//************************************
		virtual bool isSelected() const { return m_Selected; };

		//************************************
		//! The method sets the selected state
		//! \param Selected <in> the selected state to set the item to
		//************************************
		virtual void setSelected( bool Selected ) { m_Selected = Selected; };

		//************************************
		//! The method gets the array index
		//! \return the array index
		//************************************
		virtual int getArrayIndex() const { return m_ArrayIndex; };

		//************************************
		//! The method sets the array index
		//! \param rIndex <in> the array index
		//************************************
		virtual void setArrayIndex( int Index );

		//************************************
		//! The method checks if the item is an array
		//! \param isArray <in> true if item is an array
		//! \return True if the item is an array
		//************************************
		virtual void setArray(bool isArray ) { m_Array = isArray; };

		//************************************
		//! The method checks if the item is an array
		//! \return True if the item is an array
		//************************************
		virtual bool isArray() const { return m_Array; };
	#pragma endregion Public Methods

	#pragma region Member Variables
	private:
		SVString			m_Name;						//The name of the item
		SVString			m_Location;					//The location of the item
		SVString			m_DisplayLocation;			//The display location of the item
		SVString			m_ItemTypeName;				//The name of the data type of the item
		_variant_t			m_ItemKey;					//The item key
		bool				m_Array;					//Is array
		int					m_ArrayIndex;				//The array index
		bool				m_Selected;					//Item selected
	#pragma endregion Member Variables
	};

} /* namespace ObjectSelectorLibrary */ } /* namespace Seidenader */

namespace SvOsl = Seidenader::ObjectSelectorLibrary;
