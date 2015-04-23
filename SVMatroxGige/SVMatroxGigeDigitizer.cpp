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

#include "stdafx.h"
#include "SVMatroxGigeDigitizer.h"
#include "SVGigeCameraParametersLibrary/SVGigeCameraDBParser.h"

#define NUM_ACQUISITION_BUFFERS 5

SVMatroxGigeDigitizer::SVMatroxGigeDigitizer(unsigned long deviceNumber, unsigned char systemHandle)
: m_deviceNumber(deviceNumber)
, m_SystemHandle(systemHandle)
, m_Handle(0xFF)
, m_lIsStarted(false)
, m_lStartIndex(-1)
, m_lLastUsedIndex(-1)
, m_pBufferInterface(NULL)
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

		m_triggerCallback = p_rValue.m_triggerCallback;

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
	if (hr == S_OK)
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
	if (hr == S_OK)
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
	m_lineInputMoniker = moniker.ToString();
	m_lineInputRisingEventName = GetLineInputRisingEventName();
	m_lineInputFallingEventName = GetLineInputFallingEventName();
}

SVMatroxString SVMatroxGigeDigitizer::GetLineInputRisingEventName() const
{
	const SVGigeDeviceParameterStruct& deviceParamStruct = GetFeature(SVGigeParameterInputEventName);
	
	SVGigeFeatureString featureName;
	if (deviceParamStruct.accessor.feature.HasTranslation())
	{
		deviceParamStruct.accessor.feature.GetGigeFeatureString(m_lineInputMoniker + " Rising", featureName);
	}
	return featureName.ToString();
}

SVMatroxString SVMatroxGigeDigitizer::GetLineInputFallingEventName() const
{
	const SVGigeDeviceParameterStruct& deviceParamStruct = GetFeature(SVGigeParameterInputEventName);
	
	SVGigeFeatureString featureName;
	if (deviceParamStruct.accessor.feature.HasTranslation())
	{
		deviceParamStruct.accessor.feature.GetGigeFeatureString(m_lineInputMoniker + " Falling", featureName);
	}
	return featureName.ToString();
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

const SVCallbackStruct& SVMatroxGigeDigitizer::GetTriggerCallback() const
{
	return m_triggerCallback;
}

void SVMatroxGigeDigitizer::SetTriggerCallback(SVCallbackStruct& callback)
{
	m_triggerCallback = callback;
}

void SVMatroxGigeDigitizer::ClearTriggerCallback()
{
	m_triggerCallback.m_pCallback = NULL;
	m_triggerCallback.m_pOwner = NULL;
	m_triggerCallback.m_pData = NULL;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVMatroxGige\SVMatroxGigeDigitizer.cpp_v  $
 * 
 *    Rev 1.0   22 Apr 2013 14:43:56   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   21 Jan 2013 09:59:14   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   29 Jun 2012 16:43:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Jun 2010 08:01:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  695
 * SCR Title:  Upgrade Acq to Allow for LUT, Light Reference, and Model Updates for GigE
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to include changes to allow for GigE Light Reference and LUT to work.  Added functionality to read GigE camera database from camera file to allow for changes to camera functionality to be externalized.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 07:50:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new buffer management and buffer transfer methodologies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:19:02   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/