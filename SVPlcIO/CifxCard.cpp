//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file CifxCard.cpp
/// \brief Implementation of the class CifXCard
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <cifXErrors.h>
#include <cifXUser.h>
#include <Epl_Common_Defines.h>
#include <EplCn_If_Public.h>
#include <Hil_ApplicationCmd.h>

#include "CifXCard.h"
#include "InspectionState.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

//#define TRACE_PLC
//This is to avoid CheckInclude false positives EPLCN_IF_QUEUE_NAME

namespace SvPlc
{
constexpr uint32_t cCifxChannel = 0;
constexpr uint32_t cDriverTimeout = 100;
constexpr uint32_t cWaitTimeout = 500;
constexpr uint32_t cResetTimeout = 5000;
constexpr uint32_t cTimeout = 2;

constexpr uint16_t cMaxHostTries = 50;
constexpr const char* cBoardName = "CIFx0";
/* Identity Information */
constexpr uint32_t cVendorId = 0x00000044UL;	///Hilscher Vendor ID
constexpr uint32_t cProductCode = 1UL;			///CIFX
constexpr uint32_t cSerialNumber = 0UL;			///Use serial number from SecMem or FDL
constexpr uint32_t cRevisionNumber = 0UL;

#if defined (TRACE_THEM_ALL) || defined (TRACE_PLC)
//This is used to check which config set does not match
ConfigDataSet gPlcConfig[cConfigListSize];
#endif

void APIENTRY notificationHandler(uint32_t notification, uint32_t, void* , void* pvUser)
{
	if (pvUser == nullptr)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidMemoryPointer, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	CifXCard* pCifX = reinterpret_cast<CifXCard*> (pvUser);

	pCifX->readProcessData(notification);
}

CifXCard::CifXCard(uint16_t CifXNodeId, uint16_t MaxPlcDataSize):
 m_CifXNodeId(CifXNodeId)
,m_maxPlcDataSize(MaxPlcDataSize)
{
}

void CifXCard::readProcessData(uint32_t notification)
{
	double timeStamp = SvUl::GetTimeStamp();
	if (notification != m_notifyType || false == m_cifxLoadLib.isInitilized())
	{
		return;
	}

	int32_t result = m_cifxLoadLib.m_pChannelIORead(m_hChannel, 0, 0, m_maxPlcDataSize, m_pReadBuffer.get(), cTimeout);
	if (CIFX_NO_ERROR == result)
	{
		InputData inputData;
		inputData.m_notificationTime = timeStamp;
		uint8_t* pData = m_pReadBuffer.get();
		memcpy(&inputData.m_telegram, pData, sizeof(Telegram));
		pData += sizeof(Telegram);
		static uint16_t prevContentID {0U};

		if (prevContentID != inputData.m_telegram.m_contentID)
		{
			prevContentID = inputData.m_telegram.m_contentID;
			switch (inputData.m_telegram.m_content)
			{
				case TelegramContent::VersionData:
				{
					memcpy(&inputData.m_dynamicData[0], pData, sizeof(PlcVersion));
					break;
				}

#if defined (TRACE_THEM_ALL) || defined (TRACE_PLC)
				case TelegramContent::ConfigurationData:
				{
					memcpy(&gPlcConfig, pData, cConfigListSize * sizeof(ConfigDataSet));
				}
#endif
				case TelegramContent::OperationData:
				{
					memcpy(&inputData.m_dynamicData[0], pData, cCmdDataSize);
					break;
				}
				default:
				{
					break;
				}
			}
			if (m_hTelegramReadEvent != nullptr)
			{
				//Reduce lock guard scope
				{
					std::lock_guard<std::mutex> guard {m_cifxMutex};
					m_inputDataQueue.emplace(std::move(inputData));
				}

				::SetEvent(m_hTelegramReadEvent);
			}
		}
	}
	if (CIFX_NOTIFY_SYNC == m_notifyType)
	{
		uint32_t syncErrorCount{ 0 };
		m_cifxLoadLib.m_pChannelSyncState(m_hChannel, CIFX_SYNC_SIGNAL_CMD, cDriverTimeout, &syncErrorCount);
	}
}

bool CifXCard::popInputDataQueue()
{
	std::lock_guard<std::mutex> guard {m_cifxMutex};
	if (false == m_inputDataQueue.empty())
	{
		m_currentInputData = m_inputDataQueue.front();
		m_inputDataQueue.pop();
		return true;
	}
	return false;
}

void CifXCard::closeCifX()
{
	if(m_cifxLoadLib.isInitilized())
	{
		::OutputDebugString("Closing application and freeing buffers...\n");
		m_cifxLoadLib.m_pChannelUnregisterNotification(m_hChannel, m_notifyType);
		m_cifxLoadLib.m_pChannelBusState(m_hChannel, CIFX_BUS_STATE_OFF, nullptr, cDriverTimeout);
		SendRecvCmdPkt(HIL_UNREGISTER_APP_REQ);
		::OutputDebugString("Closing Channel...\n");
		m_cifxLoadLib.m_pChannelClose(m_hChannel);
		::OutputDebugString("Closing Driver...\n");
		m_cifxLoadLib.m_pDriverClose(m_hDriver);
		::OutputDebugString("CifX card closed.\n");
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxClosed, SvStl::SourceFileParams(StdMessageParams));
	}
}


