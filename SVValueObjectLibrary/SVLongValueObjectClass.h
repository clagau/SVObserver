//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueObjectClass
//* .File Name       : $Workfile:   SVLongValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:18:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVLongValueObjectClass : public SVValueObjectClass<long>
{
	SV_DECLARE_CLASS( SVLongValueObjectClass );

public:
	SVLongValueObjectClass( LPCTSTR ObjectName );
	SVLongValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLONGVALUEOBJECT );
	SVLongValueObjectClass( const SVLongValueObjectClass& rhs );
	const SVLongValueObjectClass& operator = (const SVLongValueObjectClass& rhs);

	virtual ~SVLongValueObjectClass();

protected:
	virtual double ValueType2Double(const long& rValue) const override { return static_cast<double> (rValue); };
	virtual _variant_t ValueType2Variant( const long& rValue ) const override { return _variant_t( rValue ); };
	virtual long Variant2ValueType( const _variant_t& rValue ) const override { return long( rValue ); };

	/// Convert a string in a long. Throw an exception if the string isn't convertible into a long.
	/// \param strValue [in] The input string
	/// \returns char Return value.
	virtual long ConvertString2Type(const SVString& rValue ) const override;

	virtual SVString ConvertType2String(const long& rValue) const override { return FormatOutput(rValue); };

	virtual void WriteValues(SVObjectWriter& rWriter) override;
private:
	void LocalInitialize();
};

