//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
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
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

constexpr unsigned int cTimerResolution = 1;
constexpr LPCTSTR cTriggerName = "SoftwareTrigger_1.Dig_";			///This name must match the name in the SVHardwareManifest

void CALLBACK TimerProc(UINT timerID, UINT , DWORD_PTR pData, DWORD_PTR , DWORD_PTR )
{
	SVSoftwareTriggerDevice* pDevice = reinterpret_cast<SVSoftwareTriggerDevice*> (pData);

	if(nullptr != pDevice)
	{
		for(const auto& rTimer : pDevice->getTimerList())
		{
			if(timerID == rTimer.m_timerID)
			{
				pDevice->dispatchTrigger(rTimer.m_triggerIndex);
			}
		}
	}
}

HRESULT SVSoftwareTriggerDevice::Initialize(bool init)
{
	HRESULT hr = S_OK;
	if (false == init)
	{
		for(auto& rTimer : m_timerList)
		{
			::timeKillEvent(rTimer.m_timerID);
			rTimer.m_timerID = 0;
		}
		::timeEndPeriod(cTimerResolution);

		m_triggerCallbackMap.clear();
	}
	return hr;
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
	std::string triggerName {cTriggerName};
	triggerName += std::to_string(triggerIndex - 1);
	result.SetString(triggerName.c_str());
	return result;
}

unsigned long SVSoftwareTriggerDevice::TriggerGetParameterCount(unsigned long) const
{
	return 2UL;
}

_variant_t SVSoftwareTriggerDevice::TriggerGetParameterName( unsigned long, unsigned long index) const
{
	_variant_t result;

	// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
	// however at some time in the future the Dll parameters may be implemented
	// as an array and therefore this enum may not apply.
	switch (index)
	{
		case SVIOParameterEnum::TriggerPeriod:
			result.SetString(_T("Trigger Timer Period"));
			break;
			
		case SVIOParameterEnum::SVBoardVersion:
			result.SetString(_T("Board Version"));
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
		case SVIOParameterEnum::TriggerPeriod:
		{
			result = GetTriggerPeriod(triggerIndex);
			break;
		}
				
		case SVIOParameterEnum::SVBoardVersion:
		{
			result.SetString(_T("Software Trigger Module 1.0 firmware 0x0500"));
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

void SVSoftwareTriggerDevice::dispatchTrigger(unsigned long triggerIndex)
{
	if (m_moduleReady)
	{
		int triggerChannel = triggerIndex - 1;
		bool validChannel = triggerChannel >= 0 && triggerChannel < m_timerList.size();

		SvTrig::IntVariantMap triggerData;
		triggerData[SvTrig::TriggerDataEnum::TimeStamp] = _variant_t(SvUl::GetTimeStamp());
		triggerData[SvTrig::TriggerDataEnum::TriggerChannel] = _variant_t(triggerChannel);

		auto iter = m_triggerCallbackMap.find(triggerIndex);
		if (m_triggerCallbackMap.end() != iter && validChannel && false == m_timerList[triggerChannel].m_pause)
		{
			iter->second(triggerData);
		}

		///Check if a new trigger period
		if(validChannel && m_timerList[triggerChannel].m_newSetting)
		{
			::timeKillEvent(m_timerList[triggerChannel].m_timerID);
			m_timerList[triggerChannel].m_newSetting = false;
			if(false == m_timerList[triggerChannel].m_pause && 0 != m_timerList[triggerChannel].m_period)
			{
				m_timerList[triggerChannel].m_timerID = ::timeSetEvent(m_timerList[triggerChannel].m_period, 0, TimerProc, reinterpret_cast<DWORD_PTR> (this), TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
			}
		}
	}
}

void SVSoftwareTriggerDevice::beforeStartTrigger(unsigned long triggerIndex)
{
	if(false == m_moduleReady)
	{
		::timeBeginPeriod(cTimerResolution);
		m_moduleReady = true;
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
	int triggerChannel = triggerIndex - 1;
	TimerInfo& rTimer = m_timerList[triggerChannel];
	assert(0 == rTimer.m_timerID);
	rTimer.m_newSetting = false;
	if (false == rTimer.m_pause)
	{
		rTimer.m_timerID = ::timeSetEvent(rTimer.m_period, 0, TimerProc, reinterpret_cast<DWORD_PTR> (this), TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
	}
}

void SVSoftwareTriggerDevice::beforeStopTrigger(unsigned long triggerIndex)
{
	int triggerChannel = triggerIndex - 1;
	TimerInfo& rTimer = m_timerList[triggerChannel];
	if(false == rTimer.m_pause && TIMERR_NOERROR != ::timeKillEvent(rTimer.m_timerID))
	{
		assert(false);
	}
	rTimer.m_timerID = 0;
	m_moduleReady = false;

	if (std::none_of(m_timerList.begin(), m_timerList.end(), [](const auto& rEntry) { return rEntry.m_timerID != 0; }))
	{
		::timeEndPeriod(cTimerResolution);
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}
}
long SVSoftwareTriggerDevice::GetTriggerPeriod( unsigned long triggerIndex) const
{
	int triggerChannel = triggerIndex - 1;
	return static_cast<long> (m_timerList[triggerChannel].m_period);
}

HRESULT SVSoftwareTriggerDevice::SetTriggerPeriod(unsigned long triggerIndex, long period)
{
	HRESULT result {E_INVALIDARG};

	if(0 <= period)
	{
		int triggerChannel = triggerIndex - 1;
		TimerInfo& rTimer = m_timerList[triggerChannel];
		rTimer.m_period = static_cast<uint16_t> (period);
		rTimer.m_triggerIndex = triggerIndex;
		if (true == m_moduleReady)
		{
			if(false == rTimer.m_pause && 0 != rTimer.m_period)
			{
				rTimer.m_timerID = ::timeSetEvent(rTimer.m_period, 0, TimerProc, reinterpret_cast<DWORD_PTR> (this), TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
			}
			else
			{
				rTimer.m_newSetting = true;
			}
		}
		result = S_OK;
	}
	return result;
}

void SVSoftwareTriggerDevice::SetTriggerPause(unsigned long triggerIndex, bool pause)
{
	int triggerChannel = triggerIndex - 1;
	TimerInfo& rTimer = m_timerList[triggerChannel];
	rTimer.m_pause = pause;
	rTimer.m_triggerIndex = triggerIndex;
	if (true == m_moduleReady)
	{
		if (false == rTimer.m_pause && 0 != rTimer.m_period)
		{
			rTimer.m_timerID = ::timeSetEvent(rTimer.m_period, 0, TimerProc, reinterpret_cast<DWORD_PTR> (this), TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		}
		else
		{
			rTimer.m_newSetting = true;
		}
	}
}