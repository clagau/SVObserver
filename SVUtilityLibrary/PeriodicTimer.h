//*****************************************************************************
/// \copyright COPYRIGHT (c) 2022,2022 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
	std::atomic_uint32_t m_period {0U};		//(�s)
	std::atomic_bool m_newSetting {false};
	uint32_t m_initialDelay {0U};			//(�s)
};

struct TimerInfo
{
	std::string m_name {};
	uint32_t m_initialDelay {0U};			//(�s)
	uint32_t m_period {0U};					//(�s)
	bool m_pause {false};
	std::function<void(const std::string&, double)> m_callbackFunction {nullptr};
};

class PeriodicTimer
{
public:
	PeriodicTimer() = default;
	~PeriodicTimer() = default;

	static void SetTimer(const TimerInfo& rtimerInfo);
	static void CloseTimer(const std::string& rName);
	std::mutex& GetMutex() {return m_protectTimer;}
	void KillTimer();

private:
	static void CreateTimer(const TimerInfo& rtimerInfo);
	void NotifyTimer(double timestamp);

	PeriodicTimerInfo m_periodicTimerInfo {};
	std::thread m_thread {};
	std::mutex m_protectTimer;
	std::vector<TimerInfo> m_timerInfoList {};
};


} //namespace SvUl
