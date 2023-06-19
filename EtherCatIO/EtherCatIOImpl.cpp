//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file EthercatIOImpl.cpp
/// \brief Exported functions of the DLL
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EtherCatIOImpl.h"
#include "TriggerEngineConnection.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVStatusLibrary/GlobalPath.h"
#pragma endregion Includes

namespace SvEcat
{
#pragma region Declarations
constexpr unsigned long cInputCount = 8;
constexpr unsigned long cOutputCount = 16;
constexpr unsigned long cBuffSize = 255;
constexpr LPCTSTR cIniFile = _T("EtherCatIO.ini");
constexpr LPCTSTR cSettingsGroup = _T("Settings");
constexpr LPCTSTR cPLCSimulation = _T("PLCSimulation");
constexpr LPCTSTR cLogBaseFileName = _T("LogBaseFileName");
constexpr LPCTSTR cLogType = _T("LogType");
constexpr LPCTSTR cLogFilter = _T("LogFilter");
constexpr LPCTSTR cInputDataName = _T("_InputData.log");
constexpr LPCTSTR cEcatInputHeading = _T("Channel; Count; Trigger Timestamp; TimeStamp\n");
constexpr LPCTSTR cInputDataFormat = _T("{:d}; {:d}; {:f}; {:f}\n");
constexpr LPCTSTR cOutputDataName = _T("_OutputData.log");
constexpr LPCTSTR cEcatOutputHeading = _T("Timestamp; Output Nr.; state\n");
constexpr LPCTSTR cOutputDataFormat = _T("{:f}; {:d}; {:b}\n");
constexpr LPCTSTR cTriggerName = _T("HardwareTrigger.Dig_");			///This name must match the name in the SVHardwareManifest
#pragma endregion Declarations
EthercatIOImpl::EthercatIOImpl()
	: m_inLogger {boost::log::keywords::channel = cInputDataName}
	, m_outLogger {boost::log::keywords::channel = cOutputDataName}
{
}

HRESULT EthercatIOImpl::Initialize(bool bInit)
{
	HRESULT hr = S_OK;

	if (bInit)
	{
		std::lock_guard<std::mutex> guard(m_protectIO);
		TCHAR buffer[cBuffSize];
		memset(buffer, 0, cBuffSize);
		std::string iniFile = SvStl::GlobalPath::Inst().GetBinPath(cIniFile);
		::GetPrivateProfileString(cSettingsGroup, cLogBaseFileName, "", buffer, cBuffSize, iniFile.c_str());
		m_ecatInputParam.m_logFileName = buffer;
		memset(buffer, 0, cBuffSize);
		::GetPrivateProfileString(cSettingsGroup, cPLCSimulation, "", buffer, cBuffSize, iniFile.c_str());
		m_ecatInputParam.m_simulationFile = buffer;
		m_ecatInputParam.m_triggerType = m_ecatInputParam.m_simulationFile.empty() ? EcatTriggerType::HardwareTriggerEcat : EcatTriggerType::SimulatedTriggerEcat;
		m_ecatInputParam.m_logType = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cLogType, 0, iniFile.c_str()));
		m_ecatInputParam.m_logFilter = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cLogFilter, 0, iniFile.c_str()));

		//Check if logging should stay active!
		if (LogType::NoLogging != m_ecatInputParam.m_logType)
		{
			//@TODO[gra][10.21][02.12.2022]: This should be replaced by std::filesystem::last_write_time when file_time_type can be used
			HANDLE hFileIni = ::CreateFile(iniFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if (nullptr != hFileIni)
			{
				FILETIME fTime;
				::GetFileTime(hFileIni, nullptr, nullptr, &fTime);
				::CloseHandle(hFileIni);
				SYSTEMTIME sysTime;
				::FileTimeToSystemTime(&fTime, &sysTime);
				SYSTEMTIME nowTime;
				::GetSystemTime(&nowTime);
				if (sysTime.wDay != nowTime.wDay || sysTime.wMonth != nowTime.wMonth || sysTime.wYear != nowTime.wYear)
				{
					m_ecatInputParam.m_logType = LogType::NoLogging;
					::WritePrivateProfileString(cSettingsGroup, cLogType, _T("0"), iniFile.c_str());
				}
			}
		}
	}
	else
	{
		m_triggerCallbackMap.clear();
	}
	return hr;
}

unsigned long EthercatIOImpl::GetInputCount()
{
	///Required by SVObserver to initialize IO
	return cInputCount;
}

unsigned long EthercatIOImpl::GetOutputCount()
{
	///Required by SVObserver to initialize IO
	return cOutputCount;
}

bool EthercatIOImpl::GetInputBit(unsigned long inputNr) const
{
	if (inputNr <= cInputCount)
	{
		uint32_t inputValue = Tec::getInputs();
		uint32_t bitMask {1UL << inputNr};
		return (0 != (inputValue & bitMask));
	}
	return false;
}

