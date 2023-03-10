//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file CifxCard.h
/// \brief Declaration of the class CifXCard
/// Also contains the helper class ProcedureCallInformation
//******************************************************************************
#pragma once

#pragma region Includes
#include "ConfigDataSet.h"
#include "InspectionCommand.h"
#include "InspectionState.h"
#include "Telegram.h"
#include "SVStatusLibrary/SourceFileParams.h"
#include "Triggering/CifxLoadLibrary.h"
#pragma endregion Includes

namespace SvPlc
{
struct PlcInputParam;
struct InspectionState1;

constexpr size_t cCmdDataSize = std::max(sizeof(InspectionCommand1), sizeof(InspectionCommand2));

enum class PlcVersion : uint16_t
{
	PlcDataNone = 0,
	PlcData1 = 0x0501,		//1.5 order is reversed to send correctly to the PLC (Note Version 1.3 and 1.4 have been marked obsolete)
	PlcData2 = 0x0201		//1.2 order is reversed to send correctly to the PLC
};

struct InputData
{
	InputData() { memset(&m_dynamicData[0], 0, m_dynamicData.size()); }
	double m_notificationTime {0.0};
	Telegram m_telegram;
	std::array<uint8_t, cCmdDataSize> m_dynamicData;
};

class CifXCard
{
public:
	explicit CifXCard(const PlcInputParam& rPlcInput);
	~CifXCard() = default;

	HRESULT OpenAndInitializeCifX();
	void closeCifX();

	void readProcessData(uint32_t notification);

	InputData popInputDataQueue();

	void setHandleForTelegramReceptionEvent(HANDLE hEvent) { m_hTelegramReadEvent = hEvent; }

	void queueResult(uint8_t channel, ChannelOut1&& channelOut);

	bool isProtocolInitialized() { return m_protocolInitialized; }
	uint32_t getTriggerDataOffset() { return m_triggerDataOffset; }
	void setReady(bool ready);
	PlcVersion getPlcVersion() const { return m_plcVersion; }

private:
	int32_t OpenCifX();
	int32_t SendConfigurationToCifX();
	int32_t WarmstartAndInitializeCifX();

	void sendVersion(const Telegram& rTelegram, uint16_t plcVersion);
	void sendConfigList(const Telegram& rTelegram, const ConfigDataSet* const pConfigDataSet);
	void sendOperationData(const Telegram& rTelegram, const InspectionState1& rState);
	int32_t SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt);
	int32_t SendRecvCmdPkt(uint32_t ulCmd);
	void BuildConfigurationReq(CIFX_PACKET* ptPacket, uint16_t NodeId, uint16_t DataLength);

	std::vector<ConfigDataSet> createConfigList(TelegramLayout layout);
	void writeResponseData(const Telegram& rInputTelegram,  const uint8_t* pSdoDynamic, size_t sdoDynamicSize);

	const PlcInputParam& m_rPlcInput;

	CIFXHANDLE m_hDriver {nullptr};
	CIFXHANDLE m_hChannel {nullptr};
	std::unique_ptr<uint8_t[]> m_pReadBuffer;
	std::unique_ptr<uint8_t[]> m_pWriteBuffer;

	bool m_protocolInitialized {false};
	bool m_ready {false};
	uint32_t m_notifyType {0UL};
	uint16_t m_contentID {0};

	HANDLE m_hTelegramReadEvent {nullptr};

	std::mutex m_cifxMutex;
	SvTrig::CifxLoadLibrary m_cifxLoadLib;
	std::queue<InputData> m_inputDataQueue;
	PlcVersion m_plcVersion{ PlcVersion::PlcDataNone };
	std::queue<InspectionState1> m_inspectionStateQueue;
	uint32_t m_triggerDataOffset{ 0UL };	//The memory offset in bytes to the trigger data starting from the inspection command struct
	SvStl::SourceFileParams m_sourceFileParam; //The last recorded source file parameter setting

	std::vector<std::pair<TelegramLayout, std::vector<ConfigDataSet>>> m_configDataSetVector;
	std::filebuf m_logContentFile;
	std::filebuf m_logSendFile;
	typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> m_pSink {nullptr};
	boost::log::sources::channel_logger_mt<std::string> m_contentLogger;
	boost::log::sources::channel_logger_mt<std::string> m_sendLogger;
};

} //namespace SvPlc
