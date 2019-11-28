//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGige
//* .File Name       : $Workfile:   SVMatroxGige.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.6  $
//* .Check In Date   : $Date:   01 Dec 2014 12:28:58  $
//******************************************************************************
#pragma region Includes
#include "StdAfx.h"
//Moved to precompiled header: #include <map>
#include "SVMatroxGige.h"
#include "SVMatroxGigeApp.h"
#include "SVMatroxLibrary/SVMatroxSystemInterface.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMatroxGigeDeviceParameterManager.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

// helpers for System/Digitizer Handles
#define SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE( SystemHandle, DigitizerHandle ) \
	( static_cast<unsigned long>( (SystemHandle) ) << 8 ) | static_cast<unsigned char>( (DigitizerHandle) & 0xFF )

#define SV_EXTRACT_MATROXGIGE_SYSTEM_HANDLE( handle ) ( static_cast<unsigned char>( (handle) >> 8 ) )
#define SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE( handle ) ( static_cast<unsigned char>( (handle) ) )


SVMatroxGige::SVMatroxGige()
{
}

SVMatroxGige::~SVMatroxGige()
{
}

bool SVMatroxGige::IsValidDigitizer(unsigned long channel) const
{
	bool bRetVal = IsValidDigitizerHandle(channel);
	if (bRetVal)
	{
		const SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);
		bRetVal = ( nullptr != l_rCamera.m_Digitizer.get() && !l_rCamera.m_Digitizer.get()->empty() );
	}
	return bRetVal;
}

bool SVMatroxGige::IsValidDigitizerHandle(unsigned long channel) const
{
	unsigned char systemHandle = SV_EXTRACT_MATROXGIGE_SYSTEM_HANDLE(channel);
	unsigned char digitizerHandle = SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(channel);

	return m_Systems.IsValidHandle(systemHandle) && m_Systems.IsValidComponentHandle(systemHandle, digitizerHandle);
}

// Callback used for MdigHook - Start Frame
__int64 SVMatroxGige::DigitizerStartFrameCallback( __int64 HookType, __int64 EventId, void* pContext )
{
	return SVMatroxGige::DigitizerCallback(HookType, EventId, pContext);
}

// Callback used for MdigHook - End Frame
__int64 SVMatroxGige::DigitizerEndFrameCallback( __int64 HookType, __int64 EventId, void* pContext )
{
	return SVMatroxGige::DigitizerCallback(HookType, EventId, pContext);
}

// Callback used for MdigProcess
__int64 SVMatroxGige::ProcessFrame( __int64 HookType, __int64 HookId, void* pContext )
{
	try
	{
		SVMatroxGigeDigitizer* l_pCamera = reinterpret_cast<SVMatroxGigeDigitizer*> (pContext);
		if ( nullptr != l_pCamera )
		{
			if ( l_pCamera->m_lIsStarted == 1 )
			{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
				TRACE( "ProcessFrame\n" );
#endif

				//
				if (l_pCamera->m_frameStack.get() == 1)
				{
					// check for Corrupted first
					bool bIsCorrupt = false;
					HRESULT l_Code = SVMatroxDigitizerInterface::IsCorruptedFrame(HookId, bIsCorrupt);
					if (l_Code == S_OK)
					{
						if (!bIsCorrupt) 
						{
							__int64 ModifiedBufferId(0);

							// Retrieve the MIL_ID of the grabbed buffer.
							l_Code = SVMatroxDigitizerInterface::GetGrabBuffer(HookId, ModifiedBufferId);

							if (l_Code == S_OK && ModifiedBufferId)
							{
								// Copy the data
								g_svTheApp.m_svSystem.ProcessEndFrame(*l_pCamera, ModifiedBufferId);
							}
						}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
						else
						{
							TRACE( "Corrupt Frame\n" );
						}
#endif
					}
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
					std::string Temp = SvUl::Format( _T("Process End Frame Callback - Camera %d-%d\n"), l_pCamera->m_SystemHandle, l_pCamera->m_Handle );
					TRACE( Temp.c_str() );
#endif
				}
				else
				{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
					TRACE( "Start Frame Count %d\n", l_pCamera->m_frameStack.get() );
#endif
				}
				l_pCamera->m_frameStack.clear();
			}
		}
	}
	catch (...)
	{
		// Log Exception ?
	}

	return 0;
}

// General handler for MdigHook callbacks (Indirect)
__int64 SVMatroxGige::DigitizerCallback( __int64 HookType, __int64 EventId, void* pContext )
{
	try
	{
		SVMatroxGigeDigitizer* l_pCamera = reinterpret_cast<SVMatroxGigeDigitizer *>(pContext);
		if ( nullptr != l_pCamera )
		{
			if ( l_pCamera->m_lIsStarted == 1 )
			{
				if( SVMatroxDigitizerInterface::IsEventGrabFrameStart(HookType) )
				{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
					TRACE( "Gige Start Frame\n" );
#endif
					// Do StartFrame Logic
					g_svTheApp.m_svSystem.ProcessStartFrame( *l_pCamera );

#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
					std::string Temp = SvUl::Format( _T("Process Start Frame Callback - Camera %d-%d Buffer %d\n"), 
						l_pCamera->m_SystemHandle, 
						l_pCamera->m_Handle, 
						l_pCamera->m_lStartIndex );
					TRACE( Temp.c_str() );
#endif
				}
				
				else if( SVMatroxDigitizerInterface::IsEventGrabFrameEnd(HookType) )
				{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
					TRACE( "Gige End Frame\n" );
#endif
				}
			}
		}
	}
	catch ( ... )
	{
		// Log Exception ?
	}
	return 0L;
}

