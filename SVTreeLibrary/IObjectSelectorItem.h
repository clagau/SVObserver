//******************************************************************************
//* COPYRIGHT (c) 2013 by Seidenader, Markt Schwaben
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : IObjectSelectorItem
//* .File Name       : $Workfile:   IObjectSelectorItem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Jul 2014 17:04:46  $
//* ----------------------------------------------------------------------------
//* This interface is used to define a single selector item for the tree
//******************************************************************************

#pragma once
#pragma region Includes
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVTreeLibrary
	{
		interface IObjectSelectorItem abstract
		{
		public:
		#pragma region Declarations
			/**********
			 The selector item state enumerations
			***********/
			enum AttributeEnum
			{
				AttributeNone	= 0,
				Node			= 1 << 1,
				Leaf			= 1 << 2,
				Checkable		= 1 << 3,
				NameEditable	= 1 << 4,
				Array			= 1 << 5
			};

			/**********
			 The selector item checked state enumerations
			***********/
			enum CheckedStateEnum
			{
				CheckedStateNone,
				Unchecked,
				Checked,
				TriState
			};

			/**********
			 The selector item type enumerations
			***********/
			enum ItemTypeEnum
			{
				ItNone,
				ItDword,
				ItLong,
				ItDouble,
				ItBool,
				ItPoint,
				ItByte,
				ItString,
				ItInt64,
				ItChar,
				ItVariant
			};
		#pragma endregion Declarations

		public:
		#pragma region Public Methods
			/**********
			 The method gets the name of the item
			 \return the name of the item
			***********/
			virtual const SVString& getName() const = 0;

			/**********
			 The method sets the name of the item
			 \param rName <in> a reference to the name of the item
			***********/
			virtual void setName( const SVString& rName ) = 0;

			/**********
			 The method gets the location of the item
			 \return the location of the item
			***********/
			virtual const SVString& getLocation() const = 0;

			/**********
			 The method sets the location of the item
			 \param rLocation <in> a reference to the location of the item
			***********/
			virtual void setLocation( const SVString& rLocation ) = 0;

			/**********
			 The method gets the item type
			 \return the type of the item
			***********/
			virtual const ItemTypeEnum& getItemType() const = 0;

			/**********
			 The method sets the tree path of the item
			 \param rItemType <in> a reference to the type of the item
			***********/
			virtual void setItemType( const ItemTypeEnum ItemType ) = 0;

			/**********
			 The method gets the handle to the corresponding tree item
			 \return the handle of the tree item
			***********/
			virtual const HTREEITEM& getTreeItem() const = 0;

			/**********
			 The method sets the handle of the tree item
			 \param rItem <in> a reference to the handle of the item
			***********/
			virtual void setTreeItem( const HTREEITEM& rItem ) = 0;

			/**********
			 The method gets the item key (unique identifier)
			 \return the item key as a variant
			***********/
			virtual const _variant_t& getItemKey() const = 0;

			/**********
			 The method sets the item key (unique identifier) 
			 \param rItemKey <in> a reference to the key for the item
			***********/
			virtual void setItemKey( const _variant_t& rItemKey ) = 0;

			/**********
			 The method gets the attributes of the item
			 \return the item attribute
			***********/
			virtual const AttributeEnum& getAttribute() const = 0;

			/**********
			 The method sets the item attribute
			 \param rAttribute <in> the attribute to set the item to
			***********/
			virtual void setAttibute( const AttributeEnum& rAttribute ) = 0;

			/**********
			 The method gets the corresponding item icon number (icon for tree)
			 \return the item icon number
			***********/
			virtual int getIconNumber() const = 0;

			/**********
			 The method sets the item icon number
			 \param IconNumber <in> the icon number to set the item to
			***********/
			virtual void setIconNumber( const int IconNumber ) = 0;

			/**********
			 The method gets the checked state
			 \return the checked state
			***********/
			virtual const CheckedStateEnum& getCheckedState() const = 0;

			/**********
			 The method sets the checked state
			 \param Checked <in> the checked state to set the item to
			***********/
			virtual void setCheckedState( const CheckedStateEnum& rCheckedState ) = 0;

			/**********
			 The method gets the original checked state
			 \return the original checked state
			***********/
			virtual const CheckedStateEnum& getOrgCheckedState() const = 0;

			/**********
			 The method sets the original checked state
			 \param Checked <in> the checked state to set the item to
			***********/
			virtual void setOrgCheckedState( const CheckedStateEnum& rCheckedState ) = 0;

			/**********
			 The method gets the array index
			 \return the array index
			***********/
			virtual const int& getArrayIndex() const = 0;

			/**********
			 The method sets the array index
			 \param rIndex <in> the array index
			***********/
			virtual void setArrayIndex( const int& rIndex ) = 0;

			/**********
			 The method sets the state of the modified flag
			 \param Modified <in> the state to set the flag to
			***********/
			virtual void setModified( const bool Modified ) = 0;

			/**********
			 The method checks if the item is a node
			 \return True if the item is a node
			***********/
			virtual bool isNode() const = 0;

			/**********
			 The method checks if the item is a leaf
			 \return True if the item is a leaf
			***********/
			virtual bool isLeaf() const = 0;

			/**********
			 The method checks if the item is an array
			 \return True if the item is an array
			***********/
			virtual bool isArray() const = 0;

			/**********
			 The method checks if the item has been modified
			 \return True if the item has been modified
			***********/
			virtual bool isModified() const = 0;
		#pragma endregion Public Methods
		};
	} //namespace SVObserver
} //namespace Seidenader

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTreeLibrary\IObjectSelectorItem.h_v  $
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
