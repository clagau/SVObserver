//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCharValueObjectClass
//* .File Name       : $Workfile:   SVCharValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:02  $
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

class SVCharValueObjectClass : public SVValueObjectClass<char>
{
	SV_DECLARE_CLASS

public:
	explicit SVCharValueObjectClass(LPCTSTR ObjectName);
	SVCharValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCHARVALUEOBJECT );
	SVCharValueObjectClass( const SVCharValueObjectClass& rhs );
	const SVCharValueObjectClass& operator = ( const SVCharValueObjectClass& rhs );
	void setStandardFormatString();


	virtual ~SVCharValueObjectClass();

protected:
	virtual double ValueType2Double(const TCHAR& rValue) const override { return static_cast<double> (rValue); }
	virtual _variant_t ValueType2Variant(const TCHAR* pValue) const override { return (nullptr != pValue) ? _variant_t(*pValue) : _variant_t(); }
	virtual TCHAR Variant2ValueType(const _variant_t& rValue) const override { return TCHAR(rValue); }

	/// Convert a string in a char. Throw an exception if the string isn't convertible into a char
	/// \param rValue [in] The input string
	/// \returns char Return value.
	virtual TCHAR ConvertString2Type(const std::string& rValue ) const override;

	virtual std::string ConvertType2String(const TCHAR& rValue) const override { return FormatOutput(rValue); };

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) override;
private:
	void LocalInitialize();
};

#pragma warning (pop)

} //namespace SvVol
