//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileNameValueObjectClass
//* .File Name       : $Workfile:   SVFileNameValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:38:34  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVLibrary/SVFileNameClass.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#pragma endregion Includes

class SVFileNameValueObjectClass : public SVValueObjectClass<SVString>  
{
	SV_DECLARE_CLASS( SVFileNameValueObjectClass );

public:
	SVFileNameValueObjectClass( LPCTSTR ObjectName );
	SVFileNameValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVFILENAMEVALUEOBJECT );
	SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs);
	const SVFileNameValueObjectClass& operator = ( const SVFileNameValueObjectClass& rhs );

	virtual ~SVFileNameValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;

	virtual void Persist(SVObjectWriter& rWriter) override;

	virtual HRESULT SetValue( const SVString& rValue, int Bucket = -1, int Index = -1 ) override;
	HRESULT SetDefaultValue( const SVString& rValue, bool bResetAll ) override;

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

private:
	void LocalInitialize();

	SVFileNameClass m_FileName;
};

