//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGigeDigitizer
//* .File Name       : $Workfile:   SVMatroxGigeDigitizer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 14:43:56  $
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "SVMatroxGigeDigitizer.h"
#include "SVGigeCameraParametersLibrary/SVGigeCameraDBParser.h"
#pragma endregion Includes

#define NUM_ACQUISITION_BUFFERS 5

SVMatroxGigeDigitizer::SVMatroxGigeDigitizer(unsigned long deviceNumber, unsigned char systemHandle)
: m_deviceNumber(deviceNumber)
, m_SystemHandle(systemHandle)
, m_Handle(0xFF)
, m_lIsStarted(false)
, m_lStartIndex(-1)
, m_lLastUsedIndex(-1)
, m_pBufferInterface(nullptr)
, m_StartFrameTimeStamp(0) 
, m_lineState(false)
{
	m_params.TriggerType = SVMatroxGigeTrigger::HardwareTrigger;
	m_params.TriggerMode = SVMatroxDigitizerGrab::SVEdgeRising;
}

SVMatroxGigeDigitizer::SVMatroxGigeDigitizer( const SVMatroxGigeDigitizer& p_rValue)
{
	*this = p_rValue;
}

SVMatroxGigeDigitizer::~SVMatroxGigeDigitizer()
{
}

const SVMatroxGigeDigitizer& SVMatroxGigeDigitizer::operator=( const SVMatroxGigeDigitizer& p_rValue )
{
	if (this != &p_rValue)
	{
		m_deviceNumber = p_rValue.m_deviceNumber;
		m_SystemHandle = p_rValue.m_SystemHandle;
		m_Handle = p_rValue.m_Handle;
		m_Digitizer = p_rValue.m_Digitizer;

		m_Name = p_rValue.m_Name;
		m_FullName = p_rValue.m_FullName;

		m_svCameraId = p_rValue.m_svCameraId;
		m_lIsStarted = p_rValue.m_lIsStarted;

		m_lStartIndex = p_rValue.m_lStartIndex;
		m_lLastUsedIndex = p_rValue.m_lLastUsedIndex;

		m_StartFrameTimeStamp = p_rValue.m_StartFrameTimeStamp ;
		m_lineState = p_rValue.m_lineState;

		m_params = p_rValue.m_params;

		m_AcqBuffers = p_rValue.m_AcqBuffers;

		m_triggerCallbackInfo = p_rValue.m_triggerCallbackInfo;

		m_lineInputMoniker = p_rValue.m_lineInputMoniker;
		m_lineInputRisingEventName = p_rValue.m_lineInputRisingEventName;
		m_lineInputFallingEventName = p_rValue.m_lineInputFallingEventName;
		m_eventList = p_rValue.m_eventList;
	}
	return *this;
}

HRESULT SVMatroxGigeDigitizer::CreateAcquisitionBuffers(const SVMatroxSystem& rSystem, unsigned long width, unsigned long height, unsigned long format)
{
	AcqBufferCreator l_creator;
	SVMatroxBufferCreateStruct l_createStruct;
	HRESULT hr = l_creator.BuildCreateStruct(width, height, format, l_createStruct);
	if (S_OK == hr)
	{
		hr = m_AcqBuffers.Create(rSystem, l_createStruct, NUM_ACQUISITION_BUFFERS);
	}
	return hr;
}

HRESULT SVMatroxGigeDigitizer::DestroyAcquistionBuffers()
{
	m_AcqBuffers.Destroy();
	return S_OK;
}

HRESULT SVMatroxGigeDigitizer::StartGrabArray(SVMatroxDigitizerInterface::SVGrabProcessFunc func)
{
	HRESULT hr = SVMatroxDigitizerInterface::EnableCorruptedFrameMonitoring(*(m_Digitizer.get()), true);
	if (S_OK == hr)
	{
		hr = SVMatroxDigitizerInterface::StartGrabArray(*(m_Digitizer.get()), m_AcqBuffers, SVMatroxDigitizerGrab::SVGrabAsynchronous, func, this);
	}
	return hr;
}

HRESULT SVMatroxGigeDigitizer::StopGrabArray(SVMatroxDigitizerInterface::SVGrabProcessFunc func)
{
	HRESULT hr = SVMatroxDigitizerInterface::EnableCorruptedFrameMonitoring(*(m_Digitizer.get()), false);
	hr = SVMatroxDigitizerInterface::StopGrabArray(*(m_Digitizer.get()), m_AcqBuffers, SVMatroxDigitizerGrab::SVGrabSynchronous, func, this);
	
	return hr;
}

