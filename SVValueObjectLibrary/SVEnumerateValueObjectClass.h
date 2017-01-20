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
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#include "ObjectInterfaces\IEnumerateValueObject.h"
#pragma endregion Includes

typedef std::pair<SVString, long> SVEnumeratePair;
typedef std::vector<SVEnumeratePair> SVEnumerateVector;

class SVEnumerateValueObjectClass : public SVValueObjectClassImpl <long>, public SvOi::IEnumerateValueObject
{
	SV_DECLARE_CLASS( SVEnumerateValueObjectClass );
public:
	SVEnumerateValueObjectClass( LPCTSTR ObjectName );
	SVEnumerateValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVENUMERATEVALUEOBJECT );
	const SVEnumerateValueObjectClass& operator = (const SVEnumerateValueObjectClass& rhs);

	virtual ~SVEnumerateValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual void Persist(SVObjectWriter& rWriter) override;

	virtual HRESULT GetObjectValue( const SVString& rValueName, VARIANT& rVariantValue ) const override;
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

	HRESULT SetDefaultValue( LPCTSTR strValue, bool bResetAll );

	//using base::SetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT SetDefaultValue( const long& lValue, bool bResetAll ) {return base::SetDefaultValue(lValue, bResetAll);}

#pragma region IEnumerateValueObject
	virtual SvOi::NameValueList GetEnumList() const override;
#pragma endregion IEnumerateValueObject

	IMPLEMENT_VALUE_OBJECT_GET_SET()

	// this function should move to a library!
	static bool ToNumber(const SVString& str, long& rlValue);

protected:
	virtual HRESULT GetValueAt( int nBucket, int iIndex, DWORD& rValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, SVString& rValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual HRESULT SetValueAt( int nBucket, int iIndex, int value );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, DWORD value );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, const SVString& value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;
	
	virtual HRESULT CompareWithCurrentValueImpl( const SVString& rCompare ) const override;
	virtual HRESULT GetNormalizedValueImpl( const SVString& strValue, SVString& rstrNormalized ) const override;

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
