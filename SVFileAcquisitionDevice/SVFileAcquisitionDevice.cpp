//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDevcieExports
//* .File Name       : $Workfile:   SVFileAcquisitionDevice.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.3  $
//* .Check In Date   : $Date:   01 Dec 2014 12:16:00  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "CameraLibrary\SVDeviceParams.h"
#include "SVFileAcquisitionDevice.h"
#include "SVFileAcquisitionDeviceParamEnum.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVImageLibrary/SVImageBufferInterface.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "TriggerInformation/SVTriggerActivatorFunc.h"
#include "TriggerInformation/SVTriggerCallbackFunc.h"
#include "TriggerInformation/SVTriggerEnums.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

static const long MaxFileCameras = 12;

SVFileAcquisitionDevice::SVFileAcquisitionDevice()
: m_lRefCount(0)
{
}

SVFileAcquisitionDevice::~SVFileAcquisitionDevice()
{
	Close();
}
// Handle is just the original
static unsigned long GetDigitizerHandle(unsigned long p_ulIndex)
{
	return p_ulIndex;
}

bool SVFileAcquisitionDevice::IsValidDigitizer(unsigned long triggerchannel) const
{
	return ( triggerchannel < m_cameras.size() );
}

SVFileCamera& SVFileAcquisitionDevice::GetDigitizer(unsigned long triggerchannel)
{
	return m_cameras[ triggerchannel ];
}

const SVFileCamera& SVFileAcquisitionDevice::GetDigitizer(unsigned long triggerchannel) const
{
	return m_cameras[ triggerchannel ];
}

