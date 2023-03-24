//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file CifXCardEcat.h
/// \brief Settings and interface to the CifX card as EtherCat Master
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStatusLibrary/SourceFileParams.h"
#include "Triggering/CifxLoadLibrary.h"
#pragma endregion Includes

namespace SvEcat
{
struct EcatInputParam;

constexpr uint16_t cEtherCatDataSize = 8; //< the maximum size of the EtherCAT packet in bytes

struct EcatInputData
{
	double m_notificationTime {0.0};
	std::array<uint8_t, cEtherCatDataSize> m_dynamicData {0,0,0,0,0,0,0,0};
};

class CifXCardEcat
{
public:
	explicit CifXCardEcat(const EcatInputParam& rEcatInput);
	~CifXCardEcat() = default;

	HRESULT OpenAndInitializeCifX();
	void closeCifX();

	void readProcessData(uint32_t notification);

	EcatInputData popInputDataQueue();

	void setOutput(uint8_t outputNr, bool state);

	void setHandleForTelegramReceptionEvent(HANDLE hEvent) { m_hTelegramReadEvent = hEvent; }

	void setReady(bool ready);
	bool isReady() const { return m_ready; }

private:
	int32_t OpenCifX();
	int32_t WarmstartAndInitializeCifX();

	int32_t SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt);
	int32_t SendRecvCmdPkt(uint32_t ulCmd);

	void writeResponseData(const std::array<uint8_t, cEtherCatDataSize>& rOutputData);

	const EcatInputParam& m_rEcatInput;

	CIFXHANDLE m_hDriver {nullptr};
	CIFXHANDLE m_hChannel {nullptr};
	std::unique_ptr<uint8_t[]> m_pReadBuffer {};
	std::unique_ptr<uint8_t[]> m_pWriteBuffer {};

	bool m_ready {false};
	std::array<uint8_t, cEtherCatDataSize> m_outputData {0,0,0,0,0,0,0,0};
	std::array<uint8_t, cEtherCatDataSize> m_sentOutputData {0,0,0,0,0,0,0,0};

	HANDLE m_hTelegramReadEvent {nullptr};

	std::mutex m_cifxMutex {};
	SvTrig::CifxLoadLibrary m_cifxLoadLib {};
	std::queue<EcatInputData> m_inputDataQueue {};
	EcatInputData m_currentInputData {};
	SvStl::SourceFileParams m_sourceFileParam {}; //The last recorded source file parameter setting

	std::filebuf m_logSendFile;
	typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> m_pSink {nullptr};
	boost::log::sources::channel_logger_mt<std::string> m_sendLogger;
};

} //namespace SvEcat
