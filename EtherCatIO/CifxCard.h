//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file CifxCard.h
/// \brief Declaration of the class CifXCard
/// Also contains the helper class ProcedureCallInformation
//******************************************************************************
#pragma once

#pragma region Includes
#include "SVStatusLibrary/SourceFileParams.h"
#include "Triggering/CifxLoadLibrary.h"
#pragma endregion Includes

namespace SvEcat
{

constexpr uint16_t cEtherCatDataSize = 8; //< the maximum size of the EtherCAT packet in bytes

struct InputData
{
	double m_notificationTime {0.0};
	std::array<uint8_t, cEtherCatDataSize> m_dynamicData {0,0,0,0,0,0,0,0};
};

class CifXCard
{
public:
	CifXCard() = default;
	~CifXCard() = default;

	HRESULT OpenAndInitializeCifX();
	void closeCifX();

	void readProcessData(uint32_t notification);

	const InputData& getCurrentInputData() { return m_currentInputData; }
	bool popInputDataQueue();

	void setHandleForTelegramReceptionEvent(HANDLE h) { m_hTelegramReadEvent = h; }

	void sendOperationData(const std::array<uint8_t, cEtherCatDataSize>& rOutputData);

	void setReady(bool ready);
	bool isReady() const { return m_ready; }

private:
	int32_t OpenCifX();
	int32_t SendConfigurationToCifX();
	int32_t WarmstartAndInitializeCifX();

	int32_t SendRecvPkt(CIFX_PACKET* pSendPkt, CIFX_PACKET* pRecvPkt);
	int32_t SendRecvCmdPkt(uint32_t ulCmd);

	void writeResponseData(const std::array<uint8_t, cEtherCatDataSize>& rOutputData);

	CIFXHANDLE m_hDriver {nullptr};
	CIFXHANDLE m_hChannel {nullptr};
	std::unique_ptr<uint8_t[]> m_pReadBuffer {};
	std::unique_ptr<uint8_t[]> m_pWriteBuffer {};

	bool m_ready {false};

	HANDLE m_hTelegramReadEvent {nullptr};

	std::mutex m_cifxMutex {};
	SvTrig::CifxLoadLibrary m_cifxLoadLib {};
	std::queue<InputData> m_inputDataQueue {};
	InputData m_currentInputData {};
	SvStl::SourceFileParams m_sourceFileParam {}; //The last recorded source file parameter setting

};

} //namespace SvPlc
