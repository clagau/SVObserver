//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraParamValidateClass
//* .File Name       : $Workfile:   SVGigeCameraParamValidateClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:41:40  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "CameraLibrary/SVDeviceParam.h"
#pragma endregion Includes

class SVDeviceParamCollection;

class SVGigeCameraParamValidateClass  
{
public:
	SVGigeCameraParamValidateClass();
	virtual ~SVGigeCameraParamValidateClass();

	bool ShouldUpdateParams( SVDeviceParamEnum p_eLastSetParam );

	HRESULT UpdateParams( SVDeviceParamCollection& p_rFileParams, SVDeviceParamCollection& p_rDeviceParams );
};

