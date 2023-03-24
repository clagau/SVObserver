//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file HardwareTriggerEcat.cpp
/// This is the class used for triggers set with the data from the Hilscher card
/// using Master EtherCat
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "HardwareTriggerEcat.h"
#include "EtherCatConnection.h"
#include "Triggering/TriggerData.h"
#pragma endregion Includes

namespace SvEcat
{

constexpr LPCTSTR cOperationDataName = _T("_OperationData.log");
constexpr LPCTSTR cPlcOperationDataHeading = _T("Channel;TriggerTimeStamp;Trigger Inputs;Digital Inputs\n");
constexpr LPCTSTR cOperationDataFormat = _T("{:d}; {:f}; {:#08x}; {:#08x}\n");


HardwareTriggerEcat::HardwareTriggerEcat(const EcatInputParam& rEcatInput) : TriggerSource(rEcatInput.m_pTriggerDataCallBack)
, m_rEcatInput {rEcatInput}
, m_cifXCard {rEcatInput}
, m_operationLogger {boost::log::keywords::channel = cOperationDataName}
{
	::OutputDebugString("Triggers are received from EtherCat via CifX card.\n");
}

HardwareTriggerEcat::~HardwareTriggerEcat()
{
	if (nullptr != m_pSink)
	{
		m_pSink->flush();
		m_pSink->stop();
		boost::log::core::get()->remove_sink(m_pSink);
	}
	if (m_logOperationDataFile.is_open())
	{
		m_logOperationDataFile.close();
	}

	m_cifXCard.closeCifX();
}

HRESULT HardwareTriggerEcat::initialize()
{
	HRESULT result{E_FAIL};

	if(m_initialized == false)
	{
		m_cifXCard.setHandleForTelegramReceptionEvent(g_hSignalEvent);
		result = m_cifXCard.OpenAndInitializeCifX();
		if (S_OK != result)
		{
			::OutputDebugString("Could not open and initialize PLC connection!\n");
			return result;
		}
	}

	if (LogType::EcatOperationData == (m_rEcatInput.m_logType & LogType::EcatOperationData))
	{
		std::string fileName {m_rEcatInput.m_logFileName + cOperationDataName};
		if (nullptr != m_logOperationDataFile.open(fileName.c_str(), std::ios::out | std::ios::trunc))
		{
			boost::shared_ptr<std::ostream> stream = boost::make_shared<std::ostream>(&m_logOperationDataFile);
			m_pSink = boost::make_shared<text_sink>();
			m_pSink->locked_backend()->add_stream(stream);
			auto filterOperationData = [](const boost::log::attribute_value_set& rAttribSet) -> bool {return rAttribSet["Channel"].extract<std::string>() == cOperationDataName; };
			m_pSink->set_filter(filterOperationData);
			boost::log::core::get()->add_sink(m_pSink);
			std::string fileData(cPlcOperationDataHeading);
			BOOST_LOG(m_operationLogger) << fileData.c_str();
		}
	}

	return result;
}

uint32_t HardwareTriggerEcat::getInputs() const
{
	uint32_t result {0UL};
	constexpr uint8_t cDigitalInputOffset {1};
	::memcpy(&result, &m_previousInputData.m_dynamicData[cDigitalInputOffset], sizeof(result));
	return result;
}

void HardwareTriggerEcat::setOutput(uint8_t outputNr, bool state)
{
	m_cifXCard.setOutput(outputNr, state);
}

void HardwareTriggerEcat::analyzeTelegramData()
{
	m_inputData = m_cifXCard.popInputDataQueue();
	while (m_inputData.m_notificationTime != 0.0)
	{
		checkForNewTriggers();
		m_previousInputData = m_inputData;
		m_inputData = m_cifXCard.popInputDataQueue();
	}
}

void HardwareTriggerEcat::createTriggerData(uint8_t channel)
{
	uint8_t inputMask = (1U << channel);
	bool channelTriggerDataValid = (m_inputData.m_dynamicData[0] & inputMask) != 0 && (m_previousInputData.m_dynamicData[0] & inputMask) == 0;
	if (m_logOperationDataFile.is_open() && (0 == m_rEcatInput.m_logFilter || channelTriggerDataValid))
	{
		std::string fileData = std::format(cOperationDataFormat, channel + 1, m_inputData.m_notificationTime, m_inputData.m_dynamicData[0], m_inputData.m_dynamicData[1]);
		BOOST_LOG(m_operationLogger) << fileData.c_str();
	}
	if (channelTriggerDataValid)
	{
		SvTrig::TriggerData triggerData;
		triggerData.m_channel = channel;
		triggerData.m_triggerTimestamp = m_inputData.m_notificationTime;
		sendTriggerData(triggerData);
	}
}
} //namespace SvEcat
