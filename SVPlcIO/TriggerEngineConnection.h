/// \copyright COPYRIGHT (c) 2017, 2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerEngineConnection.h
/// \brief the interface to the TriggerEngine: declarations

#pragma once

namespace SvPlc
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
void startTriggerEngine(std::function<void(const TriggerReport&)> reportTrigger, TriggerType triggerType, uint16_t plcNodeID, uint16_t plcTransferTime, const std::string& rAdditionalData);
void stopTriggerEngine();

HRESULT initTriggerEngine();

void setReady(bool ready);
void setTriggerChannel(uint8_t channel, bool active);

void writeResult(const ResultReport& rReportResult);
} //namespace Tec

} //namespace SvPlc