HRESULT SVMatroxGigeDigitizer::BuildGigeFeaturesMap(BSTR xmlData)
{
	m_featuresMap.clear();

	HRESULT hr = SVGigeCameraDBParser::Parse(xmlData, m_featuresMap);

	return hr;
}

const SVGigeDeviceParameterStruct& SVMatroxGigeDigitizer::GetFeature(SVGigeParameterEnum paramEnum) const
{
	SVGigeDeviceParameterMap::const_iterator featureIt = m_featuresMap.find(paramEnum);
	if (featureIt != m_featuresMap.end())
	{
		return featureIt->second;
	}

	// create an empty one
	static SVGigeDeviceParameterStruct emptyStruct
	(
		"", VT_EMPTY, 
		SVGigeParameterAccessor
		(
			SVGigeFeature
			(
				"", 
				SVMatroxDigitizerFeature::SVTypeBool, 
				SVGigeFeature::NotSupported, 
				SVGigeFeature::ReadOnly
			)
		)
	);
	return emptyStruct;
}

unsigned long SVMatroxGigeDigitizer::GetDeviceNumber() const
{
	return m_deviceNumber;
}

bool SVMatroxGigeDigitizer::HasLineInput() const
{
	return !m_lineInputMoniker.empty();
}

void SVMatroxGigeDigitizer::SetLineInputMoniker(const SVString& moniker)
{
	m_lineInputMoniker = moniker.c_str();
	m_lineInputRisingEventName = GetLineInputRisingEventName();
	m_lineInputFallingEventName = GetLineInputFallingEventName();
}

SVString SVMatroxGigeDigitizer::GetLineInputRisingEventName() const
{
	const SVGigeDeviceParameterStruct& deviceParamStruct = GetFeature(SVGigeParameterInputEventName);
	
	SVGigeFeatureString featureName;
	if (deviceParamStruct.accessor.feature.HasTranslation())
	{
		deviceParamStruct.accessor.feature.GetGigeFeatureString(m_lineInputMoniker + " Rising", featureName);
	}
	return featureName.c_str();
}

SVString SVMatroxGigeDigitizer::GetLineInputFallingEventName() const
{
	const SVGigeDeviceParameterStruct& deviceParamStruct = GetFeature(SVGigeParameterInputEventName);
	
	SVGigeFeatureString featureName;
	if (deviceParamStruct.accessor.feature.HasTranslation())
	{
		deviceParamStruct.accessor.feature.GetGigeFeatureString(m_lineInputMoniker + " Falling", featureName);
	}
	return featureName.c_str();
}

bool SVMatroxGigeDigitizer::IsAcquisitionTriggered() const
{
	return m_params.bAcquisitionTriggered;
}

void SVMatroxGigeDigitizer::SetAcquisitionTriggered(bool bAcquisitionTriggered)
{
	m_params.bAcquisitionTriggered = bAcquisitionTriggered;
}

bool SVMatroxGigeDigitizer::IsGigeEventListValid() const
{
	return (m_eventList.size() > 0);
}

HRESULT SVMatroxGigeDigitizer::GetGigeEventList()
{
	m_eventList.clear();
	return SVMatroxDigitizerInterface::GetGigeEventList(*(m_Digitizer.get()), m_eventList);
}

long SVMatroxGigeDigitizer::GetLineRisingEvent() const
{
	long eventId = 0;
	SVGigeEventList::const_iterator it = m_eventList.find(m_lineInputRisingEventName);
	if (it != m_eventList.end())
	{
		eventId = it->second;
	}
	return eventId;
}

long SVMatroxGigeDigitizer::GetLineFallingEvent() const
{
	long eventId = 0;
	SVGigeEventList::const_iterator it = m_eventList.find(m_lineInputFallingEventName);
	if (it != m_eventList.end())
	{
		eventId = it->second;
	}
	return eventId;
}

bool SVMatroxGigeDigitizer::GetLineState() const
{
	return m_lineState;
}

void SVMatroxGigeDigitizer::SetLineState(bool bState)
{
	m_lineState = bState;
}

const SvTh::TriggerCallbackInformation& SVMatroxGigeDigitizer::GetTriggerCallback() const
{
	return m_triggerCallbackInfo;
}

void SVMatroxGigeDigitizer::SetTriggerCallback(const SvTh::TriggerCallbackInformation& callback)
{
	m_triggerCallbackInfo = callback;
}

void SVMatroxGigeDigitizer::ClearTriggerCallback()
{
	m_triggerCallbackInfo.m_pCallback = nullptr;
	m_triggerCallbackInfo.m_TriggerParameters = SvTh::TriggerParameters();
}

