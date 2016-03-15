// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekDCamAcquisitionProxy.h
// * .File Name       : $Workfile:   SVIntekDCamAcquisitionProxy.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:05:32  $
// ******************************************************************************

#ifndef INCL_SVINTEKDCAMACQUISITIONPROXY_H
#define INCL_SVINTEKDCAMACQUISITIONPROXY_H

#include "SV1394CameraFileLibrary/SVDCamAcquisitionProxy.h"
#include "SVAcquisitionClass.h"

class SVIntekDCamAcquisitionProxy : public SVDCamAcquisitionProxy
{
private:
	SVAcquisitionClassPtr m_pOwner;

public:
	SVIntekDCamAcquisitionProxy();
	virtual ~SVIntekDCamAcquisitionProxy();
	void SetOwner( LPCTSTR p_szOwnerName );

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

#endif

