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
	explicit BasicValueObject(LPCTSTR ObjectName, SVObjectClass* pOwner = nullptr, bool Node = false, SvPb::SVObjectSubTypeEnum ObjectSubType = SvPb::SVNotSetSubObjectType);

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
	virtual HRESULT setDefaultValue(const _variant_t& ) override { return E_NOTIMPL; }
	virtual _variant_t getDefaultValue() const override { return _variant_t(); };
	virtual HRESULT setValue(const _variant_t& rValue, int Index = -1, bool fixArrasize = false ) override;
	virtual HRESULT getValue(_variant_t& rValue, int Index = -1) const override;
	virtual HRESULT getValues(std::vector<_variant_t>& ) const override { return E_NOTIMPL; }
	virtual HRESULT setValue(const std::string& rValueString, int Index = -1) override;
	virtual HRESULT getValue(std::string& rValueString, int Index = -1, const std::string& rFormatString = _T("")/*currently always ignored*/) const override;
	virtual void setResetOptions( bool , SvOi::SVResetItemEnum  ) override {};
	virtual void validateValue( const _variant_t&  ) const override {};
	virtual bool isArray() const override { return false; };
	virtual int32_t getArraySize() const override { return 1; };
	virtual int32_t getResultSize() const override { return 1; };
	virtual SvOi::SVResetItemEnum getResetItem() const override { return SvOi::SVResetItemNone; };
	virtual bool ResetAlways() const override { return false; };
	virtual int32_t getByteSize(bool useResultSize, bool memBlockData) const override;
	virtual DWORD GetType() const override { return m_Value.vt; };
	virtual void setSaveValueFlag(bool ) override { };
	virtual void setTrData(int32_t memOffset, int32_t memSize, int32_t pos) override;
	virtual int32_t getTrPos() const override { return m_trPos; }
	virtual int32_t getMemOffset() const override { return 0L; }
	virtual void setMemBlockPointer(uint8_t* pMemBlockBase) override;
	virtual void updateMemBlockData() override;

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

	virtual bool isIndirectValue() const override { return false; };

	virtual DWORD GetObjectColor() const override;

	virtual std::string getFixedWidthFormatString(uint32_t totalWidth, uint32_t decimals) override;

	void setStandardFormatString() override {} //not currently used in this class

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
	std::mutex			m_valueMutex;		//Mutex to protect the value
	std::string			m_Description;		//The description text for the object
	uint8_t*			m_pMemBlock {nullptr}; //The memory block pointer to copy the data too
	bool				m_Created {false};	//Object is created
	bool				m_Node {false};		//Object is only a node in the tree structure
	bool				m_hasChanged {true};//Flag to determine data has changed since last update
	int32_t				m_memOffset {-1L};	//The trigger record memory offset
	int32_t				m_memSizeReserved {0L};///The block memory size reserved
	int32_t				m_trPos {-1L};		//The trigger record position
#pragma endregion Member Variables
};

typedef std::shared_ptr<BasicValueObject> BasicValueObjectPtr;
typedef std::pair<std::string, BasicValueObjectPtr> NameBasicValuePtrPair;
typedef std::vector<NameBasicValuePtrPair> NameBasicValuePtrVector;

} //namespace SvVol

#pragma region Inline
#include "BasicValueObject.inl"
#pragma endregion Inline

