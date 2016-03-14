//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeCameraProxy.h
//* .File Name       : $Workfile:   SVTestGigeCameraProxy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2013 19:38:20  $
//******************************************************************************

#ifndef SVTESTGIGECAMERAPROXY_H
#define SVTESTGIGECAMERAPROXY_H

#include "SVGigeCameraFileLibrary/SVGigeCameraFileReader.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraProxy.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVUtilityLibrary/SVString.h"

class SVDigitizerLoadLibraryClass;
class SVTestAcquisitionClass;

class SVTestGigeCameraProxy : public SVGigeCameraProxy
{
private:
	SVTestAcquisitionClass* m_pAcquisition;

	//
	HRESULT SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const;
	SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );

public:
	SVTestGigeCameraProxy();
	virtual ~SVTestGigeCameraProxy();

	void SetOwner(SVTestAcquisitionClass* pAcquisition);
	
	HRESULT GoOnline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT GoOffline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool IsOnline() const;

	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetCameraFormatParameters(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam);
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SetGigeFeatureOverrides(const SVString& xmlData, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SVLUTToSafeArray(const SVLut& lut, _variant_t& output);
};

#endif

