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

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVPOINT.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVPointValueObjectClass : public SVValueObjectClassImpl< SVPOINT >
{
	SV_DECLARE_CLASS( SVPointValueObjectClass );

public:
	SVPointValueObjectClass( LPCTSTR ObjectName );
	SVPointValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVPOINTVALUEOBJECT );
	SVPointValueObjectClass( const SVPointValueObjectClass& rhs );
	const SVPointValueObjectClass& operator = (const SVPointValueObjectClass& rhs);

	virtual ~SVPointValueObjectClass();

	virtual void Persist(SVObjectWriter& rWriter);

	HRESULT GetDefaultValue( POINT& rPoint ) const;

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const CString& value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, POINT value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, POINT& rPoint ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;

private:
	void LocalInitialize();

	/// Convert a string in a SVPOINT. Throw an exception if the string isn't convertible into a SVPOINT
	/// \param strValue [in] The input string
	/// \returns char Return value.
	SVPOINT convertString2Point(const CString& rValue ) const;
};

