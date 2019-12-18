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
#include "SVFileAcquisitionDevice.h"
#include "SVFileAcquisitionDeviceParamEnum.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "Definitions/SVImageFormatEnum.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

SVFileAcquisitionDevice::SVFileAcquisitionDevice()
: m_lRefCount(0)
{
}

SVFileAcquisitionDevice::~SVFileAcquisitionDevice()
{
	Destroy(true);
}
// Handle is just the original
static unsigned long GetDigitizerHandle(unsigned long index)
{
	return index;
}

bool SVFileAcquisitionDevice::IsValidDigitizer(unsigned long triggerchannel) const
{
	return ( triggerchannel < MaxFileCameras);
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
		for (int i = 0; i < MaxFileCameras; i++)
		{
			std::string camName{"File.Dig_"};
			camName += std::to_string(i);
			m_cameras[i].SetName(camName);
		}
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::Destroy( bool p_bClose )
{
	HRESULT l_hrOk = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &m_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &m_lRefCount, 0 );
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetCount( unsigned long &p_rulCount )
{
	HRESULT l_hrOk = S_OK;

	p_rulCount = MaxFileCameras;

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetName( unsigned long index, BSTR &p_rbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if(index < MaxFileCameras)
	{
		l_hrOk = S_OK;

		if ( nullptr != p_rbstrName )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = nullptr;
		}

		p_rbstrName = _bstr_t(m_cameras[index].GetName().c_str()).Detach();
	} 
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetHeight( unsigned long index, unsigned long &p_rulHeight )
{
	HRESULT l_hrOk = S_FALSE;

	if(index < MaxFileCameras)
	{
		p_rulHeight = m_cameras[index].GetImageSize().cy;

		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetWidth( unsigned long index, unsigned long &p_rulWidth )
{
	HRESULT l_hrOk = S_FALSE;

	if(index < MaxFileCameras)
	{
		p_rulWidth = m_cameras[index].GetImageSize().cx;

		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetFormat( unsigned long index, int &p_riFormat )
{
	HRESULT l_hrOk = S_FALSE;

	if(index < MaxFileCameras)
	{
		switch( m_cameras[index].GetImageFormat() )
		{
			case SvDef::SVImageFormatMono16:
			case SvDef::SVImageFormatRGB888:
			case SvDef::SVImageFormatRGB8888:
			case SvDef::SVImageFormatRGB161616:
			case SvDef::SVImageFormatY411:
			case SvDef::SVImageFormatY422:
			case SvDef::SVImageFormatY444:
			{
				p_riFormat = SvDef::SVImageFormatRGB8888;

				break;
			}
			default:
			{
				p_riFormat = m_cameras[index].GetImageFormat();

				break;
			}
		}

		l_hrOk = S_OK;
	} 
	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraGetParameterList( unsigned long index, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		::VariantClear( p_pvarValue );

		if(index < MaxFileCameras)
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

HRESULT SVFileAcquisitionDevice::CameraGetParameterName( unsigned long index, int p_iParameterID, BSTR &p_rbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if (index < MaxFileCameras)
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

HRESULT SVFileAcquisitionDevice::CameraGetParameter( unsigned long index, int p_iParameterID, int *p_piParameterIDType, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		if(index < MaxFileCameras)
		{
			SVFileCamera& rCamera = m_cameras[ index ];

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

HRESULT SVFileAcquisitionDevice::CameraSetParameter( unsigned long index, int p_iParameterID, int p_iParameterIDType, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if(index < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[ index ];

		switch( p_iParameterID )
		{
			case SVFileAcquisitionParameterImageFormat:
			{
				rCamera.SetImageFormat(static_cast<SvDef::SVImageFormatEnum>(p_pvarValue->ulVal));
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
				std::string fileName;
				fileName.assign(_bstr_t(p_pvarValue->bstrVal));
				rCamera.SetFileName(fileName);
				l_hrOk = S_OK;
				break;
			}

			case SVFileAcquisitionParameterDirectory:
			{
				std::string directoryName;
				directoryName.assign(_bstr_t(p_pvarValue->bstrVal));
				rCamera.SetDirectory(directoryName);
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

HRESULT SVFileAcquisitionDevice::CameraBufferCreateAll( unsigned long index )
{
	HRESULT l_hrOk = S_OK;

	if (MaxFileCameras <= index)
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraBufferDestroyAll( unsigned long index )
{
	HRESULT l_hrOk = S_OK;

	if (MaxFileCameras <= index)
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraRegisterBufferInterface( unsigned long index, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_OK;

	if ( index < MaxFileCameras)
	{
		m_cameras[index].m_pBufferInterface = p_pInterface;
	} 
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraUnregisterBufferInterface( unsigned long index )
{
	HRESULT l_hrOk = S_OK;

	if(index < MaxFileCameras)
	{
		m_cameras[index].m_pBufferInterface = nullptr;
	} 
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVFileAcquisitionDevice::CameraStart( unsigned long index )
{
	HRESULT result = S_FALSE;

	if(index < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[index];
		if ( ::InterlockedExchange( &(rCamera.m_lIsStarted), 1 ) == 0 )
		{
			// Check if already Running
			if( ! rCamera.IsRunning() )
			{
				SVFileCamera::EventHandler startHandler = boost::bind(&SVFileAcquisitionDevice::CameraProcessStartFrame, this, _1);
				SVFileCamera::EventHandler endHandler = boost::bind(&SVFileAcquisitionDevice::CameraProcessEndFrame, this, _1);

				result = rCamera.Start( startHandler, endHandler, index);
			}
			
			if( S_OK != result )
			{
				::InterlockedExchange( &(rCamera.m_lIsStarted), 0 );
			}
		}
		else
		{
			// Camera is already started.
			result = S_OK;
		}
	}
	return result;
}

HRESULT SVFileAcquisitionDevice::CameraStop( unsigned long index )
{
	HRESULT l_hrOk = S_OK;

	if(index < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[index];
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
HRESULT SVFileAcquisitionDevice::CameraProcessStartFrame( unsigned long index )
{
	HRESULT l_hrOk = S_OK;

	if(index < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[ index ];

		if ( 1 == rCamera.m_lIsStarted && nullptr != rCamera.m_pBufferInterface )
		{
			rCamera.m_StartTimeStamp = SvTl::GetTimeStamp();
		}
		else
		{
			l_hrOk = S_FALSE;
		}
	}
	return l_hrOk;
}

// call this when Loader finishes ?
HRESULT SVFileAcquisitionDevice::CameraProcessEndFrame( unsigned long index )
{
	HRESULT l_hrOk = S_OK;

	if ( index < MaxFileCameras)
	{
		SVFileCamera& rCamera = m_cameras[index];
		if ( rCamera.m_lIsStarted && nullptr != rCamera.m_pBufferInterface )
		{
			SvTrc::IImagePtr pImage = rCamera.m_pBufferInterface->GetNextBuffer();
			
			if( nullptr != pImage && nullptr != pImage->getHandle() )
			{
				l_hrOk = rCamera.CopyImage(pImage.get());

				if (S_OK != l_hrOk)
				{
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
						std::string outputText{"FileAcquisition::CopyImage - Error in Format\r\n"};
						::OutputDebugString(outputText.c_str());
#endif

					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_IMAGE_FORMAT_ERROR, SvStl::Tid_FileAcquisition_FormatError, SvStl::SourceFileParams(StdMessageParams));
				}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
				else
				{
					std::string outputText {"Error In BufferGetAddress\r\n"};
					::OutputDebugString(outputText.c_str());
					l_hrOk = E_FAIL;
				}
#endif
			}
			else
			{
				SvDef::StringVector msgList;
				msgList.push_back(rCamera.GetName()+" : "+rCamera.GetFileName());
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_IMAGE_FORMAT_ERROR, SvStl::Tid_CameraEndFrame_GetNextBufferFailed, msgList, SvStl::SourceFileParams(StdMessageParams));
				l_hrOk = E_FAIL;
			}
			
			//Send this command also if buffer failed to trigger the PPQ-Thread to give it a change for cleanup.
			rCamera.m_pBufferInterface->UpdateWithCompletedBuffer(pImage, rCamera.m_StartTimeStamp, SvTl::GetTimeStamp());
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

HRESULT SVFileAcquisitionDevice::CameraLoadFiles(unsigned long index, SAFEARRAY* p_psaFileNames)
{
	HRESULT hr = S_FALSE;
	return hr;
}

HRESULT SVFileAcquisitionDevice::InternalTriggerEnable( unsigned long index )
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT SVFileAcquisitionDevice::InternalTrigger( unsigned long index )
{
	return FireOneShot(index);
}

HRESULT SVFileAcquisitionDevice::FireOneShot( unsigned long index )
{
	HRESULT hr = S_FALSE;
	if( index >= 0 && index < MaxFileCameras)
	{
		hr = m_cameras[index].DoOneShot();
	}
	return hr;
}


