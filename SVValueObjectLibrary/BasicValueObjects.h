//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObjects
//* .File Name       : $Workfile:   BasicValueObjects.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   17 Jul 2014 17:39:38  $
//* ----------------------------------------------------------------------------
//* This class is used as the container to store BasicValueObject objects
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <sequential_tree.h>
#include "BasicValueObject.h"
#pragma endregion Includes

namespace SvVol
{

class BasicValueObjects
{
public:
#pragma region Declarations
	typedef tcl::sequential_tree<BasicValueObjectPtr> ValueTree;
	typedef ValueTree::iterator vt_iterator;
	typedef ValueTree::const_iterator vt_const_iterator;
	typedef ValueTree::pre_order_iterator vt_pre_order_iterator;
	typedef ValueTree::const_pre_order_iterator vt_const_pre_order_iterator;
	typedef std::vector<BasicValueObjectPtr> ValueVector;
	typedef std::map<std::string, uint32_t > DottedNameObjectIdMap;
#pragma endregion Declarations

#pragma region Constructor
	BasicValueObjects();

	virtual ~BasicValueObjects();
#pragma endregion Constructor

public:
#pragma region Public Methods
	//************************************
	//! The method gets the value object given a name
	//! \param DottedName <in> the dotted name of the object to get
	//! \returns The value object as a pointer
	//************************************
	BasicValueObjectPtr getValueObject( LPCTSTR DottedName ) const;

	//************************************
	//! The method gets the value given a name
	//! \param DottedName <in> the dotted name of the object to get
	//! \param rValue <out> reference to the value of the object
	//! \returns SOK on success
	//************************************
	template <typename ELEMENT_TYPE>
	HRESULT getValue( LPCTSTR DottedName, ELEMENT_TYPE& rValue ) const;

	//************************************
	//! The method sets the value with the given name if does not exist it is created
	//! \param DottedName <in> the dotted variable name to set
	//! \param Value <in> the value to set the variable to
	//! \param pOwner <in> pointer to the owner of the object
	//! \param ObjectType <in> the object type
	//! \returns reference to the value object
	//************************************
	template <typename ELEMENT_TYPE>
	BasicValueObjectPtr setValueObject( LPCTSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner = nullptr, SvPb::SVObjectSubTypeEnum ObjectType = SvPb::SVNotSetSubObjectType);

	//************************************
	//! The method gets the value list which fit the dotted branch name
	//! \param rObjectList <out> reference to the object list
	//! \param DottedBranch <in> the dotted branch name to filter
	//! \param AttributesAllowedFilter <in> the object filter setting
	//************************************
	void getValueList( ValueVector& rObjectList, LPCTSTR DottedBranch, UINT AttributesAllowedFilter ) const;

	//************************************
	//! The method deletes the value object with the given name and all empty parent nodes
	//! \param DottedName <in> the object corresponding to the dotted name to delete
	//! \returns S_OK on success
	//************************************
	HRESULT deleteValueObject( LPCTSTR DottedName );

	//************************************
	//! The method gets the BasicValueObject tree 
	//! \returns a reference to the tree
	//************************************
	inline const ValueTree& getTree() const;
#pragma endregion Public Methods

#pragma region Private Methods
private:
	//************************************
	//! The method creates the value with the given name and all necessary nodes
	//! \param DottedName <in> the dotted variable name to create
	//! \param Value <in> the value to set the variable to
	//! \param pOwner <in> pointer to the owner of the object
	//! \param ObjectType <in> the object type
	//! \returns the created iterator
	//************************************
	template <typename ELEMENT_TYPE>
	vt_const_iterator createValueObject( LPCTSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner, SvPb::SVObjectSubTypeEnum ObjectType );

	//************************************
	//! The method attempts to find the requested dotted name
	//! \param DottedName <in> the dotted name to find
	//! \returns the found iterator or end() if not found
	//************************************
	vt_const_iterator findValueObject( LPCTSTR DottedName ) const;

	//************************************
	//! The method attempts to find the requested parsed name
	//! \param rParsedName <in> reference to the parsed name
	//! \returns the found iterator or end() if not found
	//************************************
	vt_const_iterator findValueObject( const SVObjectNameInfo& rParsedName ) const;

	//************************************
	//! The method attempts to find the requested parsed name
	//! \param rParsedName <in> reference to the parsed name
	//! \returns the found iterator or end() if not found
	//************************************
	vt_iterator findValueObject( const SVObjectNameInfo& rParsedName );

	//************************************
	//! The method attempts to find the requested child name in the given branch
	//! \param rStartIter <in> a reference to the branch start iterator
	//! \param rEndIter <in> a reference to the branch end iterator
	//! \param ChildName <in> the child name to find in the branch
	//! \returns the found iterator or end() if not found
	//************************************
	vt_const_iterator findChildObject( const vt_const_iterator& rStartIter, const vt_const_iterator& rEndIter, LPCTSTR ChildName ) const;
#pragma endregion Private Methods

private:
#pragma region Member Variables
	ValueTree m_Tree;				//! The tree of BasicValueObjects
	static const DottedNameObjectIdMap m_StaticUniqueIDMap; //! The list of dotted names with static unique ids
#pragma endregion Member Variables
};

} //namespace SvVol

#pragma region Inline
#include "BasicValueObjects.inl"
#pragma endregion Inline

