//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PowerLinkConnection.cpp
/// \brief Definition of the class PowerlinkConnection
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "HardwareTriggerSource.h"
#include "PowerLinkConnection.h"
#include "SimulatedTriggerSource.h"
#include "TriggerEngineConnection.h"
#include "Triggering/ResultData.h"
#pragma endregion Includes

namespace SvPlc
{
HANDLE g_hSignalEvent {nullptr};
std::atomic_bool g_runThread{false};

PowerlinkConnection::PowerlinkConnection(const PlcInputParam& rPlcInput)
{
	g_hSignalEvent = ::CreateEvent(nullptr, false, false, nullptr);

	switch (rPlcInput.m_triggerType)
	{
	case TriggerType::HardwareTrigger:
		m_pTriggersource = std::make_unique<HardwareTriggerSource>(rPlcInput);
		break;
	case TriggerType::SimulatedTrigger:
		m_pTriggersource = std::make_unique<SimulatedTriggerSource>(rPlcInput);
		break;
	default:
		break;
	}
}

PowerlinkConnection::~PowerlinkConnection()
{
	if (nullptr != g_hSignalEvent)
	{
		::CloseHandle(g_hSignalEvent);
		g_hSignalEvent = nullptr;
	}
}

void PowerlinkConnection::setReady(bool ready)
{
	m_pTriggersource->setReady(ready);
}

void PowerlinkConnection::setTriggerChannel(uint8_t channel, bool active)
{
	if(channel < cNumberOfChannels)
	{
		//If no active channel then stop event thread
		if(m_pTriggersource->setTriggerChannel(channel, active))
		{
			StartEventSignalThread();
		}
		else
		{
			StopEventSignalThread();
		}
	}
}

void PowerlinkConnection::writeResult(const SvTrig::ResultData& rResultData)
{
	if (rResultData.m_channel < cNumberOfChannels)
	{
		m_pTriggersource->queueResult(rResultData);
	}
}

HRESULT PowerlinkConnection::initialize()
{
	return m_pTriggersource->initialize();
}

void PowerlinkConnection::StartEventSignalThread()
{
	if (false == m_eventSignalThread.joinable())
	{
		g_runThread = true;
		auto eventHandler = [this]() { return EventHandler(); };
		m_eventSignalThread = std::thread(&PowerlinkConnection::EventSignalThread, this, eventHandler);
		
		::SetThreadPriority(m_eventSignalThread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
		::OutputDebugString("Event signal thread started\n");
	}
}

void PowerlinkConnection::StopEventSignalThread()
{
	g_runThread = false;
	::SetEvent(g_hSignalEvent);
	if (m_eventSignalThread.joinable())
	{
		m_eventSignalThread.join();
		::OutputDebugString("Event signal thread stopped\n");
	}
}

void PowerlinkConnection::EventSignalThread(std::function<void()> pCallback)
{
	while (g_runThread)
	{
		// wait for Client to signal
		if(WAIT_OBJECT_0 == WaitForSingleObject(g_hSignalEvent, INFINITE))
		{
			// cppcheck-suppress knownConditionTrueFalse ;g_runThread is atomic
			if(g_runThread)
			{
				pCallback();
			}
		}
	}
}

void PowerlinkConnection::EventHandler()
{
	m_pTriggersource->analyzeTelegramData();
}

} //namespace SvPlc
