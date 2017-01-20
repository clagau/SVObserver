//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDWordValueObjectClass
//* .File Name       : $Workfile:   SVDWordValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 10:23:34  $
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

class SVDWordValueObjectClass : public SVValueObjectClassImpl <DWORD>
{
	SV_DECLARE_CLASS( SVDWordValueObjectClass );

public:
	SVDWordValueObjectClass( LPCTSTR ObjectName );
	SVDWordValueObjectClass( SVObjectClass* POwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVDWORDVALUEOBJECT );
	SVDWordValueObjectClass( const SVDWordValueObjectClass& rhs );
	const SVDWordValueObjectClass& operator = ( const SVDWordValueObjectClass& rhs );

	virtual ~SVDWordValueObjectClass();
	virtual void Persist(SVObjectWriter& rWriter) override;

	virtual HRESULT SetOutputFormat(OutputFormat outputFormat) override;

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:

	virtual HRESULT SetValueAt( int iBucket, int iIndex, const SVString& value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const VARIANT& rValue );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, SVString& rValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;

private:
	void LocalInitialize();

	/// Convert a string in a DWORD. Throw an exception if the string isn't convertible into a DWORD.
	/// \param strValue [in] The input string
	/// \returns char Return value.
	DWORD convertString2DWord(const SVString& rValue ) const;
};

#pragma warning (pop)
