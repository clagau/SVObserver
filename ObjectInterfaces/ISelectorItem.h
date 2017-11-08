//*****************************************************************************
/// \copyright (c) 2015,2015 by Seidenader Maschinenbau GmbH
/// All Rights Reserved 
//*****************************************************************************
/// This is an interface to access items to be used for the object selector
//******************************************************************************

#pragma once
#pragma region Includes

#pragma endregion Includes

namespace SvOi
{
	interface ISelectorItem abstract
	{
	public:
	#pragma region Declarations
	public:
	#pragma region Public Methods
		//************************************
		//! The method gets the name of the item
		//! \return the name of the item
		//************************************
		virtual const std::string& getName() const = 0;

		//************************************
		//! The method sets the name of the item
		//! \param rName <in> string name of the item
		//************************************
		virtual void setName( LPCTSTR Name ) = 0;

		//************************************
		//! The method gets the location of the item
		//! \return the location of the item
		//************************************
		virtual const std::string& getLocation() const = 0;

		//************************************
		//! The method sets the location of the item
		//! \param rLocation <in> location of the item as a string
		//************************************
		virtual void setLocation( LPCTSTR rLocation ) = 0;

		//************************************
		//! The method gets the display location of the item
		//! \return the display location of the item
		//************************************
		virtual const std::string& getDisplayLocation() const = 0;

		//************************************
		//! The method sets the location of the item
		//! \param rDisplayLocation <in> display location of the item as a string
		//************************************
		virtual void setDisplayLocation( LPCTSTR rDisplayLocation ) = 0;

		//************************************
		//! The method gets the name of the item type
		//! \return the type of the item as a string
		//************************************
		virtual const std::string& getItemTypeName() const = 0;

		//************************************
		//! The method sets the name of the item type
		//! \param rItemType <in> string indicating the item's type
		//************************************
		virtual void setItemTypeName( LPCTSTR ItemTypeName ) = 0;

		//************************************
		//! The method gets the item key (unique identifier)
		//! \return the item key as a variant
		//************************************
		virtual const _variant_t& getItemKey() const = 0;

		//************************************
		//! The method sets the item key (unique identifier) 
		//! \param rItemKey <in> a reference to the key for the item
		//************************************
		virtual void setItemKey( const _variant_t& rItemKey ) = 0;

		//************************************
		//! The method returns the selected state the state
		//! \return the checked state
		//************************************
		virtual bool isSelected() const = 0;

		//************************************
		//! The method sets the selected state
		//! \param Selected <in> the selected state to set the item to
		//************************************
		virtual void setSelected( bool Selected ) = 0;

		//************************************
		//! The method gets the array index
		//! \return the array index
		//************************************
		virtual int getArrayIndex() const = 0;

		//************************************
		//! The method sets the array index
		//! \param rIndex <in> the array index
		//************************************
		virtual void setArrayIndex( int Index ) = 0;

		//************************************
		//! The method checks if the item is an array
		//! \param isArray <in> true if item is an array
		//! \return True if the item is an array
		//************************************
		virtual void setArray(bool isArray ) = 0;

		//************************************
		//! The method checks if the item is an array
		//! \return True if the item is an array
		//************************************
		virtual bool isArray() const = 0;

	#pragma endregion Public Methods
	};
} //namespace SvOi
