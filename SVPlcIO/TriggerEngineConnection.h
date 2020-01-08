/// \copyright COPYRIGHT (c) 2017, 2017 by Seidenader Maschinenbau GmbH
/// All Rights Reserved
/// \file TriggerEngineConnection.h
/// \brief the interface to the TriggerEngine: declarations

#pragma once

#include "PlcDataTypes.h"

namespace SvPlc
{

namespace Tec
{
void startTriggerEngine(std::function<void(const TriggerReport&)> reportTrigger, uint16_t plcTransferTime, uint16_t simulateTriggers);
void stopTriggerEngine();

void setReady(bool ready);
void setTriggerChannel(uint8_t channel, bool active, uint32_t period=0UL);

void writeResult(const ResultReport& rReportResult);
} //namespace Tec

} //namespace SvPlc


