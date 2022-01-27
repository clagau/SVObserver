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
#include "SVMatroxLibrary/SVMatroxSystemInterface.h"
#include "Definitions/StringTypeDef.h"
#include "SVUtilityLibrary/StringHelper.h"
#include "SVMatroxGigeDeviceParameterManager.h"
#include "SVImageLibrary/SVAcquisitionBufferInterface.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferInterface.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

SVMatroxGige g_matroxAcqDevice;

// helpers for System/Digitizer Handles
#define SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE( SystemHandle, DigitizerHandle ) \
	( static_cast<unsigned long>( (SystemHandle) ) << 8 ) | static_cast<unsigned char>( (DigitizerHandle) & 0xFF )

SVMatroxGige::~SVMatroxGige()
{
	Destroy(true);
}

HRESULT SVMatroxGige::Create()
{
	HRESULT result{ S_OK };

	if ( ::InterlockedIncrement( &m_lRefCount ) == 1 )
	{
		long sysCount {0L};
		//This is to check if Matrox Startup has already been called if not then it shall be called by the dll
		if (SVMEE_INVALID_HANDLE == SVMatroxApplicationInterface::GetSystemCount(sysCount))
		{
			SVMatroxApplicationInterface::Startup();
			m_DllMatroxStartup = true;
		}
		result = CreateSystems();

		if (S_OK != result)
		{
			Destroy( true );
		}
	}

	return result;
}

HRESULT SVMatroxGige::Destroy( bool close /*=false*/)
{
	if ( close || ::InterlockedDecrement( &m_lRefCount ) <= 0 )
	{
		::InterlockedExchange( &m_lRefCount, 0 );

		DestroySystems();
		//If dll called startup then in needs to call shutdown
		if (m_DllMatroxStartup)
		{
			SVMatroxApplicationInterface::Shutdown();
		}
	}

	return S_OK;
}

unsigned long SVMatroxGige::GetDigitizerHandle(long index) const
{
	unsigned long handle = 0;

	// index is zero based and must be equal to or greater than zero
	if (index >= 0)
	{
		// iterate thru systems and associated digitizers
		for (SVMatroxGigeSystemList::const_iterator it = m_Systems.begin(); it != m_Systems.end(); ++it)
		{
			const SVMatroxGigeDigitizerList& list = it->second.GetDigitizerList();

			// check if digitizer belongs to this system
			if (index < static_cast<long>(list.size()))
			{
				SVMatroxGigeDigitizerList::const_iterator digIt = list.begin();
				std::advance(digIt, index);

				handle = SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE(it->first, digIt->first);
				break;
			}
			else
			{
				index -= static_cast<long> (list.size());
			}
		}
	}
	return handle;
}

unsigned long SVMatroxGige::CameraGetCount() const
{
	return std::accumulate(m_Systems.begin(), m_Systems.end(), 0, 
		[](long sum, const auto& rEntry) { return std::plus<long>()(sum, static_cast<unsigned long> (rEntry.second.GetDigitizerList().size())); });
}

_variant_t SVMatroxGige::CameraGetName(unsigned long digitizerHandle) const
{
	_variant_t result;

	if (IsValidDigitizerHandle(digitizerHandle))
	{
		const SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);

		result.SetString(rCamera.m_FullName.c_str());
	} 
	return result;
}

unsigned long SVMatroxGige::CameraGetWidth(unsigned long digitizerHandle) const
{
	unsigned long result{ 0UL };

	if (IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle))
	{
		const SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);
		SIZE size;
		if (S_OK == SVMatroxDigitizerInterface::GetSize(*(rCamera.m_Digitizer.get()), size.cx, size.cy))
		{
			result = static_cast<unsigned long> (size.cx);
		}
	}

	return result;
}

unsigned long SVMatroxGige::CameraGetHeight( unsigned long digitizerHandle) const
{
	unsigned long result{ 0UL };

	if (IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle))
	{
		const SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);
		SIZE size;
		if (S_OK == SVMatroxDigitizerInterface::GetSize(*(rCamera.m_Digitizer.get()), size.cx, size.cy))
		{
			result = static_cast<unsigned long> (size.cy);
		}
	}

	return result;
}

int SVMatroxGige::CameraGetFormat(unsigned long digitizerHandle) const
{
	int result{ SvDef::SVImageFormatUnknown };

	if (IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle))
	{
		const SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);

		switch (rCamera.m_params.Format)
		{
		case SvDef::SVImageFormatBGR888:
		case SvDef::SVImageFormatBGR888X:
		{
			result = SvDef::SVImageFormatBGR888X;
			break;
		}
		default:
		{
			result = rCamera.m_params.Format;
			break;
		}
		}
	}
	return result;
}

