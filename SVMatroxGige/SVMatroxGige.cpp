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

#include "StdAfx.h"
#include "SVMatroxGige.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h" // for SV_CURRENT_MIL_VERSION define
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxSystemInterface.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVUtilityLibrary/SVUtilityGlobals.h"
#include "SVTriggerLibrary/SVTriggerActivatorFunc.h"
#include "SVTriggerLibrary/SVTriggerCallbackFunc.h"
#include "SVTriggerLibrary/SVTriggerEnums.h"
#include "SVStatusLibrary/SVException.h"
#include "SVMessage/SVMessage.h"
#include "SVGigeCameraParametersLibrary/SVGigeEnums.h"
#include "SVMatroxGigeApp.h"
#include "SVMatroxGigeCallbackStruct.h"
#include "SVMatroxGigeDeviceParameterManager.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVImageLibrary/SVImageBufferHandleImage.h"
#include "SVImageLibrary/SVImageBufferHandleInterface.h"
#include "SVMaterialsLibrary/SVMaterials.h"

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

bool SVMatroxGige::IsValidDigitizer(unsigned long p_Handle) const
{
	bool bRetVal = IsValidDigitizerHandle(p_Handle);
	if (bRetVal)
	{
		const SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);
		bRetVal = ( l_rCamera.m_Digitizer.get() != NULL && !l_rCamera.m_Digitizer.get()->empty() );
	}
	return bRetVal;
}

bool SVMatroxGige::IsValidDigitizerHandle(unsigned long p_Handle) const
{
	unsigned char systemHandle = SV_EXTRACT_MATROXGIGE_SYSTEM_HANDLE(p_Handle);
	unsigned char digitizerHandle = SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(p_Handle);

	return m_Systems.IsValidHandle(systemHandle) && m_Systems.IsValidComponentHandle(systemHandle, digitizerHandle);
}

// Callback used for MdigHook - Start Frame
SVMatroxIdentifier SVMatroxGige::DigitizerStartFrameCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext )
{
	return SVMatroxGige::DigitizerCallback(HookType, EventId, p_pvContext);
}

// Callback used for MdigHook - End Frame
SVMatroxIdentifier SVMatroxGige::DigitizerEndFrameCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext )
{
	return SVMatroxGige::DigitizerCallback(HookType, EventId, p_pvContext);
}

