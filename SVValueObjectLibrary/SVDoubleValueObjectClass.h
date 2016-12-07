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
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVDoubleValueObjectClass : public SVValueObjectClassImpl <double>
{
	SV_DECLARE_CLASS( SVDoubleValueObjectClass );

public:
	SVDoubleValueObjectClass( LPCTSTR ObjectName );
	SVDoubleValueObjectClass( SVObjectClass* POwner = nullptr , int StringResourceID = IDS_CLASSNAME_SVDOUBLEVALUEOBJECT );
	SVDoubleValueObjectClass( const SVDoubleValueObjectClass& rhs );
	const SVDoubleValueObjectClass& operator = (const SVDoubleValueObjectClass& rhs);

	virtual ~SVDoubleValueObjectClass();
	virtual void Persist(SVObjectWriter& rWriter) override;

	virtual HRESULT SetObjectValue( SVObjectAttributeClass* pDataObject ) override;	// for compat loading legacy SVDoubleVectorObjectClass

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, const CString& value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, int value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, long value );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, long& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, DWORD& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;

private:
	void LocalInitialize();
};

