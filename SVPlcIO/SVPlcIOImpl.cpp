//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMPL
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVPlcIOImpl.h"
#include "PlcDataTypes.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "TriggerEngineConnection.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
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
constexpr LPCTSTR cPLCNodeID = _T("PLC-NodeID");
constexpr LPCTSTR cOutputFileName = _T("OutputFileName");
constexpr LPCTSTR cPlcInputName = _T("_PlcInput.txt");
constexpr LPCTSTR cPlcOutputName = _T("_PlcOutput.txt");
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
		::GetPrivateProfileString(cSettingsGroup, cOutputFileName, "", buffer, cBuffSize, iniFile.c_str());
		m_logFileName = buffer;
		memset(buffer, 0, cBuffSize);
		::GetPrivateProfileString(cSettingsGroup, cPLCSimulation, "", buffer, cBuffSize, iniFile.c_str());
		m_plcSimulateFile = buffer;
		m_plcTransferTime = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cPLCTransferTime, 0, iniFile.c_str()));
		m_plcNodeID = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cPLCNodeID, 0, iniFile.c_str()));
		///If no setting then try to derive the PLC node ID from the SVIM computer name
		if(0 == m_plcNodeID)
		{
			DWORD size{cBuffSize};
			memset(buffer, 0, cBuffSize);
			if(::GetComputerName(buffer, &size))
			{
				std::string computerName{buffer};
				auto iter = cComputerNameNodeID.find(computerName);
				if(cComputerNameNodeID.end() != iter)
				{
					m_plcNodeID = static_cast<uint16_t> (iter->second);
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

HRESULT SVPlcIOImpl::SetOutputData(unsigned long triggerIndex, const SvTi::IntVariantMap& rData)
{
	ResultReport reportResult;

	//PLC channel is zero based while SVObserver trigger index is one based!
	reportResult.m_channel = static_cast<uint8_t> (triggerIndex - 1);
	reportResult.m_timestamp = SvTl::GetTimeStamp();
	auto iterData = rData.find(SvTi::TriggerDataEnum::ObjectID);
	if (rData.end() != iterData)
	{
		reportResult.m_currentObjectID = iterData->second;
	}
	iterData = rData.find(SvTi::TriggerDataEnum::OutputData);
	if (rData.end() != iterData)
	{
		const _variant_t& rResult{iterData->second};
		if((VT_UI1 | VT_ARRAY) ==  rResult.vt && rResult.parray->rgsabound[0].cElements == cResultSize)
		{
			memcpy(&reportResult.m_results[0], rResult.parray->pvData, cResultSize * sizeof(uint8_t));
		}
	}

	Tec::writeResult(reportResult);

	if (m_logOutFile.is_open() && cMaxPlcTriggers > reportResult.m_channel)
	{
		uint32_t outputCount = ++m_outputCount[reportResult.m_channel];
		std::string resultString;
		for (auto& rResult : reportResult.m_results)
		{
			resultString += std::to_string(rResult) + ' ';
		}
		std::string fileData = SvUl::Format(_T("%d; %d; %f; %d; %s\r\n"), triggerIndex, outputCount, reportResult.m_timestamp, reportResult.m_currentObjectID, resultString.c_str());
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
		if(false == m_logFileName.empty())
		{
			std::string fileName = m_logFileName;
			fileName += cPlcInputName;
			m_logInFile.open(fileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
			if(m_logInFile.is_open())
			{
				std::string fileData(cPlcInputHeading);
				m_logInFile.write(fileData.c_str(), fileData.size());
			}
			fileName = m_logFileName;
			fileName += cPlcOutputName;
			m_logOutFile.open(fileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
			if (m_logOutFile.is_open())
			{
				std::string fileData(cPlcOutputHeading);
				m_logOutFile.write(fileData.c_str(), fileData.size());
			}
		}
		Tec::startTriggerEngine(std::bind(&SVPlcIOImpl::reportTrigger, this, std::placeholders::_1), m_plcNodeID, m_plcTransferTime, m_plcSimulateFile);
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
		bool activeTrigger{false};
		for (const auto& rStarted : m_triggerStarted)
		{
			activeTrigger |= rStarted;
		}
		if(!activeTrigger)
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
		result.SetString(SvUl::Format("PLC Version %.2f ", 1.0).c_str());
	}
	return result;
}

HRESULT SVPlcIOImpl::TriggerSetParameterValue(unsigned long, unsigned long index, const _variant_t& rValue)
{
	HRESULT result{ E_NOTIMPL };

	if (SVIOParameterEnum::PlcSimulatedTrigger == index && VT_BSTR == rValue.vt)
	{
		m_plcSimulateFile = _bstr_t(rValue.bstrVal);
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
		result.SetString(SvUl::Format("PLC Version %.2f ", 1.0).c_str());
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
		::OutputDebugString(SvUl::Format("Module ready = %s\n", m_moduleReady ? "True" : "False").c_str());
	}

	return result;
}

void SVPlcIOImpl::reportTrigger(const TriggerReport& rTriggerReport)
{

	if(cMaxPlcTriggers > rTriggerReport.m_channel)
	{
		if(false == m_triggerStarted[rTriggerReport.m_channel] || false == rTriggerReport.m_isValid)
		{
			return;
		}
		++m_inputCount[rTriggerReport.m_channel];
	}
	

	/// Only call trigger callbacks if the module ready is set this avoids problems with the PPQ Object not being ready
	if(m_moduleReady)
	{
		//PLC channel is zero based while SVObserver trigger index is one based!
		unsigned long triggerIndex = rTriggerReport.m_channel + 1;

		SvTi::IntVariantMap triggerData;
		triggerData[SvTi::TriggerDataEnum::TimeStamp] = _variant_t(rTriggerReport.m_triggerTimestamp);
		triggerData[SvTi::TriggerDataEnum::TriggerChannel] = _variant_t(rTriggerReport.m_channel);
		triggerData[SvTi::TriggerDataEnum::ObjectID] = _variant_t(rTriggerReport.m_currentObjectID);
		triggerData[SvTi::TriggerDataEnum::TriggerIndex] = _variant_t(rTriggerReport.m_triggerIndex);
		triggerData[SvTi::TriggerDataEnum::TriggerPerObjectID] = _variant_t(rTriggerReport.m_triggerPerObjectID);

		auto iter = m_triggerCallbackMap.find(triggerIndex);
		if (m_triggerCallbackMap.end() != iter)
		{
			iter->second(std::move(triggerData));
		}

		if(m_logInFile.is_open() && cMaxPlcTriggers > rTriggerReport.m_channel)
		{
			const TriggerReport& rData = rTriggerReport;
			///This is required as m_inputCount[rData.m_channel] is atomic
			uint32_t inputCount = m_inputCount[rData.m_channel];
			std::string fileData = SvUl::Format(_T("%d; %d; %f; %d; %d; %d; %f\r\n"), triggerIndex, inputCount, rData.m_triggerTimestamp, rData.m_currentObjectID, rData.m_triggerIndex, rData.m_triggerPerObjectID, SvTl::GetTimeStamp());
			m_logInFile.write(fileData.c_str(), fileData.size());
		}
	}
}
} //namespace SvPlc
