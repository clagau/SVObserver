//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerSource.cpp
/// \brief Definition of the class TriggerSource
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TriggerSource.h"
#pragma endregion Includes

namespace SvPlc
{

void TriggerSource::setTriggerChannel(uint8_t channel, bool active, uint32_t period)
{
	std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
	//Channel range already checked so no need to check it again
	m_triggerChannels[channel].m_active = active;
	m_triggerChannels[channel].m_period = period;
	if(active == false)
	{
		m_triggerChannels[channel].m_newTrigger = false;
	}
	printOutput("Trigger channel use:");
	for (uint8_t channel = 0; channel < c_NumberOfChannels; channel++)
	{
		printOutput(m_triggerChannels[channel].m_active ? "I" : "_");
	}
	printOutput("\n");
}


void TriggerSource::addTriggerReport(TriggerReport&& triggerReport)
{
	uint8_t channel = triggerReport.m_channel;
	std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
	m_triggerChannels[channel].m_report = std::move(triggerReport);
	m_triggerChannels[channel].m_newTrigger = true;
}


bool TriggerSource::checkForNewTriggers()
{
	for (uint8_t channel = 0; channel < c_NumberOfChannels; channel++)
	{
		if (m_triggerChannels[channel].m_active)
		{
			createTriggerReport(channel);
		}	
	}
	return true;
}

TriggerReport TriggerSource::getNewTriggerReport(uint8_t channel)
{
	TriggerReport result;

	std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
	if(m_triggerChannels[channel].m_newTrigger)
	{
		result = m_triggerChannels[channel].m_report;
		m_triggerChannels[channel].m_newTrigger = false;
	}

	return result;
}

} //namespace SvPlc
