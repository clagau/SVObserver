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

std::vector<TriggerReport> g_PlcListInput;
std::vector<ResultReport> g_PlcListOutput;

#pragma region Declarations
constexpr unsigned long cInputCount = 0;
constexpr unsigned long cOutputCount = 14;
constexpr LPCTSTR cIniFile = "SVPLCIO.ini";
constexpr LPCTSTR cSettingsGroup = "Settings";
constexpr LPCTSTR cPLCSimulation = "PLCSimulation";
constexpr LPCTSTR cPLCTransferTime = "PLC-TransferTime";
constexpr LPCTSTR cPLCNodeID = "PLC-NodeID";
constexpr LPCTSTR cOutputFileName = "OutputFileName";
constexpr LPCTSTR cTriggerName = "HardwareTrigger.Dig_";			///This name must match the name in the SVHardwareManifest
#pragma endregion Declarations

void triggerDispatcher(SvTh::IntVariantMap&& triggerData, SvTh::DispatcherVector&& dispatchVector)
{
	for (auto& rDispatcher : dispatchVector)
	{
		if (rDispatcher.m_IsStarted)
		{
			rDispatcher.SetData(triggerData);
			rDispatcher.Dispatch();
		}
	}
}

HRESULT SVPlcIOImpl::Initialize(bool bInit)
{
	HRESULT hr = S_OK;

	if (bInit)
	{
		std::lock_guard<std::mutex> guard(m_protectIO);
		char buffer[255];
		memset(buffer, 0, 255);
		std::string iniFile = SvStl::GlobalPath::Inst().GetBinPath(cIniFile);
		::GetPrivateProfileString(cSettingsGroup, cOutputFileName, "", buffer, 255, iniFile.c_str());
		m_OutputFileName = buffer;
		memset(buffer, 0, 255);
		::GetPrivateProfileString(cSettingsGroup, cPLCSimulation, "", buffer, 255, iniFile.c_str());
		m_plcSimulateFile = buffer;
		m_plcTransferTime = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cPLCTransferTime, 0, iniFile.c_str()));
		m_plcNodeID = static_cast<uint16_t> (::GetPrivateProfileInt(cSettingsGroup, cPLCNodeID, 0, iniFile.c_str()));
	}
	else
	{
		m_TriggerDispatchers.Clear();
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

unsigned long SVPlcIOImpl::GetPortCount()
{
	return 0L;
}

HRESULT SVPlcIOImpl::GetInputValue(unsigned long* pValue)
{
	HRESULT result {E_FAIL};

	if(nullptr != pValue)
	{
		//Note we are inverting the signal because discrete IO uses inverted IOs
		*pValue = 0L;
		result = S_OK;
	}

	return result;
}

HRESULT SVPlcIOImpl::SetOutputValue(unsigned long )
{
	return S_OK;
}

HRESULT SVPlcIOImpl::SetOutputData(unsigned long triggerIndex, const SvTh::IntVariantMap& rData)
{
	HRESULT result {E_FAIL};
	ResultReport reportResult;

	//PLC channel is zero based while SVObserver trigger index is one based!
	reportResult.m_channel = static_cast<uint8_t> (triggerIndex - 1);
	reportResult.m_timestamp = SvTl::GetTimeStamp();
	auto iterData = rData.find(SvTh::TriggerDataEnum::ObjectID);
	if (rData.end() != iterData)
	{
		reportResult.m_currentObjectID = iterData->second;
	}
	iterData = rData.find(SvTh::TriggerDataEnum::OutputData);
	if (rData.end() != iterData)
	{
		const _variant_t& rResult{iterData->second};
		if((VT_UI1 | VT_ARRAY) ==  rResult.vt && rResult.parray->rgsabound[0].cElements == cResultSize)
		{
			memcpy(&reportResult.m_results[0], rResult.parray->pvData, cResultSize * sizeof(uint8_t));
		}
	}
	Tec::writeResult(reportResult);
	result = S_OK;
	if (false == m_OutputFileName.empty())
	{
		g_PlcListOutput.emplace_back(reportResult);
	}
	return result;
}

HRESULT SVPlcIOImpl::GetBoardVersion(long& )
{
	return S_OK;
}

HRESULT SVPlcIOImpl::GetInputBit(unsigned long , bool& )
{
	return S_OK;
}

HRESULT SVPlcIOImpl::SetOutputBit(unsigned long , bool )
{
	return S_OK;
}

unsigned long SVPlcIOImpl::GetTriggerCount()
{
	return cMaxPlcTriggers;
}

unsigned long SVPlcIOImpl::GetTriggerHandle(unsigned long triggerIndex)
{
	return (cMaxPlcTriggers > triggerIndex) ? (triggerIndex + 1) : 0;
}

BSTR SVPlcIOImpl::GetTriggerName(unsigned long triggerIndex)
{
	std::string triggerName{cTriggerName};

	triggerName += std::to_string(triggerIndex - 1);
	BSTR name = _bstr_t(triggerName.c_str()).Detach();
	return name;
}

void SVPlcIOImpl::beforeStartTrigger(unsigned long triggerIndex)
{
	m_currentTriggerChannel = static_cast<int8_t> (triggerIndex - 1);

	if (false == m_engineStarted)
	{
		g_PlcListInput.clear();
		g_PlcListOutput.clear();
		if(false == m_OutputFileName.empty())
		{
			g_PlcListInput.reserve(20000);
			g_PlcListOutput.reserve(20000);
		}
		Tec::startTriggerEngine(std::bind(&SVPlcIOImpl::reportTrigger, this, std::placeholders::_1), m_plcNodeID, m_plcTransferTime, m_plcSimulateFile);
		Tec::setReady(m_moduleReady);
		m_engineStarted = true;
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
}

HRESULT SVPlcIOImpl::afterStartTrigger(HRESULT result)
{
	if (S_OK == result && false == m_engineInitialized)
	{
		result = Tec::initTriggerEngine();
		m_engineInitialized = true;
	}
	if(S_OK == result)
	{
		if(0 <= m_currentTriggerChannel && cNumberOfChannels > m_currentTriggerChannel)
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

			if(false == m_OutputFileName.empty())
			{
				std::string fileName = m_OutputFileName;
				fileName += "_PlcInput.txt";
				::DeleteFile(fileName.c_str());

				uint32_t inputCount[cMaxPlcTriggers] {0, 0, 0, 0};
				std::string fileData = _T("Channel; Count; Timestamp; ObjectID; Trigger Index; Trigger per ObjectID\r\n");
				for (const auto& rData : g_PlcListInput)
				{
					++inputCount[rData.m_channel];
					fileData += SvUl::Format(_T("%d; %d; %f; %d; %d; %d\r\n"), rData.m_channel + 1, inputCount[rData.m_channel], rData.m_triggerTimestamp, rData.m_currentObjectID, rData.m_triggerIndex, rData.m_triggerPerObjectID);
				}
				std::ofstream outputFile {fileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app};
				if(outputFile.is_open())
				{
					outputFile.write(fileData.c_str(), fileData.size());
					outputFile.close();
				}

				fileName = m_OutputFileName;
				fileName += "_PlcOutput.txt";
				::DeleteFile(fileName.c_str());

				uint32_t outputCount[cMaxPlcTriggers] {0, 0, 0, 0};
				fileData = _T("Channel; Count; Timestamp; ObjectID; Results\r\n");
				for (const auto& rData : g_PlcListOutput)
				{
					std::string resultString;
					for(auto& rResult : rData.m_results)
					{
						resultString += std::to_string(rResult) + ' ';
					}
					++outputCount[rData.m_channel];
					fileData += SvUl::Format(_T("%d; %d; %f; %d; %s\r\n"), rData.m_channel + 1, outputCount[rData.m_channel], rData.m_timestamp, rData.m_currentObjectID, resultString.c_str());
				}
				outputFile.open(fileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app);
				if (outputFile.is_open())
				{
					outputFile.write(fileData.c_str(), fileData.size());
					outputFile.close();
				}
			}
		}
	}
}



HRESULT SVPlcIOImpl::TriggerGetParameterCount(unsigned long , unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = 1UL;
		result = S_OK;
	}
	return result;
}

