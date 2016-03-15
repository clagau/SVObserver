// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVMatroxDCamAcquisitionProxy.cpp
// * .File Name       : $Workfile:   SVMatroxDCamAcquisitionProxy.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 12:31:12  $
// ******************************************************************************

#include "stdafx.h"
#include "SVMatroxDCamAcquisitionProxy.h"
#include "SVAcquisitionClass.h"

SVMatroxDCamAcquisitionProxy::SVMatroxDCamAcquisitionProxy()
{
	m_pOwner = NULL;
}

SVMatroxDCamAcquisitionProxy::~SVMatroxDCamAcquisitionProxy()
{
}

void SVMatroxDCamAcquisitionProxy::SetOwner(SVAcquisitionClass* pOwner)
{
	ASSERT( pOwner != NULL );
	m_pOwner = pOwner;
}

HRESULT SVMatroxDCamAcquisitionProxy::WriteCameraRegister( unsigned long ulAddress, unsigned long ulWriteValue )
{
	return m_pOwner->WriteCameraRegister( ulAddress, ulWriteValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return m_pOwner->ReadCameraRegister( ulAddress, rulValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->WriteCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->ReadCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVMatroxDCamAcquisitionProxy::SetDCAMParameter( const SVDeviceParamWrapper& rwParam )
{
	return m_pOwner->SetStandardCameraParameter( rwParam );
}

bool SVMatroxDCamAcquisitionProxy::CameraDeviceParamExists( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.ParameterExists( e );
}

const SVDeviceParamWrapper& SVMatroxDCamAcquisitionProxy::GetCameraDeviceParam( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.Parameter( e );
}

SVDeviceParamWrapper& SVMatroxDCamAcquisitionProxy::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.GetParameter( e );
}

HRESULT SVMatroxDCamAcquisitionProxy::SetCameraDeviceParam( const SVDeviceParamWrapper& rw )
{
	return m_pOwner->m_DeviceParams.SetParameter( rw );
}

bool SVMatroxDCamAcquisitionProxy::IsCameraOnline()
{
	return m_pOwner->IsOnline();
}

