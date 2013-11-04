//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxDigitizerInterface
//* .File Name       : $Workfile:   SVMatroxDigitizerInterface.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:29:06  $
//******************************************************************************

#ifndef SVMATROXDIGITIZERINTERFACE_H
#define SVMATROXDIGITIZERINTERFACE_H

#include <map>
#include <comdef.h>
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
#include "SVMatroxDigitizerLightReferenceEnums.h"

typedef std::map<SVMatroxString, long> SVGigeEventList;

class SVMatroxDigitizerInterface
{
public:
	typedef SVMatroxStatusInformation::SVStatusCode SVStatusCode;
	typedef SVMatroxHookFunctionPtr SVGrabProcessFunc;

	static SVStatusCode Allocate(const SVMatroxSystem& System, long DeviceNum, SVMatroxDigitizer& digitizerID);
	static SVStatusCode Allocate(const SVMatroxSystem& System, long DeviceNum, const SVMatroxString& DataFormat, SVMatroxDigitizer& digitizerID);
	static SVStatusCode Release(SVMatroxDigitizer& DigitizerID);

	static SVStatusCode Channel(const SVMatroxDigitizer& DigitizerID, long Channel);

	static SVStatusCode IsCorruptedFrame(long milEventID, bool& bIsCorrupted);
	static SVStatusCode GetGrabTimeStamp(const SVMatroxDigitizer& DigitizerID, double& timestamp);
	static SVStatusCode GetGrabBuffer(long milEventID, SVMatroxIdentifier& bufferID);

	static SVStatusCode GetHookInfo(long milEventID, SVMatroxDigitizerHook::SVHookInfoEnum InfoType, void* UserPtr);
	
	static SVStatusCode SetHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr,
                                  void* UserDataPtr);

	static SVStatusCode ReleaseHookFunction(const SVMatroxDigitizer& DigitizerID,
                                  SVMatroxDigitizerHook::SVHookTypeEnum HookType,
                                  SVMatroxHookFunctionPtr HookHandlerPtr,
								  void* UserDataPtr);

	static bool IsEventGrabFrameStart(long EventType);
	static bool IsEventGrabFrameEnd(long EventType);
	static bool IsEventErrorFrameNotify(long EventType);
	static bool IsEventDevicePresentNotify(long EventType);

	// Getters
	// Board
	static SVStatusCode Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVBoardEnum InquireType, long& Value);

	// Camera
	static SVStatusCode GetVendor(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value);
	static SVStatusCode GetModel(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value);

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
	
	// User Bits
	static SVStatusCode Get(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerInquire::SVUserBitEnum InquireType, long& Value);

	// Gige
	static SVStatusCode GetGigeVersion(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value);
	static SVStatusCode GetGigeSerialNumber(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value);
	static SVStatusCode GetGigeSpecificInfo(const SVMatroxDigitizer& DigitizerID, SVMatroxString& Value);
	static SVStatusCode GetGigeMACAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value);
	static SVStatusCode GetGigeIPAddress(const SVMatroxDigitizer& DigitizerID, __int64& Value);
	static SVStatusCode ShowGigeProperties(const SVMatroxDigitizer& DigitizerID);

	// Setters
	static SVStatusCode EnableLastGrabInTrueBuffer(const SVMatroxDigitizer& DigitizerID, bool bEnable);
	static SVStatusCode Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVCameraEnum ControlType, double Value);

	static SVStatusCode EnableCorruptedFrameMonitoring(const SVMatroxDigitizer& DigitizerID, bool bEnable);

	static SVStatusCode Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabEnum ControlType, const _variant_t& Value);
	static SVStatusCode Set(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerControl::SVUserBitEnum ControlType, const _variant_t& Value);

	// LUT
	static SVStatusCode GetLutID(const SVMatroxDigitizer& DigitizerID, SVMatroxIdentifier& LutBufId);
	static SVStatusCode GetLutNumberOfBands(const SVMatroxDigitizer& DigitizerID, long& Value);
	static SVStatusCode GetLutBandSize(const SVMatroxDigitizer& DigitizerID, long& Value);
	static SVStatusCode SetLut(const SVMatroxDigitizer& DigitizerID, const SVMatroxBuffer& LutBufId);

	// Light Reference
	static SVStatusCode GetReference(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerLightReference::SVLightReferenceEnum ReferenceType, double& ReferenceLevel);
	static SVStatusCode SetReference(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerLightReference::SVLightReferenceEnum ReferenceType, double ReferenceLevel);

	// Gige Feature Getter/Setter
	static SVStatusCode GetFeature(const SVMatroxDigitizer& DigitizerID,
									const SVMatroxString& FeatureName,
									SVMatroxDigitizerFeature::SVFeatureTypeEnum FeatureType,
									variant_t& FeatureValue);

	static SVStatusCode SetFeature(const SVMatroxDigitizer& DigitizerID,
                                 const SVMatroxString& FeatureName,
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

	static SVStatusCode SetGrabTriggerMode(const SVMatroxDigitizer& DigitizerID, SVMatroxDigitizerGrab::SVGrabTriggerModeEnum grabTriggerMode);
	static SVStatusCode SetGrabHardwareTriggerSource(const SVMatroxDigitizer& DigitizerID, long portNo);
	static SVStatusCode SetGrabSoftwareTriggerSource(const SVMatroxDigitizer& DigitizerID);
	static SVStatusCode FireSoftwareTrigger(const SVMatroxDigitizer& DigitizerID);
	static SVStatusCode EnableTrigger(const SVMatroxDigitizer& DigitizerID, bool bEnable);

	// helper
	static SVStatusCode Destroy(SVMatroxDigitizer& p_rDigitizer);

	static SVStatusCode GetGigeCameraTimestamp(long milEventID, double& timestamp);
	static SVStatusCode GetGigeEventType(long milEventID, long& p_rEventType);
	static SVStatusCode SetGigeEvent(const SVMatroxDigitizer& DigitizerID, const SVMatroxString& name, const SVMatroxString& value);
	static SVStatusCode GetGigeEventList(const SVMatroxDigitizer& DigitizerID, SVGigeEventList& list);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxDigitizerLibrary\SVMatroxDigitizerInterface.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:29:06   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   18 Jan 2013 15:36:54   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added GetGigeEventList method
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jun 2012 16:38:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   29 Jul 2009 16:13:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Jun 2009 15:32:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/