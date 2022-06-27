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
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

constexpr int cTimerConversion = 10000;
constexpr LPCTSTR cTriggerName = "SoftwareTrigger_1.Dig_";		///This name must match the name in the SVHardwareManifest

void CycleTimer(SVSoftwareTriggerDevice::TimerInfo& rTimerInfo, std::function<void(unsigned long, double)> dispatcher)
{
	enum TimerHandles
	{
		ShutDown,
		Timer,
		HandleCount
	};

	HANDLE timerHandle = ::CreateWaitableTimerEx(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
	if (nullptr != timerHandle)
	{
		LARGE_INTEGER pauseTime {};
		while (rTimerInfo.m_timerOn)
		{
			if (rTimerInfo.m_newSetting)
			{
				pauseTime.QuadPart = -static_cast<int64_t>(rTimerInfo.m_period * cTimerConversion);
			}
			if (0 == ::SetWaitableTimer(timerHandle, &pauseTime, 0, nullptr, nullptr, FALSE))
			{
				::CancelWaitableTimer(timerHandle);
				::CloseHandle(timerHandle);
				timerHandle = nullptr;
				rTimerInfo.m_timerOn = false;
			}

			if (nullptr != timerHandle)
			{
				HANDLE eventHandles[TimerHandles::HandleCount] {timerHandle, rTimerInfo.m_shutdown};
				if (::WaitForMultipleObjects(TimerHandles::HandleCount, eventHandles, false, INFINITE) == WAIT_OBJECT_0)
				{
					if (rTimerInfo.m_timerOn && nullptr != dispatcher)
					{
						double timeStamp {SvUl::GetTimeStamp()};
						dispatcher(rTimerInfo.m_triggerIndex, timeStamp);
					}
				}
			}
		}
		::CancelWaitableTimer(timerHandle);
		::CloseHandle(timerHandle);
	}
}

HRESULT SVSoftwareTriggerDevice::Initialize()
{
	return S_OK;
}

HRESULT SVSoftwareTriggerDevice::Destroy()
{
	for (auto& rTimer : m_timerList)
	{
		if (rTimer.m_thread.joinable())
		{
			rTimer.m_timerOn = false;
			rTimer.m_thread.join();
		}
	}

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


void SVSoftwareTriggerDevice::dispatchTrigger(unsigned long triggerIndex, double timestamp)
{
	if (m_moduleReady)
	{
		int triggerChannel = triggerIndex - 1;
		bool validChannel = triggerChannel >= 0 && triggerChannel < m_timerList.size();

		SvTrig::TriggerData triggerData;
		triggerData[SvTrig::TriggerDataEnum::TimeStamp] = _variant_t(timestamp);
		triggerData[SvTrig::TriggerDataEnum::TriggerChannel] = _variant_t(static_cast<uint8_t> (triggerChannel));

		auto iter = m_triggerCallbackMap.find(triggerIndex);
		if (m_triggerCallbackMap.end() != iter && validChannel && false == m_timerList[triggerChannel].m_pause)
		{
			iter->second(std::move(triggerData));
		}
	}
}

void SVSoftwareTriggerDevice::beforeStartTrigger(unsigned long triggerIndex)
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	int triggerChannel = triggerIndex - 1;
	TimerInfo& rTimer = m_timerList[triggerChannel];
	rTimer.m_timerOn = true;
	if (nullptr != rTimer.m_shutdown)
	{
		::CloseHandle(rTimer.m_shutdown);
		rTimer.m_shutdown = nullptr;
	}
	rTimer.m_shutdown = ::CreateEvent(NULL, false, false, NULL);
	rTimer.m_newSetting = true;
	std::function<void(unsigned long, double)> dispatcher = [this](unsigned long triggerIndex, double timestamp) { dispatchTrigger(triggerIndex, timestamp); };
	rTimer.m_thread = std::thread(&CycleTimer, std::ref(rTimer), dispatcher);
}

void SVSoftwareTriggerDevice::beforeStopTrigger(unsigned long triggerIndex)
{
	int triggerChannel = triggerIndex - 1;
	TimerInfo& rTimer = m_timerList[triggerChannel];
	if (rTimer.m_thread.joinable() && rTimer.m_timerOn)
	{
		rTimer.m_timerOn = false;
		::SetEvent(rTimer.m_shutdown);
		rTimer.m_thread.join();
		::CloseHandle(rTimer.m_shutdown);
		rTimer.m_shutdown = nullptr;
	}

	if (std::none_of(m_timerList.begin(), m_timerList.end(), [](const auto& rTimer) { return rTimer.m_thread.joinable(); }))
	{
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
		rTimer.m_newSetting = true;
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
	rTimer.m_newSetting = true;
}