//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPlcIOImpl
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
#include "Triggering/IODeviceBase.h"

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

	HRESULT SetOutputData(unsigned long triggerIndex, const SvTrig::IntVariantMap& rData);

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

	void reportTrigger(const TriggerReport&);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	uint16_t m_plcTransferTime {0};
	uint16_t m_plcNodeID {0};
	long m_PlcVersion {0L};
	std::atomic_bool m_engineStarted {false};
	std::atomic_bool m_engineInitialized {false};
	std::atomic_bool m_triggerStarted[cMaxPlcTriggers]{false, false, false, false};
	std::atomic_uint32_t m_inputCount[cMaxPlcTriggers] {0UL, 0UL, 0UL, 0UL};
	std::atomic_uint32_t m_outputCount[cMaxPlcTriggers] {0UL, 0UL, 0UL, 0UL};
	std::atomic_int8_t m_currentTriggerChannel{-1};

	std::string m_plcSimulateFile;
	std::string m_logFileName;
	std::ofstream m_logInFile;
	std::ofstream m_logOutFile;
#pragma endregion Member Variables
};

} //namespace SvPlc
