//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file CifXCardEcat.cpp
/// \brief Settings and interface to the CifX card as EtherCat Master
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include <cifXErrors.h>
#include <cifXUser.h>
#include <Hil_ApplicationCmd.h>
#include <TLR_Types.h>

#include "CifXCardEcat.h"
#include "EtherCatDataTypes.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/MessageTextEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#pragma endregion Includes

//This is to avoid CheckInclude false positives EPLCN_IF_QUEUE_NAME

namespace SvEcat
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

constexpr LPCTSTR cSendDataName = _T("_SendData.log");
constexpr LPCTSTR cPlcSendHeading = _T("Timestamp; Output\n");
constexpr LPCTSTR cSendDataFormat = _T("{:f}; {:#08x}\n");

void APIENTRY notificationHandler(uint32_t notification, uint32_t, void* , void* pvUser)
{
	if (pvUser == nullptr)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_93_GENERAL_WARNING, SvStl::Tid_InvalidMemoryPointer, SvStl::SourceFileParams(StdMessageParams));
		return;
	}

	CifXCardEcat* pCifX = reinterpret_cast<CifXCardEcat*> (pvUser);

	pCifX->readProcessData(notification);
}

CifXCardEcat::CifXCardEcat(const EcatInputParam& rEcatInput) :
	m_rEcatInput(rEcatInput)
	, m_sendLogger {boost::log::keywords::channel = cSendDataName}
{
}

HRESULT CifXCardEcat::OpenAndInitializeCifX()
{
	HRESULT result = OpenCifX();

	if (CIFX_NO_ERROR == result)
	{
		result = WarmstartAndInitializeCifX();
	}

	if (CIFX_NO_ERROR == result)
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxInitializationSuccess, m_sourceFileParam);
		
		if (LogType::EcatSendData == (m_rEcatInput.m_logType & LogType::EcatSendData))
		{
			std::string fileName {m_rEcatInput.m_logFileName + cSendDataName};
			if (nullptr != m_logSendFile.open(fileName.c_str(), std::ios::out | std::ios::trunc))
			{
				boost::shared_ptr<std::ostream> stream = boost::make_shared<std::ostream>(&m_logSendFile);
				m_pSink = boost::make_shared<text_sink>();
				m_pSink->locked_backend()->add_stream(stream);
				auto filterContent = [](const boost::log::attribute_value_set& rAttribSet) -> bool {return rAttribSet["Channel"].extract<std::string>() == cSendDataName; };
				m_pSink->set_filter(filterContent);
				boost::log::core::get()->add_sink(m_pSink);
				std::string fileData(cPlcSendHeading);
				BOOST_LOG(m_sendLogger) << fileData.c_str();
			}
		}
	}
	else
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CifxInitializationError, m_sourceFileParam);
	}
	return result;
}

