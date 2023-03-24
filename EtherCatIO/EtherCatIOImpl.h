//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file EthercatIOImpl.h
/// \brief Exported functions of the DLL
//******************************************************************************

#pragma once

#pragma region Includes
#include "EtherCatDataTypes.h"
#include "Triggering/IODeviceBase.h"
#pragma endregion Includes

namespace SvEcat
{

#pragma region Declarations
constexpr unsigned long cMaxEtherCatTriggers = 4;

struct TriggerReport;
#pragma endregion Declarations

class EthercatIOImpl : public SvTrig::IODeviceBase
{
#pragma region Constructor
public:
	EthercatIOImpl();
	virtual ~EthercatIOImpl() = default;
#pragma endregion Constructor

#pragma region Public Methods
public:
	HRESULT Initialize(bool bInit);
	
	// Digital I/O
	unsigned long GetInputCount();
	unsigned long GetOutputCount();

	bool GetInputBit(unsigned long inputNr) const;
	unsigned long GetInputValue() const;
	HRESULT SetOutputBit(unsigned long outputNr, bool state);

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

	void NotifyTriggerData(const SvTrig::TriggerData& rTriggerData);
#pragma endregion Private Methods

#pragma region Member Variables
private:
	EcatInputParam m_ecatInputParam;

	std::atomic_bool m_engineStarted {false};
	std::atomic_bool m_engineInitialized {false};
	std::array<std::atomic_bool, cMaxEtherCatTriggers> m_triggerStarted{false, false, false, false};
	std::array<uint32_t, cMaxEtherCatTriggers> m_inputCount {0UL, 0UL, 0UL, 0UL};
	std::array<uint32_t, cMaxEtherCatTriggers> m_outputCount {0UL, 0UL, 0UL, 0UL};
	int8_t m_currentTriggerChannel{-1};

	std::filebuf m_logInFile;
	std::filebuf m_logOutFile;
	typedef boost::log::sinks::asynchronous_sink<boost::log::sinks::text_ostream_backend> text_sink;
	boost::shared_ptr<text_sink> m_pSink {nullptr};
	boost::log::sources::channel_logger_mt<std::string> m_inLogger;
	boost::log::sources::channel_logger_mt<std::string> m_outLogger;
#pragma endregion Member Variables
};

} //namespace SvEcat