HRESULT SVMatroxGige::Open()
{
	HRESULT hr = S_OK;
	return hr;
}

HRESULT SVMatroxGige::Create()
{
	HRESULT hr = S_OK;

	if ( ::InterlockedIncrement( &m_lRefCount ) == 1 )
	{
		hr = CreateSystems();

		if ( S_OK != hr )
		{
			Destroy( true );
		}
	}

	return hr;
}

HRESULT SVMatroxGige::CreateSystems()
{
	HRESULT hr = S_OK;
	long l_lSystemCount = 0;

	HRESULT l_Code = SVMatroxApplicationInterface::GetSystemCount( l_lSystemCount );
	if (l_Code == S_OK)
	{
		long gigeSystemID = 0;
		long soliosGigeSystemID = 0;

		for ( long i = 0; i < l_lSystemCount; i++ )
		{
			std::string Name;

			l_Code = SVMatroxApplicationInterface::GetSystemName( i, Name );
			
			if ( l_Code == S_OK )
			{
				// Check for GIGE system
				if( std::string::npos != Name.find( _T("GIGE") ) )
				{
					// Check for Solios GIGE
					if(std::string::npos != Name.find( _T("SOLIOS") ) )
					{
						AddSystem(Name, soliosGigeSystemID++);
					}
					else // GIGE system (dongle ?)
					{
						l_Code = AddSystem(Name, gigeSystemID++);
						if ( S_OK != l_Code )
						{
							hr = l_Code;
						}
					}
				}
			}
			else
			{
				hr = l_Code;
			}
		}
		// if no cameras are attached, then the system allocation fails
		// Not sure that is an error per se...
		//hr = m_Systems.size() ? S_OK : S_FALSE;
	}
	else
	{
		hr = l_Code;
	}
	return hr;
}

