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

///controls access to process memory (input and output buffers) of the CifX card 
class CifXCard
{
public:
	CifXCard(uint32_t CifXNodeId, uint32_t MaxRtdSize);
	~CifXCard() = default;

	HRESULT OpenAndInitializeCifX(); ///< starts the Powerlink connection to the PLC-CPU
	void closeCifX(); ///< ends the Powerlink connection to the PLC-CPU

	int32_t OpenCifX();
	int32_t SendConfigurationToCifX();
	int32_t WarmstartAndInitializeCifX();

	void readProcessData();

	uint32_t currentResult() { return m_currentResult; }

	bool canProcessDataBeRead() { return m_processDataCanBeRead; }
	uint64_t getProcessDataReadCount() { return m_processDataReadCount; };
	double telegramReceiveTime() { return m_TelegramReceiveTime; }

	double getSyncTime() { return m_syncTime;  }
	uint32_t getSyncSocRel() { return m_syncSocRelative;  }

	void setHandleForTelegramReceptionEvent(HANDLE h) { m_hTelegramReadEvent = h; }

	void sendVersion();
	void setPlcLoopSyncTime();
	void sendConfigList();
	void sendOperationData(const InspectionState& rState);

	const Telegram& getInputTelegram() { return m_inputTelegram; }
	const InspectionCommand& getInspectionCmd() { return m_inspectionCmd; }
	bool isProtocolInitialized() { return m_protocolInitialized; }
	void setReady(bool ready) { m_ready = ready; } 

private:
	uint32_t SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt);
	uint32_t SendRecvEmptyPkt(uint32_t ulCmd);
	void BuildConfigurationReq(CIFX_PACKET* ptPacket, uint8_t NodeId, uint16_t DataLength);

	std::vector<ConfigDataSet> createConfigList(TelegramLayout layout);
	void writeResponseData(const uint8_t* pSdoDynamic, size_t sdoDynamicSize);

	uint32_t m_currentResult {0UL};

	const uint32_t m_CifXNodeId {0UL}; //< The powerlink node ID used for the Hilscher CifX card
	const uint32_t m_maxPlcDataSize {0UL}; // the size of the PLC data in bytes

	CIFXHANDLE m_hDriver {nullptr};				/// Cifx driver handle
	CIFXHANDLE m_hChannel {nullptr};			/// Cifx channel handle
	std::unique_ptr<uint8_t> m_pReadBuffer;		///Receive buffer
	std::unique_ptr<uint8_t> m_pWriteBuffer;	///Send buffer

	bool m_processDataCanBeRead {false};
	uint64_t m_processDataReadCount {0ULL}; ///< running count of process data reads. This is also the number of interrupts 
	std::atomic<double> m_TelegramReceiveTime {0.0};

	double m_syncTime {0.0};
	int32_t m_syncSocRelative {INT_MAX};
	bool m_protocolInitialized {false};
	bool m_ready {false};
	uint16_t m_contentID {0};

	uint32_t m_plcSendTime {0UL};

	HANDLE m_hTelegramReadEvent {nullptr};

	std::mutex m_cifxMutex;

	CifxLoadLibrary m_cifxLoadLib;
	Telegram m_inputTelegram;
	TimeSync m_timeSync;
	InspectionCommand m_inspectionCmd;

	std::map<TelegramLayout, std::vector<ConfigDataSet>> m_configDataSetsMap;
};

} //namespace SvPlc
