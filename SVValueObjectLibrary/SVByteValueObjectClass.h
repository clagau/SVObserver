//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVByteValueObjectClass : public SVValueObjectClass<BYTE>
{
	// This is needed since SVByteVectorObjectClass has been changed
	// to use SVByteValueObjectClass instead of SVObjectByteArrayClass
	// and the MIL LUT functions needs the pointer to a BYTE block
	friend class SVLUTOperatorClass;

	SV_DECLARE_CLASS( SVByteValueObjectClass );

public:
	SVByteValueObjectClass( LPCTSTR ObjectName );
	SVByteValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBYTEVALUEOBJECT );
	SVByteValueObjectClass( const SVByteValueObjectClass& rhs );
	const SVByteValueObjectClass& operator = ( const SVByteValueObjectClass& rhs );

	virtual ~SVByteValueObjectClass();
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;	// for compat loading legacy SVByteVectorObjectClass

	virtual HRESULT SetOutputFormat(OutputFormat outputFormat) override;

protected:
	virtual double ValueType2Double(const BYTE& rValue) const override { return static_cast<double> (rValue); };
	virtual _variant_t ValueType2Variant( const BYTE& rValue ) const override { return _variant_t( rValue ); };
	virtual BYTE Variant2ValueType( const _variant_t& rValue ) const override { return BYTE( rValue ); };

	/// Convert a string in a byte. Throw an exception if the string isn't convertible into a byte
	/// \param strValue [in] The input string
	/// \returns BYTE Return value.
	virtual BYTE ConvertString2Type( const SVString& rValue ) const override;

	virtual SVString ConvertType2String(const BYTE& rValue) const override { return FormatOutput(rValue); };

	virtual void WriteValues(SVObjectWriter& rWriter) override;
private:
	void LocalInitialize();
};

#pragma warning (pop)
