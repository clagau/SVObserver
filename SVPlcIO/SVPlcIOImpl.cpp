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

std::vector<RecordData> g_PlcListInput;
std::vector<RecordData> g_PlcListOutput;

uint32_t g_PlcTriggerCount[cNumberTriggers] {0, 0, 0, 0};
uint32_t g_PlcResultCount[cNumberTriggers] {0, 0, 0, 0};

#pragma region Declarations
SVPlcIOImpl g_Plc;

constexpr unsigned long cInputCount = 0;
constexpr unsigned long cOutputCount = 14;
constexpr LPCTSTR c_IniFile = "SVPLCIO.ini";
constexpr LPCTSTR c_SettingsGroup = "Settings";
constexpr LPCTSTR c_PLCSimulation = "PLCSimulation";
constexpr LPCTSTR c_PLC_Transfer = "PLC-TransferTime";
constexpr LPCTSTR c_DelayedReportTrigger = "DelayedReportTrigger";
constexpr LPCTSTR c_BoardName = "IO_Board_1.Dig_%d";
#pragma endregion Declarations

void triggerWait(SvTh::IntVariantMap&& triggerData, SvTh::DispatcherVector&& dispatchVector, bool delayedReportTrigger)
{
	double now = SvTl::GetTimeStamp();

	if(delayedReportTrigger)
	{
		long long sleepTime = static_cast<long long> (triggerData[SvTh::TriggerDataEnum::TimeStamp].dblVal - now);
		if(sleepTime > 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
		}
	}

	for(auto& rDispatcher : dispatchVector)
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
		std::lock_guard<std::mutex> guard(m_protectPlc);
		char buffer[255];
		memset(buffer, 0, 255);
		std::string iniFile = SvStl::GlobalPath::Inst().GetBinPath(c_IniFile);
		::GetPrivateProfileString(c_SettingsGroup, "OutputFileName", "", buffer, 255, iniFile.c_str());
		m_OutputFileName = buffer;
		m_plcSimulation = static_cast<uint16_t> (::GetPrivateProfileInt(c_SettingsGroup, c_PLCSimulation, 0, iniFile.c_str()));
		m_plcTransferTime =::GetPrivateProfileInt(c_SettingsGroup, c_PLC_Transfer, 0, iniFile.c_str());
		m_delayedReportTrigger = (1 == ::GetPrivateProfileInt(c_SettingsGroup, c_DelayedReportTrigger, 0, iniFile.c_str())) ? true : false;
		m_isInitialized = true;
	}
	else
	{
		m_TriggerDispatchers.Clear();
		m_isInitialized = false;
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

HRESULT SVPlcIOImpl::SetOutputValue(unsigned long value)
{
	return S_OK;
}

HRESULT SVPlcIOImpl::SetOutputData(unsigned long triggerChannel, const SvTh::IntVariantMap& rData)
{
	HRESULT result {E_FAIL};
	ResultReport reportResult;

	reportResult.m_channel = static_cast<uint8_t> (triggerChannel);
	auto iterData = rData.find(SvTh::TriggerDataEnum::ObjectID);
	if (rData.end() != iterData)
	{
		reportResult.m_currentObjectID = iterData->second;
	}
	iterData = rData.find(SvTh::TriggerDataEnum::OutputData);
	if (rData.end() != iterData)
	{
		const _variant_t& rResult{iterData->second};
		if((VT_UI1 | VT_ARRAY) ==  rResult.vt && rResult.parray->rgsabound[0].cElements == c_ResultSize)
		{
			memcpy(&reportResult.m_results[0], rResult.parray->pvData, c_ResultSize * sizeof(uint8_t));
		}
	}
	Tec::writeResult(reportResult);
	result = S_OK;
	if (false == m_OutputFileName.empty())
	{
		g_PlcResultCount[triggerChannel]++;

		g_PlcListOutput.emplace_back(RecordData {reportResult.m_channel,g_PlcResultCount[triggerChannel], SvTl::GetTimeStamp(), reportResult.m_currentObjectID, reportResult.m_results[0] == 6, std::string{}});
	}
	return result;
}

HRESULT SVPlcIOImpl::GetBoardVersion(long& rlVer)
{
	return S_OK;
}

HRESULT SVPlcIOImpl::GetInputBit(unsigned long bitNum, bool& rBitVal)
{
	return S_OK;
}

HRESULT SVPlcIOImpl::SetOutputBit(unsigned long bitNum, bool bitVal)
{
	return S_OK;
}

unsigned long SVPlcIOImpl::GetTriggerCount()
{
	return cNumberTriggers;
}

unsigned long SVPlcIOImpl::GetTriggerHandle(unsigned long triggerIndex)
{
	return (cNumberTriggers > triggerIndex) ? (triggerIndex + 1) : 0;
}

BSTR SVPlcIOImpl::GetTriggerName(unsigned long triggerChannel)
{
	std::string triggerName;

	triggerName = SvUl::Format(c_BoardName, triggerChannel - 1);
	BSTR name = _bstr_t(triggerName.c_str()).Detach();
	return name;
}

void SVPlcIOImpl::beforeStartTrigger(unsigned long triggerChannel)
{
	m_currentTriggerIndex = static_cast<int8_t> (triggerChannel - 1);

	if (false == m_engineStarted)
	{
		for(int i=0; i < cNumberTriggers; ++i)
		{
			g_PlcTriggerCount[i] = 0;
			g_PlcResultCount[i] = 0;
		}
		g_PlcListInput.clear();
		g_PlcListOutput.clear();
		m_OutputData.clear();
		if(false == m_OutputFileName.empty())
		{
			g_PlcListInput.reserve(20000);
			g_PlcListOutput.reserve(20000);
		}
		Tec::startTriggerEngine(std::bind(&SVPlcIOImpl::reportTrigger, this, std::placeholders::_1), m_plcTransferTime, m_plcSimulation);
		Tec::setReady(m_moduleReady);
		m_engineStarted = true;
	}
}

HRESULT SVPlcIOImpl::afterStartTrigger(HRESULT result)
{
	if (S_OK == result)
	{
		result = Tec::initTriggerEngine();
		if(S_OK ==result)
		{
			if(0 <= m_currentTriggerIndex && c_NumberOfChannels > m_currentTriggerIndex)
			{
				if (false == m_triggerStarted[m_currentTriggerIndex])
				{
					Tec::setTriggerChannel(static_cast<uint8_t> (m_currentTriggerIndex), true, m_plcSimulation);
					m_triggerStarted[m_currentTriggerIndex] = true;
				}
			}
		}
		else
		{
			///Clean up all triggers and engine
			for(uint8_t i=0; i < cNumberTriggers; ++i)
			{
				if (m_triggerStarted[i])
				{
					m_triggerStarted[i] = false;
					//Trigger Engine trigger channel is zero based while SVObserver is one based!
					Tec::setTriggerChannel(i, false, 0);
				}
			}
			Tec::stopTriggerEngine();
			m_engineStarted = false;
		}
	}
	return result;
}

void SVPlcIOImpl::beforeStopTrigger(unsigned long triggerChannel)
{
	int triggerIndex = triggerChannel - 1;
	if (0 <= triggerIndex && cNumberTriggers > triggerIndex)
	{
		if (m_triggerStarted[triggerIndex])
		{
			m_triggerStarted[triggerIndex] = false;
			//Trigger Engine trigger channel is zero based while SVObserver is one based!
			Tec::setTriggerChannel(static_cast<uint8_t> (triggerIndex), false, 0);
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
			::OutputDebugString("Irq Enabled reset\n");

			if(false == m_OutputFileName.empty())
			{
				std::string fileName = m_OutputFileName;
				fileName += "_PlcInput.txt";
				::DeleteFile(fileName.c_str());

				std::string fileData = _T("Channel; Count; Timestamp; ObjectID; calc Timestamp; SOC TriggerTime\r\n");
				for (const auto& rData : g_PlcListInput)
				{
					fileData += SvUl::Format(_T("%d; %d; %f; %d; %s\r\n"), rData.m_channel, rData.m_count, rData.m_timeStamp, rData.m_objectID, rData.m_text.c_str());
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

				fileData = _T("Channel; Count; Timestamp; ObjectID;Good\r\n");
				for (const auto& rData : g_PlcListOutput)
				{
					fileData += SvUl::Format(_T("%d; %d; %f; %d; %d\r\n"), rData.m_channel, rData.m_count, rData.m_timeStamp, rData.m_objectID, rData.m_good ? 1 : 0);
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
	{
		std::lock_guard<std::mutex> guard(m_protectPlc);
		m_TriggerDispatchers.ContainsNoActiveTriggers();
	}
}



HRESULT SVPlcIOImpl::TriggerGetParameterCount(unsigned long triggerChannel, unsigned long* pCount)
{
	HRESULT result {E_FAIL};

	if (nullptr != pCount)
	{
		*pCount = 3L;
		result = S_OK;
	}
	return result;
}

HRESULT SVPlcIOImpl::TriggerGetParameterName(unsigned long triggerChannel, unsigned long index, BSTR* pName)
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
			//This is required for backward compatibility so set it to default trigger rising edge
			case SVSignalEdge:
			{
				*pName = ::SysAllocString(L"Edge");
				result = S_OK;
				break;
			}

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

HRESULT SVPlcIOImpl::TriggerGetParameterValue(unsigned long triggerChannel, unsigned long index, VARIANT* pValue)
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

HRESULT SVPlcIOImpl::TriggerSetParameterValue(unsigned long triggerChannel, unsigned long index, VARIANT* pValue)
{
	HRESULT result{E_FAIL};
	if (nullptr != pValue)
	{
	}
	return result;

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

// GetParameterName
// This function creates a BSTR with the name of the parameter
// It is the responsibility of the caller to free the BSTR.
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
		else if (SVFanState == index)
		{
			*pName = ::SysAllocString(L"Fan State(R)");
		}
		else if (SVBoardType == index)
		{
			*pName = ::SysAllocString(L"Board Type(R/W)");
		}
		else if (SVFanFreq == index)
		{
			*pName = ::SysAllocString(L"Fan Frequency");
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
			else if (SVBoardType == index)			// Board Type : X-Series, D-Series, or A-Series Boards.
			{
				pValue->vt = VT_I4;
				if (S_OK == hr)
				{
					pValue->lVal = 0L;
				}
			}
		}
	}
	return hr;
}

// SetParameterValue
// This function sets the value specified by ulIndex.
// if the index is not supported for setting, then an error is returned.
HRESULT SVPlcIOImpl::SetParameterValue(unsigned long index, VARIANT* pValue)
{
	HRESULT result{E_FAIL};

	if(SVModuleReady == index && nullptr != pValue && VT_BOOL == pValue->vt)
	{
		m_moduleReady = pValue->boolVal ? true : false;
		Tec::setReady(m_moduleReady);
	}

	return result;
}

void SVPlcIOImpl::reportTrigger(const TriggerReport& rTriggerReport)
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	if(cNumberTriggers > rTriggerReport.m_channel)
	{
		if(false == m_triggerStarted[rTriggerReport.m_channel])
		{
			return;
		}
	}
	
	SvTh::IntVariantMap triggerData;
	triggerData[SvTh::TriggerDataEnum::TimeStamp] = _variant_t(rTriggerReport.m_triggerTimestamp);
	triggerData[SvTh::TriggerDataEnum::ObjectID] = _variant_t(rTriggerReport.m_currentObjectID);
	triggerData[SvTh::TriggerDataEnum::TriggerIndex] = _variant_t(rTriggerReport.m_triggerIndex);
	triggerData[SvTh::TriggerDataEnum::TriggerPerObjectID] = _variant_t(rTriggerReport.m_triggerPerObjectID);

	//PLC channel is zero based while SVObserver is one based!
	int triggerIndex = rTriggerReport.m_channel + 1;

	if(false == m_OutputFileName.empty())
	{
		g_PlcTriggerCount[rTriggerReport.m_channel]++;
	
		g_PlcListInput.emplace_back(RecordData {static_cast<uint8_t> (triggerIndex), g_PlcTriggerCount[rTriggerReport.m_channel], rTriggerReport.m_triggerTimestamp, rTriggerReport.m_currentObjectID, false, rTriggerReport.m_text});
	}

	/// Only call trigger callbacks if the module ready is set this avoids problems with the PPQ Object not being ready
	if(m_moduleReady)
	{
		std::lock_guard<std::mutex> guard(m_protectPlc);
		for (auto ChannelAndDispatcherList : m_TriggerDispatchers.GetDispatchers())
		{
			//Trigger channel is one based
			if(triggerIndex == ChannelAndDispatcherList.first)
			{
				SvTh::DispatcherVector dispatchVector = ChannelAndDispatcherList.second;
				std::async(std::launch::async, [&] { triggerWait(std::move(triggerData), std::move(dispatchVector), m_delayedReportTrigger); });
				break;
			}
		}
	}
}
} //namespace SvPlc
