//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file HardwareTriggerSource.h
/// This is the class used for triggers set with the data from the Hilscher card
//******************************************************************************
#pragma once

#pragma region Includes
#include "CifxCard.h"
#include "InspectionCommand.h"
#include "InspectionState.h"
#include "TriggerSource.h"
#pragma endregion Includes

namespace SvPlc
{
/// processes all information that is either gained from the incoming process data or put into outgoing process data 
class HardwareTriggerSource : public TriggerSource
{
public:
	HardwareTriggerSource(uint16_t plcTransferTime);
	virtual ~HardwareTriggerSource();

	virtual bool isReady() override	{ return m_cifXCard.isProtocolInitialized(); }
	virtual bool initialize() override;
	virtual void queueResult(uint8_t channel, ChannelOut&& channelOut) override;
	virtual bool analyzeTelegramData() override;
	virtual void setReady(bool ready) { m_cifXCard.setReady(ready); }

protected: 	
	uint64_t getCurrentInterruptCount();

private:
	uint32_t getObjectID(uint8_t channel);
	virtual  void createTriggerReport(uint8_t channel) override;

	/// Calculates the trigger offset time from the relative SOC and time stamp
	int32_t getPlcTriggerTime(int32_t socRelative, int16_t timeStamp);
	double getExecutionTime(uint8_t channel);

	uint16_t m_plcTransferTime {0};

	uint16_t m_svimContentID {0};
	uint32_t m_numberOfFailures {0UL};

	uint32_t m_ProcessDataChanges {0UL};

	int8_t m_NewSequenceCode[c_NumberOfChannels] = {0, 0, 0, 0};
	int8_t m_OldSequenceCode[c_NumberOfChannels] = {0, 0, 0, 0};

	bool m_triggerDataChanged {false};
	InspectionCommand m_previousTriggerData;
	InspectionState m_inspectionState;

	CifXCard m_cifXCard;

	bool m_initialized {false};
};

} //namespace SvPlc
