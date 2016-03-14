//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerLightReferenceEnums
//* .File Name       : $Workfile:   SVMatroxDigitizerLightReferenceEnums.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:29:28  $
//******************************************************************************

#ifndef SVMATROXDIGITIZERLIGHTREFERENCEENUMS_H
#define SVMATROXDIGITIZERLIGHTREFERENCEENUMS_H

#include "SVMatroxLibrary/SVMatroxTypedefs.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"


/**
@SVObjectName Matrox Digitizer Light Reference Enum

@SVObjectOverview This enum is used to get/set the Light Reference parameters of the Digitizer.

@SVObjectOperations None

*/
struct SVMatroxDigitizerLightReference
{
	enum SVLightReferenceEnum
	{
		SVBrightness,		// M_BRIGHTNESS_REF
		SVContrast,			// M_CONTRAST_REF
		SVWhiteBalanceU,	// M_WHITE_BALANCE_U
		SVWhiteBalanceV,	// M_WHITE_BALANCE_V
		SVSaturation,		// M_SATURATION_REF (Color only)
		SVHue,				// M_HUE_REF (Color only)
	};
	// define convertor for from/to SVEnum/Matrox types
	typedef SVMatroxEnumConvertor<SVLightReferenceEnum, long> SVLightReferenceEnumMap;
	static SVLightReferenceEnumMap m_LightReferenceEnumConvertor;
};

#endif

