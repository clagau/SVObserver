//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDPointValueObjectClass
//* .File Name       : $Workfile:   SVDPointValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:20:56  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <assert.h>
//Moved to precompiled header: #include <atltypes.h>
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"

class SVDPointValueObjectClass : public SVValueObjectClassImpl <SVDPointClass>
{
	SV_DECLARE_CLASS( SVDPointValueObjectClass );

public:
	SVDPointValueObjectClass( LPCTSTR ObjectName );
	SVDPointValueObjectClass( SVObjectClass* POwner = NULL , 
	                          int StringResourceID = IDS_CLASSNAME_SVDPOINTVALUEOBJECT );
	const SVDPointValueObjectClass& operator = (const SVDPointValueObjectClass& rhs);

	virtual ~SVDPointValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure );
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject );
#ifdef USE_OBJECT_SCRIPT
	virtual void GetObjectScript( CString& RStrScript, CString& RStrAliasTable, int Indent = 0 );
#endif
	virtual void Persist(SVObjectWriter& rWriter);

	HRESULT GetDefaultValue( POINT& rPoint );
	HRESULT SetDefaultValue( POINT Point, bool bResetAll );
	HRESULT GetDefaultValue( CPoint& rPoint );
	HRESULT SetDefaultValue( CPoint Point, bool bResetAll );

	//using base::GetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT GetDefaultValue( SVDPointClass& rPoint ) {return base::GetDefaultValue(rPoint);}
	//using base::SetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT SetDefaultValue( SVDPointClass& Point, bool bResetAll ) {return base::SetDefaultValue(Point, bResetAll);}

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:

	virtual HRESULT SetValueAt( int nBucket, int iIndex, const POINT Point );
	virtual HRESULT SetValueAt( int nBucket, int iIndex, const CPoint Point );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );

	virtual HRESULT GetValueAt( int nBucket, int iIndex, CPoint& rPoint ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, POINT& rPoint ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, double& rstrValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, CString& rstrValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();

};

