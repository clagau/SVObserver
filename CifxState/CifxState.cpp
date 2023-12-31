//*****************************************************************************
/// \copyright COPYRIGHT (c) 2020,2020 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file CifxState.cpp
/// \brief Console main file to test the Cifx card
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <array>
#include <cifXErrors.h>
#include <cifXUser.h>
#include <Epl_Common_Defines.h>
#include <EplCn_If_Public.h>
#include <Hil_ApplicationCmd.h>
#include "Triggering/CifxLoadLibrary.h"
#include "SVPlcIO/Telegram.h"
#pragma endregion Includes

//This is to avoid CheckInclude false positives EPLCN_IF_QUEUE_NAME

constexpr uint32_t cCifxChannel = 0;
constexpr unsigned long cBuffSize = 255;
constexpr uint32_t cDriverTimeout = 200;
constexpr uint32_t cWaitTimeout = 500;
constexpr uint32_t cResetTimeout = 5000;
constexpr uint16_t cMaxHostTries = 50;
constexpr uint32_t cTimeout = 2;

constexpr const char* cBoardName = "CIFx0";
constexpr const char* cResult = "Result={}\n";
constexpr const char* cMissingMessageID = "Message ID {} missing\n";
constexpr const char* cCheckPlcC = "-CHECKPLCCOMS";
constexpr const char* cPlcIniFilePath = "C:\\SVObserver\\Bin\\SVPlcIO.ini";
constexpr const char* cSettingsGroup ="Settings";
constexpr const char* cPD0Version = "PD0Version";
constexpr const char* cPLCNodeID = "PLC-NodeID";

const uint32_t cCifXNodeId = 11; //The Powerlink Node Id used for the Hilscher CifX card value shall be 11-14 (SVIM1-SVIM4)
const uint32_t cMaxPLC_DataSize = 456; //The maximum size of the PLC-data in bytes Telegram = 20 Bytes Dynamic = 436 Bytes

/* Identity Information */
constexpr uint32_t cVendorId = 0x00000044UL;	///Hilscher Vendor ID
constexpr uint32_t cProductCode = 1UL;			///CIFX
constexpr uint32_t cSerialNumber = 0UL;			///Use serial number from SecMem or FDL
constexpr uint32_t cRevisionNumber = 0UL;

///This is a map which defines the PLC node ID depending on the SVIM computer name
static const std::map<std::string, uint32_t> cComputerNameNodeID =
{
	{std::string {"SVIM01"}, 11},
	{std::string {"SVIM02"}, 12},
	{std::string {"SVIM03"}, 13},
	{std::string {"SVIM04"}, 14},
	{std::string {"SVIM05"}, 21},
	{std::string {"SVIM06"}, 22},
	{std::string {"SVIM07"}, 23},
	{std::string {"SVIM08"}, 24},
	{std::string {"SVIM09"}, 31},
	{std::string {"SVIM10"}, 32},
	{std::string {"SVIM11"}, 33},
	{std::string {"SVIM12"}, 34},
};


SvTrig::CifxLoadLibrary g_cifxLoadLib;
CIFXHANDLE g_hDriver {nullptr};
CIFXHANDLE g_hChannel {nullptr};
uint32_t g_plcNodeID {cCifXNodeId};
uint32_t g_notificationType { 0UL };
std::atomic<int> g_notificationSync{ 0 };
uint8_t g_PlcBuffer[cMaxPLC_DataSize];
double g_lastTimestamp {0.0};
double g_intDeltaMin {0.0};
double g_intDeltaMax {0.0};

enum CifxMessage : int
{
	LoadLibrarySuccess = 1,
	OpenDriverSuccess,
	OpenChannelSuccess,
	CifxSystemStartSuccess,
	CifxDeviceReadySuccess,
	CifxSendConfigSuccess,
	CifxResetSuccess,
	CifxDeviceRunningSuccess,
	CifxRegisterAppSuccess,
	CifxSetTriggerTypeSuccess,
	CifxDMASuccess,
	CifxRegisterNotifySyncSuccess,
	CifxBusStateOnSuccess,
	CifxPlcDataVersion,
	CifxConfigData,
	CifxOperationData,
	CifxNotificationsReceived,
	CifxTestSuccess,
	LoadLibraryError = 1000,
	OpenDriverError,
	OpenChannelError,
	CifxSystemStartError,
	CifxDeviceReadyError,
	CifxSendConfigError,
	CifxResetError,
	CifxDeviceRunningError,
	CifxRegisterAppError,
	CifxSetTriggerTypeError,
	CifxDMAError,
	CifxRegisterNotifySyncError,
	CifxBusStateOnError,
};

