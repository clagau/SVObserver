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
		if ( nullptr != l_pShutter )
		{
			if ( S_OK != l_pShutter->SetDetailLevel( l_lDetailLevel ) )
			{
				l_hrOk = S_FALSE;
			}
		}

		SVDeviceParam *l_pShutterDelay = p_rFileParams.GetParameter( DeviceParamShutterDelay );
		if ( nullptr != l_pShutterDelay )
		{
			if (S_OK != l_pShutterDelay->SetDetailLevel( l_lDetailLevel ) )
			{
				l_hrOk = S_FALSE;
			}
		}
	}
	
	return l_hrOk;
}

