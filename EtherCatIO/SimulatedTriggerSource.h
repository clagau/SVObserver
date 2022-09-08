//*****************************************************************************
/// \copyright COPYRIGHT (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file SimulatedTriggerSource.h
/// This is the class used to simulate triggers without the Hilscher card
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerSource.h"
#include "SVUtilityLibrary/PeriodicTimer.h"
#pragma endregion Includes

namespace SvEcat
{
struct SimulatedTriggerData
{
	std::string m_name {};
	uint8_t m_channel {0};
	long m_objectNumber {0L};
	uint32_t m_objectID {0UL};
	uint8_t m_triggerPerObjectID {0};
	uint32_t m_initialDelay {0UL};
	uint32_t m_period {0UL};
	uint32_t m_objectDelay {0UL};
	std::vector<std::string> m_LoadImageList {};
};

struct ChannelData
{
	SimulatedTriggerData m_simulatedTriggerData {};
	SvUl::TimerInfo m_timerInfo {};
	std::map<uint32_t, std::string> m_objectIDFileMap {};
	std::ofstream m_resultFile {};
	bool m_intialize {false};
};

/// a simplified simulation of HardwareTriggerSource
class SimulatedTriggerSource : public TriggerSource
{
	enum FileParamColumnPos
	{
		triggerName=0,
		triggerIndex,
		objectNumber,
		objectID,
		triggerPerObjectID,
		initialDelay,
		machineSpeed,
		objectDelay,
		validationFolder,
	};
public:
	explicit SimulatedTriggerSource(std::function<void(const TriggerReport&)> pReportTrigger, const std::string& rSimulateFile);
	virtual ~SimulatedTriggerSource() = default;

	virtual HRESULT initialize() override;
	virtual bool setTriggerChannel(uint8_t channel, bool active) override;
	virtual void analyzeTelegramData() override;
	virtual uint32_t getInputs() const override { return 0UL; }
	virtual void setOutput(uint8_t outputNr, bool state) override;
	virtual void createTriggerReport(uint8_t channel) override;

private:
	HRESULT initChannel(const std::vector<std::vector<std::string>>& rCycleParamList);

	void dispatchTrigger(const std::string& rName, double timestamp);

	std::array<ChannelData, cNumberOfChannels> m_channel;
	std::string m_plcSimulateFile;

	std::array<std::atomic_ulong, cNumberOfChannels> m_ObjectsGood{ 0UL, 0UL, 0UL, 0UL };
	std::array<std::atomic_ulong, cNumberOfChannels> m_ObjectsBad {0UL, 0UL, 0UL, 0UL};
	std::array<std::atomic_ulong, cNumberOfChannels> m_ObjectsInvalid {0UL, 0UL, 0UL, 0UL};
};

} //namespace SvEcat
