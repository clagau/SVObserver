//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file CifxCard.h
/// \brief Declaration of the class CifXCard
/// Also contains the helper class ProcedureCallInformation
//******************************************************************************
#pragma once

#pragma region Includes
#include "CifxCommonData.h"
#include "ConfigDataSet.h"
#include "InspectionCommand.h"
#include "Telegram.h"
#include "TimeSync.h"
#pragma endregion Includes

struct InspectionState;

///controls access to process memory (input and output buffers) of the CifX card 
class CifXCard
{
public:
	CifXCard(uint32_t CifXNodeId, uint32_t MaxRtdSize);
	~CifXCard() = default;

	bool OpenAndInitializeCifX(); ///< starts the Powerlink connection to the PLC-CPU
	void closeCifX(); ///< ends the Powerlink connection to the PLC-CPU

	uint32_t OpenCifX(char* pBoardName);
	uint32_t closeDriver(bool showDetails);
	uint32_t SendConfigurationToCifX();
	uint32_t WarmstartAndInitializeCifX();

	void readProcessData();
	void writeProcessData();
	void sendSyncCommand();

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
	void sendDefaultResponse();

	const Telegram& getInputTelegram() { return m_inputTelegram; }
	const InspectionCommand& getInspectionCmd() { return m_inspectionCmd; }
	bool isProtocolInitialized() { return m_protocolInitialized; }

private:

	std::vector<ConfigDataSet> createConfigList();
	void writeResponseData(const uint8_t* pSdoDynamic, size_t sdoDynamicSize);

	uint32_t m_currentResult {0UL};

	const uint32_t m_CifXNodeId {0UL}; //< The powerlink node ID used for the Hilscher CifX card
	const uint32_t m_maxPlcDataSize {0UL}; // the size of the PLC data in bytes

	bool m_processDataCanBeRead {false};
	uint64_t m_processDataReadCount {0ULL}; ///< running count of process data reads. This is also the number of interrupts 
	double m_TelegramReceiveTime {0.0};

	double m_syncTime {0.0};
	int32_t m_syncSocRelative {INT_MAX};
	bool m_protocolInitialized {false};
	uint16_t m_contentID {0};

	uint32_t m_plcSendTime {0UL};

	HANDLE m_hTelegramReadEvent {nullptr};

	CommonData m_commonData;
	Telegram m_inputTelegram;
	TimeSync m_timeSync;
	InspectionCommand m_inspectionCmd;

	//ConfigDataSet m_testInConfig[c_ConfigListSize];		//Only required for test purposes
};

