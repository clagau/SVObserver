//******************************************************************************
//* COPYRIGHT (c) 2009 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "SVMatroxLibrary/SVMatroxBufferCreateStruct.h"
#include "SVGigeCameraParametersLibrary/SVGigeCameraDBParser.h"
#include "SVGigeCameraParametersLibrary/SVGigeDeviceParameterStruct.h"
#pragma endregion Includes

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define NUM_ACQUISITION_BUFFERS 5

SVMatroxGigeDigitizer::SVMatroxGigeDigitizer(unsigned long deviceNumber, unsigned char systemHandle)
: m_deviceNumber(deviceNumber)
, m_SystemHandle(systemHandle)
{
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

		m_params = p_rValue.m_params;

		m_AcqBuffers = p_rValue.m_AcqBuffers;

		m_lineInputMoniker = p_rValue.m_lineInputMoniker;
		m_lineInputRisingEventName = p_rValue.m_lineInputRisingEventName;
		m_lineInputFallingEventName = p_rValue.m_lineInputFallingEventName;
		m_eventList = p_rValue.m_eventList;
	}
	return *this;
}

HRESULT SVMatroxGigeDigitizer::CreateAcquisitionBuffers(const SVMatroxSystem& rSystem, unsigned long width, unsigned long height, unsigned long format)
{
	SVMatroxBufferCreateStruct createStruct;
	createStruct.m_eAttribute = SVBufAttGrabImageProc;
	HRESULT hr = BuildCreateStruct(width, height, format, createStruct);
	if (S_OK == hr)
	{
		hr = m_AcqBuffers.Create(rSystem, createStruct, NUM_ACQUISITION_BUFFERS);
	}
	return hr;
}

HRESULT SVMatroxGigeDigitizer::DestroyAcquistionBuffers()
{
	m_AcqBuffers.Destroy();
	return S_OK;
}

HRESULT SVMatroxGigeDigitizer::StartGrabArray(SVMatroxDigitizerInterface::SVMatroxHookFunctionPtr func)
{
	HRESULT hr = SVMatroxDigitizerInterface::EnableCorruptedFrameMonitoring(*(m_Digitizer.get()), true);
	if (S_OK == hr)
	{
		hr = SVMatroxDigitizerInterface::StartGrabArray(*(m_Digitizer.get()), m_AcqBuffers, SVMatroxDigitizerGrab::SVGrabAsynchronous, func, this);
	}
	return hr;
}

HRESULT SVMatroxGigeDigitizer::StopGrabArray(SVMatroxDigitizerInterface::SVMatroxHookFunctionPtr func)
{
	HRESULT hr = SVMatroxDigitizerInterface::EnableCorruptedFrameMonitoring(*(m_Digitizer.get()), false);
	if (S_OK == hr)
	{
		hr = SVMatroxDigitizerInterface::StopGrabArray(*(m_Digitizer.get()), m_AcqBuffers, SVMatroxDigitizerGrab::SVGrabSynchronous, func, this);
	}
	return hr;
}

HRESULT SVMatroxGigeDigitizer::BuildGigeFeaturesMap(BSTR xmlData)
{
	m_featuresMap.clear();

	HRESULT hr = SVGigeCameraDBParser::Parse(xmlData, m_featuresMap);

	return hr;
}

const SVGigeDeviceParameterStruct& SVMatroxGigeDigitizer::GetFeature(SvDef::SVGigeParameterEnum paramEnum) const
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
				SVFeatureTypeEnum::SVTypeBool,
				SVGigeFeature::NotSupported, 
				SVGigeFeature::ReadOnly
			)
		)
	);
	return emptyStruct;
}

bool SVMatroxGigeDigitizer::HasLineInput() const
{
	return !m_lineInputMoniker.empty();
}

void SVMatroxGigeDigitizer::SetLineInputMoniker(const std::string& moniker)
{
	m_lineInputMoniker = moniker.c_str();
	m_lineInputRisingEventName = GetLineInputRisingEventName();
	m_lineInputFallingEventName = GetLineInputFallingEventName();
}

std::string SVMatroxGigeDigitizer::GetLineInputRisingEventName() const
{
	const SVGigeDeviceParameterStruct& deviceParamStruct = GetFeature(SvDef::SVGigeParameterInputEventName);
	
	std::string featureName;
	if (deviceParamStruct.accessor.feature.HasTranslation())
	{
		deviceParamStruct.accessor.feature.GetGigeFeatureString(m_lineInputMoniker + " Rising", featureName);
	}
	return featureName;
}

std::string SVMatroxGigeDigitizer::GetLineInputFallingEventName() const
{
	const SVGigeDeviceParameterStruct& deviceParamStruct = GetFeature(SvDef::SVGigeParameterInputEventName);
	
	std::string featureName;
	if (deviceParamStruct.accessor.feature.HasTranslation())
	{
		deviceParamStruct.accessor.feature.GetGigeFeatureString(m_lineInputMoniker + " Falling", featureName);
	}
	return featureName;
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

HRESULT SVMatroxGigeDigitizer::BuildCreateStruct(unsigned long p_ulWidth, unsigned long p_ulHeight, unsigned long p_ulFormat, SVMatroxBufferCreateStruct& rCreateStruct)
{
	HRESULT l_hr = S_OK;

	rCreateStruct.m_lSizeX = p_ulWidth;
	rCreateStruct.m_lSizeY = p_ulHeight;

	switch (p_ulFormat)
	{
	case SvDef::SVImageFormatMono8:
		rCreateStruct.m_lSizeBand = 1;
		rCreateStruct.m_eType = SV8BitUnsigned;
		break;

	case SvDef::SVImageFormatBGR888:
	case SvDef::SVImageFormatBGR888X:
		rCreateStruct.m_lSizeBand = 3;
		rCreateStruct.m_eType = SV8BitUnsigned;
		break;

	default:
		l_hr = S_FALSE;
		break;
	}
	return l_hr;
}

