//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVByteValueObjectClass
//* .File Name       : $Workfile:   SVByteValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:46  $
//******************************************************************************

#pragma once

#pragma warning (push)
#pragma warning (disable : 4244)

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{

class SVByteValueObjectClass : public SVValueObjectClass<BYTE>
{
	// This is needed since SVByteVectorObjectClass has been changed
	// to use SVByteValueObjectClass instead of SVObjectByteArrayClass
	// and the MIL LUT functions needs the pointer to a BYTE block
	friend class SVLUTOperator;

	SV_DECLARE_CLASS

public:
	explicit SVByteValueObjectClass(LPCTSTR ObjectName);
	SVByteValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBYTEVALUEOBJECT );
	SVByteValueObjectClass( const SVByteValueObjectClass& rhs );
	const SVByteValueObjectClass& operator = ( const SVByteValueObjectClass& rhs );

	virtual ~SVByteValueObjectClass();
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;	// for compat loading legacy SVByteVectorObjectClass

	virtual HRESULT SetOutputFormat(OutputFormat outputFormat) override;

	void setStandardFormatString() override {} //not currently used in this class

protected:
	virtual double ValueType2Double(const BYTE& rValue) const override { return static_cast<double> (rValue); }
	virtual _variant_t ValueType2Variant(const BYTE* pValue) const override { return (nullptr != pValue) ? _variant_t(*pValue) : _variant_t(); }
	virtual BYTE Variant2ValueType(const _variant_t& rValue) const override { return BYTE(rValue); }

	/// Convert a string in a byte. Throw an exception if the string isn't convertible into a byte
	/// \param strValue [in] The input string
	/// \returns BYTE Return value.
	virtual BYTE ConvertString2Type( const std::string& rValue ) const override;

	virtual std::string ConvertType2String(const BYTE& rValue) const override { return FormatOutput(rValue); };

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) const override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) const override;
private:
	void LocalInitialize();
};

#pragma warning (pop)

} //namespace SvVol
