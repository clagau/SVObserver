//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeReadIPAddress
//* .File Name       : $Workfile:   SVGigeReadIPAddress.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2013 15:02:02  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVGigeFeature.h"
#pragma endregion Includes

struct SVGigeReadIPAddress
{
	// Getter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const;
};

