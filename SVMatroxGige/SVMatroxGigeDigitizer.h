//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxGigeCameraId.h"
#include "SVMatroxGigeAcqParams.h"
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"
#include "SVMatroxDigitizerLibrary/SVMatroxDigitizerInterface.h"
#include "SVMatroxLibrary/SVMatroxBufferArray.h"
#pragma endregion Includes

#pragma region Declarations
struct SVGigeDeviceParameterStruct;
class SVAcquisitionBufferInterface;
#pragma endregion Declarations

class SVConditionVariable
{
private:
	volatile long m_value {0L};

public:
	void Increment() { ::InterlockedIncrement(&m_value); }
	void clear() { ::InterlockedExchange(&m_value, 0); }
	long get() const { return m_value; }
};

class SVMatroxGigeDigitizer
{
private:
	SVMatroxBufferArray m_AcqBuffers;
	SVGigeDeviceParameterMap m_featuresMap;
	long m_deviceNumber{0L};
	std::string m_lineInputMoniker;
	std::string m_lineInputRisingEventName;
	std::string m_lineInputFallingEventName;
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

	HRESULT StartGrabArray(SVMatroxDigitizerInterface::SVMatroxHookFunctionPtr func);
	HRESULT StopGrabArray(SVMatroxDigitizerInterface::SVMatroxHookFunctionPtr func);

	HRESULT BuildGigeFeaturesMap(BSTR xmlData);
	const SVGigeDeviceParameterStruct& GetFeature(SvDef::SVGigeParameterEnum paramEnum) const;

	long GetDeviceNumber() const { return m_deviceNumber; }

	bool HasLineInput() const;
	void SetLineInputMoniker(const std::string& moniker);
	std::string GetLineInputRisingEventName() const;
	std::string GetLineInputFallingEventName() const;

	bool IsGigeEventListValid() const;
	HRESULT GetGigeEventList();

#pragma region Member Variables
public:
	std::string m_Name;
	std::string m_FullName;

	unsigned char m_SystemHandle{0};
	unsigned char m_Handle{0xff};

	long m_lIsStarted{false}; // long (not bool) because of InterlockedExchange...

	// Image processing buffers in use
	long m_lStartIndex{-1};
	long m_lLastUsedIndex{-1};

	SVConditionVariable m_frameStack {};
	SVMatroxDigitizerPtr m_Digitizer {};
	SVMatroxGigeCameraId m_svCameraId {};

	double m_StartFrameTimeStamp{0.0};
	bool m_chunkData {false};

	SVMatroxGigeAcqParams m_params;

	SVAcquisitionBufferInterface* m_pBufferInterface{nullptr};

private:
#pragma endregion Member Variables
	HRESULT BuildCreateStruct(unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxBufferCreateStruct& rCreateStruct);
};
