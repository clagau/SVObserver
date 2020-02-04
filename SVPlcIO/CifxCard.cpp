//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file CifxCard.cpp
/// \brief Implementation of the class CifXCard
//******************************************************************************

#pragma region Includes
#include "stdafx.h"

#include "CifXCard.h"
#include "cifXErrors.h"
#include "cifXUser.h"
#include "InspectionState.h"
#include "PlcDataVersion.h"
#include "TLR_Results.h"
#include "TLR_Types.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr TLR_UINT32 c_cifxChannel = 0;
constexpr uint32_t c_timeout = 2;
#ifdef UNDER_RTSS
	constexpr char* c_BoardName = '\0';
#else
	constexpr char* c_BoardName = "CIFx0";
#endif

void APIENTRY interruptHandler(uint32_t notification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
	if (pvUser == nullptr)
	{
		printOutput("\t\t 'cifX' == nullptr\n");
		return;
	}

	CifXCard* pCifX = reinterpret_cast<CifXCard*> (pvUser);

	switch(notification)
	{
		case CIFX_NOTIFY_PD0_IN:
		{
			pCifX->readProcessData();
			break;
		}
		case CIFX_NOTIFY_SYNC:
		{
			pCifX->sendSyncCommand();
			break;
		}
	}
}

CifXCard::CifXCard(uint32_t CifXNodeId, uint32_t MaxPlcDataSize):
	m_CifXNodeId(CifXNodeId), m_maxPlcDataSize(MaxPlcDataSize), m_currentResult(CIFX_NO_ERROR)
{
}


TLR_RESULT CifXCard::OpenCifX(char* pBoardName)
{
	TLR_RESULT tResult = CIFX_DEV_NOT_READY;


	printOutput("Opening driver...\n");
	/* Open Driver */
	tResult = xDriverOpen(&m_commonData.m_hDriver);
	if (CIFX_NO_ERROR != tResult)
	{
		return tResult;
	}
	DRIVER_INFORMATION driverInfo;
	xDriverGetInformation(m_commonData.m_hDriver, sizeof(driverInfo), reinterpret_cast<void*> (&driverInfo));
	std::stringstream outputStream;
	outputStream << "Driver version " << driverInfo.abDriverVersion << '\n';
	printOutput(outputStream.str().c_str());


	printOutput("Opening channel ...\n");

	tResult = xChannelOpen(&m_commonData.m_hDriver, pBoardName, c_cifxChannel, &m_commonData.m_hChannel);
	if (CIFX_NO_ERROR != tResult)
	{
		return tResult;
	}
	CHANNEL_INFORMATION channelInfo;
	xChannelInfo(m_commonData.m_hChannel, sizeof(channelInfo), reinterpret_cast<void*> (&channelInfo));
	outputStream.clear();
	outputStream << "Firmware " << channelInfo.usFWMajor << '.' << channelInfo.usFWMinor << '\n';
	printOutput(outputStream.str().c_str());

	printOutput("Processing system restart...\n");
	tResult = xChannelReset(m_commonData.m_hChannel, CIFX_SYSTEMSTART, 2000);

	if (CIFX_NO_ERROR == tResult)
	{
		/* cifX successfully reset */

		/* Toggle Application Ready State Flag */

		TLR_UINT32  dummy;
		do
		{
			tResult = xChannelHostState(m_commonData.m_hChannel, CIFX_HOST_STATE_READY, &dummy, c_DriverTimeout_ms);

			/* if Dev is not ready, retry that action */
			if (CIFX_DEV_NOT_READY == tResult)
			{
				/* retry after 500 ms */
				Sleep(500);
			}
		} while (tResult == CIFX_DEV_NOT_READY);
	}
	m_configDataSetsMap[TelegramLayout::Layout1] = createConfigList(TelegramLayout::Layout1);
	m_configDataSetsMap[TelegramLayout::Layout2] = createConfigList(TelegramLayout::Layout2);

	return tResult;
}


TLR_RESULT CifXCard::SendConfigurationToCifX()
{
	/* common variables for packets */
	CIFX_PACKET tSendPkt = {{0}};
	CIFX_PACKET tRecvPkt = {{0}};
	CIFX_PACKET* ptRecvPkt = nullptr;

	/* build configuration packet */
	TLR_RESULT tResult = BuildConfigurationReq(&tSendPkt, m_CifXNodeId, m_maxPlcDataSize);

	/* send configuration Req packet */
	printOutput("Sending configuration request...\n");
	tResult = m_commonData.SendRecvPkt(&tSendPkt, &tRecvPkt);
	/* check if we got an error within configuration packet */
	if (TLR_S_OK != tResult)
	{
		printOutput("Configuration confirmation not received.");
		return tResult;

	}
	if (TLR_S_OK != tRecvPkt.tHeader.ulState)
	{
		std::stringstream outputStream;
		outputStream << "Configuration packet returned with error code: 0x" << std::hex << tRecvPkt.tHeader.ulState << '\n';
		printOutput(outputStream.str().c_str());
		return tResult;
	}

	return tResult;
}


