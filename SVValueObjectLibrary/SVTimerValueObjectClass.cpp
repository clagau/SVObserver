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

void SVTimerValueObjectClass::Start()
{
	m_Start = SvTl::GetTimeStamp();
}

bool SVTimerValueObjectClass::Stop()
{
	double Value = SvTl::ConvertTo( SvTl::Microseconds, ( SvTl::GetTimeStamp() - m_Start ) );

	return S_OK == SetValue(static_cast<__int64> (Value));
}

void SVTimerValueObjectClass::LocalInitialize()
{
	m_Start = SvTl::GetTimeStamp();
	SetTypeName(_T("Timer"));
	setOutputFormat(_T("%I64u (µs)"));
}

