//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInt64ValueObjectClass
//* .File Name       : $Workfile:   SVInt64ValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:04:52  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

namespace SvVol
{

class SVInt64ValueObjectClass : public SVValueObjectClass<__int64>
{
	SV_DECLARE_CLASS

public:
	explicit SVInt64ValueObjectClass(LPCTSTR ObjectName);
	SVInt64ValueObjectClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINT64VALUEOBJECT );
	const SVInt64ValueObjectClass& operator = (const SVInt64ValueObjectClass& rhs );

	virtual std::string getFixedWidthFormatString(uint32_t totalWidth, uint32_t decimals) override;

	void setStandardFormatString();

	virtual ~SVInt64ValueObjectClass();

protected:
	virtual double ValueType2Double(const __int64& rValue) const override { return static_cast<double> (rValue); }
	virtual _variant_t ValueType2Variant(const __int64* pValue) const override { return (nullptr != pValue) ? _variant_t(*pValue) : _variant_t(); }
	virtual __int64 Variant2ValueType(const _variant_t& rValue) const override { return __int64(rValue); }

	//! Convert a string in a __int64. Throw an exception if the string isn't convertible into a __int64.
	//! \param rValue [in] The input string
	//! \returns converted value.
	virtual __int64 ConvertString2Type( const std::string& rValue ) const override;

	virtual std::string ConvertType2String(const __int64& rValue) const override { return FormatOutput(rValue); };

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) const override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) const override;
private:
	void LocalInitialize();
};

} //namespace SvVol
