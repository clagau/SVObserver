//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PowerLinkConnection.h
/// \brief Definition of the class PowerlinkConnection
//******************************************************************************
#pragma once

#pragma region Includes
#include "EtherCatDataTypes.h"
#pragma endregion Includes


namespace SvEcat
{
class TriggerSource;
struct EcatInputParam;

extern HANDLE g_hSignalEvent;

class EtherCatConnection
{
public:
	explicit EtherCatConnection(const EcatInputParam& rEcatInput);
	~EtherCatConnection();

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

	std::thread m_eventSignalThread;
	std::unique_ptr<TriggerSource> m_pTriggersource = nullptr;
};

} //namespace SvEcat
