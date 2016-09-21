//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTimerValueObjectClass
//* .File Name       : $Workfile:   SVTimerValueObjectClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   30 Jul 2013 12:24:30  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTimerValueObjectClass.h"
#include "SVObjectLibrary/SVClsids.h"
#pragma endregion Includes

namespace	// only for this file
{
	const CString DEFAULT_TAG_SAVE(_T(".Default"));
	const CString BUCKET_TAG_SAVE(_T(".Array"));	// for backwards compatibility
	const CString ARRAY_TAG_SAVE(_T(".Array_Elements"));	// new style; one bucket, all array values

	const CString DEFAULT_TAG_LOAD(_T("Default"));
	const CString BUCKET_TAG_LOAD(_T("Array"));	// for backwards compatibility
	const CString ARRAY_TAG_LOAD(_T("Array_Elements"));	// new style; one bucket, all array values
}	// end file scope namespace

SV_IMPLEMENT_CLASS(SVTimerValueObjectClass, SVTimerValueObjectClassGuid);

SVTimerValueObjectClass::SVTimerValueObjectClass(LPCTSTR ObjectName)
						:SVInt64ValueObjectClass(ObjectName)
{
	LocalInitialize();
}

SVTimerValueObjectClass::SVTimerValueObjectClass(SVObjectClass *pOwner, int StringResourceID)
						:SVInt64ValueObjectClass(pOwner, StringResourceID)
{
	LocalInitialize();
}

SVTimerValueObjectClass::~SVTimerValueObjectClass()
{
	SetObjectDepth(0);
	ResetObject();
}

BOOL SVTimerValueObjectClass::Start()
{
	BOOL bOk = true;

	m_Start = SVClock::GetTimeStamp();

	return bOk;
}

BOOL SVTimerValueObjectClass::Stop(long lIndex)
{
	double l_Value = SVClock::ConvertTo( SVClock::Microseconds, ( SVClock::GetTimeStamp() - m_Start ) );

	return S_OK == SetValue(lIndex, l_Value);
}

HRESULT SVTimerValueObjectClass::GetValueAt(int iBucket, int iIndex, CString& rstrValue) const
{
	__int64 value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	//if ( S_OK == hr ) //@WARNING - log an error ?
	{
		rstrValue.Format("%I64u (µs)", value);
	}
	return hr;
}

HRESULT SVTimerValueObjectClass::GetValueAt( int iBucket, int iIndex, VARIANT& rValue ) const
{
	__int64 Value=0;
	_variant_t l_Temp;
	l_Temp.Attach( rValue );
	HRESULT hr = base::GetValueAt( iBucket, iIndex, Value );
	if( S_OK == hr )
	{
		l_Temp = Value;
	}
	else
	{
		l_Temp.Clear();
	}
	rValue = l_Temp.Detach();

	return hr;
}

void SVTimerValueObjectClass::LocalInitialize()
{
	m_Start = SVClock::GetTimeStamp();
	m_strTypeName = "Timer";
}

