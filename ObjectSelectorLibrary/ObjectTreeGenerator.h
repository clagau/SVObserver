//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectTreeGenerator
//* .File Name       : $Workfile:   ObjectTreeGenerator.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   07 Jan 2015 18:10:22  $
//* ----------------------------------------------------------------------------
//* This class is the interface between the object manager and object selector
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <assert.h>
#include "Definitions/StringTypeDef.h"
#include "SVContainerLibrary/ObjectTreeItems.h"
#include "SVUtilityLibrary/SVGUID.h"
#include "SVContainerLibrary/SelectorItem.h"
#include "SVContainerLibrary/SelectorItem.h"
#pragma endregion Includes

namespace SvOsl
{
	class ObjectTreeGenerator
	{
	public:
	#pragma region Declarations
		//************************************
		//! The object selector dialog type enumerator
		//************************************
		enum SelectorTypeEnum
		{
			TypeNone				= 0x00,
			TypeSetAttributes		= 0x01,			//! Selected items are set using the object attributes
			TypeSingleObject		= 0x02,			//! Single selection mode can only select one item at any time
			TypeMultipleObject		= 0x04,			//! Multiple selection mode
		};

		//************************************
		//! The filter type enumerator either input or output name filter
		//************************************
		enum FilterEnum
		{
			FilterInput,
			FilterOutput
		};
	#pragma endregion Declarations

	public:
	#pragma region Constructor
		ObjectTreeGenerator();

		virtual ~ObjectTreeGenerator();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		//************************************
		//! The method returns the singleton of the class
		//! \return the reference to the static object
		//************************************
		static ObjectTreeGenerator& Instance();

		//************************************
		//! The method clears all items from the selector
		//! \param ClearAll <in> true to clear the selected and modified lists
		//************************************
		void Clear( bool ClearAll = true );

		//************************************
		//! The method inserts tree items
		//! \param rSelectorItems <in> const reference to a list of selector items
		//************************************
		void insertTreeObjects(const SvCl::SelectorItemVector& rSelectorItems);

		//************************************
		//! The method displays the object selector dialog
		//! \param title <in>:  dialog title
		//! \param mainTabTitle <in>:  title for the main tab
		//! \param filterTabTitle <in>:  title for the filter tab
		//! \param pParent <in>:  pointer to the parent, default is nullptr
		//! \return the result of the dialog
		//************************************
		INT_PTR showDialog( LPCTSTR title, LPCTSTR mainTabTitle, LPCTSTR filterTabTitle, CWnd* pParent = nullptr );

		//************************************
		//! The method checks the items listed in the tree selector
		//! \param rItems <in> the set of items to check as names
		//! \return true if successful
		//************************************
		bool setCheckItems( const SvDef::StringSet& rItems );

		//************************************
		//! The method sets the location filter
		//! \param rType <in> reference to the filter type
		//! \param rFilter <in> reference to the filter to set
		//! \param rReplace <in> the reference to the replace text
		//************************************
		void setLocationFilter( const FilterEnum& rType, const std::string& rFilter, const std::string& rReplace );

		//************************************
		//! The method converts the location if the object is an array.
		//! \param rItem <in>:  reference to the item to check
		//! \return the new location with inserted name
		//************************************
		std::string convertObjectArrayName( const SvCl::SelectorItem& rItem ) const;

		//************************************
		//! The method gets the list of selected objects
		//! \return the reference to the results
		//************************************
		inline const SvCl::SelectorItemVector& getSelectedObjects() const;

		//************************************
		//! The method gets the list of modified objects
		//! \return the reference to the modified objects
		//************************************
		inline const SvCl::SelectorItemVector& getModifiedObjects() const;

		//************************************
		//! The method gets the single object selection result
		//! \return the single selected object
		//************************************
		inline SvCl::SelectorItem getSingleObjectResult() const;

		//************************************
		//! The method sets the selector type
		//! \param rSelectorType <in> reference to the object selector type
		//************************************
		inline void setSelectorType( const SelectorTypeEnum& rSelectorType );

	#pragma endregion Public Methods

	private:
	#pragma region Private Methods
		//************************************
		//! The method inserts an object into the tree list
		//! \param rObjectRef <in> reference to the object
		//************************************
		void insertTreeObject( const SvCl::SelectorItem& rItem );

		//************************************
		//! The method checks if the tree has been modified
		//! \return true if tree has been modified
		//************************************
		bool checkModifiedItems();

		//************************************
		//! The method sets the attributes of all leaf items that were modified
		//************************************
		void setItemAttributes();

		//************************************
		//! The method filters a location and returns the result
		//! \param rFilters<in>, reference to the filters to be used
		//! \param rLocation <in> reference to the location before filtering
		//! \return filtered location
		//************************************
		std::string getFilteredLocation( const SvDef::TranslateMap& rFilters, const std::string& rLocation ) const;

		//************************************
		//! The method converts the location to the required format using the location filter and array index
		//************************************
		void convertLocation(SvCl::SelectorItem& rSelectedItem );
	#pragma endregion Private Methods

	private:
	#pragma region Member Variables
		SvCl::ObjectTreeItems	m_TreeContainer;	//The tree container to store all tree items
		SvCl::SelectorItemVector m_SelectedObjects;//The selected objects
		SvCl::SelectorItemVector m_ModifiedObjects;//The modified objects
		SvDef::TranslateMap m_LocationInputFilters;	//The location input filters
		SvDef::TranslateMap m_LocationOutputFilters;//The location output filters
		SelectorTypeEnum m_SelectorType;			//The selector type
		UINT m_AttributesFilter;					//The attributes filter
		long m_LeafCount;							//The number of leafs in the selector (only as debug information)
	#pragma endregion Member Variables
	};
} //namespace SvOsl

#pragma region Inline
#include "ObjectTreeGenerator.inl"
#pragma endregion Inline
