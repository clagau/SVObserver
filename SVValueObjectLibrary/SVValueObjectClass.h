//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObject
//* .File Name       : $Workfile:   SVValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   15 May 2014 15:08:08  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <set>
//Moved to precompiled header: #include <assert.h>
#include "ObjectInterfaces/IValueObject.h"
#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVObjectLibrary/SVObjectClass.h"
#include "SVArrayValueHolder.h"

#pragma endregion Includes

enum SVResetItemEnum // used with Embedded Object registration.
{
//  Used in cases where changing the Tool value can have an impact on 
// 	dependent Tools. In most cases this would be overkill.	
	SVResetItemIP = 0,

//  Generally the default.  
	SVResetItemTool = 1,

//  An optimization IF you are editing a sub-task object (friend or child)
//  and you can get away with only parent task for some duration.  Usually  
//  requires "manual" Tool level reset when accepting sub-task values. 
	SVResetItemOwner = 2,

//	An SVResetItemNone example are Tool Results, which do not require reseting the 
//  Tool, Owner, or IP when their values change.
	SVResetItemNone = 3,
	SVResetItemSize,
};

/**
@SVObjectName Value Object

@SVObjectOverview This is the base object for all value holding object in the application.  This object manages the data an allows for automatic conversion from on type to another.

@SVObjectOperations This object contains the operators for creating, reseting, updating, and extracting the value information contained within this object.

*/
class SVValueObjectClass : public SVObjectClass, public SvOi::IValueObject
{
//	SV_DECLARE_CLASS( SVValueObjectClass ); // Not factory constructable at this time
public:
	SVValueObjectClass( LPCTSTR ObjectName );
	SVValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVVALUEOBJECT );

