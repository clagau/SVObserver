//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraFileInfoStruct
//* .File Name       : $Workfile:   SVGigeCameraFileInfoStruct.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:41:54  $
//******************************************************************************

#ifndef SVGIGECAMERAFILEINFOSTRUCT_H
#define SVGIGECAMERAFILEINFOSTRUCT_H

#include "SVUtilityLibrary/SVString.h"

struct SVGigeCameraFileInfoStruct
{
	// input params
	SVString sFilename;
	bool bColorSystem;

	// output params
	SVString sVersion;
	SVString sCameraType;

	SVString sFeatureOverrides;

	SVGigeCameraFileInfoStruct() : bColorSystem(false) {}
};

#endif

