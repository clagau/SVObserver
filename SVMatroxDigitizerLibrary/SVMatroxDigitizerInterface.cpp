//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerInterface
//* .File Name       : $Workfile:   SVMatroxDigitizerInterface.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   04 Dec 2014 13:53:44  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxLibrary/SVMatroxImagingLibrary.h"
#include "SVMatroxDigitizerInterface.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
#if SV_DESIRED_MIL_VERSION == 0x0900
const MIL_INT cFeatureControlType = M_DEFAULT;
const MIL_INT cFeatureControlValueEnumType = M_TYPE_STRING_ENUMERATION;
const MIL_INT cCameraVendorSizeType = M_VENDOR_SIZE;
const MIL_INT cGCSerialNumberSizeType = M_GC_SERIAL_NUMBER_LENGTH;
#else
const MIL_INT cFeatureControlType = M_FEATURE_VALUE;
const MIL_INT cFeatureControlValueEnumType = M_TYPE_STRING;
const MIL_INT cCameraVendorSizeType = M_CAMERA_VENDOR_SIZE;
const MIL_INT cGCSerialNumberSizeType = M_GC_SERIAL_NUMBER_SIZE;
#endif
#pragma endregion Declarations

/**
@SVOperationName Allocate

@SVOperationDescription This method allocates the Matrox Handle using the Default Data Format for the Digitizer.

*/
HRESULT SVMatroxDigitizerInterface::Allocate(const SVMatroxSystem& SystemID,
									  long DeviceNum,
									  SVMatroxDigitizer& digitizerID)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_ID l_digitizerID = M_NULL;
		MdigAlloc(SystemID.m_SystemIdentifier, DeviceNum, (MIL_TEXT_PTR)M_DEFAULT, M_DEFAULT, &l_digitizerID);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			digitizerID.m_DigitizerIdentifier = l_digitizerID;
			// for camera teaming to make sure the settings are the same after a reboot 
			// works even if not using teaming
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_FRAME_MAX_RETRIES, 30);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_FRAME_TIMEOUT, 100);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_PACKET_MAX_RETRIES, 3);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_PACKET_TIMEOUT, 10); 
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_MAX_LEADING_PACKET_MISSED, 0x7FFFFFFF);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_MAX_NBR_PACKETS_OUT_OF_ORDER, 0x7FFFFFFF);
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
HRESULT SVMatroxDigitizerInterface::Allocate(const SVMatroxSystem& SystemID,
									  long DeviceNum,
									  const std::string& DataFormat,
									  SVMatroxDigitizer& digitizerID)
{
	HRESULT l_Code( S_OK );
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
		if (l_Code == S_OK)
		{
			digitizerID.m_DigitizerIdentifier = l_digitizerID;
			// for camera teaming to make sure the settings are the same after a reboot 
			// works even if not using teaming
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_FRAME_MAX_RETRIES, 30);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_FRAME_TIMEOUT, 100);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_PACKET_MAX_RETRIES, 3);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_PACKET_TIMEOUT, 10); 
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_MAX_LEADING_PACKET_MISSED, 0x7FFFFFFF);
			MdigControl (digitizerID.m_DigitizerIdentifier, M_GC_MAX_NBR_PACKETS_OUT_OF_ORDER, 0x7FFFFFFF); 				
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

