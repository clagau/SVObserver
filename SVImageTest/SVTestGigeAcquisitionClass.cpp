//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeAcquisitionClass.cpp
//* .File Name       : $Workfile:   SVTestGigeAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:10:46  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTestGigeAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#include "Definitions/SVGigeEnums.h"
#include "SVGigeCameraFileLibrary/SVGigeCameraFileReader.h"
#pragma endregion Includes

SVTestGigeAcquisitionClass::SVTestGigeAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer)
: SVTestAcquisitionClass( p_rSubsystem, p_hDigitizer )
{
	m_gigeCameraProxy.SetOwner( this );
}

SVTestGigeAcquisitionClass::~SVTestGigeAcquisitionClass()
{
}

HRESULT SVTestGigeAcquisitionClass::ReadCameraFile( const std::string& rFilename )
{

	SVGigeCameraFileReader reader(rFilename, false);
	HRESULT hr = reader.ReadCameraFileImpl( m_CameraFileDeviceParams );

	// Set GigeFeature Overrides
	if (S_OK == hr)
	{
		hr = SetGigeFeatureOverrides(reader.GetFeatureOverrides());
	}
	 
	hr = SetDeviceParameters( m_CameraFileDeviceParams );
	return hr;
}

HRESULT SVTestGigeAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = m_gigeCameraProxy.IsValidCameraFileParameters(rDeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

bool SVTestGigeAcquisitionClass::CameraMatchesCameraFile()
{
	bool bRetVal = m_gigeCameraProxy.CameraMatchesCameraFile(m_CameraFileDeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return bRetVal;
}

HRESULT SVTestGigeAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	// Send notification to start tracking main camera parameters
	_variant_t dummy;
	HRESULT hr = m_rSubsystem.m_svDigitizers.ParameterSetValue(m_hDigitizer, SvDef::SVGigeBeginTrackParameters, dummy);

	if ( S_OK == hr )
	{
		hr = SVTestAcquisitionClass::SetDeviceParameters( rDeviceParams );
	}
	if ( S_OK == hr )
	{
		hr = InitializeDevice( rDeviceParams );
	}
	return hr;
}

HRESULT SVTestGigeAcquisitionClass::InitializeDevice( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = m_gigeCameraProxy.InitializeDevice(rDeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

HRESULT SVTestGigeAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rw )
{
	HRESULT hr = m_gigeCameraProxy.SetStandardCameraParameter(rw, m_DeviceParams, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	return hr;
}

bool SVTestGigeAcquisitionClass::StartAcquire(SVCameraPage& p_rDisplay)
{
	m_bOnline = true;

	// Send notification to end tracking main camera parameters
	_variant_t dummy;
	HRESULT hr = m_rSubsystem.m_svDigitizers.ParameterSetValue(m_hDigitizer, SvDef::SVGigeEndTrackParameters, dummy);
	if (S_OK == hr)
	{
		// Send Strobe & trigger
		hr = m_gigeCameraProxy.GoOnline(m_hDigitizer, &m_rSubsystem.m_svDigitizers);
	}
	return SVTestAcquisitionClass::StartAcquire(p_rDisplay);
}

void SVTestGigeAcquisitionClass::StopAcquire()
{
	SVTestAcquisitionClass::StopAcquire();

	// Stop Trigger & strobe
	m_gigeCameraProxy.GoOffline(m_hDigitizer, &m_rSubsystem.m_svDigitizers);
}

HRESULT SVTestGigeAcquisitionClass::SetGigeFeatureOverrides(const std::string& rFeatureOverrides)
{
	return m_gigeCameraProxy.SetGigeFeatureOverrides(rFeatureOverrides, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
}

