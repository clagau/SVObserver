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
#include "ObjectInterfaces/IValueObject.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVDeviceParam;

class BasicValueObject : public SVObjectClass, public SvOi::IValueObject
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
	BasicValueObject( LPCTSTR ObjectName, SVObjectClass* pOwner=nullptr, bool Node=false, SVObjectSubTypeEnum ObjectSubType = SVNotSetSubObjectType);

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

#pragma region virtual method (IValueObject)
	//! Gets the value for Value object NOTE this comes from IObjectClass
	//! \param rValue [out] The reference to write the value to
	//! \param Index [in] The corresponding array index to write to, if required
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValue(double& rValue, int Index = -1, int Bucket = -1) const override;

	//! Gets the default value for the Value object
	//! \returns the default value as a variant
	virtual _variant_t getDefaultValue() const override { return _variant_t(); };

	//************************************
	//! The method sets the value object
	//! \param rValue <in> a reference of the variant to set
	//! \param Index <in> standard value object parameter
	//! \returns SOK on success
	//************************************
	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1) override;

	//************************************
	//! The method gets the value object
	//! \param rValue <out> a reference to the variant to get
	//! \param Index <in> standard value object parameter
	//! \param Bucket <in> standard value object parameter
	//! \returns SOK on success
	//************************************
	virtual HRESULT getValue(_variant_t& rValue, int Index = -1, int Bucket = -1) const override;

	//! Gets the values for Value object
	//! \param rValue [out] The reference to _variant_t vector to store the values
	//! \param Bucket [in] The corresponding bucket index to get, if required
	//! \returns S_OK if succeeded
	virtual HRESULT getValues(std::vector<_variant_t>& rValues, int Bucket = -1) const override { return E_NOTIMPL; };

	//************************************
	//! The method sets the value object
	//! \param rValue <in> a reference of the string to set
	//! \param Index <in> standard value object parameter
	//! \returns SOK on success
	//************************************
	virtual HRESULT setValue(const SVString& rValue, int Index = -1) override;

	//************************************
	//! The method gets the value object
	//! \param Value <out> a value of type string to get
	//! \param Index <in> standard value object parameter
	//! \param Bucket <in> standard value object parameter
	//! \returns SOK on success
	//************************************
	virtual HRESULT getValue(SVString& rValue, int Index = -1, int Bucket = -1) const override;

	//! Set the value object bucketized flag
	//! \param isBucketized [in] true if bucketized
	virtual void setBucketized( bool isBucketized ) override {};

	/// Set the reset options for the value object
	/// \param bResetAlways [in] 
	/// \param eResetItem [in] Which item to reset
	virtual void setResetOptions( bool bResetAlways, SvOi::SVResetItemEnum eResetItem ) override {};

	/// Validate the value. If value invalid an exception message will be thrown.
	/// \param rValue [in] The value to validate
	virtual void validateValue( const _variant_t& rValue ) const override {};

	/// Checks if the value object is an array
	/// \returns true if an array
	virtual bool isArray() const override { return false; };

	/// Gets the size of the value object array
	/// \returns size 0 of not an array
	virtual int getArraySize() const override { return 0; };

	/// Gets the result size of the value object
	/// \returns size
	virtual int getResultSize() const override { return 0; };

	/// Gets the reset item type
	/// \returns the reset item enum type
	virtual SvOi::SVResetItemEnum getResetItem() const override { return SvOi::SVResetItemNone; };

	//! Gets the reset always value object flag
	//! \returns true if reset always
	virtual bool ResetAlways() const override { return false; };

	//! Copies the last set value to the destination bucket
	//! \returns the result of copying
	virtual HRESULT CopyValue( int DestBucket ) { return E_NOTIMPL; };

	//! Returns he value object byte size
	//! \returns the number of bytes for the data
	virtual DWORD GetByteSize() const override;

	//! Returns the variant type of the value object
	//! \returns the VT type
	virtual DWORD GetType() const override { return m_Value.vt; };

	//! Copies the value object to the memory block
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \param Index [in] The index of the array (-1 if no array)
	//! \returns S_OK if successful
	virtual HRESULT CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index = -1) const override;
#pragma endregion virtual method (IObjectClass)

	//************************************
	//! The method sets the value object
	//! \param Value <in> a value of type ELEMENT_TYPE to set
	//! \returns SOK on success
	//************************************
	template <typename ELEMENT_TYPE>
	HRESULT setValue( const ELEMENT_TYPE Value );

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

	virtual DWORD GetObjectColor() const override;
#pragma endregion Public Methods

private:
#pragma region Private Methods
	//************************************
	//! The method creates the value object
	//! \param pOwner <in> pointer to the parent object
	//************************************
	void Create( SVObjectClass* pOwner );

	//************************************
	//! The method destroys the value object
	//************************************
	void Destroy();

	//************************************
	//! The method locks the value object for writing
	//************************************
	void Lock();

	//************************************
	//! The method unlocks the value object
	//************************************
	void Unlock();

	//************************************
	//! The method refreshes the object and sends its parent a RefreshObject
	//! \param pSender <in> pointer to the original sender of the notification
	//! \param Type <in> what type of refresh pre or post
	//! \returns S_OK on success
	//************************************
	virtual HRESULT RefreshObject( const SVObjectClass* const pSender, RefreshObjectType Type ) override;

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

