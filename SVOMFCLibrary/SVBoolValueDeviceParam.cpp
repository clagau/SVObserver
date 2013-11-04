//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVBoolValueDeviceParam
//* .File Name       : $Workfile:   SVBoolValueDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 12:59:10  $
//******************************************************************************
#include "Stdafx.h"
#include "SVBoolValueDeviceParam.h"

SVBoolValueDeviceParam::SVBoolValueDeviceParam()
: bValue(false) 
{
	m_eDataType = DeviceDataTypeBool;
}

SVBoolValueDeviceParam::SVBoolValueDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(typeEnum)
, bValue(false) 
{
	m_eDataType = DeviceDataTypeBool;
}

SVBoolValueDeviceParam::SVBoolValueDeviceParam(const SVBoolValueDeviceParam& rhs) 
: SVDeviceParam(rhs)
{
	m_eDataType = DeviceDataTypeBool;
	bValue = rhs.bValue;
	info = rhs.info;
}

SVClonable* SVBoolValueDeviceParam::CloneImpl() const
{
	return new SVBoolValueDeviceParam(*this);
}

SVBoolValueDeviceParam::SVBoolValueDeviceParam(bool rhs)	// not const bool& so that we get automatic conversion from other integral types for free
: SVDeviceParam(DeviceParamInvalid)
{
	m_eDataType = DeviceDataTypeBool;
	bValue = rhs;
}

SVBoolValueDeviceParam::operator bool() const
{
	return bValue;
}

SVBoolValueDeviceParam& SVBoolValueDeviceParam::operator=(const SVBoolValueDeviceParam& rhs)
{
	if ( this != &rhs )
	{
		bValue = rhs.bValue;
		info = rhs.info;
	}
	return *this;
}

SVBoolValueDeviceParam& SVBoolValueDeviceParam::operator=(bool bVal)
{
	bValue = bVal;
	return *this;
}

HRESULT SVBoolValueDeviceParam::GetValue(VARIANT& rv) const
{
	return ToVariant(bValue, rv);
}

HRESULT SVBoolValueDeviceParam::SetValue(const VARIANT& rv)
{
	return FromVariant(bValue, rv);
}

HRESULT SVBoolValueDeviceParam::SetMetadata(const SVDeviceParam* pBaseParam)
{
	HRESULT hr = S_FALSE;
	if (const SVBoolValueDeviceParam* pParam = dynamic_cast<const SVBoolValueDeviceParam*>(pBaseParam))
	{
		info = pParam->info;
		hr = S_OK;
	}
	return hr;
}

#if defined(_DEBUG) || defined(_MINDEBUG)

bool& BoolValue(SVDeviceParamWrapper& w) 
{
	const SVBoolValueDeviceParam* p = w.DerivedValue(p); 
	if (p == NULL) 
	{
		w = SVBoolValueDeviceParam(); 
		p = w.DerivedValue(p);
	} 
	ASSERT(p); 
	return const_cast<SVBoolValueDeviceParam*>(p)->bValue;
}

bool BoolValue(const SVDeviceParamWrapper& w) 
{
	const SVBoolValueDeviceParam* p = w.DerivedValue(p); 
	if (p) 
		return p->bValue; 

	ASSERT(FALSE); 
	return 0;
}

SVDeviceParamWrapper DeviceParam(bool b) 
{
	return SVBoolValueDeviceParam(b);
}

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVBoolValueDeviceParam.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 12:59:10   bWalter
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
