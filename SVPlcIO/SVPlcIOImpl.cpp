//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMPL
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <time.h>
//Moved to precompiled header: #include <functional>
//Moved to precompiled header: #include <future>
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <string>
#include "SVPlcIOImpl.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "TriggerEngineConnection.h"
#include "SVTimerLibrary/SVClock.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary/MessageManager.h"
#include "SVStatusLibrary/GlobalPath.h"
#include "SVUtilityLibrary/StringHelper.h"

//#define TRIGGER_SIMULATE
//#define PLC_LPTIO

#ifdef PLC_LPTIO
//@TODO[gra][8.20][08.04.2019]: Needs to be removed after LPT functionality no longer in PLC required (for timing purposes)
#include "LPT/SVLptDll.h"
#pragma comment(lib, "SVLpt")

enum SVHWControlEnum
{
	//		NoMode Not used anywhere.
	//		SVControlNoMode               = 0x001B, // 0000 0000 0001 1011
	SVControlCameraSetup = 0x001A, // 0000 0000 0001 1010
	SVControlWriteDigital0 = 0x0019, // 0000 0000 0001 1001
	SVControlWriteDigital1 = 0x0018, // 0000 0000 0001 1000
	SVControlWriteDigital1_110 = 0x001B, // 0000 0000 0001 1011
	SVControlReadDigitalInputs = 0x0013, // 0000 0000 0001 0011
	SVControlReadCommand = 0x0012, // 0000 0000 0001 0010
	SVControlCommand = 0x0011, // 0000 0000 0001 0001
	SVControlSelectMode = 0x0014, // 0000 0000 0001 0100
	SVControlEnableInterrupt = 0x0010, // 0000 0000 0001 0000
	SVControlBidirectional = 0x0020, // 0000 0000 0010 0000
	SVControlUseBitLevel = 0x1000, // 0001 0000 0000 0000
};

enum SVTriggerChannel
{
	SVTriggerNone = 0,
	SVTrigger1 = 0x08,	// S3- Error
	SVTrigger2 = 0x10,	// S4+ SelectIn
	SVTrigger3 = 0x20,	// S5+ PaperEnd
	SVTrigger4 = 0x80,	// S7- Busy
};

//@TODO[gra][8.20][08.04.2019]
#endif

#pragma endregion Includes

#pragma region Declarations
SVPlcIOImpl g_Plc;

constexpr unsigned long cNumberPorts = 2;
constexpr unsigned long cInputCount = 16;
constexpr unsigned long cOutputCount = 16;
constexpr LPCTSTR c_IniFile = "SVPLCIO.ini";
constexpr LPCTSTR c_SettingsGroup = "Settings";
constexpr LPCTSTR c_TriggerGroup = "Trigger%d";
constexpr LPCTSTR c_DataValidIndex = "DataValidIndex";
constexpr LPCTSTR c_ObjectGoodIndex = "ObjectGoodIndex";
constexpr LPCTSTR c_ConditionIndex = "ConditionIndex";
constexpr LPCTSTR c_PLCSimulation = "PLCSimulation";
constexpr LPCTSTR c_TriggerSimulation = "TriggerSimulation";
constexpr LPCTSTR c_BoardName = "IO_Board_1.Dig_%d";

#pragma endregion Declarations

#ifdef TRIGGER_SIMULATE
void TriggerTimer(std::function<void(const TriggerReport&)> pTrigger, uint8_t channel, long period, const std::atomic_bool* pRun)
{
	TriggerReport triggerRecord{channel, 0L, 0L, 0LL, true};

	while (*pRun)
	{
		triggerRecord.m_objectID++;
		triggerRecord.m_triggerTime_ms = SvTl::GetTimeStamp();
		pTrigger(triggerRecord);
		Sleep(period);
	}

	triggerRecord.m_objectID = 0L;
}


#else
#pragma comment(lib, "TriggerEngineConnection.lib")
#pragma comment(lib, "RtxBasics.lib")
#endif

