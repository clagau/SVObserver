//*****************************************************************************
/// \copyright COPYRIGHT (c) 2022,2022 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PeriodicTimer.cpp
/// \brief A periodic timer using a thread with waitable timers
//******************************************************************************
#pragma region Includes
#include "stdafx.h"
#include "PeriodicTimer.h"
#include "SVClock.h"
#pragma endregion Includes

constexpr int cTimerConversion = 10;
constexpr int cMaxTimers = 10;

namespace SvUl
{

std::vector<std::unique_ptr<PeriodicTimer>> PeriodicTimer::m_periodicTimerList {};
bool PeriodicTimer::m_init {false};

void CycleTimer(PeriodicTimerInfo& rTimerInfo, std::function<void(double)> dispatcher)
{
	enum TimerHandles
	{
		Timer,
		ShutDown,
		HandleCount
	};

	if (0 != rTimerInfo.m_initialDelay)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(rTimerInfo.m_initialDelay));
	}

	LARGE_INTEGER waitTime {};
	HANDLE timerHandle = ::CreateWaitableTimerEx(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
	if (nullptr != timerHandle)
	{
		while (rTimerInfo.m_timerOn)
		{
			if (rTimerInfo.m_newSetting)
			{
				waitTime.QuadPart = -static_cast<int64_t>(rTimerInfo.m_period * cTimerConversion);
				rTimerInfo.m_newSetting = false;
			}
			if (0 == ::SetWaitableTimer(timerHandle, &waitTime, 0, nullptr, nullptr, false))
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
					double timeStamp {GetTimeStamp()};
					if (rTimerInfo.m_timerOn && nullptr != dispatcher)
					{
						dispatcher(timeStamp);
					}
				}
			}
		}
		::CancelWaitableTimer(timerHandle);
		::CloseHandle(timerHandle);
	}
}

PeriodicTimer::~PeriodicTimer()
{
	KillTimer();
}

void PeriodicTimer::SetTimer(const TimerInfo& rTimerInfo)
{
	bool createTimer {true};
	PeriodicTimer* pPeriodicTimer {nullptr};
	for (auto& rTimer : m_periodicTimerList)
	{
		std::lock_guard lock {rTimer->GetMutex()};
		auto iterTimerInfo = std::find_if(rTimer->m_timerInfoList.begin(), rTimer->m_timerInfoList.end(), [&rTimerInfo](const auto& rEntry) { return rEntry.m_name == rTimerInfo.m_name; });
		if (rTimer->m_timerInfoList.end() != iterTimerInfo)
		{
			//If this is the only reference it can be changed on the fly
			if (1 == rTimer->m_timerInfoList.size())
			{
				rTimer->m_periodicTimerInfo.m_period = rTimerInfo.m_period;
				rTimer->m_periodicTimerInfo.m_newSetting = true;
				return;
			}
			else if (iterTimerInfo->m_period != rTimerInfo.m_period)
			{
				rTimer->m_timerInfoList.erase(iterTimerInfo);
				break;
			}
			else
			{
				return;
			}
		}
		if (rTimer->m_periodicTimerInfo.m_initialDelay == rTimerInfo.m_initialDelay && rTimer->m_periodicTimerInfo.m_period == rTimerInfo.m_period)
		{
			pPeriodicTimer = rTimer.get();
			createTimer = false;
		}
	}

	if (createTimer)
	{
		PeriodicTimer::CreateTimer(rTimerInfo);
	}
	else if (nullptr != pPeriodicTimer)
	{
		std::lock_guard lock {pPeriodicTimer->GetMutex()};
		pPeriodicTimer->m_timerInfoList.push_back(rTimerInfo);
	}
}

void PeriodicTimer::CloseTimer(LPCTSTR name)
{
	for (auto iterTimer = m_periodicTimerList.begin(); m_periodicTimerList.end() != iterTimer;)
	{
		if (nullptr != *iterTimer)
		{
			const auto& rTimer = *iterTimer;
			bool removeTimer {false};
			{
				std::lock_guard lock {rTimer->GetMutex()};
				auto iterTimerInfo = std::find_if(rTimer->m_timerInfoList.begin(), rTimer->m_timerInfoList.end(), [&name](const auto& rEntry) { return rEntry.m_name == name; });
				if (rTimer->m_timerInfoList.end() != iterTimerInfo)
				{
					if (1 == rTimer->m_timerInfoList.size())
					{
						removeTimer = true;
					}
					else
					{
						rTimer->m_timerInfoList.erase(iterTimerInfo);
						break;
					}
				}

			}
			if (removeTimer)
			{
				(*iterTimer)->KillTimer();
				(*iterTimer).reset();
			}
		}
		++iterTimer;
	}
	if (std::all_of(m_periodicTimerList.begin(), m_periodicTimerList.end(), [](const auto& rTimer) { return nullptr == rTimer; }))
	{
		m_periodicTimerList.clear();
		m_init = false;
	}

}

void PeriodicTimer::KillTimer()
{
	bool isJoinable {false};
	if (m_thread.joinable())
	{
		m_periodicTimerInfo.m_timerOn = false;
		::SetEvent(m_periodicTimerInfo.m_shutdown);
		m_thread.join();
		::CloseHandle(m_periodicTimerInfo.m_shutdown);
		m_periodicTimerInfo.m_shutdown = nullptr;
		isJoinable = m_thread.joinable();
	}
}

void PeriodicTimer::CreateTimer(const TimerInfo& rTimerInfo)
{
	if (false == m_init)
	{
		m_periodicTimerList.reserve(cMaxTimers);
		m_init = true;
	}
	auto& rTimer = m_periodicTimerList.emplace_back(std::make_unique<PeriodicTimer>());
	rTimer->m_timerInfoList.reserve(cMaxTimers);
	rTimer->m_timerInfoList.push_back(rTimerInfo);
	rTimer->m_periodicTimerInfo.m_shutdown = ::CreateEvent(NULL, false, false, NULL);
	rTimer->m_periodicTimerInfo.m_timerOn = true;
	rTimer->m_periodicTimerInfo.m_period = rTimerInfo.m_period;
	rTimer->m_periodicTimerInfo.m_initialDelay = rTimerInfo.m_initialDelay;
	rTimer->m_periodicTimerInfo.m_newSetting = true;
	std::function<void(double)> notifier = [&rTimer](double timestamp) { rTimer->NotifyTimer(timestamp); };
	rTimer->m_thread = std::thread(&CycleTimer, std::ref(rTimer->m_periodicTimerInfo), notifier);
	::SetThreadPriority(rTimer->m_thread.native_handle(), THREAD_PRIORITY_HIGHEST);
}

void PeriodicTimer::NotifyTimer(double timestamp)
{
	std::lock_guard lock{m_protectTimer};
	for (const auto& rTimerInfo : m_timerInfoList)
	{
		rTimerInfo.m_callbackFunction(rTimerInfo.m_name, timestamp);
	}
}

} //namespace SvUl
