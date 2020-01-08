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
HANDLE g_hTelegramEvent {nullptr};
HANDLE g_hStopEvent {nullptr};

PowerlinkConnection::PowerlinkConnection(std::function<void(const TriggerReport&)> pReportTrigger,
										 uint16_t plcTransferTime, uint16_t simulateTriggers) :
	m_pReportTrigger(pReportTrigger)
{
	g_hTelegramEvent = ::CreateEvent(nullptr, false, false, _T("PL New Telegram Event"));
	g_hStopEvent = ::CreateEvent(nullptr, true, false, _T("PL Stop Event"));

	if (simulateTriggers > 0)
	{
		m_pTriggersource = std::make_unique<SimulatedTriggerSource>();
	}
	else
	{
		m_pTriggersource = std::make_unique<HardwareTriggerSource>(plcTransferTime);
	}

	m_pTriggersource->initialize();
}

PowerlinkConnection::~PowerlinkConnection()
{
	if (nullptr != g_hTelegramEvent)
	{
		::CloseHandle(g_hTelegramEvent);
		g_hTelegramEvent = nullptr;
	}
	if (nullptr != g_hStopEvent)
	{
		::CloseHandle(g_hStopEvent);
		g_hStopEvent = nullptr;
	}
}

void PowerlinkConnection::setReady(bool ready)
{
	m_pTriggersource->setReady(ready);
}

void PowerlinkConnection::setTriggerChannel(uint8_t channel, bool active, uint32_t period)
{
	if (channel >= 0 && channel < c_NumberOfChannels)
	{
		m_pTriggersource->setTriggerChannel(channel, active, period);
		//If no active channel then stop event thread
		if(m_pTriggersource->initialize() == false)
		{
			//The thread needs to be closed here because for some reason at a later stage the events no longer function!
			::SetEvent(g_hStopEvent);
			::OutputDebugString("Set Stop Event\n");
			if (m_eventSignalThread.joinable())
			{
				m_eventSignalThread.join();
				::OutputDebugString("Reset Stop Event\n");
				::ResetEvent(g_hStopEvent);
			}
		}
	}
}

void PowerlinkConnection::writeResult(const ResultReport& rResultReport)
{
	ChannelOut channelOut;
	channelOut.m_currentObjectID = rResultReport.m_objectID;
	channelOut.m_results = rResultReport.m_results;
	m_pTriggersource->queueResult(rResultReport.m_channel, std::move(channelOut));
}

void PowerlinkConnection::StartEventSignalThread()
{
	if (!m_eventSignalThread.joinable())
	{
		m_eventSignalThread = std::thread(&PowerlinkConnection::EventSignalThread, this, 
											std::bind(&PowerlinkConnection::EventHandler, this));
	}
}

void PowerlinkConnection::EventSignalThread(std::function<void()> pCallback)
{
	HANDLE checkHandles[2];

	checkHandles[0] = g_hStopEvent;
	checkHandles[1] = g_hTelegramEvent;

	bool run{true};

	while (run)
	{
		// wait for Client to signal
		DWORD Event = WaitForMultipleObjects(2, checkHandles, false, INFINITE);
		switch (Event)
		{
			case WAIT_OBJECT_0:
			{
				run = false;
				break;
			}
			case 	WAIT_OBJECT_0 + 1:
			{
				pCallback();
				break;
			}
		}
	}
}

void PowerlinkConnection::EventHandler()
{
	if(m_pTriggersource->analyzeTelegramData())
	{
		for(int i=0; i < c_NumberOfChannels; ++i)
		{
			if(nullptr != m_pReportTrigger)
			{
				const TriggerReport& rReport = m_pTriggersource->getNewTriggerReport(i);
				if (rReport.isValid() && m_pTriggersource->getChannel(i).m_active)
				{
					m_pReportTrigger(rReport);
				}
			}
		}
	}
}

} //namespace SvPlc
