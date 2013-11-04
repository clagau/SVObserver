//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraParamValidateClass
//* .File Name       : $Workfile:   SVGigeCameraParamValidateClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:42:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeCameraParamValidateClass.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"

SVGigeCameraParamValidateClass::SVGigeCameraParamValidateClass()
{
}

SVGigeCameraParamValidateClass::~SVGigeCameraParamValidateClass()
{
}

bool SVGigeCameraParamValidateClass::ShouldUpdateParams( SVDeviceParamEnum p_eLastSetParam )
{
	bool l_bOk = false;
	
	switch ( p_eLastSetParam )
	{
		case DeviceParamCameraTriggerMode:
		{
			l_bOk = true;

			break;
		}

		default:
		{
			break;
		}
	}
	
	return l_bOk;
}

HRESULT SVGigeCameraParamValidateClass::UpdateParams( SVDeviceParamCollection& p_rFileParams, SVDeviceParamCollection& p_rDeviceParams )
{
	HRESULT l_hrOk = S_OK;
	
	SVDeviceParamWrapper l_wTriggerMode = p_rDeviceParams.Parameter( DeviceParamCameraTriggerMode );
	if ( l_wTriggerMode.IsValid() )
	{
		ULONG l_ulTriggerMode = LongValue( l_wTriggerMode );
		long l_lDetailLevel = 0;

		switch ( l_ulTriggerMode )
		{
		case 1:
			{
				l_lDetailLevel = 99;

				break;
			}
		default:
			{
				break;
			}
		}

		SVDeviceParam *l_pShutter = p_rFileParams.GetParameter( DeviceParamShutter );
		if ( l_pShutter != NULL )
		{
			if ( l_pShutter->SetDetailLevel( l_lDetailLevel ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}
		}

		SVDeviceParam *l_pShutterDelay = p_rFileParams.GetParameter( DeviceParamShutterDelay );
		if ( l_pShutterDelay != NULL )
		{
			if ( l_pShutterDelay->SetDetailLevel( l_lDetailLevel ) != S_OK )
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	
	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraFileLibrary\SVGigeCameraParamValidateClass.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:42:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 16:51:04   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
