//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : RootObject
//* .File Name       : $Workfile:   RootObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   17 Jul 2014 17:56:30  $
//* ----------------------------------------------------------------------------
//* This class is used to define the root node for the class object manager
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVObjectLibrary\SVObjectClass.h"
#include "RootChildren.h"
#include "SVConfigurationObject.h"
#pragma endregion Includes

class RootObject : public SVObjectClass
{
#pragma region Declarations
	SV_DECLARE_CLASS( RootObject );
#pragma endregion Declarations

public:
#pragma region Constructor
	//************************************
	//! The class constructor
	//! \param ObjectName <in> the name of the object
	//************************************
	RootObject( LPCSTR ObjectName );

	//************************************
	//! The class constructor
	//! \param pOwner <in> a pointer to the parent object
	//! \param StringResourceID <in> the resource string ID for the name
	//************************************
	RootObject( SVObjectClass *pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_ROOTOBJECT );

	virtual ~RootObject();
#pragma endregion Constructor

public:
#pragma region Public Methods
	//************************************
	//! The method creates the configuration object
	//! \return true on success
	//************************************
	bool createConfigurationObject();

	//************************************
	//! The method destroys the configuration object
	//! \return true on success
	//************************************
	void destroyConfigurationObject();

	//! The method finds the object corresponding to the name
	//! \param rpObject <out> a reference pointer to the object being searched
	//! \param rNameInfo <in> a reference to the name of the object to search for
	//! \param Index <in> the index of the name to search for
	//! \return SOK on success
	//************************************
	virtual HRESULT GetChildObject( SVObjectClass*& p_rpObject, const SVObjectNameInfo& p_rNameInfo, const long p_Index = 0 ) const override;

	//************************************
	//! The method gets the Root Children object
	//! \return reference to the Root Children
	//************************************
	 RootChildren& getRootChildren() { return m_RootChildren; };

	//************************************
	//! The method refreshes the object
	//! \param pSender <out> a pointer to the object which sent the event
	//! \param Type <in> the type of refresh
	//! \return SOK on success
	//************************************
	 virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type ) override;

	//************************************
	//! The static method gets a root child object value
	//! \param DottedName <in> the dotted name of the object to get
	//! \return pointer to the value object
	//************************************
	static BasicValueObjectPtr getRootChildObjectValue( LPCTSTR DottedName );

	//************************************
	//! The static method gets a root child value if available
	//! \param DottedName <in> the dotted name of the object to get
	//! \param rValue <out> reference to the value of the object
	//! \return S_OK on success
	//************************************
	template <typename ELEMENT_TYPE>
	static HRESULT getRootChildValue( LPCTSTR DottedName, ELEMENT_TYPE& rValue );

	//************************************
	//! The static method sets a root child value
	//! \param DottedName <in> the dotted name of the object to set
	//! \param rValue <out> reference to the value of the object
	//! \return pointer to the value object
	//************************************
	template <typename ELEMENT_TYPE>
	static BasicValueObjectPtr setRootChildValue( LPCTSTR DottedName, const ELEMENT_TYPE& rValue );

	//************************************
	/// The method fill up a list with filtered root child objects.
	/// \param rObjectList [in,out] The list which will be filled.
	/// \param Path [in] The path of the requested objects. Default = "", means all objects.
	/// \param AttributesAllowedFilter [in] These flags indicate which attributes must be set for the objects to be added. For each flag, 1 means "must be set", and 0 mean "don't care".  Default = 0, means all objects.
	//************************************
	static void getRootChildObjectList( BasicValueObjects::ValueVector& rObjectList, LPCTSTR Path = _T(""), UINT AttributesAllowedFilter = 0);

	//************************************
	//! The method gets a root child object name list.
	//! \param rObjectNameList <out> The returned list of names.
	//! \param Path <in> the path of object to return. Default = "", means all objects.
	//! \param AttributesAllowedFilter <in> These flags indicate which attributes must be set for the objects to be returned. For each flag, 1 means "must be set", and 0 mean "don't care".  Default = 0, means all objects.
	//************************************
	static void getRootChildNameList( SVStringArray& rObjectNameList, LPCTSTR Path = _T(""), UINT AttributesAllowedFilter = 0 );

	//************************************
	//! The static method deletes a root child value object
	//! \param DottedName <in> the dotted name of the object to delete
	//! \return S_OK on success
	//************************************
	static HRESULT deleteRootChildValue( LPCTSTR DottedName );

	//************************************
	//! The static method resets a root child
	//! \param Name <in> the root child name to reset
	//! \return S_OK on success
	//************************************
	static HRESULT resetRootChildValue( LPCTSTR Name );
#pragma endregion Public Methods

private:
#pragma region Private Methods
	//************************************
	//! The method initializes the root object
	//! \return true on success
	//************************************
	bool Initialize();

	//************************************
	//! The method creates the root children
	//! \return true on success
	//************************************
	bool createRootChildren();

	//************************************
	//! The method creates the root child
	//! \param ChildName <in> The root child name to create
	//! \param Object <in> the object type of the root child
	//! \return true on success
	//************************************
	bool createRootChild( LPCTSTR ChildName, SVObjectTypeEnum ObjectType );
#pragma endregion Private Methods

private:
#pragma region Member Variables
	RootChildren				m_RootChildren;						//The tree for all root children
	SVConfigurationObjectPtr	m_pConfigurationObject;				//The main node configuration pointer object
#pragma endregion Member Variables
};

#pragma region Inline
#include "RootObject.inl"
#pragma endregion Inline

