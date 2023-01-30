/// \copyright COPYRIGHT (c) 2017, 2017 by K�rber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file TriggerEngineConnection.h
/// \brief the interface to the TriggerEngine: declarations

#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvTrig
{
struct ResultData;
}

namespace SvPlc
{
struct PlcInputParam;

namespace Tec
{
void startTriggerEngine(const PlcInputParam& rPlcInput);
void stopTriggerEngine();

HRESULT initTriggerEngine();

void setReady(bool ready);
void setTriggerChannel(uint8_t channel, bool active);

void writeResult(const SvTrig::ResultData& rResultData);
} //namespace Tec

} //namespace SvPlc


