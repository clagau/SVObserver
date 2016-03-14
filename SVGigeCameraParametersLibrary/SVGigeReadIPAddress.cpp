//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeReadIPAddress
//* .File Name       : $Workfile:   SVGigeReadIPAddress.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   12 Jun 2013 15:01:40  $
//******************************************************************************

#include "stdafx.h"
#include "SVGigeReadIPAddress.h"

HRESULT SVGigeReadIPAddress::operator()(SVMatroxDigitizerRef Digitizer, const SVGigeFeature& rFeature, _variant_t& rValue) const
{ 
	//SVMatroxString value;
	__int64 value;
	SVString sTmp;
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetGigeIPAddress(*(Digitizer.get()), value);
	if (l_Code == SVMEE_STATUS_OK)
	{
		sTmp = SvUl_SF::Format("%u.%u.%u.%u", 
					static_cast<unsigned char>(value),
					static_cast<unsigned char>(value >> 8) & 0xFF,
					static_cast<unsigned char>(value >> 16) & 0xFF,
					static_cast<unsigned char>(value >> 24) & 0xFF);
		rValue = _bstr_t(sTmp.c_str()).Detach();
	}
	return l_Code;
}

