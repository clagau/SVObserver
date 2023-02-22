//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

namespace SvVol
{

#pragma region Declarations
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#pragma endregion Declarations

SV_IMPLEMENT_CLASS(SVTimerValueObjectClass, SvPb::TimerValueClassId);

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
	m_Start = SvUl::GetTimeStamp();
}

bool SVTimerValueObjectClass::Stop()
{
	double Value = SvUl::ConvertTo( SvUl::Microseconds, ( SvUl::GetTimeStamp() - m_Start ) );

	return S_OK == SetValue(static_cast<__int64> (Value));
}


void SVTimerValueObjectClass::setStandardFormatString()
{
	setOutputFormat(_T("{} (µs)"));
}


void SVTimerValueObjectClass::LocalInitialize()
{
	m_ObjectTypeInfo.m_SubType = SvPb::SVTimerValueObjectType;

	m_Start = SvUl::GetTimeStamp();
	SetTypeName(_T("Timer"));
	setStandardFormatString();
}


} //namespace SvVol