// Callback used for MdigProcess
SVMatroxIdentifier SVMatroxGige::ProcessFrame( SVMatroxIdentifier HookType, SVMatroxIdentifier HookId, void* p_pvContext )
{
	try
	{
		SVMatroxGigeDigitizer* l_pCamera = reinterpret_cast<SVMatroxGigeDigitizer *>(p_pvContext);
		if ( l_pCamera != NULL )
		{
			if ( l_pCamera->m_lIsStarted == 1 )
			{
				TRACE( "ProcessFrame\n" );

				//
				if (l_pCamera->m_frameStack.get() == 1)
				{
					if (l_pCamera->IsAcquisitionTriggered())
					{
						g_svTheApp.m_svSystem.DoAcquisitionTrigger(*l_pCamera, HookId);
					}
					// check for Corrupted first
					bool bIsCorrupt = false;
					SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::IsCorruptedFrame(HookId, bIsCorrupt);
					if (l_Code == SVMEE_STATUS_OK)
					{
						if (!bIsCorrupt) 
						{
							SVMatroxIdentifier ModifiedBufferId(0);

							// Retrieve the MIL_ID of the grabbed buffer.
							l_Code = SVMatroxDigitizerInterface::GetGrabBuffer(HookId, ModifiedBufferId);

							if (l_Code == SVMEE_STATUS_OK && ModifiedBufferId)
							{
								// Copy the data
								g_svTheApp.m_svSystem.ProcessEndFrame(*l_pCamera, ModifiedBufferId);
							}
						}
						else
						{
							TRACE( "Corrupt Frame\n" );
						}
					}
					CString l_csbuf;
					l_csbuf.Format("Process End Frame Callback - Camera %d-%d",
									l_pCamera->m_SystemHandle, l_pCamera->m_Handle);
					TRACE( "%s\n", l_csbuf );
				}
				else
				{
					TRACE( "Start Frame Count %d\n", l_pCamera->m_frameStack.get() );
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
SVMatroxIdentifier SVMatroxGige::DigitizerCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext )
{
	try
	{
		SVMatroxGigeDigitizer* l_pCamera = reinterpret_cast<SVMatroxGigeDigitizer *>(p_pvContext);
		if ( l_pCamera != NULL )
		{
			if ( l_pCamera->m_lIsStarted == 1 )
			{
				if( SVMatroxDigitizerInterface::IsEventGrabFrameStart(HookType) )
				{
					TRACE( "Gige Start Frame\n" );
					// Do StartFrame Logic
					g_svTheApp.m_svSystem.ProcessStartFrame( *l_pCamera );

					CString l_csbuf;
					l_csbuf.Format("Process Start Frame Callback - Camera %d-%d Buffer %d", 
								l_pCamera->m_SystemHandle, 
								l_pCamera->m_Handle, 
								l_pCamera->m_lStartIndex );
					TRACE( "%s\n", l_csbuf );
				}
				
				else if( SVMatroxDigitizerInterface::IsEventGrabFrameEnd(HookType) )
				{
					TRACE( "Gige End Frame\n" );
					/* For AsyncQueued only
					// Do EndFrame Logic
					g_svTheApp.m_svSystem.ProcessEndFrame( *l_pCamera );

					CString l_csbuf;
					l_csbuf.Format("Process End Frame Callback - Camera %d-%d", l_pCamera->m_SystemHandle, l_pCamera->m_Handle );
					TRACE( "%s\n", l_csbuf );
					*/
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

SVMatroxIdentifier __stdcall SVMatroxGige::LineEdgeEventCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext )
{
	SVMatroxGigeDigitizer* l_pCamera = reinterpret_cast<SVMatroxGigeDigitizer *>(p_pvContext);
	// Get State of the line
	long l_EventType = 0;
	SVMatroxDigitizerInterface::GetGigeEventType(EventId, l_EventType);

	long risingEvent = l_pCamera->GetLineRisingEvent();
	long fallingEvent = l_pCamera->GetLineFallingEvent();

	if (l_EventType == risingEvent) // LineN Rising Event
	{
		l_pCamera->SetLineState(true);
	}
	else if (l_EventType == fallingEvent)  // LineN Falling Event
	{
		l_pCamera->SetLineState(false);
	}
	return 0L;
}

void SVMatroxGige::DoAcquisitionTrigger( const SVMatroxGigeDigitizer& p_rCamera, SVMatroxIdentifier HookId )
{
	// Get The Timestamp
	double timestamp = 0.0;
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetGigeCameraTimestamp(HookId, timestamp);
	if (l_Code == SVMEE_STATUS_OK)
	{
		// Simulate Trigger and send Timestamp and Line State...
		bool lineState = p_rCamera.GetLineState();
		const SVCallbackStruct& callback = p_rCamera.GetTriggerCallback();
		if (callback.m_pCallback)
		{
			SVMaterials materials;
			materials.AddMaterial("Timestamp", _variant_t(timestamp));
			materials.AddMaterial("LineState", _variant_t((lineState) ? VARIANT_TRUE : VARIANT_FALSE));
			materials.AddMaterial("StartFrameTimestamp", _variant_t(p_rCamera.m_StartFrameTimeStamp));
			
			callback.m_pCallback(callback.m_pOwner, reinterpret_cast<void *>(&materials));
		}
	}
	else
	{
		TRACE("SVMatroxGige::DoAcquisitionTrigger unable to get timestamp\n");
	}
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
		m_triggerMgr.Create(_T("SVMatrox GigE"), SVAffinityAcq );

		hr = CreateSystems();

		if ( hr != S_OK )
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

	SVMatroxApplicationInterface::SVStatusCode l_Code = SVMatroxApplicationInterface::GetSystemCount( l_lSystemCount );
	if (l_Code == SVMEE_STATUS_OK)
	{
		long gigeSystemID = 0;
		long soliosGigeSystemID = 0;

		for ( long i = 0; i < l_lSystemCount; i++ )
		{
			SVMatroxString l_Name;

			l_Code = SVMatroxApplicationInterface::GetSystemName( i, l_Name );
			
			if ( l_Code == SVMEE_STATUS_OK )
			{
				// Check for GIGE system
				if (l_Name.find("GIGE") != SVMatroxString::npos )
				{
					// Check for Solios GIGE
					if (l_Name.find("SOLIOS") != SVMatroxString::npos)
					{
						AddSystem(l_Name, soliosGigeSystemID++);
					}
					else // GIGE system (dongle ?)
					{
						AddSystem(l_Name, gigeSystemID++);
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

HRESULT SVMatroxGige::AddSystem(const SVMatroxString& rName, long SystemNumber)
{
	HRESULT hr = S_OK;

	// Allocate the Matrox System
	SVMatroxGigeSystem l_System;
	l_System.m_System = new SVMatroxSystem;
	l_System.m_Name = _T("Matrox_GIGE");
	l_System.m_matroxSystemName = rName;

	// 
	SVMatroxApplicationInterface::SVStatusCode l_Code = SVMatroxSystemInterface::Allocate(rName, SystemNumber, *(l_System.m_System.get()));
	if (l_Code == SVMEE_STATUS_OK)
	{
		hr = m_Systems.Add(l_System);
		
		if (hr == S_OK)
		{
			SVMatroxGigeSystem& rSystem = m_Systems.Get(l_System.m_Handle, hr);
			if (hr == S_OK)
			{
				// Register camera present hook
#if SV_CURRENT_MIL_VERSION == 0x0900
				l_Code = SVMatroxSystemInterface::SetHookFunction(*(rSystem.m_System.get()), SVMatroxSystemHook::SVCameraPresent, SVMatroxGige::CameraPresentCallback, (void *)(l_System.m_Handle)); 
#endif
				if (l_Code == SVMEE_STATUS_OK)
				{
					hr = CreateDigitizers(rSystem);
				}
				else
				{
					hr = S_FALSE;
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
	m_triggerMgr.Destroy();

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
	SVMatroxSystemInterface::SVStatusCode l_Code = SVMatroxSystemInterface::Get(*(system.m_System.get()), SVMatroxSystemInquire::SVDigitizerNum, l_DigCount);
	if (l_Code == SVMEE_STATUS_OK)
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
	l_camera.m_Name.Format(_T("Dig_%d"), digitizerIndex);
	l_camera.m_FullName.Format(_T("%s.%s"), system.m_Name.ToString(), l_camera.m_Name.ToString());
	l_camera.m_Digitizer = new SVMatroxDigitizer;

	hr = AllocDigitizer(system, digitizerIndex, l_camera);
	if (hr == S_OK)
	{
		hr = ReadCameraFormat(l_camera);
	}
	if (hr == S_OK)
	{
		hr = system.AddDigitizer(l_camera);
	}
	return hr;
}

HRESULT SVMatroxGige::AllocDigitizer(SVMatroxGigeSystem& system, long digitizerIndex, SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_OK;
	// where to get the data format from? //"gigevision_640x480_mono8.dcf";
	SVMatroxString dataFormat = "gigevision_currentstate.dcf"; // from Matrox Sample code...
	
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::Allocate(*(system.m_System.get()), digitizerIndex, dataFormat, *(p_rCamera.m_Digitizer.get()));
	
	// Read Serial Number if no prior errors
	if (l_Code == SVMEE_STATUS_OK)
	{
		//
		hr = ReadCameraSerialNumber(p_rCamera);
		if (hr == S_OK)
		{
			hr = ReadCameraIPAddress(p_rCamera);
		}
		if (l_Code == SVMEE_STATUS_OK)
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

		if ( DestroyDigitizer( it->second ) != S_OK )
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
	if (!rCamera.m_Digitizer.empty())
	{
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::Destroy(*(rCamera.m_Digitizer.get()));

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

		if ( DestroySystem( it->second ) != S_OK )
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

	SVMatroxSystemInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;
	// Unregister camera present hook
#if SV_CURRENT_MIL_VERSION == 0x0900
	l_Code = SVMatroxSystemInterface::ReleaseHookFunction(*(rSystem.m_System.get()), SVMatroxSystemHook::SVCameraPresent, SVMatroxGige::CameraPresentCallback, (void *)(rSystem.m_Handle)); 
#endif
	l_Code = SVMatroxSystemInterface::Destroy(*(rSystem.m_System.get()));
	if (l_Code != SVMEE_STATUS_OK)
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

HRESULT SVMatroxGige::CameraGetName( unsigned long p_Handle, BSTR &p_rbstrName )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(p_Handle) )
	{
		hr = S_OK;

		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );

			p_rbstrName = NULL;
		}

		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		p_rbstrName = l_rCamera.m_FullName.ToBSTR();
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraGetHeight( unsigned long p_Handle, unsigned long &p_rulHeight )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		SIZE size;
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetSize(*(l_rCamera.m_Digitizer.get()), size.cx, size.cy);
		if (l_Code == SVMEE_STATUS_OK)
		{
			p_rulHeight = size.cy;
			hr = S_OK;
		}
	} 

	return hr;
}

HRESULT SVMatroxGige::CameraGetWidth( unsigned long p_Handle, unsigned long &p_rulWidth )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		SIZE size;
		SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetSize(*(l_rCamera.m_Digitizer.get()), size.cx, size.cy);
		if (l_Code == SVMEE_STATUS_OK)
		{
			p_rulWidth = size.cx;
			hr = S_OK;
		}
	} 

	return hr;
}

HRESULT SVMatroxGige::CameraGetParameterList( unsigned long p_Handle, VARIANT *p_pvarValue )
{
	HRESULT hr = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		::VariantClear( p_pvarValue );

		if ( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
		{
			SAFEARRAYBOUND l_psabData[1];
			long l_plIndex[1];
			l_psabData[0].cElements = SVGigeParameterSize;
			l_psabData[0].lLbound = 0;

			p_pvarValue->vt = VT_ARRAY | VT_I4;
			p_pvarValue->parray = ::SafeArrayCreate( VT_I4, 1, l_psabData );

			for( long i = 0; i < SVGigeParameterSize; i++ )
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

HRESULT SVMatroxGige::CameraGetParameterName( unsigned long p_Handle, int p_iParameterID, BSTR* p_pBstrName )
{
	HRESULT hr = S_FALSE;

	if ( p_pBstrName != NULL )
	{
		if ( *p_pBstrName != NULL )
		{
			::SysFreeString( *p_pBstrName );

			*p_pBstrName = NULL;
		}

		if ( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
		{
			SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);
			hr = SVMatroxGigeDeviceParameterManager::GetParameterName( l_rCamera, p_iParameterID, p_pBstrName );
		}
	}
	return hr;
}

HRESULT SVMatroxGige::CameraGetParameter( unsigned long p_Handle, int p_iParameterID, int *p_piParameterTypeID, VARIANT *p_pvarValue )
{
	HRESULT hr = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		::VariantClear( p_pvarValue );

		if( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
		{
			SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

			hr = SVMatroxGigeDeviceParameterManager::GetParameter(l_rCamera, p_iParameterID, p_piParameterTypeID, p_pvarValue);
		}
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraSetParameter( unsigned long p_Handle, int p_iParameterID, int p_iParameterTypeID, VARIANT *p_pvarValue )
{
	HRESULT hr = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
		{
			SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

			switch (p_iParameterID)
			{
				case SVGigeParameterFeatureOverrides:
				{
					_bstr_t xmlData = *p_pvarValue;
					hr = l_rCamera.BuildGigeFeaturesMap(xmlData);
				}
				break;

				case SVGigeBeginTrackParameters:
				{
					TrackCameraParams(l_rCamera);
					hr = S_OK;
				}
				break;

				case SVGigeEndTrackParameters:
				{
					LockMainCameraParamList(l_rCamera);
					hr = S_OK;
				}
				break;

				case SVGigeParameterLineInput:
				{
					l_rCamera.SetLineInputMoniker(p_pvarValue->bstrVal);
					/*
					hr = SVMatroxGigeDeviceParameterManager::SetParameter(l_rCamera, p_iParameterID, p_iParameterTypeID, p_pvarValue);
					// Keep track of parameters sent for reconnect logic
					SetCameraParam(l_rCamera, p_iParameterID, p_iParameterTypeID, p_pvarValue);
					*/
				}
				break;

				case SVGigeParameterInputEvent: // Internal use only, not settable via normal logic
				case SVGigeParameterInputEventName: // Internal use only, not settable via normal logic
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

HRESULT SVMatroxGige::CameraGetFormat( unsigned long p_Handle, int &p_riFormat )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		switch( l_rCamera.m_params.Format )
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
				p_riFormat = l_rCamera.m_params.Format;
				break;
			}
		}

		hr = S_OK;
	} 
	return hr;
}

HRESULT SVMatroxGige::CameraBufferCreateAll( unsigned long p_Handle, unsigned long p_ulCount )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizerHandle(p_Handle) && IsValidDigitizer(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		hr = CameraBufferDestroyAll( p_Handle );

		if ( hr == S_OK )
		{
			const SVMatroxGigeSystem& rSystem = m_Systems.Get(l_rCamera.m_SystemHandle, hr);

			hr = ReadCameraFormat(l_rCamera);

			if ( hr == S_OK )
			{
				//
				hr = l_rCamera.CreateAcquisitionBuffers(*(rSystem.m_System.get()), 
													l_rCamera.m_params.Width, 
													l_rCamera.m_params.Height,
													l_rCamera.m_params.Format);
			}
		}
		
		if ( hr != S_OK )
		{
			CameraBufferDestroyAll( p_Handle );
		}
	}
	return hr;
}

HRESULT SVMatroxGige::CameraBufferUnlockAll(unsigned long p_Handle)
{
	HRESULT hr = S_OK;

	if ( IsValidDigitizerHandle(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

HRESULT SVMatroxGige::CameraBufferDestroyAll(unsigned long p_Handle)
{
	HRESULT hr = S_OK;

	// make sure there is a valid handle to the digitizer
	if ( IsValidDigitizer(p_Handle) )
	{
		// Stop the camera if it's active
		if ( IsCameraActive(p_Handle) )
		{
			hr = CameraStop( p_Handle );
		}

		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);
		l_rCamera.DestroyAcquistionBuffers();
	}
	return hr;
}

HRESULT SVMatroxGige::CameraRegisterBufferInterface( unsigned long p_Handle, SVAcquisitionBufferInterface* p_pInterface )
{
	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizer(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		l_rCamera.m_pBufferInterface = p_pInterface;
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVMatroxGige::CameraUnregisterBufferInterface( unsigned long p_Handle )
{
	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizer(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		l_rCamera.m_pBufferInterface = NULL;
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVMatroxGige::CameraStart( unsigned long p_Handle )
{
	HRESULT hr = S_FALSE;

	// make sure there is a valid handle to the digitizer
	if ( IsValidDigitizer( p_Handle ) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

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
			if ( hr == S_OK )
			{
				hr = StartDigitizer(p_Handle, l_rCamera);
			}
			
			if ( hr != S_OK )
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

HRESULT SVMatroxGige::CameraStop( unsigned long p_Handle )
{
	HRESULT hr = S_OK;

	// make sure there is a valid handle for the digitizer
	if ( IsValidDigitizer( p_Handle ) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		if ( ::InterlockedExchange( &(l_rCamera.m_lIsStarted), 0 ) == 1 )
		{
			hr = StopDigitizer(l_rCamera);
			
			// reset to Hardware Trigger
			l_rCamera.m_params.TriggerType = SVMatroxGigeTrigger::HardwareTrigger;

			if( SVMatroxGigeDeviceParameterManager::IsParameterSupported(l_rCamera, SVGigeParameterStrobeEnable) == S_OK )
			{
				_variant_t value("Disable");

				hr = SVMatroxGigeDeviceParameterManager::SetParameter(l_rCamera, SVGigeParameterTriggerEnable, 0, &value);
			}
		}
	} 
	return hr;
}

HRESULT SVMatroxGige::StartDigitizer(unsigned long p_Handle, SVMatroxGigeDigitizer& p_rCamera)
{
	CameraBufferUnlockAll( p_Handle );

	//
	p_rCamera.m_frameStack.clear();

	//
	HRESULT hr = RegisterMatroxDigitizerHooks(p_rCamera);
	if ( hr == S_OK )
	{
		hr = SetGrabMode(p_rCamera);
	}
	if ( hr == S_OK )
	{
		hr = EnableTriggering(p_rCamera);
	}
	if ( hr == S_OK )
	{
		// Start the Asynchronous Grab (using MdigProcess)
		hr = p_rCamera.StartGrabArray(SVMatroxGige::ProcessFrame);
	}
	return hr;
}

HRESULT SVMatroxGige::StopDigitizer(SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_OK;
	if (!p_rCamera.m_Digitizer.empty())
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
		if (hr == S_OK)
		{
			// Buffer index is set and locked
			long l_lIndex = p_rCamera.m_lStartIndex;

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

HRESULT SVMatroxGige::ProcessEndFrame( SVMatroxGigeDigitizer& p_rCamera, SVMatroxIdentifier p_SrcBufferID)
{
	HRESULT hr = CameraEndFrame( p_rCamera, p_SrcBufferID );

	return hr;
}

HRESULT SVMatroxGige::CameraStartFrame( SVMatroxGigeDigitizer& p_rCamera )
{
	HRESULT hr = S_OK;

	if ( p_rCamera.m_lIsStarted == 1 && p_rCamera.m_pBufferInterface != NULL )
	{
		p_rCamera.m_StartFrameTimeStamp = p_rCamera.m_pBufferInterface->GetTimeStamp();
	}
	else
	{
		hr = S_FALSE;
	}

	return hr;
}

HRESULT SVMatroxGige::CameraEndFrame( SVMatroxGigeDigitizer& p_rCamera, SVMatroxIdentifier p_SrcBufferID )
{
	HRESULT hr = S_OK;

	SVClock::SVTimeStamp l_StartFrameTimeStamp = p_rCamera.m_StartFrameTimeStamp;

	p_rCamera.m_StartFrameTimeStamp = 0;

	if ( p_rCamera.m_lIsStarted == 1 && p_rCamera.m_pBufferInterface != NULL )
	{
		SVImageBufferInterface l_Buffer;
		
		if( p_rCamera.m_pBufferInterface->GetNextBuffer( l_Buffer ) == S_OK )
		{
			SVImageBufferHandleImage l_MilBuffer;

			if( !( l_Buffer.m_ImageHandle.empty() ) )
			{
				l_Buffer.m_ImageHandle->GetData( l_MilBuffer );
			}

			if( !( l_MilBuffer.empty() ) )
			{
				// copy the image data
				//if( p_pSrcBuffer != NULL )
				{
					SVMatroxBufferInterface::SVStatusCode l_Code = SVMatroxBufferInterface::CopyBuffer( l_MilBuffer.GetBuffer(), p_SrcBufferID );
					hr = l_Code;

				}

				if( hr == S_OK )
				{
					l_Buffer.SetStartFrameTimeStamp( l_StartFrameTimeStamp );
					l_Buffer.SetEndFrameTimeStamp();

					hr = p_rCamera.m_pBufferInterface->UpdateWithCompletedBuffer( l_Buffer );
				}
			}
			else
			{
				hr = E_FAIL;

				// For Mil to Non Mil Buffer copy
				unsigned char *l_pDstBuffer = l_Buffer.GetBufferAddress();

				if( l_pDstBuffer != NULL )
				{
					// copy the image data
					SVBitmapInfo bmpInfo;
					SVMatroxBufferInterface::SVStatusCode sc = SVMatroxBufferInterface::GetBitmapInfo(bmpInfo, p_SrcBufferID);
					if (sc == SVMEE_STATUS_OK)
					{
						sc = SVMatroxBufferInterface::CopyBuffer(bmpInfo, l_pDstBuffer, p_SrcBufferID);
						if (sc == SVMEE_STATUS_OK)
						{
							l_Buffer.SetStartFrameTimeStamp( l_StartFrameTimeStamp );
							l_Buffer.SetEndFrameTimeStamp();

							hr = p_rCamera.m_pBufferInterface->UpdateWithCompletedBuffer( l_Buffer );
						}
					}
				}
			}
		}
		else
		{
			hr = E_FAIL;
		}
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

// Just a stub, actual loading is done in SVObserver via SVGigeCameraFileReader ?
HRESULT SVMatroxGige::CameraLoadFiles(unsigned long p_ulHandle, SAFEARRAY* p_psaFileNames)
{
	HRESULT hr = S_FALSE;
	// in order to load the dcf file, we have to destroy and recreate the digitizer ?
	return hr;
}

HRESULT SVMatroxGige::ReadCameraSerialNumber(SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;
	SVMatroxString serialNo;

	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetGigeSerialNumber(*(p_rCamera.m_Digitizer.get()), serialNo);
	if (l_Code == SVMEE_STATUS_OK)
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

	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetGigeIPAddress(*(p_rCamera.m_Digitizer.get()), ipAddress);

	if ( l_Code == SVMEE_STATUS_OK )
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
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::GetFormat(*(p_rCamera.m_Digitizer.get()), format);

	if (l_Code == SVMEE_STATUS_OK)
	{
		p_rCamera.m_params.Width = format.size.cx;
		p_rCamera.m_params.Height = format.size.cy;
		p_rCamera.m_params.XOffsetPos = format.offset.x;
		p_rCamera.m_params.YOffsetPos = format.offset.y;

		switch (format.bitDepth)
		{
			case 8:
				p_rCamera.m_params.Format = SVImageFormatMono8;
				break;

			case 32:
				p_rCamera.m_params.Format = SVImageFormatRGB8888;
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

bool SVMatroxGige::IsCameraActive(unsigned long p_Handle)
{
	bool bRetVal = false;
	if ( IsValidDigitizerHandle(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);
		bRetVal = (l_rCamera.m_lIsStarted) ? true : false;
	}
	return bRetVal;
}

HRESULT SVMatroxGige::InternalTriggerEnable( unsigned long p_Handle )
{
	HRESULT hr = S_FALSE;
	if (IsValidDigitizer(p_Handle))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);

		l_rCamera.m_params.TriggerType = SVMatroxGigeTrigger::SoftwareTrigger;

		hr = S_OK;
	}
	return hr;
}

HRESULT SVMatroxGige::InternalTrigger( unsigned long p_Handle )
{
	HRESULT hr = S_FALSE;
	if (IsValidDigitizer(p_Handle))
	{
		if (m_triggerMgr.HasSubscribers(p_Handle))
		{
			hr = m_triggerMgr.Fire(p_Handle);
		}
		else
		{
			hr = FireOneShot(p_Handle);
		}
	}
	return hr;
}

HRESULT SVMatroxGige::RegisterInternalTriggerCallback( unsigned long p_Handle, SVCallbackStruct& callbackStruct )
{
	HRESULT hr = S_FALSE;

	if (IsValidDigitizer(p_Handle))
	{
		SVTriggerCallbackStruct triggerCallbackStruct; 
		triggerCallbackStruct.pCallback = callbackStruct.m_pCallback;
		triggerCallbackStruct.pOwner = callbackStruct.m_pOwner;
		triggerCallbackStruct.pData = callbackStruct.m_pData;

		m_triggerMap.Add(p_Handle, triggerCallbackStruct);
		
		typedef SVTriggerActivatorFunc<SVMatroxGige> Activator;
		typedef SVTriggerCallbackFunc<SVMatroxGige> TriggerCallback;
		SVTriggerHandler handler(p_Handle, 
								SVTriggerActivator(new Activator(this, &SVMatroxGige::FireOneShot)), 
								SVTriggerCallback(new TriggerCallback(this, &SVMatroxGige::DispatchTriggerCallback)));

		hr = m_triggerMgr.Subscribe(p_Handle, handler);
	}
	return hr;
}

HRESULT SVMatroxGige::UnregisterInternalTriggerCallback( unsigned long p_Handle, SVCallbackStruct& callbackStruct )
{
	HRESULT hr = S_FALSE;

	if (IsValidDigitizer(p_Handle))
	{
		SVTriggerCallbackStruct triggerCallbackStruct; 
		triggerCallbackStruct.pCallback = callbackStruct.m_pCallback;
		triggerCallbackStruct.pOwner = callbackStruct.m_pOwner;
		triggerCallbackStruct.pData = callbackStruct.m_pData;

		m_triggerMap.Remove(p_Handle, triggerCallbackStruct);
		hr = m_triggerMgr.Unsubscribe( p_Handle );
	}
	return hr;
}

HRESULT SVMatroxGige::UnregisterAllInternalTriggerCallbacks( unsigned long p_Handle )
{
	HRESULT hr = S_FALSE;

	if (IsValidDigitizer(p_Handle))
	{
		m_triggerMap.RemoveAll(p_Handle);
		hr = m_triggerMgr.Unsubscribe( p_Handle );
	}
	return hr;
}

HRESULT SVMatroxGige::FireOneShot( unsigned long p_Handle )
{
	HRESULT hr = S_FALSE;

	if ( IsValidDigitizer(p_Handle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_Handle);
		
		hr = SVMatroxDigitizerInterface::SetFeature(*(l_rCamera.m_Digitizer.get()), "TriggerSoftware", SVMatroxDigitizerFeature::SVTypeCommand, _variant_t(0L));
	}
	return hr;
}

HRESULT SVMatroxGige::DispatchTriggerCallback( unsigned long p_Handle )
{
	HRESULT hr = m_triggerMap.Dispatch( p_Handle );
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
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameStart, SVMatroxGige::DigitizerStartFrameCallback, (void *)&p_rCamera);
		
	if (l_Code == SVMEE_STATUS_OK)
	{
		l_Code = SVMatroxDigitizerInterface::SetHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameEnd, SVMatroxGige::DigitizerEndFrameCallback, (void *)&p_rCamera);
	}
	if (l_Code == SVMEE_STATUS_OK)
	{
		if (p_rCamera.HasLineInput())
		{
			l_Code = SVMatroxDigitizerInterface::SetHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGigeEvent, SVMatroxGige::LineEdgeEventCallback, (void *)&p_rCamera);
			if (l_Code == SVMEE_STATUS_OK)
			{
				// Enable Gige Event(s)
				l_Code = EnableGigeEvents(p_rCamera);
			}
		}
	}
	return l_Code;
}

HRESULT SVMatroxGige::UnRegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera)
{
	// Unregister Matrox Hook callback
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;
	if (!p_rCamera.m_Digitizer.empty())
	{
		l_Code = SVMatroxDigitizerInterface::ReleaseHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameEnd, &SVMatroxGige::DigitizerEndFrameCallback, (void *)&p_rCamera);
		l_Code = SVMatroxDigitizerInterface::ReleaseHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameStart, &SVMatroxGige::DigitizerStartFrameCallback, (void *)&p_rCamera);

		if (p_rCamera.IsAcquisitionTriggered())
		{
			l_Code = SVMatroxDigitizerInterface::ReleaseHookFunction(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGigeEvent, SVMatroxGige::LineEdgeEventCallback, (void *)&p_rCamera);

			// Disable Gige Event(s)
			l_Code = DisableGigeEvents(p_rCamera);
		}
	}
	return l_Code;
}

HRESULT SVMatroxGige::EnableGigeEvents(const SVMatroxGigeDigitizer& p_rCamera)
{
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;
	// Enable the GIGE Event(s)
	const SVMatroxString& risingName =  p_rCamera.GetLineInputRisingEventName();
	const SVMatroxString& fallingName = p_rCamera.GetLineInputFallingEventName();

	if (!risingName.empty() && !fallingName.empty())
	{
		const SVGigeDeviceParameterStruct& param = p_rCamera.GetFeature(SVGigeParameterInputEvent);
		SVGigeFeatureString featureValue;
		param.accessor.feature.GetGigeFeatureString("Enable", featureValue);

		l_Code = SVMatroxDigitizerInterface::SetGigeEvent(*(p_rCamera.m_Digitizer.get()), risingName.c_str(), featureValue.c_str());
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetGigeEvent(*(p_rCamera.m_Digitizer.get()), fallingName.c_str(), featureValue.c_str());
		}
	}
	else
	{
		l_Code = SVMEE_INVALID_PARAMETER;
	}
	return l_Code;
}

HRESULT SVMatroxGige::DisableGigeEvents(const SVMatroxGigeDigitizer& p_rCamera)
{
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMEE_STATUS_OK;
	// Disable the GIGE Event(s)
	const SVMatroxString& risingName =  p_rCamera.GetLineInputRisingEventName();
	const SVMatroxString& fallingName = p_rCamera.GetLineInputFallingEventName();
	if (!risingName.empty() && !fallingName.empty())
	{
		const SVGigeDeviceParameterStruct& param = p_rCamera.GetFeature(SVGigeParameterInputEvent);
		SVGigeFeatureString featureValue;
		param.accessor.feature.GetGigeFeatureString("Disable", featureValue);
		l_Code = SVMatroxDigitizerInterface::SetGigeEvent(*(p_rCamera.m_Digitizer.get()), risingName.c_str(), featureValue.c_str());
		if (l_Code == SVMEE_STATUS_OK)
		{
			l_Code = SVMatroxDigitizerInterface::SetGigeEvent(*(p_rCamera.m_Digitizer.get()), fallingName.c_str(), featureValue.c_str());
		}
	}
	else
	{
		l_Code = SVMEE_INVALID_PARAMETER;
	}
	return l_Code;
}

HRESULT SVMatroxGige::SetGrabMode(const SVMatroxGigeDigitizer& p_rCamera)
{
	// Set Grab Timeout to Infinite
	SVMatroxDigitizerInterface::SVStatusCode l_Code = SVMatroxDigitizerInterface::SetInfiniteGrabTimeout(*(p_rCamera.m_Digitizer.get()));
	if (l_Code == SVMEE_STATUS_OK)
	{
		// Set Grab Mode to Asynchronous
		l_Code = SVMatroxDigitizerInterface::SetGrabMode(*(p_rCamera.m_Digitizer.get()), SVMatroxDigitizerGrab::SVGrabAsynchronous);
	}
	return l_Code;
}

HRESULT SVMatroxGige::EnableTriggering(const SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT hr = S_FALSE;

	const SVMatroxDigitizer& rDigitizer = *(p_rCamera.m_Digitizer.get());

	if (p_rCamera.m_params.TriggerType != SVMatroxGigeTrigger::SoftwareTrigger)
	{
		_variant_t value("External Trigger");
		hr = SVMatroxGigeDeviceParameterManager::SetParameter(p_rCamera, SVGigeParameterTriggerSource, 0, &value);
	}
	else
	{
		_variant_t value("Software Trigger");
		hr = SVMatroxGigeDeviceParameterManager::SetParameter(p_rCamera, SVGigeParameterTriggerSource, 0, &value);
	}

	if (hr == S_OK)
	{
		_variant_t value("Enable");
		hr = SVMatroxGigeDeviceParameterManager::SetParameter(p_rCamera, SVGigeParameterTriggerEnable, 0, &value);
	}
	return hr;
}

// Camera Detection - Destroy and recreate System(s), enumerate available digitizers per system
void SVMatroxGige::ScanForCameras()
{
	DestroySystems();
	CreateSystems();
}

const SVMatroxGigeDigitizer& SVMatroxGige::GetDigitizer(unsigned long p_Handle) const
{
	HRESULT hr = S_FALSE;

	unsigned char systemHandle = SV_EXTRACT_MATROXGIGE_SYSTEM_HANDLE(p_Handle);
	unsigned char digitizerHandle = SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(p_Handle);

	const SVMatroxGigeSystem& rSystem = m_Systems.Get(systemHandle, hr);
	return rSystem.GetDigitizer(digitizerHandle, hr);
}

SVMatroxGigeDigitizer& SVMatroxGige::GetDigitizer(unsigned long p_Handle)
{
	HRESULT hr = S_FALSE;

	unsigned char systemHandle = SV_EXTRACT_MATROXGIGE_SYSTEM_HANDLE(p_Handle);
	unsigned char digitizerHandle = SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(p_Handle);

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
SVMatroxIdentifier SVMatroxGige::CameraPresentCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext )
{
	unsigned char systemHandle = reinterpret_cast<unsigned char>(p_pvContext);
	HRESULT hr = S_OK;
	SVMatroxGigeSystem& system = g_svTheApp.m_svSystem.m_Systems.Get(systemHandle, hr);
	const SVMatroxSystem& l_rMatroxSystem = *(system.m_System.get());

	// Inquire the camera present state (present or not present).
	long IsCamPresent = 0;
	SVMatroxSystemInterface::SVStatusCode statusCode = SVMatroxSystemInterface::GetHookInfo(l_rMatroxSystem, EventId, SVMatroxSystemHookInfo::SVCameraPresent, IsCamPresent);
	if (statusCode == SVMEE_STATUS_OK)
	{
		// Inquire the camera's digitizer device number.
		long deviceNumber = 0;
		statusCode = SVMatroxSystemInterface::GetHookInfo(l_rMatroxSystem, EventId, SVMatroxSystemHookInfo::SVCameraNumber, deviceNumber);
		if (statusCode == SVMEE_STATUS_OK)
		{
			if (IsCamPresent)
			{
				// log an exception
				CString l_csbuf;
				l_csbuf.Format("CameraPresentCallback - Connect - Device %d", deviceNumber);

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVMATROXGIGE_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

				g_svTheApp.m_svSystem.HandleConnect(system, static_cast<unsigned char>(deviceNumber));
			}
			else
			{
				// log an exception
				CString l_csbuf;
				l_csbuf.Format("CameraPresentCallback - Disconnect - Device %d", deviceNumber);

				SVException l_svLog;

				SETEXCEPTION1( l_svLog, SVMSG_SVMATROXGIGE_NO_ERROR, l_csbuf );
				l_svLog.LogException( l_csbuf );

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
	bool bHasCamera = (hr == S_OK) ? true : false;
	
	if (bHasCamera)
	{
		SVMatroxGigeDigitizer& l_rCamera = p_rSystem.GetDigitizer(SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(handle), hr);
		if (hr == S_OK)
		{
			if (l_rCamera.m_Digitizer.empty())
			{
				// Allocate, recreate buffers, send camera parameters and start acquire
				l_rCamera.m_Digitizer = new SVMatroxDigitizer;
				hr = AllocDigitizer(p_rSystem, deviceNumber, l_rCamera);
			
				if (hr == S_OK)
				{
					if (l_rCamera.m_lIsStarted)
					{
						// Set the Camera Parameters before recreating buffers and restarting acquisition
						hr = m_trackedCameraParameters.ReloadCameraParams(l_rCamera.m_svCameraId.m_serialNo, l_rCamera);
						if (hr == S_OK)
						{
							hr = ReadCameraFormat(l_rCamera);

							if (hr == S_OK)
							{
								//
								hr = l_rCamera.CreateAcquisitionBuffers(*(p_rSystem.m_System.get()), 
																l_rCamera.m_params.Width, 
																l_rCamera.m_params.Height,
																l_rCamera.m_params.Format);
						
								if (hr == S_OK)
								{
									hr = StartDigitizer(handle, l_rCamera);
									if (hr != S_OK)
									{
										// log an exception
										CString l_csbuf;
										l_csbuf.Format("HandleConnect - StartDigitizer Error - Device %d", deviceNumber);

										SVException l_svLog;

										SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_RECONNECT_ERROR, l_csbuf );
										l_svLog.LogException( l_csbuf );
									}
								}
								else
								{
									// log an exception
									CString l_csbuf;
									l_csbuf.Format("HandleConnect - Reload Camera Parameters Error (%d) - Device %d", hr, deviceNumber);

									SVException l_svLog;

									SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_RECONNECT_ERROR, l_csbuf );
									l_svLog.LogException( l_csbuf );
								}
							}
							else
							{
								// log an exception
								CString l_csbuf;
								l_csbuf.Format("HandleConnect - CreateAcquisitionBuffers Error (%d) - Device %d", hr, deviceNumber);

								SVException l_svLog;

								SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_RECONNECT_ERROR, l_csbuf );
								l_svLog.LogException( l_csbuf );
							}
						}
						else
						{
							// log an exception
							CString l_csbuf;
							l_csbuf.Format("HandleConnect - ReadCameraFormat Error (%d) - Device %d", hr, deviceNumber);

							SVException l_svLog;

							SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_RECONNECT_ERROR, l_csbuf );
							l_svLog.LogException( l_csbuf );
						}
					}
				}
				else
				{
					// log an exception
					CString l_csbuf;
					l_csbuf.Format("HandleConnect - AllocDigitizer Error (%d) - Device %d", hr, deviceNumber);

					SVException l_svLog;

					SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_RECONNECT_ERROR, l_csbuf );
					l_svLog.LogException( l_csbuf );
				}
			}
		}
		else
		{
			// log an exception
			CString l_csbuf;
			l_csbuf.Format("HandleConnect - GetDigitizer Error (%d) - Device %d", hr, deviceNumber);

			SVException l_svLog;

			SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_RECONNECT_ERROR, l_csbuf );
			l_svLog.LogException( l_csbuf );
		}
	}
	else
	{
		// log an exception
		CString l_csbuf;
		l_csbuf.Format("HandleConnect - FindCamera Error (%d) - Device %d", hr, deviceNumber);

		SVException l_svLog;

		SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_RECONNECT_ERROR, l_csbuf );
		l_svLog.LogException( l_csbuf );
	}
}

void SVMatroxGige::HandleDisconnect(SVMatroxGigeSystem& p_rSystem, long deviceNumber)
{
	// Get Digitizer
	unsigned long handle = 0;
	HRESULT hr = FindCamera(p_rSystem, deviceNumber, handle);

	if (hr == S_OK)
	{
		SVMatroxGigeDigitizer& l_rCamera = p_rSystem.GetDigitizer(SV_EXTRACT_MATROXGIGE_DIGITIZER_HANDLE(handle), hr);
		if (hr == S_OK)
		{
			if (l_rCamera.m_lIsStarted)
			{
				hr = StopDigitizer(l_rCamera);
			}
			// destroy matrox acquisition buffers
			l_rCamera.DestroyAcquistionBuffers();

			// destroy matrox digitizer handle
			DestroyDigitizer(l_rCamera);
			l_rCamera.m_Digitizer.clear();
		}
		else
		{
			// log an exception
			CString l_csbuf;
			l_csbuf.Format("HandleDisconnect - GetDigitizer Error (%d) - Device %d", hr, deviceNumber);

			SVException l_svLog;

			SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_DISCONNECT_ERROR, l_csbuf );
			l_svLog.LogException( l_csbuf );
		}
	}
	else
	{
		// log an exception
		CString l_csbuf;
		l_csbuf.Format("HandleDisconnect - FindCamera Error (%d) - Device %d", hr, deviceNumber);

		SVException l_svLog;

		SETEXCEPTION1( l_svLog, SVMSG_MATROX_GIGE_DISCONNECT_ERROR, l_csbuf );
		l_svLog.LogException( l_csbuf );
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

HRESULT SVMatroxGige::TriggerGetCount(unsigned long& p_ulCount)
{
	HRESULT l_Result = CameraGetCount(p_ulCount);
	return l_Result;
}

// convert ordinal to handle
unsigned long SVMatroxGige::TriggerGetHandle(unsigned long p_ulIndex)
{
	return GetDigitizerHandle(p_ulIndex); 
}

HRESULT SVMatroxGige::TriggerGetName(unsigned long p_ulHandle, BSTR& p_rbstrName)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		if ( p_rbstrName != NULL )
		{
			::SysFreeString( p_rbstrName );
			p_rbstrName = NULL;
		}

		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		SVString name;
		name.Format("CameraTrigger.Dig_%d", l_rCamera.GetDeviceNumber());
		p_rbstrName = name.ToBSTR();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVMatroxGige::TriggerRegisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.SetTriggerCallback(p_Callback);
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVMatroxGige::TriggerUnregisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVMatroxGige::TriggerUnregisterAllCallbacks(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.ClearTriggerCallback();
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVMatroxGige::TriggerStart(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.m_params.bAcquisitionTriggered = true;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVMatroxGige::TriggerStop(unsigned long p_ulHandle)
{
	HRESULT l_Result = S_FALSE;
	if (IsValidDigitizer(p_ulHandle))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.m_params.bAcquisitionTriggered = false;
		l_Result = S_OK;
	}
	return l_Result;
}

HRESULT SVMatroxGige::TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount )
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

HRESULT SVMatroxGige::TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
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

HRESULT SVMatroxGige::TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( ::VariantClear( p_pvarValue ) == S_OK )
		{
			if (IsValidDigitizer(p_ulHandle))
			{
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

HRESULT SVMatroxGige::TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
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
						l_hrOk = SetAcquisitionTriggered( p_ulHandle, (p_pvarValue->boolVal == VARIANT_TRUE) ? true : false );
					}
					break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVMatroxGige::IsAcquisitionTriggered(unsigned long p_ulHandle, bool& bAcquisitionTriggered) const
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		l_hrOk = S_OK;
		const SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		bAcquisitionTriggered = l_rCamera.IsAcquisitionTriggered();
	}
	return l_hrOk;
}

HRESULT SVMatroxGige::SetAcquisitionTriggered(unsigned long p_ulHandle, bool bAcquisitionTriggered)
{
	HRESULT l_hrOk = S_FALSE;

	if (IsValidDigitizer(p_ulHandle))
	{
		l_hrOk = S_OK;
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(p_ulHandle);
		l_rCamera.SetAcquisitionTriggered(bAcquisitionTriggered);
	}
	return l_hrOk;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGige.cpp_v  $
 * 
 *    Rev 1.6   01 Dec 2014 12:28:58   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread name and attribute for SVThreadManager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   12 Jun 2014 13:08:24   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  871
 * SCR Title:  Create Additional Visual Studio Solutions
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Removed duplicate global functions located in SVOMFCLibrary. Should use functions from SVUtilityLibrary.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   02 Oct 2013 11:52:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platforms.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Sep 2013 12:35:42   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  855
 * SCR Title:  Fix GigE Camreas so they can be used on two different PPQs
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified CameraStart to return S_OK if camera is already started.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   12 Jun 2013 15:13:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.16   22 May 2013 10:16:50   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   AllocDigitizer - will now ReadCameraIPAddress, added new method ReadCameraIPAddress
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:22:44   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   29 Apr 2013 13:04:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:37:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   01 Apr 2013 16:00:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  820
 * SCR Title:  Baumer Camera Trigger Debounce
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised CameraGetParameterName method 
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   21 Jan 2013 09:59:14   sjones
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
 *    Rev 1.12   14 Aug 2012 15:38:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed include statements to allow the new MIL 9.0 functionality to be available.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   18 Jul 2012 12:35:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow gige to be compiled with both MIL 8.0 and 9.0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   18 Jul 2012 11:51:50   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow gige to be compiled with both MIL 8.0 and 9.0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   29 Jun 2012 16:43:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   10 Feb 2011 13:54:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to now use shared pointer instead of untracked structure.  This fixes destruction, tracking and rebuilding issues.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   27 Jan 2011 10:21:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jul 2010 13:50:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source to foce the trigger edge parameter and strobe edge parameter to be updated correctly.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   02 Jun 2010 08:01:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Feb 2010 10:00:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change thread creation location.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Oct 2009 09:24:44   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to correct issues from 5.30  updates
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 09:04:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Fixed issues with moved header includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 07:50:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new buffer management and buffer transfer methodologies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:17:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