HRESULT CifXCard::OpenAndInitializeCifX(const std::string& rAdditionalData)
{
	HRESULT result = OpenCifX(rAdditionalData);

	if (CIFX_NO_ERROR == result)
	{
		result = SendConfigurationToCifX();
		
		if (CIFX_NO_ERROR == result)
		{
			result = WarmstartAndInitializeCifX();
		}
	}

	if (CIFX_NO_ERROR == result)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxInitializationSuccess, m_sourceFileParam);
	}
	else
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CifxInitializationError, m_sourceFileParam);
	}
	return result;
}

void CifXCard::sendVersion()
{
	m_protocolInitialized = false;
	m_plcVersion = PlcVersion::PlcDataNone;
	PlcVersion plcVersion{ PlcVersion::PlcData1 };
	if (0 == memcmp(&plcVersion, &m_currentInputData.m_dynamicData[0], sizeof(PlcVersion)))
	{
		m_plcVersion = plcVersion;
	}
	else
	{
		plcVersion = PlcVersion::PlcData2;
		if (0 == memcmp(&plcVersion, &m_currentInputData.m_dynamicData[0], sizeof(PlcVersion)))
		{
			m_plcVersion = plcVersion;
		}
	}

	const uint8_t* pData = reinterpret_cast<const uint8_t*> (&m_plcVersion);
	writeResponseData(m_currentInputData.m_telegram, pData, sizeof(PlcVersion));

	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	SvDef::StringVector msgList;
	msgList.push_back(std::to_string(LOBYTE(plcVersion)));
	msgList.push_back(std::to_string(HIBYTE(plcVersion)));
	Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxPlcVersion, msgList, SvStl::SourceFileParams(StdMessageParams));
	::OutputDebugString("Send Version\n");
	//Now that the version is known create the config list
	::OutputDebugString("Create config lists\n");
	m_configDataSetVector.reserve(2);
	m_configDataSetVector.emplace_back(TelegramLayout::Layout1, createConfigList(TelegramLayout::Layout1));
	m_configDataSetVector.emplace_back(TelegramLayout::Layout2, createConfigList(TelegramLayout::Layout2));
}

