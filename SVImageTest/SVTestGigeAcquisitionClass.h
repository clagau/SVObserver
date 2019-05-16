//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeAcquisitionClass.h
//* .File Name       : $Workfile:   SVTestGigeAcquisitionClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:10:10  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVTestAcquisitionClass.h"
#include "SVTestGigeCameraProxy.h"
#pragma endregion Includes

class SVTestGigeAcquisitionClass : public SVTestAcquisitionClass
{
private:
	SVTestGigeCameraProxy m_gigeCameraProxy;

	HRESULT SetGigeFeatureOverrides(const std::string& rFeatureOverrides);

public:
	SVTestGigeAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer);
	virtual ~SVTestGigeAcquisitionClass();

	virtual HRESULT ReadCameraFile( const std::string&) override;

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams ) override;
	virtual bool CameraMatchesCameraFile() override;
	
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams ) override;
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams ) override;

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw ) override;

	virtual bool StartAcquire(SVCameraPage& p_rDisplay) override;
	virtual void StopAcquire() override;
};

