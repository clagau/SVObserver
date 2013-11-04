//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLongValueDeviceParam
//* .File Name       : $Workfile:   SVLongValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:07:18  $
//******************************************************************************
#include "Stdafx.h"
#include <cmath>
#include "SVLongValueDeviceParam.h"

SVLongValueDeviceParam::SVLongValueDeviceParam()
: lValue(0) 
{
	m_eDataType = DeviceDataTypeLong;
	init();
}

SVLongValueDeviceParam::SVLongValueDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
, lValue(0) 
{
	m_eDataType = DeviceDataTypeLong;
	init();
}

SVLongValueDeviceParam::SVLongValueDeviceParam(const SVLongValueDeviceParam& rhs) 
: SVDeviceParam( rhs )
{
	m_eDataType = DeviceDataTypeLong;
	lValue = rhs.lValue;
	info = rhs.info;
}

SVClonable* SVLongValueDeviceParam::CloneImpl() const
{
	return new SVLongValueDeviceParam(*this);
}

SVLongValueDeviceParam::SVLongValueDeviceParam(long rhs)	// not const long& so that we get automatic conversion from other integral types for free
{
	m_eDataType = DeviceDataTypeLong;
	lValue = rhs;
	init();
}

SVLongValueDeviceParam::operator long() const
{
	return lValue;
}

void SVLongValueDeviceParam::init()
{
	m_eDataType = DeviceDataTypeLong;
	info.min = 0;
	info.max = LONG_MAX;
	info.multiplier = 1.0;
	info.offset = 0;
	info.sUnits = "";
}

SVLongValueDeviceParam& SVLongValueDeviceParam::operator=(const SVLongValueDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		lValue = rhs.lValue;
		info = rhs.info;
	}
	return *this;
}

SVLongValueDeviceParam& SVLongValueDeviceParam::operator=(long lVal)
{
	lValue = lVal;
	return *this;
}

HRESULT SVLongValueDeviceParam::GetValue(VARIANT& rv) const
{
	return ToVariant(lValue, rv);
}

HRESULT SVLongValueDeviceParam::SetValue(const VARIANT& rv)
{
	return FromVariant(lValue, rv);
}

long SVLongValueDeviceParam::GetScaledValue() const
{
	return static_cast<long>(ceil( lValue * info.multiplier ));
}

void SVLongValueDeviceParam::SetScaledValue(long lNewValue)
{
	if(info.multiplier != 0)
	{
		lValue = static_cast<long>(lNewValue / info.multiplier);
	}
	else
	{
		lValue = lNewValue;
	}
}

long SVLongValueDeviceParam::GetScaledMin() const
{
	return static_cast<long>(ceil(info.min * info.multiplier));
}

long SVLongValueDeviceParam::GetScaledMax() const
{
	return static_cast<long>(ceil(info.max * info.multiplier));
}

HRESULT SVLongValueDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVLongValueDeviceParam* pParam = dynamic_cast<const SVLongValueDeviceParam*>(pBaseParam))
	{
		info = pParam->info;
		hr = S_OK;
	}
	return hr;
}

#if defined(_DEBUG) || defined(_MINDEBUG)

long& LongValue(SVDeviceParamWrapper& w) 
{
	const SVLongValueDeviceParam* p = w.DerivedValue(p); 
	if (p == NULL) 
	{
		w = SVLongValueDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	return const_cast<SVLongValueDeviceParam*>(p)->lValue;
}

long LongValue(const SVDeviceParamWrapper& w) 
{
	const SVLongValueDeviceParam* p = w.DerivedValue(p); 
	if (p) 
		return p->lValue; 
	
	ASSERT(FALSE); 
	return 0; 
}

SVDeviceParamWrapper DeviceParam(long l) 
{
	return SVLongValueDeviceParam(l);
}

SVDeviceParamWrapper DeviceParam(int i) 
{
	return SVLongValueDeviceParam(i);
}

SVDeviceParamWrapper DeviceParam(short i) 
{
	return SVLongValueDeviceParam(i);
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVLongValueDeviceParam.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:07:18   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Apr 2013 13:37:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor that takes an argument of type SVDeviceParamEnum
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Jun 2009 08:50:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin (Moved from SVDeviceParams)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
