//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPointValueObjectClass
//* .File Name       : $Workfile:   SVPointValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 13:29:38  $
//******************************************************************************

#pragma once

#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVUtilityLibrary/SVPOINT.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"

class SVPointValueObjectClass : public SVValueObjectClassImpl< SVPOINT >
{
	SV_DECLARE_CLASS( SVPointValueObjectClass );

public:
	SVPointValueObjectClass( LPCTSTR ObjectName );
	SVPointValueObjectClass( SVObjectClass* POwner = NULL , int StringResourceID = IDS_CLASSNAME_SVPOINTVALUEOBJECT );
	SVPointValueObjectClass( const SVPointValueObjectClass& rhs );
	const SVPointValueObjectClass& operator = (const SVPointValueObjectClass& rhs);

	virtual ~SVPointValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& rstrScript, CString& rstrAliasTable, int iIndent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	HRESULT GetDefaultValue( POINT& rPoint );

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, POINT value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, POINT& rPoint ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();

};

