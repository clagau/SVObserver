//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEmptyGetter
//* .File Name       : $Workfile:   SVGigeEmptyGetter.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:06  $
//******************************************************************************
#ifndef SVGIGEEMPTYGETTER_H
#define SVGIGEEMPTYGETTER_H

#include <comdef.h>
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerRef.h"
#include "SVGigeFeature.h"

struct SVGigeEmptyGetter
{
	//Getter
	HRESULT operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const;
};

#endif

