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

#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVBoolValueObjectClass : public SVValueObjectClassImpl <BOOL> // really int
{
	SV_DECLARE_CLASS( SVBoolValueObjectClass );

public:

	static HRESULT GetNormalizedValue( const SVString& rValue, SVString& rNormalized );

	SVBoolValueObjectClass( LPCTSTR ObjectName );
	SVBoolValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVBOOLVALUEOBJECT );
	SVBoolValueObjectClass(const SVBoolValueObjectClass& rhs);
	const SVBoolValueObjectClass& operator = (const SVBoolValueObjectClass& rhs);

	virtual ~SVBoolValueObjectClass();
	virtual void Persist(SVObjectWriter& rWriter) override;

	HRESULT GetValidTypes( SVStringVector& rTypes ) const;

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double Value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, long Value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, bool Value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const SVString& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double&  rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, SVString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, bool&    rValue ) const;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;

	virtual HRESULT CompareWithCurrentValueImpl( const SVString& rCompare ) const override;
	virtual HRESULT GetNormalizedValueImpl( const SVString& rValue, SVString& rNormalized ) const override;

private:
	void LocalInitialize();

	/// Convert a string in a bool. Throw an exception if the string isn't convertible into a bool
	/// \param strValue [in] The input string: "True", 1 or -1 is true, "false" and 0 is false.
	/// \returns BOOL Return value.
	BOOL ConvertString2Bool( const SVString& rValue ) const;
};