HRESULT SVMatroxGige::AddSystem(const std::string& rName, long SystemNumber)
{
	HRESULT hr = S_OK;

	// Allocate the Matrox System
	SVMatroxGigeSystem l_System;
	l_System.m_System = SVMatroxSystemRef{ new SVMatroxSystem };
	l_System.m_Name = _T("Matrox_GIGE");
	l_System.m_matroxSystemName = rName;

	// 
	HRESULT l_Code = SVMatroxSystemInterface::Allocate(rName, SystemNumber, *(l_System.m_System.get()));
	if (l_Code == S_OK)
	{
		hr = m_Systems.Add(l_System);
		
		if (S_OK == hr)
		{
			SVMatroxGigeSystem& rSystem = m_Systems.Get(l_System.m_Handle, hr);
			if (S_OK == hr)
			{
				// Register camera present hook
				l_Code = SVMatroxSystemInterface::SetHookFunction(*(rSystem.m_System.get()), SVMatroxSystemHook::SVCameraPresent, SVMatroxGige::CameraPresentCallback, reinterpret_cast<void*> (&rSystem.m_Handle)); 
				if (l_Code == S_OK)
				{
					hr = CreateDigitizers(rSystem);
				}
				else
				{
					hr = l_Code;
				}
			}
		}
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVMatroxGige::Destroy( bool p_bClose )
{
	HRESULT hr = S_OK;

	if ( p_bClose || ::InterlockedDecrement( &m_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &m_lRefCount, 0 );

		DestroyBuffers();
		DestroySystems();
	}

	return hr;
}

HRESULT SVMatroxGige::DestroyBuffers()
{
	HRESULT hr = S_OK;

	return hr;
}

HRESULT SVMatroxGige::CreateDigitizers(SVMatroxGigeSystem& system)
{
	HRESULT hr = S_OK;

	long l_DigCount = 0;
	HRESULT l_Code = SVMatroxSystemInterface::Get(*(system.m_System.get()), SVMatroxSystemInquire::SVDigitizerNum, l_DigCount);
	if (l_Code == S_OK)
	{
		for ( long i = 0; i < l_DigCount; i++ )
		{
			hr = CreateDigitizer( system, i );
		}
	}
	else
	{
		hr = l_Code;
	}
	return hr;
}

HRESULT SVMatroxGige::CreateDigitizer(SVMatroxGigeSystem& system, long digitizerIndex)
{
	HRESULT hr = S_OK;

	SVMatroxGigeDigitizer l_camera(digitizerIndex, system.m_Handle);
	l_camera.m_Name  = SvUl::Format(_T("Dig_%d"), digitizerIndex);
	l_camera.m_FullName = SvUl::Format(_T("%s.%s"), system.m_Name.c_str(), l_camera.m_Name.c_str());
	l_camera.m_Digitizer = SVMatroxDigitizerPtr{ new SVMatroxDigitizer };

	hr = AllocDigitizer(system, digitizerIndex, l_camera);
	if (S_OK == hr)
	{
		hr = ReadCameraFormat(l_camera);
	}
	if (S_OK == hr)
	{
		hr = system.AddDigitizer(l_camera);
	}
	return hr;
}

HRESULT SVMatroxGige::AllocDigitizer(SVMatroxGigeSystem& system, long digitizerIndex, SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_OK;
	// where to get the data format from? //"gigevision_640x480_mono8.dcf";
	std::string dataFormat = "gigevision_currentstate.dcf"; // from Matrox Sample code...
	
	HRESULT l_Code = SVMatroxDigitizerInterface::Allocate(*(system.m_System.get()), digitizerIndex, dataFormat, *(p_rCamera.m_Digitizer.get()));
	
	// Read Serial Number if no prior errors
	if (l_Code == S_OK)
	{
		//
		hr = ReadCameraSerialNumber(p_rCamera);
		if (S_OK == hr)
		{
			hr = ReadCameraIPAddress(p_rCamera);
		}
		if (l_Code == S_OK)
		{
			p_rCamera.GetGigeEventList();
		}
	}
	else 
	{
		hr = l_Code;
	}
	return hr;
}

HRESULT SVMatroxGige::DestroyDigitizers(SVMatroxGigeSystem& rSystem)
{
	HRESULT hr = S_OK;

	SVMatroxGigeDigitizerList& rList = rSystem.GetDigitizerList();
	SVMatroxGigeDigitizerList::iterator it;
	for (it = rList.begin();it != rList.end();++it)
	{
		CameraStop(SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE(rSystem.m_Handle, it->first)); // Stop Acquiring...

		if ( S_OK != DestroyDigitizer( it->second ) )
		{
			hr = S_FALSE;
		}
	}
	//
	rList.clear();

	return hr; 
}

HRESULT SVMatroxGige::DestroyDigitizer(SVMatroxGigeDigitizer& rCamera)
{
	HRESULT hr = S_OK;
	
	// Destroy Digitizer
	if(nullptr != rCamera.m_Digitizer)
	{
		HRESULT l_Code = SVMatroxDigitizerInterface::Destroy(*(rCamera.m_Digitizer.get()));

		hr = l_Code; 
	}
	return hr;
}

HRESULT SVMatroxGige::DestroySystems()
{
	HRESULT hr = S_OK;

	SVMatroxGigeSystemList::iterator it;
	for (it = m_Systems.begin();it != m_Systems.end();++it)
	{
		DestroyDigitizers(it->second);

		if ( S_OK != DestroySystem( it->second ) )
		{
			hr = S_FALSE;
		}
	}
	m_Systems.clear();

	return hr; 
}

HRESULT SVMatroxGige::DestroySystem(SVMatroxGigeSystem& rSystem)
{
	HRESULT hr = S_OK;

	// Unregister camera present hook
	/*HRESULT l_Code = */SVMatroxSystemInterface::ReleaseHookFunction(*(rSystem.m_System.get()), SVMatroxSystemHook::SVCameraPresent, SVMatroxGige::CameraPresentCallback, reinterpret_cast<void*> (&rSystem.m_Handle));
	HRESULT l_Code = SVMatroxSystemInterface::Destroy(*(rSystem.m_System.get()));
	if (l_Code != S_OK)
	{
		hr = l_Code; 
	}
	return hr; 
}

HRESULT SVMatroxGige::Close()
{
	HRESULT hr = Destroy( true );
	return hr;
}

HRESULT SVMatroxGige::CameraGetCount( unsigned long &p_rulCount )
{
	HRESULT hr = S_OK;

	p_rulCount = 0;

	// iterate thru systems and associated digitizers
	for (SVMatroxGigeSystemList::iterator it = m_Systems.begin();it != m_Systems.end();++it)
	{
		p_rulCount += static_cast< unsigned long >( it->second.GetDigitizerList().size() );
	}
	return hr;
}

HRESULT SVMatroxGige::CameraGetName( unsigned long channel, BSTR &p_rbstrName )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(channel) )
	{
		hr = S_OK;

		if ( nullptr != p_rbstrName )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = nullptr;
		}

		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		p_rbstrName = _bstr_t(l_rCamera.m_FullName.c_str());
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraGetHeight( unsigned long channel, unsigned long &p_rulHeight )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		SIZE size;
		HRESULT l_Code = SVMatroxDigitizerInterface::GetSize(*(l_rCamera.m_Digitizer.get()), size.cx, size.cy);
		if (l_Code == S_OK)
		{
			p_rulHeight = size.cy;
			hr = S_OK;
		}
	} 

	return hr;
}

HRESULT SVMatroxGige::CameraGetWidth( unsigned long channel, unsigned long &p_rulWidth )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		SIZE size;
		HRESULT l_Code = SVMatroxDigitizerInterface::GetSize(*(l_rCamera.m_Digitizer.get()), size.cx, size.cy);
		if (l_Code == S_OK)
		{
			p_rulWidth = size.cx;
			hr = S_OK;
		}
	} 

	return hr;
}

