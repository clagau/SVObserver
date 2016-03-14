//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTestGigeAcquisitionClass.cpp
//* .File Name       : $Workfile:   SVTestGigeAcquisitionClass.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 11:10:46  $
//******************************************************************************

#include "stdafx.h"
#include <comdef.h>
#include "SVTestGigeAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"

SVTestGigeAcquisitionClass::SVTestGigeAcquisitionClass(SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer)
: SVTestAcquisitionClass( p_rSubsystem, p_hDigitizer )
{
	m_gigeCameraProxy.SetOwner( this );
}

SVTestGigeAcquisitionClass::~SVTestGigeAcquisitionClass()
{
}

HRESULT SVTestGigeAcquisitionClass::ReadCameraFile( const CString& sFile )
{
	SVGigeCameraFileInfoStruct info;
	info.sFilename = sFile;
	info.bColorSystem = false;

	m_DeviceParams.Clear();
	HRESULT hr = SVGigeCameraProxy::ReadCameraFileImpl( info, m_CameraFileDeviceParams );

//	if ( hr == SVMSG_INCORRECT_CHECKSUM )
//	{
//	}
	// SEJ - Set GigeFeature Overrides
	if (hr == S_OK)
	{
		hr = SetGigeFeatureOverrides(info.sFeatureOverrides);
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
	// SEJ - send notification to start tracking main camera parameters
	_variant_t dummy;
	HRESULT hr = m_rSubsystem.m_svDigitizers.ParameterSetValue(m_hDigitizer, SVGigeBeginTrackParameters, 0, &dummy);

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

	// SEJ - send notification to end tracking main camera parameters
	_variant_t dummy;
	HRESULT hr = m_rSubsystem.m_svDigitizers.ParameterSetValue(m_hDigitizer, SVGigeEndTrackParameters, 0, &dummy);
	if (hr == S_OK)
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

HRESULT SVTestGigeAcquisitionClass::SetGigeFeatureOverrides(const SVString& featureOverrides)
{
	return m_gigeCameraProxy.SetGigeFeatureOverrides(featureOverrides, m_hDigitizer, &m_rSubsystem.m_svDigitizers);
}

