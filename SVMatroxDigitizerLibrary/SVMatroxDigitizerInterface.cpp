//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerInterface
//* .File Name       : $Workfile:   SVMatroxDigitizerInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:30  $
//******************************************************************************
#include "stdafx.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h"
#include "SVMatroxDigitizerInterface.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVUtilityLibrary/SVString.h"

/**
@SVOperationName Allocate

@SVOperationDescription This method allocates the Matrox Handle using the Default Data Format for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Allocate(const SVMatroxSystem& SystemID,
									  long DeviceNum,
									  SVMatroxDigitizer& digitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_ID l_digitizerID = M_NULL;
		MdigAlloc(SystemID.m_SystemIdentifier, DeviceNum, (MIL_TEXT_PTR)M_DEFAULT, M_DEFAULT, &l_digitizerID);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			digitizerID.m_DigitizerIdentifier = l_digitizerID;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Allocate

@SVOperationDescription This method allocates the Matrox Handle for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Allocate(const SVMatroxSystem& SystemID,
									  long DeviceNum,
									  const SVMatroxString& DataFormat,
									  SVMatroxDigitizer& digitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_ID l_digitizerID = M_NULL;
		size_t len = DataFormat.length();
		char* pDataFormatStr = new char[len + 1];
		strcpy_s(pDataFormatStr, len + 1, DataFormat.c_str());

		MdigAlloc(SystemID.m_SystemIdentifier, DeviceNum, pDataFormatStr, M_DEFAULT, &l_digitizerID);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			digitizerID.m_DigitizerIdentifier = l_digitizerID;
		}
		delete [] pDataFormatStr;
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Release

@SVOperationDescription This method releases the handle to the Digitizer.

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Release(SVMatroxDigitizer& DigitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if (DigitizerID.m_DigitizerIdentifier != 0)
		{
			MdigFree(DigitizerID.m_DigitizerIdentifier);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Channel

@SVOperationDescription This method selects the active input data or synchronization channel for the specified digitizer. If the digitizer does not have the specified channel, an error is generated and the last selected channel remains effective. The default channel corresponds to the default channel for the specified digitizer's DCF. 

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Channel(const SVMatroxDigitizer& DigitizerID, long Channel)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		DIG_CONTROL_TYPE l_Channel = Channel;
		MdigChannel(DigitizerID.m_DigitizerIdentifier, l_Channel);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName IsCorruptedFrame

@SVOperationDescription This method determines if the frame is corrupted

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::IsCorruptedFrame(SVMatroxIdentifier milEventID, bool& bIsCorrupted)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT value;
		MdigGetHookInfo(milEventID, M_CORRUPTED_FRAME, &value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();

		if (l_Code == SVMEE_STATUS_OK)
		{
			bIsCorrupted = (value == M_YES) ? true : false;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetGrabTimeStamp

@SVOperationDescription This method gets the timestamp for the grabbed image

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGrabTimeStamp(const SVMatroxDigitizer& DigitizerID, double& timestamp)
{
	SVStatusCode l_Code (SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigGetHookInfo(DigitizerID.m_DigitizerIdentifier, M_MODIFIED_BUFFER + M_GRAB_TIME_STAMP, &timestamp);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetGrabBuffer

@SVOperationDescription This method gets the buffer that was grabbed into.
This method can only be called from the callback func used with MdigProcess

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGrabBuffer(SVMatroxIdentifier milEventID, SVMatroxIdentifier& bufferID)
{
	SVStatusCode l_Code (SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigGetHookInfo(milEventID, M_MODIFIED_BUFFER + M_BUFFER_ID, &bufferID);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetHookInfo

@SVOperationDescription This method gets the Hook Info for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetHookInfo(SVMatroxIdentifier milEventID, SVMatroxDigitizerHook::SVHookInfoEnum InfoType, void* UserPtr)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxInt l_matroxType = 0;
		HRESULT hr = SVMatroxDigitizerHook::m_HookInfoEnumConvertor.ConvertEnumToMatroxType(InfoType, l_matroxType);
		if (hr == S_OK)
		{
			MdigGetHookInfo(milEventID, l_matroxType, UserPtr);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetGigeCameraTimestamp

@SVOperationDescription This method gets the Gige camera's timestamp 

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeCameraTimestamp(SVMatroxIdentifier milEventID, double& timestamp)
{
	SVStatusCode l_Code (SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
#if SV_CURRENT_MIL_VERSION == 0x0900
		MdigGetHookInfo(milEventID, M_GC_CAMERA_TIME_STAMP, &timestamp);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
#endif
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetGigeEventType

@SVOperationDescription This method gets the Event Type

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeEventType(SVMatroxIdentifier milEventID, long& p_rEventType)
{
	SVStatusCode l_Code (SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
#if SV_CURRENT_MIL_VERSION == 0x0900
		SVMatroxInt l_EventType;
		MdigGetHookInfo(milEventID, M_GC_EVENT_TYPE, &l_EventType);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		// map it to some SVR bullshit...
		p_rEventType = static_cast<long>(l_EventType);
#endif
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetHookFunction

@SVOperationDescription This method sets the Hook Function (callback) for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetHookFunction(const SVMatroxDigitizer& DigitizerID,
																					SVMatroxDigitizerHook::SVHookTypeEnum HookType,
													                                SVMatroxHookFunctionPtr HookHandlerPtr,
																					void* UserDataPtr)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxHookType = 0;
		HRESULT hr = SVMatroxDigitizerHook::m_HookTypeEnumConvertor.ConvertEnumToMatroxType(HookType, l_MatroxHookType);
		if (hr == S_OK)
		{
			MdigHookFunction(DigitizerID.m_DigitizerIdentifier, l_MatroxHookType, HookHandlerPtr, UserDataPtr);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName ReleaseHookFunction

@SVOperationDescription This method removes the Hook Function (callback) for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::ReleaseHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr, 
								  void* UserDataPtr)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxHookType = 0;
		HRESULT hr = SVMatroxDigitizerHook::m_HookTypeEnumConvertor.ConvertEnumToMatroxType(HookType, l_MatroxHookType);
		if (hr == S_OK)
		{
			// check for hook first before removing ?
			l_MatroxHookType |= M_UNHOOK;
			MdigHookFunction(DigitizerID.m_DigitizerIdentifier, l_MatroxHookType, HookHandlerPtr, UserDataPtr);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = hr;
		}
	}

#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName IsEventGrabFrameStart

@SVOperationDescription This method evaluates the EventType to see if it's the Grab Frame Start event.

*/

