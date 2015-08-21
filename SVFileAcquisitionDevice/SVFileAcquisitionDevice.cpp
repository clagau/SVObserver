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

#include "stdafx.h"
#include "SVFileAcquisitionDevice.h"
#include <boost/bind.hpp>
#include "SVFileAcquisitionDeviceParamEnum.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVImageLibrary/SVImageBufferInterface.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVHBitmapUtilitiesLibrary/SVImageFormatEnum.h"
#include "SVTriggerLibrary/SVTriggerActivatorFunc.h"
#include "SVTriggerLibrary/SVTriggerCallbackFunc.h"
#include "SVTriggerLibrary/SVTriggerEnums.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVMaterialsLibrary/SVMaterials.h"

static const long MaxFileCameras = 12;

SVFileAcquisitionDevice::SVFileAcquisitionDevice()
: m_lRefCount(0)
{
}

SVFileAcquisitionDevice::~SVFileAcquisitionDevice()
{
	Close();
}
// Handle is just the oridnal
static unsigned long GetDigitizerHandle(unsigned long p_ulIndex)
{
	return p_ulIndex;
}

bool SVFileAcquisitionDevice::IsValidDigitizer(unsigned long p_ulHandle) const
{
	return ( p_ulHandle < m_cameras.size() );
}

SVFileCamera& SVFileAcquisitionDevice::GetDigitizer(unsigned long p_ulHandle)
{
	return m_cameras[ p_ulHandle ];
}

const SVFileCamera& SVFileAcquisitionDevice::GetDigitizer(unsigned long p_ulHandle) const
{
	return m_cameras[ p_ulHandle ];
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
			SVString camName;
			camName.Format("File.Dig_%d", i);
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

		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = NULL;
		}

		p_rbstrName = m_cameras[p_ulIndex].GetName().ToBSTR();
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

	if ( p_pvarValue != NULL )
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

		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = NULL;
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

	if ( p_pvarValue != NULL )
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
				rCamera.SetFileName(p_pvarValue->bstrVal);
				l_hrOk = S_OK;
				break;
			}

			case SVFileAcquisitionParameterDirectory:
			{
				rCamera.SetDirectory(p_pvarValue->bstrVal);
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
		m_cameras[p_ulIndex].m_pBufferInterface = NULL;
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
			
			if( l_hrOk != S_OK )
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

		if ( rCamera.m_lIsStarted == 1 && rCamera.m_pBufferInterface != NULL )
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

		if ( rCamera.m_lIsStarted && rCamera.m_pBufferInterface != NULL )
		{
			if (rCamera.IsAcquisitionTriggered())
			{
				DoAcquisitionTrigger(rCamera);
			}
			SVImageBufferInterface l_Buffer;
			
			if( rCamera.m_pBufferInterface->GetNextBuffer( l_Buffer ) == S_OK )
			{
				l_Buffer.SetStartFrameTimeStamp( rCamera.m_StartTimeStamp );
				l_Buffer.SetEndFrameTimeStamp( rCamera.m_pBufferInterface->GetTimeStamp() );

				unsigned char *l_pBuffer = l_Buffer.GetBufferAddress();

				if( l_pBuffer != NULL )
				{
					l_hrOk = rCamera.CopyImage( l_pBuffer );

					if( l_hrOk == S_OK )
					{
						rCamera.m_pBufferInterface->UpdateWithCompletedBuffer( l_Buffer );
					}
					else
					{
						wsprintf(l_szbuf, "FileAcquisition::CopyImage - Error in Format");
						TRACE( "%s\n", l_szbuf );

						CString l_csBuf;
						l_csBuf.Format(_T("E: FileAcquisition::CopyImage - Error in Format"));

						SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
						Exception.setMessage( SVMSG_IMAGE_FORMAT_ERROR, l_csBuf, StdMessageParams );
					}
				}
				else
				{
					wsprintf(l_szbuf,"Error In BufferGetAddress" );
					TRACE( "%s\n", l_szbuf );
				}
			}
		}
		else
		{
			wsprintf(l_szbuf,"IsStarted = %s", (rCamera.m_lIsStarted) ? "TRUE" : "FALSE" );
			TRACE( "%s\n", l_szbuf );
		}
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

HRESULT SVFileAcquisitionDevice::RegisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct )
{
	SVTriggerCallbackStruct triggerCallbackStruct; 
	triggerCallbackStruct.pCallback = callbackStruct.m_pCallback;
	triggerCallbackStruct.pOwner = callbackStruct.m_pOwner;
	triggerCallbackStruct.pData = callbackStruct.m_pData;

	m_triggerMap.Add(p_ulIndex, triggerCallbackStruct);
	
	typedef SVTriggerActivatorFunc<SVFileAcquisitionDevice> Activator;
	typedef SVTriggerCallbackFunc<SVFileAcquisitionDevice> TriggerCallback;
	SVTriggerHandler handler(p_ulIndex, 
							SVTriggerActivator(new Activator(this, &SVFileAcquisitionDevice::FireOneShot)), 
							SVTriggerCallback(new TriggerCallback(this, &SVFileAcquisitionDevice::DispatchTriggerCallback)));

	return m_triggerMgr.Subscribe(p_ulIndex, handler);
}

