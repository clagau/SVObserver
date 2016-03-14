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
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVOMFCLibrary/SVFileNameClass.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVFileNameValueObjectClass : public SVValueObjectClassImpl <CString>  
{
	SV_DECLARE_CLASS( SVFileNameValueObjectClass );

public:
	SVFileNameValueObjectClass( LPCTSTR ObjectName );
	SVFileNameValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVFILENAMEVALUEOBJECT );
	SVFileNameValueObjectClass( const SVFileNameValueObjectClass& rhs);
	const SVFileNameValueObjectClass& operator = ( const SVFileNameValueObjectClass& rhs );

	virtual ~SVFileNameValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );

	virtual HRESULT SetObjectValue( const SVString& p_rValueName, const _variant_t& p_rVariantValue );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );

#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	HRESULT SetDefaultValue( CString value, bool BResetAll );

//	IMPLEMENT_VALUE_OBJECT_GET_SET_NO_T()
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, FROMTYPE value) { return SetValueAt(iBucket, 0, value); }
	template <typename FROMTYPE>
	__forceinline HRESULT SetValue(int iBucket, int iIndex, FROMTYPE value) { return SetValueAt(iBucket, iIndex, value); }
	
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(TOTYPE& rValue) const { return GetValueAt(m_iLastSetIndex, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, TOTYPE& rValue) const { return GetValueAt(iBucket, 0, rValue); }
	template <typename TOTYPE>
	__forceinline HRESULT GetValue(int iBucket, int iIndex, TOTYPE& rValue) const { return GetValueAt(iBucket, iIndex, rValue); }

	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, CString value ) { return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) { return S_FALSE; } //return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) { return S_FALSE; } //return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) { return S_FALSE; } //return SetValueAt(iBucket, iIndex, value); }
	
protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const {return base::GetValueAt(iBucket, iIndex, rValue);}

private:
	void LocalInitialize();

	SVFileNameClass m_svFileName;

};

