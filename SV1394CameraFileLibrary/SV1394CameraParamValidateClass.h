//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SV1394CameraParamValidateClass
//* .File Name       : $Workfile:   SV1394CameraParamValidateClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:32:58  $
//******************************************************************************

#pragma once

#include "SVOMFCLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"

class SV1394CameraParamValidateClass  
{
public:
	SV1394CameraParamValidateClass();
	virtual ~SV1394CameraParamValidateClass();

	bool ShouldUpdateParams( SVDeviceParamEnum p_eLastSetParam );

	HRESULT UpdateParams( SVDeviceParamCollection& p_rFileParams, SVDeviceParamCollection& p_rDeviceParams );

};

