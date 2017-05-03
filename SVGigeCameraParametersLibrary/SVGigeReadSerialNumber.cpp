//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeReadSerialNumber
//* .File Name       : $Workfile:   SVGigeReadSerialNumber.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:57:08  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeReadSerialNumber.h"
#include "SVUtilityLibrary/SVString.h"
#pragma endregion Includes

HRESULT SVGigeReadSerialNumber::operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
{ 
	SVString value;
	HRESULT l_Code = SVMatroxDigitizerInterface::GetGigeSerialNumber(*(Digitizer.get()), value);
	if (l_Code == S_OK)
	{
		rValue = _bstr_t(SVString(value).c_str()).Detach();
	}
	return l_Code;
}

