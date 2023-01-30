//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDevice
//* .File Name       : $Workfile:   SVSoftwareTriggerDevice.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:29:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSoftwareTriggerDevice.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#pragma endregion Includes

constexpr uint32_t cConversionMicrosec = 1000;
constexpr LPCTSTR cTriggerName = "SoftwareTrigger_1.Dig_";		///This name must match the name in the SVHardwareManifest

HRESULT SVSoftwareTriggerDevice::Initialize()
{
	for (uint32_t i = 0; i < static_cast<uint32_t> (m_timerList.size()); ++i)
	{
		m_timerList[i].m_name =cTriggerName;
		m_timerList[i].m_name += std::to_string(i);
	}
	return S_OK;
}

HRESULT SVSoftwareTriggerDevice::Destroy()
{
	m_triggerCallbackMap.clear();
	return S_OK;
}

unsigned long SVSoftwareTriggerDevice::GetTriggerCount() const
{
	return cMaxSoftwareTriggers;
}

unsigned long SVSoftwareTriggerDevice::GetTriggerHandle(unsigned long index) const
{
	return index + 1;
}

_variant_t SVSoftwareTriggerDevice::GetTriggerName(unsigned long triggerIndex) const
{
	_variant_t result;
	
	result.SetString(m_timerList[triggerIndex - 1].m_name.c_str());
	return result;
}

unsigned long SVSoftwareTriggerDevice::TriggerGetParameterCount(unsigned long) const
{
	return 3UL;
}

_variant_t SVSoftwareTriggerDevice::TriggerGetParameterName( unsigned long, unsigned long index) const
{
	_variant_t result;

	switch (index)
	{
		case SVIOParameterEnum::SVModuleReady:
			result.SetString(_T("Module Ready(R/W)"));
			break;
		case SVIOParameterEnum::SVBoardVersion:
			result.SetString(_T("Board Version(R)"));
			break;
		case SVIOParameterEnum::TriggerPeriod:
			result.SetString(_T("Trigger Timer Period(R/W)"));
			break;
		default:
			break;
	}
	return result;
}

_variant_t SVSoftwareTriggerDevice::TriggerGetParameterValue( unsigned long triggerIndex, unsigned long Index) const
{
	_variant_t result;

	// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
	// however at some time in the future the Dll parameters may be implemented
	// as an array and therefore this enum may not apply.
	switch ( Index)
	{
		case SVIOParameterEnum::SVModuleReady:
		{
			result = m_moduleReady ? TRUE : FALSE;
			break;
		}
		case SVIOParameterEnum::SVBoardVersion:
		{
			result.SetString(_T("Software Trigger Module 1.0 firmware 0x0500"));
			break;
		}
		case SVIOParameterEnum::TriggerPeriod:
		{
			result = GetTriggerPeriod(triggerIndex);
			break;
		}

		default:
			break;
	}
	return result;
}

HRESULT SVSoftwareTriggerDevice::TriggerSetParameterValue( unsigned long triggerIndex, unsigned long Index, const _variant_t& rValue )
{
	HRESULT result{ E_FAIL };

	switch (Index)
	{
		case SVIOParameterEnum::SVModuleReady:
		{
			if (VT_BOOL == rValue.vt)
			{
				m_moduleReady = rValue.boolVal ? true : false;
				result = S_OK;
			}
			break;
		}
		case SVIOParameterEnum::TriggerPeriod:
		{
			if(VT_I4 == rValue.vt)
			{
				result = SetTriggerPeriod(triggerIndex, rValue.lVal);
			}
			break;
		}
		case SVIOParameterEnum::TriggerPause:
		{
			if (VT_BOOL == rValue.vt)
			{
				SetTriggerPause(triggerIndex, rValue.boolVal ? true : false);
				result = S_OK;
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return result;
}

void SVSoftwareTriggerDevice::beforeStartTrigger(unsigned long triggerIndex)
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	int triggerChannel = triggerIndex - 1;
	SvUl::TimerInfo& rTimer = m_timerList[triggerChannel];
	rTimer.m_callbackFunction = [this](const std::string& rName, double timestamp) {  dispatchTrigger(rName, timestamp); };
	SvUl::PeriodicTimer::SetTimer(rTimer);
}

void SVSoftwareTriggerDevice::beforeStopTrigger(unsigned long triggerIndex)
{
	int triggerChannel = triggerIndex - 1;
	SvUl::TimerInfo& rTimer = m_timerList[triggerChannel];
	SvUl::PeriodicTimer::CloseTimer(rTimer.m_name.c_str());
	rTimer.m_callbackFunction = nullptr;
	
	if (std::none_of(m_timerList.begin(), m_timerList.end(), [](const auto& rTimer) { return nullptr != rTimer.m_callbackFunction; }))
	{
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}
}
long SVSoftwareTriggerDevice::GetTriggerPeriod( unsigned long triggerIndex) const
{
	int triggerChannel = triggerIndex - 1;
	return static_cast<long> (m_timerList[triggerChannel].m_period / cConversionMicrosec);
}

HRESULT SVSoftwareTriggerDevice::SetTriggerPeriod(unsigned long triggerIndex, long period)
{
	HRESULT result {E_INVALIDARG};

	if(0 <= period)
	{
		int triggerChannel = triggerIndex - 1;
		SvUl::TimerInfo& rTimer = m_timerList[triggerChannel];
		rTimer.m_period = static_cast<uint32_t> (period * cConversionMicrosec);
		if (nullptr != rTimer.m_callbackFunction)
		{
			SvUl::PeriodicTimer::SetTimer(rTimer);
		}
		result = S_OK;
	}
	return result;
}

void SVSoftwareTriggerDevice::SetTriggerPause(unsigned long triggerIndex, bool pause)
{
	int triggerChannel = triggerIndex - 1;
	SvUl::TimerInfo& rTimer = m_timerList[triggerChannel];
	rTimer.m_pause = pause;
}

void SVSoftwareTriggerDevice::dispatchTrigger(const std::string& rName, double timestamp)
{
	if (m_moduleReady)
	{
		int triggerChannel {-1};
		for (int i = 0; i < static_cast<int> (m_timerList.size()); ++i)
		{
			if (rName == m_timerList[i].m_name)
			{
				triggerChannel = i;
				break;
			}
		}
		bool validChannel = triggerChannel >= 0 && triggerChannel < m_timerList.size();

		SvTrig::TriggerData triggerData;
		triggerData.m_triggerTimestamp = timestamp;
		triggerData.m_channel = static_cast<uint8_t> (triggerChannel);

		auto iter = m_triggerCallbackMap.find(triggerChannel + 1);
		if (m_triggerCallbackMap.end() != iter && validChannel && false == m_timerList[triggerChannel].m_pause)
		{
			iter->second(std::move(triggerData));
		}
	}
}
