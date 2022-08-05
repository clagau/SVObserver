//*****************************************************************************
/// \copyright COPYRIGHT (c) 2022,2022 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PeriodicTimer.h
/// \brief A periodic timer using a thread with waitable timers
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvUl
{
struct PeriodicTimerInfo
{
	HANDLE m_shutdown {nullptr};
	std::atomic_bool m_timerOn {false};
	std::atomic_uint32_t m_period {0U};		//(µs)
	std::atomic_bool m_newSetting {false};
	uint32_t m_initialDelay {0U};			//(µs)
};

struct TimerInfo
{
	std::string m_name {};
	uint32_t m_initialDelay {0U};			//(µs)
	uint32_t m_period {0U};					//(µs)
	bool m_pause {false};
	std::function<void(const std::string&, double)> m_callbackFunction {nullptr};
};

class PeriodicTimer
{
public:
	PeriodicTimer() = default;
	~PeriodicTimer();

	static void SetTimer(const TimerInfo& rtimerInfo);
	static void CloseTimer(const std::string& rName);
	std::mutex& GetMutex() {return m_protectTimer;}
	void KillTimer();
private:
	static void CreateTimer(const TimerInfo& rtimerInfo);
	void NotifyTimer(double timestamp);

	static std::vector<std::unique_ptr<PeriodicTimer>> m_periodicTimerList;
	static bool m_init;
	PeriodicTimerInfo m_periodicTimerInfo {};
	std::thread m_thread {};
	std::mutex m_protectTimer;
	std::vector<TimerInfo> m_timerInfoList {};
};


} //namespace SvUl
