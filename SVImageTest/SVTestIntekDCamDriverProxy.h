// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekDCamDriverProxy.h
// * .File Name       : $Workfile:   SVTestIntekDCamDriverProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:36  $
// ******************************************************************************

#ifndef INCL_SVTESTINTEKDCAMDRIVERPROXY_H
#define INCL_SVTESTINTEKDCAMDRIVERPROXY_H

#include "SV1394CameraFileLibrary/SV1394CameraFileReader.h"
#include "SV1394CameraFileLibrary/SVDCamDriverProxy.h"

class SVDCamDriver;
class SVDigitizerLoadLibraryClass;
class SVTestAcquisitionClass;

#define SVMSG_SVO_20_INCORRECT_CAMERA_FILE 20

class SVTestIntekDCamDriverProxy : public SVDCamDriverProxy
{
private:
	SVTestAcquisitionClass* m_pAcquisition;

public:
	SVTestIntekDCamDriverProxy();
	virtual ~SVTestIntekDCamDriverProxy();

	void SetOwner(SVTestAcquisitionClass* pAcquisition);
	void Init(SVDCamDriver* pCameraDriver);
	
	bool IsOnline();
	
	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVImageTest\SVTestIntekDCamDriverProxy.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 11:15:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 11:42:56   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:43:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
