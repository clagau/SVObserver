// ******************************************************************************
// * COPYRIGHT (c) 2006 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTestAcquisitionClass.cpp
// * .File Name       : $Workfile:   SVTestAcquisitionClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   08 May 2013 14:51:44  $
// ******************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "SVTestAcquisitionClass.h"
#include "SVTestAcquisitionSubsystem.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVCameraPage.h"

SVTestAcquisitionClass::SVTestAcquisitionClass( SVTestAcquisitionSubsystem& p_rSubsystem, unsigned long p_hDigitizer )
: m_rSubsystem( p_rSubsystem ), m_hDigitizer( p_hDigitizer ), m_bOnline( false )
{
}

SVTestAcquisitionClass::~SVTestAcquisitionClass()
{
}

SVClock::SVTimeStamp SVTestAcquisitionClass::GetTimeStamp() const
{
	SVClock::SVTimeStamp l_TimeStamp = SVClock::GetTimeStamp();
	return l_TimeStamp;
}

size_t SVTestAcquisitionClass::GetBufferWidth() const
{
	unsigned long l_Width = 0;

	m_rSubsystem.m_svDigitizers.GetBufferWidth( m_hDigitizer, &l_Width );

	return l_Width;
}

size_t SVTestAcquisitionClass::GetBufferHeight() const
{
	unsigned long l_Height = 0;

	m_rSubsystem.m_svDigitizers.GetBufferHeight( m_hDigitizer, &l_Height );

	return l_Height;
}

int SVTestAcquisitionClass::GetBufferFormat() const
{
	int l_Format = SVImageFormatUnknown;

	m_rSubsystem.m_svDigitizers.GetBufferFormat( m_hDigitizer, &l_Format );

	return l_Format;
}

HRESULT SVTestAcquisitionClass::GetNextBuffer( SVImageBufferInterface& p_rBuffer )
{
	HRESULT l_Status = S_OK;

	if( m_pDisplay != NULL )
	{
		l_Status = m_pDisplay->GetNextBuffer( p_rBuffer );
	}
	else
	{
		l_Status = E_FAIL;

		p_rBuffer.clear();
	}

	return l_Status;
}

HRESULT SVTestAcquisitionClass::UpdateWithCompletedBuffer( const SVImageBufferInterface& p_rBuffer )
{
	HRESULT l_Status = S_OK;

	if( m_pDisplay != NULL )
	{
		l_Status = m_pDisplay->UpdateWithCompletedBuffer( p_rBuffer );
	}
	else
	{
		l_Status = E_FAIL;
	}

	return l_Status;
}

HRESULT SVTestAcquisitionClass::ReadCameraFile( const CString& sFile )
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
		if ( ((const SVDeviceParam*) w) != NULL )
			m_DeviceParams.GetParameter( iter->first ) = w;
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
	
HRESULT SVTestAcquisitionClass::WriteCameraRegister( unsigned long ulAddress, unsigned long ulValue )
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

HRESULT SVTestAcquisitionClass::ReadCameraRegister( unsigned long ulAddress, unsigned long& rulValue )
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

HRESULT SVTestAcquisitionClass::WriteCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT hr = E_NOTIMPL;
	return hr;
}

HRESULT SVTestAcquisitionClass::ReadCameraRegisterBlock( unsigned long ulAddress, unsigned long ulCount, unsigned long* paulValue )
{
	HRESULT hr = E_NOTIMPL;
	return hr;
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
	unsigned long l_ulHandle = 0;

	m_pDisplay = &p_rDisplay;

	m_rSubsystem.m_svDigitizers.CreateBuffers( m_hDigitizer, 10 );
	m_rSubsystem.m_svDigitizers.RegisterBufferInterface( m_hDigitizer, this );

	m_pDisplay->m_CameraImage.UpdateDisplayBufferInfo( GetBufferWidth(), GetBufferHeight(), GetBufferFormat() );

	m_bOnline = m_rSubsystem.m_svDigitizers.Start( m_hDigitizer ) == S_OK;

	return m_bOnline;
}

void SVTestAcquisitionClass::StopAcquire( )
{
	unsigned long l_ulHandle = 0;

	m_bOnline = false;
	
	m_rSubsystem.m_svDigitizers.Stop( m_hDigitizer );

	m_rSubsystem.m_svDigitizers.UnregisterBufferInterface( m_hDigitizer );
	m_rSubsystem.m_svDigitizers.DestroyBuffers( m_hDigitizer );
	
	m_pDisplay = NULL;
}

void SVTestAcquisitionClass::FireSoftwareTrigger()
{
	m_rSubsystem.m_svDigitizers.InternalTrigger(m_hDigitizer);
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVImageTest\SVTestAcquisitionClass.cpp_v  $
 * 
 *    Rev 1.1   08 May 2013 14:51:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Apr 2013 12:40:40   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 11:10:32   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   04 Oct 2012 09:09:26   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  602
 * SCR Title:  Revise the Toolset Parsing and Object Creation Methodology
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Changed from post incrementor to pre incrementor.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jul 2009 15:53:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   15 Sep 2008 14:11:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added whitespace before FireSoftwareTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   15 Sep 2008 14:03:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added FireSoftwareTrigger method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Mar 2008 09:43:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  622
 * SCR Title:  Update SVImageTest Application to Acquire on Multiple Cameras Simultaneously
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   Added functionality for processing multiple camera acquisitions at the same time.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0.1.0   13 Jul 2009 15:40:20   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Update image test program to use the new acquisition methodology for transferring the buffers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Nov 2006 10:43:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  584
 * SCR Title:  Revise the SVImageTest utility program to load 1394 camera files
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
