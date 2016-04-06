//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVVariantValueObjectClass
//* .File Name       : $Workfile:   SVVariantValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 16:20:02  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVVariantValueObjectClass : public SVValueObjectClassImpl< _variant_t >
{
	SV_DECLARE_CLASS( SVVariantValueObjectClass );

public:
	SVVariantValueObjectClass( LPCTSTR ObjectName );
	SVVariantValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVVARIANTVALUEOBJECT );
	SVVariantValueObjectClass( const SVVariantValueObjectClass& rhs );
	const SVVariantValueObjectClass& operator = ( const SVVariantValueObjectClass& rhs );

	virtual ~SVVariantValueObjectClass();

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );
	virtual void Persist(SVObjectWriter& rWriter);

	BOOL SetType( int vt );
	HRESULT SetValueKeepType( int iBucket, LPCTSTR value ) {return SetValueKeepType(iBucket, 0, value);}
	HRESULT SetValueKeepType( int iBucket, int iIndex, LPCTSTR value );
	HRESULT SetDefaultValue( const VARIANT& rValue, bool bResetAll );
	HRESULT GetDefaultValue( _variant_t& rValue );
	VARTYPE GetDefaultType() const;

	IMPLEMENT_VALUE_OBJECT_GET_SET_NO_T()

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, BOOL value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, BYTE value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, DWORD value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, long value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, BYTE& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, BOOL& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;

	static CString ToString(const VARIANT& rvt, bool bScript = false );

private:
	void LocalInitialize();
};


