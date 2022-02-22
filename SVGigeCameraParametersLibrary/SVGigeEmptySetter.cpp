//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVGigeFeature.h"
#pragma endregion Includes

HRESULT SVGigeEmptySetter::operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& , const _variant_t& )
{
	return S_OK;
}

