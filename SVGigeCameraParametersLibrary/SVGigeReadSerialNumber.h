//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeReadSerialNumber
//* .File Name       : $Workfile:   SVGigeReadSerialNumber.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:56:56  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizer.h"
#pragma endregion Includes

class SVGigeFeature;

struct SVGigeReadSerialNumber
{
	// Getter
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const;
};