static const std::map<CifxMessage, LPCTSTR> cErrorMessages
{
	{CifxMessage::LoadLibrarySuccess, "Successfully loaded cifX32DLL.dll with NodeID={}\n"},
	{CifxMessage::OpenDriverSuccess, "Driver version {} opened\n"},
	{CifxMessage::OpenChannelSuccess, "Channel opened succesfully\n"},
	{CifxMessage::CifxSystemStartSuccess, "Cifx card successfully started system with Firmware {}\n"},
	{CifxMessage::CifxDeviceReadySuccess, "Cifx card device ready\n"},
	{CifxMessage::CifxSendConfigSuccess, "Cifx successfully sent config\n"},
	{CifxMessage::CifxResetSuccess, "Cifx card successfully reset\n"},
	{CifxMessage::CifxDeviceRunningSuccess, "Cifx card device running\n"},
	{CifxMessage::CifxRegisterAppSuccess, "Cifx successfully registered app\n"},
	{CifxMessage::CifxSetTriggerTypeSuccess, "Cifx set trigger type successfully \n"},
	{CifxMessage::CifxDMASuccess, "DMA successfully set\n"},
	{CifxMessage::CifxRegisterNotifySyncSuccess, "Sync notify successfully registered\n"},
	{CifxMessage::CifxBusStateOnSuccess, "Bus state on successfully set\n"},
	{CifxMessage::CifxPlcDataVersion, "PLC Data Version {}\n"},
	{CifxMessage::CifxConfigData, "PLC Config Layout {}\n"},
	{CifxMessage::CifxOperationData, "PLC Operation Data\n"},
	{CifxMessage::CifxNotificationsReceived, "Notifications received {}\n"},
	{CifxMessage::CifxTestSuccess, "Cfix test successful\n"},
	{CifxMessage::LoadLibraryError, "Could not load cifX32DLL.dll [{:#x}]\n"},
	{CifxMessage::OpenDriverError, "Driver could not be opened [{:#x}]\n"},
	{CifxMessage::OpenChannelError, "Channel could not be opened [{:#x}]\n"},
	{CifxMessage::CifxSystemStartError, "Cifx card could not start system [{:#x}]\n"},
	{CifxMessage::CifxDeviceReadyError, "Cifx card device not ready error [{:#x}]\n"},
	{CifxMessage::CifxSendConfigError, "Cifx could not send config [{:#x}]\n"},
	{CifxMessage::CifxResetError, "Cifx card could not be reset [{:#x}]\n"},
	{CifxMessage::CifxDeviceRunningError, "Cifx card device not running error [{:#x}]\n"},
	{CifxMessage::CifxRegisterAppError, "Cifx could not register app [{:#x}]\n"},
	{CifxMessage::CifxSetTriggerTypeError, "Cifx could not set trigger type [{:#x}]\n"},
	{CifxMessage::CifxDMAError, "DMA could not be set [{:#x}]\n"},
	{CifxMessage::CifxRegisterNotifySyncError, "Sync Notify could not be registered [{:#x}]\n"},
	{CifxMessage::CifxBusStateOnError, "Bus state on could not be set [{:#x}]\n"},
};

double GetTimeStamp()
{
	return static_cast<double> (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()) / 1000000.0;
}

void printMessage(CifxMessage messageID, int32_t result = 0, LPCTSTR pText = nullptr)
{
	const auto iter = cErrorMessages.find(messageID);
	if(cErrorMessages.end() != iter)
	{
		if(nullptr == pText)
		{
			//result can only be not 0 when pText is nullptr
			if(0 != result)
			{
				std::cout << std::vformat(iter->second, std::make_format_args(result));
			}
			else
			{
				std::cout << iter->second;
			}
		}
		else
		{	
			std::cout << std::vformat(iter->second, std::make_format_args(pText));
		}
	}
	else
	{
		std::cout << std::format(cMissingMessageID, static_cast<int>(messageID));
	}
}