bool SVMatroxDigitizerInterface::IsEventGrabFrameStart(SVMatroxIdentifier EventType)
{
	return ((EventType & M_GRAB_FRAME_START ) == M_GRAB_FRAME_START);
}

/**
@SVOperationName IsEventGrabFrameEnd

@SVOperationDescription This method evaluates the EventType to see if it's the Grab Frame End event.

*/

bool SVMatroxDigitizerInterface::IsEventGrabFrameEnd(SVMatroxIdentifier EventType)
{
	return ((EventType & M_GRAB_FRAME_END ) == M_GRAB_FRAME_END);
}

/**
@SVOperationName AbortGrab

@SVOperationDescription This method aborts the grab in progress for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::AbortGrab(const SVMatroxDigitizer& DigitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_ABORT, (double)M_DEFAULT);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetGrabMode

@SVOperationDescription This method sets the Grab Mode for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetGrabMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT32 l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabModeEnumConvertor.ConvertEnumToMatroxType(grabMode, l_MatroxType);
		if (hr == S_OK)
		{
			MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_MODE, l_MatroxType);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = hr;
		}
	}

#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetGrabTimeout

@SVOperationDescription This method sets the Grab Timeout value for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetGrabTimeout(const SVMatroxDigitizer& DigitizerID, long value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_TIMEOUT, static_cast<MIL_INT32>(value));
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}

#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetInfiniteGrabTimeout

@SVOperationDescription This method sets the Grab Timeout to infinite (no timeout) for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetInfiniteGrabTimeout(const SVMatroxDigitizer& DigitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_TIMEOUT, M_INFINITE);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}

#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetGrabTriggerMode

@SVOperationDescription This method sets the Grab Trigger Mode for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetGrabTriggerMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabTriggerModeEnum grabTriggerMode)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT32 l_MatroxGrabTriggerMode = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabTriggerModeEnumConvertor.ConvertEnumToMatroxType(grabTriggerMode, l_MatroxGrabTriggerMode);
		if (hr == S_OK)
		{
			MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_TRIGGER_MODE, l_MatroxGrabTriggerMode);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetGrabHardwareTriggerSource

@SVOperationDescription This method sets the Hardware Grab Trigger Source for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetGrabHardwareTriggerSource(const SVMatroxDigitizer& DigitizerID, long portNo)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_TRIGGER_SOURCE, M_HARDWARE_PORT0 + portNo);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetGrabSoftwareTriggerSource

@SVOperationDescription This method sets the Software Grab Trigger Source for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetGrabSoftwareTriggerSource(const SVMatroxDigitizer& DigitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_TRIGGER_SOURCE, M_SOFTWARE);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName FireSoftwareTrigger

@SVOperationDescription This method fires the Software Trigger for Grab. It requires the Trigger Source to be Software

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::FireSoftwareTrigger(const SVMatroxDigitizer& DigitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_TRIGGER, M_ACTIVATE);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		
		/* GIGE Specific
		l_Code =  SVMatroxDigitizerInterface::SetFeature(DigitizerID, "TriggerSoftware", SVMatroxDigitizerFeature::SVTypeCommand, _variant_t(0L));
		*/
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName EnableTrigger

