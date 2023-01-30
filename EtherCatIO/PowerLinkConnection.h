//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PowerLinkConnection.h
/// \brief Definition of the class PowerlinkConnection
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerSource.h"
#pragma endregion Includes


namespace SvEcat
{
struct TriggerReport;
struct ResultReport;
enum class TriggerType;

extern HANDLE g_hSignalEvent;

class PowerlinkConnection
{
public:
	PowerlinkConnection(std::function<void(const SvTrig::TriggerData&)> pTriggerDataCallback, TriggerType triggerType, const std::string& rAdditionalData);
	~PowerlinkConnection();

	void setReady(bool ready);
	void setTriggerChannel(uint8_t channel, bool active);
	uint32_t getInputs() const;
	void setOutput(uint8_t outputNr, bool state);
	HRESULT initialize();

private:
	void StartEventSignalThread();
	void StopEventSignalThread();
	void EventSignalThread(std::function<void()> pCallback);
	void EventHandler();

	//void SendProductsToPlc(); ///sends (via m_pdx) information with the CifX card
	
	//void useResult(const Product& rProduct); ///< prepares a product for sending to the cifX card
	//void QueueProduct(const Product& rProduct); ///< called for products the reusults of which are ready to be sent to the PLC

	std::thread m_eventSignalThread;

	std::unique_ptr<TriggerSource> m_pTriggersource = nullptr;

	uint32_t m_mostRecentlyReceivedOid[cNumberOfChannels] = {0,0,0,0};

};

} //namespace SvEcat