HRESULT SVFileAcquisitionDevice::UnregisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct )
{
	SVTriggerCallbackStruct triggerCallbackStruct; 
	triggerCallbackStruct.pCallback = callbackStruct.m_pCallback;
	triggerCallbackStruct.pOwner = callbackStruct.m_pOwner;
	triggerCallbackStruct.pData = callbackStruct.m_pData;

	m_triggerMap.Remove(p_ulIndex, triggerCallbackStruct);
	return m_triggerMgr.Unsubscribe( p_ulIndex);
}

HRESULT SVFileAcquisitionDevice::UnregisterAllInternalTriggerCallbacks( unsigned long p_ulIndex )
{
	m_triggerMap.RemoveAll(p_ulIndex);
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
	HRESULT hr = m_triggerMap.Dispatch( p_ulIndex );
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

HRESULT SVFileAcquisitionDevice::TriggerGetName(unsigned long p_ulHandle, BSTR& p_rbstrName)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );
			p_rbstrName = NULL;
		}

		SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		SVString name;
		name.Format("CameraTrigger.Dig_%d", p_ulHandle);
		p_rbstrName = name.ToBSTR();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerRegisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.SetTriggerCallback(p_Callback);
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerUnregisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerUnregisterAllCallbacks(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerStart(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.m_bAcquisitionTriggered = true;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerStop(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.m_bAcquisitionTriggered = false;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVFileAcquisitionDevice::TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		if (IsValidDigitizer(p_ulHandle))
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

HRESULT SVFileAcquisitionDevice::TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );
			*p_pbstrName = NULL;
		}

		if (IsValidDigitizer(p_ulHandle))
		{
			switch ( p_ulIndex )
			{
				case SVAcquisitionTriggered:
				*p_pbstrName = ::SysAllocString( L"Acquisition Triggered" );
				break;
			}
			if ( *p_pbstrName != NULL )
			{
				l_hrOk = S_OK;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( ::VariantClear( p_pvarValue ) == S_OK )
		{
			if (IsValidDigitizer(p_ulHandle))
			{
				// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				switch ( p_ulIndex)
				{
					case SVAcquisitionTriggered:
					{
						p_pvarValue->vt = VT_BOOL;
						bool bAcquisitonTriggered;
						l_hrOk = IsAcquisitionTriggered(p_ulHandle, bAcquisitonTriggered);
						if (l_hrOk == S_OK)
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

HRESULT SVFileAcquisitionDevice::TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		if ( p_pvarValue != NULL )
		{
			switch (p_ulIndex)
			{
				case SVAcquisitionTriggered:
				{
					if( p_pvarValue->vt == VT_BOOL )
					{
						l_hrOk = SetAcquisitionTriggered( p_ulHandle, (p_pvarValue->boolVal == VARIANT_TRUE) ? true : false);
					}
					break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::IsAcquisitionTriggered(unsigned long p_ulHandle, bool& bAcquisitionTriggered) const
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		l_hrOk = S_OK;
		const SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		bAcquisitionTriggered = l_rCamera.IsAcquisitionTriggered();
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::SetAcquisitionTriggered(unsigned long p_ulHandle, bool bAcquisitionTriggered)
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		l_hrOk = S_OK;
		SVFileCamera& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.SetAcquisitionTriggered(bAcquisitionTriggered);
	}
	return l_hrOk;
}

void SVFileAcquisitionDevice::DoAcquisitionTrigger(SVFileCamera& p_rCamera)
{
	SVClock::SVTimeStamp timestamp = SVClock::GetTimeStamp();
	// Simulate Trigger and send Timestamp and Line State...
	bool lineState = p_rCamera.GetLineState(); // could simulate line state via a socket connection
	const SVCallbackStruct& callback = p_rCamera.GetTriggerCallback();
	if (callback.m_pCallback)
	{
		SVMaterials materials;
		materials.AddMaterial("Timestamp", _variant_t(timestamp));
		materials.AddMaterial("LineState", _variant_t((lineState) ? VARIANT_TRUE : VARIANT_FALSE));
		materials.AddMaterial("StartFrameTimestamp", _variant_t(p_rCamera.m_StartTimeStamp));
			
		callback.m_pCallback(callback.m_pOwner, reinterpret_cast<void *>(&materials));
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVFileAcquisitionDevice\SVFileAcquisitionDevice.cpp_v  $
 * 
 *    Rev 1.3   01 Dec 2014 12:16:00   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Thread Manager Settings
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2013 06:14:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   07 May 2013 20:04:34   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 012.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   29 Apr 2013 12:30:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 09:26:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   21 Jan 2013 09:47:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Feb 2010 09:51:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change thread creation location.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Sep 2009 08:45:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jul 2009 15:00:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   05 Dec 2008 15:23:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised Loading modes for File Acquisition
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 07:58:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/