//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVPlcIOImpl
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <vector>
#include "TriggerInformation/IODeviceBase.h"

namespace SvPlc
{

#pragma region Declarations
constexpr unsigned long cMaxPlcTriggers = 4;

struct TriggerReport;
#pragma endregion Declarations

class SVPlcIOImpl : public SvTi::IODeviceBase
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
	unsigned long GetPortCount();

	HRESULT GetInputValue(unsigned long* pValue);
	HRESULT SetOutputValue(unsigned long value);
	HRESULT SetOutputData(unsigned long triggerIndex, const SvTh::IntVariantMap& rData);
	HRESULT GetInputBit(unsigned long bitNum, bool& bitVal);
	HRESULT SetOutputBit(unsigned long bitNum, bool bitVal);
	
	HRESULT GetBoardVersion(long& p_rlVer);

	// Triggers
	unsigned long GetTriggerCount();
	unsigned long GetTriggerHandle(unsigned long triggerIndex);
	BSTR GetTriggerName(unsigned long triggerIndex);

	HRESULT TriggerGetParameterCount(unsigned long triggerIndex, unsigned long *pCount);
	HRESULT TriggerGetParameterName(unsigned long triggerIndex, unsigned long ulIndex, BSTR *pName);
	HRESULT TriggerGetParameterValue(unsigned long triggerIndex, unsigned long ulIndex, VARIANT *pValue);
	HRESULT TriggerSetParameterValue(unsigned long triggerIndex, unsigned long ulIndex, VARIANT *pValue);

	// Non-Trigger Parameter Functions
	HRESULT GetParameterCount(unsigned long *pulCount);
	HRESULT GetParameterName(unsigned long ulIndex, BSTR *pName);
	HRESULT GetParameterValue(unsigned long ulIndex, VARIANT *pValue);
	HRESULT SetParameterValue(unsigned long ulIndex, VARIANT *pValue);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void beforeStartTrigger(unsigned long triggerIndex) override;
	HRESULT afterStartTrigger(HRESULT hr) override;
	void beforeStopTrigger(unsigned long triggerIndex) override;

	void reportTrigger(const TriggerReport&);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	uint16_t m_plcSimulation {0};
	uint16_t m_plcTransferTime {0};
	uint16_t m_plcNodeID {0};
	long m_PlcVersion {0L};
	std::atomic_bool m_engineStarted {false};
	std::atomic_bool m_engineInitialized {false};
	std::atomic_bool m_triggerStarted[cMaxPlcTriggers]{false, false, false, false};
	std::atomic_int8_t m_currentTriggerChannel{-1};

	std::string m_OutputFileName;
#pragma endregion Member Variables
};

} //namespace SvPlc