void APIENTRY notificationHandler(uint32_t notification, uint32_t , void* , void* )
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	if (g_notificationType == notification)
	{
		static double timeStampPrev {0.0};
		static SvPlc::TelegramLayout layout {SvPlc::TelegramLayout::NoneLayout};
		double timeStamp = GetTimeStamp();
		int32_t result = g_cifxLoadLib.m_pChannelIORead(g_hChannel, 0, 0, cMaxPLC_DataSize, g_PlcBuffer, cTimeout);

		if (CIFX_NO_ERROR == result)
		{
			static uint16_t contentID {0};
			static bool operationData {false};

			std::string outputText {std::to_string(timeStamp) + ';' + std::to_string(timeStampPrev) + '\n'};
			::OutputDebugString(outputText.c_str());
			SvPlc::Telegram& rTelegram = reinterpret_cast<SvPlc::Telegram&> (g_PlcBuffer);

			switch (rTelegram.m_content)
			{
				case SvPlc::TelegramContent::VersionData:
				{
					static std::string version {};
					std::string reqVersion{std::to_string(g_PlcBuffer[20]) + '.' + std::to_string(g_PlcBuffer[21])};
					if (version != reqVersion)
					{
						version = reqVersion;
						printMessage(CifxMessage::CifxPlcDataVersion, 0, version.c_str());
				}
					layout = SvPlc::TelegramLayout::NoneLayout;
					operationData = false;
					break;
				}
				case SvPlc::TelegramContent::ConfigurationData:
				{
					if (layout != rTelegram.m_layout)
					{
						layout = rTelegram.m_layout;
						printMessage(CifxMessage::CifxConfigData, 0, std::to_string(layout).c_str());
					}
					break;
				}
				case SvPlc::TelegramContent::OperationData:
				{
					if (false == operationData)
					{
						operationData = true;
						printMessage(CifxMessage::CifxOperationData);
					}
					memset(g_PlcBuffer + sizeof(SvPlc::Telegram), 0, cMaxPLC_DataSize - sizeof(SvPlc::Telegram));
					break;
				}
			}

			if (g_lastTimestamp > 0.0)
			{
				double diff = timeStamp - g_lastTimestamp;
				if (g_intDeltaMax < diff && diff < 10.0)
				{
					g_intDeltaMax = diff;
				}
				if (g_intDeltaMin == 0.0 || g_intDeltaMin > diff)
				{
					g_intDeltaMin = diff;
				}
			}
			g_lastTimestamp = timeStamp;

			++contentID;
			if (0 == contentID)
			{
				++contentID;
			}
			rTelegram.m_referenceID = rTelegram.m_contentID;
			rTelegram.m_contentID = contentID;
			rTelegram.m_type = SvPlc::TelegramType::Response;
			rTelegram.m_systemStatus = SvPlc::SystemStatus::AppReady;

			g_cifxLoadLib.m_pChannelIOWrite(g_hChannel, 0, 0, cMaxPLC_DataSize, g_PlcBuffer, cTimeout);
		}
		timeStampPrev = timeStamp;

		g_notificationSync++;
		if (CIFX_NOTIFY_SYNC == g_notificationType)
		{
			uint32_t errorSyncCount{ 0 };
			g_cifxLoadLib.m_pChannelSyncState(g_hChannel, CIFX_SYNC_SIGNAL_CMD, cDriverTimeout, &errorSyncCount);
		}
	}
}

