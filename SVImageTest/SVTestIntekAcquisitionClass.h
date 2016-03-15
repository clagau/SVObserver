// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekAcquisitionClass.h
// * .File Name       : $Workfile:   SVTestIntekAcquisitionClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:13:18  $
// ******************************************************************************

#ifndef INCL_SVTESTINTEKACQUISITIONCLASS_H
#define INCL_SVTESTINTEKACQUISITIONCLASS_H

#include "SVTestAcquisitionClass.h"
#include "SVTestIntekDCamDriverProxy.h"
#include "SVTestIntekDCamAcquisitionProxy.h"

class SVTestIntekAcquisitionClass : public SVTestAcquisitionClass
{
private:
	SVTestIntekDCamDriverProxy m_dcamDriverProxy;
	SVTestIntekDCamAcquisitionProxy m_dcamAcquisitionProxy;
	SVDCamDriver* m_pCameraDriver;

public:
	SVTestIntekAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer);
	virtual ~SVTestIntekAcquisitionClass();

	virtual HRESULT ReadCameraFile( const CString& sFile );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual bool CameraMatchesCameraFile();
	
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw );

	virtual bool StartAcquire( SVCameraPage& p_rDisplay );
	virtual void StopAcquire();

private:
	void DestroyDCamDriver();
};

#endif

