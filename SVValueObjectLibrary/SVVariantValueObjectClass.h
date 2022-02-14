//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantValueObjectClass
//* .File Name       : $Workfile:   SVVariantValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:20:02  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

///SVValueObjectLibrary
namespace SvVol
{

class SVVariantValueObjectClass : public SVValueObjectClass<_variant_t>
{
	SV_DECLARE_CLASS

public:
	explicit SVVariantValueObjectClass( LPCTSTR ObjectName );
	explicit SVVariantValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVVARIANTVALUEOBJECT );
	explicit SVVariantValueObjectClass( const SVVariantValueObjectClass& rhs );
	SVVariantValueObjectClass& operator = ( const SVVariantValueObjectClass& rhs );

	virtual ~SVVariantValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;
	virtual HRESULT SetValue(const _variant_t& rValue, int Index = -1) override;
	virtual HRESULT SetArrayValues(const ValueVector& rValues) override;
	virtual HRESULT SetDefaultValue(const _variant_t& rValue, bool bResetAll = true) override;
	virtual void setMemBlockPointer(uint8_t* pMemBlockBase) override;
	virtual void updateMemBlockData() override;

	virtual HRESULT SetValueKeepType(LPCTSTR Value) {return SetValueKeepType(Value, 0);}
	virtual HRESULT SetValueKeepType(LPCTSTR Value, int Index);
	VARTYPE GetDefaultType() const { return GetDefaultValue().vt; }
	VARTYPE GetValueType() const { return (nullptr !=  valuePtr()) ? valuePtr()->vt : VT_NULL; };

	void setStandardFormatString() override {} //not currently used in this class
	virtual bool isArray() const override;


protected:
	static std::string ToString(const VARIANT& rvt, bool bScript = false );

	virtual _variant_t* reserveLocalMemory() override;
	virtual void clearMemoryBlockPointer() override { m_pMemBlockData = nullptr; }

	virtual double ValueType2Double(const _variant_t& rValue) const override;
	virtual _variant_t ValueType2Variant(const _variant_t* pValue) const override { return (nullptr != pValue) ? *pValue : _variant_t(); }
	virtual _variant_t Variant2ValueType(const _variant_t& rValue) const override;

	//! Convert a string in a variant. Throw an exception if the string isn't convertible into a variant
	//! \param rValue [in] The input string
	//! \returns the converted value.
	virtual _variant_t ConvertString2Type( const std::string& rValue ) const override;
	virtual _variant_t ConvertString2Type(const std::string& rValue, const _variant_t& rDefaultValue) const override;

	virtual std::string ConvertType2String( const _variant_t& rValue ) const override;

	//! Returns the value object byte size
	//! \returns the number of bytes for the data
	virtual int32_t getByteSize(bool useResultSize, bool memBlockData) const override;

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) const override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) const override;
private:
	void LocalInitialize();

	///Variant value objects have specialized data and the allocateDataMemory points to this vector
	std::vector<_variant_t> m_variantData;

	///The memory block pointer where the variant values are to be copied too
	uint8_t* m_pMemBlockData {nullptr};
};

} //namespace SvVol
