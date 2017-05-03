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
	typedef SVMatroxHookFunctionPtr SVGrabProcessFunc;

	static HRESULT Allocate(const SVMatroxSystem& System, long DeviceNum, SVMatroxDigitizer& digitizerID);
	static HRESULT Allocate(const SVMatroxSystem& System, long DeviceNum, const SVString& DataFormat, SVMatroxDigitizer& digitizerID);
	static HRESULT Release(SVMatroxDigitizer& DigitizerID);

	static HRESULT IsCorruptedFrame(SVMatroxIdentifier milEventID, bool& bIsCorrupted);
	static HRESULT GetGrabTimeStamp(const SVMatroxDigitizer& DigitizerID, double& timestamp);
	static HRESULT GetGrabBuffer(SVMatroxIdentifier milEventID, SVMatroxIdentifier& bufferID);

	static HRESULT GetHookInfo(SVMatroxIdentifier milEventID, SVMatroxDigitizerHook::SVHookInfoEnum InfoType, void* UserPtr);
	
	static HRESULT SetHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr,
                                  void* UserDataPtr);

	static HRESULT ReleaseHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr,
								  void* UserDataPtr);

	static bool IsEventGrabFrameStart(SVMatroxIdentifier EventType);
	static bool IsEventGrabFrameEnd(SVMatroxIdentifier EventType);
	static bool IsEventErrorFrameNotify(SVMatroxIdentifier EventType);
	static bool IsEventDevicePresentNotify(SVMatroxIdentifier EventType);

	// Getters
	// Board
	static HRESULT Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVBoardEnum InquireType, long& Value);

	// Camera
	static HRESULT GetVendor(const SVMatroxDigitizer& DigitizerID, SVString& Value);
	static HRESULT GetModel(const SVMatroxDigitizer& DigitizerID, SVString& Value);

	static HRESULT GetColorMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVColorModeEnum& Value);
	static HRESULT GetScanMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVScanModeEnum& Value);
	
	static HRESULT GetBitDepth(const SVMatroxDigitizer& DigitizerID, long& bitDepth);
	static HRESULT GetNumberOfBands(const SVMatroxDigitizer& DigitizerID, long& numBands);

	static HRESULT GetOffset(const SVMatroxDigitizer& DigitizerID, long& XOffset, long& YOffset);
	static HRESULT SetOffset(const SVMatroxDigitizer& DigitizerID, long XOffset, long YOffset);

	static HRESULT GetSize(const SVMatroxDigitizer& DigitizerID, long& X, long& Y);
	static HRESULT SetSize(const SVMatroxDigitizer& DigitizerID, long X, long Y);

	static HRESULT GetFormat(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerFormat& format);
	
	// Grab
//	static HRESULT Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVGrabEnum InquireType, _variant_t& Value);

	// Frame
	static HRESULT Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVFrameEnum InquireType, long& Value);

	// Gige
	static HRESULT GetGigeSerialNumber(const SVMatroxDigitizer& DigitizerID, SVString& Value);
	static HRESULT GetGigeMACAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value);
	static HRESULT GetGigeIPAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value);
	static HRESULT ShowGigeProperties(const SVMatroxDigitizer& DigitizerID);

	// Setters
	static HRESULT EnableLastGrabInTrueBuffer(const SVMatroxDigitizer& DigitizerID, bool bEnable);
	static HRESULT Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVCameraEnum ControlType, double Value);

	static HRESULT EnableCorruptedFrameMonitoring(const SVMatroxDigitizer& DigitizerID, bool bEnable);

	// Gige Feature Getter/Setter
	static HRESULT GetFeature(const SVMatroxDigitizer& DigitizerID,
									const SVString& FeatureName,
									SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
									variant_t& FeatureValue);

	static HRESULT SetFeature(const SVMatroxDigitizer& DigitizerID,
                                 const SVString& FeatureName,
                                 SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
                                 const variant_t& FeatureValue);

	// Grab 
	static HRESULT GrabInProgress(const SVMatroxDigitizer& DigitizerID, bool& bInProgress);
	static HRESULT Grab(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId);
	static HRESULT GrabContinuous(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& DestImageId);
	static HRESULT GrabWait(const SVMatroxDigitizer& DigitizerID, long Flag);

	static HRESULT StartGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData);
	static HRESULT StopGrabArray(const SVMatroxDigitizer& DigitizerID, SVMatroxBufferArray& p_rBufferArray, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode, SVGrabProcessFunc func, void* userData);

	static HRESULT Halt(const SVMatroxDigitizer& DigitizerID);
	static HRESULT AbortGrab(const SVMatroxDigitizer& DigitizerID);
	static HRESULT SetGrabMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabModeEnum grabMode);
	static HRESULT SetGrabTimeout(const SVMatroxDigitizer& DigitizerID, long value);
	static HRESULT SetInfiniteGrabTimeout(const SVMatroxDigitizer& DigitizerID);

	// helper
	static HRESULT Destroy(SVMatroxDigitizer& p_rDigitizer);

	static HRESULT GetGigeCameraTimestamp(SVMatroxIdentifier milEventID, double& timestamp);
	static HRESULT GetGigeEventType(SVMatroxIdentifier milEventID, long& p_rEventType);
	static HRESULT SetGigeEvent(const SVMatroxDigitizer& DigitizerID, const SVString& name, const SVString& value);
	static HRESULT GetGigeEventList(const SVMatroxDigitizer& DigitizerID, SVGigeEventList& list);
};

