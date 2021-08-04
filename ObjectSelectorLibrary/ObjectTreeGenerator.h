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
#include "SVProtoBuf/SVRC.h"
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
			TypeNone,
			TypeSingleObject,			//! Single selection mode can only select one item at any time
			TypeMultipleObject,			//! Multiple selection mode
		};

	#pragma endregion Declarations

	public:
	#pragma region Constructor
		ObjectTreeGenerator() = default;

		virtual ~ObjectTreeGenerator() = default;
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
		void insertTreeObjects(const SvPb::TreeItem& rTreeItem);

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
		//! The method gets the list of selected objects
		//! \return the reference to the results
		//************************************
		inline const SvDef::StringVector& getSelectedObjects() const { return m_SelectedObjects; }

		//************************************
		//! The method gets the list of modified objects
		//! \return the reference to the modified objects
		//************************************
		inline const SvDef::StringVector& getModifiedObjects() const { return m_ModifiedObjects; }

		//************************************
		//! The method gets the single object selection result
		//! \return the single selected object
		//************************************
		std::string getSingleObjectResult() const;

		//************************************
		//! The method sets the selector type
		//! \param rSelectorType <in> reference to the object selector type
		//! \param helpID <in> the ID for the help file
		//************************************
		void setSelectorType( const SelectorTypeEnum& rSelectorType, int helpID = 0);

		SvCl::ObjectTreeItems& getTreeContainer() { return m_TreeContainer; };

		//************************************
		//! The method checks if the tree has been modified
		//! \return true if tree has been modified
		//************************************
		bool checkModifiedItems();
#pragma endregion Public Methods

	private:
	#pragma region Private Methods
		void insertChildren(const SvPb::TreeItem& rTreeItem);

		//************************************
		//! The method inserts an object into the tree list
		//! \param rTreeItem <in> reference to tree item to insert
		//************************************
		void insertTreeObject(const SvPb::TreeItem& rTreeItem);
#pragma endregion Private Methods

	private:
	#pragma region Member Variables
		SvCl::ObjectTreeItems	m_TreeContainer;	//The tree container to store all tree items
		SvDef::StringVector m_SelectedObjects;		//The selected objects
		SvDef::StringVector m_ModifiedObjects;		//The modified objects
		SelectorTypeEnum m_SelectorType{TypeNone};	//The selector type
		int m_helpID{0};							//The ID for the help file
		long m_LeafCount{0L};						//The number of leafs in the selector (only as debug information)
	#pragma endregion Member Variables
	};
} //namespace SvOsl
