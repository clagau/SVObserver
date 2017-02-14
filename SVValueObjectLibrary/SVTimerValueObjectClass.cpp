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

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

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

HRESULT SVTimerValueObjectClass::GetValueAt(int iBucket, int iIndex, SVString& rValue) const
{
	__int64 value=0;

	HRESULT hr = base::GetValueAt(iBucket, iIndex, value);
	//if ( S_OK == hr ) //@WARNING - log an error ?
	{
		rValue = SvUl_SF::Format(_T("%I64u (µs)"), value);
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
	SetTypeName( _T("Timer") );
}

