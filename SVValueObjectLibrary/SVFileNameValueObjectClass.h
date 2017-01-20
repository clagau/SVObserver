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
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVFileNameValueObjectClass : public SVValueObjectClassImpl <SVString>  
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

	HRESULT SetDefaultValue( LPCTSTR value, bool BResetAll );

//	IMPLEMENT_VALUE_OBJECT_GET_SET_NO_T()
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, FROMTYPE Value) { return SetValueAt(iBucket, 0, Value); }
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, int iIndex, FROMTYPE Value) { return SetValueAt(iBucket, iIndex, Value); }
	
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(TOTYPE& rValue) const { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, TOTYPE& rValue) const { return GetValueAt(iBucket, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, int iIndex, TOTYPE& rValue) const { return GetValueAt(iBucket, iIndex, rValue); }

	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, const SVString& rValue ) override { return SetValueAt(iBucket, iIndex, rValue); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) override { return S_FALSE; }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) override { return S_FALSE; }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) override { return S_FALSE; }
	
protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const SVString& rValue );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, SVString& rValue ) const {return base::GetValueAt(iBucket, iIndex, rValue);}

private:
	void LocalInitialize();

	SVFileNameClass m_svFileName;
};

