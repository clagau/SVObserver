//******************************************************************************
//* COPYRIGHT (c) 2009 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVGigeCameraProxy
//* .File Name       : $Workfile:   SVGigeCameraProxy.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:43:34  $
//******************************************************************************

#pragma region Includes
#include "CameraLibrary/SVDeviceParam.h"
#pragma endregion Includes

namespace SvTrig
{
class SVDigitizerLoadLibraryClass;
}

class SVDeviceParamCollection;

class SVGigeCameraProxy
{
protected:
	SVGigeCameraProxy();

public:
	virtual ~SVGigeCameraProxy();

	
	virtual HRESULT InitializeDevice( const SVDeviceParamWrapper& rwParam, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)=0;
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )=0;

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw, SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )=0;
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer )=0;
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams, SVDeviceParamCollection& rDestDeviceParams );
	
	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)=0;
	virtual bool CameraMatchesCameraFile(const SVDeviceParamCollection& rCameraFileDeviceParams, unsigned long hDigitizer, SvTrig::SVDigitizerLoadLibraryClass* pDigitizer)=0;
};

