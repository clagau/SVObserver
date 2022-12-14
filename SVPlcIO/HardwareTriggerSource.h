//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file HardwareTriggerSource.h
/// This is the class used for triggers set with the data from the Hilscher card
//******************************************************************************
#pragma once

#pragma region Includes
#include "CifxCard.h"
#include "PlcDataTypes.h"
#include "TriggerSource.h"
#pragma endregion Includes

namespace SvPlc
{
class HardwareTriggerSource : public TriggerSource
{
public:
	explicit HardwareTriggerSource(const PlcInputParam& rPlcInput);
	virtual ~HardwareTriggerSource();

	virtual bool isReady() override	{ return m_cifXCard.isProtocolInitialized(); }
	virtual HRESULT initialize() override;
	virtual void analyzeTelegramData() override;
	virtual void queueResult(uint8_t channel, ChannelOut1&& channelOut) override;
	virtual void setReady(bool ready) override { m_cifXCard.setReady(ready); }

private:
	virtual  void createTriggerReport(uint8_t channel) override;

	/// Calculates the trigger offset time from the relative SOC and time stamp
	int32_t getPlcTriggerTime(int32_t socRelative, int16_t timeStamp);
	double getExecutionTime(int32_t socRelative, int16_t timeStamp, double notificationTime);

	PlcInputParam m_plcInput;		//This value must be above m_cifxCard as it is passed by reference
	uint16_t m_svimContentID {0};
	uint32_t m_numberOfFailures {0UL};

	uint32_t m_ProcessDataChanges {0UL};
	int8_t m_previousSequenceCode[cNumberOfChannels] = { 0, 0, 0, 0 };

	InputData m_inputData {};

	CifXCard m_cifXCard;
	bool m_initialized {false};
	bool m_changedData {false};

	std::filebuf m_logOperationDataFile;
	typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> m_pSink {nullptr};
	boost::log::sources::channel_logger_mt<std::string> m_logger;
};

} //namespace SvPlc
