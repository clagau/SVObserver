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

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVUtilityLibrary/SVDPointClass.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVDPointValueObjectClass : public SVValueObjectClassImpl <SVDPointClass>
{
	SV_DECLARE_CLASS( SVDPointValueObjectClass );

public:
	SVDPointValueObjectClass( LPCTSTR ObjectName );
	SVDPointValueObjectClass( SVObjectClass* POwner = nullptr, 
	                          int StringResourceID = IDS_CLASSNAME_SVDPOINTVALUEOBJECT );
	const SVDPointValueObjectClass& operator = (const SVDPointValueObjectClass& rhs);

	virtual ~SVDPointValueObjectClass();

	virtual BOOL CreateObject( SVObjectLevelCreateStruct* pCreateStructure ) override;
	virtual HRESULT SetObjectValue( SVObjectAttributeClass* PDataObject ) override;
	virtual void Persist(SVObjectWriter& rWriter) override;

	HRESULT GetDefaultValue( POINT& rPoint ) const;
	HRESULT SetDefaultValue( const POINT& Point, bool bResetAll );

	//using base::GetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT GetDefaultValue( SVDPointClass& rPoint ) const { return base::GetDefaultValue(rPoint); }
	//using base::SetDefaultValue;// does not work in VC6; write a forwarding function
	HRESULT SetDefaultValue( const SVDPointClass& Point, bool bResetAll ) {return base::SetDefaultValue(Point, bResetAll);}

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:
	virtual HRESULT SetValueAt( int nBucket, int iIndex, const POINT& Point );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const CString& value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );

	virtual HRESULT GetValueAt( int nBucket, int iIndex, POINT& rPoint ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, double& rstrValue ) const;
	virtual HRESULT GetValueAt( int nBucket, int iIndex, CString& rstrValue ) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

	virtual void ValidateValue( int iBucket, int iIndex, const SVString& rValue ) const override;

private:
	void LocalInitialize();

	/// Convert a string in a SVDPointClass. Throw an exception if the string isn't convertible into a SVDPointClass
	/// \param strValue [in] The input string
	/// \returns char Return value.
	SVDPointClass convertString2DPoint(const CString& rValue ) const;
};