void CifXCard::sendConfigList()
{
	uint8_t layoutIndex = m_currentInputData.m_telegram.m_layout;
	if(0 < layoutIndex )
	{
		///Layout index is always either 1 or 2
		layoutIndex = (layoutIndex % 2) == 1 ? 1 : 2;
		const auto iter = std::find_if(m_configDataSetVector.begin(), m_configDataSetVector.end(), [&layoutIndex](const auto& rEntry) { return rEntry.first == static_cast<TelegramLayout> (layoutIndex); });
		if(iter != m_configDataSetVector.end() && 0 < iter->second.size())
		{
			const uint8_t* pData = reinterpret_cast<const uint8_t*> (&iter->second[0]);
			writeResponseData(m_currentInputData.m_telegram, pData, sizeof(ConfigDataSet) * cConfigListSize);
			SvStl::MessageManager Msg(SvStl::MsgType::Log);
			SvDef::StringVector msgList{std::to_string(layoutIndex)};
			Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxPlcConfigData, msgList, SvStl::SourceFileParams(StdMessageParams));
			::OutputDebugString("Send config data Layout\n");
#if defined (TRACE_THEM_ALL) || defined (TRACE_PLC)
			if (cConfigListSize == iter->second.size())
			{
				const std::vector<ConfigDataSet>& rConfigList = iter->second;
				for (int i = 0; i < cConfigListSize; ++i)
				{
					if (rConfigList[i].m_byteSize != gPlcConfig[i].m_byteSize ||
						rConfigList[i].m_dataType != gPlcConfig[i].m_dataType ||
						rConfigList[i].m_mode != gPlcConfig[i].m_mode ||
						rConfigList[i].m_startByte != gPlcConfig[i].m_startByte)
					{
						std::string text{ _T("ConfigID ") };
						text += std::to_string(i) + _T(" ==> SVObserver Data: ");
						text += std::to_string(rConfigList[i].m_byteSize) + ',' + std::to_string(rConfigList[i].m_dataType) + ',' + std::to_string(rConfigList[i].m_mode) + ',' + std::to_string(rConfigList[i].m_startByte);
						text += _T("<> PLC Data: ");
						text += std::to_string(gPlcConfig[i].m_byteSize) + ',' + std::to_string(gPlcConfig[i].m_dataType) + ',' + std::to_string(gPlcConfig[i].m_mode) + ',' + std::to_string(gPlcConfig[i].m_startByte) + '\n';
						::OutputDebugString(text.c_str());
					}
				}
			}
#endif
		}
	}
}

void CifXCard::sendOperationData(const InspectionState1& rState)
{
	if (false == m_protocolInitialized)
	{
		m_protocolInitialized = (m_plcVersion != PlcVersion::PlcDataNone);
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxPlcOpertaion, SvStl::SourceFileParams(StdMessageParams));
		::OutputDebugString("Operation mode\n");
	}
	//insState2 must declared be here due to pData pointing to it!
	InspectionState2 insState2;
	size_t dynamicDataSize {0ULL};
	const uint8_t* pData {nullptr};
	if (PlcVersion::PlcData1 == m_plcVersion)
	{
		pData = reinterpret_cast<const uint8_t*> (&rState);
		dynamicDataSize = sizeof(InspectionState1);
	}
	else if (PlcVersion::PlcData2 == m_plcVersion)
	{
		for (unsigned int i = 0; i < cNumberOfChannels; ++i)
		{
			//ChannelOut2 does not have the measurement value array ChannelOut1 so do not copy it
			memcpy(&insState2.m_channels[i], &rState.m_channels[i], sizeof(ChannelOut2));
		}
		pData = reinterpret_cast<const uint8_t*> (&insState2);
		dynamicDataSize = sizeof(InspectionState2);
	}

	writeResponseData(m_currentInputData.m_telegram, pData, dynamicDataSize);
}

void CifXCard::setReady(bool ready)
{
	std::lock_guard<std::mutex> guard{m_cifxMutex};
	m_ready = ready;
}

