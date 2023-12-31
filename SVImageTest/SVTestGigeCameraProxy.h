//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeCameraProxy.h
//* .File Name       : $Workfile:   SVTestGigeCameraProxy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   15 May 2013 19:38:20  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVGigeCameraFileLibrary/SVGigeCameraProxy.h"
#pragma endregion Includes


namespace SvTrig
{
	class SVDigitizerLoadLibraryClass;
}//namespace SvTrig


class SVCameraFormatsDeviceParam;
class SVLut;
class SVTestAcquisitionClass;

class SVTestGigeCameraProxy : public SVGigeCameraProxy
{
private:
	SVTestAcquisitionClass* m_pAcquisition;

	//
	HRESULT SetDigitizerParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer );

	SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );

public:
	SVTestGigeCameraProxy();
	virtual ~SVTestGigeCameraProxy();

	void SetOwner(SVTestAcquisitionClass* pAcquisition);
	
	HRESULT GoOnline(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT GoOffline(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	bool IsOnline() const;

	HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer );
	HRESULT SetCameraFormatParameters(unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer, const SVCameraFormatsDeviceParam* pParam);
	HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer );
	
	HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);

	HRESULT SetGigeFeatureOverrides(const std::string& rXmlData, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer);
	HRESULT SetDigitizerParameter( const SVDeviceParamWrapper& rw, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer );

	HRESULT SVLUTToSafeArray(const SVLut& lut, _variant_t& output);
};

