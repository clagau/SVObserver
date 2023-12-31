//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file HardwareTriggerSource.cpp
/// This is the class used for triggers set with the data from the Hilscher card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "HardwareTriggerSource.h"
#include "PowerLinkConnection.h"
#include "Telegram.h"
#include "Triggering/ResultData.h"
#include "Triggering/TriggerData.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr uint8_t cUnitControlActive = 1;
constexpr int32_t cMicrosecondsPerMillisecond = 1000;
constexpr uint32_t cTimeWrap = 65536;		//Constant when time offset negative need to wrap it by 16 bits

constexpr LPCTSTR cOperationDataName = _T("_OperationData.log");
constexpr LPCTSTR cPlcOperationDataHeading = _T("ContentID;Channel;Timestamp;TriggerTimeStamp;TriggerDataValid;UnitControl;Sequence;TriggerIndex;ObjectType;ObjectID\n");
constexpr LPCTSTR cOperationDataFormat = _T("{:d}; {:d}; {:f}; {:f}; {:b}; {:d}; {:d}; {:d}; {:d}; {}\n");

HardwareTriggerSource::HardwareTriggerSource(const PlcInputParam& rPlcInput) : TriggerSource(rPlcInput.m_pTriggerDataCallBack)
, m_plcInput {rPlcInput}
, m_cifXCard(m_plcInput)
, m_operationLogger {boost::log::keywords::channel = cOperationDataName}
{
	::OutputDebugString("Triggers are received from PLC via CifX card.\n");
}

HardwareTriggerSource::~HardwareTriggerSource()
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