HRESULT SVMatroxDigitizerInterface::Release(SVMatroxDigitizer& DigitizerID)
{
	HRESULT l_Code( S_OK );
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
@SVOperationName IsCorruptedFrame

@SVOperationDescription This method determines if the frame is corrupted

*/
HRESULT SVMatroxDigitizerInterface::IsCorruptedFrame(SVMatroxIdentifier milEventID, bool& bIsCorrupted)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT value;
		MdigGetHookInfo(milEventID, M_CORRUPTED_FRAME, &value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();

		if (l_Code == S_OK)
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

HRESULT SVMatroxDigitizerInterface::GetGrabTimeStamp(const SVMatroxDigitizer& DigitizerID, double& timestamp)
{
	HRESULT l_Code (S_OK );
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

HRESULT SVMatroxDigitizerInterface::GetGrabBuffer(SVMatroxIdentifier milEventID, SVMatroxIdentifier& bufferID)
{
	HRESULT l_Code (S_OK );
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

HRESULT SVMatroxDigitizerInterface::GetHookInfo(SVMatroxIdentifier milEventID, SVMatroxDigitizerHook::SVHookInfoEnum InfoType, void* UserPtr)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxInt l_matroxType = 0;
		HRESULT hr = SVMatroxDigitizerHook::m_HookInfoEnumConvertor.ConvertEnumToMatroxType(InfoType, l_matroxType);
		if (S_OK == hr)
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

HRESULT SVMatroxDigitizerInterface::GetGigeCameraTimestamp(SVMatroxIdentifier milEventID, double& timestamp)
{
	HRESULT l_Code (S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigGetHookInfo(milEventID, M_GC_CAMERA_TIME_STAMP, &timestamp);
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
@SVOperationName GetGigeEventType

@SVOperationDescription This method gets the Event Type

*/

HRESULT SVMatroxDigitizerInterface::GetGigeEventType(SVMatroxIdentifier milEventID, long& p_rEventType)
{
	HRESULT l_Code (S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		SVMatroxInt l_EventType;
		MdigGetHookInfo(milEventID, M_GC_EVENT_TYPE, &l_EventType);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		// map it to some SVR bullshit...
		p_rEventType = static_cast<long>(l_EventType);
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

HRESULT SVMatroxDigitizerInterface::SetHookFunction(const SVMatroxDigitizer& DigitizerID,
																					SVMatroxDigitizerHook::SVHookTypeEnum HookType,
													                                SVMatroxHookFunctionPtr HookHandlerPtr,
																					void* UserDataPtr)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxHookType = 0;
		HRESULT hr = SVMatroxDigitizerHook::m_HookTypeEnumConvertor.ConvertEnumToMatroxType(HookType, l_MatroxHookType);
		if (S_OK == hr)
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

HRESULT SVMatroxDigitizerInterface::ReleaseHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr, 
								  void* UserDataPtr)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxHookType = 0;
		HRESULT hr = SVMatroxDigitizerHook::m_HookTypeEnumConvertor.ConvertEnumToMatroxType(HookType, l_MatroxHookType);
		if (S_OK == hr)
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

HRESULT SVMatroxDigitizerInterface::AbortGrab(const SVMatroxDigitizer& DigitizerID)
{
	HRESULT l_Code( S_OK );
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

HRESULT SVMatroxDigitizerInterface::SetGrabMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT32 l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabModeEnumConvertor.ConvertEnumToMatroxType(grabMode, l_MatroxType);
		if (S_OK == hr)
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

HRESULT SVMatroxDigitizerInterface::SetGrabTimeout(const SVMatroxDigitizer& DigitizerID, long value)
{
	HRESULT l_Code( S_OK );
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

HRESULT SVMatroxDigitizerInterface::SetInfiniteGrabTimeout(const SVMatroxDigitizer& DigitizerID)
{
	HRESULT l_Code( S_OK );
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
@SVOperationName Get

@SVOperationDescription This method gets the Board values for the Digitizer

*/

HRESULT SVMatroxDigitizerInterface::Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVBoardEnum InquireType, long& Value)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerInquire::m_BoardEnumConvertor.ConvertEnumToMatroxType(InquireType, l_MatroxType);
		if (S_OK == hr)
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

HRESULT SVMatroxDigitizerInterface::GetColorMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVColorModeEnum& ColorMode)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long matroxValue;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_COLOR_MODE, &matroxValue);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			HRESULT hr = SVMatroxDigitizerInquire::m_ColorModeEnumConvertor.ConvertEnumFromMatroxType(matroxValue, ColorMode);
			if (S_OK != hr) // Converison Error
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

HRESULT SVMatroxDigitizerInterface::GetScanMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVScanModeEnum& ScanMode)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long matroxValue;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SCAN_MODE, &matroxValue);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (S_OK == l_Code)
		{
			HRESULT hr = SVMatroxDigitizerInquire::m_ScanModeEnumConvertor.ConvertEnumFromMatroxType(matroxValue, ScanMode);
			if (S_OK != hr) // Converison Error
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

HRESULT SVMatroxDigitizerInterface::GetFormat(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerFormat& format)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT len;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_FORMAT_SIZE, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			MIL_TEXT_PTR pFormat = new MIL_TEXT_CHAR[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_FORMAT, pFormat);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == S_OK)
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

HRESULT SVMatroxDigitizerInterface::GetSize(const SVMatroxDigitizer& DigitizerID, long& X, long& Y)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("Width"), M_TYPE_MIL_INT32, &X);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("Height"), M_TYPE_MIL_INT32, &Y);
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

HRESULT SVMatroxDigitizerInterface::SetSize(const SVMatroxDigitizer& DigitizerID, long X, long Y)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("Width"), M_TYPE_MIL_INT32, &X);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();

		if (l_Code == S_OK)
		{
			MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("Height"), M_TYPE_MIL_INT32, &Y);
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
HRESULT SVMatroxDigitizerInterface::GetBitDepth(const SVMatroxDigitizer& DigitizerID, long& BitDepth)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* GigE syntax
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SIZE_BIT, &BitDepth);
		*/
		BitDepth = 0;

		MIL_TEXT_CHAR pixelFormat[32];
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("PixelFormat"), cFeatureControlValueEnumType, pixelFormat);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();

		if (l_Code == S_OK)
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
HRESULT SVMatroxDigitizerInterface::GetNumberOfBands(const SVMatroxDigitizer& DigitizerID, long& numBands)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		/* GigE syntax
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_SIZE_BAND, &numBands);
		*/
		char pixelFormat[32];
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("PixelFormat"), cFeatureControlValueEnumType, pixelFormat);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
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
HRESULT SVMatroxDigitizerInterface::GetOffset(const SVMatroxDigitizer& DigitizerID, long& XOffset, long& YOffset)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("OffsetX"), M_TYPE_MIL_INT32, &XOffset);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();

		if (l_Code == S_OK)
		{
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("OffsetY"), M_TYPE_MIL_INT32, &YOffset);
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
HRESULT SVMatroxDigitizerInterface::SetOffset(const SVMatroxDigitizer& DigitizerID, long XOffset, long YOffset)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("OffsetX"), M_TYPE_MIL_INT32, &XOffset);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();

		if (l_Code == S_OK)
		{
			MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("OffsetY"), M_TYPE_MIL_INT32, &YOffset);
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

