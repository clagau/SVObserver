//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraParamValidateClass
//* .File Name       : $Workfile:   SVGigeCameraParamValidateClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:41:40  $
//******************************************************************************

#pragma once

#include "CameraLibrary/SVDeviceParams.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"

class SVGigeCameraParamValidateClass  
{
public:
	SVGigeCameraParamValidateClass();
	virtual ~SVGigeCameraParamValidateClass();

	bool ShouldUpdateParams( SVDeviceParamEnum p_eLastSetParam );

	HRESULT UpdateParams( SVDeviceParamCollection& p_rFileParams, SVDeviceParamCollection& p_rDeviceParams );
};

