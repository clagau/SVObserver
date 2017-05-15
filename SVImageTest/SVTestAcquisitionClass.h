// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionClass.h
// * .File Name       : $Workfile:   SVTestAcquisitionClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   01 Oct 2013 08:25:48  $
// ******************************************************************************

#pragma once


#include "CameraLibrary/SVDeviceParamCollection.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/SVString.h"

class SVCameraPage;
class SVTestAcquisitionSubsystem;

class SVTestAcquisitionClass : public SVAcquisitionBufferInterface
{

public:
	SVTestAcquisitionSubsystem& m_rSubsystem;
	SVDeviceParamCollection m_DeviceParams;
	SVDeviceParamCollection m_CameraFileDeviceParams;
	SVCameraPage* m_pDisplay;

	SVTestAcquisitionClass( SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer );
	virtual ~SVTestAcquisitionClass();

	virtual SvTl::SVTimeStamp GetTimeStamp() const override;
	
	virtual unsigned long GetBufferWidth() const override;
	virtual unsigned long GetBufferHeight() const override;
	virtual int GetBufferFormat() const override;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer ) override;
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer ) override;

	virtual HRESULT ReadCameraFile( const SVString& rFilename );

	HRESULT GetDeviceParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	HRESULT GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams );

	virtual bool CameraMatchesCameraFile();
	
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw );

	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rParams );
	
	bool IsOnline();
	virtual bool StartAcquire(SVCameraPage& p_rDisplay);
	virtual void StopAcquire();
	
	void FireSoftwareTrigger();

protected:
	unsigned long m_hDigitizer;
	bool m_bOnline;
};

