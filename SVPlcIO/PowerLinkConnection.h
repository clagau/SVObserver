//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file PowerLinkConnection.h
/// \brief Definition of the class PowerlinkConnection
//******************************************************************************
#pragma once

#pragma region Includes
#include "PlcDataTypes.h"
#pragma endregion Includes

namespace SvTrig
{
struct ResultData;
}

namespace SvPlc
{
class TriggerSource;
struct PlcInputParam;
struct ResultReport;

extern HANDLE g_hSignalEvent;

class PowerlinkConnection
{
public:
	explicit PowerlinkConnection(const PlcInputParam& rPlcInput);
	~PowerlinkConnection();

	void setReady(bool ready);
	void setTriggerChannel(uint8_t channel, bool active);
	void writeResult(const SvTrig::ResultData& rResultData);
	HRESULT initialize();

private:
	void StartEventSignalThread();
	void StopEventSignalThread();
	void EventSignalThread(std::function<void()> pCallback);
	void EventHandler();

	std::thread m_eventSignalThread;

	std::unique_ptr<TriggerSource> m_pTriggersource {nullptr};

	uint32_t m_mostRecentlyReceivedOid[cNumberOfChannels] = {0,0,0,0};
};

} //namespace SvPlc
