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


namespace SvPlc
{
struct PlcInputParam;
struct ResultReport;

extern HANDLE g_hSignalEvent;

class PowerlinkConnection
{
public:
	PowerlinkConnection(const PlcInputParam& rPlcInput);
	~PowerlinkConnection();

	void setReady(bool ready);
	void setTriggerChannel(uint8_t channel, bool active);
	void writeResult(const ResultReport& rResultReport);
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

} //namespace SvPlc