void CifXCardEcat::closeCifX()
{
	if (nullptr != m_pSink)
	{
		m_pSink->flush();
		m_pSink->stop();
		boost::log::core::get()->remove_sink(m_pSink);
	}
	if (m_logSendFile.is_open())
	{
		m_logSendFile.close();
	}

	if (m_cifxLoadLib.isInitilized())
	{
		::OutputDebugString("Closing application and freeing buffers...\n");
		m_cifxLoadLib.m_pChannelUnregisterNotification(m_hChannel, CIFX_NOTIFY_PD0_IN);
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

void CifXCardEcat::readProcessData(uint32_t notification)
{
	double timeStamp = SvUl::GetTimeStamp();
	if (notification != CIFX_NOTIFY_PD0_IN || false == m_cifxLoadLib.isInitilized())
	{
		return;
	}

	int32_t result = m_cifxLoadLib.m_pChannelIORead(m_hChannel, 0, 0, cEtherCatDataSize, m_pReadBuffer.get(), cTimeout);
	if (CIFX_NO_ERROR == result)
	{
		EcatInputData inputData;
		inputData.m_notificationTime = timeStamp;
		memcpy(&inputData.m_dynamicData[0], m_pReadBuffer.get(), cEtherCatDataSize);
		bool outputDataChanged {false};

		{
			std::lock_guard<std::mutex> guard {m_cifxMutex};
			if (m_hTelegramReadEvent != nullptr)
			{
				m_inputDataQueue.emplace(std::move(inputData));
				::SetEvent(m_hTelegramReadEvent);
			}
			outputDataChanged = m_sentOutputData != m_outputData;

			m_sentOutputData = m_outputData;
		}
		writeResponseData(m_sentOutputData);

		if (m_logSendFile.is_open())
		{
			if (0 == m_rEcatInput.m_logFilter || outputDataChanged)
			{
				uint32_t outputValue {0};
				memcpy(&outputValue, &m_sentOutputData[0], sizeof(outputValue));
				std::string fileData = std::format(cSendDataFormat, timeStamp, outputValue);
				BOOST_LOG(m_sendLogger) << fileData.c_str();
			}
		}
	}

	static bool threadSetPriority {false};
	if (false == threadSetPriority)
	{
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		threadSetPriority = true;
	}
}

EcatInputData CifXCardEcat::popInputDataQueue()
{
	EcatInputData result {};
	std::lock_guard<std::mutex> guard {m_cifxMutex};
	if (false == m_inputDataQueue.empty())
	{
		result = m_inputDataQueue.front();
		m_inputDataQueue.pop();
	}
	return result;
}

void CifXCardEcat::setOutput(uint8_t outputNr, bool state)
{
	constexpr uint8_t cBitsPerModule = 8;
	uint8_t outputIndex = outputNr / cBitsPerModule;
	uint8_t outputBit = outputNr % cBitsPerModule;
	uint8_t outputMask = state ? (1 << outputBit) : ~(1 << outputBit);
	if (outputIndex < m_outputData.size())
	{
		std::lock_guard<std::mutex> guard {m_cifxMutex};
		if (state)
		{
			m_outputData[outputIndex] |= outputMask;
		}
		else
		{
			m_outputData[outputIndex] &= outputMask;
		}
	}
}

void CifXCardEcat::setReady(bool ready)
{
	std::lock_guard<std::mutex> guard{m_cifxMutex};
	m_ready = ready;
}

int32_t CifXCardEcat::OpenCifX()
{
	int32_t result {CIFX_DEV_NOT_READY};

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
	std::string driverVersion {driverInfo.abDriverVersion};
	::OutputDebugString((_T("Driver version ") + driverVersion + '\n').c_str());

	::OutputDebugString("Opening channel ...\n");
	result = m_cifxLoadLib.m_pChannelOpen(m_hDriver, const_cast<char*> (cBoardName), cCifxChannel, &m_hChannel);
	if (CIFX_NO_ERROR != result)
	{
		m_sourceFileParam = SvStl::SourceFileParams(StdMessageParams);
		return result;
	}
	CHANNEL_INFORMATION channelInfo;
	m_cifxLoadLib.m_pChannelInfo(m_hChannel, sizeof(channelInfo), reinterpret_cast<void*> (&channelInfo));
	std::string firmware {std::to_string(channelInfo.usFWMajor) + '.' + std::to_string(channelInfo.usFWMinor) + '.' + std::to_string(channelInfo.usFWBuild) + '.' + std::to_string(channelInfo.usFWRevision)};
	std::string protocolType {reinterpret_cast<char*> (channelInfo.abFWName)};
	::OutputDebugString((protocolType + ' ' + firmware + '\n').c_str());
	SvStl::MessageManager Msg(SvStl::MsgType::Log);
	Msg.setMessage(SVMSG_SVO_94_GENERAL_Informational, SvStl::Tid_CifxVersionInfo,{driverVersion, protocolType, firmware}, SvStl::SourceFileParams(StdMessageParams));

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

int32_t CifXCardEcat::WarmstartAndInitializeCifX()
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

		m_pReadBuffer = std::make_unique<uint8_t[]>(cEtherCatDataSize);
		m_pWriteBuffer = std::make_unique<uint8_t[]>(cEtherCatDataSize);

		/* initialize the read and write buffer with zero */
		memset(m_pReadBuffer.get(), 0, cEtherCatDataSize);
		memset(m_pWriteBuffer.get(), 0, cEtherCatDataSize);

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

		result = m_cifxLoadLib.m_pChannelRegisterNotification(m_hChannel, CIFX_NOTIFY_PD0_IN, notificationHandler, reinterpret_cast<void*> (this));
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

int32_t CifXCardEcat::SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt)
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

int32_t CifXCardEcat::SendRecvCmdPkt(uint32_t command)
{
	CIFX_PACKET sendPkt;
	CIFX_PACKET recvPkt;

	///Need to initialize CIFX_PACKET manually!
	memset(&sendPkt, 0, sizeof(sendPkt));
	memset(&recvPkt, 0, sizeof(recvPkt));

	TLR_EMPTY_PACKET_T* pPacket = reinterpret_cast<TLR_EMPTY_PACKET_T*> (&sendPkt);
	pPacket->tHead.ulCmd = command;
	pPacket->tHead.ulDest = HIL_PACKET_DEST_DEFAULT_CHANNEL;

	return SendRecvPkt(&sendPkt, &recvPkt);
}

void CifXCardEcat::writeResponseData(const std::array<uint8_t, cEtherCatDataSize>& rOutputData)
{
	if(cEtherCatDataSize == rOutputData.size())
	{
		std::lock_guard<std::mutex> guard {m_cifxMutex};
		memcpy(m_pWriteBuffer.get(), &rOutputData[0], cEtherCatDataSize);
	}
	else
	{
		std::lock_guard<std::mutex> guard {m_cifxMutex};
		memset(m_pWriteBuffer.get(), 0, cEtherCatDataSize);
	}

	if (CIFX_NO_ERROR != m_cifxLoadLib.m_pChannelIOWrite(m_hChannel, 0, 0, cEtherCatDataSize, m_pWriteBuffer.get(), cTimeout))
	{
		SvStl::MessageManager Msg(SvStl::MsgType::Log);
		Msg.setMessage(SVMSG_SVO_92_GENERAL_ERROR, SvStl::Tid_CifxInitializationError, SvStl::SourceFileParams(StdMessageParams));
	}
}
} //namespace SvEcat
