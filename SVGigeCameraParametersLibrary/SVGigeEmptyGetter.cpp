//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEmptyGetter
//* .File Name       : $Workfile:   SVGigeEmptyGetter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:34  $
//******************************************************************************
#include "Stdafx.h"
#include "SVGigeEmptyGetter.h"

HRESULT SVGigeEmptyGetter::operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
{
	return S_OK;
}

