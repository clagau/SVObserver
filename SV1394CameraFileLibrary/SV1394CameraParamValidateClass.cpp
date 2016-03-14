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

