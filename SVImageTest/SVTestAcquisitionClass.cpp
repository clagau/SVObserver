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
#include "CameraLibrary\SVDeviceParams.h"
#include "resource.h"
#include "SVTestAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVCameraPage.h"
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
	unsigned long l_Width = 0;

	m_rSubsystem.m_svDigitizers.GetBufferWidth( m_hDigitizer, &l_Width );

	return l_Width;
}

unsigned long SVTestAcquisitionClass::GetBufferHeight() const
{
	unsigned long l_Height = 0;

	m_rSubsystem.m_svDigitizers.GetBufferHeight( m_hDigitizer, &l_Height );

	return l_Height;
}

int SVTestAcquisitionClass::GetBufferFormat() const
{
	int l_Format = SvDef::SVImageFormatUnknown;

	m_rSubsystem.m_svDigitizers.GetBufferFormat( m_hDigitizer, &l_Format );

	return l_Format;
}

SvTrc::IImagePtr SVTestAcquisitionClass::GetNextBuffer()
{
	return (nullptr != m_pDisplay) ?  m_pDisplay->m_CameraImage.GetNextBuffer() :  SvTrc::IImagePtr();
}

HRESULT SVTestAcquisitionClass::UpdateWithCompletedBuffer(const SvTrc::IImagePtr& rImage, const SvTl::SVTimeStamp StartTick, const SvTl::SVTimeStamp StopTick)
{
	HRESULT l_Status = S_OK;

	if( nullptr != m_pDisplay )
	{
		l_Status = m_pDisplay->UpdateWithCompletedBuffer(rImage);
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

HRESULT SVTestAcquisitionClass::InitializeDevice( const SVDeviceParamCollection& rDeviceParams )
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

HRESULT SVTestAcquisitionClass::IsValidCameraFileParameters( SVDeviceParamCollection& rDeviceParams )
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
	

HRESULT SVTestAcquisitionClass::SetStandardCameraParameter( const SVDeviceParamWrapper& rw )
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
	unsigned long triggerchannel = 0;

	m_pDisplay = &p_rDisplay;

	m_rSubsystem.m_svDigitizers.CreateBuffers( m_hDigitizer, 10 );
	m_rSubsystem.m_svDigitizers.RegisterBufferInterface( m_hDigitizer, this );

	m_pDisplay->m_CameraImage.UpdateDisplayBufferInfo( GetBufferWidth(), GetBufferHeight(), GetBufferFormat() );

	m_bOnline = S_OK == m_rSubsystem.m_svDigitizers.Start( m_hDigitizer );

	return m_bOnline;
}

void SVTestAcquisitionClass::StopAcquire( )
{
	unsigned long triggerchannel = 0;

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

