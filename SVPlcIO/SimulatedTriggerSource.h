//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file SimulatedTriggerSource.h
/// This is the class used to simulate triggers without the Hilscher card
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerSource.h"
#pragma endregion Includes

namespace SvPlc
{
struct SimulatedTriggerData
{
	std::string m_name;
	uint8_t m_channel {0};
	uint32_t m_objectNumber {0UL};
	uint32_t m_objectID {0UL};
	uint8_t m_triggerPerObjectID {0};
	uint32_t m_initialDelay {0UL};
	uint32_t m_period {0UL};
	uint32_t m_objectDelay {0UL};
};

struct ChannelData
{
	std::thread m_timerThread;
	std::atomic_bool m_runThread{false};
	SimulatedTriggerData m_simulatedTriggerData;
	std::map<uint32_t, double> m_objectIDTimeMap;
	std::ofstream m_resultFile;
};

/// a simplified simulation of HardwareTriggerSource
class SimulatedTriggerSource : public TriggerSource
{
public:
	explicit SimulatedTriggerSource(std::function<void(const TriggerReport&)> pReportTrigger, const std::string& rSimulateFile);
	virtual ~SimulatedTriggerSource() = default;

	virtual HRESULT initialize() override;
	virtual bool setTriggerChannel(uint8_t channel, bool active) override;
	virtual void analyzeTelegramData() override;
	virtual void queueResult(uint8_t channel, ChannelOut1&& channelOut) override;
	virtual void createTriggerReport(uint8_t channel) override;

private:
	std::array<ChannelData, cNumberOfChannels> m_channel;
	std::string m_plcSimulateFile;

	std::array<std::atomic_ulong, cNumberOfChannels> m_ObjectsGood{ 0UL, 0UL, 0UL, 0UL };
	std::array<std::atomic_ulong, cNumberOfChannels> m_ObjectsBad {0UL, 0UL, 0UL, 0UL};
	std::array<std::atomic_ulong, cNumberOfChannels> m_ObjectsInvalid {0UL, 0UL, 0UL, 0UL};
};

} //namespace SvPlc
