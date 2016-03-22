// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekDCamDriverProxy.h
// * .File Name       : $Workfile:   SVIntekDCamDriverProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:05:50  $
// ******************************************************************************

#pragma once

#include "SV1394CameraFileLibrary/SV1394CameraFileReader.h"
#include "SV1394CameraFileLibrary/SVDCamDriverProxy.h"

class SVDCamDriver;
class SVDigitizerLoadLibraryClass;
class SVIntekAcquisitionClass;

class SVIntekDCamDriverProxy : public SVDCamDriverProxy
{
private:
	SVIntekAcquisitionClass* m_pAcquisition;

public:
	SVIntekDCamDriverProxy();
	virtual ~SVIntekDCamDriverProxy();
	void SetOwner(SVIntekAcquisitionClass* pAcquisition);
	void Init(SVDCamDriver* pCameraDriver);
	bool IsOnline() const;

//	HRESULT LoadFiles(SVFileNameArrayClass& rFiles, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
//	HRESULT ReadCameraFile( const CString& sFilename, SVDeviceParamCollection& rParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
		
	
	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

//	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams );
//	HRESULT UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam );
//	HRESULT UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam );

	HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
};

