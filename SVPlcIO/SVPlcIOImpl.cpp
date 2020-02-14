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

namespace SvPlc
{

std::vector<RecordData> g_PlcListInput;
std::vector<RecordData> g_PlcListOutput;

uint32_t g_PlcTriggerCount[cNumberTriggers] {0, 0, 0, 0};
uint32_t g_PlcResultCount[cNumberTriggers] {0, 0, 0, 0};
std::atomic_bool g_enableInterrupt{false};

#pragma endregion Includes

#pragma region Declarations
SVPlcIOImpl g_Plc;

constexpr uint8_t c_InspectionBad = 5;
constexpr uint8_t c_InspectionGood = 6;
constexpr unsigned long cNumberPorts = 2;
constexpr unsigned long cInputCount = 16;
constexpr unsigned long cOutputCount = 16;
constexpr LPCTSTR c_IniFile = "SVPLCIO.ini";
constexpr LPCTSTR c_SettingsGroup = "Settings";
constexpr LPCTSTR c_TriggerGroup = "Trigger%d";
constexpr LPCTSTR c_DataValidIndex = "DataValidIndex";
constexpr LPCTSTR c_ObjectGoodIndex = "ObjectGoodIndex";
constexpr LPCTSTR c_ConditionIndex = "ConditionIndex";
constexpr LPCTSTR c_SVO_ReadyIndex = "SVO-ReadyIndex";
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
		m_Output = 0;
		m_Input = 0;
		char buffer[255];
		memset(buffer, 0, 255);
		std::string iniFile = SvStl::GlobalPath::Inst().GetBinPath(c_IniFile);
		::GetPrivateProfileString(c_SettingsGroup, "OutputFileName", "", buffer, 255, iniFile.c_str());
		m_OutputFileName = buffer;
		m_plcSimulation = static_cast<uint16_t> (::GetPrivateProfileInt(c_SettingsGroup, c_PLCSimulation, 0, iniFile.c_str()));
		m_plcTransferTime =::GetPrivateProfileInt(c_SettingsGroup, c_PLC_Transfer, 0, iniFile.c_str());
		m_readyBit =::GetPrivateProfileInt(c_SettingsGroup, c_SVO_ReadyIndex, 0, iniFile.c_str());
		m_readyBit = (0 < m_readyBit) ? m_readyBit - 1 : -1;
		m_delayedReportTrigger = (1 == ::GetPrivateProfileInt(c_SettingsGroup, c_DelayedReportTrigger, 0, iniFile.c_str())) ? true : false;
		for(int i=0; i < cNumberTriggers; i++)
		{
			TriggerParameter& rTrigger = m_trigger[i];
			rTrigger.m_ObjectID = 0;
			std::string triggerGroup = SvUl::Format(c_TriggerGroup, i + 1);
			rTrigger.m_conditionIndex = static_cast<long> (::GetPrivateProfileInt(triggerGroup.c_str(), c_ConditionIndex, -1, iniFile.c_str()));
			rTrigger.m_dataValidIndex = static_cast<long> (::GetPrivateProfileInt(triggerGroup.c_str(), c_DataValidIndex, -1, iniFile.c_str()));
			rTrigger.m_objectGoodIndex = static_cast<long> (::GetPrivateProfileInt(triggerGroup.c_str(), c_ObjectGoodIndex, -1, iniFile.c_str()));
			//Make it a zero based index
			rTrigger.m_dataValidIndex = (0 < rTrigger.m_dataValidIndex) ? rTrigger.m_dataValidIndex - 1 : -1;
			rTrigger.m_conditionIndex = (0 < rTrigger.m_conditionIndex) ? rTrigger.m_conditionIndex - 1 : -1;
			rTrigger.m_objectGoodIndex = (0 < rTrigger.m_objectGoodIndex) ? rTrigger.m_objectGoodIndex - 1 : -1;

			rTrigger.m_period = static_cast<long> (::GetPrivateProfileInt(triggerGroup.c_str(), "TriggerPeriod", 0, iniFile.c_str()));
		}

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
	return cInputCount;
}

unsigned long SVPlcIOImpl::GetOutputCount()
{
	return cOutputCount;
}

unsigned long SVPlcIOImpl::GetPortCount()
{
	return cNumberPorts;
}

