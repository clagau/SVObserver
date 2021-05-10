//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file HardwareTriggerSource.cpp
/// This is the class used for triggers set with the data from the Hilscher card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "HardwareTriggerSource.h"
#include "PowerLinkConnection.h"
#include "Telegram.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr uint8_t cUnitControlActive = 1;
constexpr int32_t cMicrosecondsPerMillisecond = 1000;
constexpr uint32_t cTimeWrap = 65536;		//Constant when time offset negative need to wrap it by 16 bits
constexpr uint16_t cCifXNodeId = 11; //< The Powerlink Node Id used for the Hilscher CifX card value shall be 11-14 (SVIM1-SVIM4)
constexpr uint16_t cmaxPLC_DataSize = 456; //< the maximum size of the PLC-data in bytes Telegram = 20 Bytes Dynamic = 436 Bytes
//Make sure that the telegram in structure is smaller than the Hilscher card incoming buffer size
static_assert(sizeof(Telegram) + cCmdDataSize <= cmaxPLC_DataSize, "Read buffer size is to small");
static_assert(sizeof(Telegram) + std::max(sizeof(InspectionState1), sizeof(InspectionState2)) <= cmaxPLC_DataSize, "Write buffer size is to small");
static_assert(sizeof(Telegram) + cConfigListSize * sizeof(ConfigDataSet) <= cmaxPLC_DataSize, "Write buffer size is to small");

HardwareTriggerSource::HardwareTriggerSource(std::function<void(const TriggerReport&)> pReportTrigger, uint16_t plcNodeID, uint16_t plcTransferTime, const std::string& rAdditionalData) : TriggerSource(pReportTrigger)
, m_plcTransferTime {plcTransferTime}
, m_additionalData {rAdditionalData}
, m_cifXCard((0 != plcNodeID) ? plcNodeID : cCifXNodeId, cmaxPLC_DataSize)
{
	memset(&m_previousTriggerData[0], 0, m_previousTriggerData.size());
	::OutputDebugString("Triggers are received from PLC via CifX card.\n");
}


HardwareTriggerSource::~HardwareTriggerSource()
{
	m_cifXCard.closeCifX();
}

HRESULT HardwareTriggerSource::initialize()
{
	HRESULT result{E_FAIL};

	if(m_initialized == false)
	{
		m_cifXCard.setHandleForTelegramReceptionEvent(g_hSignalEvent);
		result = m_cifXCard.OpenAndInitializeCifX(m_additionalData);
		if (S_OK != result)
		{
			::OutputDebugString("Could not open and initialize PLC connection!\n");
			return result;
		}
	}

	return result;
}

void HardwareTriggerSource::queueResult(uint8_t channel, ChannelOut1&& channelOut)
{
	///Channel has already been checked
	std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
	if(true == m_inspectionStateQueue.empty() || 0 != m_inspectionStateQueue.front().m_channels[channel].m_objectID)
	{
		m_inspectionStateQueue.emplace(InspectionState1{});
	}
	m_inspectionStateQueue.back().m_channels[channel] = std::move(channelOut);
}

void HardwareTriggerSource::analyzeTelegramData()
{
	m_cifXCard.popInputDataQueue();
	const InputData& rInputData = m_cifXCard.getCurrentInputData();
	switch (rInputData.m_telegram.m_content)
	{
		case TelegramContent::VersionData:
		{
			m_cifXCard.sendVersion();
			break;
		}
		case TelegramContent::TimeSyncData:
		{
			break;
		}
		case TelegramContent::ConfigurationData:
		{
			m_cifXCard.sendConfigList();
			break;
		}
		case TelegramContent::OperationData:
		{
			InspectionState1 sendInpectionState;
			{
				std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
				if(false == m_inspectionStateQueue.empty())
				{
					sendInpectionState = std::move(m_inspectionStateQueue.front());
					m_inspectionStateQueue.pop();
				}
			}
			m_cifXCard.sendOperationData(sendInpectionState);

			uint32_t triggerOffset = m_cifXCard.getTriggerDataOffset();
			//If new trigger data check for new triggers
			if(m_cifXCard.isProtocolInitialized() && 0 != memcmp(&m_previousTriggerData[0] + triggerOffset, &m_cifXCard.getCurrentInputData().m_dynamicData[0] + triggerOffset, cCmdDataSize - triggerOffset))
			{
				memcpy(&m_previousTriggerData[0], &m_cifXCard.getCurrentInputData().m_dynamicData[0], cCmdDataSize);
				checkForNewTriggers();
			}
			break;
		}
		default:
		{
			break;
		}
	}
}

