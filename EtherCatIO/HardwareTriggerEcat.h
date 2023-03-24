//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file HardwareTriggerEcat.h
/// This is the class used for triggers set with the data from the Hilscher card
/// using Master EtherCat
//******************************************************************************
#pragma once

#pragma region Includes
#include "CifxCardEcat.h"
#include "TriggerSource.h"
#pragma endregion Includes

//#define ETHERCAT_TIMER

namespace SvEcat
{
/// processes all information that is either gained from the incoming process data or put into outgoing process data 
class HardwareTriggerEcat : public TriggerSource
{
public:
	explicit HardwareTriggerEcat(const EcatInputParam& rEcatInput);
	virtual ~HardwareTriggerEcat();

	virtual bool isReady() const override	{ return m_cifXCard.isReady(); }
	virtual HRESULT initialize() override;
	virtual void analyzeTelegramData() override;
	virtual uint32_t getInputs() const override;
	virtual void setOutput(uint8_t outputNr, bool state) override;
	virtual void setReady(bool ready) override { m_cifXCard.setReady(ready); }

private:
	virtual  void createTriggerData(uint8_t channel) override;

	const EcatInputParam& m_rEcatInput;

	EcatInputData m_inputData {};
	EcatInputData m_previousInputData {};
	
	CifXCardEcat m_cifXCard;
	bool m_initialized {false};

	std::filebuf m_logOperationDataFile;
	typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> m_pSink {nullptr};
	boost::log::sources::channel_logger_mt<std::string> m_operationLogger;
};

} //namespace SvEcat
