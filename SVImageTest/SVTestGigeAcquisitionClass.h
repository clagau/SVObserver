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

#include "SVTestAcquisitionClass.h"
#include "SVTestGigeCameraProxy.h"

class SVTestGigeAcquisitionClass : public SVTestAcquisitionClass
{
private:
	SVTestGigeCameraProxy m_gigeCameraProxy;

	HRESULT SetGigeFeatureOverrides(const SVString& rFeatureOverrides);

public:
	SVTestGigeAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer);
	virtual ~SVTestGigeAcquisitionClass();

	virtual HRESULT ReadCameraFile( const SVString&) override;

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams ) override;
	virtual bool CameraMatchesCameraFile() override;
	
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams ) override;
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams ) override;

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw ) override;

	virtual bool StartAcquire(SVCameraPage& p_rDisplay) override;
	virtual void StopAcquire() override;
};

