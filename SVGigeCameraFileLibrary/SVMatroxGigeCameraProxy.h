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
#pragma once

#pragma region Includes
#include "SVGigeCameraFileLibrary/SVGigeCameraProxy.h"
#pragma endregion Includes

namespace SvTh
{
	class SVDigitizerLoadLibraryClass;
}//namespace SvTh

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
	HRESULT SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer );

	SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );

public:
	SVMatroxGigeCameraProxy();
	virtual ~SVMatroxGigeCameraProxy();

	void SetOwner(SvIe::SVMatroxGigeAcquisitionClass* pAcquisition);
	
	HRESULT GoOnline(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT GoOffline(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);
	bool IsOnline() const;

	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetCameraFormatParameters(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam);
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer );
	
	//************************************
	/// Check if camera file valid to current camera. Returns always TRUE, if invalid it throw an exception.
	//************************************
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SetGigeFeatureOverrides(const std::string& xmlData, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer );

	bool CanSoftwareTrigger() const;
	HRESULT EnableSoftwareTrigger(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SoftwareTrigger(unsigned long hDigitizer, SvTh::SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SVLUTToSafeArray(const SVLut& lut, _variant_t& output);
};