HRESULT SVMatroxGige::CameraBufferCreateAll(unsigned long digitizerHandle)
{
	HRESULT result{ E_FAIL };

	if (IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle))
	{
		SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);

		result = CameraBufferDestroyAll(digitizerHandle);

		if (S_OK == result)
		{
			const SVMatroxGigeSystem& rSystem = m_Systems.Get(rCamera.m_SystemHandle, result);

			result = ReadCameraFormat(rCamera);

			if (S_OK == result)
			{
				result = rCamera.CreateAcquisitionBuffers(*(rSystem.m_System.get()),
					rCamera.m_params.Width,
					rCamera.m_params.Height,
					rCamera.m_params.Format);
			}
		}

		if (S_OK != result)
		{
			CameraBufferDestroyAll(digitizerHandle);
		}
	}
	return result;
}

HRESULT SVMatroxGige::CameraBufferDestroyAll(unsigned long digitizerHandle)
{
	HRESULT hr = S_OK;

	// make sure there is a valid handle to the digitizer
	if (IsValidDigitizer(digitizerHandle))
	{
		// Stop the camera if it's active
		if (IsCameraActive(digitizerHandle))
		{
			hr = CameraStop(digitizerHandle);
		}

		SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);
		rCamera.DestroyAcquistionBuffers();
	}
	return hr;
}

_variant_t SVMatroxGige::CameraGetParameterName( unsigned long digitizerHandle, int parameterID) const
{
	_variant_t result;

	if ( IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle) )
	{
		const SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);
		result = SVMatroxGigeDeviceParameterManager::GetParameterName(rCamera, parameterID);
	}

	return result;
}

_variant_t SVMatroxGige::CameraGetParameter(unsigned long digitizerHandle, int parameterID) const
{
	_variant_t result;

	if( IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle))
	{
		const SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);
		result = SVMatroxGigeDeviceParameterManager::GetParameter(rCamera, parameterID);
	}

	return result;
}

HRESULT SVMatroxGige::CameraSetParameter(unsigned long digitizerHandle, int parameterID, const _variant_t& rValue)
{
	HRESULT result{ E_FAIL };
	
	if (IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle))
	{
		SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);

		switch (parameterID)
		{
			case  SvDef::SVGigeParameterFeatureOverrides:
			{
				_bstr_t xmlData{ rValue };
				result = rCamera.BuildGigeFeaturesMap(xmlData);
				break;
			}
			case  SvDef::SVGigeBeginTrackParameters:
			{
				TrackCameraParams(rCamera);
				result = S_OK;
				break;
			}
			case  SvDef::SVGigeEndTrackParameters:
			{
				LockMainCameraParamList(rCamera);
				result = S_OK;
				break;
			}
			case  SvDef::SVGigeParameterLineInput:
			{
				rCamera.SetLineInputMoniker(SvUl::createStdString(rValue));
				result = S_OK;
				break;
			}
			case  SvDef::SVGigeParameterInputEvent: // Internal use only, not settable via normal logic
			case  SvDef::SVGigeParameterInputEventName: // Internal use only, not settable via normal logic
			{
				break;
			}
			case SvDef::SVTriggerType:
			{
				rCamera.m_params.TriggerType = static_cast<SvDef::TriggerType> (rValue.lVal);
				break;
			}

			default:
			{
				result = SVMatroxGigeDeviceParameterManager::SetParameter(rCamera, parameterID, rValue);

				// Keep track of parameters sent for reconnect logic
				SetCameraParam(rCamera, parameterID, rValue);
			}
			break;
		}
	} 
	return result;
}

_variant_t SVMatroxGige::CameraGetParameterList(unsigned long digitizerHandle) const
{
	_variant_t result;

	if (IsValidDigitizerHandle(digitizerHandle) && IsValidDigitizer(digitizerHandle))
	{
		SAFEARRAYBOUND arrayBound[1];
		arrayBound[0].cElements = SvDef::SVGigeParameterSize;
		arrayBound[0].lLbound = 0;

		result.vt = VT_ARRAY | VT_I4;
		result.parray = ::SafeArrayCreate(VT_I4, 1, arrayBound);

		for (long i = 0; i < SvDef::SVGigeParameterSize; i++)
		{
			::SafeArrayPutElement(result.parray, &i, reinterpret_cast<void*> (&i));
		}
	}

	return result;
}

