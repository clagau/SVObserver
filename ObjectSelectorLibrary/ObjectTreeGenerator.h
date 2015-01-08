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
#include "SVObjectLibrary\SVObjectReference.h"
#include "SVObjectLibrary\SVOutputInfoListClass.h"
#include "SVTreeLibrary\ObjectTreeItems.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace ObjectSelectorLibrary
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

			/**********
			 The method sets the attribute filters
			 \param AttributesSetFilter <in> attributes set filter
			 \param AttributesAllowedFilter <in> attributes allowed filter
			***********/
			void setAttributeFilters( UINT AttributesSetFilter, UINT AttributesAllowedFilter = 0 );

			/**********
			 The method inserts tree objects using the branch name
			 \param rTreeName <in> reference to the tree name to insert
			***********/
			void insertTreeObjects( const SVString& rTreeName );

			/**********
			 The method inserts tree objects using a names list
			 \param rLocationList <in> reference to the list of object locations
			***********/
			void insertTreeObjects( const SVStringArray& rLocationList );

			/**********
			 The method inserts a list of objects into the tree
			 \param rObjectList <in> reference to the object list
			***********/
			void insertTreeObjects( const SVObjectReferenceVector& rObjectList );

			/**********
			 The method inserts a list of objects into the tree
			 \param rTree <in> reference to the object tree list
			***********/
			void insertTreeObjects( const SVTreeLibrary::ObjectTreeItems& rTree );

			/**********
			 The method inserts an object into the tree list
			 \param rObjectRef <in> reference to the object
			***********/
			void insertTreeObject( const SVObjectReference& rObjectRef );

			/**********
			 The method inserts an object into the tree list
			 \param rLocation <in> reference to the item location
			***********/
			void insertTreeObject( const SVString& rLocation );

			/**********
			 The method inserts an object into the tree list
			 \param rObjectItem <in> reference to a object selector item
			***********/
			void insertTreeObject( const SVTreeLibrary::ObjectSelectorItem& rObjectItem );

			/**********
			 The method inserts the tool set output object list
			 \param rOutputList <out> reference to the output list
			***********/
			void insertOutputList( SVOutputInfoListClass& rOutputList );

			//**********
			// Description:  The method displays the object selector dialog
			// Parameter:  title <in>:  reference to the dialog title
			// Parameter:  mainTabTitle <in>:  reference to the title for the main tab
			// Parameter:  filterTabTitle <in>:  reference to the title for the filter tab
			// Parameter:  pParent <in>:  pointer to the parent, default is nullptr
			// Return:  INT_PTR:  the result of the dialog
			//***********
			INT_PTR showDialog( const SVString& title, const SVString& mainTabTitle, const SVString& filterTabTitle, CWnd* pParent = nullptr );

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
			// Parameter:  rObjectRef <in>:  reference to the object to check
			// Parameter:  rLocation <in, out>:  reference to the location which is changed if an array
			// Returns:  int - the object index number (if array) or -1 (if not an array)
			//************************************
			int convertObjectArrayName( const SVObjectReference& rObjectRef, SVString& rLocation ) const;

			/**********
			 The method sets the whole array flag
			***********/
			inline void setAllowWholeArrays( bool AllowWholeArrays );

			/**********
			 The method gets the whole array flag
			***********/
			inline bool getAllowWholeArrays() const;

			//************************************
			// Description:  The method gets the results of the object selection
			// Returns:  const ObjectSelectorItems& - the reference to the results
			//************************************
			inline const SVTreeLibrary::ObjectSelectorItems& getResults() const;

			/**********
			 The method gets the single object selection result
			 \return The single select object
			***********/
			inline SVTreeLibrary::ObjectSelectorItem getSingleObjectResult() const;

			/**********
			 The method sets the selector type
			 \param rSelectorType <in> reference to the object selector type
			***********/
			inline void setSelectorType( const SelectorTypeEnum& rSelectorType );

			/**********
			 The method sets the selector item variable type int string etc.. 
			 \param rObjectRef <in> reference to the object
			 \param rSelectorItem <in, out> reference to the selector item
			***********/
			static void setSelectorItemType( const SVObjectReference& rObjectRef, SVTreeLibrary::ObjectSelectorItem& rSelectorItem );
		#pragma endregion Public Methods

		private:
		#pragma region Private Methods
			/**********
			 The method filters the object list
			 \param rOutputList <in> reference to the output list (Tried to make it const however failed due to methods not const)
			 \param rObjectList <out> reference to the object list
			***********/
			void filterObjects( SVOutputInfoListClass& rOutputList, SVObjectReferenceVector& rObjectList );

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
			void convertLocation();
		#pragma endregion Private Methods

		private:
		#pragma region Member Variables
			SVTreeLibrary::ObjectTreeItems	m_TreeContainer;//The tree container to store all tree items
			SVTreeLibrary::ObjectSelectorItems m_Results;	//The checked results
			TranslateMap m_LocationInputFilters;			//The location input filters
			TranslateMap m_LocationOutputFilters;			//The location output filters
			SelectorTypeEnum m_SelectorType;				//The selector type
			UINT m_AttributesSetFilter;						//The attribute showing the set items
			UINT m_AttributesAllowedFilter;					//The attribute allowed filter
			bool m_AllowWholeArray;							//Allow whole array selection
		#pragma endregion Member Variables
		};

		#pragma region Inline
		#include "ObjectTreeGenerator.inl"
		#pragma endregion Inline
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\ObjectSelectorLibrary\ObjectTreeGenerator.h_v  $
 * 
 *    Rev 1.3   07 Jan 2015 18:10:22   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added method insertTreeObject.
 * Changed method setSelectorItemType to public.
 * Renamed method checkLocationFilters to getFilteredLocation to fix the issue where environment items did not show type information on the Object Selector Filter page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   16 Dec 2014 17:43:50   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Added a parameter for the showDialog method to specify the text for the title of the Filter page.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Dec 2014 03:31:40   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Added support for list of multiple item checked results
 * Added Methds convertObjectArrayName; convertLocation; getResults
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 11:16:26   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/