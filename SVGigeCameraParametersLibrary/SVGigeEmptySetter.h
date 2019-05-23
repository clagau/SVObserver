//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEmptySetterFunc
//* .File Name       : $Workfile:   SVGigeEmptySetter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:42  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizer.h"
#pragma endregion Includes

class SVGigeFeature;

struct SVGigeEmptySetter
{
	// Setter
	HRESULT operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue);
};

