//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : ObjectSelectorItem
//* .File Name       : $Workfile:   ObjectSelectorItem.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   16 Dec 2014 18:16:34  $
//* ----------------------------------------------------------------------------
//* This class is used to define a single selector item for the tree
//******************************************************************************

#pragma once

#pragma region Includes
#include <comdef.h>
#include <deque>
#include "IObjectSelectorItem.h"
#include "SVUtilityLibrary/SVSharedPtr.h"
#pragma endregion Includes

namespace Seidenader
{
	namespace SVTreeLibrary
	{
		class ObjectSelectorItem : public IObjectSelectorItem
		{
		public:
		#pragma region Constructor
			/**********
			 The class constructor
			***********/
			ObjectSelectorItem();

			/**********
			 The class copy constructor
			 \param rObject <in> a reference to the object selector item
			***********/
			ObjectSelectorItem( const ObjectSelectorItem& rObject );

			/**********
			 The class destructor
			***********/
			virtual ~ObjectSelectorItem();
		#pragma endregion Constructor

		public:
		#pragma region Public Methods
			/**********
			 The method clears all values stored in the object
			***********/
			void Clear();

			/**********
			 The assignment operator
			 \param rObject <in> a reference to another object
			 \return a reference to itself
			***********/
			const ObjectSelectorItem& operator=( const ObjectSelectorItem& rObject );

			/**********
			 The method checks if the objects are the same
			 \param rObject <in> a reference to another object
			 \return True if the same
			***********/
			bool operator==( const ObjectSelectorItem& rObject ) const;

			/**********
			 The method checks if the objects are different
			 \param rObject <in> a reference to another object
			 \return True if different
			***********/
			bool operator!=( const ObjectSelectorItem& rObject ) const;

			/**********
			 The method gets the name of the item
			 \return the name of the item
			***********/
			virtual const SVString& getName() const override;

			/**********
			 The method sets the name of the item
			 \param rName <in> a reference to the name of the item
			***********/
			virtual void setName( const SVString& rName ) override;

			/**********
			 The method gets the location of the item
			 \return the tree path of the item
			***********/
			virtual const SVString& getLocation() const override;

			/**********
			 The method sets the tree path of the item
			 \param rLocation <in> a reference to the location of the item
			***********/
			virtual void setLocation( const SVString& rLocation ) override;

			/**********
			 The method gets the tree display location of the item
			 \return the display location of the item
			***********/
			virtual const SVString& getDisplayLocation() const override;

			/**********
			 The method sets the tree display location of the item
			 \param rDisplayLocation <in> a reference to the display location of the item
			***********/
			virtual void setDisplayLocation( const SVString& rDisplayLocation ) override;

			/**********
			 The method gets the name of the item type
			 \return the type of the item as a string
			***********/
			virtual const SVString& getItemTypeName() const override;

			/**********
			 The method sets the name of the item type
			 \param rItemType <in> a reference to the string indicating the item's type
			***********/
			virtual void setItemTypeName( const SVString& ItemTypeName ) override;

			/**********
			 The method gets the handle to the corresponding tree item
			 \return the handle of the tree item
			***********/
			virtual const HTREEITEM& getTreeItem() const override;

			/**********
			 The method sets the handle of the tree item
			 \param rItem <in> a reference to the handle of the item
			***********/
			virtual void setTreeItem( const HTREEITEM& rItem ) override;

			/**********
			 The method gets the item key (unique identifier)
			 \return the item key as a variant
			***********/
			virtual const _variant_t& getItemKey() const override;

			/**********
			 The method sets the item key (unique identifier) 
			 \param rItemKey <in> a reference to the key for the item
			***********/
			virtual void setItemKey( const _variant_t& rItemKey ) override;

			/**********
			 The method gets the attributes of the item
			 \return the item attribute
			***********/
			virtual const AttributeEnum& getAttribute() const override;

			/**********
			 The method sets the item attribute
			 \param rAttribute <in> the attribute to set the item to
			***********/
			virtual void setAttibute( const AttributeEnum& rAttribute ) override;

