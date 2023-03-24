//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PowerLinkConnection.cpp
/// \brief Definition of the class PowerlinkConnection
//******************************************************************************


#pragma region Includes
#include "stdafx.h"
#include "HardwareTriggerEcat.h"
#include "EtherCatConnection.h"
#include "SimulatedTriggerEcat.h"
#include "TriggerEngineConnection.h"
#pragma endregion Includes

namespace SvEcat
{
HANDLE g_hSignalEvent {nullptr};
std::atomic_bool g_runThread{false};

EtherCatConnection::EtherCatConnection(const EcatInputParam& rEcatInput)
{
	g_hSignalEvent = ::CreateEvent(nullptr, false, false, nullptr);

	switch (rEcatInput.m_triggerType)
	{
	case EcatTriggerType::HardwareTriggerEcat:
		m_pTriggersource = std::make_unique<HardwareTriggerEcat>(rEcatInput);
		break;
	case EcatTriggerType::SimulatedTriggerEcat:
		m_pTriggersource = std::make_unique<SimulatedTriggerEcat>(rEcatInput);
		break;
	default:
		break;
	}
}

EtherCatConnection::~EtherCatConnection()
{
	if (nullptr != g_hSignalEvent)
	{
		::CloseHandle(g_hSignalEvent);
		g_hSignalEvent = nullptr;
	}
}

void EtherCatConnection::setReady(bool ready)
{
	m_pTriggersource->setReady(ready);
}

void EtherCatConnection::setTriggerChannel(uint8_t channel, bool active)
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

uint32_t EtherCatConnection::getInputs() const
{
	return m_pTriggersource->getInputs();
}

void EtherCatConnection::setOutput(uint8_t outputNr, bool state)
{
	m_pTriggersource->setOutput(outputNr, state);
}

HRESULT EtherCatConnection::initialize()
{
	return m_pTriggersource->initialize();
}

void EtherCatConnection::StartEventSignalThread()
{
	if (!m_eventSignalThread.joinable())
	{
		g_runThread = true;
		auto eventHandler = [this]() { return EventHandler(); };
		m_eventSignalThread = std::thread(&EtherCatConnection::EventSignalThread, this, eventHandler);
		
		::SetThreadPriority(m_eventSignalThread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
		::OutputDebugString("Event signal thread started\n");
	}
}

void EtherCatConnection::StopEventSignalThread()
{
	g_runThread = false;
	::SetEvent(g_hSignalEvent);
	if (m_eventSignalThread.joinable())
	{
		m_eventSignalThread.join();
		::OutputDebugString("Event signal thread stopped\n");
	}
}

void EtherCatConnection::EventSignalThread(std::function<void()> pCallback)
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

void EtherCatConnection::EventHandler()
{
	m_pTriggersource->analyzeTelegramData();
}

} //namespace SvEcat
