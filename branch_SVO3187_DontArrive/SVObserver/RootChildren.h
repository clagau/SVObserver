//******************************************************************************
//* COPYRIGHT (c) 2014 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootChildren
//* .File Name       : $Workfile:   RootChildren.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.5  $
//* .Check In Date   : $Date:   16 Dec 2014 17:53:30  $
//* ----------------------------------------------------------------------------
//* This class is used as the root child object  to 
//* store specific objects 
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVValueObjectLibrary/BasicValueObjects.h"
#pragma endregion Includes

class RootChildren
{
#pragma region Declarations
#pragma endregion Declarations

public:
#pragma region Constructor
	RootChildren();

	~RootChildren();
#pragma endregion Constructor

public:
#pragma region Public Methods
	//************************************
	//! The method searches the object with the corresponding name
	//! \param rpObject <out> a reference pointer to the object being searched
	//! \param rNameInfo <in> a reference to the name of the object to search for
	//! \param Index <in> the index of the name to search for
	//! \return SOK on success
	//************************************
	HRESULT GetChildObject( SVObjectClass*& rpObject, const SVObjectNameInfo& rNameInfo, const long Index = 0 ) const;

	//************************************
	//! The method gets the value object given a name
	//! \param DottedName <in> the dotted name of the object to get
	//! \returns The value object as a pointer
	//************************************
	SvVol::BasicValueObjectPtr getValueObject( LPCTSTR DottedName ) const;

	//************************************
	//! The method gets the object list
	//! \param rObjectList <out> The returned list of objects.
	//! \param DottedBranch <in> the dotted branch name of object to return. Default = "", means all objects.
	//! \param AttributesAllowedFilter <in> These flags indicate which attributes must be set for the objects to be returned. For each flag, 1 means "must be set", and 0 mean "don't care".  Default = 0, means all objects.
	//************************************
	void getObjectList(SvVol::BasicValueObjects::ValueVector& rObjectList, LPCTSTR DottedBranch,  UINT AttributesAllowedFilter ) const;

	//************************************
	//! The method gets the object list
	//! \param DottedName <in> the dotted name to get
	//! return SOK on success
	//************************************
	HRESULT deleteValue( LPCTSTR DottedName );

	//************************************
	//! The method gets the value of a name
	//! \param DottedName <in> the dotted name to get
	//! \param rValue <out> reference to the value of the object
	//! return SOK on success
	//************************************
	template <typename ELEMENT_TYPE>
	HRESULT getValue( LPCTSTR DottedName, ELEMENT_TYPE& rValue ) const;

	//************************************
	//! The method sets the value with the given name if does not exist it is created
	//! \param DottedName <in> the dotted name to set
	//! \param Value <in> the value to set the variable to
	//! \param pOwner <in> pointer to the owner of the object
	//! \param ObjectType <in> the object subtype
	//! \returns reference to the value object
	//************************************
	template <typename ELEMENT_TYPE>
	SvVol::BasicValueObjectPtr setValue(LPCTSTR DottedName, const ELEMENT_TYPE Value, SVObjectClass* pOwner = nullptr, SvPb::SVObjectSubTypeEnum ObjectType = SvPb::SVNotSetSubObjectType);
#pragma endregion Public Methods

#pragma region Member Variables
private:
	SvVol::BasicValueObjects	m_RootChildrenValues;			//Container tree for all root children values
#pragma endregion Member Variables
};

#pragma region Inline
#include "RootChildren.inl"
#pragma endregion Inline