HRESULT SVMatroxDigitizerInterface::GetVendor(const SVMatroxDigitizer& DigitizerID, std::string& Value)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT len;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, cCameraVendorSizeType, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			MIL_TEXT_PTR pVendor = new MIL_TEXT_CHAR[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_CAMERA_VENDOR, pVendor);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == S_OK)
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

HRESULT SVMatroxDigitizerInterface::GetModel(const SVMatroxDigitizer& DigitizerID, std::string& Value)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_TEXT_CHAR Model[256];
		MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("DeviceModelName"), M_TYPE_STRING, &Model);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
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

HRESULT SVMatroxDigitizerInterface::GrabInProgress(const SVMatroxDigitizer& DigitizerID, bool& bInProgress)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT value = 0;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GRAB_IN_PROGRESS, &value);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
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

@SVOperationDescription This method gets the Frame Count values for the Digitizer

*/

HRESULT SVMatroxDigitizerInterface::Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVFrameEnum InquireType, long& Value)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerInquire::m_FrameEnumConvertor.ConvertEnumToMatroxType(InquireType, l_MatroxType);
		if (S_OK == hr)
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
@SVOperationName GetGigeSerialNumber

@SVOperationDescription This method gets the Gige Serial Number for the Digitizer

*/

HRESULT SVMatroxDigitizerInterface::GetGigeSerialNumber(const SVMatroxDigitizer& DigitizerID, std::string& Value)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MIL_INT len;
		MdigInquire(DigitizerID.m_DigitizerIdentifier, cGCSerialNumberSizeType, &len);
		l_Code =  SVMatroxApplicationInterface::GetLastStatus();
		if (l_Code == S_OK)
		{
			char* pSerialNo = new char[len + 1];
			MdigInquire(DigitizerID.m_DigitizerIdentifier, M_GC_SERIAL_NUMBER, pSerialNo);
			l_Code =  SVMatroxApplicationInterface::GetLastStatus();
			if (l_Code == S_OK)
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
@SVOperationName GetMACAddress

@SVOperationDescription This method gets the MAC Address for the Digitizer

*/

HRESULT SVMatroxDigitizerInterface::GetGigeMACAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value)
{
	HRESULT l_Code( S_OK );
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

HRESULT SVMatroxDigitizerInterface::GetGigeIPAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value)
{
	HRESULT l_Code( S_OK );
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
@SVOperationName Set

@SVOperationDescription This method sets the Control Camera Value for the Digitizer.

*/

HRESULT SVMatroxDigitizerInterface::EnableLastGrabInTrueBuffer(const SVMatroxDigitizer& DigitizerID, bool bEnable)
{
	HRESULT l_Code( S_OK );
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

HRESULT SVMatroxDigitizerInterface::Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVCameraEnum ControlType, double Value)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxType = 0;
		HRESULT hr = SVMatroxDigitizerControl::m_CameraEnumConvertor.ConvertEnumToMatroxType(ControlType, l_MatroxType);
		if (S_OK == hr)
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
@SVOperationName EnableCorruptedFrameMonitoring

@SVOperationDescription This method Enables/Disables the Corrupted Frame Monitoring for the Digitizer.

*/
HRESULT SVMatroxDigitizerInterface::EnableCorruptedFrameMonitoring(const SVMatroxDigitizer& DigitizerID, bool bEnable)
{
	HRESULT l_Code( S_OK );
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
@SVOperationName ShowGigeProperties

@SVOperationDescription This method shows the built-in Matrox Gige properties Dialog for the Digitizer.

*/
HRESULT SVMatroxDigitizerInterface::ShowGigeProperties(const SVMatroxDigitizer& DigitizerID)
{
	HRESULT l_Code( S_OK );
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
@SVOperationName GetFeature

@SVOperationDescription This method gets the Camera Feature from the XML camera data for the Digitizer

*/
HRESULT SVMatroxDigitizerInterface::GetFeature(const SVMatroxDigitizer& DigitizerID,
                                  const std::string& FeatureName,
                                  SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
                                  variant_t& FeatureValue)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MatroxType l_matroxFeatureType = 0;
		HRESULT hr = SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor.ConvertEnumToMatroxType(FeatureType, l_matroxFeatureType);
		if (S_OK == hr)
		{
			char featureNameStr[256];
			strcpy_s(featureNameStr, sizeof(featureNameStr), FeatureName.c_str());

			switch (FeatureValue.vt)
			{
				case VT_I1:		// Byte
				case VT_UI1: 
					{
						unsigned char value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
						{
							FeatureValue = value;
						}
					}
					break;

				case VT_I2:		// Short
				case VT_UI2: 
					{
						unsigned short value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
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
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
						{
							FeatureValue = value;
						}
					}
					break;

				case VT_I8:		// int64
				case VT_UI8:
					{
						__int64 value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
						{
							FeatureValue.llVal = value;
						}
					}
					break;

				case VT_BSTR:	// String
					{
						char value[256]; // how to determine size needed ahead of time ?
						
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
						{
							FeatureValue.bstrVal = _bstr_t(value);
						}
					}
					break;

				case VT_R4:		// Float
					{
						float value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
						{
							FeatureValue.fltVal = value;
						}
					}
					break;

				case VT_R8:		// Double
					{
						double value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
						{
							FeatureValue.dblVal = value;
						}
					}
					break;

				case VT_BOOL:	// Bool
					{
						bool value;
						MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						if (l_Code == S_OK)
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
HRESULT SVMatroxDigitizerInterface::SetFeature(const SVMatroxDigitizer& DigitizerID,
                                 const std::string& FeatureName,
                                 SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
                                 const variant_t& FeatureValue)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		MatroxType l_matroxFeatureType = 0;
		MIL_INT64 controlType = cFeatureControlType;
#if SV_DESIRED_MIL_VERSION != 0x0900
		if (SVMatroxDigitizerFeature::SVTypeCommand == FeatureType)
		{
			controlType = M_FEATURE_EXECUTE;
		}
#endif
		HRESULT hr = SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor.ConvertEnumToMatroxType(FeatureType, l_matroxFeatureType);
		if (S_OK == hr)
		{
			char featureNameStr[256];
			strcpy_s(featureNameStr, sizeof(featureNameStr), FeatureName.c_str());

			switch (FeatureValue.vt)
			{
				case VT_EMPTY: // M_DEFAULT or M_NULL
					MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, M_NULL);
					l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					break;

				case VT_I1:		// Byte
				case VT_UI1: 
					{
						unsigned char value = FeatureValue.bVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_I2:		// Short
				case VT_UI2:
					{
						short value = FeatureValue.iVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_I4:		// long
				case VT_UI4:
				case VT_INT:	// INT, UINT
				case VT_UINT:
					{
						long value = FeatureValue.lVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_I8:		// int64
				case VT_UI8:
					{
						__int64 value = FeatureValue.llVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_BSTR:	// String
					{
						std::string strValue = SvUl::createStdString(FeatureValue.bstrVal);
						size_t len = strValue.size() + 1;
						char* pValue = new char[len];
						strcpy_s(pValue, len, strValue.c_str()); 
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, pValue);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
						delete [] pValue;
					}
					break;

				case VT_R4:		// Float
					{
						float value = FeatureValue.fltVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_R8:		// Double
					{
						double value = FeatureValue.dblVal;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, &value);
						l_Code =  SVMatroxApplicationInterface::GetLastStatus();
					}
					break;

				case VT_BOOL:	// Bool
					{
						bool value = (FeatureValue.boolVal) ? true : false;
						MdigControlFeature(DigitizerID.m_DigitizerIdentifier, controlType, featureNameStr, l_matroxFeatureType, &value);
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
HRESULT SVMatroxDigitizerInterface::Grab(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId)
{
	HRESULT l_Code( S_OK );
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
HRESULT SVMatroxDigitizerInterface::GrabContinuous(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId)
{
	HRESULT l_Code( S_OK );
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
HRESULT SVMatroxDigitizerInterface::GrabWait(const SVMatroxDigitizer& DigitizerID, long Flag)
{
	HRESULT l_Code( S_OK );
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

HRESULT SVMatroxDigitizerInterface::StartGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxOperationFlag = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabModeEnumConvertor.ConvertEnumToMatroxType(grabMode, l_MatroxOperationFlag);
		if (S_OK == hr)
		{
			SVMatroxBufferArray::BufferIDList l_List = p_rBufferArray.GetList(); 
			long numBuffers = static_cast< long >( l_List.size() );
			SVMatroxIdentifier* pList = nullptr;

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
HRESULT SVMatroxDigitizerInterface::StopGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long l_MatroxOperationFlag = 0;
		HRESULT hr = SVMatroxDigitizerGrab::m_GrabModeEnumConvertor.ConvertEnumToMatroxType(grabMode, l_MatroxOperationFlag);
		if (S_OK == hr)
		{
			SVMatroxBufferArray::BufferIDList l_List = p_rBufferArray.GetList(); 
			long numBuffers = static_cast< long >( l_List.size() );
			SVMatroxIdentifier* pList = nullptr;

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
HRESULT SVMatroxDigitizerInterface::Halt(const SVMatroxDigitizer& DigitizerID)
{
	HRESULT l_Code( S_OK );
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
HRESULT SVMatroxDigitizerInterface::Destroy(SVMatroxDigitizer& p_rDigitizer)
{
	HRESULT l_Code( SVMatroxDigitizerInterface::Release(p_rDigitizer) );
	
	// Clear the handle ?
	p_rDigitizer.m_DigitizerIdentifier = 0;

	return l_Code;
}

/**
@SVOperationName SetGigeEvent

@SVOperationDescription This method Enables/Disables Event Notification for the Digitizer.

*/
HRESULT SVMatroxDigitizerInterface::SetGigeEvent(const SVMatroxDigitizer& DigitizerID, const std::string& eventName, const std::string& value)
{
	HRESULT l_Code( S_OK );

	MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("EventSelector"), cFeatureControlValueEnumType, eventName.c_str());
	l_Code =  SVMatroxApplicationInterface::GetLastStatus();

	MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("EventNotification"), cFeatureControlValueEnumType, value.c_str());
	l_Code =  SVMatroxApplicationInterface::GetLastStatus();
	return l_Code;
}

/**
@SVOperationName GetGigeEventList

@SVOperationDescription This method emunerates the Event Names to get the EventType Ids for the Digitizer.

*/

HRESULT SVMatroxDigitizerInterface::GetGigeEventList(const SVMatroxDigitizer& DigitizerID, SVGigeEventList& list)
{
	HRESULT l_Code( S_OK );
	SVMatroxInt EventCnt = 0;
	
#if SV_DESIRED_MIL_VERSION == 0x0900
	MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_ELEMENT_COUNT, MIL_TEXT("EventSelector"), M_TYPE_STRING_ENUMERATION, &EventCnt);
#else
	MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_FEATURE_ENUM_ENTRY_COUNT, MIL_TEXT("EventSelector"), M_DEFAULT, &EventCnt);
#endif
	l_Code = SVMatroxApplicationInterface::GetLastStatus();

	if (l_Code == S_OK && EventCnt)
	{
		for ( SVMatroxInt i = 0; i < EventCnt && l_Code == S_OK; i++ )
		{
			SVMatroxInt Len = 0;

#if SV_DESIRED_MIL_VERSION == 0x0900
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_ELEMENT + M_LENGTH + i, MIL_TEXT("EventSelector"), M_TYPE_STRING_ENUMERATION, &Len);
#else
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_FEATURE_ENUM_ENTRY_NAME + M_STRING_SIZE + i, MIL_TEXT("EventSelector"), M_DEFAULT, &Len);
#endif

			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			
			if (l_Code == S_OK && Len)
			{
				MIL_TEXT_PTR pEventName = new MIL_TEXT_CHAR[Len];
				
#if SV_DESIRED_MIL_VERSION == 0x0900
				MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_ELEMENT + i, MIL_TEXT("EventSelector"), M_TYPE_STRING_ENUMERATION, pEventName);
#else
				MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_FEATURE_ENUM_ENTRY_NAME + i, MIL_TEXT("EventSelector"), M_DEFAULT, pEventName);
#endif
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				
				if (l_Code == S_OK)
				{
					std::string name(pEventName); // MIL_TEXT is null terminated
					SVMatroxInt eventType = -1;
					std::string evName = "Event";
					evName += name;
					MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, evName.c_str(), M_TYPE_MIL_INT, &eventType);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == S_OK)
					{
						list.insert(std::make_pair(name, static_cast<long>(eventType)));
					}
				}
				delete [] pEventName;
			}
		}
	}
	return l_Code;
}
