//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCharValueObjectClass
//* .File Name       : $Workfile:   SVCharValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:53:02  $
//******************************************************************************

#pragma once

#pragma warning (push)
#pragma warning (disable : 4244)

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVCharValueObjectClass : public SVValueObjectClassImpl <TCHAR>
{
	SV_DECLARE_CLASS( SVCharValueObjectClass );

public:
	SVCharValueObjectClass( LPCTSTR ObjectName );
	SVCharValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVCHARVALUEOBJECT );
	SVCharValueObjectClass( const SVCharValueObjectClass& rhs );
	const SVCharValueObjectClass& operator = ( const SVCharValueObjectClass& rhs );

	virtual ~SVCharValueObjectClass();
	virtual void Persist(SVObjectWriter& rWriter) override;

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const SVString& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, SVString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;

private:
	void LocalInitialize();

	/// Convert a string in a char. Throw an exception if the string isn't convertible into a char
	/// \param strValue [in] The input string
	/// \returns char Return value.
	TCHAR convertString2Char(const SVString& rValue ) const;
};

#pragma warning (pop)
