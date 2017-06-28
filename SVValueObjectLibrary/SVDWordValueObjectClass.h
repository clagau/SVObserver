//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDWordValueObjectClass
//* .File Name       : $Workfile:   SVDWordValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:34  $
//******************************************************************************

#pragma once

#pragma warning (push)
#pragma warning (disable : 4244)

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVDWordValueObjectClass : public SVValueObjectClass<DWORD>
{
	SV_DECLARE_CLASS( SVDWordValueObjectClass );

public:
	SVDWordValueObjectClass( LPCTSTR ObjectName );
	SVDWordValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDWORDVALUEOBJECT );
	SVDWordValueObjectClass( const SVDWordValueObjectClass& rhs );
	const SVDWordValueObjectClass& operator = ( const SVDWordValueObjectClass& rhs );

	virtual ~SVDWordValueObjectClass();

	virtual HRESULT SetOutputFormat(OutputFormat outputFormat) override;

protected:
	virtual double ValueType2Double(const DWORD& rValue) const override { return static_cast<double> (rValue); };
	virtual _variant_t ValueType2Variant( const DWORD& rValue ) const override { return _variant_t( rValue ); };
	virtual DWORD Variant2ValueType( const _variant_t& rValue ) const override { return DWORD( rValue ); };

	//! Convert a string in a DWORD. Throw an exception if the string isn't convertible into a DWORD.
	//! \param rValue [in] The input string
	//! \returns converted value.
	virtual DWORD ConvertString2Type(const SVString& rValue ) const override;

	virtual SVString ConvertType2String(const DWORD& rValue) const override { return FormatOutput(rValue); };

	virtual void WriteValues(SVObjectWriter& rWriter) override;
private:
	void LocalInitialize();
};

#pragma warning (pop)