#ifdef PLC_LPTIO
//@TODO[gra][8.20][08.04.2019]: Needs to be removed after LPT functionality no longer in PLC required (for timing purposes)
void ReadWriteLPT(uint8_t controlPort, uint8_t value, int8_t bitNr = -1)
{
	static uint8_t currentOutput[2]{0xff, 0xff};

	if(bitNr >= static_cast<int8_t> (cOutputCount / 2))
	{
		return;
	}
	HRESULT hResult = WriteControlPort(SVControlEnableInterrupt);

	uint8_t& rOutput = (SVControlWriteDigital1_110 == controlPort) ? currentOutput[1] : currentOutput[0];
	hResult = WriteControlPort(SVControlEnableInterrupt | controlPort);
	double Start = SvTl::GetTimeStamp();
	unsigned char status;
	hResult = ReadStatusPort(status);
	while (S_OK == hResult && 0 == (status & 128))
	{
		SvTl::SVTimeStamp Check = SvTl::GetTimeStamp();

		if (SvTl::ConvertTo(SvTl::Microseconds, (Check - Start)) > 16000)
		{
			hResult = ReadStatusPort(status);
			if (0 == (status & 128))
			{
				hResult = S_FALSE;
			}
			break;
		}
		hResult = ReadStatusPort(status);
	}
	uint8_t dataValue{0};
	if(-1 == bitNr)
	{
		dataValue = value;
	}
	else
	{
		dataValue = rOutput;
		dataValue = (1 == value) ? dataValue | (1 << bitNr) : dataValue & ~(1 << bitNr);
	}

	hResult = WriteDataPort(dataValue);
	if(S_OK == hResult)
	{
		rOutput = dataValue;
	}

	hResult = WriteControlPort(SVControlEnableInterrupt | controlPort | SVControlSelectMode);

	if (S_OK == hResult)
	{
		Start = SvTl::GetTimeStamp();
		hResult = ReadStatusPort(status);
		while (S_OK == hResult && 0 != (status & 128))
		{
			SvTl::SVTimeStamp Check = SvTl::GetTimeStamp();
			if (SvTl::ConvertTo(SvTl::Microseconds, (Check - Start)) > 16000)
			{
				hResult = ReadStatusPort(status);
				if (S_OK == hResult && 0 != (status & 128))
				{
					hResult = S_FALSE;
				}
				break;
			}
			hResult = ReadStatusPort(status);
		}
		hResult = WriteControlPort(SVControlEnableInterrupt | controlPort);
	}
	hResult = WriteControlPort(SVControlEnableInterrupt);
}

void LptInterruptHandler()
{
	static unsigned char LastTriggerState{0};

	unsigned char StatusReg;
	HRESULT result = ReadStatusPort(StatusReg);
	if (S_OK == result)
	{
		for (int i = 0; i < cNumberTriggers - 1; ++i)
		{
			short nTriggerBit = SVTriggerNone;

			// Get the trigger bit 
			switch (i) //the 1 based channel.
			{
				case 0: // trigger 1 Bit 3 of Status port
				{
					nTriggerBit = SVTrigger1;
					break;
				}
				case 1:
				{
					nTriggerBit = SVTrigger2;
					break;
				}
				case 2:
				{
					nTriggerBit = SVTrigger3;
					break;
				}
				default:
				{
					break;
				}
			}

			if (nTriggerBit != SVTriggerNone && (nTriggerBit & StatusReg) != (nTriggerBit & LastTriggerState))
			{
				bool bitState = (0 != (nTriggerBit & StatusReg));
			}
		}
	}
	LastTriggerState = StatusReg;
}

void SetOutputBitTimed(SVHWControlEnum controlType, int8_t bitNr, long resetTime)
{
	//LPT port is inverted 0 = on 1 = off
	ReadWriteLPT(controlType, 0, bitNr);
	Sleep(resetTime);
	ReadWriteLPT(controlType, 1, bitNr);
}
//@TODO[gra][8.20][08.04.2019]
#endif

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
		m_triggerSimulation = (1 == ::GetPrivateProfileInt(c_SettingsGroup, c_TriggerSimulation, 0, iniFile.c_str())) ? true : false;
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
			rTrigger.m_outputBit = static_cast<long> (::GetPrivateProfileInt(triggerGroup.c_str(), "TriggerOutputBit", -1, iniFile.c_str()));
