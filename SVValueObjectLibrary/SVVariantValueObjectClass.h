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

namespace SvVol
{

class SVVariantValueObjectClass : public SVValueObjectClass<_variant_t>
{
	SV_DECLARE_CLASS( SVVariantValueObjectClass );

public:
	SVVariantValueObjectClass( LPCTSTR ObjectName );
	SVVariantValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVVARIANTVALUEOBJECT );
	SVVariantValueObjectClass( const SVVariantValueObjectClass& rhs );
	const SVVariantValueObjectClass& operator = ( const SVVariantValueObjectClass& rhs );

	virtual ~SVVariantValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;

	HRESULT SetValueKeepType(LPCTSTR Value) {return SetValueKeepType(Value, 0);}
	HRESULT SetValueKeepType(LPCTSTR Value, int Index);
	VARTYPE GetDefaultType() const;
	VARTYPE GetValueType() const { return Value().vt; };

protected:
	static std::string ToString(const VARIANT& rvt, bool bScript = false );

	virtual double ValueType2Double(const _variant_t& rValue) const override;
	virtual _variant_t ValueType2Variant( const _variant_t& rValue ) const override { return rValue; };
	virtual _variant_t Variant2ValueType( const _variant_t& rValue ) const override { return rValue; };

	//! Convert a string in a variant. Throw an exception if the string isn't convertible into a variant
	//! \param rValue [in] The input string
	//! \returns the converted value.
	virtual _variant_t ConvertString2Type( const std::string& rValue ) const override;

	virtual std::string ConvertType2String( const _variant_t& rValue ) const override;

	//! Returns the value object byte size
	//! \returns the number of bytes for the data
	virtual DWORD GetByteSize() const override;

	//! Copies the value object to the memory block
	//! \param pMemoryBlock [in] Pointer to the byte address of the memory block
	//! \param MemByteSize [in] The memory block byte size
	//! \param Index [in] The index of the array (-1 if no array)
	//! \returns S_OK if successful
	virtual HRESULT CopyToMemoryBlock(BYTE* pMemoryBlock, DWORD MemByteSize, int Index = -1) const override;

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) override;
private:
	void LocalInitialize();
};

} //namespace SvVol
