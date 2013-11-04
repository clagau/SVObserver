//******************************************************************************
//* COPYRIGHT (c) 2005 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxLicenseInterface
//* .File Name       : $Workfile:   SVMatroxLicenseInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 15:22:12  $
//******************************************************************************

#ifndef SV_MATROX_LICENSE_INTERFACE
#define SV_MATROX_LICENSE_INTERFACE
//#pragma once

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

#endif
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxLibrary\SVMatroxLicenseInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 15:22:12   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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