#ifdef PLC_LPTIO
			//@TODO[gra][8.20][08.04.2019]: Needs to be removed after LPT functionality no longer in PLC required (for timing purposes)
			//Check if we should load the LPT driver
			if (1 == ::GetPrivateProfileInt(c_SettingsGroup, "UseLPTDriver", 0, iniFile.c_str()))
			{
				//Make it a zero based index
				rTrigger.m_outputBit = (0 < rTrigger.m_outputBit) ? rTrigger.m_outputBit - 1 : -1;
				if (rTrigger.m_outputBit < 0 || rTrigger.m_outputBit >= cOutputCount)
				{
					rTrigger.m_outputBit = -1L;
				}
				if(-1L != rTrigger.m_outputBit)
				{
					if(!m_isLptActive)
					{
						m_resetOutputTime = ::GetPrivateProfileInt(c_SettingsGroup, "ResetOutputTime", 0, iniFile.c_str());
						HRESULT result = Open();
						if (S_OK == result)
						{
							ReadWriteLPT(SVControlCommand, NO_CMD);
							//Unlock the outputs of the LPT port
							ReadWriteLPT(SVControlCommand, UNLOCK_CMD);
							constexpr char c_UnlockText[] = "Unlock";
							for(const auto& rChar : c_UnlockText)
							{
								ReadWriteLPT(SVControlCommand, static_cast<uint8_t> (rChar));
							}
							time_t timeVal;
							time(&timeVal);
							//We want to set the first 4 bytes
							uint8_t* pTimeByte = reinterpret_cast<uint8_t*> (&timeVal);
							ReadWriteLPT(SVControlCommand, NO_CMD);
							// Get the Rabbit ready to receive a string of bytes for RTC set
							ReadWriteLPT(SVControlCommand, WRITE_RTC);
							for (int i = 0; i < sizeof(uint32_t); i++)
							{
								ReadWriteLPT(SVControlCommand, *pTimeByte);	// Write 4 Bytes to fill a long in the Rabbit.
								pTimeByte++;
							}

							//Set Bit0-Bit7 and Bit8-Bit15 to off
							ReadWriteLPT(SVControlWriteDigital0, 0xff);
							ReadWriteLPT(SVControlWriteDigital1_110, 0xff);
							EnableInterrupt(&LptInterruptHandler);
						}
					}

					m_isLptActive = true;
				}
			}
			//@TODO[gra][8.20][08.04.2019]
#endif
		}

		m_isInitialized = true;
	}
	else
	{
		m_TriggerDispatchers.Clear();
		m_isInitialized = false;

#ifdef PLC_LPTIO
		//@TODO[gra][8.20][08.04.2019]: Needs to be removed after LPT functionality no longer in PLC required (for timing purposes)
		if(m_isLptActive)
		{
			DisableInterrupt();
			Close();
		}
		//@TODO[gra][8.20][08.04.2019]
#endif
	}
	return hr;
}