@SVOperationDescription This method enables/disables the triggering for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::EnableTrigger(const SVMatroxDigitizer& DigitizerID, bool bEnable)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_TRIGGER, (bEnable) ? M_ENABLE : M_DISABLE);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code; 
}

/**
@SVOperationName Get

@SVOperationDescription This method gets the Board values for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVBoardEnum InquireType, long& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerInquire::m_BoardEnumConvertor.ConvertEnumToMatroxType(InquireType, l_MatroxType);
		if (hr == S_OK)
		{
			MdigInquire(DigitizerID.m_DigitizerIdentifier, l_MatroxType, &Value);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetColorMode

@SVOperationDescription This method gets the Color Mode for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetColorMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVColorModeEnum& ColorMode)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long matroxValue;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_COLOR_MODE, &matroxValue);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			HRESULT hr = SVMatroxDigitizerInquire::m_ColorModeEnumConvertor.ConvertEnumFromMatroxType(matroxValue, ColorMode);
			if (hr != S_OK) // Converison Error
			{
				l_Code = hr;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetScanMode

@SVOperationDescription This method gets the Scan Mode for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetScanMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVScanModeEnum& ScanMode)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long matroxValue;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SCAN_MODE, &matroxValue);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			HRESULT hr = SVMatroxDigitizerInquire::m_ScanModeEnumConvertor.ConvertEnumFromMatroxType(matroxValue, ScanMode);
			if (hr != S_OK) // Converison Error
			{
				l_Code = hr;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetFormat

@SVOperationDescription This method gets the Format for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetFormat(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerFormat& format)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT len;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_FORMAT_SIZE, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MIL_TEXT_PTR pFormat = new MIL_TEXT_CHAR[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_FORMAT, pFormat);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == SVMEE_STATUS_OK)
			{
				GetSize(DigitizerID, format.size.cx, format.size.cy);
				GetBitDepth(DigitizerID, format.bitDepth);
				GetOffset(DigitizerID, format.offset.x, format.offset.y);
				GetNumberOfBands(DigitizerID, format.numBands);
			}
			delete [] pFormat;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetSize

@SVOperationDescription This method gets the Size of the source image for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetSize(const SVMatroxDigitizer& DigitizerID, long& X, long& Y)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* SEJ
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SOURCE_SIZE_X, &X);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SOURCE_SIZE_Y, &Y);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		*/
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "Width", M_TYPE_MIL_INT32, &X);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "Height", M_TYPE_MIL_INT32, &Y);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetSize

@SVOperationDescription This method sets the Size of the source image for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetSize(const SVMatroxDigitizer& DigitizerID, long X, long Y)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* SEJ(GIGE)
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_SOURCE_SIZE_X, X);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigControl(DigitizerID.m_DigitizerIdentifier, M_SOURCE_SIZE_Y, Y);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		*/
		MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "Width", M_TYPE_MIL_INT32, &X);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "Height", M_TYPE_MIL_INT32, &Y);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetBitDepth

