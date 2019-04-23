//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxLicenseInterface
//* .File Name       : $Workfile:   SVMatroxLicenseInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 11:08:08  $
//******************************************************************************

#include "stdafx.h"
#include "SVMatroxLicenseInterface.h"
#include "SVMatroxImagingLibrary.h"
#include "SVMatroxTypedefs.h"

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

	SVMatroxInt LicenseModules=0;
	try
	{
		SVMatroxInt appID = MappInquire( M_CURRENT_APPLICATION, 0 );
		SVMatroxInt milInquire = MappInquire(appID, M_LICENSE_MODULES, &LicenseModules);//get all licenses off dongle, dev dongle
																					//only have M_LICENSE_DEBUG
		SVMatroxInt milVal = MappInquire(M_ID_KEY_FINGERPRINT,M_NULL);

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
	}
	catch(...)
	{
		hrRet = E_FAIL;
	}

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

