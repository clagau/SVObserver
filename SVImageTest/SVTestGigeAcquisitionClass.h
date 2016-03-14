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

#ifndef SVTESTGIGEACQUISITIONCLASS_H
#define SVTESTGIGEACQUISITIONCLASS_H

#include "SVTestAcquisitionClass.h"
#include "SVTestGigeCameraProxy.h"

class SVTestGigeAcquisitionClass : public SVTestAcquisitionClass
{
private:
	SVTestGigeCameraProxy m_gigeCameraProxy;

	HRESULT SetGigeFeatureOverrides(const SVString& featureOverrides);

public:
	SVTestGigeAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer);
	virtual ~SVTestGigeAcquisitionClass();

	virtual HRESULT ReadCameraFile( const CString& sFile );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual bool CameraMatchesCameraFile();
	
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw );

	virtual bool StartAcquire(SVCameraPage& p_rDisplay);
	virtual void StopAcquire();
};

#endif

