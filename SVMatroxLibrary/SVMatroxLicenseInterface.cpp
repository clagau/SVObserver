//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxLicenseInterface
//* .File Name       : $Workfile:   SVMatroxLicenseInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:22:04  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxLicenseInterface.h"
#include "SVMatroxImagingLibrary.h"

SVMatroxLicenseInterface::SVMatroxLicenseInterface(void)
{
	m_bMatroxImageLicense = false;
	m_bMatroxGigELicense = false;
	m_bMatroxIdentificationLicense = false;
}

SVMatroxLicenseInterface::~SVMatroxLicenseInterface(void)
{
}

HRESULT SVMatroxLicenseInterface::InitMatroxLicense()
{
	HRESULT hrRet = S_OK;
	bool bDebug = false;

	MIL_ID milID = MsysAlloc(M_SYSTEM_DEFAULT , M_DEFAULT, M_DEFAULT, M_NULL);

	try
	{
#if SV_CURRENT_MIL_VERSION == 0x0900
		MIL_INT LicenseModules=0;
		MIL_INT milInquire = MsysInquire(milID, M_LICENSE_MODULES, &LicenseModules);//get all licenses off dongle, dev dongle
																					//only have M_LICENSE_DEBUG

		MIL_INT milVal = MappInquire(M_HW_LIC_KEY_FINGERPRINT ,M_NULL); //check to see if dongle has any fingerprint/dev dongle have
																 //no fingerprint

		if ( !milVal ) 
		{   //check to see if it is not a pre-programmed dongle
			milVal = MappInquire(M_HW_ID_KEY_FINGERPRINT,M_NULL);
			if ( !milVal )
			{   // check for key plus component
				milVal = MappInquire(M_HW_ID_KEY_PLUS_FINGERPRINT,M_NULL);
			}
			if ( !milVal )
			{	//check for any hardware fingerprint
				milVal = MappInquire(M_ANY_FINGERPRINT, M_NULL);
			}
		}

		//check to see if it is a dev license
		if ( LicenseModules & M_LICENSE_DEBUG )
		{
			bDebug = true;
		}
		//Set Matrox Dongle License
		if ( milVal || bDebug)
		{
			m_bMatroxImageLicense = true;
		}

		//Set Matrox Identy License
		if ( (LicenseModules & ( M_LICENSE_CODE )) || bDebug )
		{
			m_bMatroxIdentificationLicense = true;
		}

		//Set Matrox GigE License
		if ( (LicenseModules & ( M_LICENSE_INTERFACE )) || bDebug )
		{
			m_bMatroxGigELicense = true;
		}
#else
	    m_bMatroxImageLicense = true;
	    m_bMatroxIdentificationLicense = true;
	    m_bMatroxGigELicense = true;
#endif
	}
	catch(...)
	{
		hrRet = E_FAIL;
	}

	MsysFree(milID); 

	return hrRet;
}

bool SVMatroxLicenseInterface::HasMatroxLicense()
{
	return m_bMatroxImageLicense;
}

bool SVMatroxLicenseInterface::HasMatroxIdentificationLicense()
{
	return m_bMatroxIdentificationLicense;
}

bool SVMatroxLicenseInterface::HasMatroxGigELicense()
{
	return m_bMatroxGigELicense;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxLicenseInterface.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:22:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2013 08:43:40   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added #if checks to see if it was MIL 9
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   27 Feb 2013 10:01:06   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  796
 * SCR Title:  Add a License Manager that will handle the Matrox Licenses
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first iteration of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/