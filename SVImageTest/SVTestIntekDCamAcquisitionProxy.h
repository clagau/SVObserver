// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekDCamAcquisitionProxy.h
// * .File Name       : $Workfile:   SVTestIntekDCamAcquisitionProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:15:28  $
// ******************************************************************************

#pragma once

#include "SV1394CameraFileLibrary/SVDCamAcquisitionProxy.h"

class SVTestAcquisitionClass;

class SVTestIntekDCamAcquisitionProxy : public SVDCamAcquisitionProxy
{
private:
	SVTestAcquisitionClass* m_pOwner;

public:
	SVTestIntekDCamAcquisitionProxy();
	virtual ~SVTestIntekDCamAcquisitionProxy();
	void SetOwner(SVTestAcquisitionClass * pOwner);

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue );
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue );
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue );
	virtual HRESULT SetDCAMParameter( const SVDeviceParamWrapper& rwParam );	// for parameters that are settable through MIL (all standard DCAM params)
	virtual bool CameraDeviceParamExists( SVDeviceParamEnum e );
	virtual const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const;
	virtual SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e );
	virtual HRESULT SetCameraDeviceParam( const SVDeviceParamWrapper& rw );

	virtual bool IsCameraOnline() const;
};

