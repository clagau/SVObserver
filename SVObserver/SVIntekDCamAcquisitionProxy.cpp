// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekDcamAcquisitionProxy.cpp
// * .File Name       : $Workfile:   SVIntekDCamAcquisitionProxy.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   23 Apr 2013 11:05:22  $
// ******************************************************************************

#include "stdafx.h"
#include "SVIntekDCamAcquisitionProxy.h"
#include "SVAcquisitionClass.h"
#include "SVDigitizerProcessingClass.h"

SVIntekDCamAcquisitionProxy::SVIntekDCamAcquisitionProxy()
{
	m_pOwner = NULL;
}

SVIntekDCamAcquisitionProxy::~SVIntekDCamAcquisitionProxy()
{
}

void SVIntekDCamAcquisitionProxy::SetOwner( LPCTSTR p_szOwnerName )
{
	m_pOwner = SVDigitizerProcessingClass::Instance().GetAcquisitionDevice( p_szOwnerName );
}

HRESULT SVIntekDCamAcquisitionProxy::WriteCameraRegister( unsigned long ulAddress, unsigned long ulWriteValue )
{
	return m_pOwner->WriteCameraRegister( ulAddress, ulWriteValue );
}

HRESULT SVIntekDCamAcquisitionProxy::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return m_pOwner->ReadCameraRegister( ulAddress, rulValue );
}

HRESULT SVIntekDCamAcquisitionProxy::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->WriteCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVIntekDCamAcquisitionProxy::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->ReadCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVIntekDCamAcquisitionProxy::SetDCAMParameter( const SVDeviceParamWrapper& rwParam )
{
	return m_pOwner->SetStandardCameraParameter( rwParam );
}

bool SVIntekDCamAcquisitionProxy::CameraDeviceParamExists( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.ParameterExists( e );
}

const SVDeviceParamWrapper& SVIntekDCamAcquisitionProxy::GetCameraDeviceParam( SVDeviceParamEnum e ) const
{
	return m_pOwner->m_DeviceParams.Parameter( e );
}

SVDeviceParamWrapper& SVIntekDCamAcquisitionProxy::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pOwner->m_DeviceParams.GetParameter( e );
}

HRESULT SVIntekDCamAcquisitionProxy::SetCameraDeviceParam( const SVDeviceParamWrapper& rw )
{
	return m_pOwner->m_DeviceParams.SetParameter( rw );
}

bool SVIntekDCamAcquisitionProxy::IsCameraOnline() const
{
	return m_pOwner->IsOnline();
}