HRESULT SVPlcIOImpl::GetInputValue(unsigned long* pValue)
{
	HRESULT result {E_FAIL};

	if(nullptr != pValue)
	{
		//Note we are inverting the signal because discrete IO uses inverted IOs
		std::lock_guard<std::mutex> guard(m_protectPlc);
		WORD inputValue = ~m_Input;
		*pValue = static_cast<unsigned long> (inputValue);
		result = S_OK;
	}

	return result;
}

HRESULT SVPlcIOImpl::SetOutputValue(unsigned long value)
{
	//Note we are inverting the signal because discrete IO uses inverted IOs
	WORD outputValue = ~static_cast<WORD> (value);
	std::lock_guard<std::mutex> guard(m_protectPlc);
	m_Output = outputValue;
	return S_OK;
}

HRESULT SVPlcIOImpl::SetOutputData(unsigned long triggerChannel, const SvTh::IntVariantMap& rData)
{
	HRESULT result {E_FAIL};

	SvTh::IntVariantMap::const_iterator iterData {rData.end()};
	iterData = rData.find(SvTh::TriggerDataEnum::ObjectID);
	if (rData.end() != iterData)
	{
		m_trigger[triggerChannel - 1].m_ObjectID = iterData->second;
		result = S_OK;
	}
	return result;
}

// GetBoardVersion
// Get Board Version sends a Sub Command value of 0x56 (GET_BOARD_VERSION)
// This puts the parallel board in a mode that will send the 
// Board version on the next four ControlReadCommands.
//  
HRESULT SVPlcIOImpl::GetBoardVersion(long& rlVer)
{
	return S_OK;
}

HRESULT SVPlcIOImpl::GetInputBit(unsigned long bitNum, bool& rBitVal)
{
	HRESULT result {E_FAIL};

	if (bitNum < cInputCount)
	{
		WORD bitMask = 1 << bitNum;
		std::lock_guard<std::mutex> guard(m_protectPlc);
		//Note we are inverting the signal because discrete IO uses inverted IOs
		rBitVal = 0 == (m_Input & bitMask) ? true : false;
		result = S_OK;
	}
	return result;
}

HRESULT SVPlcIOImpl::SetOutputBit(unsigned long bitNum, bool bitVal)
{
	HRESULT result {E_FAIL};

	if(bitNum < cOutputCount)
	{
		WORD bitMask = 1 << bitNum;
		{
			std::lock_guard<std::mutex> guard(m_protectPlc);
			//Note we are inverting the signal because discrete IO uses inverted IOs
			m_Output = bitVal ? m_Output & ~bitMask : m_Output | bitMask;
		}
		int triggerIndex{-1};
		//Check if a data valid signal has been triggered
		for(int i=0; i < cNumberTriggers && -1 == triggerIndex; ++i)
		{
			triggerIndex = (bitNum == m_trigger[i].m_dataValidIndex) ? i : -1;
		}
		//Check only when Data valid is set (bitVal = false due to Inverted signal)
		if(-1 != triggerIndex && false == bitVal && 0 != m_trigger[triggerIndex].m_ObjectID)
		{
			WriteResult(triggerIndex);
		}
		if(bitNum == m_readyBit)
		{
			//bitVal = false due to Inverted signal
			Tec::setReady(false == bitVal);
		}
		result = S_OK;
	}
	return result;
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
	std::lock_guard<std::mutex> guard(m_protectPlc);
	int triggerIndex = triggerChannel - 1;
	if (0 <= triggerIndex && cNumberTriggers > triggerIndex)
	{
		TriggerParameter& rTrigger = m_trigger[triggerIndex];
		if (!m_engineStarted)
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
			//Set the ready bit
			WORD bitMask = 1 << m_readyBit;
			Tec::setReady(0 != (bitMask & m_Output));
			m_engineStarted = true;
			g_enableInterrupt = true;
			::OutputDebugString("Irq Enabled set\n");
		}
		if (!rTrigger.m_started)
		{
			//Trigger Engine trigger channel is zero based while SVObserver is one based!
			Tec::setTriggerChannel(static_cast<uint8_t> (triggerIndex), true, rTrigger.m_period);
			rTrigger.m_started = true;
		}
	}
}


