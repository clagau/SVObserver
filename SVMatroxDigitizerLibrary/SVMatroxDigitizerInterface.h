//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerInterface
//* .File Name       : $Workfile:   SVMatroxDigitizerInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Oct 2013 10:45:30  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <comdef.h>
#include "SVUtilityLibrary/SVString.h"
#include "SVMatroxLibrary/SVMatroxStatusInformation.h"
#include "SVMatroxLibrary/SVMatroxSystem.h"
#include "SVMatroxLibrary/SVMatroxBuffer.h"
#include "SVMatroxLibrary/SVMatroxBufferArray.h"
#include "SVMatroxDigitizer.h"
#include "SVMatroxDigitizerControlEnums.h"
#include "SVMatroxDigitizerInquireEnums.h"
#include "SVMatroxDigitizerHookEnums.h"
#include "SVMatroxDigitizerFeatureEnums.h"
#include "SVMatroxDigitizerGrabEnums.h"
#pragma endregion Includes

typedef std::map<SVString, long> SVGigeEventList;

class SVMatroxDigitizerInterface
{
public:
	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;
	typedef SVMatroxHookFunctionPtr SVGrabProcessFunc;

	static SVStatusCode Allocate(const SVMatroxSystem& System, long DeviceNum, SVMatroxDigitizer& digitizerID);
	static SVStatusCode Allocate(const SVMatroxSystem& System, long DeviceNum, const SVString& DataFormat, SVMatroxDigitizer& digitizerID);
	static SVStatusCode Release(SVMatroxDigitizer& DigitizerID);

	static SVStatusCode IsCorruptedFrame(SVMatroxIdentifier milEventID, bool& bIsCorrupted);
	static SVStatusCode GetGrabTimeStamp(const SVMatroxDigitizer& DigitizerID, double& timestamp);
	static SVStatusCode GetGrabBuffer(SVMatroxIdentifier milEventID, SVMatroxIdentifier& bufferID);

	static SVStatusCode GetHookInfo(SVMatroxIdentifier milEventID, SVMatroxDigitizerHook::SVHookInfoEnum InfoType, void* UserPtr);
	
	static SVStatusCode SetHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr,
                                  void* UserDataPtr);

	static SVStatusCode ReleaseHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr,
								  void* UserDataPtr);

	static bool IsEventGrabFrameStart(SVMatroxIdentifier EventType);
	static bool IsEventGrabFrameEnd(SVMatroxIdentifier EventType);
	static bool IsEventErrorFrameNotify(SVMatroxIdentifier EventType);
	static bool IsEventDevicePresentNotify(SVMatroxIdentifier EventType);

	// Getters
	// Board
	static SVStatusCode Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVBoardEnum InquireType, long& Value);

	// Camera
	static SVStatusCode GetVendor(const SVMatroxDigitizer& DigitizerID, SVString& Value);
	static SVStatusCode GetModel(const SVMatroxDigitizer& DigitizerID, SVString& Value);

	static SVStatusCode GetColorMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVColorModeEnum& Value);
	static SVStatusCode GetScanMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVScanModeEnum& Value);
	
	static SVStatusCode GetBitDepth(const SVMatroxDigitizer& DigitizerID, long& bitDepth);
	static SVStatusCode GetNumberOfBands(const SVMatroxDigitizer& DigitizerID, long& numBands);

	static SVStatusCode GetOffset(const SVMatroxDigitizer& DigitizerID, long& XOffset, long& YOffset);
	static SVStatusCode SetOffset(const SVMatroxDigitizer& DigitizerID, long XOffset, long YOffset);

	static SVStatusCode GetSize(const SVMatroxDigitizer& DigitizerID, long& X, long& Y);
	static SVStatusCode SetSize(const SVMatroxDigitizer& DigitizerID, long X, long Y);

	static SVStatusCode GetFormat(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerFormat& format);
	
	// Grab
//	static SVStatusCode Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVGrabEnum InquireType, _variant_t& Value);

	// Frame
	static SVStatusCode Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVFrameEnum InquireType, long& Value);

	// Gige
	static SVStatusCode GetGigeSerialNumber(const SVMatroxDigitizer& DigitizerID, SVString& Value);
	static SVStatusCode GetGigeMACAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value);
	static SVStatusCode GetGigeIPAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value);
	static SVStatusCode ShowGigeProperties(const SVMatroxDigitizer& DigitizerID);

	// Setters
	static SVStatusCode EnableLastGrabInTrueBuffer(const SVMatroxDigitizer& DigitizerID, bool bEnable);
	static SVStatusCode Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVCameraEnum ControlType, double Value);

	static SVStatusCode EnableCorruptedFrameMonitoring(const SVMatroxDigitizer& DigitizerID, bool bEnable);

	// Gige Feature Getter/Setter
	static SVStatusCode GetFeature(const SVMatroxDigitizer& DigitizerID,
									const SVString& FeatureName,
									SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
									variant_t& FeatureValue);

	static SVStatusCode SetFeature(const SVMatroxDigitizer& DigitizerID,
                                 const SVString& FeatureName,
                                 SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
                                 const variant_t& FeatureValue);

	// Grab 
	static SVStatusCode GrabInProgress(const SVMatroxDigitizer& DigitizerID, bool& bInProgress);
	static SVStatusCode Grab(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId);
	static SVStatusCode GrabContinuous(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId);
	static SVStatusCode GrabWait(const SVMatroxDigitizer& DigitizerID, long Flag);

	static SVStatusCode StartGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData);
	static SVStatusCode StopGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData);

	static SVStatusCode Halt(const SVMatroxDigitizer& DigitizerID);
	static SVStatusCode AbortGrab(const SVMatroxDigitizer& DigitizerID);
	static SVStatusCode SetGrabMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode);
	static SVStatusCode SetGrabTimeout(const SVMatroxDigitizer& DigitizerID, long value);
	static SVStatusCode SetInfiniteGrabTimeout(const SVMatroxDigitizer& DigitizerID);

	// helper
	static SVStatusCode Destroy(SVMatroxDigitizer& p_rDigitizer);

	static SVStatusCode GetGigeCameraTimestamp(SVMatroxIdentifier milEventID, double& timestamp);
	static SVStatusCode GetGigeEventType(SVMatroxIdentifier milEventID, long& p_rEventType);
	static SVStatusCode SetGigeEvent(const SVMatroxDigitizer& DigitizerID, const SVString& name, const SVString& value);
	static SVStatusCode GetGigeEventList(const SVMatroxDigitizer& DigitizerID, SVGigeEventList& list);
};

