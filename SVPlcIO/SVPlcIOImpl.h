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
constexpr unsigned long cNumberTriggers = 4;

struct TriggerReport;
class SVPlcIOImpl;
extern SVPlcIOImpl g_Plc;
#pragma endregion Declarations

struct RecordData
{
	RecordData(uint8_t channel, uint32_t count, double timeStamp, uint32_t objectID, bool good) :
		m_channel {channel}, m_count {count}, m_timeStamp {timeStamp}, m_objectID {objectID}, m_good{good}
	{
	}

	uint8_t m_channel {0};
	uint32_t m_count {0UL};
	uint32_t m_objectID {0UL};
	double m_timeStamp {0.0};
	bool m_good {false};
};

struct TriggerParameter
{
	uint32_t m_ObjectID {0};
	std::atomic_bool m_started{false};
	long m_conditionIndex {-1L};
	long m_dataValidIndex {-1L};
	long m_objectGoodIndex {-1L};
	unsigned long m_period {0UL};
	std::thread m_timer;
};


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
	HRESULT SetOutputData(unsigned long triggerChannel, const SvTh::IntVariantMap& rData);
	HRESULT GetInputBit(unsigned long bitNum, bool& bitVal);
	HRESULT SetOutputBit(unsigned long bitNum, bool bitVal);
	
	HRESULT GetBoardVersion(long& p_rlVer);

	// Triggers
	unsigned long GetTriggerCount();
	unsigned long GetTriggerHandle(unsigned long triggerIndex);
	BSTR GetTriggerName(unsigned long triggerChannel);

	void beforeStartTrigger(unsigned long) override;
	void beforeStopTrigger(unsigned long) override;

	HRESULT TriggerGetParameterCount(unsigned long triggerChannel, unsigned long *pCount);
	HRESULT TriggerGetParameterName(unsigned long triggerChannel, unsigned long ulIndex, BSTR *pName);
	HRESULT TriggerGetParameterValue(unsigned long triggerChannel, unsigned long ulIndex, VARIANT *pValue);
	HRESULT TriggerSetParameterValue(unsigned long triggerChannel, unsigned long ulIndex, VARIANT *pValue);

	// Non-Trigger Parameter Functions
	HRESULT GetParameterCount(unsigned long *pulCount);
	HRESULT GetParameterName(unsigned long ulIndex, BSTR *pName);
	HRESULT GetParameterValue(unsigned long ulIndex, VARIANT *pValue);
	HRESULT SetParameterValue(unsigned long ulIndex, VARIANT *pValue);
#pragma endregion Public Methods

#pragma region Private Methods
private:
	void WriteResult(int triggerChannel);
	void reportTrigger(const TriggerReport&);


#pragma endregion Private Methods

#pragma region Member Variables
private:
	uint16_t m_Input {0};
	uint16_t m_Output {0};
	bool m_isInitialized {false};
	bool m_delayedReportTrigger {false};
	uint16_t m_plcSimulation {0};
	uint16_t m_plcTransferTime {0};
	int m_readyBit {0};
	long m_PlcVersion {0L};
	bool m_engineStarted {false};
	TriggerParameter m_trigger[cNumberTriggers];

	std::mutex m_protectPlc;

	//@TODO[gra][8.20][08.04.2019]: Needs to be removed after LPT functionality no longer in PLC required (for timing purposes)
	bool m_isLptActive {false};
	long m_resetOutputTime{0L};
	std::string m_OutputFileName;
	std::vector<std::string> m_OutputData;
	//@TODO[gra][8.20][08.04.2019]
#pragma endregion Member Variables
};

} //namespace SvPlc
