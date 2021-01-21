//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file CifxCard.h
/// \brief Declaration of the class CifXCard
/// Also contains the helper class ProcedureCallInformation
//******************************************************************************
#pragma once

#pragma region Includes
#include "CifxLoadLibrary.h"
#include "ConfigDataSet.h"
#include "InspectionCommand.h"
#include "Telegram.h"
#include "TimeSync.h"
#pragma endregion Includes

namespace SvPlc
{
struct InspectionState;

struct InputData
{
	double m_interruptTime {0.0};
	int32_t m_syncSocRelative {0L};
	Telegram m_telegram;
	TimeSync m_timeSync;
	InspectionCommand m_inspectionCmd;
};

///controls access to process memory (input and output buffers) of the CifX card 
class CifXCard
{
public:
	CifXCard(uint16_t CifXNodeId, uint16_t MaxRtdSize);
	~CifXCard() = default;

	HRESULT OpenAndInitializeCifX(const std::string& rAdditionalData);
	void closeCifX();

	void readProcessData(uint32_t notification);

	const InputData& getCurrentInputData() { return m_currentInputData; }
	void popInputDataQueue();

	void setHandleForTelegramReceptionEvent(HANDLE h) { m_hTelegramReadEvent = h; }

	void sendVersion();
	void sendConfigList();
	void sendOperationData(const InspectionState& rState);

	bool isProtocolInitialized() { return m_protocolInitialized; }
	void setReady(bool ready);

private:
	int32_t OpenCifX(const std::string& rAdditionalData);
	int32_t SendConfigurationToCifX();
	int32_t WarmstartAndInitializeCifX();

	int32_t SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt);
	int32_t SendRecvCmdPkt(uint32_t ulCmd);
	void BuildConfigurationReq(CIFX_PACKET* ptPacket, uint16_t NodeId, uint16_t DataLength);

	std::vector<ConfigDataSet> createConfigList(TelegramLayout layout);
	void writeResponseData(const uint8_t* pSdoDynamic, size_t sdoDynamicSize);

	const uint16_t m_CifXNodeId {0};
	const uint16_t m_maxPlcDataSize {0UL};

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
	CifxLoadLibrary m_cifxLoadLib;
	std::queue<InputData> m_inputDataQueue;
	InputData m_currentInputData;

	std::map<TelegramLayout, std::vector<ConfigDataSet>> m_configDataSetsMap;
};

} //namespace SvPlc
