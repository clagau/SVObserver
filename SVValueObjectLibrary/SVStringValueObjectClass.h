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
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

class SVStringValueObjectClass : public SVValueObjectClass<SVString>
{
	SV_DECLARE_CLASS( SVStringValueObjectClass );

public:
	SVStringValueObjectClass( LPCTSTR ObjectName );
	SVStringValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVSTRINGVALUEOBJECT );
	SVStringValueObjectClass( const SVStringValueObjectClass& rhs);
	const SVStringValueObjectClass& operator = (const SVStringValueObjectClass& rhs);

	virtual ~SVStringValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;

protected:
	virtual double ValueType2Double(const SVString& rValue) const override { return E_NOTIMPL; };
	virtual _variant_t ValueType2Variant( const SVString& rValue ) const override { return _variant_t( rValue.c_str() ); };
	virtual SVString Variant2ValueType( const _variant_t& rValue ) const override { return SvUl_SF::createSVString( rValue ); };

	//! This just returns the input value as no coversion is required
	//! \param rValue [in] The input string
	//! \returns converted value.
	virtual SVString ConvertString2Type( const SVString& rValue ) const override;

	//! This conversion needs to be overloaded for type SVString
	//! \param rValue [in] Type to convert
	/// \returns the SVString
	virtual SVString ConvertType2String( const SVString& rValue ) const override { return rValue; };

	virtual void WriteValues(SVObjectWriter& rWriter) override;
private:
	void LocalInitialize();
};

