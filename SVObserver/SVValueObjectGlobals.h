//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVValueObjectGlobals
//* .File Name       : $Workfile:   SVValueObjectGlobals.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:17:46  $
//******************************************************************************

#ifndef SVVALUEOBJECTGLOBALS_H
#define SVVALUEOBJECTGLOBALS_H

#define IMPLEMENT_VALUE_OBJECT_GET_SET_NO_T() \
	template <typename FROMTYPE>\
		__forceinline HRESULT SetValue(int iBucket, FROMTYPE value) { return SetValueAt(iBucket, 0, value); }\
	template <typename FROMTYPE>\
		__forceinline HRESULT SetValue(int iBucket, int iIndex, FROMTYPE value) { return SetValueAt(iBucket, iIndex, value); }\
	\
	template <typename TOTYPE>\
		__forceinline HRESULT GetValue(TOTYPE& rValue) const { return GetValueAt(m_iLastSetIndex, 0, rValue); }\
	template <typename TOTYPE>\
		__forceinline HRESULT GetValue(int iBucket, TOTYPE& rValue) const { return GetValueAt(iBucket, 0, rValue); }\
	template <typename TOTYPE>\
		__forceinline HRESULT GetValue(int iBucket, int iIndex, TOTYPE& rValue) const { return GetValueAt(iBucket, iIndex, rValue); }\
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, CString value ) { return SetValueAt(iBucket, iIndex, value);}\
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) { return SetValueAt(iBucket, iIndex, value);}\
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) { return SetValueAt(iBucket, iIndex, value);}\
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) { return SetValueAt(iBucket, iIndex, value);}\

#define IMPLEMENT_VALUE_OBJECT_GET_SET() \
	IMPLEMENT_VALUE_OBJECT_GET_SET_NO_T()\
	__forceinline  HRESULT SetValueAt( int iBucket, int iIndex, value_type value ) {return base::SetValueAt(iBucket, iIndex, value);}\
	__forceinline  HRESULT GetValueAt( int iBucket, int iIndex, value_type& rValue ) const {return base::GetValueAt(iBucket, iIndex, rValue);}\


// Templated GetValue functions - allows inline access to values

// example of usage:
//
// bool value = GetValue<bool>(m_voBoolValueObject);
// CString value = GetValue<CString>(m_voBoolValueObject);
//
// bool bBucketValue = GetValue<bool>(m_voBoolValueObject, iBucket);

template < typename VALUETYPE, class VALUEOBJECTCLASS >
inline VALUETYPE GetValue( VALUEOBJECTCLASS& valueobject )
{
	VALUETYPE value;
	BOOL bOk = ( valueobject.GetValue( value ) == S_OK );
	ASSERT( bOk );
	return value;
}

template < typename VALUETYPE, class VALUEOBJECTCLASS >
inline VALUETYPE GetValue( VALUEOBJECTCLASS& valueobject, int iBucket )
{
	VALUETYPE value;
	BOOL bOk = valueobject.GetValue( iBucket, value );
	ASSERT( bOk );
	return value;
}

template < typename VALUETYPE, class VALUEOBJECTCLASS >
inline VALUETYPE GetValue( VALUEOBJECTCLASS& valueobject, int iBucket, int iIndex )
{
	VALUETYPE value;
	BOOL bOk = valueobject.GetValue( iBucket, iIndex, value );
	ASSERT( bOk );
	return value;
}

#endif

