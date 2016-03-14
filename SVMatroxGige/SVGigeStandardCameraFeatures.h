//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeStandardCameraFeatures
//* .File Name       : $Workfile:   SVGigeStandardCameraFeatures.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:36:58  $
//******************************************************************************

#ifndef SVGIGESTANDARDCAMERAFEATURES_H
#define SVGIGESTANDARDCAMERAFEATURES_H

#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"

class SVGigeStandardCameraFeatures
{
public:
	static const SVGigeDeviceParameterMap& GetStandardFeatures();

private:
	SVGigeStandardCameraFeatures();
	~SVGigeStandardCameraFeatures();
};

#endif

