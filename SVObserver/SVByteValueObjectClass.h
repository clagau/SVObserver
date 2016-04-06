//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVByteValueObjectClass
//* .File Name       : $Workfile:   SVByteValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:45:46  $
//******************************************************************************

#pragma once

#pragma warning (push)
#pragma warning (disable : 4244)

#pragma region Includes
#include "SVObjectLibrary/SVObjectScriptUsage.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVByteValueObjectClass : public SVValueObjectClassImpl <BYTE>
{
	// This is needed since SVByteVectorObjectClass has been changed
	// to use SVByteValueObjectClass instead of SVObjectByteArrayClass
	// and the MIL LUT functions needs the pointer to a BYTE block
	friend class SVLUTOperatorClass;

	SV_DECLARE_CLASS( SVByteValueObjectClass );

public:
	SVByteValueObjectClass( LPCTSTR ObjectName );
	SVByteValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBYTEVALUEOBJECT );
	SVByteValueObjectClass( const SVByteValueObjectClass& rhs );
	const SVByteValueObjectClass& operator = ( const SVByteValueObjectClass& rhs );

	virtual ~SVByteValueObjectClass();
	virtual void Persist(SVObjectWriter& rWriter);
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject );	// for compat loading legacy SVByteVectorObjectClass

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& vtValue );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, CString value );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();
};

#pragma warning (pop)

