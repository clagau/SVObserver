//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
#include "SVContainerLibrary/SVObjectArrayClassTemplate.h"
#pragma endregion Includes

typedef std::pair<std::string, long> SVEnumeratePair;
typedef std::vector<SVEnumeratePair> SVEnumerateVector;

class SVEnumerateValueObjectClass : public SVValueObjectClass<long>, public SvOi::IEnumerateValueObject
{
	SV_DECLARE_CLASS( SVEnumerateValueObjectClass );
public:
	SVEnumerateValueObjectClass( LPCTSTR ObjectName );
	SVEnumerateValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVENUMERATEVALUEOBJECT );
	const SVEnumerateValueObjectClass& operator = (const SVEnumerateValueObjectClass& rhs);

	virtual ~SVEnumerateValueObjectClass();

	//virtual HRESULT GetObjectValue( const std::string& rValueName, _variant_t& rValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	const SVEnumerateVector& GetEnumVector() const {return m_enumVector; };

	bool GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const;
	bool GetEnumeratorName( long lValue, std::string& rEnumerator ) const;
	bool GetEnumTypes( std::string& rEnumList ) const;

	bool SetEnumTypes( const SVEnumerateVector& rVec );
	bool SetEnumTypes( LPCTSTR szEnumList );
	bool SetEnumTypes( int StringResourceID );

	HRESULT SetDefaultValue( LPCTSTR Value, bool bResetAll = true );

	virtual HRESULT SetDefaultValue( const long& rValue, bool bResetAll = true ) override { return __super::SetDefaultValue( rValue, bResetAll ); };

#pragma region IEnumerateValueObject
	virtual SvOi::NameValueList GetEnumList() const override;
#pragma endregion IEnumerateValueObject

	//IMPLEMENT_VALUE_OBJECT_GET_SET()

	// this function should move to a library!
	static bool ToNumber(const std::string& str, long& rlValue);

protected:
	//This is a specialized version as it is required to return the enum text not value
	virtual HRESULT GetVariantValue(_variant_t& rValue, int Index = -1, int Bucket = -1) const override;

	//The variant should have the enum text and not the long value
	virtual double ValueType2Double(const long& rValue) const override { return static_cast<double> (rValue); };
	virtual _variant_t ValueType2Variant( const long& rValue ) const override { return _variant_t( rValue ); };
	virtual long Variant2ValueType( const _variant_t& rValue ) const override { return long( rValue ); };

	//! Convert a string in an enum. Throw an exception if the string isn't convertible into an enum
	//! \param rValue [in] The input string
	//! \returns long value.
	virtual long ConvertString2Type( const std::string& rValue ) const override;

	virtual std::string ConvertType2String( const long& rValue ) const override;

	virtual void WriteValues(SvOi::IObjectWriter& rWriter) override;
	virtual void WriteDefaultValues(SvOi::IObjectWriter& rWriter) override;
private:
	void LocalInitialize();
	
#pragma region Member Variables
private:
	SVEnumerateVector m_enumVector;
#pragma endregion Member Variables
};