uint32_t SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt)
{
	if (nullptr == pSendPkt || nullptr == pRecvPkt)
	{
		return static_cast<uint32_t> (E_POINTER);
	}
	/* fire the packet */
	uint32_t result = g_cifxLoadLib.m_pChannelPutPacket(g_hChannel, pSendPkt, cDriverTimeout);
	if (CIFX_NO_ERROR != result)
	{
		return result;
	}

	/* ok, at this point we have successfully sent a packet */

	/* check for packets to receive */
	/* the ulCnfCmd is always: ulReqCmd | 0x01 */
	uint32_t ulCnfCmd = pSendPkt->tHeader.ulCmd | 0x01;

	while (CIFX_NO_ERROR == (result = g_cifxLoadLib.m_pChannelGetPacket(g_hChannel, sizeof(*pRecvPkt), pRecvPkt, cDriverTimeout)))
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

int32_t SendRecvCmdPkt(uint32_t command)
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

void closeCifX()
{
	if(nullptr != g_hChannel)
	{
		g_cifxLoadLib.m_pChannelBusState(g_hChannel, CIFX_BUS_STATE_OFF, nullptr, cDriverTimeout);
		g_cifxLoadLib.m_pChannelUnregisterNotification(g_hChannel, g_notificationType);
		SendRecvCmdPkt(HIL_UNREGISTER_APP_REQ);
		g_cifxLoadLib.m_pChannelClose(g_hChannel);
		g_hChannel = nullptr;
	}

	if(nullptr != g_hDriver)
	{
		g_cifxLoadLib.m_pDriverClose(g_hDriver);
		g_hDriver = nullptr;
	}
}


int processError(CifxMessage messageID, int32_t result)
{
	closeCifX();
	printMessage(messageID, result);

	std::cout << std::format(cResult, static_cast<int>(messageID));
	return messageID;
}

void BuildConfigurationReq(CIFX_PACKET* pPacket, uint8_t NodeId, uint16_t DataLength)
{
	EPLCN_IF_SET_CONFIG_REQ_T* pConfigReq = reinterpret_cast<EPLCN_IF_SET_CONFIG_REQ_T*> (pPacket);

	pConfigReq->tHead.ulDest = 0x20;						//Destination of packet
	pConfigReq->tHead.ulSrc = 0x10;							//Source of packet, process queue not always necessary
	pConfigReq->tHead.ulLen = sizeof(pConfigReq->tData);	//Length of packet data without header
	pConfigReq->tHead.ulCmd = EPLCN_IF_SET_CONFIG_REQ;
	pConfigReq->tHead.ulSta = 0;							//Status code of operation
	pConfigReq->tHead.ulExt = 0;							//extension

	uint32_t stackCfgFlags = (CIFX_NOTIFY_SYNC == g_notificationType) ? MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_USE_CUSTOM_PDO_OBJ : MSK_EPLCN_IF_CFG_STACK_CFG_FLAGS_DISABLE_HOST_TRIGGERED_PREQ_XCHG;
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

int32_t SendConfigurationToCifX()
{
	/* common variables for packets */
	CIFX_PACKET sendPkt;
	CIFX_PACKET recvPkt;

	memset(&sendPkt, 0, sizeof(sendPkt));
	memset(&recvPkt, 0, sizeof(recvPkt));

	/* build configuration packet */
	BuildConfigurationReq(&sendPkt, static_cast<uint8_t> (g_plcNodeID), static_cast<uint16_t> (cMaxPLC_DataSize));

	return SendRecvPkt(&sendPkt, &recvPkt);
}

int main(int argc, char* args[])
{
	bool isPLcConnected{false};

	if (argc >= 2)
	{
		std::string cmdOption{args[1]};
		std::transform(cmdOption.begin(), cmdOption.end(), cmdOption.begin(),
					   [](unsigned char c) { return static_cast<char> (std::toupper(c)); }
		);

		if(cmdOption == cCheckPlcC)
		{
			isPLcConnected = true;
		}
		else
		{
			std::cout << "CifxState -CheckPlcComs -h\n";
			std::cout << "Calling the program without options shall load the Hilscher driver and open it (Hilscher card must be present) then open Channel 0\n";
			std::cout << "-CheckPlcComs => this option shall additionally test the communication with the PLC\n";
			std::cout << "-h => this option shows the help\n";
			std::cout << "\n\nThe possible return values for CifxState are as follows:\n0\t= Successful\n";
			CifxMessage messageID{CifxMessage::LoadLibraryError};

			
			std::map<CifxMessage, LPCTSTR>::const_iterator iter = cErrorMessages.find(messageID);
			while(cErrorMessages.end() != iter)
			{
				std::string text(iter->second);
				size_t pos = text.find("[0x%x]");
				if(std::string::npos != pos)
				{
					text = text.substr(0, pos - 1);
					text += '\n';
				}
				std::cout << std::format("{}\t= {}", static_cast<int>(iter->first), text.c_str());
				messageID = static_cast<CifxMessage> (messageID + 1);
				iter = cErrorMessages.find(messageID);
			}

			return -1;
		}
	}

	memset(g_PlcBuffer, 0, cMaxPLC_DataSize);

	DWORD size {cBuffSize};
	TCHAR buffer[cBuffSize];
	memset(buffer, 0, cBuffSize);
	if (::GetComputerName(buffer, &size))
	{
		std::string computerName {buffer};
		auto iter = cComputerNameNodeID.find(computerName);
		if (cComputerNameNodeID.end() != iter)
		{
			g_plcNodeID = iter->second;
		}
	}
	memset(buffer, 0, cBuffSize);
	::GetPrivateProfileString(cSettingsGroup, cPD0Version, "3.4.0.2", buffer, cBuffSize, cPlcIniFilePath);
	std::string Pd0Version = buffer;
	g_plcNodeID = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cPLCNodeID, g_plcNodeID, cPlcIniFilePath));

	::SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	if (S_OK == g_cifxLoadLib.Open())
	{
		printMessage(CifxMessage::LoadLibrarySuccess, 0, std::to_string(g_plcNodeID).c_str());
	}
	else
	{
		return processError(CifxMessage::LoadLibraryError, 0);
	}

	int32_t result = g_cifxLoadLib.m_pDriverOpen(&g_hDriver);
	if (CIFX_NO_ERROR == result)
	{
		DRIVER_INFORMATION driverInfo;
		memset(&driverInfo, 0, sizeof(driverInfo));
		g_cifxLoadLib.m_pDriverGetInformation(g_hDriver, sizeof(driverInfo), reinterpret_cast<void*> (&driverInfo));
		printMessage(CifxMessage::OpenDriverSuccess, 0, driverInfo.abDriverVersion);
	}
	else
	{
		return processError(CifxMessage::OpenDriverError, result);
	}
	result = g_cifxLoadLib.m_pChannelOpen(g_hDriver, const_cast<char*> (cBoardName), cCifxChannel, &g_hChannel);
	if(CIFX_NO_ERROR == result)
	{
		printMessage(CifxMessage::OpenChannelSuccess);
	}
	else
	{
		return processError(CifxMessage::OpenChannelError, result);
	}

	result = g_cifxLoadLib.m_pChannelReset(g_hChannel, CIFX_CHANNELINIT, cResetTimeout);
	if (CIFX_NO_ERROR == result)
	{
		printMessage(CifxMessage::CifxResetSuccess);
	}
	else
	{
		return processError(CifxMessage::CifxResetError, result);
	}

	result = g_cifxLoadLib.m_pChannelReset(g_hChannel, CIFX_SYSTEMSTART, cResetTimeout);
	//Wait a while for the system start to 
	Sleep(cResetTimeout);
	if (CIFX_NO_ERROR == result)
	{
		CHANNEL_INFORMATION channelInfo;
		g_cifxLoadLib.m_pChannelInfo(g_hChannel, sizeof(channelInfo), reinterpret_cast<void*> (&channelInfo));
		std::string outputString(reinterpret_cast<char*> (channelInfo.abFWName));
		std::string firmware;
		firmware = std::to_string(channelInfo.usFWMajor) + '.' + std::to_string(channelInfo.usFWMinor) + '.' + std::to_string(channelInfo.usFWBuild) + '.' + std::to_string(channelInfo.usFWRevision);
		g_notificationType = (Pd0Version == firmware) ? CIFX_NOTIFY_PD0_IN : CIFX_NOTIFY_SYNC;
		outputString += ' ' + firmware;
		printMessage(CifxMessage::CifxSystemStartSuccess, 0, outputString.c_str());
	}
	else
	{
		return processError(CifxMessage::CifxSystemStartError, result);
	}

	if(isPLcConnected)
	{
		int hostTries {1};
		uint32_t  dummy {0UL};
		do
		{
			result = g_cifxLoadLib.m_pChannelHostState(g_hChannel, CIFX_HOST_STATE_READY, &dummy, cDriverTimeout);

			if (CIFX_DEV_NOT_READY == result)
			{
				///Sleep and try again
				Sleep(cWaitTimeout);
				++hostTries;
			}
		} while (CIFX_DEV_NOT_READY == result && hostTries < cMaxHostTries);
		if (CIFX_DEV_NOT_READY == result)
		{
			return processError(CifxMessage::CifxDeviceReadyError, result);
		}
		else
		{
			printMessage(CifxMessage::CifxDeviceReadySuccess);
		}

		result = SendConfigurationToCifX();
		if (CIFX_NO_ERROR == result)
		{
			printMessage(CifxMessage::CifxSendConfigSuccess);
		}
		else
		{
			return processError(CifxMessage::CifxSendConfigError, result);
		}

		result = g_cifxLoadLib.m_pChannelReset(g_hChannel, CIFX_CHANNELINIT, cResetTimeout);
		if (CIFX_NO_ERROR == result)
		{
			printMessage(CifxMessage::CifxResetSuccess);
		}
		else
		{
			return processError(CifxMessage::CifxResetError, result);
		}

		hostTries = 1;

		do
		{
			result = g_cifxLoadLib.m_pChannelHostState(g_hChannel, CIFX_HOST_STATE_READY, &dummy, cDriverTimeout);
			if (CIFX_DEV_NOT_RUNNING == result)
			{
				///Sleep and try again
				Sleep(cWaitTimeout);
				++hostTries;
			}
		} while (CIFX_DEV_NOT_RUNNING == result && hostTries < cMaxHostTries);
		if (CIFX_DEV_NOT_RUNNING == result)
		{
			return processError(CifxMessage::CifxDeviceRunningError, result);
		}
		else
		{
			printMessage(CifxMessage::CifxDeviceRunningSuccess);
		}

		result = SendRecvCmdPkt(HIL_REGISTER_APP_REQ);
		if (CIFX_NO_ERROR == result)
		{
			printMessage(CifxMessage::CifxRegisterAppSuccess);
		}
		else
		{
			return processError(CifxMessage::CifxRegisterAppError, result);
		}

		if (CIFX_NOTIFY_SYNC == g_notificationType)
		{
			result = SendRecvCmdPkt(HIL_SET_TRIGGER_TYPE_REQ);
			if (CIFX_NO_ERROR == result)
			{
				printMessage(CifxMessage::CifxSetTriggerTypeSuccess);
			}
			else
			{
				return processError(CifxMessage::CifxSetTriggerTypeError, result);
			}
		}

		uint32_t State {0UL};
		result = g_cifxLoadLib.m_pChannelDMAState(g_hChannel, CIFX_DMA_STATE_ON, &State);
		if (CIFX_NO_ERROR == result)
		{
			printMessage(CifxMessage::CifxDMASuccess);
		}
		else
		{
			return processError(CifxMessage::CifxDMAError, result);
		}

		result = g_cifxLoadLib.m_pChannelRegisterNotification(g_hChannel, g_notificationType, notificationHandler, nullptr);
		if (CIFX_NO_ERROR == result)
		{
			printMessage(CifxMessage::CifxRegisterNotifySyncSuccess);
		}
		else
		{
			return processError(CifxMessage::CifxRegisterNotifySyncError, result);
		}

		///Sleep for 10 seconds to check if interrupts are received
		Sleep(cResetTimeout * 2);
		std::string notifications;
		notifications = "Notifications=" + std::to_string(g_notificationSync) + "\nInterrupt Delta Min = " + std::to_string(g_intDeltaMin) + " Interrupt Delta Max = " + std::to_string(g_intDeltaMax);
		printMessage(CifxMessage::CifxNotificationsReceived, 0, notifications.c_str());
	}

	closeCifX();
	printMessage(CifxMessage::CifxTestSuccess);
	std::cout << std::format(cResult, 0);
	return 0;
}
