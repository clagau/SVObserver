//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file SVPlcIOImpl.cpp
/// \brief Exported functions of the DLL
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPlcIOImpl.h"
#include "TriggerEngineConnection.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "Triggering/ResultData.h"
#include "Triggering/TriggerData.h"
#pragma endregion Includes

namespace SvPlc
{
#pragma region Declarations
constexpr unsigned long cInputCount = 0;
constexpr unsigned long cOutputCount = 14;
constexpr unsigned long cBuffSize = 255;
constexpr LPCTSTR cIniFile = _T("SVPLCIO.ini");
constexpr LPCTSTR cSettingsGroup = _T("Settings");
constexpr LPCTSTR cPLCSimulation = _T("PLCSimulation");
constexpr LPCTSTR cPLCTransferTime = _T("PLC-TransferTime");
constexpr LPCTSTR cPD0Version = _T("PD0Version");
constexpr LPCTSTR cPLCNodeID = _T("PLC-NodeID");
constexpr LPCTSTR cLogBaseFileName = _T("LogBaseFileName");
constexpr LPCTSTR cLogType = _T("LogType");
constexpr LPCTSTR cPlcInputName = _T("_PlcInput.log");
constexpr LPCTSTR cPlcOutputName = _T("_PlcOutput.log");
constexpr LPCTSTR cPlcInputHeading = _T("Channel; Count; Trigger Timestamp; ObjectID; Trigger Index; Trigger per ObjectID; TimeStamp\r\n");
constexpr LPCTSTR cPlcOutputHeading = _T("Channel; Count; Timestamp; ObjectID; Results\r\n");
constexpr LPCTSTR cTriggerName = _T("HardwareTrigger.Dig_");			///This name must match the name in the SVHardwareManifest
#pragma endregion Declarations

HRESULT SVPlcIOImpl::Initialize(bool bInit)
{
	HRESULT hr = S_OK;

	///This is a map which defines the PLC node ID depending on the SVIM computer name
	static const std::map<std::string, uint32_t> cComputerNameNodeID = 
	{
		{std::string {"SVIM01"}, 11u},
		{std::string {"SVIM02"}, 12u},
		{std::string {"SVIM03"}, 13u},
		{std::string {"SVIM04"}, 14u},
		{std::string {"SVIM05"}, 21u},
		{std::string {"SVIM06"}, 22u},
		{std::string {"SVIM07"}, 23u},
		{std::string {"SVIM08"}, 24u},
		{std::string {"SVIM09"}, 31u},
		{std::string {"SVIM10"}, 32u},
		{std::string {"SVIM11"}, 33u},
		{std::string {"SVIM12"}, 34u},
	};

	if (bInit)
	{
		std::lock_guard<std::mutex> guard(m_protectIO);
		TCHAR buffer[cBuffSize];
		memset(buffer, 0, cBuffSize);
		std::string iniFile = SvStl::GlobalPath::Inst().GetBinPath(cIniFile);
		::GetPrivateProfileString(cSettingsGroup, cLogBaseFileName, "", buffer, cBuffSize, iniFile.c_str());
		m_plcInputParam.m_logFileName = buffer;
		memset(buffer, 0, cBuffSize);
		::GetPrivateProfileString(cSettingsGroup, cPLCSimulation, "", buffer, cBuffSize, iniFile.c_str());
		m_plcInputParam.m_simulationFile = buffer;
		m_plcInputParam.m_triggerType = m_plcInputParam.m_simulationFile.empty() ? TriggerType::HardwareTrigger : TriggerType::SimulatedTrigger;
		memset(buffer, 0, cBuffSize);
		::GetPrivateProfileString(cSettingsGroup, cPD0Version, "", buffer, cBuffSize, iniFile.c_str());
		m_plcInputParam.m_PD0Version = buffer;
		m_plcInputParam.m_plcTransferTime = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cPLCTransferTime, 0, iniFile.c_str()));
		m_plcInputParam.m_plcNodeID = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cPLCNodeID, 0, iniFile.c_str()));
		m_plcInputParam.m_logType = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cLogType, 0, iniFile.c_str()));
		//Check if logging should stay active!
		if (LogType::NoLogging != m_plcInputParam.m_logType)
		{
			//@TODO[gra][10.21][02.12.2022]: This should be replaced by std::filesystem::last_write_time when file_time_type can be used
			HANDLE hFileIni = ::CreateFile(iniFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
			if(nullptr != hFileIni)
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
					m_plcInputParam.m_logType = LogType::NoLogging;
					::WritePrivateProfileString(cSettingsGroup, cLogType, _T("0"), iniFile.c_str());
				}
			}
		}
		///If no setting then try to derive the PLC node ID from the SVIM computer name
		if(0 == m_plcInputParam.m_plcNodeID)
		{
			DWORD size{cBuffSize};
			memset(buffer, 0, cBuffSize);
			if(::GetComputerName(buffer, &size))
			{
				std::string computerName{buffer};
				auto iter = cComputerNameNodeID.find(computerName);
				if(cComputerNameNodeID.end() != iter)
				{
					m_plcInputParam.m_plcNodeID = static_cast<uint16_t> (iter->second);
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

unsigned long SVPlcIOImpl::GetInputCount()
{
	///Required by SVObserver to initialize IO
	return cInputCount;
}

unsigned long SVPlcIOImpl::GetOutputCount()
{
	///Required by SVObserver to initialize IO
	return cOutputCount;
}

HRESULT SVPlcIOImpl::SetOutputData(const SvTrig::ResultData& rResultData)
{
	Tec::writeResult(rResultData);

	if (m_logOutFile.is_open() && cMaxPlcTriggers > rResultData.m_channel)
	{
		uint32_t outputCount = ++m_outputCount[rResultData.m_channel];
		std::string resultString;
		for (auto& rResult : rResultData.m_results)
		{
			resultString += std::to_string(rResult) + ' ';
		}
		std::string fileData = std::format(_T("{}; {}; {}; {}; {}\r\n"), rResultData.m_channel+1, outputCount, SvUl::GetTimeStamp(), rResultData.m_objectID, resultString.c_str());
		m_logOutFile.write(fileData.c_str(), fileData.size());
	}
	return S_OK;
}

unsigned long SVPlcIOImpl::GetTriggerCount() const
{
	return cMaxPlcTriggers;
}

unsigned long SVPlcIOImpl::GetTriggerHandle(unsigned long triggerIndex) const
{
	return (cMaxPlcTriggers > triggerIndex) ? (triggerIndex + 1) : 0;
}

_variant_t SVPlcIOImpl::GetTriggerName(unsigned long triggerIndex) const
{
	_variant_t result;

	std::string triggerName{cTriggerName};
	triggerName += std::to_string(triggerIndex - 1);
	result.SetString(triggerName.c_str());
	return result;
}

void SVPlcIOImpl::beforeStartTrigger(unsigned long triggerIndex)
{
	m_currentTriggerChannel = static_cast<int8_t> (triggerIndex - 1);

	if (false == m_engineStarted)
	{
		if (LogType::PlcIn == (m_plcInputParam.m_logType & LogType::PlcIn))
		{
			std::string fileName {m_plcInputParam.m_logFileName};
			fileName += cPlcInputName;
			m_logInFile.open(fileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
			if (m_logInFile.is_open())
			{
				std::string fileData(cPlcInputHeading);
				m_logInFile.write(fileData.c_str(), fileData.size());
			}
		}
		if (LogType::PlcOut == (m_plcInputParam.m_logType & LogType::PlcOut))
		{
			std::string fileName {m_plcInputParam.m_logFileName};
			fileName += cPlcOutputName;
			m_logOutFile.open(fileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
			if (m_logOutFile.is_open())
			{
				std::string fileData(cPlcOutputHeading);
				m_logOutFile.write(fileData.c_str(), fileData.size());
			}
		}
		m_plcInputParam.m_pTriggerDataCallBack = [this](const SvTrig::TriggerData& rTriggerData) { return NotifyTriggerData(rTriggerData); };
		Tec::startTriggerEngine(m_plcInputParam);
		Tec::setReady(m_moduleReady);
		m_engineStarted = true;
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
}

HRESULT SVPlcIOImpl::afterStartTrigger()
{
	HRESULT result{ S_OK };

	if (false == m_engineInitialized)
	{
		result = Tec::initTriggerEngine();
		m_engineInitialized = true;
	}
	if(S_OK == result)
	{
		if(0 <= m_currentTriggerChannel && cMaxPlcTriggers > m_currentTriggerChannel)
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
		for(uint8_t i=0; i < cMaxPlcTriggers; ++i)
		{
			if (m_triggerStarted[i])
			{
				m_triggerStarted[i] = false;
				//Trigger Engine trigger channel is zero based while SVObserver is one based!
				Tec::setTriggerChannel(i, false);
			}
		}
		Tec::stopTriggerEngine();
		m_engineStarted = false;
		m_engineInitialized = false;
	}
	return result;
}

void SVPlcIOImpl::beforeStopTrigger(unsigned long triggerIndex)
{
	int triggerChannel = triggerIndex - 1;
	if (0 <= triggerChannel && cMaxPlcTriggers > triggerChannel)
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

			if(m_logInFile.is_open())
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

unsigned long SVPlcIOImpl::TriggerGetParameterCount(unsigned long) const
{
	return 1UL;
}

_variant_t SVPlcIOImpl::TriggerGetParameterName(unsigned long , unsigned long index) const
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

_variant_t SVPlcIOImpl::TriggerGetParameterValue(unsigned long , unsigned long index) const
{
	_variant_t result;

	if (index == SVBoardVersion)
	{
		result.SetString(std::format("PLC Version {:.2f} ", 1.0).c_str());
	}
	return result;
}

HRESULT SVPlcIOImpl::TriggerSetParameterValue(unsigned long, unsigned long index, const _variant_t& rValue)
{
	HRESULT result{ E_NOTIMPL };

	if (SVIOParameterEnum::PlcSimulatedTrigger == index && VT_BSTR == rValue.vt)
	{
		m_plcInputParam.m_simulationFile = _bstr_t(rValue.bstrVal);
		m_plcInputParam.m_triggerType = m_plcInputParam.m_simulationFile.empty() ? TriggerType::HardwareTrigger : TriggerType::SimulatedTrigger;
		result = S_OK;
	}

	return result;
}

unsigned long SVPlcIOImpl::GetParameterCount() const
{
	return 1UL;
}

_variant_t SVPlcIOImpl::GetParameterName(unsigned long index) const
{
	_variant_t result;

	if (SVBoardVersion == index)
	{
		result.SetString(_T("Board Version(R)"));
	}
	return result;
}

_variant_t SVPlcIOImpl::GetParameterValue(unsigned long index) const
{
	_variant_t result;

	if (index == SVBoardVersion)
	{
		result.SetString(std::format("PLC Version {:.2f} ", 1.3).c_str());
	}
	return result;
}

HRESULT SVPlcIOImpl::SetParameterValue(unsigned long index, const _variant_t& rValue)
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

void SVPlcIOImpl::NotifyTriggerData(const SvTrig::TriggerData& rTriggerData)
{

	if(cMaxPlcTriggers > rTriggerData.m_channel)
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

		auto iter = m_triggerCallbackMap.find(triggerIndex);
		if (m_triggerCallbackMap.end() != iter)
		{
			SvTrig::TriggerData triggerData = rTriggerData;
			iter->second(std::move(triggerData));
		}

		if(m_logInFile.is_open() && cMaxPlcTriggers > rTriggerData.m_channel)
		{
			const SvTrig::TriggerData& rData = rTriggerData;
			std::string objectIDList;
			for (int i = 0; i < cObjectMaxNr; ++i)
			{
				objectIDList += (0 == i) ? "" : ",";
				objectIDList += std::to_string(rData.m_objectData[i].m_objectID);
			}
			///This is required as m_inputCount[rData.m_channel] is atomic
			uint32_t inputCount = m_inputCount[rData.m_channel];
			std::string fileData = std::format(_T("{}; {}; {}; {}; {:d}; {:d}; {}\r\n"), triggerIndex, inputCount, rData.m_triggerTimestamp, objectIDList.c_str(), rData.m_triggerIndex, rData.m_triggerPerObjectID, SvUl::GetTimeStamp());
			m_logInFile.write(fileData.c_str(), fileData.size());
		}
	}
}
} //namespace SvPlc
