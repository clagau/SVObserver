//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDigitizer
//* .File Name       : $Workfile:   SVMatroxGigeDigitizer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   08 May 2013 15:22:50  $
//******************************************************************************
#pragma once

#pragma region Includes
//Moved to precompiled header: #include <deque>
//Moved to precompiled header: #include <stack>
//Moved to precompiled header: #include <vector>
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVTimerLibrary/SVClock.h"

#include "SVMatroxGigeCameraId.h"
#include "SVMatroxGigeAcqParams.h"
#include "SVMatroxGigeBufferCreator.h"
#include "TriggerHandling/TriggerBasics.h"
#pragma endregion Includes

#pragma region Declarations
class SVAcquisitionBufferInterface;
#pragma endregion Declarations

class SVConditionVariable
{
private:
	volatile long m_value;

public:
	void Increment() { ::InterlockedIncrement(&m_value); }
	void clear() { ::InterlockedExchange(&m_value, 0); }
	long get() const { return m_value; }
};

class SVMatroxGigeDigitizer
{
private:
	typedef SVMatroxGigeBufferCreator<SVBufAttGrabImageProcDib> AcqBufferCreator;
	SVMatroxBufferArray m_AcqBuffers;
	SVGigeDeviceParameterMap m_featuresMap;
	unsigned long m_deviceNumber;
	std::string m_lineInputMoniker;
	std::string m_lineInputRisingEventName;
	std::string m_lineInputFallingEventName;
	bool m_lineState;
	SVGigeEventList m_eventList;

public:
#pragma region Constructor
	SVMatroxGigeDigitizer(unsigned long deviceNumber=-1, unsigned char systemHandle=0xFF);
	SVMatroxGigeDigitizer( const SVMatroxGigeDigitizer& p_rValue );
	virtual ~SVMatroxGigeDigitizer();
#pragma endregion Constructor

	const SVMatroxGigeDigitizer& operator=( const SVMatroxGigeDigitizer& p_rValue );

	// Buffer Management
	HRESULT CreateAcquisitionBuffers(const SVMatroxSystem& rSystem, unsigned long width, unsigned long height, unsigned long format);
	HRESULT DestroyAcquistionBuffers();

	HRESULT StartGrabArray(SVMatroxDigitizerInterface::SVGrabProcessFunc func);
	HRESULT StopGrabArray(SVMatroxDigitizerInterface::SVGrabProcessFunc func);

	HRESULT BuildGigeFeaturesMap(BSTR xmlData);
	const SVGigeDeviceParameterStruct& GetFeature(SVGigeParameterEnum paramEnum) const;

	unsigned long GetDeviceNumber() const;

	bool HasLineInput() const;
	void SetLineInputMoniker(const std::string& moniker);
	std::string GetLineInputRisingEventName() const;
	std::string GetLineInputFallingEventName() const;

	bool IsAcquisitionTriggered() const;
	void SetAcquisitionTriggered(bool bAcquisitionTriggered);

	bool IsGigeEventListValid() const;
	HRESULT GetGigeEventList();
	long GetLineRisingEvent() const;
	long GetLineFallingEvent() const;

	void SetLineState(bool bState);
	bool GetLineState() const;

	const SvTh::TriggerDispatcher& GetTriggerDispatcher() const;
	void SetTriggerDispatcher(const SvTh::TriggerDispatcher& rDispatcher);
	void ClearTriggerCallback();

#pragma region Member Variables
public:
	std::string m_Name;
	std::string m_FullName;

	unsigned char m_SystemHandle;
	unsigned char m_Handle;

	long m_lIsStarted; // long (not bool) because of InterlockedExchange...

	// Image processing buffers in use
	long m_lStartIndex;
	long m_lLastUsedIndex;

	SVConditionVariable m_frameStack;
	SVMatroxDigitizerRef m_Digitizer;
	SVMatroxGigeCameraId m_svCameraId;

	SvTl::SVTimeStamp m_StartFrameTimeStamp;

	SVMatroxGigeAcqParams m_params;

	SVAcquisitionBufferInterface* m_pBufferInterface;

private:
	SvTh::TriggerDispatcher m_dispatcher;
#pragma endregion Member Variables
};

