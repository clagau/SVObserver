//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file SVPlcIOImpl.h
/// \brief Exported functions of the DLL
//******************************************************************************

#pragma once

#pragma region Includes
#include "PlcDataTypes.h"
#include "Triggering/IODeviceBase.h"
#pragma endregion Includes

namespace SvTrig
{
struct ResultData;
struct TriggerData;
}

namespace SvPlc
{

#pragma region Declarations
constexpr unsigned long cMaxPlcTriggers = 4;

struct TriggerReport;
#pragma endregion Declarations

class SVPlcIOImpl : public SvTrig::IODeviceBase
{
#pragma region Constructor
public:
	SVPlcIOImpl() = default;
	virtual ~SVPlcIOImpl() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Initialize(bool bInit);
	
	// Digital I/O
	unsigned long GetInputCount();
	unsigned long GetOutputCount();

	HRESULT SetOutputData(const SvTrig::ResultData& rResultData);

	// Triggers
	unsigned long GetTriggerCount() const;
	unsigned long GetTriggerHandle(unsigned long triggerIndex) const;
	_variant_t GetTriggerName(unsigned long triggerIndex) const;

	unsigned long TriggerGetParameterCount(unsigned long triggerIndex) const;
	_variant_t TriggerGetParameterName(unsigned long triggerIndex, unsigned long ulIndex) const;
	_variant_t TriggerGetParameterValue(unsigned long triggerIndex, unsigned long ulIndex) const;
	HRESULT TriggerSetParameterValue(unsigned long triggerIndex, unsigned long ulIndex, const _variant_t& rValue);

	// Non-Trigger Parameter Functions
	unsigned long GetParameterCount() const;
	_variant_t GetParameterName(unsigned long ulIndex) const;
	_variant_t GetParameterValue(unsigned long ulIndex) const;
	HRESULT SetParameterValue(unsigned long ulIndex, const _variant_t& rValue);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void beforeStartTrigger(unsigned long triggerIndex) override;
	HRESULT afterStartTrigger() override;
	void beforeStopTrigger(unsigned long triggerIndex) override;

	void NotifyTriggerData(const SvTrig::TriggerData&);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	PlcInputParam m_plcInputParam;
	std::atomic_bool m_engineStarted {false};
	std::atomic_bool m_engineInitialized {false};
	std::array<std::atomic_bool, cMaxPlcTriggers> m_triggerStarted{false, false, false, false};
	std::array<std::atomic_uint32_t, cMaxPlcTriggers> m_inputCount {0UL, 0UL, 0UL, 0UL};
	std::array<std::atomic_uint32_t, cMaxPlcTriggers> m_outputCount {0UL, 0UL, 0UL, 0UL};
	std::atomic_int8_t m_currentTriggerChannel{-1};

	std::ofstream m_logInFile;
	std::ofstream m_logOutFile;
#pragma endregion Member Variables
};

} //namespace SvPlc
