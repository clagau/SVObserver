//******************************************************************************
//* COPYRIGHT (c) 2010 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeLUTSetter
//* .File Name       : $Workfile:   SVGigeLUTSetter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:57:02  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizer.h"
#pragma endregion Includes

class SVGigeFeature;

// Note: SafeArray must be allocated to the proper dimension for the camera
// LUTEnable must be set prior to setting the Lut values
struct SVGigeLUTSetter
{
	// Setter
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const;

	HRESULT GetLutBandBounds(SAFEARRAY* parray, long& lBandLBound, long& lBandUBound) const;
	HRESULT GetLutBandSizeBounds(SAFEARRAY* parray, long& lBandSizeLBound, long& lBandSizeUBound) const;
};

