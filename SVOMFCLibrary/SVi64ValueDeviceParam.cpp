//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVi64ValueDeviceParam
//* .File Name       : $Workfile:   SVi64ValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:06:46  $
//******************************************************************************

#include "Stdafx.h"
#include "SVi64ValueDeviceParam.h"

SVi64ValueDeviceParam::SVi64ValueDeviceParam()
: iValue(0)
{
	init();
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
, iValue(0)
{
	init();
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(const SVi64ValueDeviceParam& rhs) 
: SVDeviceParam(rhs)
{
	m_eDataType = DeviceDataTypei64;
	iValue = rhs.iValue;
	info = rhs.info;
}

SVClonable* SVi64ValueDeviceParam::CloneImpl() const
{
	return new SVi64ValueDeviceParam(*this);
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(const __int64& rhs)
{
	m_eDataType = DeviceDataTypei64;
	iValue = rhs;
	init();
}

SVi64ValueDeviceParam::SVi64ValueDeviceParam(const LARGE_INTEGER& rhs)
{
	iValue = rhs.QuadPart;
	init();
}

SVi64ValueDeviceParam::operator __int64() const
{
	return iValue;
}

void SVi64ValueDeviceParam::init()
{
	m_eDataType = DeviceDataTypei64;
	info.min = 0;
	info.max = _I64_MAX;
}

SVi64ValueDeviceParam& SVi64ValueDeviceParam::operator=(const SVi64ValueDeviceParam& rhs)
{
	if (this != &rhs)
	{
		iValue = rhs.iValue;
		info = rhs.info;
	}
	return *this;
}

HRESULT SVi64ValueDeviceParam::GetValue(VARIANT& rv) const
{
	return ToVariant(iValue, rv);
}

HRESULT SVi64ValueDeviceParam::SetValue(const VARIANT& rv)
{
	return FromVariant(iValue, rv);
}

HRESULT SVi64ValueDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVi64ValueDeviceParam* pParam = dynamic_cast<const SVi64ValueDeviceParam*>(pBaseParam))
	{
		info = pParam->info;
		hr = S_OK;
	}
	return hr;
}

#if defined(_DEBUG) || defined(_MINDEBUG)

__int64& i64Value(SVDeviceParamWrapper& w) 
{
	const SVi64ValueDeviceParam* p = w.DerivedValue(p); 
	if (p == NULL) 
	{
		w = SVi64ValueDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	ASSERT(p); 
	return const_cast<SVi64ValueDeviceParam*>(p)->iValue;
}

__int64 i64Value(const SVDeviceParamWrapper& w) 
{
	const SVi64ValueDeviceParam* p = w.DerivedValue(p); 
	if (p) 
		return p->iValue; 
	
	ASSERT(FALSE); 
	return 0;
}

SVDeviceParamWrapper DeviceParam(__int64 i) 
{
	return SVi64ValueDeviceParam(i);
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVi64ValueDeviceParam.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:06:46   bWalter
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
