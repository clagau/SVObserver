//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraParamValidateClass
//* .File Name       : $Workfile:   SV1394CameraParamValidateClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:32:58  $
//******************************************************************************

#include "stdafx.h"
#include "SV1394CameraParamValidateClass.h"
#include "SVOMFCLibrary/SVLongValueDeviceParam.h"

SV1394CameraParamValidateClass::SV1394CameraParamValidateClass()
{
}

SV1394CameraParamValidateClass::~SV1394CameraParamValidateClass()
{
}

bool SV1394CameraParamValidateClass::ShouldUpdateParams( SVDeviceParamEnum p_eLastSetParam )
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

HRESULT SV1394CameraParamValidateClass::UpdateParams( SVDeviceParamCollection& p_rFileParams, SVDeviceParamCollection& p_rDeviceParams )
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
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SV1394CameraFileLibrary\SV1394CameraParamValidateClass.cpp_v  $
 * 
 *    Rev 1.0   18 Apr 2013 17:32:58   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Jun 2009 09:46:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Include file definitions due to changes in SVDeviceParams
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 14:55:50   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   23 Jul 2004 08:21:06   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  435
 * SCR Title:  Add Bulb Mode Operation to the Digital SVIM for Point Gray Dragonfly Cameras
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   New classes added to handle the validation of 1394 camera parameters and adjust constant/visibility data.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