int32_t CifXCard::OpenCifX(const std::string& rAdditionalData)
{
	int32_t result{ CIFX_DEV_NOT_READY };

	m_sourceFileParam.clear();
	result = m_cifxLoadLib.Open();
	/// Load Dll failed!
	if (S_OK != result)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CifxDllLoadError, SvStl::SourceFileParams(StdMessageParams));
		return result;
	}
	::OutputDebugString("Opening driver...\n");
	result = m_cifxLoadLib.m_pDriverOpen(&m_hDriver);
	if (CIFX_NO_ERROR != result)
	{
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		return result;
	}
	DRIVER_INFORMATION driverInfo;
	memset(&driverInfo, 0, sizeof(driverInfo));
	m_cifxLoadLib.m_pDriverGetInformation(m_hDriver, sizeof(driverInfo), reinterpret_cast<void*> (&driverInfo));
	std::string driverVersion{ driverInfo.abDriverVersion };
	::OutputDebugString((_T("Driver version ") + driverVersion + '\n').c_str() );

	::OutputDebugString("Opening channel ...\n");
	result = m_cifxLoadLib.m_pChannelOpen(m_hDriver, const_cast<char*> (cBoardName), cCifxChannel, &m_hChannel);
	if (CIFX_NO_ERROR != result)
	{
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		return result;
	}
	CHANNEL_INFORMATION channelInfo;
	m_cifxLoadLib.m_pChannelInfo(m_hChannel, sizeof(channelInfo), reinterpret_cast<void*> (&channelInfo));
	std::string firmware{"Power Link FW: " + std::to_string(channelInfo.usFWMajor) + '.' + std::to_string(channelInfo.usFWMinor) + '.' + std::to_string(channelInfo.usFWBuild) + '.' + std::to_string(channelInfo.usFWRevision) };
	m_notifyType =  (rAdditionalData == firmware) ? CIFX_NOTIFY_PD0_IN : CIFX_NOTIFY_SYNC;
	::OutputDebugString(reinterpret_cast<char*> (channelInfo.abFWName));
	::OutputDebugString((' ' + firmware + '\n').c_str());
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	SvDef::StringVector msgList;
	msgList.push_back(driverVersion);
	msgList.push_back(firmware);
	Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxVersionInfo, msgList, SvStl::SourceFileParams(StdMessageParams));

	result = m_cifxLoadLib.m_pChannelReset(m_hChannel, CIFX_CHANNELINIT, cResetTimeout);
	if (CIFX_NO_ERROR != result)
	{
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		return result;
	}
	::OutputDebugString("Processing system restart...\n");
	result = m_cifxLoadLib.m_pChannelReset(m_hChannel, CIFX_SYSTEMSTART, cResetTimeout);
	if (CIFX_NO_ERROR == result)
	{
		int hostTries{ 1 };
		uint32_t  dummy{ 0UL };
		do
		{
			result = m_cifxLoadLib.m_pChannelHostState(m_hChannel, CIFX_HOST_STATE_READY, &dummy, cDriverTimeout);

			if (CIFX_DEV_NOT_READY == result)
			{
				///Sleep and try again
				Sleep(cWaitTimeout);
				++hostTries;
			}
		} while (result == CIFX_DEV_NOT_READY && hostTries < cMaxHostTries);
		if (CIFX_NO_ERROR != result)
		{
			m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		}
	}

	return result;
}

int32_t CifXCard::SendConfigurationToCifX()
{
	/* common variables for packets */
	CIFX_PACKET sendPkt;
	CIFX_PACKET recvPkt;

	m_sourceFileParam.clear();
	memset(&sendPkt, 0, sizeof(sendPkt));
	memset(&recvPkt, 0, sizeof(recvPkt));

	/* build configuration packet */
	BuildConfigurationReq(&sendPkt, m_CifXNodeId, m_maxPlcDataSize);

	/* send configuration Req packet */
	int32_t result = SendRecvPkt(&sendPkt, &recvPkt);
	/* check if we got an error within configuration packet */
	if (CIFX_NO_ERROR != result)
	{
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		return result;

	}
	if (CIFX_NO_ERROR != recvPkt.tHeader.ulState)
	{
		std::stringstream outputStream;
		outputStream << std::hex << recvPkt.tHeader.ulState;
		SvDef::StringVector msgList;
		msgList.push_back(outputStream.str());
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CifxConfigReturnError, msgList, m_sourceFileParam);
		return result;
	}

	return result;
}