unsigned long EthercatIOImpl::GetInputValue() const
{
	return Tec::getInputs();
}

HRESULT EthercatIOImpl::SetOutputBit(unsigned long outputNr, bool state)
{
	Tec::setOutput(static_cast<uint8_t> (outputNr), state);
	if (m_logOutFile.is_open())
	{
		std::string fileData = std::format(cOutputDataFormat, SvUl::GetTimeStamp(), outputNr, state);
		BOOST_LOG(m_outLogger) << fileData.c_str();
	}
	return S_OK;
}


unsigned long EthercatIOImpl::GetTriggerCount() const
{
	return cMaxEtherCatTriggers;
}

unsigned long EthercatIOImpl::GetTriggerHandle(unsigned long triggerIndex) const
{
	return (cMaxEtherCatTriggers > triggerIndex) ? (triggerIndex + 1) : 0;
}

_variant_t EthercatIOImpl::GetTriggerName(unsigned long triggerIndex) const
{
	_variant_t result;

	std::string triggerName{cTriggerName};
	triggerName += std::to_string(triggerIndex - 1);
	result.SetString(triggerName.c_str());
	return result;
}

void EthercatIOImpl::beforeStartTrigger(unsigned long triggerIndex)
{
	m_currentTriggerChannel = static_cast<int8_t> (triggerIndex - 1);

	if (false == m_engineInitialized)
	{
		if (LogType::EcatInData == (m_ecatInputParam.m_logType & LogType::EcatInData))
		{
			std::string fileName {m_ecatInputParam.m_logFileName + cInputDataName};
			if (nullptr != m_logInFile.open(fileName.c_str(), std::ios::out | std::ios::trunc))
			{
				boost::shared_ptr<std::ostream> stream = boost::make_shared<std::ostream>(&m_logInFile);
				m_pSink = boost::make_shared<text_sink>();
				m_pSink->locked_backend()->add_stream(stream);
				auto filterContent = [](const boost::log::attribute_value_set& rAttribSet) -> bool {return rAttribSet["Channel"].extract<std::string>() == cInputDataName; };
				m_pSink->set_filter(filterContent);
				boost::log::core::get()->add_sink(m_pSink);
				std::string fileData(cEcatInputHeading);
				BOOST_LOG(m_inLogger) << fileData.c_str();
			}
		}
		if (LogType::EcatOutData == (m_ecatInputParam.m_logType & LogType::EcatOutData))
		{
			std::string fileName {m_ecatInputParam.m_logFileName + cOutputDataName};
			if (nullptr != m_logOutFile.open(fileName.c_str(), std::ios::out | std::ios::trunc))
			{
				boost::shared_ptr<std::ostream> stream = boost::make_shared<std::ostream>(&m_logOutFile);
				if (nullptr == m_pSink)
				{
					m_pSink = boost::make_shared<text_sink>();
				}
				m_pSink->locked_backend()->add_stream(stream);
				auto filterContent = [](const boost::log::attribute_value_set& rAttribSet) -> bool {return rAttribSet["Channel"].extract<std::string>() == cOutputDataName; };
				m_pSink->set_filter(filterContent);
				boost::log::core::get()->add_sink(m_pSink);
				std::string fileData(cEcatOutputHeading);
				BOOST_LOG(m_outLogger) << fileData.c_str();
			}
		}

		m_ecatInputParam.m_pTriggerDataCallBack = [this](const SvTrig::TriggerData& rTriggerData) { return NotifyTriggerData(rTriggerData); };
		Tec::startTriggerEngine(m_ecatInputParam);
		m_engineStarted = true;
		Tec::setReady(m_moduleReady);
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
}

HRESULT EthercatIOImpl::afterStartTrigger()
{
	HRESULT result{ S_OK };

	if (false == m_engineInitialized)
	{
		result = Tec::initTriggerEngine();
		m_engineInitialized = true;
	}
	if(S_OK == result)
	{
		if(0 <= m_currentTriggerChannel && cMaxEtherCatTriggers > m_currentTriggerChannel)
		{
			if (false == m_triggerStarted[m_currentTriggerChannel])
			{
				Tec::setTriggerChannel(static_cast<uint8_t> (m_currentTriggerChannel), true);
				m_triggerStarted[m_currentTriggerChannel] = true;
			}
		}
	}
	else
	{
		///Clean up all triggers and engine
		for(uint8_t i=0; i < cMaxEtherCatTriggers; ++i)
		{
			if (m_triggerStarted[i])
			{
				m_triggerStarted[i] = false;
				//Trigger Engine trigger channel is zero based while SVObserver is one based!
				Tec::setTriggerChannel(i, false);
			}
		}
		m_engineInitialized = false;
	}
	return result;
}

void EthercatIOImpl::beforeStopTrigger(unsigned long triggerIndex)
{
	int triggerChannel = triggerIndex - 1;
	if (0 <= triggerChannel && cMaxEtherCatTriggers > triggerChannel)
	{
		if (m_triggerStarted[triggerChannel])
		{
			m_triggerStarted[triggerChannel] = false;
			//Trigger Engine trigger channel is zero based while SVObserver is one based!
			Tec::setTriggerChannel(static_cast<uint8_t> (triggerChannel), false);
		}
		//Still some active trigger
		bool activeTrigger {std::any_of(m_triggerStarted.begin(), m_triggerStarted.end(), [](const auto& rActive)
		{
			return rActive.load();
		})};
		if(false == activeTrigger)
		{
			Tec::stopTriggerEngine();
			m_engineStarted = false;
			m_engineInitialized = false;
			::OutputDebugString("Irq Enabled reset\n");
			::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);

			if (nullptr != m_pSink)
			{
				m_pSink->flush();
				m_pSink->stop();
				boost::log::core::get()->remove_sink(m_pSink);
			}
			if (m_logInFile.is_open())
			{
				m_logInFile.close();
			}
			if (m_logOutFile.is_open())
			{
				m_logOutFile.close();
			}
		}
	}
}

