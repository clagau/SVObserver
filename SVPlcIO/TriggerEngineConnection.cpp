/// \copyright COPYRIGHT (c) 2017, 2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
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
void startTriggerEngine(const PlcInputParam& rPlcInput)
{
	g_pPowerLink = std::make_unique<PowerlinkConnection>(rPlcInput);
	::OutputDebugString("Start Trigger Engine!\n");
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

void setTriggerChannel(uint8_t channel, bool active)
{
	if(nullptr != g_pPowerLink)
	{
		g_pPowerLink->setTriggerChannel(channel, active);
	}
}

void writeResult(const SvTrig::ResultData& rResultData)
{
	if(nullptr != g_pPowerLink)
	{
		g_pPowerLink->writeResult(rResultData);
	}
}
} //namespace Tec

} //namespace SvPlc
