//******************************************************************************
//* COPYRIGHT (c) 2005 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxLicenseInterface
//* .File Name       : $Workfile:   SVMatroxLicenseInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:22:12  $
//******************************************************************************

#pragma once

class SVMatroxLicenseInterface
{
public:
	SVMatroxLicenseInterface(void);
	~SVMatroxLicenseInterface(void);

	HRESULT InitMatroxLicense();
	bool HasMatroxLicense();
	bool HasMatroxIdentificationLicense();
	bool HasMatroxGigELicense();

private:
	bool m_bMatroxImageLicense;
	bool m_bMatroxIdentificationLicense;
	bool m_bMatroxGigELicense;
};
