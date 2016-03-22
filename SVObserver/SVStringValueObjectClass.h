//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVStringValueObjectClass
//* .File Name       : $Workfile:   SVStringValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:15:24  $
//******************************************************************************

#pragma once

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"

class SVStringValueObjectClass : public SVValueObjectClassImpl < CString >
{
	SV_DECLARE_CLASS( SVStringValueObjectClass );

public:
	SVStringValueObjectClass( LPCTSTR ObjectName );
	SVStringValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVSTRINGVALUEOBJECT );
	SVStringValueObjectClass( const SVStringValueObjectClass& rhs);
	const SVStringValueObjectClass& operator = (const SVStringValueObjectClass& rhs);

	virtual ~SVStringValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

//	IMPLEMENT_VALUE_OBJECT_GET_SET()
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
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, double value ) { return S_FALSE; } //SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, long value ) { return S_FALSE; } //return SetValueAt(iBucket, iIndex, value); }
	virtual HRESULT SetValueAtConvert( int iBucket, int iIndex, int value ) { return S_FALSE; } //SetValueAt(iBucket, iIndex, value); }

	__forceinline  HRESULT SetValueAt( int iBucket, int iIndex, value_type value ) {return base::SetValueAt(iBucket, iIndex, value); }
	__forceinline  HRESULT GetValueAt( int iBucket, int iIndex, value_type& rValue ) const {return base::GetValueAt(iBucket, iIndex, rValue); }


protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();

};

