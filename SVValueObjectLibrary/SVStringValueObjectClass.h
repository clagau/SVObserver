//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueObjectClass
//* .File Name       : $Workfile:   SVStringValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:15:24  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClass.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

namespace SvVol
{

class SVStringValueObjectClass : public SVValueObjectClass<std::string>
{
	SV_DECLARE_CLASS( SVStringValueObjectClass );

public:
	explicit SVStringValueObjectClass( LPCTSTR ObjectName );
	explicit SVStringValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTRINGVALUEOBJECT );
	explicit SVStringValueObjectClass( const SVStringValueObjectClass& rhs);
	const SVStringValueObjectClass& operator = (const SVStringValueObjectClass& rhs);

	virtual ~SVStringValueObjectClass() = default;

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;
	virtual HRESULT SetArrayValues(const ValueVector& rValues) override;

	virtual void setMemBlockPointer(uint8_t* pMemBlockBase) override;
	virtual void updateMemBlockData() const override;

	void SetMaxByteSize(int maxSize) { m_maxByteSize = maxSize; }

protected:
	virtual std::string* reserveLocalMemory() override;
	virtual void clearMemoryBlockPointer() override { m_pMemBlockData = nullptr; }

	virtual double ValueType2Double(const std::string& rValue) const { return 0.0; }
	virtual _variant_t ValueType2Variant(const std::string* pValue) const override { return (nullptr != pValue) ? _variant_t(pValue->c_str()) : _variant_t(); }
	virtual std::string Variant2ValueType(const _variant_t& rValue) const override { return SvUl::createStdString(rValue); }

	//! This just returns the input value as no conversion is required
	//! \param rValue [in] The input string
	//! \returns converted value.
	virtual std::string ConvertString2Type( const std::string& rValue ) const override { return rValue; }

	//! This conversion needs to be overloaded for type std::string
	//! \param rValue [in] Type to convert
	/// \returns the std::string
	virtual std::string ConvertType2String( const std::string& rValue ) const override { return rValue; };

	//! Returns the value object byte size for string either m_maxByteSize or string length
	//! \returns the number of bytes for the data
	virtual int32_t getByteSize(bool useResultSize, bool memBlockData) const override;

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) override;
private:
	void LocalInitialize();

	int m_maxByteSize{0};		//This is the maximum byte size to reserve space for strings which can change size during run

	///String value objects have specialized data and the allocateDataMemory points to this vector
	std::vector<std::string> m_stringData;

	///The memory block pointer where the text are to be copied too
	uint8_t* m_pMemBlockData{nullptr};	
};

} //namespace SvVol