int32_t CifXCard::WarmstartAndInitializeCifX()
{
	m_sourceFileParam.clear();
	if (false == m_cifxLoadLib.isInitilized())
	{
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		return E_FAIL;
	}
	::OutputDebugString("Processing channel init...\n");

	uint32_t result = m_cifxLoadLib.m_pChannelReset(m_hChannel, CIFX_CHANNELINIT, cResetTimeout);

	if (CIFX_NO_ERROR != result)
	{
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		return result;
	}
	::OutputDebugString("Waiting for Warm start");

	uint32_t  dummy{ 0UL };
	do
	{
		result = m_cifxLoadLib.m_pChannelHostState(m_hChannel, CIFX_HOST_STATE_READY, &dummy, cDriverTimeout);
		::OutputDebugString(".");
	} while (CIFX_DEV_NOT_RUNNING == result);
	::OutputDebugString(" -> complete.\n");


	/* check CifX state */
	if ((CIFX_NO_ERROR == result) || (CIFX_DEV_NO_COM_FLAG == result))
	{
		if (CIFX_DEV_NO_COM_FLAG == result)
		{
			::OutputDebugString("No communication!\n");
		}

		m_pReadBuffer = std::make_unique<uint8_t[]>(m_maxPlcDataSize);
		m_pWriteBuffer = std::make_unique<uint8_t[]>(m_maxPlcDataSize);

		/* initialize the read and write buffer with zero */
		memset(m_pReadBuffer.get(), 0, m_maxPlcDataSize);
		memset(m_pWriteBuffer.get(), 0, m_maxPlcDataSize);

		///Only call this when using sync notification
		if (CIFX_NOTIFY_SYNC == m_notifyType)
		{
			result = SendRecvCmdPkt(HIL_SET_TRIGGER_TYPE_REQ);
			if (CIFX_NO_ERROR != result)
			{
				m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
				return result;
			}
		}

		result = SendRecvCmdPkt(HIL_REGISTER_APP_REQ);
		if (CIFX_NO_ERROR != result)
		{
			m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
			return result;
		}

		uint32_t State{ 0UL };
		result = m_cifxLoadLib.m_pChannelDMAState(m_hChannel, CIFX_DMA_STATE_ON, &State);
		if (CIFX_NO_ERROR != result)
		{
			m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
			return result;
		}
		::OutputDebugString("DMA is on\n");

		result = m_cifxLoadLib.m_pChannelRegisterNotification(m_hChannel, m_notifyType, notificationHandler, reinterpret_cast<void*> (this));
		if (CIFX_NO_ERROR != result)
		{
			m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
			return result;
		}
		::OutputDebugString("Cifx Notification Handler Ready\n");

		result = CIFX_NO_ERROR;
	}

	return result;
}

int32_t CifXCard::SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt)
{
	if(nullptr == pSendPkt || nullptr == pRecvPkt)
	{
		return E_POINTER;
	}
	/* fire the packet */
	int32_t result = m_cifxLoadLib.m_pChannelPutPacket(m_hChannel, pSendPkt, cDriverTimeout);
	if (CIFX_NO_ERROR != result)
	{
		return result;
	}

	/* ok, at this point we have successfully sent a packet */

	/* check for packets to receive */
	/* the ulCnfCmd is always: ulReqCmd | 0x01 */
	uint32_t ulCnfCmd = pSendPkt->tHeader.ulCmd | 0x01;

	while (CIFX_NO_ERROR == (result = m_cifxLoadLib.m_pChannelGetPacket(m_hChannel, sizeof(*pRecvPkt), pRecvPkt, cDriverTimeout)))
	{
		/* check for our packet */
		if (pRecvPkt->tHeader.ulCmd == ulCnfCmd)
		{
			/* it is our packet, so return its status as result */
			result = pRecvPkt->tHeader.ulState;
			/* Note: we also return the packet which we have received (by reference, see signature of function) */
			/* we have received our packet, so we can break here*/
			break;
		}
	}

	return result;
}


int32_t CifXCard::SendRecvCmdPkt(uint32_t command)
{
	CIFX_PACKET sendPkt;
	CIFX_PACKET recvPkt;

	///Need to initialize CIFX_PACKET manually!
	memset(&sendPkt, 0, sizeof(sendPkt));
	memset(&recvPkt, 0, sizeof(recvPkt));

	if (HIL_SET_TRIGGER_TYPE_REQ == command)
	{
		HIL_SET_TRIGGER_TYPE_REQ_T* pPacket = reinterpret_cast<HIL_SET_TRIGGER_TYPE_REQ_T*> (&sendPkt);
		memset(pPacket, 0, sizeof(HIL_SET_TRIGGER_TYPE_REQ_T));
		pPacket->tHead.ulCmd = command;
		pPacket->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
		pPacket->tHead.ulLen = sizeof(HIL_SET_TRIGGER_TYPE_REQ_DATA_T);
		pPacket->tData.usPdInHskTriggerType = HIL_TRIGGER_TYPE_SYNC_NONE;
		pPacket->tData.usPdOutHskTriggerType = HIL_TRIGGER_TYPE_SYNC_NONE;
		pPacket->tData.usSyncHskTriggerType = HIL_TRIGGER_TYPE_SYNC_TIMED_ACTIVATION;
	}
	else
	{
		TLR_EMPTY_PACKET_T* pPacket = reinterpret_cast<TLR_EMPTY_PACKET_T*> (&sendPkt);
		pPacket->tHead.ulCmd = command;
		pPacket->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;
	}

	return SendRecvPkt(&sendPkt, &recvPkt);
}

