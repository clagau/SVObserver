/// \copyright COPYRIGHT (c) 2017, 2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerEngineConnection.cpp
/// \brief the interface to the TriggerEngine: implementation

#pragma region Includes
#include "stdafx.h"
#include "TriggerEngineConnection.h"
#include "PlcDataTypes.h"
#include "PowerLinkConnection.h" 
#pragma endregion Includes

namespace SvPlc
{
std::unique_ptr<PowerlinkConnection> g_pPowerLink;

namespace Tec
{
void startTriggerEngine(std::function<void(const TriggerReport&)> reportTrigger,
	uint16_t plcNodeID, uint16_t plcTransferTime, uint16_t simulateTriggers)
{
	g_pPowerLink = std::make_unique<PowerlinkConnection>(reportTrigger, plcNodeID, plcTransferTime, simulateTriggers);
	if(nullptr != g_pPowerLink)
	{
		::OutputDebugString("Start Trigger Engine!\n");
	}
}

void stopTriggerEngine()
{
	if (nullptr != g_pPowerLink)
	{
		::OutputDebugString("Stop Trigger Engine!\n");
		g_pPowerLink.reset();
		g_pPowerLink = nullptr;
	}
}

HRESULT initTriggerEngine()
{
	if (nullptr != g_pPowerLink)
	{
		::OutputDebugString("Initialize Trigger Engine!\n");
		return g_pPowerLink->initialize();
	}
	else
	{
		return E_POINTER;
	}
}

void setReady(bool ready)
{
	if (nullptr != g_pPowerLink)
	{
		g_pPowerLink->setReady(ready);
	}
}

void setTriggerChannel(uint8_t channel, bool active, uint32_t period)
{
	if(nullptr != g_pPowerLink)
	{
		g_pPowerLink->setTriggerChannel(channel, active, period);
	}
}

void writeResult(const ResultReport& rResultReport)
{
	if(nullptr != g_pPowerLink)
	{
		g_pPowerLink->writeResult(rResultReport);
	}
}
} //namespace Tec

} //namespace SvPlc
