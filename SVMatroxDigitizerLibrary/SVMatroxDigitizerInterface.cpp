//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxDigitizer.h"
#include "SVMatroxDigitizerInterface.h"
#include "SVMatroxLibrary/SVMatroxApplicationInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferArray.h"
#include "SVMatroxLibrary/SVMatroxSystem.h"
#include "SVMatroxLibrary/SVMatroxEnumConvertor.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

#pragma region Declarations
const MIL_INT cFeatureControlType = M_FEATURE_VALUE;
const MIL_INT cFeatureControlValueEnumType = M_TYPE_STRING;
const MIL_INT cCameraVendorSizeType = M_CAMERA_VENDOR_SIZE;
const MIL_INT cGCSerialNumberSizeType = M_GC_SERIAL_NUMBER_SIZE;
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
HRESULT SVMatroxDigitizerInterface::IsCorruptedFrame(__int64 milEventID, bool& bIsCorrupted)
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
@SVOperationName GetGrabBuffer

@SVOperationDescription This method gets the buffer that was grabbed into.
This method can only be called from the callback func used with MdigProcess

*/

HRESULT SVMatroxDigitizerInterface::GetGrabBuffer(__int64 milEventID, __int64& bufferID)
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
		long long l_MatroxHookType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerHook::m_HookTypeEnumConvertor, HookType, l_MatroxHookType);
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
		long long l_MatroxHookType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerHook::m_HookTypeEnumConvertor, HookType, l_MatroxHookType);
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

bool SVMatroxDigitizerInterface::IsEventGrabFrameStart(__int64 EventType)
{
	return ((EventType & M_GRAB_FRAME_START ) == M_GRAB_FRAME_START);
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
		long long l_MatroxType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerGrab::m_GrabModeEnumConvertor, grabMode, l_MatroxType);
		if (S_OK == hr)
		{
			MdigControl(DigitizerID.m_DigitizerIdentifier, M_GRAB_MODE, static_cast<MIL_INT32> (l_MatroxType));
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
		long long l_MatroxType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerInquire::m_BoardEnumConvertor, InquireType, l_MatroxType);
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
		long long l_MatroxType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerInquire::m_FrameEnumConvertor, InquireType, l_MatroxType);
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

HRESULT SVMatroxDigitizerInterface::Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVCameraEnum ControlType, double Value)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long long l_MatroxType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerControl::m_CameraEnumConvertor, ControlType, l_MatroxType);
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
		__int64 l_matroxFeatureType = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor, FeatureType, l_matroxFeatureType);
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
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == S_OK)
					{
						FeatureValue = value;
					}
				}
				break;

				case VT_I2:		// Short
				case VT_UI2:
				{
					MIL_INT value;
					MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == S_OK)
					{
						FeatureValue = static_cast<unsigned short>(value);
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
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
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
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == S_OK)
					{
						FeatureValue.SetString(value);
					}
				}
				break;

				case VT_R4:		// Float
				{
					double value;
					MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == S_OK)
					{
						FeatureValue.fltVal = static_cast<float>(value);
					}
				}
				break;

				case VT_R8:		// Double
				{
					double value;
					MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == S_OK)
					{
						FeatureValue.dblVal = value;
					}
				}
				break;

				case VT_BOOL:	// Bool
				{
					MIL_BOOL value {0L};
					MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, featureNameStr, l_matroxFeatureType, &value);
					l_Code = SVMatroxApplicationInterface::GetLastStatus();
					if (l_Code == S_OK)
					{
						FeatureValue.boolVal = value ? true : false;
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
		__int64 l_matroxFeatureType = 0;
		MIL_INT64 controlType = cFeatureControlType;
		if (SVMatroxDigitizerFeature::SVTypeCommand == FeatureType)
		{
			controlType = M_FEATURE_EXECUTE;
		}

		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerFeature::m_FeatureTypeEnumConvertor, FeatureType, l_matroxFeatureType);
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
						MIL_INT value = FeatureValue.iVal;
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
						double value = FeatureValue.fltVal;
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
						MIL_BOOL value = (FeatureValue.boolVal) ? 1L : 0L;
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
@SVOperationName StartGrabArray

@SVOperationDescription This method cause the Digitizer to perform a grab using an array of buffers.
It will process the buffer in a round robin fashion, wrapping back to the begining of the list.

*/

HRESULT SVMatroxDigitizerInterface::StartGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVMatroxHookFunctionPtr func, void* userData)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long long l_MatroxOperationFlag = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerGrab::m_GrabModeEnumConvertor, grabMode, l_MatroxOperationFlag);
		if (S_OK == hr)
		{
			SVMatroxBufferArray::BufferIDList l_List = p_rBufferArray.GetList(); 
			long numBuffers = static_cast< long >( l_List.size() );
			__int64* pList = nullptr;

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
HRESULT SVMatroxDigitizerInterface::StopGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVMatroxHookFunctionPtr func, void* userData)
{
	HRESULT l_Code( S_OK );
#ifdef USE_TRY_BLOCKS
	try
#endif
	{
		long long l_MatroxOperationFlag = 0;
		HRESULT hr = ConvertEnumToMatroxType(SVMatroxDigitizerGrab::m_GrabModeEnumConvertor, grabMode, l_MatroxOperationFlag);
		if (S_OK == hr)
		{
			SVMatroxBufferArray::BufferIDList l_List = p_rBufferArray.GetList(); 
			long numBuffers = static_cast< long >( l_List.size() );
			__int64* pList = nullptr;

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
	MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("EventSelector"), cFeatureControlValueEnumType, eventName.c_str());
	HRESULT code =  SVMatroxApplicationInterface::GetLastStatus();

	MdigControlFeature(DigitizerID.m_DigitizerIdentifier, cFeatureControlType, MIL_TEXT("EventNotification"), cFeatureControlValueEnumType, value.c_str());
	HRESULT code2 =  SVMatroxApplicationInterface::GetLastStatus();
	if (S_OK != code)
	{
		return code;
	}
	else
	{
		return code2;
	}
}

/**
@SVOperationName GetGigeEventList

@SVOperationDescription This method emunerates the Event Names to get the EventType Ids for the Digitizer.

*/

HRESULT SVMatroxDigitizerInterface::GetGigeEventList(const SVMatroxDigitizer& DigitizerID, SVGigeEventList& list)
{
	long long EventCnt = 0;
	MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_FEATURE_ENUM_ENTRY_COUNT, MIL_TEXT("EventSelector"), M_DEFAULT, &EventCnt);
	HRESULT l_Code = SVMatroxApplicationInterface::GetLastStatus();

	if (l_Code == S_OK && EventCnt)
	{
		for ( long long i = 0; i < EventCnt && l_Code == S_OK; i++ )
		{
			long long Len = 0;
			MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_FEATURE_ENUM_ENTRY_NAME + M_STRING_SIZE + i, MIL_TEXT("EventSelector"), M_DEFAULT, &Len);
			l_Code = SVMatroxApplicationInterface::GetLastStatus();
			
			if (l_Code == S_OK && Len)
			{
				MIL_TEXT_PTR pEventName = new MIL_TEXT_CHAR[Len];
				MdigInquireFeature(DigitizerID.m_DigitizerIdentifier, M_FEATURE_ENUM_ENTRY_NAME + i, MIL_TEXT("EventSelector"), M_DEFAULT, pEventName);
				l_Code = SVMatroxApplicationInterface::GetLastStatus();
				
				if (l_Code == S_OK)
				{
					std::string name(pEventName); // MIL_TEXT is null terminated
					long long eventType = -1;
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
