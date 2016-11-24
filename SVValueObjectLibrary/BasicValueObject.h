//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : BasicValueObject
//* .File Name       : $Workfile:   BasicValueObject.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   16 Dec 2014 17:51:34  $
//* ----------------------------------------------------------------------------
//* This class is used as a value object to store different types in the 
//* object manager 
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
#include "SVObjectLibrary\SVObjectClass.h"
#include "SVUtilityLibrary\SVString.h"
#include "ObjectInterfaces/IBasicValueObject.h"
#pragma endregion Includes

class SVDeviceParam;

class BasicValueObject : public SVObjectClass, public SvOi::IBasicValueObject
{
public:
#pragma region Constructor
	//************************************
	//! The class constructor
	//! \param ObjectName <in> the name of the object
	//! \param pOwner <in> pointer to the parent object default is nullptr
	//! \param ObjectType <in> the object type
	//! \param Node <in> flag which determines if this is a node in the tree, default is false
	//************************************
	BasicValueObject( LPCTSTR ObjectName, SVObjectClass* pOwner=nullptr, bool Node=false, SVObjectTypeEnum ObjectType = SVBasicValueObjectType );

	virtual ~BasicValueObject();
#pragma endregion Constructor

public:
#pragma region Public Methods
	//************************************
	//! The method is the == operator
	//! \param rRhs <in> a reference to the value to compare
	//! \returns True if the same
	//************************************
	bool operator ==(const BasicValueObject& rRhs) const;

	//************************************
	//! The method sets the value object
	//! \param rValue <in> a reference of the variant to set
	//! \returns SOK on success
	//************************************
	HRESULT setValue( const _variant_t& rValue );

	//************************************
	//! The method sets the value object
	//! \param Value <in> a value of type string to set
	//! \returns SOK on success
	//************************************
	HRESULT setValue( const LPCTSTR Value );

	//************************************
	//! The method sets the value object
	//! \param Value <in> a value of type ELEMENT_TYPE to set
	//! \returns SOK on success
	//************************************
	template <typename ELEMENT_TYPE>
	HRESULT setValue( const ELEMENT_TYPE Value );

	//************************************
	//! The method gets the value object
	//! \param rValue <out> a reference to the variant to get
	//! \returns SOK on success
	//************************************
	HRESULT getValue( _variant_t& rValue ) const;

	//************************************
	//! The method gets the value object
	//! \param Value <out> a value of type string to get
	//! \returns SOK on success
	//************************************
	HRESULT getValue( SVString& rValue ) const;

	//************************************
	//! The method gets the value object
	//! \param rValue <out> a reference type bool to get
	//! \returns SOK on success
	//************************************
	HRESULT getValue( BOOL& rValue ) const;

	//************************************
	//! The method gets the value object
	//! \param rValue <out> a reference type bool to get
	//! \returns SOK on success
	//************************************
	HRESULT getValue( bool& rValue ) const;

	//************************************
	//! The method gets the value object
	//! \param rValue <out> a reference type ELEMENT_TYPE to get
	//! \returns SOK on success
	//************************************
	template <typename ELEMENT_TYPE>
	HRESULT getValue( ELEMENT_TYPE& rValue ) const;

	//************************************
	//! The method gets the type name of the value
	//! \returns type name
	//************************************
	virtual SVString getTypeName() const override;

	//************************************
	//! The method updates the corresponding device parameter
	//! \param pDeviceParam <in> a pointer to the parameter
	//! \returns SOK on success
	//************************************
	HRESULT updateDeviceParameter( SVDeviceParam* pDeviceParam );

	//************************************
	//! The method checks if the object is a node
	//! \returns true if object is a node
	//************************************
	inline bool isNode() const;

	//************************************
	//! The method sets the objects description
	//************************************
	inline void setDescription( LPCTSTR Description );

	//************************************
	//! The method gets the objects description
	//! \returns The objects description
	//************************************
	inline LPCTSTR getDescription();
#pragma endregion Public Methods

private:
#pragma region Private Methods
	//************************************
	//! The method creates the value object
	//! \param pOwner <in> pointer to the parent object
	//! \returns true on success
	//************************************
	virtual BOOL Create( SVObjectClass* pOwner );

	//************************************
	//! The method destroys the value object
	//! \returns true on success
	//************************************
	virtual BOOL Destroy();

	//************************************
	//! The method locks the value object for writing
	//! \returns true on success
	//************************************
	BOOL Lock();

	//************************************
	//! The method unlocks the value object
	//! \returns true on success
	//************************************
	BOOL Unlock();

	//************************************
	//! The method refreshes the object and sends its parent a RefreshObject
	//! \param pSender <in> pointer to the original sender of the notification
	//! \param Type <in> what type of refresh pre or post
	//! \returns S_OK on success
	//************************************
	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type );

	//************************************
	//! The method refreshes the object owner
	//! \param Type <in> what type of refresh pre or post
	//! \returns true on success
	//************************************
	bool RefreshOwner( RefreshObjectType Type ) const;

	//************************************
	//! The method converts an array to variant data
	//! \returns S_OK on success
	//************************************
	HRESULT ConvertArrayToVariant( _variant_t& rValue ) const;
#pragma endregion Private Methods

private:
#pragma region Member Variables
	_variant_t			m_Value;			//The value object container
	CRITICAL_SECTION	m_CriticalSection;	//The critical section object
	SVString			m_Description;		//The description text for the object
	bool				m_Created;			//Object is created
	bool				m_Node;				//Object is only a node in the tree structure
#pragma endregion Member Variables
};

typedef SVSharedPtr< BasicValueObject > BasicValueObjectPtr;
typedef std::pair< SVString, BasicValueObjectPtr > NameBasicValuePtrPair;
typedef std::vector< NameBasicValuePtrPair > NameBasicValuePtrVector;

#pragma region Inline
#include "BasicValueObject.inl"
#pragma endregion Inline

