/// \copyright COPYRIGHT (c) 2017, 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TriggerEngineConnection.cpp
/// \brief the interface to the TriggerEngine: implementation

#pragma region Includes
#include "stdafx.h"
#include "TriggerEngineConnection.h"
#include "EtherCatConnection.h" 
#pragma endregion Includes

namespace SvEcat
{
std::unique_ptr<EtherCatConnection> g_pPowerLink;

namespace Tec
{
void startTriggerEngine(const EcatInputParam& rEcatInput)
{
	g_pPowerLink = std::make_unique<EtherCatConnection>(rEcatInput);
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

uint32_t getInputs()
{
	if (nullptr != g_pPowerLink)
	{
		return g_pPowerLink->getInputs();
	}
	return 0UL;
}

void setOutput(uint8_t outputNr, bool state)
{
	if(nullptr != g_pPowerLink)
	{
		g_pPowerLink->setOutput(outputNr, state);
	}
}
} //namespace Tec

} //namespace SvEcat