unsigned long SVPlcIOImpl::GetNumPorts()
{
	return cNumberPorts;
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
		if (m_triggerSimulation)
		{
#ifdef TRIGGER_SIMULATE
			if (!rTrigger.m_timer.joinable())
			{
				rTrigger.m_started = true;
				rTrigger.m_timer = std::thread(TriggerTimer, std::bind(&SVPlcIOImpl::reportTrigger, this, std::placeholders::_1), triggerIndex, rTrigger.m_period, &rTrigger.m_started);
			}
#endif
		}
		else
		{
#ifndef TRIGGER_SIMULATE
			if (!m_engineStarted)
			{
				m_OutputData.clear();
				startTriggerEngine(std::bind(&SVPlcIOImpl::reportTrigger, this, std::placeholders::_1), m_plcSimulation);
				m_engineStarted = true;
			}
			if (!rTrigger.m_started)
			{
				//RTX trigger channel is zero based while SVObserver is one based!
				startTriggerchannel(static_cast<uint8_t> (triggerIndex));
				rTrigger.m_started = true;
			}
#endif
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
		if (m_triggerSimulation)
		{
			rTrigger.m_started = false;
			if (rTrigger.m_timer.joinable())
			{
				rTrigger.m_timer.join();
			}
		}
		else
		{
#ifndef TRIGGER_SIMULATE
			if (rTrigger.m_started)
			{
				//RTX channel is zero based while SVObserver is one based!
				stopTriggerchannel(static_cast<uint8_t> (triggerIndex));
				rTrigger.m_started = false;
			}
			//Still some active trigger
			bool activeTrigger{false};
			for (const auto& rEntry : m_trigger)
			{
				activeTrigger |= rEntry.m_started;
			}
			if(!activeTrigger)
			{
				stopTriggerEngine();
				m_engineStarted = false;

				std::string fileData;
				for(const auto& rLine : m_OutputData)
				{
					fileData += rLine;
				}

				if(!m_OutputFileName.empty())
				{
					::DeleteFile(m_OutputFileName.c_str());

					std::ofstream testFile {m_OutputFileName.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::app};
					if(testFile.is_open())
					{
						testFile.write(fileData.c_str(), fileData.size());
						testFile.close();
					}
					m_OutputData.clear();
				}
			}
#endif
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
	double now = SvTl::GetTimeStamp();
	if(cNumberTriggers > rTriggerReport.m_channel)
	{
		const TriggerParameter& rTrigger = m_trigger[rTriggerReport.m_channel];

		if(!rTrigger.m_started)
		{
			return;
		}
#ifdef PLC_LPTIO
		//@TODO[gra][8.20][08.04.2019]: Needs to be removed after LPT functionality no longer in PLC required (for timing purposes)
		if(m_isLptActive)
		{
			SVHWControlEnum controlType = (rTrigger.m_outputBit < cOutputCount / 2) ? SVControlWriteDigital0 : SVControlWriteDigital1_110;
			int8_t outputBit = static_cast<int8_t> (rTrigger.m_outputBit);
			outputBit = (SVControlWriteDigital0 == controlType) ? outputBit : outputBit - cOutputCount / 2;
			if (rTrigger.m_outputBit >= 0 && rTrigger.m_outputBit < cOutputCount)
			{
				auto setOutputThread = std::async(std::launch::async, [&] { SetOutputBitTimed(controlType, outputBit, m_resetOutputTime); });
			}
		}
#endif

		//If condition index is set then set the corresponding input
		if (-1 != rTrigger.m_conditionIndex)
		{
			WORD bitMask = 1 << rTrigger.m_conditionIndex;
			m_Input = rTriggerReport.m_isComplete ? m_Input |= bitMask : m_Input & ~bitMask;
		}
	}
	
	SvTh::IntVariantMap triggerData;
	triggerData[SvTh::TriggerDataEnum::TimeStamp] = _variant_t(now); //_variant_t(rTriggerReport.m_triggerTime_ms);
	triggerData[SvTh::TriggerDataEnum::ObjectID] = _variant_t(rTriggerReport.m_objectID);
	triggerData[SvTh::TriggerDataEnum::TriggerIndex] = _variant_t(rTriggerReport.m_triggerIndex);

	//RTX channel is zero based while SVObserver is one based!
	int triggerChannel = rTriggerReport.m_channel + 1;
	// call trigger callbacks
	for (auto ChannelAndDispatcherList : m_TriggerDispatchers.GetDispatchers())
	{
		//Trigger channel is one based
		if(triggerChannel == ChannelAndDispatcherList.first)
		{
			SvTh::DispatcherVector& list = ChannelAndDispatcherList.second;
			for (size_t i = 0;i < list.size();i++)
			{
				if (list[i].m_IsStarted)
				{
					list[i].SetData(triggerData);
					list[i].Dispatch();
				}
			}
			break;
		}
	}
}

void SVPlcIOImpl::WriteResult(int triggerIndex)
{
	ResultReport reportResult{static_cast<uint8_t> (triggerIndex), 0, 0, 0};
	const TriggerParameter& rTrigger = m_trigger[triggerIndex];
	//Reduce mutex scope
	{
		std::lock_guard<std::mutex> guard(m_protectPlc);
		reportResult.m_objectID = rTrigger.m_ObjectID;
		uint16_t bitMask = 1 << rTrigger.m_objectGoodIndex;
		reportResult.m_result = 0 != (m_Output & bitMask) ?  1 : 0;
		reportResult.m_outbits = m_Output;
#ifndef TRIGGER_SIMULATE
		writeResult(reportResult);
#endif
	}

	if (!m_OutputFileName.empty())
	{
		std::string outText = SvUl::Format(_T("Trigger = %d, ObjectID = %d, OutputValue=0x%x\n"), triggerIndex + 1, reportResult.m_objectID, reportResult.m_outbits);
		m_OutputData.emplace_back(outText);
	}
}


