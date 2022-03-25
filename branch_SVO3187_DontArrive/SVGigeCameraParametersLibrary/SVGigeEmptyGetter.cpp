//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeEmptyGetter
//* .File Name       : $Workfile:   SVGigeEmptyGetter.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:46:34  $
//******************************************************************************
#pragma region Includes
#include "Stdafx.h"
#include "SVGigeEmptyGetter.h"
#include "SVGigeFeature.h"
#pragma endregion Includes

HRESULT SVGigeEmptyGetter::operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature&, _variant_t&) const
{
	return S_OK;
}

