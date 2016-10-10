//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueObjectClass
//* .File Name       : $Workfile:   SVLongValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:18:28  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVLongValueObjectClass : public SVValueObjectClassImpl <long>
{
	SV_DECLARE_CLASS( SVLongValueObjectClass );

public:
	SVLongValueObjectClass( LPCTSTR ObjectName );
	SVLongValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVLONGVALUEOBJECT );
	SVLongValueObjectClass( const SVLongValueObjectClass& rhs );
	const SVLongValueObjectClass& operator = (const SVLongValueObjectClass& rhs);

	virtual ~SVLongValueObjectClass();
	virtual void Persist( SVObjectWriter& rWriter);

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const CString& value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, int value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;

private:
	void LocalInitialize();

	/// Convert a string in a long. Throw an exception if the string isn't convertible into a long.
	/// \param strValue [in] The input string
	/// \returns char Return value.
	long convertString2Long(const CString& rValue ) const;
};

