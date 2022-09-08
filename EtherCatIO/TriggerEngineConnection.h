/// \copyright COPYRIGHT (c) 2017, 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TriggerEngineConnection.h
/// \brief the interface to the TriggerEngine: declarations

#pragma once

namespace SvEcat
{

struct ResultReport;
struct TriggerReport;

enum class TriggerType
{
	None,
	HardwareTrigger,
	SimulatedTrigger
};

namespace Tec
{
void startTriggerEngine(std::function<void(const TriggerReport&)> reportTrigger, TriggerType triggerType, const std::string& rAdditionalData);
void stopTriggerEngine();

HRESULT initTriggerEngine();

void setReady(bool ready);
void setTriggerChannel(uint8_t channel, bool active);

uint32_t getInputs();
void setOutput(uint8_t outputNr, bool state);
} //namespace Tec

} //namespace SvPlc


