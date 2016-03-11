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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVGigeCameraParametersLibrary\SVGigeReadIPAddress.cpp_v  $
 * 
 *    Rev 1.0   12 Jun 2013 15:01:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 May 2013 10:10:58   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