TLR_RESULT CifXCard::WarmstartAndInitializeCifX()
{
	printOutput("Processing channel init...\n");

	TLR_RESULT tResult = xChannelReset(m_commonData.m_hChannel, CIFX_CHANNELINIT, 2000);

	if (CIFX_NO_ERROR != tResult)
	{
		return tResult;
	}
	printOutput("Waiting for Warmstart");
	TLR_UINT32  dummy;

	do
	{
		tResult = xChannelHostState(m_commonData.m_hChannel, CIFX_HOST_STATE_READY, &dummy, c_DriverTimeout_ms);
		printOutput(".");
	} while (CIFX_DEV_NOT_RUNNING == tResult);
	printOutput(" -> complete.\n");


	/* check CifX state */
	if ((CIFX_NO_ERROR == tResult) || (CIFX_DEV_NO_COM_FLAG == tResult))
	{
		if (CIFX_DEV_NO_COM_FLAG == tResult)
		{
			printOutput("\tNo communication!");
		}

		/* initialize the application */
		printOutput("Initializing application...\n");
		tResult = m_commonData.App_Initialize(m_maxPlcDataSize);

		if (TLR_S_OK != tResult)
		{
			return tResult;
		}

		printOutput("Setting bus state on...\n");
		/* variable for host state */
		TLR_UINT32  HostState;

		/* bus on */
		tResult = xChannelBusState(m_commonData.m_hChannel, CIFX_BUS_STATE_ON, &HostState, c_DriverTimeout_ms);

		/* bus activity begins here */
		if (CIFX_BUS_STATE_ON != HostState)
		{
			return tResult;
		}
	}

	uint32_t State;
	tResult = xChannelDMAState(m_commonData.m_hChannel, CIFX_DMA_STATE_ON, &State);

	if (CIFX_NO_ERROR != tResult)
	{
		return tResult;
	}

	tResult = xChannelRegisterNotification(m_commonData.m_hChannel, CIFX_NOTIFY_PD0_IN, interruptHandler, reinterpret_cast<void*> (this));
	if (CIFX_NO_ERROR != tResult)
	{
		return tResult;
	}

	tResult = xChannelRegisterNotification(m_commonData.m_hChannel, CIFX_NOTIFY_SYNC, interruptHandler, reinterpret_cast<void*> (this));
	if (CIFX_NO_ERROR != tResult)
	{
		return tResult;
	}

	uint8_t syncMode;
	xChannelCommonStatusBlock(m_commonData.m_hChannel, CIFX_CMD_READ_DATA, 0x30, 1, reinterpret_cast<void*> (&syncMode));

	return tResult;
}

TLR_RESULT CifXCard::closeDriver(bool showDetails)
{
	if (showDetails)
	{
		printOutput("Closing application and freeing buffers...\n");
	}
	TLR_RESULT tResult = xChannelUnregisterNotification(m_commonData.m_hChannel, CIFX_NOTIFY_PD0_IN);
	if (TLR_S_OK != tResult)
	{
		return tResult;
	}

	tResult = xChannelUnregisterNotification(m_commonData.m_hChannel, CIFX_NOTIFY_SYNC);
	if (TLR_S_OK != tResult)
	{
		return tResult;
	}
	TLR_UINT32  HostState;
	tResult = xChannelBusState(m_commonData.m_hChannel, CIFX_BUS_STATE_OFF, &HostState, c_DriverTimeout_ms);
	if (TLR_S_OK != tResult)
	{
		return tResult;
	}

	tResult = m_commonData.App_Finalize();
	if (TLR_S_OK != tResult)
	{
		return tResult;
	}

	if (showDetails)
	{
		printOutput("Closing Channel...\n");
	}
	tResult = xChannelClose(m_commonData.m_hChannel);

	if (TLR_S_OK != tResult)
	{
		return tResult;
	}

	if (showDetails)
	{
		printOutput("Closing Driver...\n");
	}
	tResult = xDriverClose(m_commonData.m_hDriver);

	return tResult;
}

