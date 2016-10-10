//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInt64ValueObjectClass
//* .File Name       : $Workfile:   SVInt64ValueObjectClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 11:04:52  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVOResource/resource.h"
#include "SVValueObjectClassImpl.h"
#include "SVValueObjectGlobals.h"
#pragma endregion Includes

class SVInt64ValueObjectClass : public SVValueObjectClassImpl <__int64>
{
	SV_DECLARE_CLASS( SVInt64ValueObjectClass );

public:
	SVInt64ValueObjectClass( LPCTSTR ObjectName);
	SVInt64ValueObjectClass( SVObjectClass* pOwner = nullptr, int StringResourceID = IDS_CLASSNAME_SVINT64VALUEOBJECT );
	const SVInt64ValueObjectClass& operator = (const SVInt64ValueObjectClass& rhs);
	virtual void Persist(SVObjectWriter& rWriter);

	virtual ~SVInt64ValueObjectClass();

	IMPLEMENT_VALUE_OBJECT_GET_SET()

protected:
	virtual HRESULT SetValueAt( int iBucket, int iIndex, int value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, const CString& value );
	virtual HRESULT SetValueAt( int iBucket, int iIndex, double value );

	virtual HRESULT GetValueAt( int iBucket, int iIndex, double& rdValue) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, CString& rstrValue) const;
	virtual HRESULT GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const;

private:
	void LocalInitialize();
};


