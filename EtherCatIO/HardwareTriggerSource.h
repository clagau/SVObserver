//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file HardwareTriggerSource.h
/// This is the class used for triggers set with the data from the Hilscher card
//******************************************************************************
#pragma once

#pragma region Includes
#include "CifxCard.h"
#include "TriggerSource.h"
#pragma endregion Includes

namespace SvEcat
{
/// processes all information that is either gained from the incoming process data or put into outgoing process data 
class HardwareTriggerSource : public TriggerSource
{
public:
	explicit HardwareTriggerSource(std::function<void(const TriggerReport&)> pReportTrigger, const std::string& rAdditionalData);
	virtual ~HardwareTriggerSource();

	virtual bool isReady() const override	{ return m_cifXCard.isReady(); }
	virtual HRESULT initialize() override;
	virtual void analyzeTelegramData() override;
	virtual uint32_t getInputs() const override;
	virtual void setOutput(uint8_t outputNr, bool state) override;
	virtual void setReady(bool ready) override { m_cifXCard.setReady(ready); }

private:
	virtual  void createTriggerReport(uint8_t channel) override;

	void writeOutputs(double timestamp);

	std::array<uint8_t, cEtherCatDataSize> m_previousInputData {0,0,0,0,0,0,0,0};
	std::array<uint8_t, cEtherCatDataSize> m_outputData {0,0,0,0,0,0,0,0};

	CifXCard m_cifXCard;
	std::string m_additionalData;
	bool m_initialized {false};

	std::atomic_bool m_timerOn {false};
	std::thread m_timerThread;
};

} //namespace SvEcat
