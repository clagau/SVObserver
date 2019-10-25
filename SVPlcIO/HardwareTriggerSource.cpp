//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file HardwareTriggerSource.cpp
/// This is the class used for triggers set with the data from the Hilscher card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "CifXCard.h"
#include "HardwareTriggerSource.h"
#include "PowerLinkConnection.h"
#include "Telegram.h"
#pragma endregion Includes

constexpr int32_t c_MicrosecondsPerMillisecond = 1'000;
constexpr uint32_t c_cifXProblemReportFrequency = 1000;
constexpr uint32_t c_SecondsToNanoSeconds = 1000'000'000;
constexpr uint32_t c_TimeWrap = 65536;		//Constant when time offset negative need to wrap it by 16 bits

const size_t m_numberOfEntriesPerChannel = 25;

const uint32_t c_CifXNodeId = 23; //< The Powerlink Node Id used for the Hilscher CifX card
const uint32_t c_maxPLC_DataSize = 456; //< the maximum size of the PLC-data in bytes Telegram = 20 Bytes Dynamic = 436 Bytes
//Make sure that the telegram in structure is smaller than the Hilscher card incoming buffer size
static_assert(sizeof(Telegram) + sizeof(InspectionCommand) <= c_maxPLC_DataSize, "Read buffer size is to small");
static_assert(sizeof(Telegram) + sizeof(InspectionState) <= c_maxPLC_DataSize, "Write buffer size is to small");
static_assert(sizeof(Telegram) + c_ConfigListSize * sizeof(ConfigDataSet) <= c_maxPLC_DataSize, "Write buffer size is to small");

HardwareTriggerSource::HardwareTriggerSource() : TriggerSource()
,m_cifXCard(c_CifXNodeId, c_maxPLC_DataSize)
{
	::OutputDebugString("Triggers are received from PLC via CifX card.\n");
}


HardwareTriggerSource::~HardwareTriggerSource()
{
	m_cifXCard.closeCifX();
}

bool HardwareTriggerSource::initialize()
{
	bool result{false};

	if(m_initialized == false)
	{
		m_cifXCard.setHandleForTelegramReceptionEvent(g_hTelegramEvent);
		if (!m_cifXCard.OpenAndInitializeCifX())
		{
			::OutputDebugString("Could not open and initialize PLC connection!\n");
		}
		m_initialized = true;
	}

	for (uint8_t i = 0; i < c_NumberOfChannels; ++i)
	{
		const TriggerChannel& rTriggerChannel = getChannel(i);
		if (rTriggerChannel.m_active)
		{
			result = true;
			break;
		}
	}

	return result;
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
			m_cifXCard.sendOperationData(m_inspectionState);
			const InspectionCommand& rInsCmd = m_cifXCard.getInspectionCmd();
			m_triggerDataChanged = m_previousTriggerData.hasTriggerDataChanged(rInsCmd);

			if (m_triggerDataChanged)
			{
				m_previousTriggerData = rInsCmd;
			}

			if (m_triggerDataChanged)
			{
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


uint32_t  HardwareTriggerSource::getObjectID(uint8_t channel)
{
	const InspectionCommand& rInsCmd = m_cifXCard.getInspectionCmd();
	m_OldSequenceCode[channel] = m_NewSequenceCode[channel];
	m_NewSequenceCode[channel] = rInsCmd.m_channels[channel].m_sequence;

	if (m_OldSequenceCode[channel] != m_NewSequenceCode[channel]) //sequence number has changed: generate a new trigger
	{
		// only if a trigger is present which is when SequenceCode is odd
		if (m_NewSequenceCode[channel] % 2)
		{
			return rInsCmd.m_channels[channel].m_currentObject.m_ID;
		}
	}
	return 0;
}


double HardwareTriggerSource::getExecutionTime(uint8_t channel)
{
	const InspectionCommand& rInsCmd = m_cifXCard.getInspectionCmd();
	const int16_t& rTimeStamp1 = rInsCmd.m_channels[channel].m_timeStamp1;
	int32_t triggerTimeOffset = getPlcTriggerTime(rInsCmd.m_socRelative, rTimeStamp1) - m_cifXCard.getSyncSocRel();
	
	return m_cifXCard.getSyncTime() - 4.8 + static_cast<double> (triggerTimeOffset) / c_MicrosecondsPerMillisecond;
}

int32_t HardwareTriggerSource::getPlcTriggerTime(int32_t socRelative, int16_t timeStamp)
{
	int32_t triggerTimeOffset{ socRelative };

	//the time stamp only contains the least significant two bytes
	memcpy(&triggerTimeOffset, &timeStamp, sizeof(timeStamp));

	//When offset is in the past we need to wrap it by 16 bits
	if (triggerTimeOffset < socRelative)
	{
		triggerTimeOffset += c_TimeWrap;
	}

	return triggerTimeOffset;
}


void HardwareTriggerSource::createTriggerInfo(uint8_t channel)
{
	const ChannelIn& rChannel = m_cifXCard.getInspectionCmd().m_channels[channel];
	m_OldSequenceCode[channel] = m_NewSequenceCode[channel];
	m_NewSequenceCode[channel] = rChannel.m_sequence;

	double triggerTimeStamp = getExecutionTime(channel);

	//When sequence number has changed and is odd generate new trigger
	if (m_OldSequenceCode[channel] != m_NewSequenceCode[channel] && (m_NewSequenceCode[channel] % 2))
	{
		//currently: simulated triggers always have one trigger per product
		TriggerInformation TriggerInfo{channel, rChannel.m_currentObject.m_ID, rChannel.m_sequence, rChannel.m_triggerIndex, rChannel.m_triggerCount, triggerTimeStamp};
		addTriggerInfo(TriggerInfo);
	}
}


void HardwareTriggerSource::PrepareProductForSending(const Product& rProduct)
{
	ChannelOut& rChannel = m_inspectionState.m_channels[rProduct.m_Channel];
	rChannel.m_currentObject.m_ID = rProduct.m_ObjectId;
	rChannel.m_results.m_value[0] =  rProduct.m_InspectionResult;
}

uint64_t HardwareTriggerSource::getCurrentInterruptCount()
{
	return m_cifXCard.getProcessDataReadCount();
}




