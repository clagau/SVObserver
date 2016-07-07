//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEmptySetter
//* .File Name       : $Workfile:   SVGigeEmptySetter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:54:38  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeEmptySetter.h"
#pragma endregion Includes

HRESULT SVGigeEmptySetter::operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, const _variant_t& rValue)
{
	return S_OK;
}

