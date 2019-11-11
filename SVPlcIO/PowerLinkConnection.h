//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file PowerLinkConnection.h
/// \brief Definition of the class PowerlinkConnection
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerSource.h"
#pragma endregion Includes


struct TriggerReport;
struct ResultReport;

extern HANDLE g_hTelegramEvent;
extern HANDLE g_hStopEvent;

class PowerlinkConnection
{
public:
	PowerlinkConnection(std::function<void(const TriggerReport&)> pReportTrigger, uint16_t simulateTriggers);
	~PowerlinkConnection();

	void SetTriggerChannel(uint8_t channel, bool active, uint32_t period);
	void writeResult(const ResultReport& rResult);

	void StartEventSignalThread();

private:
	void EventSignalThread(std::function<void()> pCallback);
	void EventHandler();

	//void SendProductsToPlc(); ///sends (via m_pdx) information with the CifX card
	
	//void useResult(const Product& rProduct); ///< prepares a product for sending to the cifX card
	//void QueueProduct(const Product& rProduct); ///< called for products the reusults of which are ready to be sent to the PLC

	std::thread m_eventSignalThread;

	std::unique_ptr<TriggerSource> m_pTriggersource = nullptr;
	std::function<void(const TriggerReport&)> m_pReportTrigger;

	uint32_t m_mostRecentlyReceivedOid[c_NumberOfChannels] = {0,0,0,0};

};

