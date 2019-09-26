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
#pragma endregion Includes

class SVDeviceParam;

namespace SvVol
{


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
	BasicValueObject( LPCTSTR ObjectName, SVObjectClass* pOwner=nullptr, bool Node=false, SvPb::SVObjectSubTypeEnum ObjectSubType = SvPb::SVNotSetSubObjectType);

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

#pragma region virtual method (IObjectClass/IValueObject)
	//! For these methods see IObject documentation
	virtual HRESULT getValue(double& rValue, int Index = -1) const override;

	//! For these methods see IValueObject documentation
	virtual HRESULT setDefaultValue(const _variant_t& rValue) override { return E_NOTIMPL; }
	virtual _variant_t getDefaultValue() const override { return _variant_t(); };
	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1, bool fixArrasize = false ) override;
	virtual HRESULT getValue(_variant_t& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<_variant_t>& rValues) const override { return E_NOTIMPL; }
	virtual HRESULT setValue(const std::string& rValue, int Index = -1) override;
	virtual HRESULT getValue(std::string& rValue, int Index = -1) const override;
	virtual void setResetOptions( bool bResetAlways, SvOi::SVResetItemEnum eResetItem ) override {};
	virtual void validateValue( const _variant_t& rValue ) const override {};
	virtual bool isArray() const override { return false; };
	virtual int getArraySize() const override { return 1; };
	virtual int getResultSize() const override { return 1; };
	virtual SvOi::SVResetItemEnum getResetItem() const override { return SvOi::SVResetItemNone; };
	virtual bool ResetAlways() const override { return false; };
	virtual long GetByteSize(bool useResultSize = true) const override;
	virtual DWORD GetType() const override { return m_Value.vt; };
	virtual long CopyToMemoryBlock(BYTE* pMemoryBlock, long MemByteSize) const override;
	virtual void setSaveValueFlag(bool shouldSaveValue) override { };
	virtual void setTrData(long memOffset, int pos) const override { m_memOffset = memOffset; m_trPos = pos; }
	virtual int getTrPos() const override { return m_trPos; }
#pragma endregion virtual method (IObjectClass/IValueObject)

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
	virtual std::string getTypeName() const override;

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
	std::string			m_Description;		//The description text for the object
	bool				m_Created;			//Object is created
	bool				m_Node;				//Object is only a node in the tree structure
	mutable long		m_memOffset {0L};	//The trigger record memory offset
	mutable int			m_trPos {-1};		//The trigger record position
#pragma endregion Member Variables
};

typedef std::shared_ptr<BasicValueObject> BasicValueObjectPtr;
typedef std::pair<std::string, BasicValueObjectPtr> NameBasicValuePtrPair;
typedef std::vector<NameBasicValuePtrPair> NameBasicValuePtrVector;

} //namespace SvVol

#pragma region Inline
#include "BasicValueObject.inl"
#pragma endregion Inline