double HardwareTriggerSource::getExecutionTime(int32_t socRelative, int16_t timeStamp, double notificationTime)
{
	int32_t triggerTimeOffset = getPlcTriggerTime(socRelative, timeStamp) - socRelative;
	return notificationTime + static_cast<double> ((triggerTimeOffset - static_cast<int32_t> (m_plcTransferTime)) / cMicrosecondsPerMillisecond);
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


void HardwareTriggerSource::createTriggerReport(uint8_t channel)
{
	PlcVersion plcVersion = m_cifXCard.getPlcVersion();
	if (PlcVersion::PlcData1 == plcVersion)
	{
		InspectionCommand1 inspectionCmd;
		memcpy(&inspectionCmd, &m_cifXCard.getCurrentInputData().m_dynamicData[0], sizeof(inspectionCmd));
		const ChannelIn1& rChannel = inspectionCmd.m_channels[channel];
		double triggerTimeStamp = getExecutionTime(inspectionCmd.m_socRelative, rChannel.m_timeStamp, m_cifXCard.getCurrentInputData().m_notificationTime);

		bool channelTriggerDataValid = (cUnitControlActive == rChannel.m_unitControl) && (0 != rChannel.m_triggerIndex);
		channelTriggerDataValid &= m_previousSequenceCode[channel] != rChannel.m_sequence && (0 != rChannel.m_sequence % 2);
		if (channelTriggerDataValid)
		{
			TriggerReport report;
			report.m_channel = channel;
			report.m_objectID = rChannel.m_objectID;
			report.m_objectType = rChannel.m_objectType;
			report.m_triggerIndex = static_cast<uint32_t> (rChannel.m_triggerIndex);
			report.m_triggerPerObjectID = rChannel.m_triggerCount;
			report.m_triggerTimestamp = triggerTimeStamp;
			report.m_isValid = (0 != inspectionCmd.m_socRelative);

			sendTriggerReport(report);
			m_previousSequenceCode[channel] = rChannel.m_sequence;
		}
	}
	else if (PlcVersion::PlcData2 == plcVersion)
	{
		InspectionCommand2 inspectionCmd;
		memcpy(&inspectionCmd, &m_cifXCard.getCurrentInputData().m_dynamicData[0], sizeof(inspectionCmd));
		const ChannelIn2& rChannel = inspectionCmd.m_channels[channel];
		double triggerTimeStamp = getExecutionTime(inspectionCmd.m_socRelative, rChannel.m_timeStamp1, m_cifXCard.getCurrentInputData().m_notificationTime);

		bool channelTriggerDataValid = (cUnitControlActive == rChannel.m_unitControl) && (0 != rChannel.m_triggerIndex);
		if (channelTriggerDataValid)
		{
			TriggerReport report;
			report.m_channel = channel;
			report.m_objectID = rChannel.m_currentObjectID;
			report.m_objectType = rChannel.m_currentObjectType;
			report.m_triggerIndex = static_cast<uint32_t> (rChannel.m_triggerIndex);
			report.m_triggerPerObjectID = rChannel.m_triggerCount;
			report.m_triggerTimestamp = triggerTimeStamp;
			report.m_isValid = (0 != inspectionCmd.m_socRelative);

			sendTriggerReport(report);
		}
	}
}
} //namespace SvPlc
