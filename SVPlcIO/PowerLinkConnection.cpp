//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
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
#pragma endregion Includes

namespace SvPlc
{
HANDLE g_hSignalEvent {nullptr};
std::atomic_bool g_runThread{false};

PowerlinkConnection::PowerlinkConnection(std::function<void(const TriggerReport&)> pReportTrigger,
										 uint16_t plcNodeID, uint16_t plcTransferTime, uint16_t simulateTriggers) :
	m_pReportTrigger(pReportTrigger)
{
	g_hSignalEvent = ::CreateEvent(nullptr, false, false, nullptr);

	if (simulateTriggers > 0)
	{
		m_pTriggersource = std::make_unique<SimulatedTriggerSource>();
	}
	else
	{
		m_pTriggersource = std::make_unique<HardwareTriggerSource>(plcNodeID, plcTransferTime);
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

void PowerlinkConnection::setTriggerChannel(uint8_t channel, bool active, uint32_t period)
{
	if(channel < cNumberOfChannels)
	{
		//If no active channel then stop event thread
		if(m_pTriggersource->setTriggerChannel(channel, active, period))
		{
			StartEventSignalThread();
		}
		else
		{
			StopEventSignalThread();
		}
	}
}

void PowerlinkConnection::writeResult(const ResultReport& rResultReport)
{
	ChannelOut channelOut;
	//Type = 1 single product
	channelOut.m_currentObjectType = 1;
	channelOut.m_currentObjectID = rResultReport.m_currentObjectID;
	channelOut.m_results = rResultReport.m_results;
	m_pTriggersource->queueResult(rResultReport.m_channel, std::move(channelOut));
}

HRESULT PowerlinkConnection::initialize()
{
	return m_pTriggersource->initialize();
}

void PowerlinkConnection::StartEventSignalThread()
{
	if (!m_eventSignalThread.joinable())
	{
		g_runThread = true;
		m_eventSignalThread = std::thread(&PowerlinkConnection::EventSignalThread, this,
											std::bind(&PowerlinkConnection::EventHandler, this));
		
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
			if(g_runThread)
			{
				pCallback();
			}
		}
	}
}

void PowerlinkConnection::EventHandler()
{
	if(m_pTriggersource->analyzeTelegramData())
	{
		for(int i=0; i < cNumberOfChannels; ++i)
		{
			if(nullptr != m_pReportTrigger)
			{
				const TriggerReport& rReport = m_pTriggersource->getNewTriggerReport(i);
				if (rReport.m_isValid && m_pTriggersource->getChannel(i).m_active)
				{
					m_pReportTrigger(rReport);
				}
			}
		}
	}
}

} //namespace SvPlc
