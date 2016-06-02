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
#include "ObjectInterfaces\ISelectorItemVector.h"
#include "SVContainerLibrary/ObjectTreeItems.h"
#include "SelectorItem.h"
#include "SelectorItemVector.h"
#include "SelectorOptions.h"
#pragma endregion Includes

namespace Seidenader { namespace ObjectSelectorLibrary
{
	class ObjectTreeGenerator
	{
	public:
	#pragma region Declarations
		/**********
			The object selector dialog type enumerator
		***********/
		enum SelectorTypeEnum
		{
			TypeNone				= 0,
			TypeSetAttributes		= 1 << 1,
			TypeSingleObject		= 1 << 2,
			TypeMultipleObject		= 1 << 3,
		};

		/**********
			The filter type enumerator
		***********/
		enum FilterEnum
		{
			FilterInput,
			FilterOutput
		};
	#pragma endregion Declarations

	public:
	#pragma region Constructor
		/**********
			The class constructor
		***********/
		ObjectTreeGenerator();

		/**********
			The class destructor
		***********/
		virtual ~ObjectTreeGenerator();
	#pragma endregion Constructor

	public:
	#pragma region Public Methods
		/**********
			The method returns the instance to the class
			\return the reference to the static object
		***********/
		static ObjectTreeGenerator& Instance();

		/**********
			The method clears all object values
			\param ClearAll <in> true to clear all values
		***********/
		void Clear( bool ClearAll = true );

		//************************************
		//! The method inserts tree items
		//! \param rSelectedItems <in> const reference to a list of selector items
		//************************************
		void insertTreeObjects( const SelectorItemVector& rSelectorItems );

		/**********
		/// This method inserts items in the tree
		/// \param rOptions <in> reference to the Selector options
		***********/
		template <typename GlobalSelector, typename PPQSelector, typename ToolsetSelector>
		void BuildSelectableItems( const SelectorOptions& rOptions );

		//**********
		// Description:  The method displays the object selector dialog
		// Parameter:  title <in>:  dialog title
		// Parameter:  mainTabTitle <in>:  title for the main tab
		// Parameter:  filterTabTitle <in>:  title for the filter tab
		// Parameter:  pParent <in>:  pointer to the parent, default is nullptr
		// Return:  INT_PTR:  the result of the dialog
		//***********
		INT_PTR showDialog( LPCTSTR title, LPCTSTR mainTabTitle, LPCTSTR filterTabTitle, CWnd* pParent = nullptr );

		/**********
			The method checks the list of items
			\param rItems <in> the set of items to check
			\return true if successful
		***********/
		bool setCheckItems( const SVStringSet& rItems );

		/**********
			The method sets the location filter
			\param rType <in> reference to the filter type
			\param rFilter <in> reference to the filter to set
			\param rReplace <in> the reference to the replace text
		***********/
		void setLocationFilter( const FilterEnum& rType, const SVString& rFilter, const SVString& rReplace );

		//************************************
		// Description:  The method converts the location if the object is an array.
		// Parameter:  rItem <in>:  reference to the item to check
		// Returns:  The new location with inserted name
		//************************************
		SVString convertObjectArrayName( const SvOi::ISelectorItem& rItem ) const;

		//************************************
		// Description:  The method gets the list of selected objects
		// Returns:  const SelectorItemVector& - the reference to the results
		//************************************
		inline const SelectorItemVector& getSelectedObjects() const;

		//************************************
		// Description:  The method gets the list of modified objects
		// Returns:  const SelectorItemVector& - the reference to the results
		//************************************
		inline const SelectorItemVector& getModifiedObjects() const;

		/**********
			The method gets the single object selection result
			\return The single select object
		***********/
		inline SelectorItem getSingleObjectResult() const;

		/**********
			The method sets the selector type
			\param rSelectorType <in> reference to the object selector type
		***********/
		inline void setSelectorType( const SelectorTypeEnum& rSelectorType );

	#pragma endregion Public Methods

	private:
	#pragma region Private Methods
		/**********
			The method inserts an object into the tree list
			\param rObjectRef <in> reference to the object
		***********/
		void insertTreeObject( const SvOi::ISelectorItem& rItem );

		/**********
			The method checks if the tree has been modified
			\return True if tree has been modified
		***********/
		bool checkModifiedItems();

		/**********
			The method sets the attributes of all leaf items that were modified
		***********/
		void setItemAttributes();

		/**********
			The method filter a location and return it. 
			\param rFilters<in>, reference to the filters to be used
			\param rLocation <in> reference to the location before filtering
			\return filtered location
		***********/
		SVString getFilteredLocation( const TranslateMap& rFilters, const SVString& rLocation ) const;

		/**********
		The method converts the location to the required format using the location filter and array index
		***********/
		void convertLocation( SelectorItem& rSelectedItem );
	#pragma endregion Private Methods

	private:
	#pragma region Member Variables
		SvCl::ObjectTreeItems	m_TreeContainer;	//The tree container to store all tree items
		SelectorItemVector m_SelectedObjects;		//The selected objects
		SelectorItemVector m_ModifiedObjects;		//The modified objects
		TranslateMap m_LocationInputFilters;		//The location input filters
		TranslateMap m_LocationOutputFilters;		//The location output filters
		SelectorTypeEnum m_SelectorType;			//The selector type
		UINT m_AttributesFilter;					//The attributes filter
		long m_LeafCount;							//The number of leafs in the selector (only as debug information)
	#pragma endregion Member Variables
	};
} /* namespace ObjectSelectorLibrary */ } /* namespace Seidenader */

#pragma region Inline
#include "ObjectTreeGenerator.inl"
#pragma endregion Inline

namespace SvOsl = Seidenader::ObjectSelectorLibrary;