HRESULT SVMatroxGige::CameraRegisterBufferInterface( unsigned long digitizerHandle, SVAcquisitionBufferInterface* pInterface )
{
	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizer(digitizerHandle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(digitizerHandle);

		l_rCamera.m_pBufferInterface = pInterface;
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVMatroxGige::CameraUnregisterBufferInterface( unsigned long digitizerHandle )
{
	HRESULT l_hrOk = S_OK;

	if ( IsValidDigitizer(digitizerHandle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(digitizerHandle);

		l_rCamera.m_pBufferInterface = nullptr;
	}
	else
	{
		l_hrOk = S_FALSE;
	}

	return l_hrOk;
}

HRESULT SVMatroxGige::CameraStart( unsigned long digitizerHandle )
{
	HRESULT hr = S_FALSE;

	// make sure there is a valid handle to the digitizer
	if ( IsValidDigitizer(digitizerHandle) )
	{
		SVMatroxGigeDigitizer& rCamera = GetDigitizer(digitizerHandle);

		if ( ::InterlockedExchange( &(rCamera.m_lIsStarted), 1 ) == 0 )
		{
			hr = S_OK;
			if (rCamera.HasLineInput())
			{
				if (!rCamera.IsGigeEventListValid())
				{
					hr = -123456;
				}
			}
			if ( S_OK == hr )
			{
				variant_t triggerSource = CameraGetParameter(digitizerHandle, SvDef::SVGigeParameterTriggerSource);
				hr = StartDigitizer(rCamera, triggerSource);
			}
			
			if ( S_OK != hr )
			{
				::InterlockedExchange( &(rCamera.m_lIsStarted), 0 );
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

HRESULT SVMatroxGige::CameraStop( unsigned long digitizerHandle )
{
	HRESULT hr = S_OK;

	// make sure there is a valid handle for the digitizer
	if ( IsValidDigitizer(digitizerHandle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(digitizerHandle);

		if ( ::InterlockedExchange( &(l_rCamera.m_lIsStarted), 0 ) == 1 )
		{
			hr = StopDigitizer(l_rCamera);
			
			// reset to Hardware Trigger
			l_rCamera.m_params.TriggerType = SvDef::TriggerType::HardwareTrigger;

			if( S_OK == SVMatroxGigeDeviceParameterManager::IsParameterSupported(l_rCamera, SvDef::SVGigeParameterStrobeEnable) )
			{
				_variant_t value("Disable");
				hr = SVMatroxGigeDeviceParameterManager::SetParameter(l_rCamera, SvDef::SVGigeParameterTriggerEnable, value);
			}
		}
	}
	return hr;
}

HRESULT SVMatroxGige::InternalTrigger( unsigned long digitizerHandle)
{
	HRESULT hr = S_FALSE;
	if ( IsValidDigitizer(digitizerHandle) )
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(digitizerHandle);
		
		hr = SVMatroxDigitizerInterface::SetFeature(*(l_rCamera.m_Digitizer.get()), "TriggerSoftware", SVMatroxDigitizerFeature::SVTypeCommand, _variant_t());
	}
	return hr;
}

// Camera Detection - Destroy and recreate System(s), enumerate available digitizers per system
void SVMatroxGige::ScanForCameras()
{
	DestroySystems();
	CreateSystems();
}

void SVMatroxGige::ProcessStartFrame(SVMatroxGigeDigitizer* pCamera)
{
	if (nullptr != pCamera)
	{
		if (pCamera->m_lIsStarted == 1)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
			::OutputDebugString("Gige Start Frame\n");
#endif
			long l_lIndex = (pCamera->m_lLastUsedIndex + 1) % 16;

			pCamera->m_lStartIndex = l_lIndex;
			pCamera->m_lLastUsedIndex = l_lIndex;
			// Keep Track of Start Frames
			pCamera->m_frameStack.Increment();

			if (nullptr != pCamera->m_pBufferInterface)
			{
				//When the time stamp is not 0 then a new start frame has been received before the last end frame!
				//This will cause a NAK! 
				if (0 != pCamera->m_StartFrameTimeStamp)
				{
					// log an exception
					SvDef::StringVector msgList;
					msgList.emplace_back(pCamera->m_FullName);
					msgList.emplace_back(SvUl::Format(_T("%.3f"), (SvUl::GetTimeStamp() - pCamera->m_StartFrameTimeStamp) * SvUl::c_MicrosecondsPerMillisecond));
					SvStl::MessageManager Exception(SvStl::MsgType::Log);
					Exception.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_NAK_Error_MissingEndFrame, msgList, SvStl::SourceFileParams(StdMessageParams));
				}
				pCamera->m_StartFrameTimeStamp = SvUl::GetTimeStamp();
			}

#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
			std::string outputText = SvUl::Format(_T("Camera Start Frame Callback - Camera %d-%d Buffer %d\n"),
				pCamera->m_SystemHandle,
				pCamera->m_Handle,
				pCamera->m_lStartIndex);
			::OutputDebugString(outputText.c_str());
#endif
		}
	}
}

void SVMatroxGige::ProcessEndFrame(SVMatroxGigeDigitizer* pCamera, __int64 HookId)
{
	if (nullptr != pCamera)
	{
		if (pCamera->m_lIsStarted == 1)
		{
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
			::OutputDebugString("Gige End Frame\n");
#endif
			if (pCamera->m_frameStack.get() == 1)
			{
				// check for Corrupted first
				bool bIsCorrupt = false;
				HRESULT l_Code = SVMatroxDigitizerInterface::IsCorruptedFrame(HookId, bIsCorrupt);
				if (l_Code == S_OK && false == bIsCorrupt)
				{
					__int64 ModifiedBufferId(0);

					// Retrieve the MIL_ID of the grabbed buffer.
					l_Code = SVMatroxDigitizerInterface::GetGrabBuffer(HookId, ModifiedBufferId);

					if (l_Code == S_OK && ModifiedBufferId)
					{
						//Save end frame time stamp before buffer copy
						double endFrameTimeStamp = SvUl::GetTimeStamp();
						double startFrameTimeStamp = pCamera->m_StartFrameTimeStamp;

						pCamera->m_StartFrameTimeStamp = 0.0;

						if (nullptr != pCamera->m_pBufferInterface)
						{
							SvOi::ITRCImagePtr pImage = pCamera->m_pBufferInterface->GetNextBuffer();
							if (nullptr != pImage)
							{
								auto pImageHandle = pImage->getHandle();
								if (nullptr != pImageHandle && !(pImageHandle->empty()))
								{
									SVMatroxBufferInterface::CopyBuffer(pImageHandle->GetBuffer(), ModifiedBufferId);
								}
							}

							//Send this command also if buffer failed to trigger the PPQ-Thread to give it a change for cleanup.
							pCamera->m_pBufferInterface->UpdateWithCompletedBuffer(pImage, startFrameTimeStamp, endFrameTimeStamp);
						}
					}
#if defined (TRACE_THEM_ALL) || defined (TRACE_FAILURE)
					else
					{
						::OutputDebugString("Corrupt Frame\n");
					}
#endif
				}
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
				std::string outputText = SvUl::Format(_T("Process End Frame Callback - Camera %d-%d\n"), pCamera->m_SystemHandle, pCamera->m_Handle);
				::OutputDebugString(outputText.c_str());
#endif
			}
#if defined (TRACE_THEM_ALL) || defined (TRACE_MATROXGIGE)
			else
			{
				std::string outputText = SvUl::Format(_T("Start Frame Count %d\n"), pCamera->m_frameStack.get());
				::OutputDebugString(outputText.c_str());
			}
#endif
			pCamera->m_frameStack.clear();
		}
	}
}

HRESULT SVMatroxGige::CreateSystems()
{
	HRESULT result{ S_OK };
	long l_lSystemCount = 0;

	HRESULT l_Code = SVMatroxApplicationInterface::GetSystemCount(l_lSystemCount);
	if (l_Code == S_OK)
	{
		long gigeSystemID = 0;
		long soliosGigeSystemID = 0;

		for (long i = 0; i < l_lSystemCount; i++)
		{
			std::string Name;

			l_Code = SVMatroxApplicationInterface::GetSystemName(i, Name);

			if (l_Code == S_OK)
			{
				// Check for GIGE system
				if (std::string::npos != Name.find(_T("GIGE")))
				{
					// Check for Solios GIGE
					if (std::string::npos != Name.find(_T("SOLIOS")))
					{
						AddSystem(Name, soliosGigeSystemID++);
					}
					else // GIGE system (dongle ?)
					{
						l_Code = AddSystem(Name, gigeSystemID++);
						if (S_OK != l_Code)
						{
							result = l_Code;
						}
					}
				}
			}
			else
			{
				result = l_Code;
			}
		}
		// if no cameras are attached, then the system allocation fails
		// Not sure that is an error per se...
		//hr = m_Systems.size() ? S_OK : S_FALSE;
	}
	else
	{
		result = l_Code;
	}
	return result;
}

HRESULT SVMatroxGige::DestroySystems()
{
	HRESULT result{ S_OK };

	SVMatroxGigeSystemList::iterator it;
	for (it = m_Systems.begin(); it != m_Systems.end(); ++it)
	{
		DestroyDigitizers(it->second);

		result = DestroySystem(it->second);
	}
	m_Systems.clear();

	return result;
}

HRESULT SVMatroxGige::DestroySystem(SVMatroxGigeSystem& rSystem)
{
	HRESULT result{ S_OK };

	// Unregister camera present hook
	/*HRESULT l_Code = */SVMatroxSystemInterface::ReleaseHookFunction(*(rSystem.m_System.get()), SVMatroxSystemHook::SVCameraPresent, SVMatroxGige::CameraPresentCallback, reinterpret_cast<void*> (&rSystem.m_Handle));
	result = SVMatroxSystemInterface::Destroy(*(rSystem.m_System.get()));
	return result;
}

HRESULT SVMatroxGige::AddSystem(const std::string& rName, long SystemNumber)
{
	HRESULT result{ S_OK };

	// Allocate the Matrox System
	SVMatroxGigeSystem l_System;
	l_System.m_System = SVMatroxSystemRef{ new SVMatroxSystem };
	l_System.m_Name = _T("Matrox_GIGE");
	l_System.m_matroxSystemName = rName;

	// 
	HRESULT l_Code = SVMatroxSystemInterface::Allocate(rName, SystemNumber, *(l_System.m_System.get()));
	if (l_Code == S_OK)
	{
		result = m_Systems.Add(l_System);

		if (S_OK == result)
		{
			SVMatroxGigeSystem& rSystem = m_Systems.Get(l_System.m_Handle, result);
			if (S_OK == result)
			{
				// Register camera present hook
				l_Code = SVMatroxSystemInterface::SetHookFunction(*(rSystem.m_System.get()), SVMatroxSystemHook::SVCameraPresent, SVMatroxGige::CameraPresentCallback, reinterpret_cast<void*> (&rSystem.m_Handle));
				if (l_Code == S_OK)
				{
					result = CreateDigitizers(rSystem);
				}
				else
				{
					result = l_Code;
				}
			}
		}
	}
	else
	{
		result = E_FAIL;
	}
	return result;
}

HRESULT SVMatroxGige::CreateDigitizers(SVMatroxGigeSystem& system)
{
	HRESULT result{ S_OK };

	long l_DigCount = 0;
	HRESULT l_Code = SVMatroxSystemInterface::Get(*(system.m_System.get()), SVMatroxSystemInquire::SVDigitizerNum, l_DigCount);
	if (l_Code == S_OK)
	{
		for (long i = 0; i < l_DigCount; i++)
		{
			result = CreateDigitizer(system, i);
		}
	}
	else
	{
		result = l_Code;
	}
	return result;
}

HRESULT SVMatroxGige::CreateDigitizer(SVMatroxGigeSystem& system, long digitizerIndex)
{
	HRESULT result{ S_OK };

	SVMatroxGigeDigitizer l_camera(digitizerIndex, system.m_Handle);
	l_camera.m_Name = SvUl::Format(_T("Dig_%d"), digitizerIndex);
	l_camera.m_FullName = SvUl::Format(_T("%s.%s"), system.m_Name.c_str(), l_camera.m_Name.c_str());
	l_camera.m_Digitizer = SVMatroxDigitizerPtr{ new SVMatroxDigitizer };

	result = AllocDigitizer(system, digitizerIndex, l_camera);
	if (S_OK == result)
	{
		result = ReadCameraFormat(l_camera);
	}
	if (S_OK == result)
	{
		result = system.AddDigitizer(l_camera);
	}
	return result;
}

HRESULT SVMatroxGige::AllocDigitizer(SVMatroxGigeSystem& system, long digitizerIndex, SVMatroxGigeDigitizer& p_rCamera)
{
	HRESULT result{ S_OK };
	// where to get the data format from? //"gigevision_640x480_mono8.dcf";
	std::string dataFormat = "gigevision_currentstate.dcf"; // from Matrox Sample code...

	HRESULT l_Code = SVMatroxDigitizerInterface::Allocate(*(system.m_System.get()), digitizerIndex, dataFormat, *(p_rCamera.m_Digitizer.get()));

	// Read Serial Number if no prior errors
	if (l_Code == S_OK)
	{
		result = ReadCameraSerialNumber(p_rCamera);
		if (S_OK == result)
		{
			result = ReadCameraIPAddress(p_rCamera);
		}
		if (l_Code == S_OK)
		{
			p_rCamera.GetGigeEventList();
		}
	}
	else
	{
		result = l_Code;
	}
	return result;
}

HRESULT SVMatroxGige::DestroyDigitizers(SVMatroxGigeSystem& rSystem)
{
	HRESULT result{ S_OK };

	SVMatroxGigeDigitizerList& rList = rSystem.GetDigitizerList();
	SVMatroxGigeDigitizerList::iterator it;
	for (it = rList.begin(); it != rList.end(); ++it)
	{
		CameraStop(SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE(rSystem.m_Handle, it->first)); // Stop Acquiring...

		result = DestroyDigitizer(it->second);
	}
	//
	rList.clear();

	return result;
}

HRESULT SVMatroxGige::DestroyDigitizer(SVMatroxGigeDigitizer& rCamera)
{
	HRESULT result{ S_OK };

	// Destroy Digitizer
	if (nullptr != rCamera.m_Digitizer)
	{
		result = SVMatroxDigitizerInterface::Destroy(*(rCamera.m_Digitizer.get()));
	}
	return result;
}

HRESULT SVMatroxGige::StartDigitizer(SVMatroxGigeDigitizer& rCamera, const _variant_t& rTriggerSource)
{
	rCamera.m_frameStack.clear();

	HRESULT hr = RegisterMatroxDigitizerHooks(rCamera);
	if (S_OK == hr)
	{
		hr = SetGrabMode(rCamera);
	}
	if (S_OK == hr)
	{
		hr = EnableTriggering(rCamera, rTriggerSource);
	}
	if (S_OK == hr)
	{
		// Start the Asynchronous Grab (using MdigProcess)
		hr = rCamera.StartGrabArray(SVMatroxGige::DigitizerEndFrameCallback);
	}
	return hr;
}

HRESULT SVMatroxGige::StopDigitizer(SVMatroxGigeDigitizer& rCamera)
{
	HRESULT hr = S_OK;
	if (nullptr != rCamera.m_Digitizer)
	{
		// stop acquiring
		hr = rCamera.StopGrabArray(SVMatroxGige::DigitizerEndFrameCallback);

		// Remove Matrox Hooks
		UnRegisterMatroxDigitizerHooks(rCamera);
	}
	//
	rCamera.m_frameStack.clear();
	return hr;
}

bool SVMatroxGige::IsValidDigitizer(unsigned long digitizerHandle) const
{
	bool bRetVal = IsValidDigitizerHandle(digitizerHandle);
	if (bRetVal)
	{
		const SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(digitizerHandle);
		bRetVal = (nullptr != l_rCamera.m_Digitizer.get() && !l_rCamera.m_Digitizer.get()->empty());
	}
	return bRetVal;
}

bool SVMatroxGige::IsValidDigitizerHandle(unsigned long digitizerHandle) const
{
	unsigned char systemHandle = static_cast<unsigned char>(digitizerHandle >> 8);
	unsigned char acquisitionHandle = static_cast<unsigned char> (digitizerHandle);

	return m_Systems.IsValidHandle(systemHandle) && m_Systems.IsValidComponentHandle(systemHandle, acquisitionHandle);
}

HRESULT SVMatroxGige::ReadCameraSerialNumber(SVMatroxGigeDigitizer& rCamera)
{
	HRESULT hr = S_FALSE;
	std::string serialNo;

	HRESULT l_Code = SVMatroxDigitizerInterface::GetGigeSerialNumber(*(rCamera.m_Digitizer.get()), serialNo);
	if (l_Code == S_OK)
	{
		rCamera.m_svCameraId.m_serialNo = serialNo;

		hr = S_OK;
	}
	return hr;
}

HRESULT SVMatroxGige::ReadCameraIPAddress(SVMatroxGigeDigitizer& rCamera)
{
	HRESULT hr = S_FALSE;
	__int64 ipAddress;

	HRESULT l_Code = SVMatroxDigitizerInterface::GetGigeIPAddress(*(rCamera.m_Digitizer.get()), ipAddress);

	if (l_Code == S_OK)
	{
		rCamera.m_svCameraId.m_ipAddress = ipAddress;

		hr = S_OK;
	}

	return hr;
}

HRESULT SVMatroxGige::ReadCameraFormat(SVMatroxGigeDigitizer& rCamera)
{
	HRESULT hr = S_FALSE;

	SVMatroxDigitizerFormat format;
	HRESULT l_Code = SVMatroxDigitizerInterface::GetFormat(*(rCamera.m_Digitizer.get()), format);

	if (l_Code == S_OK)
	{
		rCamera.m_params.Width = format.size.cx;
		rCamera.m_params.Height = format.size.cy;
		rCamera.m_params.XOffsetPos = format.offset.x;
		rCamera.m_params.YOffsetPos = format.offset.y;

		switch (format.bitDepth)
		{
		case 8:
			rCamera.m_params.Format = SvDef::SVImageFormatMono8;
			break;

		case 32:
			rCamera.m_params.Format = SvDef::SVImageFormatBGR888X;
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

bool SVMatroxGige::IsCameraActive(unsigned long digitizerHandle)
{
	bool bRetVal = false;
	if (IsValidDigitizerHandle(digitizerHandle))
	{
		SVMatroxGigeDigitizer& l_rCamera = GetDigitizer(digitizerHandle);
		bRetVal = (l_rCamera.m_lIsStarted) ? true : false;
	}
	return bRetVal;
}

HRESULT SVMatroxGige::RegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& rCamera)
{
	// Register Matrox Hook callback(s)
	if (nullptr != rCamera.m_Digitizer)
	{
		return SVMatroxDigitizerInterface::SetHookFunction(*(rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameStart, SVMatroxGige::DigitizerStartFrameCallback, (void*)&rCamera);
	}
	return S_FALSE;
}

HRESULT SVMatroxGige::UnRegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& rCamera)
{
	// Unregister Matrox Hook callback
	if (nullptr != rCamera.m_Digitizer)
	{
		return SVMatroxDigitizerInterface::ReleaseHookFunction(*(rCamera.m_Digitizer.get()), SVMatroxDigitizerHook::SVGrabFrameStart, &SVMatroxGige::DigitizerStartFrameCallback, (void*)&rCamera);
	}
	return S_OK;
}

HRESULT SVMatroxGige::SetGrabMode(const SVMatroxGigeDigitizer& rCamera)
{
	// Set Grab Timeout to Infinite
	HRESULT result = SVMatroxDigitizerInterface::SetInfiniteGrabTimeout(*(rCamera.m_Digitizer.get()));
	if (result == S_OK)
	{
		// Set Grab Mode to Asynchronous
		result = SVMatroxDigitizerInterface::SetGrabMode(*(rCamera.m_Digitizer.get()), SVMatroxDigitizerGrab::SVGrabAsynchronous);
	}
	return result;
}

HRESULT SVMatroxGige::EnableTriggering(const SVMatroxGigeDigitizer& rCamera, const _variant_t& rTriggerSource)
{
	HRESULT result{ E_FAIL };

	switch (rCamera.m_params.TriggerType)
	{
		case SvDef::TriggerType::HardwareTrigger:
		case SvDef::TriggerType::CameraTrigger:
		{
			result = SVMatroxGigeDeviceParameterManager::SetParameter(rCamera, SvDef::SVGigeParameterTriggerSource, rTriggerSource);
			break;
		}
		case SvDef::TriggerType::SoftwareTrigger:
		{
			_variant_t value("Software Trigger");
			result = SVMatroxGigeDeviceParameterManager::SetParameter(rCamera, SvDef::SVGigeParameterTriggerSource, value);
			break;
		}
		default:
		{
			break;
		}
	}

	if (S_OK == result)
	{
		_variant_t value("Enable");
		result = SVMatroxGigeDeviceParameterManager::SetParameter(rCamera, SvDef::SVGigeParameterTriggerEnable, value);
	}
	return result;
}

const SVMatroxGigeDigitizer& SVMatroxGige::GetDigitizer(unsigned long digitizerHandle) const
{

	unsigned char systemHandle = static_cast<unsigned char>(digitizerHandle >> 8);
	unsigned char acquisitionHandle = static_cast<unsigned char> (digitizerHandle);

	HRESULT code{ E_FAIL };
	const SVMatroxGigeSystem& rSystem = m_Systems.Get(systemHandle, code);
	return rSystem.GetDigitizer(acquisitionHandle, code);
}

SVMatroxGigeDigitizer& SVMatroxGige::GetDigitizer(unsigned long digitizerHandle)
{
	unsigned char systemHandle = static_cast<unsigned char>(digitizerHandle >> 8);
	unsigned char acquisitionHandle = static_cast<unsigned char> (digitizerHandle);

	HRESULT code{ E_FAIL };
	SVMatroxGigeSystem& rSystem = m_Systems.Get(systemHandle, code);
	return rSystem.GetDigitizer(acquisitionHandle, code);
}

void SVMatroxGige::HandleConnect(SVMatroxGigeSystem& rSystem, long deviceNumber)
{
	// Check if camera is already in the list 
	unsigned long handle = 0;
	HRESULT hr = FindCamera(rSystem, deviceNumber, handle);
	bool bHasCamera = (S_OK == hr) ? true : false;

	if (bHasCamera)
	{
		SVMatroxGigeDigitizer& rCamera = rSystem.GetDigitizer(static_cast<unsigned char> (handle), hr);
		if (S_OK == hr)
		{
			if (nullptr == rCamera.m_Digitizer)
			{
				// Allocate, recreate buffers, send camera parameters and start acquire
				rCamera.m_Digitizer = SVMatroxDigitizerPtr{ new SVMatroxDigitizer };
				hr = AllocDigitizer(rSystem, deviceNumber, rCamera);

				if (S_OK == hr)
				{
					if (rCamera.m_lIsStarted)
					{
						// Set the Camera Parameters before recreating buffers and restarting acquisition
						hr = m_trackedCameraParameters.ReloadCameraParams(rCamera.m_svCameraId.m_serialNo, rCamera);
						if (S_OK == hr)
						{
							hr = ReadCameraFormat(rCamera);

							if (S_OK == hr)
							{
								hr = rCamera.CreateAcquisitionBuffers(*(rSystem.m_System.get()), 
																rCamera.m_params.Width, 
																rCamera.m_params.Height,
																rCamera.m_params.Format);

								if (S_OK == hr)
								{
									variant_t triggerSource = CameraGetParameter(handle, SvDef::SVGigeParameterTriggerSource);
									hr = StartDigitizer(rCamera, triggerSource);
									if (S_OK != hr)
									{
										// log an exception
										SvDef::StringVector msgList;
										msgList.push_back(SvUl::Format(_T("%d"), hr));
										msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
										SvStl::MessageManager Exception(SvStl::MsgType::Log );
										Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_StartDigitizerError, msgList, SvStl::SourceFileParams(StdMessageParams) );
									}
								}
								else
								{
									// log an exception
									SvDef::StringVector msgList;
									msgList.push_back(SvUl::Format(_T("%d"), hr));
									msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
									SvStl::MessageManager Exception(SvStl::MsgType::Log );
									Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_ReloadError, msgList, SvStl::SourceFileParams(StdMessageParams) );
								}
							}
							else
							{
								// log an exception
								SvDef::StringVector msgList;
								msgList.push_back(SvUl::Format(_T("%d"), hr));
								msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
								SvStl::MessageManager Exception(SvStl::MsgType::Log );
								Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_CreateError, msgList, SvStl::SourceFileParams(StdMessageParams) );
							}
						}
						else
						{
							// log an exception
							SvDef::StringVector msgList;
							msgList.push_back(SvUl::Format(_T("%d"), hr));
							msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
							SvStl::MessageManager Exception(SvStl::MsgType::Log );
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
					SvStl::MessageManager Exception(SvStl::MsgType::Log );
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
			SvStl::MessageManager Exception(SvStl::MsgType::Log );
			Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_GetDigitizerError, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
	else
	{
		// log an exception
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
		SvStl::MessageManager Exception(SvStl::MsgType::Log );
		Exception.setMessage( SVMSG_MATROX_GIGE_RECONNECT_ERROR, SvStl::Tid_MatroxGigE_Connect_FindCameraError, msgList, SvStl::SourceFileParams(StdMessageParams) );
	}
}

void SVMatroxGige::HandleDisconnect(SVMatroxGigeSystem& rSystem, long deviceNumber)
{
	// Get Digitizer
	unsigned long handle = 0;
	HRESULT hr = FindCamera(rSystem, deviceNumber, handle);

	if (S_OK == hr)
	{
		SVMatroxGigeDigitizer& l_rCamera = rSystem.GetDigitizer(static_cast<unsigned char> (handle), hr);
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
			SvStl::MessageManager Exception(SvStl::MsgType::Log );
			Exception.setMessage( SVMSG_MATROX_GIGE_DISCONNECT_ERROR, SvStl::Tid_MatroxGigE_Disconnect_GetDigitizerError, msgList, SvStl::SourceFileParams(StdMessageParams) );
		}
	}
	else
	{
		// log an exception
		SvDef::StringVector msgList;
		msgList.push_back(SvUl::Format(_T("%d"), hr));
		msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
		SvStl::MessageManager Exception(SvStl::MsgType::Log );
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

void SVMatroxGige::SetCameraParam(const SVMatroxGigeDigitizer& digitizer, int parameterID, _variant_t value)
{
	SVMatroxGigeCameraParam cameraParameter(parameterID, value);
	m_trackedCameraParameters.AddCameraParam(digitizer.m_svCameraId.m_serialNo, cameraParameter);
}

HRESULT SVMatroxGige::FindCamera(const SVMatroxGigeSystem& rSystem, long deviceNumber, unsigned long& rHandle)
{
	HRESULT hr = S_FALSE;

	const SVMatroxGigeDigitizerList& list = rSystem.GetDigitizerList();
	for (SVMatroxGigeDigitizerList::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		const SVMatroxGigeDigitizer& digitizer = it->second;
		if (digitizer.GetDeviceNumber() == deviceNumber)
		{
			hr = S_OK;
			rHandle = SV_MAKE_MATROXGIGE_SYSTEM_DIGITIZER_HANDLE(rSystem.m_Handle, it->first);
			break;
		}
	}
	return hr;
}

// Callback used for MdigHook - Start Frame
__int64 SVMatroxGige::DigitizerStartFrameCallback(__int64 HookType, __int64, void* pContext)
{
	try
	{
		if (SVMatroxDigitizerInterface::IsEventGrabFrameStart(HookType))
		{
			SVMatroxGigeDigitizer* pCamera = reinterpret_cast<SVMatroxGigeDigitizer*> (pContext);
			g_matroxAcqDevice.ProcessStartFrame(pCamera);
		}
	}
	catch (...)
	{
		// Log Exception ?
	}
	return 0L;
}

// Callback used for MdigProcess
__int64 SVMatroxGige::DigitizerEndFrameCallback(__int64, __int64 HookId, void* pContext)
{
	try
	{
		SVMatroxGigeDigitizer* pCamera = reinterpret_cast<SVMatroxGigeDigitizer*> (pContext);
		g_matroxAcqDevice.ProcessEndFrame(pCamera, HookId);
	}
	catch (...)
	{
		// Log Exception ?
	}

	return 0;
}

// Handler for camera disconnect/reconnect
__int64 SVMatroxGige::CameraPresentCallback(__int64, __int64 EventId, void* pContext )
{
	unsigned char systemHandle = *(reinterpret_cast<unsigned char*> (pContext));
	HRESULT code{ S_OK };
	SVMatroxGigeSystem& system = g_matroxAcqDevice.m_Systems.Get(systemHandle, code);
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
				SvStl::MessageManager Exception(SvStl::MsgType::Log );
				Exception.setMessage( SVMSG_SVMATROXGIGE_NO_ERROR, SvStl::Tid_MatroxGigE_Connect, msgList, SvStl::SourceFileParams(StdMessageParams) );

				g_matroxAcqDevice.HandleConnect(system, static_cast<unsigned char>(deviceNumber));
			}
			else
			{
				// log an exception
				SvDef::StringVector msgList;
				msgList.push_back(SvUl::Format(_T("%d"), deviceNumber));
				SvStl::MessageManager Exception(SvStl::MsgType::Log );
				Exception.setMessage( SVMSG_SVMATROXGIGE_NO_ERROR, SvStl::Tid_MatroxGigE_Disconnect, msgList, SvStl::SourceFileParams(StdMessageParams) );

				g_matroxAcqDevice.HandleDisconnect(system, deviceNumber);
			}
		}
	}
	return 0l;
}
