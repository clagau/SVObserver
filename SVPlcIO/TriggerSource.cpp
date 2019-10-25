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


void TriggerSource::setTriggerChannel(uint8_t channel, bool active, uint32_t period)
{
	std::lock_guard<std::mutex> guard {m_triggerChannelMutex};
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


void TriggerSource::addTriggerInfo(const TriggerInformation& rTriggerInfo)
{
	std::lock_guard<std::mutex> guard {m_triggerChannelMutex};
	m_triggerChannels[rTriggerInfo.m_Channel].m_currentTriggerInfo = rTriggerInfo;
	m_triggerChannels[rTriggerInfo.m_Channel].m_newTrigger = true;
}


bool TriggerSource::checkForNewTriggers()
{
	for (uint8_t channel = 0; channel < c_NumberOfChannels; channel++)
	{
		if (m_triggerChannels[channel].m_active)
		{
			createTriggerInfo(channel);
		}	
	}
	return true;
}

TriggerInformation TriggerSource::getNewTriggerInfo(uint8_t channel)
{
	TriggerInformation result;

	std::lock_guard<std::mutex> guard {m_triggerChannelMutex};
	if(m_triggerChannels[channel].m_newTrigger)
	{
		result = m_triggerChannels[channel].m_currentTriggerInfo;
		m_triggerChannels[channel].m_newTrigger = false;
	}

	return result;
}
