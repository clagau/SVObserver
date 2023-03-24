//*****************************************************************************
/// \copyright (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file EtherCatDataTypes.h
//*****************************************************************************
/// The header of the EtherCat data types
//******************************************************************************
#pragma once

#pragma region Includes
#include <stdint.h>
#pragma endregion Includes

namespace SvTrig
{
struct TriggerData;
}

namespace SvEcat
{

constexpr unsigned int cNumberOfChannels = 4; /// the number of trigger channels

enum class EcatTriggerType
{
	None,
	HardwareTriggerEcat,
	SimulatedTriggerEcat
};

enum LogType : uint16_t
{
	NoLogging = 0x0000,
	EcatInData = 0x0001,
	EcatOutData = 0x0002,
	EcatOperationData = 0x0004,
	EcatSendData = 0x0008,
};

struct EcatInputParam
{
	std::function<void(const SvTrig::TriggerData&)> m_pTriggerDataCallBack {};
	EcatTriggerType m_triggerType {EcatTriggerType::None};
	std::string m_simulationFile {};
	std::string m_logFileName {};
	uint16_t m_logType {LogType::NoLogging};
	uint16_t m_logFilter {0};
};
} //namespace SvEcat
