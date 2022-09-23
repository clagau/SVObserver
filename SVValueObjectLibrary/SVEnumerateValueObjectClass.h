//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVEnumerateValueObjectClass
//* .File Name       : $Workfile:   SVEnumerateValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:26:50  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <vector>
//Moved to precompiled header: #include <utility>
#include "SVOResource/resource.h"
#include "SVValueObjectClass.h"
#include "ObjectInterfaces/IEnumerateValueObject.h"
#pragma endregion Includes

namespace SvVol
{

class SVEnumerateValueObjectClass : public SVValueObjectClass<long>, public SvOi::IEnumerateValueObject
{
	SV_DECLARE_CLASS
public:
	explicit SVEnumerateValueObjectClass(LPCTSTR ObjectName);
	SVEnumerateValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVENUMERATEVALUEOBJECT );
	const SVEnumerateValueObjectClass& operator = (const SVEnumerateValueObjectClass& rhs);

	virtual ~SVEnumerateValueObjectClass();

	void setStandardFormatString() override {} //not currently used in this class

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;

	bool GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const;
	bool GetEnumeratorName( long lValue, std::string& rEnumerator ) const;
	bool GetEnumTypes( std::string& rEnumList ) const;

	bool SetEnumTypes( const SvDef::NameValueVector& rVec );
	bool SetEnumTypes( LPCTSTR szEnumList );
	bool SetEnumTypes( int StringResourceID );

	HRESULT SetDefaultValue( LPCTSTR Value, bool bResetAll = true );

	virtual HRESULT SetDefaultValue( const long& rValue, bool bResetAll = true ) override { return __super::SetDefaultValue( rValue, bResetAll ); };

#pragma region IEnumerateValueObject
	virtual const SvDef::NameValueVector& GetEnumVector() const override { return m_enumVector; };
#pragma endregion IEnumerateValueObject

	//IMPLEMENT_VALUE_OBJECT_GET_SET()

	// this function should move to a library!
	static bool ToNumber(const std::string& str, long& rlValue);

protected:
	//The variant should have the enum text and not the long value
	virtual double ValueType2Double(const long& rValue) const override { return static_cast<double> (rValue); }
	virtual _variant_t ValueType2Variant(const long* pValue) const override { return (nullptr != pValue) ? _variant_t(*pValue) : _variant_t(); };
	virtual long Variant2ValueType(const _variant_t& rValue) const override { return long(rValue); }

	//! Convert a string in an enum. Throw an exception if the string isn't convertible into an enum
	//! \param rValue [in] The input string
	//! \returns long value.
	virtual long ConvertString2Type( const std::string& rValue ) const override;

	virtual std::string ConvertType2String( const long& rValue ) const override;

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) const override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) const override;
private:
	void LocalInitialize();
	
#pragma region Member Variables
private:
	SvDef::NameValueVector m_enumVector;
#pragma endregion Member Variables
};

} //namespace SvVol