void CifXCard::readProcessData()
{
	m_TelegramReceiveTime = SvTl::GetTimeStamp();
	m_processDataReadCount++;

	m_currentResult = xChannelIORead(m_commonData.m_hChannel, 0, 0, m_commonData.m_readBufferSize, m_commonData.m_pReadBuffer, 2);

	if (m_currentResult != CIFX_NO_ERROR)
	{
		if (CIFX_DEV_NO_COM_FLAG != m_currentResult) // "COM-flag not set" occurs often: do not report
		{
			printOutput("\t\t ReadProcessData() failed: ");
		}
		return;
	}
	
	//Reduce lock guard scope
	{
		std::lock_guard<std::mutex> guard {m_cifxMutex};
		uint8_t* pData = m_commonData.m_pReadBuffer;
		memcpy(&m_inputTelegram, pData, sizeof(Telegram));
		pData += sizeof(Telegram);

		switch(m_inputTelegram.m_content)
		{
			case TelegramContent::TimeSyncData:
			{
				memcpy(&m_timeSync, pData, sizeof(TimeSync));
				m_inspectionCmd = InspectionCommand {};
				break;
			}
			case TelegramContent::OperationData:
			{
				memcpy(&m_inspectionCmd, pData, sizeof(InspectionCommand));
				m_syncSocRelative = m_inspectionCmd.m_socRelative;
				m_syncTime = m_TelegramReceiveTime;
				break;
			}
			default:
			{
				break;
			}
		}
	}

	if (m_hTelegramReadEvent != nullptr)
	{
		::SetEvent(m_hTelegramReadEvent);
	}

	m_processDataCanBeRead = (m_currentResult == CIFX_NO_ERROR);

}

void CifXCard::writeProcessData()
{
	// write RTD-Data

	m_currentResult = xChannelIOWrite(m_commonData.m_hChannel, 0, 0, m_commonData.m_writeBufferSize, m_commonData.m_pWriteBuffer, c_timeout);

	if (CIFX_NO_ERROR != m_currentResult)
	{
		printOutput("WriteProcess Data\n");
	}
}

void CifXCard::sendSyncCommand()
{
	xChannelSyncState(m_commonData.m_hChannel, CIFX_SYNC_ACKNOWLEDGE_CMD, c_timeout, nullptr);
}


void CifXCard::closeCifX()
{
	closeDriver(false);

	printOutput("CifX card closed.\n");
}


bool CifXCard::OpenAndInitializeCifX()
{
	TLR_RESULT result = OpenCifX(c_BoardName);

	if ((CIFX_NO_ERROR == result) || (CIFX_DEV_NOT_RUNNING == result))
	{
		result = SendConfigurationToCifX();
		
		if (!(CIFX_NO_ERROR != result))
		{
			result = WarmstartAndInitializeCifX();

			if (CIFX_DEV_NO_COM_FLAG == result)
			{
				//this is allowed here because the "other side" may not yet have started up
				return true;
			}

			if (CIFX_NO_ERROR == result)
			{
				return true;
			}
		}
	}
	return false;
}

void CifXCard::sendVersion()
{
	m_protocolInitialized = false;
	PlcDataVersion plcVersion;
	plcVersion.m_major = c_majorVersion;
	plcVersion.m_minor = c_minorVersion;
	const uint8_t* pData = reinterpret_cast<const uint8_t*> (&plcVersion);
	writeResponseData(pData, sizeof(PlcDataVersion));
}

void CifXCard::setPlcLoopSyncTime()
{
	m_protocolInitialized = false;
	m_plcSendTime = m_timeSync.m_plcBusCycles;
	writeResponseData(nullptr, 0);
}

void CifXCard::sendConfigList()
{
	m_protocolInitialized = false;
	const std::vector<ConfigDataSet>& rConfigList =  m_configDataSetsMap[m_inputTelegram.m_layout];
	if(0 < rConfigList.size())
	{
		const uint8_t* pData = reinterpret_cast<const uint8_t*> (&rConfigList[0]);
		writeResponseData(pData, sizeof(ConfigDataSet) * rConfigList.size());
	}
}

void CifXCard::sendOperationData(const InspectionState& rState)
{
	m_protocolInitialized = true;
	const uint8_t* pData = reinterpret_cast<const uint8_t*> (&rState);
	writeResponseData(pData, sizeof(InspectionState));
}

void CifXCard::sendDefaultResponse()
{
	writeResponseData(nullptr, 0);
}

