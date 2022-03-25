//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeCameraProxy.h
//* .File Name       : $Workfile:   SVMatroxGigeCameraProxy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 12:33:58  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVGigeCameraFileLibrary/SVGigeCameraProxy.h"
#include "CameraLibrary/SVDeviceParam.h"
#pragma endregion Includes

namespace SvTrig
{
	class SVDigitizerLoadLibraryClass;
}//namespace SvTrig

namespace SvIe
{
class SVMatroxGigeAcquisitionClass;
}

class SVCameraFormatsDeviceParam;
class SVLut;

class SVMatroxGigeCameraProxy : public SVGigeCameraProxy
{
private:
	SvIe::SVMatroxGigeAcquisitionClass* m_pAcquisition;

	//
	HRESULT SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer );

	SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );

public:
	SVMatroxGigeCameraProxy();
	virtual ~SVMatroxGigeCameraProxy();

	void SetOwner(SvIe::SVMatroxGigeAcquisitionClass* pAcquisition);
	
	HRESULT GoOnline(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT GoOffline(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	bool IsOnline() const;

	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer) override;
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer ) override;

	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer ) override;
	HRESULT SetCameraFormatParameters(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam);
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer ) override;
	
	//************************************
	/// Check if camera file valid to current camera. Returns always TRUE, if invalid it throw an exception.
	//************************************
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer) override;
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer) override;

	HRESULT SetGigeFeatureOverrides(const std::string& xmlData, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer );

	bool CanSoftwareTrigger() const;
	HRESULT EnableSoftwareTrigger(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SoftwareTrigger(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SVLUTToSafeArray(const SVLut& lut, _variant_t& output);
};
