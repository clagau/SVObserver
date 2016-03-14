//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDriver
//* .File Name       : $Workfile:   SVDcamDriver.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:38  $
//******************************************************************************

#ifndef SVDCAMDRIVER_H
#define SVDCAMDRIVER_H

#include "SVOMFCLibrary/SVDeviceParamCollection.h"

class SVDCamAcquisitionProxy;

enum SVDCamTriggerSourceEnum
{
	UnknownTrigger = 0,
	HardwareTrigger = 1,
	SoftwareAsyncTrigger = 2,
	SoftwareGrabTrigger = 3,
	OneShotTrigger = 4
};

class SVDCamDriver
{
public:
	SVDCamDriver();
	virtual ~SVDCamDriver();
	HRESULT SetOwner( SVDCamAcquisitionProxy* pOwner );

	virtual HRESULT SetParameters( const SVDeviceParamCollection& rDeviceParams )=0;
	virtual HRESULT SetParameter( const SVDeviceParamWrapper& rw )=0;
	virtual HRESULT SoftwareTrigger()=0;
	virtual bool CanSoftwareTrigger()=0;
	virtual HRESULT GetSoftwareTriggerRegisterAndValue(unsigned long& ulRegister, unsigned long& ulValue)=0;
	virtual HRESULT GoOnline()=0;
	virtual HRESULT GoOffline()=0;
	virtual CString GetFirmwareVersion()=0;
	virtual unsigned long GetTriggerEdge() const = 0;   // 0=Falling Edge; 1=Rising Edge

protected:
	virtual unsigned long GetDCAMTriggerPolarity() const = 0;   // 0=Active Low(Rising Edge); 1=Active High(Falling Edge)
	virtual unsigned long GetDCAMTriggerOnOff() const = 0;
	virtual unsigned long GetDCAMTriggerMode() const = 0;

	// these functions call into MatroxAcquisitionClass to access standard MIL services
	HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	HRESULT SetDCAMParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)
	bool CameraDeviceParamExists( SVDeviceParamEnum e );
	const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const;
	SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );
	HRESULT SetCameraDeviceParam( const SVDeviceParamWrapper& rw );

	bool IsCameraOnline() const;

private:
	SVDCamAcquisitionProxy* m_pOwner;

};

#endif

