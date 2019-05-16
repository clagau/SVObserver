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

#pragma region Includes
#include "stdafx.h"
#include "SVMatroxLicenseInterface.h"
#pragma endregion Includes

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

	long long LicenseModules=0;
	try
	{
		long long appID = MappInquire( M_CURRENT_APPLICATION, 0 );
		long long milInquire = MappInquire(appID, M_LICENSE_MODULES, &LicenseModules);//get all licenses off dongle, dev dongle
																					//only have M_LICENSE_DEBUG
		long long milVal = MappInquire(M_ID_KEY_FINGERPRINT,M_NULL);

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