			/**********
			 The method gets the corresponding item icon number (icon for tree)
			 \return the item icon number
			***********/
			virtual int getIconNumber() const override;

			/**********
			 The method sets the item icon number
			 \param IconNumber <in> the icon number to set the item to
			***********/
			virtual void setIconNumber( const int IconNumber ) override;

			/**********
			 The method gets the checked state
			 \param Checked <in> the checked state to set the item to
			***********/
			virtual const CheckedStateEnum& getCheckedState() const override;

			/**********
			 The method sets the state of the checked flag
			 \param Checked <in> the state to set the flag to
			***********/
			virtual void setCheckedState( const CheckedStateEnum& rCheckedState ) override;

			/**********
			 The method gets the original checked state
			 \return the original checked state
			***********/
			virtual const CheckedStateEnum& getOrgCheckedState() const override;

			/**********
			 The method sets the original checked state
			 \param rCheckedState <in> the checked state to set the item to
			***********/
			virtual void setOrgCheckedState( const CheckedStateEnum& rCheckedState ) override;

			/**********
			 The method gets the array index
			 \return the array index
			***********/
			virtual const int& getArrayIndex() const override;

			/**********
			 The method sets the array index
			 \param rIndex <in> 
			***********/
			virtual void setArrayIndex( const int& rIndex ) override;

			/**********
			 The method sets that the state of the modified flag
			 \param Modified <in> the state to set the flag to
			***********/
			virtual void setModified( const bool Modified ) override;

			/**********
			 The method checks if the item is a node
			 \return True if the item is a node
			***********/
			virtual bool isNode() const override;

			/**********
			 The method checks if the item is a leaf
			 \return True if the item is a leaf
			***********/
			virtual bool isLeaf() const override;

			/**********
			 The method checks if the item is an array
			 \return True if the item is an array
			***********/
			virtual bool isArray() const override;

			/**********
			 The method checks if the item has been modified
			 \return True if the item has been modified
			***********/
			virtual bool isModified() const override;
		#pragma endregion Public Methods

		private:
		#pragma region Member Variables
			SVString			m_Name;						//The name of the item
			SVString			m_Location;					//The location of the item
			SVString			m_DisplayLocation;			//The display location of the item
			SVString			m_ItemTypeName;				//The name of the data type of the item
			HTREEITEM			m_TreeItem;					//the corresponding tree item handle
			_variant_t			m_ItemKey;					//The item key
			AttributeEnum		m_Attribute;				//The item attribute
			int					m_IconNumber;				//The corresponding item image number
			int					m_ArrayIndex;				//The array index
			bool				m_Modified;					//Modified flag
			CheckedStateEnum	m_CheckedState;				//The item checked state
			CheckedStateEnum	m_OrgCheckedState;			//The item original checked state
		#pragma endregion Member Variables
		};

		typedef SVSharedPtr< ObjectSelectorItem > ObjectSelectorItemPtr;
	} //namespace SVTreeLibrary
} //namespace Seidenader

namespace SvTrl = Seidenader::SVTreeLibrary;

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTreeLibrary\ObjectSelectorItem.h_v  $
 * 
 *    Rev 1.2   16 Dec 2014 18:16:34   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  933
 * SCR Title:  Add Filter Tab to Object Selector (SVO-377)
 * Checked in by:  mZiegler;  Marc Ziegler
 * Change Description:  
 *   Changed types from ItemTypeEnum to SVString.  Changed the method getItemType to getItemTypeName.  Changed the method setItemType to setItemTypeName.  Changed m_ItemType to m_ItemTypeName.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   04 Dec 2014 09:02:56   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  965
 * SCR Title:  Update Object Selector Text Label; Update Icons; Add List Output
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Single object selection mode state for different icons
 * Added Methods getDisplayLocation;setDisplayLocation
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   17 Jul 2014 17:04:48   gramseier
 * Project:  SVObserver
 * Change Request (SCR) nbr:  909
 * SCR Title:  Object Selector replacing Result Picker and Output Selector SVO-72, 40, 130
 * Checked in by:  gRamseier;  Guido Ramseier
 * Change Description:  
 *   Initial check in.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/