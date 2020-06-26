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
static_assert(sizeof(Telegram) + sizeof(InspectionCommand) <= cmaxPLC_DataSize, "Read buffer size is to small");
static_assert(sizeof(Telegram) + sizeof(InspectionState) <= cmaxPLC_DataSize, "Write buffer size is to small");
static_assert(sizeof(Telegram) + cConfigListSize * sizeof(ConfigDataSet) <= cmaxPLC_DataSize, "Write buffer size is to small");

HardwareTriggerSource::HardwareTriggerSource(std::function<void(const TriggerReport&)> pReportTrigger, uint16_t plcNodeID, uint16_t plcTransferTime) : TriggerSource(pReportTrigger)
, m_plcTransferTime {plcTransferTime}
, m_cifXCard((0 != plcNodeID) ? plcNodeID : cCifXNodeId, cmaxPLC_DataSize)
{
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
		result = m_cifXCard.OpenAndInitializeCifX();
		if (S_OK != result)
		{
			::OutputDebugString("Could not open and initialize PLC connection!\n");
			return result;
		}
	}

	return result;
}

void HardwareTriggerSource::queueResult(uint8_t channel, ChannelOut&& channelOut)
{
	///Channel has already been checked
	std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
	m_inspectionState.m_channels[channel] = channelOut;
}

bool HardwareTriggerSource::analyzeTelegramData()
{
	bool result{false};

	switch (m_cifXCard.getInputTelegram().m_content)
	{
		case TelegramContent::VersionData:
		{
			m_cifXCard.sendVersion();
			break;
		}
		case TelegramContent::TimeSyncData:
		{
			m_cifXCard.setPlcLoopSyncTime();
			break;
		}
		case TelegramContent::ConfigurationData:
		{
			m_cifXCard.sendConfigList();
			break;
		}
		case TelegramContent::OperationData:
		{
			{
				std::lock_guard<std::mutex> guard {m_triggerSourceMutex};
				m_cifXCard.sendOperationData(m_inspectionState);
				//Clear the data
				m_inspectionState = InspectionState{};
			}
			const InspectionCommand& rInsCmd = m_cifXCard.getInspectionCmd();
			m_triggerDataChanged = m_previousTriggerData.hasTriggerDataChanged(rInsCmd);

			if (m_triggerDataChanged)
			{
				m_previousTriggerData = rInsCmd;
				result = checkForNewTriggers();
			}
			break;
		}
		default:
		{
			break;
		}
	}

	return result;
}

double HardwareTriggerSource::getExecutionTime(uint8_t channel)
{
	const InspectionCommand& rInsCmd = m_cifXCard.getInspectionCmd();
	const int16_t& rTimeStamp1 = rInsCmd.m_channels[channel].m_timeStamp1;
	int32_t triggerTimeOffset = getPlcTriggerTime(rInsCmd.m_socRelative, rTimeStamp1) - m_cifXCard.getSyncSocRel();
	return m_cifXCard.getSyncTime() + static_cast<double> (triggerTimeOffset - m_plcTransferTime) / cMicrosecondsPerMillisecond;
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
	const ChannelIn& rChannel = m_cifXCard.getInspectionCmd().m_channels[channel];
	m_OldSequenceCode[channel] = m_NewSequenceCode[channel];
	m_NewSequenceCode[channel] = rChannel.m_sequence;

	double triggerTimeStamp = getExecutionTime(channel);

	//When unit control is 1, sequence number has changed and is odd generate new trigger
	if (cUnitControlActive == rChannel.m_unitControl && m_OldSequenceCode[channel] != m_NewSequenceCode[channel] && (m_NewSequenceCode[channel] % 2))
	{
		TriggerReport report;
		report.m_channel = channel;
		report.m_currentObjectID = rChannel.m_currentObjectID;
		report.m_previousObjectID = rChannel.m_previousObjectID;
		report.m_triggerIndex = static_cast<uint32_t> (rChannel.m_triggerIndex);
		report.m_triggerPerObjectID = rChannel.m_triggerCount;
		report.m_triggerTimestamp = triggerTimeStamp;
		const InspectionCommand& rInsCmd = m_cifXCard.getInspectionCmd();
		report.m_isValid = (0 != rInsCmd.m_socRelative);
		
		sendTriggerReport(report);
	}
}
} //namespace SvPlc