HRESULT SVPlcIOImpl::TriggerGetParameterName(unsigned long , unsigned long index, BSTR* pName)
{
	HRESULT result{E_FAIL};

	if (nullptr != pName)
	{
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
			*pName = nullptr;
		}

		switch (index)
		{
			case SVBoardVersion:
			{
				*pName = ::SysAllocString(L"Board Version");
				result = S_OK;
				break;
			}

			default:
				break;
		}
	}
	return result;
}

HRESULT SVPlcIOImpl::TriggerGetParameterValue(unsigned long , unsigned long index, VARIANT* pValue)
{
	HRESULT result{E_FAIL};

	if (nullptr != pValue)
	{
		if (S_OK == ::VariantClear(pValue))
		{
			switch(index)
			{
				case SVBoardVersion:
				{
					WCHAR wbuf[256];
					pValue->vt = VT_BSTR;
					swprintf(wbuf, L"PLC Version %.2f ", 1.0);
					pValue->bstrVal = ::SysAllocString(wbuf);
					result = S_OK;
					break;
				}

				default:
					break;
			}
		}
	}
	return result;
}

HRESULT SVPlcIOImpl::TriggerSetParameterValue(unsigned long, unsigned long , VARIANT*)
{
	return E_FAIL;
}

// Non-Trigger Parameter Functions

