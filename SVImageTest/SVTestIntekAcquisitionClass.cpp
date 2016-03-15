// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestIntekAcquisitionClass.cpp
// * .File Name       : $Workfile:   SVTestIntekAcquisitionClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   22 Apr 2013 11:13:36  $
// ******************************************************************************

#include "stdafx.h"
#include "SVTestIntekAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SV1394CameraFileLibrary/SVDCamFactoryRegistrar.h"
#include "SVIntek/SVIntekEnums.h"

SVTestIntekAcquisitionClass::SVTestIntekAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer)
: SVTestAcquisitionClass( p_rSubsystem, p_hDigitizer )
{
	m_pCameraDriver = NULL;
	m_dcamDriverProxy.SetOwner( this );
	m_dcamAcquisitionProxy.SetOwner( this );
}

SVTestIntekAcquisitionClass::~SVTestIntekAcquisitionClass()
{
	DestroyDCamDriver();
}

HRESULT SVTestIntekAcquisitionClass::ReadCameraFile( const CString& sFile )
{
	SV1394CameraFileInfoStruct info;
	info.sFilename = sFile;
	info.bColorSystem = false;

	m_DeviceParams.Clear();
	HRESULT hr = SVDCamDriverProxy::ReadCameraFileImpl( info, m_CameraFileDeviceParams );

	DestroyDCamDriver();
	
	try
	{
		m_pCameraDriver = TheDCamDriverFactory::Instance().New( info.sCameraType );
	}
	catch ( std::runtime_error& )
	{
	}

	ASSERT( m_pCameraDriver );
	if ( m_pCameraDriver )
	{
		m_pCameraDriver->SetOwner( &m_dcamAcquisitionProxy );
		m_dcamDriverProxy.Init( m_pCameraDriver );
	}

	hr = SetDeviceParameters( m_CameraFileDeviceParams );
	return hr;
}


HRESULT SVTestIntekAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = m_dcamDriverProxy.IsValidCameraFileParameters(rDeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

bool SVTestIntekAcquisitionClass::CameraMatchesCameraFile()
{
	bool bRetVal = m_dcamDriverProxy.CameraMatchesCameraFile(m_CameraFileDeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return bRetVal;
}

HRESULT SVTestIntekAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// SEJ - send notification to start tracking main camera parameters
	_variant_t dummy;
	hr = m_rSubsystem.m_svDigitizers.ParameterSetValue(m_hDigitizer, SVIntekBeginTrackParameters, 0, &dummy);
	
	if ( hr == S_OK )
	{
		hr = SVTestAcquisitionClass::SetDeviceParameters( rDeviceParams );
	}

	if ( hr == S_OK )
	{
		hr = InitializeDevice( rDeviceParams );
	}
	return hr;
}

HRESULT SVTestIntekAcquisitionClass::InitializeDevice( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = m_dcamDriverProxy.InitializeDevice(rDeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

HRESULT SVTestIntekAcquisitionClass::WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue )
{
	HRESULT hr = m_dcamDriverProxy.WriteCameraRegister(ulAddress, ulValue, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

HRESULT SVTestIntekAcquisitionClass::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	HRESULT hr = m_dcamDriverProxy.ReadCameraRegister(ulAddress, rulValue, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

HRESULT SVTestIntekAcquisitionClass::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT hr = m_dcamDriverProxy.WriteCameraRegisterBlock(ulAddress, ulCount, paulValue, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

HRESULT SVTestIntekAcquisitionClass::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT hr = m_dcamDriverProxy.ReadCameraRegisterBlock(ulAddress, ulCount, paulValue, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

HRESULT SVTestIntekAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = m_dcamDriverProxy.SetStandardCameraParameter(rw, m_DeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

bool SVTestIntekAcquisitionClass::StartAcquire(SVCameraPage& p_rDisplay)
{
	if (m_pCameraDriver)
	{
		m_bOnline = true;
		
		/// Send Strobe & trigger
		// SEJ - send notification to end tracking main camera parameters
		_variant_t dummy;
		HRESULT hr = m_rSubsystem.m_svDigitizers.ParameterSetValue(m_hDigitizer, SVIntekEndTrackParameters, 0, &dummy);

		m_pCameraDriver->GoOnline();
	}
	return SVTestAcquisitionClass::StartAcquire(p_rDisplay);
}

void SVTestIntekAcquisitionClass::StopAcquire()
{
	SVTestAcquisitionClass::StopAcquire();

	if (m_pCameraDriver)
	{
		// Stop Trigger & strobe
		m_pCameraDriver->GoOffline();
	}
}

void SVTestIntekAcquisitionClass::DestroyDCamDriver()
{
	if ( m_pCameraDriver )
	{
		delete m_pCameraDriver;
		m_pCameraDriver = NULL;
	}
}

