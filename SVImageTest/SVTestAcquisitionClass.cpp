// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionClass.cpp
// * .File Name       : $Workfile:   SVTestAcquisitionClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   01 Oct 2013 08:26:00  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVTestAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVCameraPage.h"
#include "SVOLibrary/CameraInfo.h"
#pragma endregion Includes

SVTestAcquisitionClass::SVTestAcquisitionClass( SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer )
: m_rSubsystem( p_rSubsystem ), m_hDigitizer( p_hDigitizer ), m_bOnline( false )
{
}

SVTestAcquisitionClass::~SVTestAcquisitionClass()
{
}

unsigned long SVTestAcquisitionClass::GetBufferWidth() const
{
	return m_rSubsystem.m_svDigitizers.GetBufferWidth( m_hDigitizer);
}

unsigned long SVTestAcquisitionClass::GetBufferHeight() const
{
	return m_rSubsystem.m_svDigitizers.GetBufferHeight( m_hDigitizer);
}

int SVTestAcquisitionClass::GetBufferFormat() const
{
	return m_rSubsystem.m_svDigitizers.GetBufferFormat( m_hDigitizer);
}

SvOi::ITRCImagePtr SVTestAcquisitionClass::GetNextBuffer()
{
	return (nullptr != m_pDisplay) ?  m_pDisplay->m_CameraImage.GetNextBuffer() :  SvOi::ITRCImagePtr();
}

HRESULT SVTestAcquisitionClass::UpdateWithCompletedBuffer(CameraInfo&& cameraInfo)
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDisplay )
	{
		l_Status = m_pDisplay->UpdateWithCompletedBuffer(cameraInfo.m_pImage);
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVTestAcquisitionClass::ReadCameraFile( const std::string& )
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

HRESULT SVTestAcquisitionClass::InitializeDevice( const SVDeviceParamCollection&  )
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

HRESULT SVTestAcquisitionClass::GetDeviceParameters( SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;
	rDeviceParams = m_DeviceParams;
	return hr;
}

HRESULT SVTestAcquisitionClass::SetDeviceParameters( const SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	// don't do a straight assignment ( m_DeviceParams = rDeviceParams )
	// only copy those parameters that are specified.
	SVDeviceParamMap::const_iterator iter;
	for (iter = rDeviceParams.mapParameters.begin(); iter != rDeviceParams.mapParameters.end(); ++iter)
	{
		const SVDeviceParamWrapper& w = iter->second;
		if ( nullptr != ((const SVDeviceParam*) w) )
		{
			m_DeviceParams.GetParameter( iter->first ) = w;
		}
	}

	return hr;
}

HRESULT SVTestAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection&  )
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

HRESULT SVTestAcquisitionClass::GetCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
{
	HRESULT hr = S_OK;

	rDeviceParams = m_CameraFileDeviceParams;
	return hr;
}

bool SVTestAcquisitionClass::CameraMatchesCameraFile()
{
	bool bRetVal = true;
	return bRetVal;
}
	

HRESULT SVTestAcquisitionClass::SetStandardCameraParameter(const SVDeviceParamWrapper&)
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

bool SVTestAcquisitionClass::IsOnline()
{
	return m_bOnline;
}

bool SVTestAcquisitionClass::StartAcquire( SVCameraPage& p_rDisplay )
{
	m_pDisplay = &p_rDisplay;

	m_rSubsystem.m_svDigitizers.CreateBuffers( m_hDigitizer );
	m_rSubsystem.m_svDigitizers.RegisterBufferInterface( m_hDigitizer, this );

	m_pDisplay->m_CameraImage.UpdateDisplayBufferInfo( GetBufferWidth(), GetBufferHeight(), GetBufferFormat() );

	m_bOnline = S_OK == m_rSubsystem.m_svDigitizers.Start( m_hDigitizer );

	return m_bOnline;
}

void SVTestAcquisitionClass::StopAcquire( )
{
	m_bOnline = false;
	
	m_rSubsystem.m_svDigitizers.Stop( m_hDigitizer );

	m_rSubsystem.m_svDigitizers.UnregisterBufferInterface( m_hDigitizer );
	m_rSubsystem.m_svDigitizers.DestroyBuffers( m_hDigitizer );
	
	m_pDisplay = nullptr;
}

void SVTestAcquisitionClass::FireSoftwareTrigger()
{
	m_rSubsystem.m_svDigitizers.InternalTrigger(m_hDigitizer);
}