// GetParameterCount
// This function returns the number of available parameters.
HRESULT SVPlcIOImpl::GetParameterCount(unsigned long* pCount)
{
	HRESULT hr = E_FAIL;

	if (nullptr != pCount)
	{
		*pCount = 3;
		hr = S_OK;
	}
	return hr;
}

HRESULT SVPlcIOImpl::GetParameterName(unsigned long index, BSTR* pName)
{
	HRESULT hr = E_FAIL;

	if (nullptr != pName)
	{
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
			*pName = nullptr;
		}

		if (SVBoardVersion == index)
		{
			*pName = ::SysAllocString(L"Board Version(R)");
		}
			
		if (nullptr != *pName)
		{
			hr = S_OK;
		}
	}
	return hr;
}

// GetParameterValue
// This function Gets the parameter value specified by ulIndex.
HRESULT SVPlcIOImpl::GetParameterValue(unsigned long index, VARIANT* pValue)
{
	HRESULT hr = E_FAIL;

	if (nullptr != pValue)
	{
		if (S_OK == ::VariantClear(pValue))
		{
			// Board Version
			if (SVBoardVersion == index)
			{
				WCHAR wbuf[256];
				pValue->vt = VT_BSTR;
				swprintf(wbuf, L"PLC Version %.2f ", 1.0);
				pValue->bstrVal = ::SysAllocString(wbuf);
			}
		}
	}
	return hr;
}

HRESULT SVPlcIOImpl::SetParameterValue(unsigned long index, VARIANT* pValue)
{
	HRESULT result{E_FAIL};

	if(SVModuleReady == index && nullptr != pValue && VT_BOOL == pValue->vt)
	{
		m_moduleReady = pValue->boolVal ? true : false;
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
	}
	

	if(false == m_OutputFileName.empty())
	{
		g_PlcListInput.emplace_back(rTriggerReport);
	}

	/// Only call trigger callbacks if the module ready is set this avoids problems with the PPQ Object not being ready
	if(m_moduleReady)
	{
		//PLC channel is zero based while SVObserver trigger index is one based!
		unsigned long triggerIndex = rTriggerReport.m_channel + 1;

		SvTh::IntVariantMap triggerData;
		triggerData[SvTh::TriggerDataEnum::TimeStamp] = _variant_t(rTriggerReport.m_triggerTimestamp);
		triggerData[SvTh::TriggerDataEnum::ObjectID] = _variant_t(rTriggerReport.m_currentObjectID);
		triggerData[SvTh::TriggerDataEnum::TriggerIndex] = _variant_t(rTriggerReport.m_triggerIndex);
		triggerData[SvTh::TriggerDataEnum::TriggerPerObjectID] = _variant_t(rTriggerReport.m_triggerPerObjectID);

		///The trigger dispatcher can not be changed when the module ready flag is set so no mutex is needed
		for (auto ChannelAndDispatcherList : m_TriggerDispatchers.GetDispatchers())
		{
			//Trigger channel is one based
			if(triggerIndex == ChannelAndDispatcherList.first)
			{
				SvTh::DispatcherVector dispatchVector = ChannelAndDispatcherList.second;
				std::async(std::launch::async, [&] { triggerDispatcher(std::move(triggerData), std::move(dispatchVector)); });
				break;
			}
		}
	}
}
} //namespace SvPlc
