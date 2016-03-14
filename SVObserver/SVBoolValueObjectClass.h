//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueObjectClass
//* .File Name       : $Workfile:   SVBoolValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   30 Oct 2013 15:18:36  $
//******************************************************************************

#ifndef SVBOOLVALUEOBJECTCLASS_H
#define SVBOOLVALUEOBJECTCLASS_H

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"

class SVBoolValueObjectClass : public SVValueObjectClassImpl <BOOL> // really int
{
	SV_DECLARE_CLASS( SVBoolValueObjectClass );

public:
	typedef std::vector< CString > SVValidTypesVector;

	static HRESULT GetNormalizedValue( const CString& strValue, CString& rstrNormalized );

	SVBoolValueObjectClass( LPCTSTR ObjectName );
	SVBoolValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVBOOLVALUEOBJECT );
	SVBoolValueObjectClass(const SVBoolValueObjectClass& rhs);
	const SVBoolValueObjectClass& operator = (const SVBoolValueObjectClass& rhs);

	virtual ~SVBoolValueObjectClass();
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	HRESULT GetValidTypes( SVValidTypesVector& p_astrTypes ) const;

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, long value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, bool value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& vtValue );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double&  rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, bool&    rValue ) const;

	virtual HRESULT CompareWithCurrentValueImpl( const CString& rstrCompare ) const;
	virtual HRESULT GetNormalizedValueImpl( const CString& strValue, CString& rstrNormalized ) const;

private:
	void LocalInitialize();

};

#endif

