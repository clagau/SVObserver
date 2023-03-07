//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeReadIPAddress
//* .File Name       : $Workfile:   SVGigeReadIPAddress.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2013 15:01:40  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVGigeReadIPAddress.h"
#include "SVGigeFeature.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#pragma endregion Includes

HRESULT SVGigeReadIPAddress::operator()(SVMatroxDigitizerPtr Digitizer, const SVGigeFeature& , _variant_t& rValue) const
{ 
	//SVMatroxString value;
	__int64 value;
	HRESULT l_Code = SVMatroxDigitizerInterface::GetGigeIPAddress(*(Digitizer.get()), value);
	if (l_Code == S_OK)
	{
		std::string sTmp = std::format("{}.{}.{}.{}",
					static_cast<unsigned char>(value),
					static_cast<unsigned char>(value >> 8) & 0xFF,
					static_cast<unsigned char>(value >> 16) & 0xFF,
					static_cast<unsigned char>(value >> 24) & 0xFF);
		rValue = _bstr_t(sTmp.c_str()).Detach();
	}
	return l_Code;
}