@SVOperationDescription This method gets the Bit Depth for the source image for the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetBitDepth(const SVMatroxDigitizer& DigitizerID, long& BitDepth)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* SEJ(GIGE)
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SIZE_BIT, &BitDepth);
		*/
		BitDepth = 0;

		MIL_TEXT_CHAR pixelFormat[32];
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, MIL_TEXT("PixelFormat"), M_TYPE_STRING_ENUMERATION, pixelFormat);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();

		if (l_Code == SVMEE_STATUS_OK)
		{
			if (strstr(pixelFormat, "Mono"))
			{
				BitDepth = 8;
			}
			else
			{
				BitDepth = 32;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetNumberOfBands

@SVOperationDescription This method gets the number of bands for the source image for the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetNumberOfBands(const SVMatroxDigitizer& DigitizerID, long& numBands)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* SEJ(GIGE)
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SIZE_BAND, &numBands);
		*/
		char pixelFormat[32];
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "PixelFormat", M_TYPE_STRING_ENUMERATION, pixelFormat);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			if (strstr(pixelFormat, "Mono"))
			{
				numBands = 1;
			}
			else
			{
				numBands = 3;
			}
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetOffset

@SVOperationDescription This method gets the offset for the source image for the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetOffset(const SVMatroxDigitizer& DigitizerID, long& XOffset, long& YOffset)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* SEJ(GIGE)
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SOURCE_OFFSET_X, &XOffset);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SOURCE_OFFSET_Y, &YOffset);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		*/
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "OffsetX", M_TYPE_MIL_INT32, &XOffset);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "OffsetY", M_TYPE_MIL_INT32, &YOffset);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetOffset

@SVOperationDescription This method sets the offset for the source image for the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetOffset(const SVMatroxDigitizer& DigitizerID, long XOffset, long YOffset)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* SEJ(GIGE)
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_SOURCE_OFFSET_X, XOffset);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigControl(DigitizerID.m_DigitizerIdentifier, M_SOURCE_OFFSET_Y, YOffset);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		*/
		MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "OffsetX", M_TYPE_MIL_INT32, &XOffset);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "OffsetY", M_TYPE_MIL_INT32, &YOffset);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetVendor

@SVOperationDescription This method gets the vendor string for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetVendor(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT len;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_VENDOR_LENGTH, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			MIL_TEXT_PTR pVendor = new MIL_TEXT_CHAR[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_VENDOR, pVendor);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == SVMEE_STATUS_OK)
			{
				Value = pVendor;
			}
			delete [] pVendor;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetModel

@SVOperationDescription This method gets the model name string for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetModel(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_TEXT_CHAR Model[256];
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, "DeviceModelName", M_TYPE_STRING, &Model);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			Value = Model;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GrabInprogress

@SVOperationDescription This method gets whether a Grab is in Progress for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GrabInProgress(const SVMatroxDigitizer& DigitizerID, bool& bInProgress)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT value = 0;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GRAB_IN_PROGRESS, &value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			bInProgress = (value) ? true : false;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Get

@SVOperationDescription This method gets the Grab parameters for the Digitizer