void SVPlcIOImpl::beforeStopTrigger(unsigned long triggerChannel)
{
	std::lock_guard<std::mutex> guard(m_protectPlc);
	int triggerIndex = triggerChannel - 1;
	if (0 <= triggerIndex && cNumberTriggers > triggerIndex)
	{
		TriggerParameter& rTrigger = m_trigger[triggerIndex];
		if (rTrigger.m_started)
		{
			rTrigger.m_started = false;
			//Trigger Engine trigger channel is zero based while SVObserver is one based!
			Tec::setTriggerChannel(static_cast<uint8_t> (triggerIndex), false, rTrigger.m_period);
		}
		//Still some active trigger
		bool activeTrigger{false};
		for (const auto& rEntry : m_trigger)
		{
			activeTrigger |= rEntry.m_started;
		}
		if(!activeTrigger)
		{
			Tec::stopTriggerEngine();
			m_engineStarted = false;
			g_enableInterrupt = false;
			::OutputDebugString("Irq Enabled reset\n");

			if(false == m_OutputFileName.empty())
			{
				std::string fileName = m_OutputFileName;
				fileName += "_PlcInput.txt";
				::DeleteFile(fileName.c_str());

				std::string fileData = _T("Channel; Count; Timestamp; ObjectID\r\n");
				for (const auto& rData : g_PlcListInput)
				{
					fileData += SvUl::Format(_T("%d; %d; %f; %d\r\n"), rData.m_channel, rData.m_count, rData.m_timeStamp, rData.m_objectID);
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
				for (const auto& rData : g_PlcListInput)
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
	m_TriggerDispatchers.ContainsNoActiveTriggers();
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
			std::lock_guard<std::mutex> guard(m_protectPlc);
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
	return E_FAIL;
}

void SVPlcIOImpl::reportTrigger(const TriggerReport& rTriggerReport)
{
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	std::lock_guard<std::mutex> guard(m_protectPlc);
	if(cNumberTriggers > rTriggerReport.m_channel)
	{
		const TriggerParameter& rTrigger = m_trigger[rTriggerReport.m_channel];

		if(!rTrigger.m_started)
		{
			return;
		}

		//If condition index is set then set the corresponding input
		if (-1 != rTrigger.m_conditionIndex)
		{
			WORD bitMask = 1 << rTrigger.m_conditionIndex;
			m_Input = rTriggerReport.m_isComplete ? m_Input |= bitMask : m_Input & ~bitMask;
		}
	}
	
	SvTh::IntVariantMap triggerData;
	triggerData[SvTh::TriggerDataEnum::TimeStamp] = _variant_t(rTriggerReport.m_triggerTimestamp);
	triggerData[SvTh::TriggerDataEnum::ObjectID] = _variant_t(rTriggerReport.m_objectID);
	triggerData[SvTh::TriggerDataEnum::TriggerIndex] = _variant_t(rTriggerReport.m_triggerIndex);

	//PLC channel is zero based while SVObserver is one based!
	int triggerIndex = rTriggerReport.m_channel + 1;

	if(false == m_OutputFileName.empty())
	{
		g_PlcTriggerCount[rTriggerReport.m_channel]++;
	
		g_PlcListInput.emplace_back(RecordData {static_cast<uint8_t> (triggerIndex), g_PlcTriggerCount[rTriggerReport.m_channel], rTriggerReport.m_triggerTimestamp, rTriggerReport.m_objectID, false});
	}

	// call trigger callbacks
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

void SVPlcIOImpl::WriteResult(int triggerChannel)
{
	ResultReport reportResult;
	const TriggerParameter& rTrigger = m_trigger[triggerChannel];
	//Reduce mutex scope
	{
		std::lock_guard<std::mutex> guard(m_protectPlc);
		reportResult.m_channel = triggerChannel;
		reportResult.m_objectID = rTrigger.m_ObjectID;
		uint16_t bitMask = 1 << rTrigger.m_objectGoodIndex;
		reportResult.m_results[0] = (0 != (m_Output & bitMask)) ? c_InspectionGood : c_InspectionBad;
		///The PLC requires that the second result always is set to good!
		reportResult.m_results[1] = c_InspectionGood;
		Tec::writeResult(reportResult);
		if (false == m_OutputFileName.empty())
		{
			g_PlcResultCount[triggerChannel]++;

			g_PlcListOutput.emplace_back(RecordData{reportResult.m_channel,g_PlcResultCount[triggerChannel], SvTl::GetTimeStamp(), reportResult.m_objectID, reportResult.m_results[0] == c_InspectionGood});
		}
	}
}

} //namespace SvPlc
