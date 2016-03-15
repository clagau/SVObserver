// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVDCamAcquisitionProxy.h
// * .File Name       : $Workfile:   SVDCamAcquisitionProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   18 Apr 2013 17:32:58  $
// ******************************************************************************

#ifndef INCL_SVDCAMACQUISITIONPROXY_H
#define INCL_SVDCAMACQUISITIONPROXY_H

#include "SVOMFCLibrary/SVDeviceParamCollection.h"

/////////////////////////////////////////////////////////////////////////////////
// This class decouples the SVAcquistion class from the SVDCamDriver classes
// so that SVImageTest can use the SVDCamDrivers and load the camera files
/////////////////////////////////////////////////////////////////////////////////
class SVDCamAcquisitionProxy
{
public:
	SVDCamAcquisitionProxy() {}
	virtual ~SVDCamAcquisitionProxy() {}

	virtual HRESULT WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue )=0;
	virtual HRESULT ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )=0;
	virtual HRESULT WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )=0;
	virtual HRESULT ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )=0;
	virtual HRESULT SetDCAMParameter( const SVDeviceParamWrapper& rwParam )=0;	// for parameters that are settable through MIL (all standard DCAM params)
	virtual bool CameraDeviceParamExists( SVDeviceParamEnum e )=0;
	virtual const SVDeviceParamWrapper& GetCameraDeviceParam( SVDeviceParamEnum e ) const=0;
	virtual SVDeviceParamWrapper& GetCameraDeviceParamNonConst( SVDeviceParamEnum e )=0;
	virtual HRESULT SetCameraDeviceParam( const SVDeviceParamWrapper& rw )=0;

	virtual bool IsCameraOnline() const=0;
};
#endif

