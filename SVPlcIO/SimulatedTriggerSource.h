//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019,2019 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file SimulatedTriggerSource.h
/// This is the class used to simulate triggers without the Hilscher card
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerSource.h"
#pragma endregion Includes

/// a simplified simulation of HardwareTriggerSource
class SimulatedTriggerSource:public TriggerSource
{
public:
	SimulatedTriggerSource() = default;
	virtual ~SimulatedTriggerSource() = default;

	bool initialize() override;
	bool analyzeTelegramData() override;
	void createTriggerInfo(uint8_t channel) override;

private:
	std::thread m_timerThread[c_NumberOfChannels];
	std::atomic_bool m_runThread[c_NumberOfChannels];
};
