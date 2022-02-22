//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeStandardCameraFeatures
//* .File Name       : $Workfile:   SVGigeStandardCameraFeatures.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"
#pragma endregion Includes

class SVGigeStandardCameraFeatures
{
public:
	static const SVGigeDeviceParameterMap& GetStandardFeatures();

private:
	SVGigeStandardCameraFeatures();
	~SVGigeStandardCameraFeatures();
};

