//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file SimulatedTriggerSource.cpp
/// This is the class used to simulate triggers without the Hilscher card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "PowerLinkConnection.h"
#include "SimulatedTriggerSource.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr uint32_t c_TimeBetweenTriggerChannels_ms = 2;

struct SimulatedTriggerData
{
	uint32_t m_objectID {0UL};
	uint8_t m_sequenceID {0};
	double m_triggerTimestamp{0.0};
	bool m_newTrigger {false};
};

std::mutex g_TriggerDataMutex;
SimulatedTriggerData g_triggerData[c_NumberOfChannels];

void ChannelTimer(uint8_t channel, std::atomic_bool& rRun, uint32_t initialDelay, uint32_t period)
{
	{
		std::lock_guard<std::mutex> guard{g_TriggerDataMutex};
		g_triggerData[channel].m_objectID = 1000UL * channel;
		g_triggerData[channel].m_sequenceID = 1;
		g_triggerData[channel].m_newTrigger = false;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(initialDelay));
	while(rRun)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(period));
		
		{
			std::lock_guard<std::mutex> guard {g_TriggerDataMutex};
			g_triggerData[channel].m_objectID++;
			g_triggerData[channel].m_sequenceID += 2;
			g_triggerData[channel].m_sequenceID %= 100;
			g_triggerData[channel].m_triggerTimestamp = SvTl::GetTimeStamp();
			g_triggerData[channel].m_newTrigger = true;
		}
		::SetEvent(g_hTelegramEvent);
	}
}

bool SimulatedTriggerSource::initialize()
{
	bool result{false};

	for (uint8_t i = 0; i < c_NumberOfChannels; ++i)
	{
		const TriggerChannel& rTriggerChannel = getChannel(i);
		if(rTriggerChannel.m_active && rTriggerChannel.m_period > 0)
		{
			//Restart trigger channel
			result = true;
			if(m_runThread[i] == false)
			{
				m_runThread[i] = true;
				uint32_t initialDelay = c_TimeBetweenTriggerChannels_ms * i;
				m_timerThread[i] = std::thread(&ChannelTimer, i, std::ref(m_runThread[i]), initialDelay, rTriggerChannel.m_period);
			}
		}
		else
		{
			if (m_timerThread[i].joinable())
			{
				m_runThread[i] = false;
				m_timerThread[i].join();
			}
		}
	}
	return result;
}

bool SimulatedTriggerSource::analyzeTelegramData()
{
	bool dataChanged{false};
	{
		std::lock_guard<std::mutex> guard {g_TriggerDataMutex};
		for (uint8_t i = 0; i < c_NumberOfChannels; ++i)
		{
			if(g_triggerData[i].m_newTrigger)
			{
				dataChanged = true;
				break;
			}
		}
	}

	if(dataChanged)
	{
		return checkForNewTriggers();
	}

	return false;
}

void SimulatedTriggerSource::createTriggerReport(uint8_t channel)
{
	SimulatedTriggerData triggerData;

	{
		std::lock_guard<std::mutex> guard {g_TriggerDataMutex};
		triggerData = g_triggerData[channel];
		g_triggerData[channel].m_newTrigger = false;
	}

	if(triggerData.m_newTrigger)
	{
		//currently: simulated triggers always have one trigger per product
		TriggerReport report;
		report.m_channel = channel;
		report.m_currentObjectID = triggerData.m_objectID;
		report.m_sequence = triggerData.m_sequenceID;
		report.m_triggerIndex = 1;
		report.m_triggerPerObjectID = 1;
		report.m_triggerTimestamp = triggerData.m_triggerTimestamp;
		report.m_isValid = true;
		addTriggerReport(std::move(report));
	}
}

} //namespace SvPlc