void CifXCard::BuildConfigurationReq(CIFX_PACKET* pPacket, uint16_t NodeId, uint16_t DataLength)
{
	EPLCN_IF_SET_CONFIG_REQ_T* pConfigReq = reinterpret_cast<EPLCN_IF_SET_CONFIG_REQ_T*> (pPacket);

	pConfigReq->tHead.ulDest = 0x20;						//Destination of packet
	pConfigReq->tHead.ulSrc = 0x10;							//Source of packet, process queue not always necessary
	pConfigReq->tHead.ulLen = sizeof(pConfigReq->tData);	//Length of packet data without header
	pConfigReq->tHead.ulCmd = EPLCN_IF_SET_CONFIG_REQ;
	pConfigReq->tHead.ulSta = 0;							//Status code of operation
	pConfigReq->tHead.ulExt = 0;							//extension

	uint32_t stackCfgFlags = (CIFX_NOTIFY_SYNC == m_notifyType) ? MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_USE_CUSTOM_PDO_OBJ : MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_HOST_TRIGGERED_PREQ_XCHG;
	pConfigReq->tData.ulSystemFlags = 0;                      //System Flags  
	pConfigReq->tData.ulWatchdogTime = 1000;					//Watchdog time
	pConfigReq->tData.ulStackCfgFlags = stackCfgFlags;
	pConfigReq->tData.ulVendorId = cVendorId;
	pConfigReq->tData.ulProductCode = cProductCode;
	pConfigReq->tData.ulRevisionNumber = cRevisionNumber;
	pConfigReq->tData.ulSerialNumber = cSerialNumber;
	pConfigReq->tData.ulCycleLength = 1000;					//Cycle Length configuration (ns)
	pConfigReq->tData.ulDeviceType = 0;						//No standard profile
	pConfigReq->tData.ulFeatureFlags = 0;						//Simple configuration. Stack will set the common supported features
	pConfigReq->tData.usPReqDataSize = DataLength;			// Data length Master->Slave
	pConfigReq->tData.usPResDataSize = DataLength;			// Data length Slave->Master
	pConfigReq->tData.bPReqMappingVersion = 0;				// PReq Mapping Version
	pConfigReq->tData.bPResMappingVersion = 0;				// PRes Mapping Version
	pConfigReq->tData.usMaxPReqDataSize = EPL_C_DLL_ISOCHR_MAX_PAYL; // >= ptConfigReq->tData.usPReqDataSize
	pConfigReq->tData.usMaxPResDataSize = EPL_C_DLL_ISOCHR_MAX_PAYL; // >= ptConfigReq->tData.usPResDataSize
	pConfigReq->tData.bNodeId = static_cast<uint8_t> (NodeId);	//EPL node id (range 1 to 239)
	pConfigReq->tData.ulGatewayAddress = 0;					//Stack configures default value (192.168.100.254)
	memset(pConfigReq->tData.abNodeName, 0, 32);			//Stack configures the Host Name in form <nodeId>-<vendorId>
	pConfigReq->tData.bNumberOfStatusEntries = 0;			//configured status entries

	pConfigReq->tData.bUseCustomThreshold = 0;				//Use default Thresholds
	pConfigReq->tData.ulThresholdCollision = 0;				//Collision Threshold (ignored if set to 0)
	pConfigReq->tData.ulThresholdLossSoC = 0;					//LossSoC Threshold   (ignored if set to 0)
	pConfigReq->tData.ulThresholdLossSoA = 0;					//LossSoA Threshold   (ignored if set to 0)
	pConfigReq->tData.ulThresholdLossPReq = 0;				//LossPReq Threshold  (ignored if set to 0)
	pConfigReq->tData.ulThresholdSoCJitter = 0;				//SoCJitter Threshold (ignored if set to 0)
	pConfigReq->tData.ulThresholdCrcError = 0;				//CrcError Threshold  (ignored if set to 0)
	pConfigReq->tData.ulMinCycleLength = 0;					//Use minimum cycle length supported by the hardware
	pConfigReq->tData.ulThresholdSoCJitter = 0;				//Not evaluated, since we use default thresholds

	// Set the reserved block to 0 to ensure the compatibility with future versions of the stack
	memset(pConfigReq->tData.aulReserved, 0, sizeof(pConfigReq->tData.aulReserved));
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
	result.resize(cConfigListSize);
	size_t configIndex {0ULL};
	uint16_t startByte {1};

	switch(layout)
	{
		case TelegramLayout::Layout1:
		{
			m_triggerDataOffset = 0UL;
			if (PlcVersion::PlcData1 == m_plcVersion)
			{
				InspectionCommand1 insCmd;

				result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(insCmd.m_socRelative)], startByte, sizeof(insCmd.m_socRelative) };
				m_triggerDataOffset += result[configIndex].m_byteSize;
				//Do startByte always before configIndex
				startByte += result[configIndex].m_byteSize;
				configIndex++;

				for (int i = 0; i < insCmd.m_channels.size(); ++i)
				{
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn1::m_unitControl)], startByte, sizeof(ChannelIn1::m_unitControl) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn1::m_sequence)], startByte, sizeof(ChannelIn1::m_sequence) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn1::m_timeStamp)], startByte, sizeof(ChannelIn1::m_timeStamp) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn1::m_objectType)], startByte, sizeof(ChannelIn1::m_objectType) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn1::m_objectID)], startByte, sizeof(ChannelIn1::m_objectID) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn1::m_triggerIndex)], startByte, sizeof(ChannelIn1::m_triggerIndex) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn1::m_triggerCount)], startByte, sizeof(ChannelIn1::m_triggerCount) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}
			}
			else if (PlcVersion::PlcData2 == m_plcVersion)
			{
				InspectionCommand2 insCmd;

				result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(insCmd.m_enable)], startByte, sizeof(insCmd.m_enable) };
				m_triggerDataOffset += result[configIndex].m_byteSize;
				//Do startByte always before configIndex
				startByte += result[configIndex].m_byteSize;
				configIndex++;

				result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(insCmd.m_socAbsSeconds)], startByte, sizeof(insCmd.m_socAbsSeconds) };
				m_triggerDataOffset += result[configIndex].m_byteSize;
				startByte += result[configIndex].m_byteSize;
				configIndex++;

				result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(insCmd.m_socAbsNanoseconds)], startByte, sizeof(insCmd.m_socAbsNanoseconds) };
				m_triggerDataOffset += result[configIndex].m_byteSize;
				startByte += result[configIndex].m_byteSize;
				configIndex++;

				result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(insCmd.m_socRelative)], startByte, sizeof(insCmd.m_socRelative) };
				m_triggerDataOffset += result[configIndex].m_byteSize;
				startByte += result[configIndex].m_byteSize;
				configIndex++;

				if (insCmd.m_reserved.size() > 0)
				{
					result[configIndex] = ConfigDataSet{ 0, dataTypeList[typeid(insCmd.m_reserved[0])], startByte, sizeof(insCmd.m_reserved) };
					m_triggerDataOffset += result[configIndex].m_byteSize;
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}

				for (int i = 0; i < insCmd.m_channels.size(); ++i)
				{
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_unitControl)], startByte, sizeof(ChannelIn2::m_unitControl) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_sequence)], startByte, sizeof(ChannelIn2::m_sequence) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_timeStamp1)], startByte, sizeof(ChannelIn2::m_timeStamp1) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_timeStamp2)], startByte, sizeof(ChannelIn2::m_timeStamp2) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_currentObjectType)], startByte, sizeof(ChannelIn2::m_currentObjectType) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_currentObjectID)], startByte, sizeof(ChannelIn2::m_currentObjectID) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_previousObjectType)], startByte, sizeof(ChannelIn2::m_previousObjectType) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_previousObjectID)], startByte, sizeof(ChannelIn2::m_previousObjectID) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_triggerIndex)], startByte, sizeof(ChannelIn2::m_triggerIndex) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_triggerCount)], startByte, sizeof(ChannelIn2::m_triggerCount) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelIn2::m_socTriggerTime)], startByte, sizeof(ChannelIn2::m_socTriggerTime) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}
			}
			break;
		}
		case TelegramLayout::Layout2:
		{
			if (PlcVersion::PlcData1 == m_plcVersion)
			{
				for (int i = 0; i < cNumberOfChannels; ++i)
				{
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelOut1::m_objectType)], startByte, sizeof(ChannelOut1::m_objectType) };
					//Do startByte always before configIndex
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelOut1::m_objectID)], startByte, sizeof(ChannelOut1::m_objectID) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelOut1::m_results[0])], startByte, sizeof(ChannelOut1::m_results) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelOut1::m_measurementValues[0])], startByte, sizeof(ChannelOut1::m_measurementValues) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}
			}
			else if (PlcVersion::PlcData2 == m_plcVersion)
			{
				InspectionState2 insState;
				if (insState.m_header.size() > 0)
				{
					result[configIndex] = ConfigDataSet{ 0, dataTypeList[typeid(insState.m_header[0])], startByte, sizeof(insState.m_header) };
					//Do startByte always before configIndex
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}
				for (int i = 0; i < cNumberOfChannels; ++i)
				{
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelOut2::m_objectType)], startByte, sizeof(ChannelOut2::m_objectType) };
					//Do startByte always before configIndex
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelOut2::m_objectID)], startByte, sizeof(ChannelOut2::m_objectID) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
					result[configIndex] = ConfigDataSet{ cModeSingleDirect, dataTypeList[typeid(ChannelOut2::m_results[0])], startByte, sizeof(ChannelOut2::m_results) };
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}
			}
			break;
		}

		default:
		{
			break;
		}
	}
	///This is the way the PLC sets the not used configuration sets
	while (configIndex < cConfigListSize)
	{
		result[configIndex] = ConfigDataSet{ 0, 0, startByte, 0ULL };
		configIndex++;
	}

	return result;	
}

