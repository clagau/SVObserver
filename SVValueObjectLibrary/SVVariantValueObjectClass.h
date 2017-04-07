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
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

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
	virtual void Persist(SVObjectWriter& rWriter) override;

	BOOL SetType( int vt );
	HRESULT SetValueKeepType( int iBucket, LPCTSTR value ) {return SetValueKeepType(iBucket, 0, value);}
	HRESULT SetValueKeepType( int iBucket, int iIndex, LPCTSTR value );
	VARTYPE GetDefaultType() const;

protected:
	static SVString ToString(const VARIANT& rvt, bool bScript = false );

	virtual double ValueType2Double(const _variant_t& rValue) const override;
	virtual _variant_t ValueType2Variant( const _variant_t& rValue ) const override { return rValue; };
	virtual _variant_t Variant2ValueType( const _variant_t& rValue ) const override { return rValue; };

	//! Convert a string in a variant. Throw an exception if the string isn't convertible into a variant
	//! \param rValue [in] The input string
	//! \returns the converted value.
	virtual _variant_t ConvertString2Type( const SVString& rValue ) const override;

	virtual SVString ConvertType2String( const _variant_t& rValue ) const override;
private:
	void LocalInitialize();
};