HRESULT SVMatroxGige::CameraGetParameterList( unsigned long channel, VARIANT *p_pvarValue )
{
	HRESULT hr = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		::VariantClear( p_pvarValue );

		if ( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
		{
			SAFEARRAYBOUND l_psabData[1];
			long l_plIndex[1];
			l_psabData[0].cElements = SvDef::SVGigeParameterSize;
			l_psabData[0].lLbound = 0;

			p_pvarValue->vt = VT_ARRAY | VT_I4;
			p_pvarValue->parray = ::SafeArrayCreate( VT_I4, 1, l_psabData );

			for( long i = 0; i <  SvDef::SVGigeParameterSize; i++ )
			{
				//Parameters
				l_plIndex[0] = i;
				::SafeArrayPutElement( p_pvarValue->parray, l_plIndex, (void *)(l_plIndex) );
			}
			hr = S_OK;
		}
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraGetParameterName( unsigned long channel, int p_iParameterID, BSTR* p_pBstrName )
{
	HRESULT hr = S_FALSE;

	if ( nullptr != p_pBstrName )
	{
		if ( nullptr != *p_pBstrName )
		{
			::SysFreeString( *p_pBstrName );

			*p_pBstrName = nullptr;
		}

		if ( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
		{
			SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);
			hr = SVMatroxGigeDeviceParameterManager::GetParameterName( l_rCamera, p_iParameterID, p_pBstrName );
		}
	}
	return hr;
}

HRESULT SVMatroxGige::CameraGetParameter( unsigned long channel, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue )
{
	HRESULT hr = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		::VariantClear( p_pvarValue );

		if( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
		{
			SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

			hr = SVMatroxGigeDeviceParameterManager::GetParameter(l_rCamera, p_iParameterID, p_piParameterTypeID, p_pvarValue);
		}
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraSetParameter( unsigned long channel, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarValue )
{
	HRESULT hr = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		if ( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
		{
			SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

			switch (p_iParameterID)
			{
				case  SvDef::SVGigeParameterFeatureOverrides:
				{
					_bstr_t xmlData = *p_pvarValue;
					hr = l_rCamera.BuildGigeFeaturesMap(xmlData);
				}
				break;

				case  SvDef::SVGigeBeginTrackParameters:
				{
					TrackCameraParams(l_rCamera);
					hr = S_OK;
				}
				break;

				case  SvDef::SVGigeEndTrackParameters:
				{
					LockMainCameraParamList(l_rCamera);
					hr = S_OK;
				}
				break;

				case  SvDef::SVGigeParameterLineInput:
				{
					l_rCamera.SetLineInputMoniker(SvUl::createStdString(p_pvarValue->bstrVal));
				}
				break;

				case  SvDef::SVGigeParameterInputEvent: // Internal use only, not settable via normal logic
				case  SvDef::SVGigeParameterInputEventName: // Internal use only, not settable via normal logic
				{
				}
				break;

				default:
				{
					hr = SVMatroxGigeDeviceParameterManager::SetParameter(l_rCamera, p_iParameterID, p_iParameterTypeID, p_pvarValue);

					// Keep track of parameters sent for reconnect logic
					SetCameraParam(l_rCamera, p_iParameterID, p_iParameterTypeID, p_pvarValue);
				}
				break;
			}
		}
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraGetFormat( unsigned long channel, int &p_riFormat )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		switch( l_rCamera.m_params.Format )
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
				p_riFormat = l_rCamera.m_params.Format;
				break;
			}
		}

		hr = S_OK;
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraBufferCreateAll( unsigned long channel, unsigned long p_ulCount )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(channel) && IsValidDigitizer(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		hr = CameraBufferDestroyAll( channel );

		if ( S_OK == hr )
		{
			const SVMatroxGigeSystem& rSystem = m_Systems.Get(l_rCamera.m_SystemHandle, hr);

			hr = ReadCameraFormat(l_rCamera);

			if ( S_OK == hr )
			{
				//
				hr = l_rCamera.CreateAcquisitionBuffers(*(rSystem.m_System.get()), 
													l_rCamera.m_params.Width, 
													l_rCamera.m_params.Height,
													l_rCamera.m_params.Format);
			}
		}

		if ( S_OK != hr )
		{
			CameraBufferDestroyAll( channel );
		}
	}
	return hr;
}

HRESULT SVMatroxGige::CameraBufferUnlockAll(unsigned long channel)
{
	HRESULT hr = S_OK;

	if ( IsValidDigitizerHandle(channel) )
	{
		/*SVMatroxGigeDigitizer& l_rCamera = */GetDigitizer(channel);
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVMatroxGige::CameraBufferDestroyAll(unsigned long channel)
{
	HRESULT hr = S_OK;

	// make sure there is a valid handle to the digitizer
	if ( IsValidDigitizer(channel) )
	{
		// Stop the camera if it's active
		if ( IsCameraActive(channel) )
		{
			hr = CameraStop( channel );
		}

		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);
		l_rCamera.DestroyAcquistionBuffers();
	}
	return hr;
}

HRESULT SVMatroxGige::CameraRegisterBufferInterface( unsigned long channel, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizer(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		l_rCamera.m_pBufferInterface = p_pInterface;
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVMatroxGige::CameraUnregisterBufferInterface( unsigned long channel )
{
	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizer(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		l_rCamera.m_pBufferInterface = nullptr;
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVMatroxGige::CameraStart( unsigned long channel )
{
	HRESULT hr = S_FALSE;

	// make sure there is a valid handle to the digitizer
	if ( IsValidDigitizer( channel ) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		if ( ::InterlockedExchange( &(l_rCamera.m_lIsStarted), 1 ) == 0 )
		{
			hr = S_OK;
			if (l_rCamera.HasLineInput())
			{
				if (!l_rCamera.IsGigeEventListValid())
				{
					hr = -123456;
				}
			}
			if ( S_OK == hr )
			{
				hr = StartDigitizer(channel, l_rCamera);
			}
			
			if ( S_OK != hr )
			{
				::InterlockedExchange( &(l_rCamera.m_lIsStarted), 0 );
			}
		}
		else
		{
			// Camera is already started.
			hr = S_OK;
		}
	}

	return hr;
}

HRESULT SVMatroxGige::CameraStop( unsigned long channel )
{
	HRESULT hr = S_OK;

	// make sure there is a valid handle for the digitizer
	if ( IsValidDigitizer( channel ) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		if ( ::InterlockedExchange( &(l_rCamera.m_lIsStarted), 0 ) == 1 )
		{
			hr = StopDigitizer(l_rCamera);
			
			// reset to Hardware Trigger
			l_rCamera.m_params.TriggerType = SvDef::HardwareTrigger;

			if( S_OK == SVMatroxGigeDeviceParameterManager::IsParameterSupported(l_rCamera, SvDef::SVGigeParameterStrobeEnable) )
			{
				_variant_t value("Disable");

				hr = SVMatroxGigeDeviceParameterManager::SetParameter(l_rCamera, SvDef::SVGigeParameterTriggerEnable, 0, &value);
			}
		}
	}
	return hr;
}

HRESULT SVMatroxGige::StartDigitizer(unsigned long channel, SVMatroxGigeDigitizer& p_rCamera)
{
	CameraBufferUnlockAll( channel );

	p_rCamera.m_frameStack.clear();

	HRESULT hr = RegisterMatroxDigitizerHooks(p_rCamera);
	if ( S_OK == hr )
	{
		hr = SetGrabMode(p_rCamera);
	}
	if ( S_OK == hr )
	{
		hr = EnableTriggering(p_rCamera);
	}
	if ( S_OK == hr )
	{
		// Start the Asynchronous Grab (using MdigProcess)
		hr = p_rCamera.StartGrabArray(SVMatroxGige::ProcessFrame);
	}
	return hr;
}

HRESULT SVMatroxGige::StopDigitizer(SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_OK;
	if (nullptr != p_rCamera.m_Digitizer)
	{
		// stop acquiring
		hr = p_rCamera.StopGrabArray(SVMatroxGige::ProcessFrame);

		// Remove Matrox Hooks
		UnRegisterMatroxDigitizerHooks(p_rCamera);
	}
	//
	p_rCamera.m_frameStack.clear();
	return hr;
}

HRESULT SVMatroxGige::ProcessStartFrame( SVMatroxGigeDigitizer& p_rCamera )
{
	HRESULT hr = S_OK;

	if ( p_rCamera.m_lIsStarted == 1 )
	{
		// Get a Buffer to copy in to
		// Sets m_lStartIndex and sets Buffer lock flag
		hr = GetNextAvailableProcBuffer(p_rCamera);
		if (S_OK == hr)
		{
			// Keep Track of Start Frames
			p_rCamera.m_frameStack.Increment();

			hr = CameraStartFrame( p_rCamera );
		}
		else
		{
			p_rCamera.m_lStartIndex = -1;
		}
	}
	else
	{
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxGige::ProcessEndFrame( SVMatroxGigeDigitizer& p_rCamera, __int64 p_SrcBufferID)
{
	HRESULT hr = CameraEndFrame( p_rCamera, p_SrcBufferID );

	return hr;
}

HRESULT SVMatroxGige::CameraStartFrame( SVMatroxGigeDigitizer& rCamera )
{
	HRESULT hr = S_OK;

	if ( rCamera.m_lIsStarted == 1 && nullptr != rCamera.m_pBufferInterface )
	{
		//When the time stamp is not 0 then a new start frame has been received before the last end frame!
		//This will cause a NAK! 
		if(0 != rCamera.m_StartFrameTimeStamp)
		{
			// log an exception
			SvDef::StringVector msgList;
			msgList.emplace_back(rCamera.m_FullName);
			msgList.emplace_back(SvUl::Format(_T("%.3f"), (SvTl::GetTimeStamp() - rCamera.m_StartFrameTimeStamp) * SvTl::c_MicrosecondsPerMillisecond));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log);
			Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NAK_Error_MissingEndFrame, msgList, SvStl::SourceFileParams(StdMessageParams));
		}
		rCamera.m_StartFrameTimeStamp = SvTl::GetTimeStamp();
	}
	else
	{
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxGige::CameraEndFrame( SVMatroxGigeDigitizer& p_rCamera, __int64 p_SrcBufferID )
{
	HRESULT hr = S_OK;

	//Save end frame time stamp before buffer copy
	double endFrameTimeStamp = SvTl::GetTimeStamp();
	double startFrameTimeStamp = p_rCamera.m_StartFrameTimeStamp;

	p_rCamera.m_StartFrameTimeStamp = 0.0;

	if ( 1 == p_rCamera.m_lIsStarted && nullptr != p_rCamera.m_pBufferInterface  )
	{
		SvTrc::IImagePtr pImage = p_rCamera.m_pBufferInterface->GetNextBuffer();
		if (nullptr != pImage )
		{
			auto pImageHandle = pImage->getHandle();
			if(nullptr != pImageHandle && !(pImageHandle->empty() ) )
			{
				hr = SVMatroxBufferInterface::CopyBuffer(pImageHandle->GetBuffer(), p_SrcBufferID );
			}
			else
			{
				hr = E_FAIL;
			}
		}
		else
		{
			hr = E_FAIL;
		}

		//Send this command also if buffer failed to trigger the PPQ-Thread to give it a change for cleanup.
		HRESULT tmpHr = p_rCamera.m_pBufferInterface->UpdateWithCompletedBuffer(pImage, startFrameTimeStamp, endFrameTimeStamp);
		if (S_OK == hr)
		{
			hr = tmpHr;
		}
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

// Just a stub, actual loading is done in SVObserver via SVGigeCameraFileReader ?
HRESULT SVMatroxGige::CameraLoadFiles(unsigned long triggerchannel, SAFEARRAY* p_psaFileNames)
{
	HRESULT hr = S_FALSE;
	// in order to load the dcf file, we have to destroy and recreate the digitizer ?
	return hr;
}

HRESULT SVMatroxGige::ReadCameraSerialNumber(SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;
	std::string serialNo;

	HRESULT l_Code = SVMatroxDigitizerInterface::GetGigeSerialNumber(*(p_rCamera.m_Digitizer.get()), serialNo);
	if (l_Code == S_OK)
	{
		p_rCamera.m_svCameraId.m_serialNo = serialNo;

		hr = S_OK;
	}
	return hr;
}

HRESULT SVMatroxGige::ReadCameraIPAddress(SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;
	__int64 ipAddress;

	HRESULT l_Code = SVMatroxDigitizerInterface::GetGigeIPAddress(*(p_rCamera.m_Digitizer.get()), ipAddress);

	if ( l_Code == S_OK )
	{
		p_rCamera.m_svCameraId.m_ipAddress = ipAddress;

		hr = S_OK;
	}

	return hr;
}

HRESULT SVMatroxGige::ReadCameraFormat(SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;

	SVMatroxDigitizerFormat format;
	HRESULT l_Code = SVMatroxDigitizerInterface::GetFormat(*(p_rCamera.m_Digitizer.get()), format);

	if (l_Code == S_OK)
	{
		p_rCamera.m_params.Width = format.size.cx;
		p_rCamera.m_params.Height = format.size.cy;
		p_rCamera.m_params.XOffsetPos = format.offset.x;
		p_rCamera.m_params.YOffsetPos = format.offset.y;

		switch (format.bitDepth)
		{
			case 8:
				p_rCamera.m_params.Format = SvDef::SVImageFormatMono8;
				break;

			case 32:
				p_rCamera.m_params.Format = SvDef::SVImageFormatRGB8888;
				break;
		}
		hr = S_OK;
	}
	else
	{
		hr = l_Code;
	}
	return hr;
}

bool SVMatroxGige::IsCameraActive(unsigned long channel)
{
	bool bRetVal = false;
	if ( IsValidDigitizerHandle(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);
		bRetVal = (l_rCamera.m_lIsStarted) ? true : false;
	}
	return bRetVal;
}

HRESULT SVMatroxGige::InternalTriggerEnable(unsigned long channel)
{
	HRESULT hr = S_FALSE;
	if (IsValidDigitizer(channel))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);

		l_rCamera.m_params.TriggerType = SvDef::SoftwareTrigger;

		hr = S_OK;
	}
	return hr;
}

HRESULT SVMatroxGige::InternalTrigger( unsigned long channel )
{
	HRESULT hr = S_FALSE;
	if (IsValidDigitizer(channel))
	{
		hr = FireOneShot(channel);
	}
	return hr;
}

HRESULT SVMatroxGige::FireOneShot( unsigned long channel )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizer(channel) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(channel);
		
		hr = SVMatroxDigitizerInterface::SetFeature(*(l_rCamera.m_Digitizer.get()), "TriggerSoftware", SVMatroxDigitizerFeature::SVTypeCommand, _variant_t(0L));
	}
	return hr;
}

// Get the next available Buffer from the processing Buffer pool
HRESULT SVMatroxGige::GetNextAvailableProcBuffer(SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_OK;
	
	long l_lSize = 16;//p_rCamera.m_numProcBuffers;
	long l_lLastUsed = p_rCamera.m_lLastUsedIndex;
	long l_lIndex = ( l_lLastUsed + 1 ) % l_lSize;

	p_rCamera.m_lStartIndex = l_lIndex;
	p_rCamera.m_lLastUsedIndex = l_lIndex;

	return hr;
}

HRESULT SVMatroxGige::RegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera)
{
	// Register Matrox Hook callback(s)
	HRESULT l_Code = SVMatroxDigitizerInterface::SetHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameStart, SVMatroxGige::DigitizerStartFrameCallback, (void *)&p_rCamera);

	if (l_Code == S_OK)
	{
		l_Code = SVMatroxDigitizerInterface::SetHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameEnd, SVMatroxGige::DigitizerEndFrameCallback, (void *)&p_rCamera);
	}
	return l_Code;
}

HRESULT SVMatroxGige::UnRegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera)
{
	// Unregister Matrox Hook callback
	HRESULT l_Code = S_OK;
	if (nullptr != p_rCamera.m_Digitizer)
	{
		/*l_Code = */SVMatroxDigitizerInterface::ReleaseHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameEnd, &SVMatroxGige::DigitizerEndFrameCallback, (void *)&p_rCamera);
		l_Code = SVMatroxDigitizerInterface::ReleaseHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameStart, &SVMatroxGige::DigitizerStartFrameCallback, (void *)&p_rCamera);
	}
	return l_Code;
}

HRESULT SVMatroxGige::SetGrabMode(const SVMatroxGigeDigitizer& p_rCamera)
{
	// Set Grab Timeout to Infinite
	HRESULT l_Code = SVMatroxDigitizerInterface::SetInfiniteGrabTimeout(*(p_rCamera.m_Digitizer.get()));
	if (l_Code == S_OK)
	{
		// Set Grab Mode to Asynchronous
		l_Code = SVMatroxDigitizerInterface::SetGrabMode(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerGrab::SVGrabAsynchronous);
	}
	return l_Code;
}

HRESULT SVMatroxGige::EnableTriggering(const SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;

	if (p_rCamera.m_params.TriggerType == SvDef::HardwareTrigger)
	{
		_variant_t value("External Trigger");
		hr = SVMatroxGigeDeviceParameterManager::SetParameter(p_rCamera, SvDef::SVGigeParameterTriggerSource, 0, &value);
	}
	else if (p_rCamera.m_params.TriggerType == SvDef::SoftwareTrigger)
	{
		_variant_t value("Software Trigger");
		hr = SVMatroxGigeDeviceParameterManager::SetParameter(p_rCamera, SvDef::SVGigeParameterTriggerSource, 0, &value);
	}

	if (S_OK == hr)
	{
		_variant_t value("Enable");
		hr = SVMatroxGigeDeviceParameterManager::SetParameter(p_rCamera, SvDef::SVGigeParameterTriggerEnable, 0, &value);
	}
	return hr;
}

// Camera Detection - Destroy and recreate System(s), enumerate available digitizers per system
void SVMatroxGige::ScanForCameras()
{
	DestroySystems();
	CreateSystems();
}

const SVMatroxGigeDigitizer& SVMatroxGige::GetDigitizer(unsigned long channel) const
{
	HRESULT hr = S_FALSE;

	unsigned char systemHandle = SV_EXTRACT_MATROXGIGE_SYSTEM_HANDLE(channel);
	unsigned char digitizerHandle = SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(channel);

	const SVMatroxGigeSystem& rSystem = m_Systems.Get(systemHandle, hr);
	return rSystem.GetDigitizer(digitizerHandle, hr);
}

SVMatroxGigeDigitizer& SVMatroxGige::GetDigitizer(unsigned long channel)
{
	HRESULT hr = S_FALSE;

	unsigned char systemHandle = SV_EXTRACT_MATROXGIGE_SYSTEM_HANDLE(channel);
	unsigned char digitizerHandle = SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(channel);

	SVMatroxGigeSystem& rSystem = m_Systems.Get(systemHandle, hr);
	return rSystem.GetDigitizer(digitizerHandle, hr);
}

unsigned long SVMatroxGige::GetDigitizerHandle(unsigned long index) const
{
	unsigned long handle = -1;

	// index is zero based and must be equal to or greater than zero
	if (static_cast<long>(index) >= 0)
	{
		// iterate thru systems and associated digitizers
		for (SVMatroxGigeSystemList::const_iterator it = m_Systems.begin();it != m_Systems.end();++it)
		{
			const SVMatroxGigeDigitizerList& list = it->second.GetDigitizerList();

			// check if digitizer belongs to this system
			if (static_cast<long>(index) < static_cast<long>(list.size()))
			{
				SVMatroxGigeDigitizerList::const_iterator digIt = list.begin();
				std::advance(digIt, index);
				
				handle = SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE(it->first, digIt->first);
				break;
			}
			else
			{
				index -= static_cast< unsigned long >( list.size() );
			}
		}
	}
	return handle;
}

// Handler for camera disconnect/reconnect
__int64 SVMatroxGige::CameraPresentCallback( __int64 HookType, __int64 EventId, void* pContext )
{
	unsigned char systemHandle = *(reinterpret_cast<unsigned char*> (pContext));
	HRESULT hr = S_OK;
	SVMatroxGigeSystem& system = g_svTheApp.m_svSystem.m_Systems.Get(systemHandle, hr);
	const SVMatroxSystem& l_rMatroxSystem = *(system.m_System.get());

	// Inquire the camera present state (present or not present).
	long IsCamPresent = 0;
	HRESULT statusCode = SVMatroxSystemInterface::GetHookInfo(l_rMatroxSystem, EventId, SVMatroxSystemHookInfo::SVCameraPresent, IsCamPresent);
	if (statusCode == S_OK)
	{
		// Inquire the camera's digitizer device number.
		long deviceNumber = 0;
		statusCode = SVMatroxSystemInterface::GetHookInfo(l_rMatroxSystem, EventId, SVMatroxSystemHookInfo::SVCameraNumber, deviceNumber);
		if (statusCode == S_OK)
		{
			if (IsCamPresent)
			{
				// log an exception
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
				Exception.setMessage( SVMSG_SVMATROXGIGE_NO_ERROR, SvStl::Tid_MatroxGigE_Connect, msgList, SvStl::SourceFileParams(StdMessageParams) );

				g_svTheApp.m_svSystem.HandleConnect(system, static_cast<unsigned char>(deviceNumber));
			}
			else
			{
				// log an exception
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
				SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
				Exception.setMessage( SVMSG_SVMATROXGIGE_NO_ERROR, SvStl::Tid_MatroxGigE_Disconnect, msgList, SvStl::SourceFileParams(StdMessageParams) );

				g_svTheApp.m_svSystem.HandleDisconnect(system, deviceNumber);
			}
		}
	}
	return 0L;
}

void SVMatroxGige::HandleConnect(SVMatroxGigeSystem& p_rSystem, long deviceNumber)
{
	// Check if camera is already in the list 
	unsigned long handle = 0;
	HRESULT hr = FindCamera(p_rSystem, deviceNumber, handle);
	bool bHasCamera = (S_OK == hr) ? true : false;

	if (bHasCamera)
	{
		SVMatroxGigeDigitizer& l_rCamera = p_rSystem.GetDigitizer(SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(handle), hr);
		if (S_OK == hr)
		{
			if (nullptr == l_rCamera.m_Digitizer)
			{
				// Allocate, recreate buffers, send camera parameters and start acquire
				l_rCamera.m_Digitizer = SVMatroxDigitizerPtr{ new SVMatroxDigitizer };
				hr = AllocDigitizer(p_rSystem, deviceNumber, l_rCamera);

				if (S_OK == hr)
				{
					if (l_rCamera.m_lIsStarted)
					{
						// Set the Camera Parameters before recreating buffers and restarting acquisition
						hr = m_trackedCameraParameters.ReloadCameraParams(l_rCamera.m_svCameraId.m_serialNo, l_rCamera);
						if (S_OK == hr)
						{
							hr = ReadCameraFormat(l_rCamera);

							if (S_OK == hr)
							{
								hr = l_rCamera.CreateAcquisitionBuffers(*(p_rSystem.m_System.get()), 
																l_rCamera.m_params.Width, 
																l_rCamera.m_params.Height,
																l_rCamera.m_params.Format);

								if (S_OK == hr)
								{
									hr = StartDigitizer(handle, l_rCamera);
									if (S_OK != hr)
									{
										// log an exception
										SvDef::StringVector msgList;
										msgList.push_back(SvUl::Format(_T("%d"), hr));
										msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
										SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
										Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_StartDigitizerError, msgList, SvStl::SourceFileParams(StdMessageParams) );
									}
								}
								else
								{
									// log an exception
									SvDef::StringVector msgList;
									msgList.push_back(SvUl::Format(_T("%d"), hr));
									msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
									SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
									Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_ReloadError, msgList, SvStl::SourceFileParams(StdMessageParams) );
								}
							}
							else
							{
								// log an exception
								SvDef::StringVector msgList;
								msgList.push_back(SvUl::Format(_T("%d"), hr));
								msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
								SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
								Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_CreateError, msgList, SvStl::SourceFileParams(StdMessageParams) );
							}
						}
						else
						{
							// log an exception
							SvDef::StringVector msgList;
							msgList.push_back(SvUl::Format(_T("%d"), hr));
							msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
							SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
							Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_ReadError, msgList, SvStl::SourceFileParams(StdMessageParams) );
						}
					}
				}
				else
				{
					// log an exception
					SvDef::StringVector msgList;
					msgList.push_back(SvUl::Format(_T("%d"), hr));
					msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
					SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
					Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_AllocError, msgList, SvStl::SourceFileParams(StdMessageParams) );
				}
			}
		}
		else
		{
			// log an exception
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), hr));
			msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
			Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_GetDigitizerError, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
	else
	{
		// log an exception
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_FindCameraError, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
}

