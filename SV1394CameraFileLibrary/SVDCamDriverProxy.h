// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDCamDriverProxy.h
// * .File Name       : $Workfile:   SVDCamDriverProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:36:38  $
// ******************************************************************************

#ifndef INCL_SVDCAMDRIVERPROXY_H
#define INCL_SVDCAMDRIVERPROXY_H

#include "SV1394CameraFileReader.h"

class SVDCamDriver;
class SVDigitizerLoadLibraryClass;

/////////////////////////////////////////////////////////////////////////////////
// this class decouples the SVDCamDriver classes from the SVAcquistion class
// so that SVImagetest can use the SVDCamDrivers and load the camera files
/////////////////////////////////////////////////////////////////////////////////
class SVDCamDriverProxy
{
protected:
	SVDCamDriver* m_pCameraDriver;
	SVDCamDriverProxy();

public:
	virtual ~SVDCamDriverProxy();

	static HRESULT ReadCameraFileImpl( SV1394CameraFileInfoStruct& p_rInfo, SVDeviceParamCollection& p_rParams );
	static HRESULT SetParameterDefaults( SVDeviceParamCollection& rParams );
	
	virtual HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)=0;
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )=0;

//	virtual HRESULT UpdateLightReferenceAttributes( const SVDeviceParamCollection& rParams );
//	virtual HRESULT UpdateLightReferenceAttributes( const SVDeviceParamWrapper& rwParam );
//	virtual HRESULT UpdateLightReferenceAttributes( int iBand, const SVLongValueDeviceParam* pParam );

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )=0;
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )=0;
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )=0;
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )=0;
	
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )=0;
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer )=0;
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, SVDeviceParamCollection& rDestDeviceParams );
	
	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)=0;
	virtual bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer)=0;
};

#endif

