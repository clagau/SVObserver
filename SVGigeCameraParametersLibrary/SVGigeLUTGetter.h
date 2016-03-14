//******************************************************************************
//* COPYRIGHT (c) 2010 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeLUTGetter
//* .File Name       : $Workfile:   SVGigeLUTGetter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:36  $
//******************************************************************************

#pragma once

#include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVGigeFeature.h"

// Note: SafeArray must be allocated to the proper dimension for the camera
// LUTEnable must be set prior to getting the Lut values
struct SVGigeLUTGetter
{
	// Getter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const;

	HRESULT GetLutBandBounds(SAFEARRAY* parray, long& lBandLBound, long& lBandUBound) const;
	HRESULT GetLutBandSizeBounds(SAFEARRAY* parray, long& lBandSizeLBound, long& lBandSizeUBound) const;
};

