//******************************************************************************
//* COPYRIGHT (c) 2003 by SVResearch, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVDCamDriver
//* .File Name       : $Workfile:   SVDcamDriver.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   18 Apr 2013 17:36:36  $
//******************************************************************************

#include "stdafx.h"
#include "SVDCAMAcquisitionProxy.h"
#include "SVDCamDriver.h"


SVDCamDriver::SVDCamDriver()
: m_pOwner( NULL )
{
}

SVDCamDriver::~SVDCamDriver()
{
	m_pOwner = NULL;
}

HRESULT SVDCamDriver::SetOwner( SVDCamAcquisitionProxy* pOwner )
{
	ASSERT( pOwner != NULL );
	m_pOwner = pOwner;
	return S_OK;
}

HRESULT SVDCamDriver::WriteCameraRegister( unsigned long ulAddress, unsigned long ulWriteValue )
{
	HRESULT hr = S_OK;

	#ifdef _DEBUG
		HRESULT hrRegister;
		unsigned long ulPreValue = -1;
		hrRegister = ReadCameraRegister( ulAddress, ulPreValue );
	#endif
	
	hr = m_pOwner->WriteCameraRegister( ulAddress, ulWriteValue );

	#ifdef _DEBUG
		unsigned long ulPostValue = -1;
		hrRegister = ReadCameraRegister( ulAddress, ulPostValue );
		hrRegister+=0;
	#endif

	return hr;
}

HRESULT SVDCamDriver::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	return m_pOwner->ReadCameraRegister( ulAddress, rulValue );
}

HRESULT SVDCamDriver::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->WriteCameraRegisterBlock( ulAddress, ulCount, paulValue );
}

HRESULT SVDCamDriver::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	return m_pOwner->ReadCameraRegisterBlock( ulAddress, ulCount, paulValue );
}


HRESULT SVDCamDriver::SetDCAMParameter( const SVDeviceParamWrapper& rwParam )	// for parameters that are settable through MIL (all standard DCAM params)
{
	return m_pOwner->SetDCAMParameter( rwParam );
}

bool SVDCamDriver::CameraDeviceParamExists( SVDeviceParamEnum e )
{
	return m_pOwner->CameraDeviceParamExists( e );
}

const SVDeviceParamWrapper& SVDCamDriver::GetCameraDeviceParam( SVDeviceParamEnum e ) const
{
	return m_pOwner->GetCameraDeviceParam( e );
}

SVDeviceParamWrapper& SVDCamDriver::GetCameraDeviceParamNonConst( SVDeviceParamEnum e )
{
	return m_pOwner->GetCameraDeviceParamNonConst( e );
}

HRESULT SVDCamDriver::SetCameraDeviceParam( const SVDeviceParamWrapper& rw )
{
	return m_pOwner->SetCameraDeviceParam( rw );
}

bool SVDCamDriver::IsCameraOnline() const
{
	return m_pOwner->IsCameraOnline();
}

