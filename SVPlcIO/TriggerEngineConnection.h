/// \copyright COPYRIGHT (c) 2017, 2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TriggerEngineConnection.h
/// \brief the interface to the TriggerEngine: declarations

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvPlc
{
struct PlcInputParam;
struct ResultReport;

namespace Tec
{
void startTriggerEngine(const PlcInputParam& rPlcInput);
void stopTriggerEngine();

HRESULT initTriggerEngine();

void setReady(bool ready);
void setTriggerChannel(uint8_t channel, bool active);

void writeResult(const ResultReport& rReportResult);
} //namespace Tec

} //namespace SvPlc


