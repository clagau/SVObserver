//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDoubleValueObjectClass
//* .File Name       : $Workfile:   SVDoubleValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:20:40  $
//******************************************************************************

#ifndef SVDOUBLEVALUEOBJECTCLASS_H
#define SVDOUBLEVALUEOBJECTCLASS_H

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"

class SVDoubleValueObjectClass : public SVValueObjectClassImpl <double>
{
	SV_DECLARE_CLASS( SVDoubleValueObjectClass );

public:
	SVDoubleValueObjectClass( LPCTSTR ObjectName );
	SVDoubleValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVDOUBLEVALUEOBJECT );
	SVDoubleValueObjectClass( const SVDoubleValueObjectClass& rhs );
	const SVDoubleValueObjectClass& operator = (const SVDoubleValueObjectClass& rhs);

	virtual ~SVDoubleValueObjectClass();
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );	// for compat loading legacy SVDoubleVectorObjectClass

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, int value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, long value );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();

};

#endif