HRESULT HardwareTriggerSource::initialize()
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

	if (LogType::PlcOperationData == (m_plcInput.m_logType & LogType::PlcOperationData))
	{
		std::string fileName {m_plcInput.m_logFileName + cOperationDataName};
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

void HardwareTriggerSource::queueResult(const SvTrig::ResultData& rResultData)
{
	if (rResultData.m_channel < cNumberOfChannels)
	{
		ChannelOut1 channelOut;
		channelOut.m_objectType = rResultData.m_objectType;
		channelOut.m_objectID = rResultData.m_objectID;
		memcpy(&channelOut.m_results[0], &rResultData.m_results[0], channelOut.m_results.size());
		channelOut.m_measurementValue = static_cast<float> (rResultData.m_measurementValue);
		memcpy(&channelOut.m_serializationCode[0], &rResultData.m_serializationCode[0], channelOut.m_serializationCode.size()* sizeof(uint32_t));
		m_cifXCard.queueResult(rResultData.m_channel, std::move(channelOut));
	}
}

void HardwareTriggerSource::analyzeTelegramData()
{
	m_inputData = m_cifXCard.popInputDataQueue();
	while (TelegramContent::NoneContent != m_inputData.m_telegram.m_content)
	{
		if (TelegramContent::OperationData == m_inputData.m_telegram.m_content)
		{
				checkForNewTriggers();
			m_prevInputData = m_inputData;
			}
		m_inputData = m_cifXCard.popInputDataQueue();
		}
}

double HardwareTriggerSource::getExecutionTime(int32_t socRelative, int16_t timeStamp, double notificationTime)
{
	int32_t triggerTimeOffset = getPlcTriggerTime(socRelative, timeStamp) - socRelative;
	return notificationTime + static_cast<double> ((triggerTimeOffset - static_cast<int32_t> (m_plcInput.m_plcTransferTime)) / cMicrosecondsPerMillisecond);
}

int32_t HardwareTriggerSource::getPlcTriggerTime(int32_t socRelative, int16_t timeStamp)
{
	int32_t triggerTimeOffset{ socRelative };

	//the time stamp only contains the least significant two bytes
	memcpy(&triggerTimeOffset, &timeStamp, sizeof(timeStamp));

	//When offset is in the past we need to wrap it by 16 bits
	if (triggerTimeOffset < socRelative)
	{
		triggerTimeOffset += cTimeWrap;
	}

	return triggerTimeOffset;
}


void HardwareTriggerSource::createTriggerData(uint8_t channel)
{
	PlcVersion plcVersion = m_cifXCard.getPlcVersion();
	if (PlcVersion::PlcData1 == plcVersion)
	{
		InspectionCommand1& rInspectionCmd = reinterpret_cast<InspectionCommand1&> (m_inputData.m_dynamicData[0]);
		const ChannelIn1& rChannel = rInspectionCmd.m_channels[channel];
		const InspectionCommand1& rPrevInspectionCmd = reinterpret_cast<InspectionCommand1&> (m_prevInputData.m_dynamicData[0]);
		const ChannelIn1& rPrevChannel = rPrevInspectionCmd.m_channels[channel];
		double triggerTimeStamp = getExecutionTime(rInspectionCmd.m_socRelative, rChannel.m_timeStamp, m_inputData.m_notificationTime);
	
		bool channelTriggerDataValid {rChannel != rPrevChannel};
		channelTriggerDataValid &= (0 != rInspectionCmd.m_socRelative);
		channelTriggerDataValid &= (cUnitControlActive == rChannel.m_unitControl) && (0 != rChannel.m_triggerIndex);
		channelTriggerDataValid &= m_previousSequenceCode[channel] != rChannel.m_sequence && (0 != rChannel.m_sequence % 2);
		bool isValidObjectID {std::any_of(std::begin(rChannel.m_objectID), std::end(rChannel.m_objectID), [](const auto& objectId){return 0 != objectId; })};
		if (m_logOperationDataFile.is_open() && (0 == m_plcInput.m_logFilter || isValidObjectID))
		{
			std::string objectIDList = std::accumulate(std::begin(rChannel.m_objectID)+1, std::end(rChannel.m_objectID), std::to_string(rChannel.m_objectID[0]), [](const std::string& rText, uint32_t objectID)
			{ 
				return rText + "," + std::to_string(objectID);
			});
			std::string fileData = std::format(cOperationDataFormat, m_inputData.m_telegram.m_contentID, channel + 1, m_inputData.m_notificationTime,
				triggerTimeStamp, channelTriggerDataValid, rChannel.m_unitControl, rChannel.m_sequence, rChannel.m_triggerIndex, rChannel.m_objectType, objectIDList.c_str());
			BOOST_LOG(m_operationLogger) << fileData.c_str();
		}
		if (channelTriggerDataValid)
		{
			SvTrig::TriggerData triggerData;
			triggerData.m_channel = channel;
			triggerData.m_objectType = rChannel.m_objectType;
			triggerData.m_triggerIndex = rChannel.m_triggerIndex;
			triggerData.m_triggerPerObjectID = rChannel.m_triggerCount;
			triggerData.m_triggerTimestamp = triggerTimeStamp;
			triggerData.m_loopMode = (0 != rChannel.m_loopMode);
			//Convert to double as SVO uses only this type
			for (int i = 0; i < SvDef::cObjectIndexMaxNr; ++i)
			{
				triggerData.m_objectData[i].m_objectID = rChannel.m_objectID[i];
				triggerData.m_objectData[i].m_rotationNr = rChannel.m_rotationNr[i];
				triggerData.m_objectData[i].m_measurementValue = static_cast<double> (rChannel.m_measurementValue[i]);
			}

			sendTriggerData(triggerData);
			m_previousSequenceCode[channel] = rChannel.m_sequence;
		}
	}
	else if (PlcVersion::PlcData2 == plcVersion)
	{
		InspectionCommand2& rInspectionCmd = reinterpret_cast<InspectionCommand2&> (m_inputData.m_dynamicData[0]);
		const ChannelIn2& rChannel = rInspectionCmd.m_channels[channel];
		const InspectionCommand2& rPrevInspectionCmd = reinterpret_cast<InspectionCommand2&> (m_prevInputData.m_dynamicData[0]);
		const ChannelIn2& rPrevChannel = rPrevInspectionCmd.m_channels[channel];
		double triggerTimeStamp = getExecutionTime(rInspectionCmd.m_socRelative, rChannel.m_timeStamp1, m_inputData.m_notificationTime);

		bool channelTriggerDataValid {rChannel != rPrevChannel};
		channelTriggerDataValid &= (0 != rInspectionCmd.m_socRelative);
		channelTriggerDataValid &= (cUnitControlActive == rChannel.m_unitControl) && (0 != rChannel.m_triggerIndex);
		channelTriggerDataValid &= m_previousSequenceCode[channel] != rChannel.m_sequence && (0 != rChannel.m_sequence % 2);

		if (m_logOperationDataFile.is_open() && (0 == m_plcInput.m_logFilter || 0 != rChannel.m_currentObjectID))
		{
			std::string fileData = std::format(cOperationDataFormat, m_inputData.m_telegram.m_contentID, channel + 1, m_inputData.m_notificationTime,
				triggerTimeStamp, channelTriggerDataValid, rChannel.m_unitControl, rChannel.m_sequence, rChannel.m_triggerIndex, rChannel.m_currentObjectType, rChannel.m_currentObjectID);
			BOOST_LOG(m_operationLogger) << fileData.c_str();
		}
		if (channelTriggerDataValid)
		{
			SvTrig::TriggerData triggerData;
			triggerData.m_channel = channel;
			//In this PLC data version only one objectID available
			triggerData.m_objectData[0].m_objectID = rChannel.m_currentObjectID;
			triggerData.m_objectType = rChannel.m_currentObjectType;
			triggerData.m_triggerIndex = static_cast<uint32_t> (rChannel.m_triggerIndex);
			triggerData.m_triggerPerObjectID = rChannel.m_triggerCount;
			triggerData.m_triggerTimestamp = triggerTimeStamp;

			sendTriggerData(triggerData);
			m_previousSequenceCode[channel] = rChannel.m_sequence;
		}
	}
}
} //namespace SvPlc