std::vector<ConfigDataSet> CifXCard::createConfigList(TelegramLayout layout)
{
	static std::unordered_map<std::type_index, PlcDataType> dataTypeList
	{
		{typeid(bool), PlcDataType::BoolIndex},
		{typeid(int8_t), PlcDataType::Int8Index},
		{typeid(int16_t), PlcDataType::Int16Index},
		{typeid(int32_t), PlcDataType::Int32Index},
		{typeid(uint8_t), PlcDataType::Uint8Index},
		{typeid(uint16_t), PlcDataType::Uint16Index},
		{typeid(uint32_t), PlcDataType::Uint32Index},
		{typeid(float), PlcDataType::FloatIndex},
		{typeid(double), PlcDataType::DoubleIndex},
	};

	std::vector<ConfigDataSet> result;
	result.resize(c_ConfigListSize);
	size_t configIndex {0ULL};
	uint16_t startByte {1};

	switch(layout)
	{
		case TelegramLayout::Layout1:
		{
			InspectionCommand insCmd;

			result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(insCmd.m_enable)], startByte, sizeof(insCmd.m_enable)};
			//Do startByte always before configIndex
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(insCmd.m_socAbsSeconds)], startByte, sizeof(insCmd.m_socAbsSeconds)};
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(insCmd.m_socAbsNanoseconds)], startByte, sizeof(insCmd.m_socAbsNanoseconds)};
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(insCmd.m_socRelative)], startByte, sizeof(insCmd.m_socRelative)};
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			for(const auto& rReserved : insCmd.m_reserved)
			{
				result[configIndex] = ConfigDataSet {0, dataTypeList[typeid(rReserved)], startByte, sizeof(rReserved)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
			}

			for(const auto& rChannel : insCmd.m_channels)
			{
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_unitControl)], startByte, sizeof(rChannel.m_unitControl)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_sequence)], startByte, sizeof(rChannel.m_sequence)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_timeStamp1)], startByte, sizeof(rChannel.m_timeStamp1)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_timeStamp2)], startByte, sizeof(rChannel.m_timeStamp2)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectType)], startByte, sizeof(rChannel.m_currentObjectType)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectID)], startByte, sizeof(rChannel.m_currentObjectID)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_originalObjectType)], startByte, sizeof(rChannel.m_originalObjectType)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_originalObjectID)], startByte, sizeof(rChannel.m_originalObjectID)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_triggerIndex)], startByte, sizeof(rChannel.m_triggerIndex)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_triggerCount)], startByte, sizeof(rChannel.m_triggerCount)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_generalValue)], startByte, sizeof(rChannel.m_generalValue)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
			}
			///This is the way the PLC sets the not used configuration sets
			while(configIndex < c_ConfigListSize)
			{
				result[configIndex] = ConfigDataSet {0, 0, startByte, 0ULL};
				configIndex++;
			}
			break;
		}
		case TelegramLayout::Layout2:
		{
			InspectionState insState;
			
			for (const auto& rHeader : insState.m_header)
			{
				result[configIndex] = ConfigDataSet {0, dataTypeList[typeid(rHeader)], startByte, sizeof(rHeader)};
				//Do startByte always before configIndex
				startByte += result[configIndex].m_byteSize;
				configIndex++;
			}

			for (const auto& rChannel : insState.m_channels)
			{
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectType)], startByte, sizeof(rChannel.m_currentObjectType)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectID)], startByte, sizeof(rChannel.m_currentObjectID)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				for (const auto& rResult : rChannel.m_results)
				{
					result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rResult)], startByte, sizeof(rResult)};
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}
				result[configIndex] = ConfigDataSet {c_modeSingleDirect, dataTypeList[typeid(rChannel.m_generalValue)], startByte, sizeof(rChannel.m_generalValue)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
			}
			///This is the way the PLC sets the not used configuration sets
			while (configIndex < c_ConfigListSize)
			{
				result[configIndex] = ConfigDataSet {0, 0, startByte, 0ULL};
				configIndex++;
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

void CifXCard::writeResponseData(const uint8_t* pSdoDynamic, size_t sdoDynamicSize)
{

	Telegram outputTelegram;
	///The content ID needs to be incremented for each send
	m_contentID++;
	///Content ID is not allowed to be 0
	if(0 == m_contentID)
	{
		m_contentID++;
	}
	outputTelegram.m_contentID = m_contentID;
	outputTelegram.m_referenceID = m_inputTelegram.m_contentID;
	outputTelegram.m_type = TelegramType::Response;
	outputTelegram.m_content = m_inputTelegram.m_content;
	outputTelegram.m_layout = m_inputTelegram.m_layout;
	outputTelegram.m_systemStatus = m_ready ? SystemStatus::AppReady : SystemStatus::ComReady;

	{
		std::lock_guard<std::mutex> guard {m_cifxMutex};
		//Clear the write buffer
		memset(m_commonData.m_pWriteBuffer, 0, m_commonData.m_writeBufferSize);
		//Copy the SDO Static data
		uint8_t* pData = m_commonData.m_pWriteBuffer;
		memcpy(pData, &outputTelegram, sizeof(Telegram));
		pData += sizeof(Telegram);

		//If nullptr or size 0 then no dynamic data
		if (nullptr != pSdoDynamic && 0 != sdoDynamicSize)
		{
			//Copy the SDO Dynamic data
			memcpy(pData, pSdoDynamic, sdoDynamicSize);
		}
	}
	m_currentResult = xChannelIOWrite(m_commonData.m_hChannel, 0, 0, m_commonData.m_writeBufferSize, m_commonData.m_pWriteBuffer, 2);

	if (CIFX_NO_ERROR != m_currentResult)
	{
		printOutput("Write Response Data Failed\n");
	}

}
} //namespace SvPlc
