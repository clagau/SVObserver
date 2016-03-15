// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekDCamDriverProxy.h
// * .File Name       : $Workfile:   SVTestIntekDCamDriverProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 11:47:20  $
// ******************************************************************************

#ifndef INCL_SVTESTINTEKDCAMDRIVERPROXY_H
#define INCL_SVTESTINTEKDCAMDRIVERPROXY_H

#include "SV1394CameraFileLibrary/SV1394CameraFileReader.h"
#include "SV1394CameraFileLibrary/SVDCamDriverProxy.h"

class SVDCamDriver;
class SVDigitizerLoadLibraryClass;
class SVTestAcquisitionClass;

#include "SVMessage/SVMessage.h" // for SVMSG_SVO_20_INCORRECT_CAMERA_FILE

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

