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
#include "Epl_Common_Defines.h"
#include "TLR_Types.h"			///This needs to be before EplCn_If_Public!
#include "EplCn_If_Public.h"
#include "InspectionState.h"
#include "PlcDataVersion.h"
#include "TLR_Results.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

namespace SvPlc
{
constexpr uint32_t cCifxChannel = 0;
constexpr uint32_t cDriverTimeout = 100;
constexpr uint32_t cWaitTimeout = 500;
constexpr uint32_t cResetTimeout = 2000;
constexpr uint32_t cTimeout = 2;

constexpr uint16_t cMaxHostTries = 10;
constexpr char* cBoardName = "CIFx0";
/* Identity Information */
constexpr uint32_t cVendorId = 0x00000044UL;	///Hilscher Vendor ID
constexpr uint32_t cProductCode = 1UL;			///CIFX
constexpr uint32_t cSerialNumber = 0UL;			///Use serial number from SecMem or FDL
constexpr uint32_t cRevisionNumber = 0UL;

constexpr uint32_t cRegisterAppReq = 0x00002F10;
constexpr uint32_t cUnRegisterAppReq = 0x00002F12;


void APIENTRY interruptHandler(uint32_t notification, uint32_t ulDataLen, void* pvData, void* pvUser)
{
	if (pvUser == nullptr)
	{
		printOutput("'cifX' == nullptr\n");
		return;
	}

	CifXCard* pCifX = reinterpret_cast<CifXCard*> (pvUser);

	if(CIFX_NOTIFY_PD0_IN == notification)
	{
		pCifX->readProcessData();
	}
}

CifXCard::CifXCard(uint32_t CifXNodeId, uint32_t MaxPlcDataSize):
	m_CifXNodeId(CifXNodeId), m_maxPlcDataSize(MaxPlcDataSize), m_currentResult(CIFX_NO_ERROR)
{
}


int32_t CifXCard::OpenCifX()
{
	int32_t result{CIFX_DEV_NOT_READY};

	result = m_cifxLoadLib.Open();
	/// Load Dll failed!
	if(S_OK != result)
	{
		printOutput("Could not load cifX32DLL.dll\n");
		return result;
	}
	printOutput("Opening driver...\n");
	/* Open Driver */
	result = m_cifxLoadLib.m_pDriverOpen(&m_hDriver);
	if (CIFX_NO_ERROR != result)
	{
		return result;
	}
	DRIVER_INFORMATION driverInfo;
	memset(&driverInfo, 0, sizeof(driverInfo));
	m_cifxLoadLib.m_pDriverGetInformation(m_hDriver, sizeof(driverInfo), reinterpret_cast<void*> (&driverInfo));
	std::stringstream outputStream;
	outputStream << "Driver version " << driverInfo.abDriverVersion << '\n';
	printOutput(outputStream.str().c_str());

	printOutput("Opening channel ...\n");
	result = m_cifxLoadLib.m_pChannelOpen(m_hDriver, cBoardName, cCifxChannel, &m_hChannel);
	if (CIFX_NO_ERROR != result)
	{
		return result;
	}
	CHANNEL_INFORMATION channelInfo;
	m_cifxLoadLib.m_pChannelInfo(m_hChannel, sizeof(channelInfo), reinterpret_cast<void*> (&channelInfo));
	outputStream.clear();
	outputStream << "Firmware " << channelInfo.usFWMajor << '.' << channelInfo.usFWMinor << '\n';
	printOutput(outputStream.str().c_str());

	printOutput("Processing system restart...\n");
	result = m_cifxLoadLib.m_pChannelReset(m_hChannel, CIFX_SYSTEMSTART, cResetTimeout);

	if (CIFX_NO_ERROR == result)
	{
		int hostTries{1};
		uint32_t  dummy{0UL};
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
	}

	if (CIFX_NO_ERROR == result)
	{
		printOutput("Create config lists\n");
		m_configDataSetsMap[TelegramLayout::Layout1] = createConfigList(TelegramLayout::Layout1);
		m_configDataSetsMap[TelegramLayout::Layout2] = createConfigList(TelegramLayout::Layout2);
	}

	return result;
}

int32_t CifXCard::SendConfigurationToCifX()
{
	/* common variables for packets */
	CIFX_PACKET sendPkt;
	CIFX_PACKET recvPkt;

	memset(&sendPkt, 0, sizeof(sendPkt));
	memset(&recvPkt, 0, sizeof(recvPkt));

	/* build configuration packet */
	BuildConfigurationReq(&sendPkt, m_CifXNodeId, m_maxPlcDataSize);

	/* send configuration Req packet */
	printOutput("Sending configuration request...\n");
	uint32_t result = SendRecvPkt(&sendPkt, &recvPkt);
	/* check if we got an error within configuration packet */
	if (CIFX_NO_ERROR != result)
	{
		printOutput("Configuration confirmation not received.");
		return result;

	}
	if (CIFX_NO_ERROR != recvPkt.tHeader.ulState)
	{
		std::stringstream outputStream;
		outputStream << "Configuration packet returned with error code: 0x" << std::hex << recvPkt.tHeader.ulState << '\n';
		printOutput(outputStream.str().c_str());
		return result;
	}

	return result;
}


int32_t CifXCard::WarmstartAndInitializeCifX()
{
	if(false == m_cifxLoadLib.isInitilized())
	{
		return E_FAIL;
	}
	printOutput("Processing channel init...\n");

	uint32_t result = m_cifxLoadLib.m_pChannelReset(m_hChannel, CIFX_CHANNELINIT, cResetTimeout);

	if (CIFX_NO_ERROR != result)
	{
		return result;
	}
	printOutput("Waiting for Warmstart");
	uint32_t  dummy{0UL};

	do
	{
		result = m_cifxLoadLib.m_pChannelHostState(m_hChannel, CIFX_HOST_STATE_READY, &dummy, cDriverTimeout);
		printOutput(".");
	} while (CIFX_DEV_NOT_RUNNING == result);
	printOutput(" -> complete.\n");


	/* check CifX state */
	if ((CIFX_NO_ERROR == result) || (CIFX_DEV_NO_COM_FLAG == result))
	{
		if (CIFX_DEV_NO_COM_FLAG == result)
		{
			printOutput("\tNo communication!");
		}

		printOutput("Initialize read and write buffers\n");
		m_pReadBuffer = std::make_unique<uint8_t[]>(m_maxPlcDataSize);
		m_pWriteBuffer = std::make_unique<uint8_t[]>(m_maxPlcDataSize);

		/* initialize the read and write buffer with zero */
		memset(m_pReadBuffer.get(), 0, m_maxPlcDataSize);
		memset(m_pWriteBuffer.get(), 0, m_maxPlcDataSize);

		result = SendRecvEmptyPkt(cRegisterAppReq);
		if(CIFX_NO_ERROR != result)
		{
			return result;
		}

		/* variable for host state */
		uint32_t  HostState{0UL};
		/* bus on */
		result = m_cifxLoadLib.m_pChannelBusState(m_hChannel, CIFX_BUS_STATE_ON, &HostState, cDriverTimeout);
		/* bus activity begins here */
		if (CIFX_BUS_STATE_ON != HostState)
		{
			return result;
		}
		printOutput("Bus state is on\n");
	}

	uint32_t State{0UL};
	result = m_cifxLoadLib.m_pChannelDMAState(m_hChannel, CIFX_DMA_STATE_ON, &State);
	if (CIFX_NO_ERROR != result)
	{
		return result;
	}
	printOutput("DMA is on\n");

	result = m_cifxLoadLib.m_pChannelRegisterNotification(m_hChannel, CIFX_NOTIFY_PD0_IN, interruptHandler, reinterpret_cast<void*> (this));
	if (CIFX_NO_ERROR != result)
	{
		return result;
	}
	printOutput("Cifx Interrupt Handler Ready\n");

	return result;
}

void CifXCard::readProcessData()
{
	if (false == m_cifxLoadLib.isInitilized())
	{
		return;
	}

	m_TelegramReceiveTime = SvTl::GetTimeStamp();
	m_processDataReadCount++;

	m_currentResult = m_cifxLoadLib.m_pChannelIORead(m_hChannel, 0, 0, m_maxPlcDataSize, m_pReadBuffer.get(), 2);

	if (m_currentResult != CIFX_NO_ERROR)
	{
		if (CIFX_DEV_NO_COM_FLAG != m_currentResult) // "COM-flag not set" occurs often: do not report
		{
			printOutput("ReadProcessData() failed\n");
		}
		return;
	}
	
	//Reduce lock guard scope
	{
		std::lock_guard<std::mutex> guard {m_cifxMutex};
		uint8_t* pData = m_pReadBuffer.get();
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

void CifXCard::closeCifX()
{
	if(m_cifxLoadLib.isInitilized())
	{
		printOutput("Closing application and freeing buffers...\n");
		m_cifxLoadLib.m_pChannelUnregisterNotification(m_hChannel, CIFX_NOTIFY_PD0_IN);
		m_cifxLoadLib.m_pChannelUnregisterNotification(m_hChannel, CIFX_NOTIFY_SYNC);
		m_cifxLoadLib.m_pChannelBusState(m_hChannel, CIFX_BUS_STATE_OFF, nullptr, cDriverTimeout);
		SendRecvEmptyPkt(cUnRegisterAppReq);
		printOutput("Closing Channel...\n");
		m_cifxLoadLib.m_pChannelClose(m_hChannel);
		printOutput("Closing Driver...\n");
		m_cifxLoadLib.m_pDriverClose(m_hDriver);
		printOutput("CifX card closed.\n");
	}
}


HRESULT CifXCard::OpenAndInitializeCifX()
{
	HRESULT result = OpenCifX();

	if (CIFX_NO_ERROR == result)
	{
		result = SendConfigurationToCifX();
		
		if (!(CIFX_NO_ERROR != result))
		{
			result = WarmstartAndInitializeCifX();
		}
	}
	return result;
}

void CifXCard::sendVersion()
{
	m_protocolInitialized = false;
	PlcDataVersion plcVersion;
	plcVersion.m_major = cMajorVersion;
	plcVersion.m_minor = cMinorVersion;
	const uint8_t* pData = reinterpret_cast<const uint8_t*> (&plcVersion);
	writeResponseData(pData, sizeof(PlcDataVersion));
	printOutput("Send Version\n");
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
		printOutput("Send Config Layout\n");
	}
}

void CifXCard::sendOperationData(const InspectionState& rState)
{
	m_protocolInitialized = true;
	const uint8_t* pData = reinterpret_cast<const uint8_t*> (&rState);
	writeResponseData(pData, sizeof(InspectionState));
}

void CifXCard::setReady(bool ready)
{
	std::lock_guard<std::mutex> guard{m_cifxMutex};
	m_ready = ready;
}

/*************************************************************************************************
* @brief This function sends a packet and waits for the confirmation.
* Packets which will come up meanwhile will be passed to the standard packet handler.
* @param pSendPkt Packet which will be sent.
* @param pRecvPkt Packet which the function has received.
* @return uint32_t Returns CIFX_NO_ERROR if no error, otherwise it will return an error code.
*/
uint32_t CifXCard::SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt)
{
	if(nullptr == pSendPkt || nullptr == pRecvPkt)
	{
		return E_POINTER;
	}
	/* fire the packet */
	uint32_t result = m_cifxLoadLib.m_pChannelPutPacket(m_hChannel, pSendPkt, cDriverTimeout);
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


/*************************************************************************************************
* @brief This function sends and receives an empty packet.
* @param ulCmd Command code which will be inserted into packet header.
* @return TLR_RESULT Returns CIFX_NO_ERROR if no error, otherwise it will return an error code.
*
*/
uint32_t CifXCard::SendRecvEmptyPkt(uint32_t command)
{

	CIFX_PACKET sendPkt;
	CIFX_PACKET recvPkt;

	///Need to initialize CIFX_PACKET manually!
	memset(&sendPkt, 0, sizeof(sendPkt));
	memset(&recvPkt, 0, sizeof(recvPkt));

	TLR_EMPTY_PACKET_T* pEmptyPacket = reinterpret_cast<TLR_EMPTY_PACKET_T*> (&sendPkt);

	/* ulSrc and ulDest are set by GetPacket / SendPacket */
	pEmptyPacket->tHead.ulCmd = command;
	pEmptyPacket->tHead.ulDest = 0x20;
	pEmptyPacket->tHead.ulDestId = 0;
	pEmptyPacket->tHead.ulExt = 0;
	pEmptyPacket->tHead.ulId = 0;
	pEmptyPacket->tHead.ulLen = 0;
	pEmptyPacket->tHead.ulRout = 0;
	pEmptyPacket->tHead.ulSrc = 0;
	pEmptyPacket->tHead.ulSrcId = 0;
	pEmptyPacket->tHead.ulSta = 0;

	return SendRecvPkt(&sendPkt, &recvPkt);
}

/*************************************************************************************************
* @brief This method builds a configuration request packet.
* @param CIFX_PACKET* pPacket pointer to a CIFX_PACKET structure.
*************************************************************************************************/
void CifXCard::BuildConfigurationReq(CIFX_PACKET* pPacket, uint8_t NodeId, uint16_t DataLength)
{
	EPLCN_IF_SET_CONFIG_REQ_T* pConfigReq = reinterpret_cast<EPLCN_IF_SET_CONFIG_REQ_T*> (pPacket);

	pConfigReq->tHead.ulDest = 0x20;						//Destination of packet
	pConfigReq->tHead.ulSrc = 0x10;							//Source of packet, process queue not always necessary
	pConfigReq->tHead.ulLen = sizeof(pConfigReq->tData);	//Length of packet data without header
	pConfigReq->tHead.ulCmd = EPLCN_IF_SET_CONFIG_REQ;
	pConfigReq->tHead.ulSta = 0;							//Status code of operation
	pConfigReq->tHead.ulExt = 0;							//extension

	pConfigReq->tData.ulSystemFlags = 0;                      //System Flags  
	pConfigReq->tData.ulWatchdogTime = 1000;					//Watchdog time
	/* Stack creates default PDO objects */
	//MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_NMT_TRIGGERED_BY_APP
	//Arvid: MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_HOST_TRIGGERED_PREQ_XCHG: Damit wird die Aktualisierung des DualPort-Memorys von netX angestoßen,
	//Arvid: Lesevorgänge mit xChannelIORead werden dann immer bei ankommenden Telegrammen abgeschlossen.
	//Arvid: Synchronisation ist damit möglich, z.B. direkt nach dem abgeschlossenen Lesevorgang, aber auch mit CIFX_NOTIFY_PD0_IN - Callback.*/
	/* Configuration Flags                      */
	pConfigReq->tData.ulStackCfgFlags = MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_HOST_TRIGGERED_PREQ_XCHG;
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
	pConfigReq->tData.bNodeId = NodeId;						//EPL node id (range 1 to 239)
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
			InspectionCommand insCmd;

			result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(insCmd.m_enable)], startByte, sizeof(insCmd.m_enable)};
			//Do startByte always before configIndex
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(insCmd.m_socAbsSeconds)], startByte, sizeof(insCmd.m_socAbsSeconds)};
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(insCmd.m_socAbsNanoseconds)], startByte, sizeof(insCmd.m_socAbsNanoseconds)};
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(insCmd.m_socRelative)], startByte, sizeof(insCmd.m_socRelative)};
			startByte += result[configIndex].m_byteSize;
			configIndex++;

			if(insCmd.m_reserved.size() > 0)
			{
				result[configIndex] = ConfigDataSet {0, dataTypeList[typeid(insCmd.m_reserved[0])], startByte, sizeof(insCmd.m_reserved)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
			}

			for(const auto& rChannel : insCmd.m_channels)
			{
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_unitControl)], startByte, sizeof(rChannel.m_unitControl)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_sequence)], startByte, sizeof(rChannel.m_sequence)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_timeStamp1)], startByte, sizeof(rChannel.m_timeStamp1)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_timeStamp2)], startByte, sizeof(rChannel.m_timeStamp2)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectType)], startByte, sizeof(rChannel.m_currentObjectType)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectID)], startByte, sizeof(rChannel.m_currentObjectID)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_previousObjectType)], startByte, sizeof(rChannel.m_previousObjectType)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_previousObjectID)], startByte, sizeof(rChannel.m_previousObjectID)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_triggerIndex)], startByte, sizeof(rChannel.m_triggerIndex)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_triggerCount)], startByte, sizeof(rChannel.m_triggerCount)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_socTriggerTime)], startByte, sizeof(rChannel.m_socTriggerTime)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
			}
			///This is the way the PLC sets the not used configuration sets
			while(configIndex < cConfigListSize)
			{
				result[configIndex] = ConfigDataSet {0, 0, startByte, 0ULL};
				configIndex++;
			}
			break;
		}
		case TelegramLayout::Layout2:
		{
			InspectionState insState;
			
			if (insState.m_header.size() > 0)
			{
				result[configIndex] = ConfigDataSet {0, dataTypeList[typeid(insState.m_header[0])], startByte, sizeof(insState.m_header)};
				//Do startByte always before configIndex
				startByte += result[configIndex].m_byteSize;
				configIndex++;
			}

			for (const auto& rChannel : insState.m_channels)
			{
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectType)], startByte, sizeof(rChannel.m_currentObjectType)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_currentObjectID)], startByte, sizeof(rChannel.m_currentObjectID)};
				startByte += result[configIndex].m_byteSize;
				configIndex++;
				if (rChannel.m_results.size() > 0)
				{
					result[configIndex] = ConfigDataSet {cModeSingleDirect, dataTypeList[typeid(rChannel.m_results[0])], startByte, sizeof(rChannel.m_results)};
					startByte += result[configIndex].m_byteSize;
					configIndex++;
				}
			}
			///This is the way the PLC sets the not used configuration sets
			while (configIndex < cConfigListSize)
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
	m_currentResult = m_cifxLoadLib.m_pChannelIOWrite(m_hChannel, 0, 0, m_maxPlcDataSize, m_pWriteBuffer.get(), cTimeout);

	if (CIFX_NO_ERROR != m_currentResult)
	{
		printOutput("Write Response Data Failed\n");
	}

}
} //namespace SvPlc
