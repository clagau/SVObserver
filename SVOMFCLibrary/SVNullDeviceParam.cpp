//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVNullDeviceParam
//* .File Name       : $Workfile:   SVNullDeviceParam.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 13:07:50  $
//******************************************************************************
#include "stdafx.h"
#include "SVNullDeviceParam.h"

SVClonable* SVNullDeviceParam::CloneImpl() const
{
	return new SVNullDeviceParam(*this);
}

SVNullDeviceParam::SVNullDeviceParam()
: SVDeviceParam(DeviceParamInvalid)
{
}

// Why pass a parameter and not use it...
SVNullDeviceParam::SVNullDeviceParam(SVDeviceParamEnum typeEnum)
: SVDeviceParam(DeviceParamInvalid)
{
}

SVNullDeviceParam::SVNullDeviceParam(const SVNullDeviceParam& rhs)
: SVDeviceParam(rhs)
{
}

SVNullDeviceParam& SVNullDeviceParam::operator=(const SVNullDeviceParam& rhs)
{
	return *this;
}

// Why pass a parameter and not use it...
void SVNullDeviceParam::Init(SVDeviceParamEnum e)
{
	m_eParam = DeviceParamInvalid;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVOMFCLibrary\SVNullDeviceParam.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 13:07:50   bWalter
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
 *    Rev 1.0   03 Jun 2009 08:50:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin (Moved from SVDeviceParams)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