HRESULT SVFileAcquisitionDevice::Create()
{
	HRESULT l_hrOk = S_OK;

	if ( ::InterlockedIncrement( &m_lRefCount ) == 1 )
	{
		// Allocate Max File Cameras ?
		m_cameras.resize(MaxFileCameras);
		for (int i = 0;i < MaxFileCameras;i++)
		{
			std::string camName = SvUl::Format("File.Dig_%d", i);
			m_cameras[i].SetName(camName);
		}
		// Allocate Max Buffers ?
	}
	m_triggerMgr.Create(_T("File AcquisitionDevice Manager"), SVAffinityAcq);

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::Destroy( bool p_bClose )
{
	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &m_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &m_lRefCount, 0 );
	}

	m_triggerMgr.Destroy();

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::Open()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT SVFileAcquisitionDevice::Close()
{
	HRESULT l_hrOk = S_FALSE;

	l_hrOk = Destroy( true );

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraCreate( SVFileCameraStruct *p_pBoard, SVFileCamera *p_pCamera )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionDevice::CameraDestroy( SVFileCamera *p_pCamera )
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionDevice::CameraGetCount( unsigned long &p_rulCount )
{
	HRESULT l_hrOk = S_OK;

	p_rulCount = static_cast<unsigned long>(m_cameras.size());

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetName( unsigned long p_ulIndex, BSTR &p_rbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < m_cameras.size() )
	{
		l_hrOk = S_OK;

		if ( nullptr != p_rbstrName )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = nullptr;
		}

		p_rbstrName = _bstr_t(m_cameras[p_ulIndex].GetName().c_str()).Detach();
	} 
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetHeight( unsigned long p_ulIndex, unsigned long &p_rulHeight )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < m_cameras.size() )
	{
		p_rulHeight = m_cameras[p_ulIndex].GetImageSize().cy;

		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetWidth( unsigned long p_ulIndex, unsigned long &p_rulWidth )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < m_cameras.size() )
	{
		p_rulWidth = m_cameras[p_ulIndex].GetImageSize().cx;

		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetFormat( unsigned long p_ulIndex, int &p_riFormat )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < m_cameras.size() )
	{
		switch( m_cameras[p_ulIndex].GetImageFormat() )
		{
			case SVImageFormatMono16:
			case SVImageFormatRGB888:
			case SVImageFormatRGB8888:
			case SVImageFormatRGB161616:
			case SVImageFormatY411:
			case SVImageFormatY422:
			case SVImageFormatY444:
			{
				p_riFormat = SVImageFormatRGB8888;

				break;
			}
			default:
			{
				p_riFormat = m_cameras[p_ulIndex].GetImageFormat();

				break;
			}
		}

		l_hrOk = S_OK;
	} 
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetParameterList( unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		::VariantClear( p_pvarValue );

		if( p_ulIndex < m_cameras.size() )
		{
			SAFEARRAYBOUND l_psabData[1];
			long l_plIndex[1];

			l_psabData[0].cElements = SVFileAcquisitionParameterSize;
			l_psabData[0].lLbound = 0;

			p_pvarValue->vt = VT_ARRAY | VT_I4;
			p_pvarValue->parray = ::SafeArrayCreate( VT_I4, 1, l_psabData );

			for( int i = 0; i < SVFileAcquisitionParameterSize; i++ )
			{
				//Parameters
				l_plIndex[0] = i;
				::SafeArrayPutElement( p_pvarValue->parray, l_plIndex, (void *)(l_plIndex) );
			}

			l_hrOk = S_OK;
		}
	} 
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetParameterName( unsigned long p_ulIndex, int p_iParameterID, BSTR &p_rbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < m_cameras.size() )
	{
		l_hrOk = S_OK;

		if ( nullptr != p_rbstrName )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = nullptr;
		}

		switch( p_iParameterID )
		{
			case SVFileAcquisitionParameterImageHeight:
			{
				p_rbstrName = ::SysAllocString( L"Image Height" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVFileAcquisitionParameterImageWidth:
			{
				p_rbstrName = ::SysAllocString( L"Image Width" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVFileAcquisitionParameterImageFormat:
			{
				p_rbstrName = ::SysAllocString( L"Image Format" );
				
				l_hrOk = S_OK;

				break;
			}
			
			case SVFileAcquisitionParameterFilename:
			{
				p_rbstrName = ::SysAllocString( L"Filename" );
				
				l_hrOk = S_OK;

				break;
			}
			case SVFileAcquisitionParameterDirectory:
			{
				p_rbstrName = ::SysAllocString( L"Directory" );

				l_hrOk = S_OK;

				break;
			}

			case SVFileAcquisitionParameterLoadingMode:
			{
				p_rbstrName = ::SysAllocString( L"File Loading Mode" );

				l_hrOk = S_OK;
				break;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetParameter( unsigned long p_ulIndex, int p_iParameterID, int *p_piParameterIDType, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		if( p_ulIndex < m_cameras.size() )
		{
			SVFileCamera& rCamera = m_cameras[ p_ulIndex ];

			switch( p_iParameterID )
			{
				case SVFileAcquisitionParameterImageFormat:
				{
					::VariantClear( p_pvarValue );

					p_pvarValue->vt = VT_I4;
					p_pvarValue->ulVal = rCamera.GetImageFormat();
					
					l_hrOk = S_OK;

					break;
				}
				case SVFileAcquisitionParameterImageWidth:
				{
					::VariantClear( p_pvarValue );

					p_pvarValue->vt = VT_I4;
					p_pvarValue->ulVal = rCamera.GetImageSize().cx;
					l_hrOk = S_OK;
					break;
				}

				case SVFileAcquisitionParameterImageHeight:
				{
					::VariantClear( p_pvarValue );

					p_pvarValue->vt = VT_I4;
					p_pvarValue->ulVal = rCamera.GetImageSize().cy;
					l_hrOk = S_OK;
					break;
				}

				case SVFileAcquisitionParameterFilename:
				{
					_bstr_t l_Name = rCamera.GetFileName();

					p_pvarValue->vt = VT_BSTR;
					p_pvarValue->bstrVal = l_Name.Detach();
					l_hrOk = S_OK;
					break;
				}

				case SVFileAcquisitionParameterDirectory:
				{
					_bstr_t l_Name = rCamera.GetDirectory();

					p_pvarValue->vt = VT_BSTR;
					p_pvarValue->bstrVal = l_Name.Detach();
					l_hrOk = S_OK;
					break;
				}

				case SVFileAcquisitionParameterLoadingMode:
				{
					p_pvarValue->vt = VT_I4;
					p_pvarValue->lVal = rCamera.GetLoadingMode(); 
					l_hrOk = S_OK;
					break;
				}

				default:
				{
					break;
				}
			}
		}
	} 
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraSetParameter( unsigned long p_ulIndex, int p_iParameterID, int p_iParameterIDType, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if( p_ulIndex < m_cameras.size() )
	{
		SVFileCamera& rCamera = m_cameras[ p_ulIndex ];

		switch( p_iParameterID )
		{
			case SVFileAcquisitionParameterImageFormat:
			{
				rCamera.SetImageFormat(static_cast<SVImageFormatEnum>(p_pvarValue->ulVal));
				l_hrOk = S_OK;
				break;
			}

			case SVFileAcquisitionParameterImageWidth:
			{
				SIZE size = rCamera.GetImageSize();
				size.cx = p_pvarValue->ulVal;
				rCamera.SetImageSize(size);
				l_hrOk = S_OK;
				break;
			}

			case SVFileAcquisitionParameterImageHeight:
			{
				SIZE size = rCamera.GetImageSize();
				size.cy = p_pvarValue->ulVal;
				rCamera.SetImageSize(size);
				l_hrOk = S_OK;
				break;
			}

			case SVFileAcquisitionParameterFilename:
			{
				rCamera.SetFileName(SvUl::createStdString(p_pvarValue->bstrVal));
				l_hrOk = S_OK;
				break;
			}

			case SVFileAcquisitionParameterDirectory:
			{
				rCamera.SetDirectory(SvUl::createStdString(p_pvarValue->bstrVal));
				l_hrOk = S_OK;
				break;
			}

			case SVFileAcquisitionParameterLoadingMode:
			{
				SVFileAcquisitonLoadingModeEnum value = static_cast<SVFileAcquisitonLoadingModeEnum>(p_pvarValue->lVal);
				if (value == SingleFileMode || value == ContinuousMode || value == SingleIterationMode)
				{
					rCamera.SetLoadingMode(value);
					l_hrOk = S_OK;
				}
				break;
			}

			default:
			{
				break;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount )
{
	HRESULT l_hrOk = S_OK;

	if ( m_cameras.size() <= p_ulIndex )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraBufferDestroyAll( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( m_cameras.size() <= p_ulIndex )
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < m_cameras.size() )
	{
		m_cameras[p_ulIndex].m_pBufferInterface = p_pInterface;
	} 
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraUnregisterBufferInterface( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < m_cameras.size() )
	{
		m_cameras[p_ulIndex].m_pBufferInterface = nullptr;
	} 
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraStart( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_ulIndex < m_cameras.size() )
	{
		SVFileCamera& rCamera = m_cameras[p_ulIndex];
		if ( ::InterlockedExchange( &(rCamera.m_lIsStarted), 1 ) == 0 )
		{
			// Check if already Running
			if( ! rCamera.IsRunning() )
			{
				SVFileCamera::EventHandler startHandler = boost::bind(&SVFileAcquisitionDevice::CameraProcessStartFrame, this, _1);
				SVFileCamera::EventHandler endHandler = boost::bind(&SVFileAcquisitionDevice::CameraProcessEndFrame, this, _1);

				l_hrOk = rCamera.Start( startHandler, endHandler, p_ulIndex);
			}
			
			if( S_OK != l_hrOk )
			{
				::InterlockedExchange( &(rCamera.m_lIsStarted), 0 );
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraStop( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < m_cameras.size() )
	{
		SVFileCamera& rCamera = m_cameras[p_ulIndex];
		if ( ::InterlockedExchange( &(rCamera.m_lIsStarted), 0 ) == 1 )
		{
			// Stop File Loader Thread
			rCamera.Stop();
		}
	} 
	return l_hrOk;
}

// this is called at the start of the acquisition
// in the case of file acquisition, it's called from the loader apc callback
// which is activated via the DoOneShot signaller
HRESULT SVFileAcquisitionDevice::CameraProcessStartFrame( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < m_cameras.size() )
	{
		SVFileCamera& rCamera = m_cameras[ p_ulIndex ];

		if ( 1 == rCamera.m_lIsStarted && nullptr != rCamera.m_pBufferInterface )
		{
			rCamera.m_StartTimeStamp = rCamera.m_pBufferInterface->GetTimeStamp();
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}
	return l_hrOk;
}

// call this when Loader finishes ?
HRESULT SVFileAcquisitionDevice::CameraProcessEndFrame( unsigned long p_ulIndex )
{
	HRESULT l_hrOk = S_OK;

	if ( p_ulIndex < m_cameras.size() )
	{
		SVFileCamera& rCamera = m_cameras[p_ulIndex];
		
		char l_szbuf[128];

		if ( rCamera.m_lIsStarted && nullptr != rCamera.m_pBufferInterface )
		{
			if (rCamera.IsAcquisitionTriggered())
			{
				DoAcquisitionTrigger(rCamera);
			}
			SVImageBufferInterface l_Buffer;
			
			if( S_OK == rCamera.m_pBufferInterface->GetNextBuffer( l_Buffer ) )
			{
				l_Buffer.SetStartFrameTimeStamp( rCamera.m_StartTimeStamp );
				l_Buffer.SetEndFrameTimeStamp( rCamera.m_pBufferInterface->GetTimeStamp() );

				unsigned char *l_pBuffer = l_Buffer.GetBufferAddress();

				if( nullptr != l_pBuffer )
				{
					l_hrOk = rCamera.CopyImage( l_pBuffer );

					if( S_OK == l_hrOk )
					{
						rCamera.m_pBufferInterface->UpdateWithCompletedBuffer( l_Buffer );
					}
					else
					{
						wsprintf(l_szbuf, "FileAcquisition::CopyImage - Error in Format");
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
						TRACE( "%s\n", l_szbuf );
#endif

						SvStl::MessageMgrStd Exception( SvStl::LogOnly );
						Exception.setMessage( SVMSG_IMAGE_FORMAT_ERROR, SvStl::Tid_FileAcquisition_FormatError, SvStl::SourceFileParams(StdMessageParams) );
					}
				}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				else
				{
					wsprintf(l_szbuf,"Error In BufferGetAddress" );
					TRACE( "%s\n", l_szbuf );
				}
#endif
			}
		}
#if defined (TRACE_THEM_ALL) || defined (TRACE_ACQDEVICE)
		else
		{
			wsprintf(l_szbuf,"IsStarted = %s", (rCamera.m_lIsStarted) ? "TRUE" : "FALSE" );
			TRACE( "%s\n", l_szbuf );
		}
#endif
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraLoadFiles(unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionDevice::InternalTriggerEnable( unsigned long p_ulIndex )
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT SVFileAcquisitionDevice::InternalTrigger( unsigned long p_ulIndex )
{
	HRESULT hr = S_FALSE;
	if (m_triggerMgr.HasSubscribers(p_ulIndex))
	{
		hr = m_triggerMgr.Fire(p_ulIndex);
	}
	else
	{
		hr = FireOneShot(p_ulIndex);
	}
	return hr;
}

HRESULT SVFileAcquisitionDevice::RegisterInternalTriggerCallback( unsigned long p_ulIndex, const SvTh::TriggerDispatcher& rDispatcher )
{
	m_acquisitionTriggers.Add(p_ulIndex, rDispatcher );
	
	typedef SvTi::SVTriggerActivatorFunc<SVFileAcquisitionDevice> Activator;
	typedef SvTi::SVTriggerCallbackFunc<SVFileAcquisitionDevice> TriggerCallback;
	SvTh::SVTriggerHandler handler(p_ulIndex,
		SvTh::SVTriggerActivator(new Activator(this, &SVFileAcquisitionDevice::FireOneShot)),
		SvTh::SVTriggerCallback(new TriggerCallback(this, &SVFileAcquisitionDevice::DispatchTriggerCallback)));

	return m_triggerMgr.Subscribe(p_ulIndex, handler);
}

HRESULT SVFileAcquisitionDevice::UnregisterInternalTriggerCallback( unsigned long p_ulIndex, const SvTh::TriggerDispatcher& rDispatcher )
{
	m_acquisitionTriggers.Remove(p_ulIndex, rDispatcher);
	return m_triggerMgr.Unsubscribe( p_ulIndex);
}

HRESULT SVFileAcquisitionDevice::UnregisterAllInternalTriggerCallbacks( unsigned long p_ulIndex )
{
	m_acquisitionTriggers.RemoveAll(p_ulIndex);
	return m_triggerMgr.Unsubscribe( p_ulIndex );
}

HRESULT SVFileAcquisitionDevice::FireOneShot( unsigned long p_ulIndex )
{
	HRESULT hr = S_FALSE;
	if( p_ulIndex >= 0 && p_ulIndex < m_cameras.size())
	{
		hr = m_cameras[ p_ulIndex ].DoOneShot();
	}
	return hr;
}

HRESULT SVFileAcquisitionDevice::DispatchTriggerCallback( unsigned long p_ulIndex )
{
	HRESULT hr = m_acquisitionTriggers.Dispatch( p_ulIndex );
	return hr;
}

HRESULT SVFileAcquisitionDevice::TriggerGetCount(unsigned long& p_ulCount)
{
	HRESULT l_Result = CameraGetCount(p_ulCount);
	return l_Result;
}

// convert ordinal to handle
unsigned long SVFileAcquisitionDevice::TriggerGetHandle(unsigned long p_ulIndex)
{
	return GetDigitizerHandle(p_ulIndex); 
}

HRESULT SVFileAcquisitionDevice::TriggerGetName(unsigned long triggerchannel, BSTR& p_rbstrName)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(triggerchannel))
	{
		if ( nullptr != p_rbstrName )
		{
			::SysFreeString( p_rbstrName );
			p_rbstrName = nullptr;
		}

		SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		std::string name = SvUl::Format("CameraTrigger.Dig_%d", triggerchannel);
		p_rbstrName = _bstr_t(name.c_str());
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerRegisterCallback(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(triggerchannel))
	{
		SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		l_rCamera.SetTriggerDispatcher(rDispatcher);
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerUnregisterCallback(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(triggerchannel))
	{
		SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerUnregisterAllCallbacks(unsigned long triggerchannel)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(triggerchannel))
	{
		SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerStart(unsigned long triggerchannel)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(triggerchannel))
	{
		SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		l_rCamera.m_bAcquisitionTriggered = true;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerStop(unsigned long triggerchannel)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(triggerchannel))
	{
		SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		l_rCamera.m_bAcquisitionTriggered = false;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		if (IsValidDigitizer(triggerchannel))
		{
			*p_pulCount = 1;
			l_hrOk = S_OK;
		}
		else
		{
			*p_pulCount = 0;
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::TriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );
			*p_pbstrName = nullptr;
		}

		if (IsValidDigitizer(triggerchannel))
		{
			switch ( p_ulIndex )
			{
				case SvTi::SVAcquisitionTriggered:
				*p_pbstrName = ::SysAllocString( L"Acquisition Triggered" );
				break;
			}
			if ( nullptr != *p_pbstrName )
			{
				l_hrOk = S_OK;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::TriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		if ( S_OK == ::VariantClear( p_pvarValue ) )
		{
			if (IsValidDigitizer(triggerchannel))
			{
				// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				switch ( p_ulIndex)
				{
					case SvTi::SVAcquisitionTriggered:
					{
						p_pvarValue->vt = VT_BOOL;
						bool bAcquisitonTriggered;
						l_hrOk = IsAcquisitionTriggered(triggerchannel, bAcquisitonTriggered);
						if ( S_OK == l_hrOk )
						{
							p_pvarValue->bVal = (bAcquisitonTriggered) ? VARIANT_TRUE : VARIANT_FALSE;
						}
						break;
					}
				
					default:
						break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::TriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(triggerchannel))
	{
		if ( nullptr != p_pvarValue )
		{
			switch (p_ulIndex)
			{
				case SvTi::SVAcquisitionTriggered:
				{
					if( p_pvarValue->vt == VT_BOOL )
					{
						l_hrOk = SetAcquisitionTriggered( triggerchannel, (p_pvarValue->boolVal == VARIANT_TRUE) ? true : false);
					}
					break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::IsAcquisitionTriggered(unsigned long triggerchannel, bool& bAcquisitionTriggered) const
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(triggerchannel))
	{
		l_hrOk = S_OK;
		const SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		bAcquisitionTriggered = l_rCamera.IsAcquisitionTriggered();
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::SetAcquisitionTriggered(unsigned long triggerchannel, bool bAcquisitionTriggered)
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(triggerchannel))
	{
		l_hrOk = S_OK;
		SVFileCamera& l_rCamera = GetDigitizer(triggerchannel);
		l_rCamera.SetAcquisitionTriggered(bAcquisitionTriggered);
	}
	return l_hrOk;
}

void SVFileAcquisitionDevice::DoAcquisitionTrigger(SVFileCamera& p_rCamera)
{
	SvTl::SVTimeStamp timestamp = SvTl::GetTimeStamp();
	// Simulate Trigger and send Timestamp and Line State...
	bool lineState = p_rCamera.GetLineState(); // could simulate line state via a socket connection

	SvTh::TriggerDispatcher dispatcher(p_rCamera.GetTriggerDispatcher());

	if (dispatcher.hasCallback())
	{
		typedef  std::map<std::string, _variant_t> NameVariantMap;
		NameVariantMap Settings;
		Settings[_T("Timestamp")] = _variant_t(timestamp);
		Settings[_T("LineState")] = _variant_t((lineState) ? VARIANT_TRUE : VARIANT_FALSE);
		Settings[_T("StartFrameTimestamp")] = _variant_t(p_rCamera.m_StartTimeStamp);

		dispatcher.SetData(&Settings);
		dispatcher.Dispatch();
	}
}