public:
	virtual ~SVValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual BOOL CloseObject();
	virtual HRESULT ResetObject();

	virtual bool ResetAlways() const;
	virtual SVResetItemEnum ResetItem() const;
	virtual HRESULT SetResetOptions( bool p_bResetAlways, SVResetItemEnum p_eResetItem );

	virtual bool IsArray() const;
	virtual int GetArraySize() const;
	virtual HRESULT SetArraySize(int iSize);

	HRESULT GetResultSize(int iBucket, int& riResultSize) const;
	int GetResultSize() const;
	HRESULT SetResultSize(int iBucket, int  riResultSize);
	
	#pragma region IValueObject
	virtual HRESULT SetValue( const _variant_t& rValue ) override;
	virtual HRESULT GetValue( _variant_t& rValue ) const override;
	virtual void ValidateValue( const _variant_t& rValue ) const override;
	#pragma endregion IValueObject
	
	// NVI pattern : public interface
	HRESULT SetValue( int iBucket, LPCTSTR value )					{ return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, LPCTSTR value )		{ return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, double& value )					{ return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, double& value )		{ return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, long& value )					{ return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, long& value )		{ return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, unsigned long& value )			{ return SetValueAtConvert(iBucket, 0, static_cast <long> (value)); }
	HRESULT SetValue( int iBucket, int iIndex, unsigned long& value ) { return SetValueAtConvert(iBucket, iIndex, static_cast <long> (value)); }
	HRESULT SetValue( int iBucket, int& value )						{ return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, int& value )			{ return SetValueAtConvert(iBucket, iIndex, value); }
	HRESULT SetValue( int iBucket, const int& value )				{ return SetValueAtConvert(iBucket, 0, value); }
	HRESULT SetValue( int iBucket, int iIndex, const int& value )	{ return SetValueAtConvert(iBucket, iIndex, value); }

	HRESULT GetValue( double&  rValue ) const { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	HRESULT GetValue( CString& rValue ) const { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	HRESULT GetValue( VARIANT& rValue ) const{ return GetValueAt(m_iLastSetIndex, 0, rValue); }

	HRESULT GetValue( int iBucket, double&  rValue ) const  { return GetValueAt(iBucket, 0, rValue); }
	HRESULT GetValue( int iBucket, CString& rValue ) const  { return GetValueAt(iBucket, 0, rValue); }
	HRESULT GetValue( int iBucket, VARIANT& rValue ) const  { return GetValueAt(iBucket, 0, rValue); }

	HRESULT GetValue( int iBucket, int iIndex, double&  rValue ) const  { return GetValueAt(iBucket, iIndex, rValue); }
	HRESULT GetValue( int iBucket, int iIndex, CString& rValue ) const  { return GetValueAt(iBucket, iIndex, rValue); }
	HRESULT GetValue( int iBucket, int iIndex, VARIANT& rValue ) const  { return GetValueAt(iBucket, iIndex, rValue); }

	/// Get the value array of the last used bucket as string (semicolon-separated).
	/// \param rValue [out] The return string.
	/// \returns HRESULT
	HRESULT GetValues( CString&  rValue ) const  { return GetArrayValues(m_iLastSetIndex, rValue); }
	HRESULT GetValues( VARIANT& rValue ) const  { return GetArrayValuesAsVariant(m_iLastSetIndex, rValue); }
	HRESULT GetValues( std::vector< _variant_t >&  rValue ) const  { return GetArrayValuesAsVariantVector(m_iLastSetIndex, rValue); }

	HRESULT GetValues( int iBucket, VARIANT& rValue ) const  { return GetArrayValuesAsVariant(iBucket, rValue); }
	HRESULT GetValues( int iBucket, std::vector< _variant_t >&  rValue ) const  { return GetArrayValuesAsVariantVector(iBucket, rValue); }

	HRESULT GetValues( int iBucket, std::vector< double >&  rValue ) const  { return GetArrayValues(iBucket, rValue); }

	HRESULT GetTypeName( CString& p_rstrValue ) const;
	HRESULT SetTypeName( const LPCTSTR p_pstrValue );

	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );

	HRESULT CompareWithCurrentValue( const CString& rstrCompare ) const { return CompareWithCurrentValueImpl( rstrCompare ); }
	HRESULT GetNormalizedValue( const CString& strValue, CString& rstrNormalized ) const { return GetNormalizedValueImpl( strValue, rstrNormalized ); }

	virtual HRESULT CopyValue( int iSourceBucket, int iDestBucket ) { assert(0); return S_FALSE; }
	HRESULT CopyLastSetValue( int iDestBucket )    { return CopyValue(m_iLastSetIndex, iDestBucket); }

	BOOL GetLastSetIndex( int& iBucket ) const    { iBucket = m_iLastSetIndex; return TRUE; }
	int  GetLastSetIndex()               const    { return m_iLastSetIndex; }
	void SetLegacyVectorObjectCompatibility() { m_bLegacyVectorObjectCompatibility = true; }

protected:
	// use the NonVirtual Interface pattern (NVI; C++ Coding Standards # 39, Sutter & Alexandrescu)
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, const CString& value ) = 0;
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) = 0;
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) = 0;
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) = 0;

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double&  rValue ) const = 0;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const = 0;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const = 0;

	virtual HRESULT GetArrayValues( int iBucket, std::vector< double >&  rValue ) const = 0;
	/// Get the value array as string (semicolon-separated).
	/// \param iBucket [in] The bucket of the value.
	/// \param rValue [out] The return string.
	/// \returns HRESULT
	virtual HRESULT GetArrayValues( int iBucket, CString& rValue ) const = 0;
	virtual HRESULT GetArrayValuesAsVariant( int iBucket, VARIANT&  rValue ) const = 0;
	virtual HRESULT GetArrayValuesAsVariantVector( int iBucket, std::vector< _variant_t >&  rValue ) const = 0;

	// EB 2005 11 11
	// eventually all derived classes should implement this
	// this should replace the functionality currently in SVInspectionProcess::ProcessInputRequests
	// where it tries to determine if there is a difference between the new and old value.
	virtual HRESULT CompareWithCurrentValueImpl( const CString& rstrCompare ) const;
	virtual HRESULT GetNormalizedValueImpl( const CString& strValue, CString& rstrNormalized ) const { return E_NOTIMPL; }

	virtual DWORD_PTR  processMessage( DWORD DwMessageID, DWORD_PTR DwMessageValue, DWORD_PTR DwMessageContext );
	virtual HRESULT CreateBuckets();
	void InitializeBuckets();
	virtual int GetResultSize(int iBucket) const;
	/// Validate the value. If value invalid an exception message will be thrown.
	/// \param iBucket [in] 
	/// \param iIndex [in]
	/// \param rValue [in]
	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const;

	HRESULT ValidateIndexes(int iBucket, int iArrayIndex) const;

	inline int ArraySize() const { return m_iArraySize; }	// a fast version of GetArraySize for implementation use

	int m_iNumberOfBuckets;
	int m_iLastSetIndex;
	int m_iArraySize;
	std::vector<int> m_aiResultSize;

	SVArrayValueHolderBase* m_pBuckets;

	bool m_bLegacyVectorObjectCompatibility;

	void swap( SVValueObjectClass& rhs );
	CString m_strTypeName;

	bool m_bResetAlways;
	SVResetItemEnum m_eResetItem;

private:
	void Initialize();

	friend class SVValueObjectReference;	// direct access for speed (VC6 lack of good optimization)
};

#include "SVValueObjectClass.inl"

typedef SVVector< SVValueObjectClass* > SVValueObjectArray;
typedef std::set< SVValueObjectClass* > SVValueObjectClassPtrSet;

