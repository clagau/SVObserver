//*****************************************************************************
/// \copyright COPYRIGHT (c) 2017,2017 by Körber Pharma Inspection GmbH. All Rights Reserved
/// All Rights Reserved
/// \file EthercatIOImpl.cpp
/// \brief Exported functions of the DLL
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "EtherCatIOImpl.h"
#include "PlcDataTypes.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"
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
constexpr LPCTSTR cOutputFileName = _T("OutputFileName");
constexpr LPCTSTR cPlcInputName = _T("_EtherCatInput.txt");
constexpr LPCTSTR cPlcOutputName = _T("__EtherCatOutput.txt");
constexpr LPCTSTR cPlcInputHeading = _T("Channel; Count; Trigger Timestamp; ObjectID; Trigger Index; Trigger per ObjectID; TimeStamp\r\n");
constexpr LPCTSTR cPlcOutputHeading = _T("Channel; Count; Timestamp; ObjectID; Results\r\n");
constexpr LPCTSTR cTriggerName = _T("IO_Board_1.Dig_");			///This name must match the name in the SVHardwareManifest
#pragma endregion Declarations

HRESULT EthercatIOImpl::Initialize(bool bInit)
{
	HRESULT hr = S_OK;

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
		m_AdditionalData = buffer;
		m_triggerType = m_AdditionalData.empty() ? TriggerType::HardwareTrigger : TriggerType::SimulatedTrigger;
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
	return S_OK;
}


unsigned long EthercatIOImpl::GetTriggerCount() const
{
	return cMaxPlcTriggers;
}

unsigned long EthercatIOImpl::GetTriggerHandle(unsigned long triggerIndex) const
{
	return (cMaxPlcTriggers > triggerIndex) ? (triggerIndex + 1) : 0;
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
		auto reportTriggerCallback = [this](const TriggerReport& rTriggerReport) { return reportTrigger(rTriggerReport); };
		Tec::startTriggerEngine(reportTriggerCallback, m_triggerType, m_AdditionalData);
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
		m_engineInitialized = false;
	}
	return result;
}

void EthercatIOImpl::beforeStopTrigger(unsigned long triggerIndex)
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
		result.SetString(SvUl::Format("PLC Version %.2f ", 1.0).c_str());
	}
	return result;
}

HRESULT EthercatIOImpl::TriggerSetParameterValue(unsigned long, unsigned long index, const _variant_t& rValue)
{
	HRESULT result{ E_NOTIMPL };

	if (SVIOParameterEnum::PlcSimulatedTrigger == index && VT_BSTR == rValue.vt)
	{
		m_AdditionalData = _bstr_t(rValue.bstrVal);
		m_triggerType = m_AdditionalData.empty() ? TriggerType::HardwareTrigger : TriggerType::SimulatedTrigger;
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
		result.SetString(SvUl::Format("PLC Version %.2f ", 1.3).c_str());
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
		::OutputDebugString(SvUl::Format("Module ready = %s\n", m_moduleReady ? "True" : "False").c_str());
	}

	return result;
}

void EthercatIOImpl::reportTrigger(const TriggerReport& rTriggerReport)
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

		SvTrig::TriggerData triggerData;
		triggerData[SvTrig::TriggerDataEnum::TimeStamp] = _variant_t(rTriggerReport.m_triggerTimestamp);
		triggerData[SvTrig::TriggerDataEnum::TriggerChannel] = _variant_t(rTriggerReport.m_channel);
		triggerData[SvTrig::TriggerDataEnum::ObjectType] = _variant_t(rTriggerReport.m_objectType);
		triggerData[SvTrig::TriggerDataEnum::ObjectID] = _variant_t(rTriggerReport.m_objectID);
		triggerData[SvTrig::TriggerDataEnum::TriggerIndex] = _variant_t(rTriggerReport.m_triggerIndex);
		triggerData[SvTrig::TriggerDataEnum::TriggerPerObjectID] = _variant_t(rTriggerReport.m_triggerPerObjectID);
		triggerData[SvTrig::TriggerDataEnum::AcquisitionFile].SetString(rTriggerReport.m_acquisitionFile.c_str());

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
			std::string fileData = SvUl::Format(_T("%lu; %u; %f; %u; %hhu; %hhu; %f\r\n"), triggerIndex, inputCount, rData.m_triggerTimestamp, rData.m_objectID, rData.m_triggerIndex, rData.m_triggerPerObjectID, SvUl::GetTimeStamp());
			m_logInFile.write(fileData.c_str(), fileData.size());
		}
	}
}
} //namespace SvEcat