*/
/*
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVGrabEnum InquireType, _variant_t& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerInquire::m_GrabEnumConvertor.ConvertEnumToMatroxType(InquireType, l_MatroxType);
		if (hr == S_OK)
		{
			// determine type based on Enum category ?
			long value;
			MdigInquire(DigitizerID.m_DigitizerIdentifier, l_MatroxType, &value);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}
*/
/**
@SVOperationName Get

@SVOperationDescription This method gets the Frame Count values for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVFrameEnum InquireType, long& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerInquire::m_FrameEnumConvertor.ConvertEnumToMatroxType(InquireType, l_MatroxType);
		if (hr == S_OK)
		{
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_PROCESS_FRAME_COUNT, &Value);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetGigeVersion

@SVOperationDescription This method gets the Gige Version string for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeVersion(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT len;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_VERSION_LENGTH, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			char* pVersion = new char[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_VERSION, &pVersion);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == SVMEE_STATUS_OK)
			{
				Value = pVersion;
			}
			delete [] pVersion;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetGigeSerialNumber

@SVOperationDescription This method gets the Gige Serial Number for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeSerialNumber(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT len;

		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_SERIAL_NUMBER_LENGTH, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			char* pSerialNo = new char[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_SERIAL_NUMBER, pSerialNo);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == SVMEE_STATUS_OK)
			{
				Value = pSerialNo;
			}
			delete [] pSerialNo;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetColorMode

@SVOperationDescription This method gets the Gige Specific Info (user supplied) for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeSpecificInfo(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long len;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_SPECIFIC_INFO_LENGTH, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == SVMEE_STATUS_OK)
		{
			char* pInfo = new char[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_SPECIFIC_INFO, &pInfo);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == SVMEE_STATUS_OK)
			{
				Value = pInfo;
			}
			delete [] pInfo;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetMACAddress

@SVOperationDescription This method gets the MAC Address for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeMACAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_MAC_ADDRESS, &Value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetIPAdsress

@SVOperationDescription This method gets the IP Address for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeIPAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_IP_ADDRESS, &Value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Get

@SVOperationDescription This method gets the UserBit parameters for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVUserBitEnum InquireType, long& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxInt l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerInquire::m_UserBitEnumConvertor.ConvertEnumToMatroxType(InquireType, l_MatroxType);
		if (hr == S_OK)
		{
			MdigInquire(DigitizerID.m_DigitizerIdentifier, l_MatroxType, &Value);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Set

@SVOperationDescription This method sets the Control Camera Value for the Digitizer.

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::EnableLastGrabInTrueBuffer(const SVMatroxDigitizer& DigitizerID, bool bEnable)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_LAST_GRAB_IN_TRUE_BUFFER, (bEnable) ? M_ENABLE : M_DISABLE);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}
		
/**
@SVOperationName Set

@SVOperationDescription This method sets the Control Camera Value for the Digitizer.

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVCameraEnum ControlType, double Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerControl::m_CameraEnumConvertor.ConvertEnumToMatroxType(ControlType, l_MatroxType);
		if (hr == S_OK)
		{
			MdigControl(DigitizerID.m_DigitizerIdentifier, l_MatroxType, Value);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Set

@SVOperationDescription This method sets the Control Grab Value for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabEnum ControlType, const _variant_t& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabEnumConvertor.ConvertEnumToMatroxType(ControlType, l_MatroxType);
		if (hr == S_OK)
		{
			double value = 0.0;
			MdigControl(DigitizerID.m_DigitizerIdentifier, l_MatroxType, value);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName EnableCorruptedFrameMonitoring

@SVOperationDescription This method Enables/Disables the Corrupted Frame Monitoring for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::EnableCorruptedFrameMonitoring(const SVMatroxDigitizer& DigitizerID, bool bEnable)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_CORRUPTED_FRAME_ERROR, (bEnable) ? M_ENABLE : M_DISABLE);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Set

@SVOperationDescription This method sets the Control UserBit Value for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVUserBitEnum ControlType, const _variant_t& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxInt l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerControl::m_UserBitEnumConvertor.ConvertEnumToMatroxType(ControlType, l_MatroxType);
		if (hr == S_OK)
		{
			double value = 0.0;
			MdigControl(DigitizerID.m_DigitizerIdentifier, l_MatroxType, value);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			// conversion error
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName ShowGigeProperties

@SVOperationDescription This method shows the built-in Matrox Gige properties Dialog for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::ShowGigeProperties(const SVMatroxDigitizer& DigitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		double value = M_DEFAULT;
		MdigControl(DigitizerID.m_DigitizerIdentifier, M_GC_FEATURE_BROWSER, value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetLutID

@SVOperationDescription This method gets the Matrox Identifier for the LUT associated with the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetLutID(const SVMatroxDigitizer& DigitizerID, SVMatroxIdentifier& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_LUT_ID, &Value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetLutNumberOfBands

@SVOperationDescription This method gets the number of Lut Bands for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetLutNumberOfBands(const SVMatroxDigitizer& DigitizerID, long& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SIZE_BAND, &Value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetLutBandSize

@SVOperationDescription This method gets the size of the Lut Band for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetLutBandSize(const SVMatroxDigitizer& DigitizerID, long& Value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SIZE_BAND_LUT, &Value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetLUT

@SVOperationDescription This method sets the LUT info for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetLut(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& LutBufId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		if( ! LutBufId.empty() )
		{
			MdigLut(DigitizerID.m_DigitizerIdentifier, LutBufId.GetIdentifier() );
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else
		{
			l_Code = SVMEE_INVALID_HANDLE;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetReference

@SVOperationDescription This method gets the Light reference for the Digitizer

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetReference(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerLightReference::SVLightReferenceEnum ReferenceType, double& ReferenceLevel)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerLightReference::m_LightReferenceEnumConvertor.ConvertEnumToMatroxType(ReferenceType, l_MatroxType);
		if (hr == S_OK)
		{
			MdigInquire(DigitizerID.m_DigitizerIdentifier, l_MatroxType, &ReferenceLevel);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
		}
		else // conversion error
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetReference

@SVOperationDescription This method sets the Light reference for the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetReference(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerLightReference::SVLightReferenceEnum ReferenceType, double ReferenceLevel)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerLightReference::m_LightReferenceEnumConvertor.ConvertEnumToMatroxType(ReferenceType, l_MatroxType);
		if (hr == S_OK)
		{
			MdigReference(DigitizerID.m_DigitizerIdentifier, l_MatroxType, ReferenceLevel);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		}
		else // conversion error
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GetFeature

@SVOperationDescription This method gets the Camera Feature from the XML camera data for the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetFeature(const SVMatroxDigitizer& DigitizerID,
                                  const SVMatroxString& FeatureName,
                                  SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
                                  variant_t& FeatureValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_matroxFeatureType = 0;
		HRESULT hr = SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor.ConvertEnumToMatroxType(FeatureType, l_matroxFeatureType);
		if (hr == S_OK)
		{
			char featureNameStr[256];
			strcpy_s(featureNameStr, sizeof(featureNameStr), FeatureName.c_str());

			switch (FeatureValue.vt)
			{
				case VT_I1:		// Byte
				case VT_UI1: 
					{
						unsigned char value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							FeatureValue = value;
						}
					}
					break;

				case VT_I2:		// Short
				case VT_UI2: 
					{
						unsigned short value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							FeatureValue = value;
						}
					}
					break;

				case VT_I4:		// long
				case VT_UI4:
				case VT_INT:	// INT, UINT
				case VT_UINT:
					{
						long value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							FeatureValue = value;
						}
					}
					break;

				case VT_I8:		// int64
				case VT_UI8:
					{
						__int64 value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							FeatureValue.llVal = value;
						}
					}
					break;

				case VT_BSTR:	// String
					{
						char value[256]; // how to determine size needed ahead of time ?
						
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							SVString strValue(value);
							FeatureValue.bstrVal = strValue.ToBSTR();
						}
					}
					break;

				case VT_R4:		// Float
					{
						float value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							FeatureValue.fltVal = value;
						}
					}
					break;

				case VT_R8:		// Double
					{
						double value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							FeatureValue.dblVal = value;
						}
					}
					break;

				case VT_BOOL:	// Bool
					{
						bool value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == SVMEE_STATUS_OK)
						{
							FeatureValue.boolVal = value;
						}
					}
					break;

				default:
					l_Code = S_FALSE;
					break;
			}
		}
		else
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName SetFeature

@SVOperationDescription This method sets the Camera feature for the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetFeature(const SVMatroxDigitizer& DigitizerID,
                                 const SVMatroxString& FeatureName,
                                 SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
                                 const variant_t& FeatureValue)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_matroxFeatureType = 0;
		HRESULT hr = SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor.ConvertEnumToMatroxType(FeatureType, l_matroxFeatureType);
		if (hr == S_OK)
		{
			char featureNameStr[256];
			strcpy_s(featureNameStr, sizeof(featureNameStr), FeatureName.c_str());

			switch (FeatureValue.vt)
			{
				case VT_EMPTY: // M_DEFAULT or M_NULL
					MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, M_NULL);
					l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					break;

				case VT_I1:		// Byte
				case VT_UI1: 
					{
						unsigned char value = FeatureValue.bVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_I2:		// Short
				case VT_UI2:
					{
						short value = FeatureValue.iVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_I4:		// long
				case VT_UI4:
				case VT_INT:	// INT, UINT
				case VT_UINT:
					{
						long value = FeatureValue.lVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_I8:		// int64
				case VT_UI8:
					{
						__int64 value = FeatureValue.llVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_BSTR:	// String
					{
						SVString strValue = FeatureValue.bstrVal;
						size_t len = strValue.size() + 1;
						char* pValue = new char[len];
						strcpy_s(pValue, len, strValue.ToString()); 
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, pValue);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						delete [] pValue;
					}
					break;

				case VT_R4:		// Float
					{
						float value = FeatureValue.fltVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_R8:		// Double
					{
						double value = FeatureValue.dblVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_BOOL:	// Bool
					{
						bool value = (FeatureValue.boolVal) ? true : false;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;
				
				default:
					l_Code = S_FALSE;
					break;
			}
		}
		else
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif
	return l_Code;
}

/**
@SVOperationName Grab

@SVOperationDescription This method grabs an image from the Digitizer

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Grab(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigGrab(DigitizerID.m_DigitizerIdentifier, DestImageId.GetIdentifier());
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GrabContinuous

@SVOperationDescription This method cause the Digitizer to perform a continuous grab.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GrabContinuous(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigGrabContinuous(DigitizerID.m_DigitizerIdentifier, DestImageId.GetIdentifier());
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName GrabWait

@SVOperationDescription This method causes the Digitizer to grab and wait for it to complete.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GrabWait(const SVMatroxDigitizer& DigitizerID, long Flag)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigGrabWait(DigitizerID.m_DigitizerIdentifier, Flag);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName StartGrabArray

@SVOperationDescription This method cause the Digitizer to perform a grab using an array of buffers.
It will process the buffer in a round robin fashion, wrapping back to the begining of the list.

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::StartGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxOperationFlag = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabModeEnumConvertor.ConvertEnumToMatroxType(grabMode, l_MatroxOperationFlag);
		if (hr == S_OK)
		{
			SVMatroxBufferArray::BufferIDList l_List = p_rBufferArray.GetList(); 
			long numBuffers = static_cast< long >( l_List.size() );
			SVMatroxIdentifier* pList = NULL;

			if( 0 < numBuffers )
			{
				pList = &( l_List[ 0 ] );
			}

			if (pList && numBuffers > 0)
			{
				MdigProcess(DigitizerID.m_DigitizerIdentifier, pList, numBuffers, M_START, l_MatroxOperationFlag, func, userData);
				l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_BAD_POINTER;
			}
		}
		else
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName StopGrabArray

@SVOperationDescription This method cause the Digitizer to stop the grab using an array of buffers.
StartGrabArray must be called prior to calling this method.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::StopGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxOperationFlag = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabModeEnumConvertor.ConvertEnumToMatroxType(grabMode, l_MatroxOperationFlag);
		if (hr == S_OK)
		{
			SVMatroxBufferArray::BufferIDList l_List = p_rBufferArray.GetList(); 
			long numBuffers = static_cast< long >( l_List.size() );
			SVMatroxIdentifier* pList = NULL;

			if( 0 < numBuffers )
			{
				pList = &( l_List[ 0 ] );
			}

			if (pList && numBuffers > 0)
			{
				MdigProcess(DigitizerID.m_DigitizerIdentifier, pList, numBuffers, M_STOP, l_MatroxOperationFlag, func, userData);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
			}
			else
			{
				l_Code = SVMEE_BAD_POINTER;
			}
		}
		else
		{
			l_Code = hr;
		}
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Halt

@SVOperationDescription This method halts any pending grabs.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Halt(const SVMatroxDigitizer& DigitizerID)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigHalt(DigitizerID.m_DigitizerIdentifier);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	}
#ifdef USE_TRY_BLOCKS
	catch(...)
	{
		l_Code = SVMEE_MATROX_THREW_EXCEPTION;
		SVMatroxApplicationInterface::LogMatroxException();
	}
#endif

	return l_Code;
}

/**
@SVOperationName Destroy

@SVOperationDescription This method destroys the Handle for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::Destroy(SVMatroxDigitizer& p_rDigitizer)
{
	SVStatusCode l_Code( SVMatroxDigitizerInterface::Release(p_rDigitizer) );
	
	// Clear the handle ?
	p_rDigitizer.m_DigitizerIdentifier = 0;

	return l_Code;
}

/**
@SVOperationName SetGigeEvent

@SVOperationDescription This method Enables/Disables Event Notification for the Digitizer.

*/
SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::SetGigeEvent(const SVMatroxDigitizer& DigitizerID, const SVMatroxString& eventName, const SVMatroxString& value)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#if SV_CURRENT_MIL_VERSION == 0x0900
	MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, MIL_TEXT("EventSelector"), M_TYPE_STRING_ENUMERATION, eventName.c_str());
	l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	MdigControlFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, MIL_TEXT("EventNotification"), M_TYPE_STRING_ENUMERATION, value.c_str());
	l_Code =  SVMatroxApplicationInterface::GetLastStatus();
