//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeReadSerialNumber
//* .File Name       : $Workfile:   SVGigeReadSerialNumber.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:56:56  $
//******************************************************************************

#pragma once

#include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVGigeFeature.h"

struct SVGigeReadSerialNumber
{
	// Getter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const;
};

