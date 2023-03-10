//*****************************************************************************
/// \copyright (c) 2019,2019 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved 
/// \file PlcDataTypes.h
//*****************************************************************************
/// The header of the PLC data types
//******************************************************************************
#pragma once

#pragma region Includes
#pragma endregion Includes

namespace SvTrig
{
struct TriggerData;
}

namespace SvPlc
{

constexpr unsigned int cNumberOfChannels = 4;
constexpr unsigned int cConfigListSize = 62;
constexpr uint8_t cResultSize = 14;
constexpr uint8_t cSerialCodeSize = 5;
constexpr uint8_t cHeaderSize = 8;
constexpr uint8_t cModeSingleDirect = 1;

enum PlcDataType : uint16_t
{
	NoneDataType = 0,
	BoolIndex = 1,
	Int8Index = 2,
	Int16Index = 3,
	Int32Index = 4,
	Uint8Index = 5,
	Uint16Index = 6,
	Uint32Index = 7,
	FloatIndex = 8,
	StringIndex = 9,
	WstringIndex = 10,
	DateTimeIndex = 11,
	TimeIndex = 12,
	DateIndex = 13,
	DoubleIndex = 14,
};

enum class TriggerType
{
	None,
	HardwareTrigger,
	SimulatedTrigger
};

enum LogType : uint16_t
{
	NoLogging = 0x0000,
	PlcInData = 0x0001,
	PlcOutData = 0x0002,
	PlcContentData = 0x0004,
	PlcOperationData = 0x0008,
	PlcSendData = 0x0010,
};

struct PlcInputParam
{
	std::function<void(const SvTrig::TriggerData&)> m_pTriggerDataCallBack {};
	TriggerType m_triggerType {TriggerType::None};
	uint16_t m_plcNodeID {0};
	uint16_t m_plcTransferTime {0};
	std::string m_PD0Version {};
	std::string m_simulationFile {};
	std::string m_logFileName {};
	uint16_t m_logType {LogType::NoLogging};
	uint16_t m_logFilter {0};
};

} //namespace SvPlc