#endif
	return l_Code;
}

/**
@SVOperationName GetGigeEventList

@SVOperationDescription This method emunerates the Event Names to get the EventType Ids for the Digitizer.

*/

SVMatroxDigitizerInterface::SVStatusCode SVMatroxDigitizerInterface::GetGigeEventList(const SVMatroxDigitizer& DigitizerID, SVGigeEventList& list)
{
	SVStatusCode l_Code( SVMEE_STATUS_OK );
#if SV_CURRENT_MIL_VERSION == 0x0900
	SVMatroxInt EventCnt = 0;
	
	MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_ELEMENT_COUNT, MIL_TEXT("EventSelector"), M_TYPE_STRING_ENUMERATION, &EventCnt);
	l_Code = SVMatroxApplicationInterface::GetLastStatus();

	if (l_Code == SVMEE_STATUS_OK && EventCnt)
	{
		for ( SVMatroxInt i = 0; i < EventCnt && l_Code == SVMEE_STATUS_OK; i++ )
		{
			SVMatroxInt Len = 0;
			
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_ELEMENT + M_LENGTH + i, MIL_TEXT("EventSelector"), M_TYPE_STRING_ENUMERATION, &Len);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			
			if (l_Code == SVMEE_STATUS_OK && Len)
			{
				MIL_TEXT_PTR pEventName = new MIL_TEXT_CHAR[Len];
				
				MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_ELEMENT + i, MIL_TEXT("EventSelector"), M_TYPE_STRING_ENUMERATION, pEventName);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				
				if (l_Code == SVMEE_STATUS_OK)
				{
					SVMatroxString name(pEventName); // MIL_TEXT is null terminated
					SVMatroxInt eventType = -1;
					SVMatroxString evName = "Event";
					evName += name;
					MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_DEFAULT, evName.c_str(), M_TYPE_MIL_INT, &eventType);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == SVMEE_STATUS_OK)
					{
						list.insert(std::make_pair(name, static_cast<long>(eventType)));
					}
				}
				delete [] pEventName;
			}
		}
	}
#endif
	return l_Code;
}
//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerInterface.cpp_v  $
 * 
 *    Rev 1.1   01 Oct 2013 10:45:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:29:00   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   18 Jan 2013 15:36:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetGigeEventList method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   18 Jul 2012 12:27:10   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  771
 * SCR Title:  Upgrade to the Next Version of Matrox Imaging Library (MIL)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to allow gige to be compiled with both MIL 8.0 and 9.0.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jun 2012 16:38:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jun 2012 14:38:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  764
 * SCR Title:  Add tool comments to all tools in SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated source code needed to implement MIL 9.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jan 2011 10:17:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  712
 * SCR Title:  Fix issues with black images when using command interface (SIAC)
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with Matrox Buffer Management.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Oct 2009 09:22:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised StopGrabArray to correct hang issue
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:13:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   16 Jun 2009 12:23:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised StopGrabArray method to call MdigProcess with M_STOP and M_WAIT flags
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:32:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/