void SVMatroxGige::HandleDisconnect(SVMatroxGigeSystem& p_rSystem, long deviceNumber)
{
	// Get Digitizer
	unsigned long handle = 0;
	HRESULT hr = FindCamera(p_rSystem, deviceNumber, handle);

	if (S_OK == hr)
	{
		SVMatroxGigeDigitizer& l_rCamera = p_rSystem.GetDigitizer(SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(handle), hr);
		if (S_OK == hr)
		{
			if (l_rCamera.m_lIsStarted)
			{
				hr = StopDigitizer(l_rCamera);
			}
			// destroy matrox acquisition buffers
			l_rCamera.DestroyAcquistionBuffers();

			// destroy matrox digitizer handle
			DestroyDigitizer(l_rCamera);
			l_rCamera.m_Digitizer.reset();
		}
		else
		{
			// log an exception
			SvDef::StringVector msgList;
			msgList.push_back(SvUl::Format(_T("%d"), hr));
			msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
			SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
			Exception.setMessage( SVMSG_MATROX_GIGE_DISCONNECT_ERROR, SvStl::Tid_MatroxGigE_Disconnect_GetDigitizerError, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
	else
	{
		// log an exception
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
		SvStl::MessageMgrStd Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_MATROX_GIGE_DISCONNECT_ERROR, SvStl::Tid_MatroxGigE_Disconnect_FindCameraError, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
}

void SVMatroxGige::TrackCameraParams(const SVMatroxGigeDigitizer& digitizer)
{
	m_trackedCameraParameters.TrackCameraParams(digitizer.m_svCameraId.m_serialNo);
}

void SVMatroxGige::LockMainCameraParamList(const SVMatroxGigeDigitizer& digitizer, bool bLock)
{
	m_trackedCameraParameters.SetMainListLocked(digitizer.m_svCameraId.m_serialNo, bLock);
}

void SVMatroxGige::SetCameraParam(const SVMatroxGigeDigitizer& digitizer, int p_iParameterID, int p_iParameterTypeID, VARIANT* p_pvarValue)
{
	_variant_t val(*p_pvarValue);
	SVMatroxGigeCameraParam cameraParameter(p_iParameterID, p_iParameterTypeID, val);
	m_trackedCameraParameters.AddCameraParam(digitizer.m_svCameraId.m_serialNo, cameraParameter);
}

HRESULT SVMatroxGige::FindCamera(const SVMatroxGigeSystem& p_rSystem, long p_DeviceNumber, unsigned long& p_rHandle)
{
	HRESULT hr = S_FALSE;

	const SVMatroxGigeDigitizerList& list = p_rSystem.GetDigitizerList();
	for (SVMatroxGigeDigitizerList::const_iterator it = list.begin();it!= list.end();++it)
	{
		const SVMatroxGigeDigitizer& digitizer = it->second;
		if (digitizer.GetDeviceNumber() == p_DeviceNumber)
		{
			hr = S_OK;
			p_rHandle = SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE(p_rSystem.m_Handle, it->first);
			break;
		}
	}
	return hr;
}

