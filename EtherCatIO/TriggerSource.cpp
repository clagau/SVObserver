//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TriggerSource.cpp
/// \brief Definition of the class TriggerSource
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TriggerSource.h"
#pragma endregion Includes

namespace SvEcat
{
TriggerSource::TriggerSource(std::function<void(const SvTrig::TriggerData&)> pSendTriggerData) :
	m_pTriggerDataCallback(pSendTriggerData)
{
}

bool TriggerSource::setTriggerChannel(uint8_t channel, bool active)
{
	bool result{false};
	//Channel range already checked so no need to check it again
	m_activeChannel[channel] = active;
	std::string outputText{"Trigger channel use:"};
	for (const auto& rActive : m_activeChannel)
	{
		result |= rActive;
		outputText += rActive ? 'I' : '_';
	}
	outputText += '\n';
	::OutputDebugString(outputText.c_str());
	return result;
}

void TriggerSource::sendTriggerData(const SvTrig::TriggerData& rTriggerData)
{
	if(nullptr != m_pTriggerDataCallback)
	{
		m_pTriggerDataCallback(rTriggerData);
	}
}

void TriggerSource::checkForNewTriggers()
{
	for (uint8_t channel = 0; channel < cNumberOfChannels; channel++)
	{
		if (m_activeChannel[channel])
		{
			createTriggerData(channel);
		}	
	}
}

} //namespace SvEcat
