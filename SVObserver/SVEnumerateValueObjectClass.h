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
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#include "ObjectInterfaces\IEnumerateValueObject.h"
#pragma endregion Includes

typedef std::pair<CString, long> SVEnumeratePair;
typedef std::vector<SVEnumeratePair> SVEnumerateVector;

class SVEnumerateValueObjectClass : public SVValueObjectClassImpl <long>, public SvOi::IEnumerateValueObject
{
	SV_DECLARE_CLASS( SVEnumerateValueObjectClass );
public:
	SVEnumerateValueObjectClass( LPCTSTR ObjectName );
	SVEnumerateValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVENUMERATEVALUEOBJECT );
	const SVEnumerateValueObjectClass& operator = (const SVEnumerateValueObjectClass& rhs);

	virtual ~SVEnumerateValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual void Persist(SVObjectWriter& rWriter);

	virtual HRESULT GetObjectValue( const SVString& p_rValueName, VARIANT& p_rVariantValue ) const;
	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );

	BOOL GetEnumerator( LPCTSTR szEnumerator, long& lValue ) const;
	BOOL GetEnumeratorName( long lValue, CString& rEnumerator ) const;
	BOOL GetEnumTypes( SVEnumerateVector& rVec ) const;
	BOOL GetEnumTypes( CString& rStrEnumList ) const;
	int  GetFirstEnumTypePos() const;
	BOOL GetNextEnumType( int& rIterator, CString& rStrEnum, long& rEnumValue ) const;

	BOOL SetEnumTypes( const SVEnumerateVector& rVec );
	BOOL SetEnumTypes( LPCTSTR szEnumList );
	BOOL SetEnumTypes( int StringResourceID );

	HRESULT SetDefaultValue( CString strValue, bool bResetAll );

	//using base::SetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT SetDefaultValue( long lValue, bool bResetAll ) {return base::SetDefaultValue(lValue, bResetAll);}

#pragma region IEnumerateValueObject
	virtual SvOi::NameValueList GetEnumList() const override;
#pragma endregion IEnumerateValueObject

	IMPLEMENT_VALUE_OBJECT_GET_SET()

	// this function should move to a library!
	static bool ToNumber(const CString& str, long& rlValue);

protected:

	virtual HRESULT GetValueAt( int nBucket, int iIndex, DWORD& rValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual HRESULT SetValueAt( int nBucket, int iIndex, int value );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, DWORD value );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );
	
	virtual HRESULT CompareWithCurrentValueImpl( const CString& rstrCompare ) const;
	virtual HRESULT GetNormalizedValueImpl( const CString& strValue, CString& rstrNormalized ) const;

private:
	void LocalInitialize();
	
#pragma region Member Variables
private:
	// Sorted Table of all defined enumeration values.
	SvCl::SVObjectLongArrayClass m_enumValueTable;
	// String table of defined enumerations, 1 by 1 to value table
	SvCl::SVObjectCStringArrayClass m_enumStringTable;
#pragma endregion Member Variables
};