unsigned long EthercatIOImpl::TriggerGetParameterCount(unsigned long) const
{
	return 1UL;
}

_variant_t EthercatIOImpl::TriggerGetParameterName(unsigned long , unsigned long index) const
{
	_variant_t result;

	switch (index)
	{
		case SVBoardVersion:
		{
			result.SetString(_T("Board Version"));
			break;
		}

		default:
			break;
	}
	return result;
}

_variant_t EthercatIOImpl::TriggerGetParameterValue(unsigned long , unsigned long index) const
{
	_variant_t result;

	if (index == SVBoardVersion)
	{
		result.SetString(std::format("PLC Version {:.2} ", 1.0).c_str());
	}
	return result;
}

HRESULT EthercatIOImpl::TriggerSetParameterValue(unsigned long, unsigned long index, const _variant_t& rValue)
{
	HRESULT result{ E_NOTIMPL };

	if (SVIOParameterEnum::PlcSimulatedTrigger == index && VT_BSTR == rValue.vt)
	{
		m_ecatInputParam.m_simulationFile = _bstr_t(rValue.bstrVal);
		m_ecatInputParam.m_triggerType = m_ecatInputParam.m_simulationFile.empty() ? EcatTriggerType::HardwareTriggerEcat : EcatTriggerType::SimulatedTriggerEcat;
		result = S_OK;
	}

	return result;
}

unsigned long EthercatIOImpl::GetParameterCount() const
{
	return 1UL;
}

_variant_t EthercatIOImpl::GetParameterName(unsigned long index) const
{
	_variant_t result;

	if (SVBoardVersion == index)
	{
		result.SetString(_T("Board Version(R)"));
	}
	return result;
}

_variant_t EthercatIOImpl::GetParameterValue(unsigned long index) const
{
	_variant_t result;

	if (index == SVBoardVersion)
	{
		result.SetString(std::format("Ethercat Version {:.2} ", 1.0).c_str());
	}
	return result;
}

HRESULT EthercatIOImpl::SetParameterValue(unsigned long index, const _variant_t& rValue)
{
	HRESULT result{E_FAIL};

	if(SVModuleReady == index && VT_BOOL == rValue.vt)
	{
		m_moduleReady = rValue.boolVal ? true : false;
		Tec::setReady(m_moduleReady);
		::OutputDebugString(std::format("Module ready = {}\n", m_moduleReady ? "True" : "False").c_str());
	}

	return result;
}

void EthercatIOImpl::NotifyTriggerData(const SvTrig::TriggerData& rTriggerData)
{

	if(cMaxEtherCatTriggers > rTriggerData.m_channel)
	{
		if(false == m_triggerStarted[rTriggerData.m_channel])
		{
			return;
		}
		++m_inputCount[rTriggerData.m_channel];
	}
	
	/// Only call trigger callbacks if the module ready is set this avoids problems with the PPQ Object not being ready
	if(m_moduleReady)
	{
		//PLC channel is zero based while SVObserver trigger index is one based!
		unsigned long triggerIndex = rTriggerData.m_channel + 1;

		SvTrig::TriggerData triggerData {rTriggerData};

		auto iter = m_triggerCallbackMap.find(triggerIndex);
		if (m_triggerCallbackMap.end() != iter)
		{
			iter->second(std::move(triggerData));
		}

		if (m_logInFile.is_open() && cMaxEtherCatTriggers > rTriggerData.m_channel)
		{
			const SvTrig::TriggerData& rData = rTriggerData;
			std::string fileData = std::format(cInputDataFormat, triggerIndex, m_inputCount[rData.m_channel], rData.m_triggerTimestamp, SvUl::GetTimeStamp());
			BOOST_LOG(m_inLogger) << fileData.c_str();
		}
	}
}
} //namespace SvEcat
