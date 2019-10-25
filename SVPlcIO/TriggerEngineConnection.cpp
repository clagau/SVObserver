/// \copyright COPYRIGHT (c) 2017, 2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerEngineConnection.cpp
/// \brief the interface to the TriggerEngine: implementation

#pragma region Includes
#include "stdafx.h"
#include "TriggerEngineConnection.h"
#include "PowerLinkConnection.h" 
#pragma endregion Includes

std::unique_ptr<PowerlinkConnection> g_pPowerLink;

void startTriggerEngine(std::function<void(const TriggerReport&)> reportTrigger,
	uint16_t simulateTriggers)
{
	g_pPowerLink = std::make_unique<PowerlinkConnection>(reportTrigger, simulateTriggers);
	if(nullptr != g_pPowerLink)
	{
		::OutputDebugString("Start Trigger Engine!\n");
		g_pPowerLink->StartEventSignalThread();
	}
}


void stopTriggerEngine()
{

	if(nullptr != g_pPowerLink)
	{
		::OutputDebugString("Stop Trigger Engine!\n");
		g_pPowerLink.reset();
		g_pPowerLink = nullptr;
	}
}


void SetTriggerChannel(uint8_t channel, bool active, uint32_t period)
{
	if(nullptr != g_pPowerLink)
	{
		g_pPowerLink->SetTriggerChannel(channel, active, period);
	}
}

void writeResult(const ResultReport& rRr)
{
	if(nullptr != g_pPowerLink)
	{
		//Product product;
		//product.m_Channel = rRr.m_channel;
		//product.m_ObjectId = rRr.m_objectID;
		//product.m_InspectionResult = rRr.m_result;
		//g_pPowerLink->WriteProductResult(product);
	}
}