void CifXCard::writeResponseData(const Telegram& rInputTelegram, const uint8_t* pSdoDynamic, size_t sdoDynamicSize)
{

	Telegram outputTelegram;
	///The content ID needs to be incremented for each send
	++m_contentID;
	///Content ID is not allowed to be 0
	if(0 == m_contentID)
	{
		++m_contentID;
	}
	outputTelegram.m_contentID = m_contentID;
	outputTelegram.m_referenceID =rInputTelegram.m_contentID;
	outputTelegram.m_type = TelegramType::Response;
	outputTelegram.m_content = rInputTelegram.m_content;
	outputTelegram.m_layout = rInputTelegram.m_layout;
	outputTelegram.m_systemStatus = m_ready ? SystemStatus::AppReady : SystemStatus::ComReady;

	{
		std::lock_guard<std::mutex> guard {m_cifxMutex};
		//Clear the write buffer
		memset(m_pWriteBuffer.get(), 0, m_maxPlcDataSize);
		//Copy the SDO Static data
		uint8_t* pData = m_pWriteBuffer.get();
		memcpy(pData, &outputTelegram, sizeof(Telegram));
		pData += sizeof(Telegram);

		//If nullptr or size 0 then no dynamic data
		if (nullptr != pSdoDynamic && 0 != sdoDynamicSize)
		{
			//Copy the SDO Dynamic data
			memcpy(pData, pSdoDynamic, sdoDynamicSize);
		}
	}

	if (CIFX_NO_ERROR != m_cifxLoadLib.m_pChannelIOWrite(m_hChannel, 0, 0, m_maxPlcDataSize, m_pWriteBuffer.get(), cTimeout))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CifxInitializationError, SvStl::SourceFileParams(StdMessageParams));
	}
}
} //namespace SvPlc
