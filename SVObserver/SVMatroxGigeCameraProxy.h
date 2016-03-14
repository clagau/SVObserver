//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraProxy.h
//* .File Name       : $Workfile:   SVMatroxGigeCameraProxy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:33:58  $
//******************************************************************************

#ifndef SVMATROXGIGECAMERAPROXY_H
#define SVMATROXGIGECAMERAPROXY_H

#include "SVGigeCameraFileLibrary/SVGigeCameraFileReader.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraProxy.h"
#include "SVImageLibrary/SVImagingDeviceParams.h"
#include "SVUtilityLibrary/SVString.h"

class SVDigitizerLoadLibraryClass;
class SVMatroxGigeAcquisitionClass;

class SVMatroxGigeCameraProxy : public SVGigeCameraProxy
{
private:
	SVMatroxGigeAcquisitionClass* m_pAcquisition;

	//
	HRESULT SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const;
	SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );

public:
	SVMatroxGigeCameraProxy();
	virtual ~SVMatroxGigeCameraProxy();

	void SetOwner(SVMatroxGigeAcquisitionClass* pAcquisition);
	
	HRESULT GoOnline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT GoOffline(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool IsOnline() const;

	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetCameraFormatParameters(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam);
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );
	
	//************************************
	/// Check if camera file valid to current camera. Returns always TRUE, if invalid it throw an exception.
	//************************************
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SetGigeFeatureOverrides(const SVString& xmlData, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer );

	bool CanSoftwareTrigger() const;
	HRESULT EnableSoftwareTrigger(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SoftwareTrigger(unsigned long hDigitizer, SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SVLUTToSafeArray(const SVLut& lut, _variant_t& output);
};

#endif

