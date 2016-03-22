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

#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVOMFCLibrary/SVDeviceParamCollection.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVTimerLibrary/SVClock.h"

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

	virtual SVClock::SVTimeStamp GetTimeStamp() const;
	
	virtual unsigned long GetBufferWidth() const;
	virtual unsigned long GetBufferHeight() const;
	virtual int GetBufferFormat() const;

	virtual HRESULT GetNextBuffer( SVImageBufferInterface& p_rBuffer );
	virtual HRESULT UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer );

	virtual HRESULT ReadCameraFile( const CString& sFile );

	virtual HRESULT GetDeviceParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams );

	virtual HRESULT IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams );
	virtual HRESULT GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams );

	virtual bool CameraMatchesCameraFile();
	
	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetStandardCameraParameter( const SVDeviceParamWrapper& rw );

	virtual HRESULT InitializeDevice( const SVDeviceParamCollection& rParams );
	
	virtual bool IsOnline();
	virtual bool StartAcquire(SVCameraPage& p_rDisplay);
	virtual void StopAcquire();
	
	void FireSoftwareTrigger();

protected:
	unsigned long m_hDigitizer;
	bool m_bOnline;
};

