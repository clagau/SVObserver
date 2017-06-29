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
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClass.h"
#include "ObjectInterfaces/IEnumerateValueObject.h"
#include "SVContainerLibrary/SVObjectArrayClassTemplate.h"
#pragma endregion Includes

typedef std::pair<SVString, long> SVEnumeratePair;
typedef std::vector<SVEnumeratePair> SVEnumerateVector;

class SVEnumerateValueObjectClass : public SVValueObjectClass<long>, public SvOi::IEnumerateValueObject
{
	SV_DECLARE_CLASS( SVEnumerateValueObjectClass );
public:
	SVEnumerateValueObjectClass( LPCTSTR ObjectName );
	SVEnumerateValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVENUMERATEVALUEOBJECT );
	const SVEnumerateValueObjectClass& operator = (const SVEnumerateValueObjectClass& rhs);

	virtual ~SVEnumerateValueObjectClass();

	//virtual HRESULT GetObjectValue( const SVString& rValueName, _variant_t& rValue ) const override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;

	BOOL GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const;
	BOOL GetEnumeratorName( long lValue, SVString& rEnumerator ) const;
	BOOL GetEnumTypes( SVEnumerateVector& rVec ) const;
	BOOL GetEnumTypes( SVString& rEnumList ) const;
	int  GetFirstEnumTypePos() const;
	BOOL GetNextEnumType( int& rIterator, SVString& rEnum, long& rValue ) const;

	BOOL SetEnumTypes( const SVEnumerateVector& rVec );
	BOOL SetEnumTypes( LPCTSTR szEnumList );
	BOOL SetEnumTypes( int StringResourceID );

	HRESULT SetDefaultValue( LPCTSTR Value, bool bResetAll );

	virtual HRESULT SetDefaultValue( const long& rValue, bool bResetAll ) override { return __super::SetDefaultValue( rValue, bResetAll ); };

#pragma region IEnumerateValueObject
	virtual SvOi::NameValueList GetEnumList() const override;
#pragma endregion IEnumerateValueObject

	//IMPLEMENT_VALUE_OBJECT_GET_SET()

	// this function should move to a library!
	static bool ToNumber(const SVString& str, long& rlValue);

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
	virtual long ConvertString2Type( const SVString& rValue ) const override;

	virtual SVString ConvertType2String( const long& rValue ) const override;

	virtual void WriteValues(SVObjectWriter& rWriter) override;
private:
	void LocalInitialize();
	
#pragma region Member Variables
private:
	// Sorted Table of all defined enumeration values.
	SvCl::SVObjectLongArrayClass m_enumValueTable;
	// String table of defined enumerations, 1 by 1 to value table
	SvCl::SVObjectSVStringArrayClass m_enumStringTable;
#pragma endregion Member Variables
};
