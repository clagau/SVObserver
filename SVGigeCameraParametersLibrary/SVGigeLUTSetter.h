//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
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
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVGigeFeature.h"
#pragma endregion Includes

// Note: SafeArray must be allocated to the proper dimension for the camera
// LUTEnable must be set prior to setting the Lut values
struct SVGigeLUTSetter
{
	// Setter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue) const;

	HRESULT GetLutBandBounds(SAFEARRAY* parray, long& lBandLBound, long& lBandUBound) const;
	HRESULT GetLutBandSizeBounds(SAFEARRAY* parray, long& lBandSizeLBound, long& lBandSizeUBound) const;
};

