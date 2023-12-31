//******************************************************************************
//* COPYRIGHT (c) 2014 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMPL
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <fstream>
//Moved to precompiled header: #include <time.h>
//Moved to precompiled header: #include <functional>
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVLptIOImpl.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "SVUtilityLibrary/SVClock.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage/SVMessage.h"
#include "SVStatusLibrary\MessageManagerHelper.h"
#pragma endregion Includes

constexpr int cPortNumber = 2;
constexpr int cInputCount = 8;
constexpr int cOutputCount = 16;
constexpr int RETRY = 5;
constexpr int cOneSecond = 1000;
constexpr LPCTSTR cTriggerName = "IO_Board_1.Dig_";			///This name must match the name in the SVHardwareManifest


// ******* Timeout for I/O board Acknowledge microseconds.
constexpr int BOARD_SELECT_ACK_TIMEOUT  = 16000; // 16 millisecs

// ******* Debug Data **********
//#define LogDebugData 

#ifdef LogDebugData
constexpr int MaxDebugData = 500;

struct TriggerDebugData
{
	double m_lTimeStamp1;
	double m_lTimeStamp2;
	double m_lParTimeStamp;
	unsigned short m_dStatusReg0;
	unsigned short m_dStatusReg1;
	unsigned short m_dStatusReg2;
};

long g_lCriticalSecInits = 0;
TriggerDebugData g_TDebugData[MaxDebugData];
long g_CallbackCount = 0;
#endif

SVLptIOImpl::~SVLptIOImpl()
{
	DisableAckInterrupt();
}

HRESULT SVLptIOImpl::Initialize(bool bInit)
{
	HRESULT hr = S_OK;

	if (bInit && !IsActive())
	{
		m_lLptTriggerEdge = 0L;
		m_lIOBrdTriggerEdge = 0L;
		SetActive(1);
	
		if (!IsActive())
		{
			hr = E_FAIL;
		}
		else
		{
			bool bBiDirectional;

			hr = IsBidirectionalSupported(bBiDirectional);
			if (S_OK == hr && !bBiDirectional)
			{
				hr = E_FAIL;
			}
		}
		if (S_OK == hr)
		{
			if (S_OK != WriteUnlockString())
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log );
				Exception.setMessage( SVMSG_LPTIO_INITIALIZATION_FAILED, SvStl::Tid_Empty , SvStl::SourceFileParams(StdMessageParams) );
				return SVMSG_LPTIO_INITIALIZATION_FAILED;
			}

			// Initialize previous Output States
			for (int i = 0; i < SVNumOutputPorts; i++)
			{
				m_nPreviousOutputs[i] = 0xff;
			}

			// Initialize previous Trigger State
			unsigned char status;
			hr = GetStatusPort(status);
			if (S_OK == hr)
			{
				m_lLastTriggerState = static_cast<long>(status);
				time_t timeVal;
				time(&timeVal);
				unsigned long lTime = static_cast<unsigned long>(timeVal);
				SetRTC(lTime);

				m_lBoardVersion = GetBoardVersion();
				//! Board Version 0 means invalid version
				if( 0 == m_lBoardVersion )
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Log );
					Exception.setMessage( SVMSG_IO_BOARD_VERSION, std::format( _T("{}"), m_lBoardVersion).c_str() , SvStl::SourceFileParams(StdMessageParams) );
					return SVMSG_IO_BOARD_VERSION;
				}
				// Tom says he's not sure what these numbers represent...
				if (0x1010101 == m_lBoardVersion || 0x7f7f7f7f == m_lBoardVersion)
				{
					m_lParallelBoardInterfaceBehavior = LegacyFunctions;
				}

			}
#ifdef LogDebugData
			memset(&g_TDebugData[0], 0, sizeof(g_TDebugData)) ;
#endif
		}
	}
	else
	{
#ifdef LogDebugData
		FILE* pfh = fopen("c:\\temp\\DebugCounts.txt","w");
		double LastTimeStamp = g_TDebugData[0].m_lTimeStamp1;
		fprintf(pfh, "Critical Section inits %d", g_lCriticalSecInits);
		for (int i = 0 ;i < g_CallbackCount; i++)
		{
			INT64 Tic = static_cast<INT64>(SvUl::ConvertTo(SvUl::Microseconds, g_TDebugData[i].m_lParTimeStamp));
			INT64 Start = static_cast<INT64>(SvUl::ConvertTo(SvUl::Microseconds, g_TDebugData[i].m_lTimeStamp1));
			INT64 Last = static_cast<INT64>(SvUl::ConvertTo(SvUl::Microseconds, (g_TDebugData[i].m_lTimeStamp1 - LastTimeStamp)));
			INT64 End = static_cast<INT64>(SvUl::ConvertTo(SvUl::Microseconds, g_TDebugData[i].m_lTimeStamp2));

			fprintf(pfh, "\nTrigger %04d\n", i);
			fprintf(pfh, "%I64d Tic \n", Tic);
			fprintf(pfh, "%I64d Start | Since last Int %I64d \n", Start, Last);
			fprintf(pfh, "%I64d End \n", End); 
			fprintf(pfh, "Status Register - %02x %02x %02x\n", g_TDebugData[i].m_dStatusReg0, g_TDebugData[i].m_dStatusReg1, g_TDebugData[i].m_dStatusReg2);
			LastTimeStamp = g_TDebugData[i].m_lTimeStamp1; 
		}
		fclose(pfh);
#endif
//		SetLogValue(0xF4); // what does 0xF4 represent?
		if (IsActive())
		{
			if (S_OK != WriteLockString())
			{
				SvStl::MessageManager Exception(SvStl::MsgType::Log );
				Exception.setMessage( SVMSG_LPTIO_INITIALIZATION_FAILED, SvStl::Tid_Empty , SvStl::SourceFileParams(StdMessageParams) );
				return SVMSG_LPTIO_INITIALIZATION_FAILED;
			}

			// Block the IRQ
			DisableAckInterrupt();

			// Unload the driver
			if (IsActive())
			{
				SetActive(0);  
			}
			// Stop all triggers
			
			m_triggerCallbackMap.clear();
		}
	}
	return hr;
}

unsigned long SVLptIOImpl::GetInputCount() const
{
	return cInputCount;
}

unsigned long SVLptIOImpl::GetOutputCount() const
{
	return cOutputCount;
}

unsigned long SVLptIOImpl::GetPortCount() const
{
	return cPortNumber;
}

unsigned long SVLptIOImpl::GetInputValue() const
{
	unsigned long result;
	SVReadWriteLpt(result, SVControlReadDigitalInputs);
	result &= 0xff;
	return result;
}

HRESULT SVLptIOImpl::SetOutputValue(unsigned long val)
{
	// This function will only write the SVIMs 16 outputs
	HRESULT hr = S_OK;

	int i = 0;
	for (i = 0; i < RETRY; i++)
	{
		unsigned long data = val & 0xff;
		if (S_OK != SVReadWriteLpt(data, SVControlWriteDigital0))
		{
			hr = S_FALSE;
		}
		data = (val >> 8) & 0xff;
		if (S_OK != SVReadWriteLpt(data, SVControlWriteDigital1))
		{
			hr = S_FALSE;
		}
		if (S_OK == hr)
		{
			break;
		}
		else
		{
			WriteUnlockString();
		}
	
	}
	if (i >= 2)
	{
		Log_Info(SvStl::Tid_RetryValueLarge);
	}
	// The camera control bits could be written along with the outputs since
	// there are only 16 output bits. Here is an Example:
	//	data = (val >> 16) & 0xff;
	//	if (S_OK != SVReadWriteLpt(data, SVControlCameraSetup))
	//	{
	//		hr = S_FALSE;
	//	}

	return hr;
}

HRESULT SVLptIOImpl::SetPortOutputValue(unsigned , unsigned long val)
{
	return SetOutputValue(val);
}

// GetBoardVersion
// Get Board Version sends a Sub Command value of 0x56 (GET_BOARD_VERSION)
// This puts the parallel board in a mode that will send the 
// Board version on the next four ControlReadCommands.
//  
long SVLptIOImpl::GetBoardVersion() const
{
	long result = 0;
	unsigned long lCommand = NO_CMD;
	SVReadWriteLpt(lCommand, SVControlCommand); // send a No Mode command to reset states.

	lCommand = GET_BOARD_VERSION;
	HRESULT hr = SVReadWriteLpt(lCommand, SVControlCommand);

	if (S_OK == hr)
	{
		for (int i = 0;i < 4;i++)
		{
			unsigned long lValue;
			if (S_OK == SVReadWriteLpt(lValue, SVControlReadCommand))
			{
				result = (result << 8) | lValue;
			}
		}
	}
	return result;
}

bool SVLptIOImpl::GetInputBit(unsigned long bitNum) const
{
	bool result{ false };
	unsigned long lValue=0;
	if (bitNum < 8)
	{
		SVReadWriteLpt(lValue, SVControlReadDigitalInputs, bitNum);
	}
	result = (0 != lValue) ? true : false;
	return result;
}

HRESULT SVLptIOImpl::SetOutputBit(unsigned long bitNum, bool bitVal)
{
	HRESULT hr = S_FALSE;

	unsigned long lData = bitVal ? 1 : 0;
	int i = 0;
	for (i = 0; i < RETRY; i++)
	{
		if (bitNum < 8)
		{
			hr = SVReadWriteLpt(lData, SVControlWriteDigital0, bitNum);
		}
		else if (bitNum < 16)
		{
			hr = SVReadWriteLpt(lData, SVControlWriteDigital1, bitNum - 8);
		}
		else if (bitNum < 24)
		{
			// Why the unknown magic number...
			if (0x7f7f7f7f == m_lBoardVersion && 20 <= bitNum)
			{
				lData = (lData == 1) ? 0 : 1;
			}

			hr = SVReadWriteLpt(lData, SVControlCameraSetup, bitNum - 16);
		}
		if (S_OK == hr)
		{
			break;
		}
		else
		{
			WriteUnlockString();
		}
	}
	if (i >= 2)
	{
		Log_Info(SvStl::Tid_RetryValueLarge);
	}
	return hr;
}


// SetBoardType
// Set Board Type sends a sub command to tell the parallel board 
// to be setup for X1 hardware or to be setup for D-Series hardware
// The valid sub commands are 0x58 and 0x59
HRESULT SVLptIOImpl::SetBoardType(unsigned long BoardType)
{
	HRESULT hr = S_OK;
	
	// Sub Command set board type 
	unsigned long tmpBoardType = SET_SYSTEM_TYPE;
	hr = SVReadWriteLpt(tmpBoardType, SVControlCommand);
	if (S_OK == hr)
	{
		hr = SVReadWriteLpt(BoardType, SVControlCommand);
	}
	return hr;
}

// GetBoardType
// Get Board Type sends a sub command to tell the parallel board 
// to send the board type on the next SVControlReadCommand.
HRESULT SVLptIOImpl::GetBoardType(unsigned long& rBoardType) const
{
	HRESULT hr = S_OK;
	
	unsigned long lCommand = GET_SYSTEM_TYPE;
	// Sub Command set board type 
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		unsigned long lTmp;
		hr = SVReadWriteLpt(lTmp, SVControlReadCommand);
		rBoardType = lTmp;
	}
	return hr;
}

// SetTriggerEdge
// Set Trigger Edge sets the trigger edge in the parallel I/O board
// So we only get the interrupts for the proper edge.
HRESULT SVLptIOImpl::SetTriggerEdge(unsigned long lTriggerEdge)
{
	HRESULT hr = S_OK;
	
	if (!(0 == m_lBoardVersion || 0x1010101 == m_lBoardVersion || 0x7f7f7f7f == m_lBoardVersion))
	{
		unsigned long lCommand = SET_TRIGGER_EDGE;
		// Sub Command set board type 
		hr = SVReadWriteLpt(lCommand, SVControlCommand);
		if (S_OK == hr)
		{
			lCommand = lTriggerEdge;
			hr = SVReadWriteLpt(lCommand, SVControlCommand);
		}
	}
	else
	{
		hr = S_FALSE; //SV_WRONG_BOARD_TYPE
	}
	return hr;
}

// GetFanState
// Get Fan State sends a sub command to tell the parallel board 
// to send the fan state on the next SVControlReadCommand.
HRESULT SVLptIOImpl::GetFanState(unsigned long& rFanState) const
{
	HRESULT hr = S_OK;
	
	// Sub Command Read Fan State 
	unsigned long lCommand = READ_FANS;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		unsigned long lTmp;
		hr = SVReadWriteLpt(lTmp, SVControlReadCommand);
		if (S_OK == hr)
		{
			rFanState = lTmp;
		}
	}
	return hr;
}

// This function is for debugging fan monitoring.
HRESULT SVLptIOImpl::GetFanFreq(long* plFanFreq) const
{
	HRESULT hr = S_OK;

	unsigned long lCommand = NO_CMD;
	SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = READ_FAN_FREQ;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0; i < 4; i++)
		{
			unsigned long lTmp{0L};
			hr = SVReadWriteLpt(lTmp, SVControlReadCommand);
			if (S_OK == hr)
			{
				*(plFanFreq + i) = lTmp;
			}
			else
			{
				break;
			}
		}
	}
	return hr;
}

// This function is for debugging IO Bits.
// It reads the log from the rabbit up to 1024 elements
HRESULT SVLptIOImpl::GetLog(unsigned long* pulTS, unsigned long* pucLog, long& lSize) const
{
	HRESULT hr = S_OK;

	unsigned long lCommand = NO_CMD;
	SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = READ_LOG;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0;i < lSize && S_OK  == hr;i++)
		{
			// Logged time stamp.
			unsigned long lTimeStamp{0L};
			unsigned long lTmp{0L};
			for (int k = 0;S_OK == hr && k < 4; k++)
			{
				hr = SVReadWriteLpt(lTmp, SVControlReadCommand);
				lTimeStamp += lTmp << (k * 8);
			}

			// Logged command
			if (S_OK == hr)
			{
				hr = SVReadWriteLpt(lTmp, SVControlReadCommand);
				if (S_OK == hr)
				{
					*(pulTS + i) = lTimeStamp;
					*(pucLog + i) = lTmp;
				}
				else
				{
					hr = S_OK;
					lSize = i;
					break;
				}
			}
			else
			{
				hr = S_OK;
				lSize = i;
				break;
			}
		}
	}
	return hr;
}

// This function is for debugging IO Bits.
HRESULT SVLptIOImpl::GetRTC(unsigned long& rValue) const
{
	HRESULT hr = S_OK;

	unsigned long lCommand = NO_CMD;
	SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = READ_RTC;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0;i < 4; i++)
		{
			unsigned long lTmp{0L};
			hr = SVReadWriteLpt(lTmp, SVControlReadCommand);
			if (S_OK == hr)
			{
				rValue += lTmp << (i * 8);
			}
			else
			{
				break;
			}
		}
	}
	return hr;
}

// This function is for debugging IO Bits.
// This function sends the RTC in 6 bytes.
// byte 0 - reset value is zero.
// byte 1 - Setup command to Write RTC
// byte 2 - bits 0-7
// byte 3 - bits 8-15
// byte 4 - bits 16-23
// byte 5 - bits 24-31
HRESULT SVLptIOImpl::SetRTC(unsigned long lValue)
{
	HRESULT hr = S_OK;

	unsigned long lCommand = NO_CMD;
	SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = WRITE_RTC;	// Get the Rabbit ready to receive a string of bytes for RTC set
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0;i < 4;i++)
		{
			unsigned long lTmp = lValue >> (i * 8);
			hr = SVReadWriteLpt(lTmp, SVControlCommand);	// Write 4 Bytes to fill a long in the Rabbit.
			if (S_OK != hr)
			{
				break;
			}
		}
	}
	return hr;
}

// This function is for debugging IO Bits.
// This function sends a value to the rabbit log in 2 bytes.
// byte 0 - Setup command to Write Log.
// byte 1 - data to write in log...
HRESULT SVLptIOImpl::SetLogValue(unsigned long lValue)
{
	HRESULT hr = S_OK;
	unsigned long lCommand = WRITE_LOG;	// Get the Rabbit ready to receive a string of bytes for RTC set
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		hr = SVReadWriteLpt(lValue, SVControlCommand);	// Write Log Byte
	}
	return hr;
}

unsigned long SVLptIOImpl::GetTriggerCount() const
{
	return cMaxLptTriggers;
}

unsigned long SVLptIOImpl::GetTriggerHandle(unsigned long index) const
{
	return (index + 1);
}

_variant_t SVLptIOImpl::GetTriggerName(unsigned long triggerIndex) const
{
	_variant_t result;
	std::string triggerName {cTriggerName};
	triggerName += std::to_string(triggerIndex - 1);
	result.SetString(triggerName.c_str());
	return result;
}

void SVLptIOImpl::beforeStartTrigger(unsigned long)
{
}


HRESULT SVLptIOImpl::afterStartTrigger()
{
	// Initialize previous Trigger State
	unsigned char status;
	HRESULT result = GetStatusPort(status);
	if (S_OK == result)
	{
		m_lLastTriggerState = status;
		if( !isIrqHandlerEnabled() )
		{
			::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
			IrqHandler irqHandler = [this]() { return HandleIRQ(); };
			result = EnableAckInterrupt(irqHandler);
		}
	}

	return result;
}


HRESULT SVLptIOImpl::afterStopTrigger()
{
	if (true == m_triggerCallbackMap.empty()) // Make sure all triggers are stopped before Masking the IRQ
	{
		DisableAckInterrupt();
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}

	return S_OK;
}



unsigned long SVLptIOImpl::TriggerGetParameterCount(unsigned long) const
{
	return 3UL;
}

_variant_t SVLptIOImpl::TriggerGetParameterName(unsigned long, unsigned long index) const
{
	_variant_t result;

	// SVSignalEdge and SVBoardVersion enums are used here to make the code more clear.
	// however at some time in the future the Dll parameters may be implemented
	// as an array and therefore this enum may not apply.
	switch (index)
	{
	case SVSignalEdge:
		result.SetString(_T("Edge"));
		break;
	case SVSingleTriggerEdge:
		result.SetString(_T("Single Trigger Edge"));
		break;
	case SVBoardVersion:
		result.SetString(_T("Board Version"));
		break;
	default:
		break;
	}
	return result;
}

_variant_t SVLptIOImpl::TriggerGetParameterValue(unsigned long triggerIndex, unsigned long index) const
{
	_variant_t result;

	switch (index)
	{
		case SVSignalEdge:
		{
			long lMask{ 0L };
			switch (triggerIndex)
			{
				case 1:
				{
					lMask = SVLptIOImpl::SVTrigger1;
					break;
				}
				case 2:
				{
					lMask = SVLptIOImpl::SVTrigger2;
					break;
				}
				case 3:
				{
					lMask = SVLptIOImpl::SVTrigger3;
					break;
				}
				case 4:
				{
					lMask = SVLptIOImpl::SVTrigger4;
					break;
				}
				default:
					break;
			}

			result = ((m_lLptTriggerEdge & lMask) == lMask) ? 1L : -1L;
		}

		case SVBoardVersion:
		{
			long lVer = GetBoardVersion();
			std::string version;
			version = std::format(_T("Dedicated I/O Processor board {}.{:02d} firmware {}.{:02d}"),
				lVer >> 24 & 0xff,
				lVer >> 16 & 0xff,
				lVer >> 8 & 0xff,
				lVer & 0xff);
			result.SetString(version.c_str());
			break;
		}
	}
	return result;
}

HRESULT SVLptIOImpl::TriggerSetParameterValue(unsigned long triggerIndex, unsigned long index, const _variant_t& rValue)
{
	HRESULT result{ E_FAIL };

	switch(index)
	{
		case SVSignalEdge:
		{
			if (VT_I4 == rValue.vt)
			{
				long lMask = 0;

				switch (triggerIndex)
				{
					case 1: 
					{
						lMask = SVLptIOImpl::SVTrigger1;
						break;
					}
					case 2:
					{
						lMask = SVLptIOImpl::SVTrigger2;
						break;
					}
					case 3:
					{
						lMask = SVLptIOImpl::SVTrigger3;
						break;
					}
					case 4:
					{
						lMask = SVLptIOImpl::SVTrigger4;
						break;
					}
				}

				switch (rValue.lVal)
				{
					case 1:
					{
						m_lLptTriggerEdge |= lMask;
						m_lIOBrdTriggerEdge |= 1 << (triggerIndex -1);
						break;
					}
					case -1:
					{
						m_lLptTriggerEdge &= ~lMask;
						m_lIOBrdTriggerEdge &= ~(1 << (triggerIndex -1));
						break;
					}
					default:
					{
						break;
					}
				}
				result = SetTriggerEdge(m_lIOBrdTriggerEdge);
			}
			break;
		}

		case SVSingleTriggerEdge:
		{
			if (VT_I4 == rValue.vt)
			{
				m_bUseSingleTrigger = (0 == rValue.lVal) ? false : true;
			}
			if (VT_BOOL == rValue.vt)
			{
				m_bUseSingleTrigger = rValue.bVal ? true : false;
			}
			break;
		}

		default:
			break;
	}
	return result;
}

unsigned long SVLptIOImpl::GetParameterCount() const
{
	return 4UL;
}

_variant_t SVLptIOImpl::GetParameterName(unsigned long index) const
{
	_variant_t result;

	switch (index)
	{
		case SVIOParameterEnum::SVModuleReady:
		{
			result.SetString(_T("Module Ready(R/W)"));
			break;
		}
		case SVIOParameterEnum::SVBoardVersion:
		{
			result.SetString(_T("Board Version(R)"));
			break;
		}
		case SVIOParameterEnum::SVBoardType:
		{
			result.SetString(_T("Board Type(R/W)"));
			break;
		}
		case SVIOParameterEnum::SVFanState:
		{
			result.SetString(_T("Fan State(R)"));
			break;
		}
		case SVIOParameterEnum::SVFanFreq:
		{
			result.SetString(_T("Fan Frequency"));
			break;
		}
		default:
			break;
	}
			
	return result;
}

_variant_t SVLptIOImpl::GetParameterValue(unsigned long index) const
{
	_variant_t result;

	switch(index)
	{
		case SVIOParameterEnum::SVModuleReady:
		{
			result = m_moduleReady ? TRUE : FALSE;
			break;
		}
		case SVIOParameterEnum::SVBoardVersion:
		{
			long lVer = GetBoardVersion();
			std::string version = std::format(_T("Dedicated I/O Processor board {}.{:02d} firmware {}.{:02d}"),
				lVer >> 24 & 0xff,
				lVer >> 16 & 0xff,
				lVer >> 8 & 0xff,
				lVer & 0xff);
			result.SetString(version.c_str());
			break;
		}
		case SVIOParameterEnum::SVBoardType:
		{
			unsigned long type;
			if (S_OK == GetBoardType(type))
			{
				result = static_cast<long> (type);
			}
			break;
		}
		case SVIOParameterEnum::SVFanState:
		{
			unsigned long fanState;
			if (S_OK == GetFanState(fanState))
			{
				result = static_cast<long> (fanState);
			}
			break;
		}
		case SVIOParameterEnum::SVFanFreq:
		{
			long lTmp[4];
			if (S_OK == GetFanFreq(lTmp))
			{	// The results are only 8 bits each so stuff all 4 into a 32bit long.
				// This is just for testing.
				// If this is something we want to support then it should be wrapped up in a safearray of longs.
				result = (lTmp[0] & 0xff) |
					((lTmp[1] & 0xff) << 8) |
					((lTmp[2] & 0xff) << 16) | 
					((lTmp[3] & 0xff) << 24);  
			}
			break;
		}
		case SVIOParameterEnum::SVLogDump:
		{
			unsigned long LogCode[1024];
			unsigned long TimeStamp[1024];

			long Size = 2048;
			memset(LogCode, 0, Size);

			// Instead of sending the data back through a SafeArray, put it directly into a file here.
			if (S_OK == GetLog(TimeStamp, LogCode, Size))
			{
				FILE* fh = fopen("c:\\SVObserver\\IOBoardLog.txt", "a");
				if (nullptr != fh)
				{
					time_t timeVal;
					time(&timeVal);
					struct tm *newTime;
					newTime = localtime(&timeVal);
					char strTmp[50];
					::strftime(strTmp, 50, "%#c", newTime);
					size_t size = strlen(strTmp);
					if (size >= 25) // get rid of the newline from asctime.
						strTmp[24] = 0;

					fprintf(fh, "\nNew Log Entry %s\n", strTmp);
					for (int i = 0;i < Size; i++)
					{
						timeVal = TimeStamp[i];
						newTime = localtime(&timeVal);
						::strftime(strTmp, 50, "%#c", newTime);
						size = strlen(strTmp);
						if (size >= 25) // get rid of the newline from asctime.
						{
							strTmp[24] = 0;
						}
						fprintf(fh, "%s >function %02lx\n", strTmp, LogCode[i]);
					}
					fclose(fh);
				}
			}
			break;
		}
		case SVIOParameterEnum::SVRabbitRTC:
		{
			unsigned long lRTC = 0;
			if (S_OK == GetRTC(lRTC))
			{
				result = static_cast<long> (lRTC);
			}
			break;
		}
		default:
			break;
	}
	return result;
}

HRESULT SVLptIOImpl::SetParameterValue(unsigned long Index, const _variant_t& rValue)
{
	HRESULT result{ E_NOTIMPL };

	switch(Index)
	{
		case SVIOParameterEnum::SVModuleReady:
		{
			if (VT_BOOL == rValue.vt)
			{
				m_moduleReady = rValue.boolVal ? true : false;
				result = S_OK;
			}
			break;
		}
		case SVIOParameterEnum::SVBoardType:
		{
			if (VT_I4 == rValue.vt)
			{
				long lBoardType = rValue.lVal;
				if (S_OK == TranslateBoardType(lBoardType))
				{
					result = SetBoardType(lBoardType);
				}
				else
				{
					result = SetBoardType(rValue.lVal);
				}
			}
			break;
		}
		case SVIOParameterEnum::SVRabbitRTC:
		{
			if (VT_I4 == rValue.vt)
			{
				result = SetRTC(rValue.lVal);
			}
			break;
		}
		case SVIOParameterEnum::SVRabbitWriteLog:
		{
			if (VT_I4 == rValue.vt)
			{
				SetLogValue(rValue.lVal);
				result = S_OK;
			}
			break;
		}
		default:
			break;
	}
	return result;
}

short SVLptIOImpl::GetPreviousOutputs(long lControl) const
{
	short nPrevOutputs = 0;
	switch (lControl)
	{
		case SVControlWriteDigital0:
		{
			nPrevOutputs = m_nPreviousOutputs[0];
			break;
		}
		case SVControlWriteDigital1:
		{
			nPrevOutputs = m_nPreviousOutputs[1];
			break;
		}
		case SVControlCameraSetup:
		{
			nPrevOutputs = m_nPreviousOutputs[2];
			break;
		}
	}
	return nPrevOutputs;
}

void SVLptIOImpl::SetPreviousOutputs(long lControl, short sValue) const
{
	switch (lControl)
	{
		case SVControlWriteDigital0:
		{
			m_nPreviousOutputs[0] = sValue;
			break;
		}
		case SVControlWriteDigital1:
		{
			m_nPreviousOutputs[1] = sValue;
			break;
		}
		case SVControlCameraSetup:
		{
			m_nPreviousOutputs[2] = sValue;
			break;
		}
		default:
		{
			// Do nothing for other control types..
			break;
		}
	}
}

// Reads the lock State
// Byte 0 - Command to Read Lock State
// Byte 1 - Read Back lock state.
HRESULT SVLptIOImpl::GetLockState(bool& bLocked)
{
	HRESULT hr = S_OK;
	
	unsigned long lCommand = READ_LOCK_STATE;
	// Sub Command set board type 
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		unsigned long lTmp;
		hr = SVReadWriteLpt(lTmp, SVControlReadCommand);
		bLocked = (1 == lTmp);
	}
	return hr;
}

// Sends a command 0x55 to the Parallel board
// to enable the outputs.
HRESULT SVLptIOImpl::WriteUnlockString()
{
	HRESULT hr = S_OK;
	LPCSTR pString = "Unlock";
	
	long Retry = RETRY;
	bool bLocked = true;

	while (Retry > 0)
	{
		// Resets parallel board sub command sequence.
		unsigned long lCommand {NO_CMD};
		SVReadWriteLpt(lCommand, SVControlCommand);
		
		lCommand = UNLOCK_CMD;
		hr = SVReadWriteLpt(lCommand, SVControlCommand);
		for (int i = 0;i < 6 && S_OK == hr; i++)
		{
			lCommand = *(pString + i);
			hr = SVReadWriteLpt(lCommand, SVControlCommand);
		}
		if (S_OK == hr)	// Check if IO is actually unlocked
		{
			hr = GetLockState(bLocked);
			if( bLocked )
			{
				Sleep( cOneSecond );
			}
			else
			{
				break;
			}
		}
		Retry--;
	}
	if (Retry  < RETRY -2)
	{
		Log_Info(SvStl::Tid_RetryValueLarge);
	}
	return hr;
}

// WriteLockString 
// Sends a command 0x55 to the Parallel board
// to disable the outputs.
HRESULT SVLptIOImpl::WriteLockString()
{
	HRESULT hr = S_OK;

	LPCSTR pString = "Lock";

	long Retry = RETRY;
	bool bLocked = false;

	while (Retry > 0)
	{
		// Resets parallel board sub command sequence.
		unsigned long lCommand {NO_CMD};
		SVReadWriteLpt(lCommand, SVControlCommand);
		
		lCommand = LOCK_CMD;
		hr = SVReadWriteLpt(lCommand, SVControlCommand);
		for (int i = 0; i < 4 && S_OK == hr; i++)
		{
			lCommand = *(pString + i);
			hr = SVReadWriteLpt(lCommand, SVControlCommand);
		}
		if (S_OK == hr)
		{
			hr = GetLockState(bLocked);
			if( bLocked )
			{
				break;
			}
			else
			{
				Sleep( cOneSecond );
			}
		}
		Retry--;
	}
	if (Retry < RETRY - 2)
	{
		Log_Info(SvStl::Tid_RetryValueLarge);
	}
	return hr;
}

// This logic in this function is where we will match up SVObserver versions and Parallel board versions.
HRESULT SVLptIOImpl::TranslateBoardType(long& rBoardType)
{
	// At this time the versions have only one split at Rabbit version 1.10 and SVObserver 5.21+ which will not
	// have this function 
	if ((m_lBoardVersion & 0xffff) >= 0x10a && m_lBoardVersion != 0x1010101 && m_lBoardVersion != 0x7f7f7f7f)
	{
		rBoardType |= 0x40;
		m_lParallelBoardInterfaceBehavior = Function00ForWrite1;
	}
	else
	{
		m_lParallelBoardInterfaceBehavior = LegacyFunctions;
	}
	return S_OK;
}

HRESULT SVLptIOImpl::SVReadWriteLpt(unsigned long& rlValue, long prevControl, long lBit) const
{
	HRESULT hr = S_OK;

	std::lock_guard<std::mutex> guard(m_protectIO);
	// Reset the Quantum
	HANDLE hThread = ::GetCurrentThread();
	DWORD dwPriority = ::GetThreadPriority(hThread);
	::SetThreadPriority(hThread, dwPriority);

	unsigned char nPrevControl;
	hr = GetControlPort(nPrevControl);
	if (S_OK == hr)
	{
		if (0 != (nPrevControl & 0xf))
		{
			if (!m_isFirstTimeToReadOrWrite)
			{
				// The following warning appeared once (probably spuriously) during SVIM startup.
				// Suppressing it the first time solved this problem
				SvStl::MessageManager Exception(SvStl::MsgType::Log);
				Exception.setMessage(SVMSG_INVALID_LINE_STATE, SvStl::Tid_Lpt_WrongState, SvStl::SourceFileParams(StdMessageParams));
			}
		}
		m_isFirstTimeToReadOrWrite = false;
		// Get Value of control port interrupt Bit 
		unsigned char nVal = SVControlEnableInterrupt;
		hr = SetControlPort(nVal);	// Set Control to known state with select line off...
		if (S_OK == hr)
		{
			unsigned char n;
			hr = GetControlPort(n);
			if (S_OK == hr)
			{
				unsigned char lPort = n & 0x37; // what is with the magic numbers...
				if (lPort != nVal)
				{
					hr = S_FALSE;
				}
			}
		}
		double Start = SvUl::GetTimeStamp();

		// **** Wait for Acknowledge off...
		unsigned char status;
		hr = GetStatusPort(status);
		while (S_OK == hr && 0 == (status & 128))
		{
			double Check = SvUl::GetTimeStamp();

			if (SvUl::ConvertTo(SvUl::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
			{
				hr = GetStatusPort(status);
				if (S_OK == hr && 0 != (status & 128))
				{
					hr = S_FALSE;
				}
				break;
			}
			hr = GetStatusPort(status);
		}

		long lControl = prevControl;
		switch (lControl)
		{
			case SVControlWriteDigital1:
			{
				if (Function00ForWrite1 == m_lParallelBoardInterfaceBehavior)
				{
					lControl = SVControlWriteDigital1_110;
				}
			}
			case SVControlCameraSetup:
			case SVControlWriteDigital0:
			case SVControlCommand:
			{
				hr = SetControlPort(static_cast<unsigned char>(nVal | lControl));	// Move Data to outputs
				if (S_OK == hr)
				{
					short nNewOutput;
					if (lBit < 0) // no bits  -  use entire port.
					{
						nNewOutput = static_cast<unsigned char>(rlValue);
					}
					else
					{			// use bits.
						if (rlValue)
						{
							nNewOutput = GetPreviousOutputs(prevControl) | (1 << lBit);
						}
						else
						{
							nNewOutput = GetPreviousOutputs(prevControl) & ~(1 << lBit);
						}
					}
					hr = SetDataPort(static_cast<unsigned char> (nNewOutput));					// Write Data to Data Port
					if (S_OK == hr)
					{
						SetPreviousOutputs(prevControl, nNewOutput); //
						hr = SetControlPort(static_cast<unsigned char>(nVal | lControl | SVControlSelectMode));
						if (S_OK == hr)
						{
							// **** Wait for Acknowledge...
							Start = SvUl::GetTimeStamp();
							hr = GetStatusPort(status);
							while (S_OK == hr && 0 != (status & 128))
							{
								double Check = SvUl::GetTimeStamp();
								if (SvUl::ConvertTo(SvUl::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
								{
									hr = GetStatusPort(status);
									if (S_OK == hr && 0 != (status & 128))
									{
										hr = S_FALSE;
									}
									break;
								}
								hr = GetStatusPort(status);
							}
							if (S_OK == hr)
							{
								hr = SetControlPort(static_cast<unsigned char>(nVal | lControl));
							}
						}
					}
				}
				break;
			}
			case SVControlReadDigitalInputs:
			case SVControlReadCommand:
			{
				hr = SetControlPort(static_cast<unsigned char>(nVal | SVControlBidirectional | prevControl));
				if (S_OK == hr)
				{
					hr = SetControlPort(static_cast<unsigned char>(nVal | SVControlBidirectional | prevControl | SVControlSelectMode));
					if (S_OK == hr)
					{
						// **** Wait for Acknoledge...
						Start = SvUl::GetTimeStamp();
						hr = GetStatusPort(status);
						while (S_OK == hr && 0 != (status & 128))
						{
							double Check = SvUl::GetTimeStamp();
							if (SvUl::ConvertTo(SvUl::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
							{
								hr = GetStatusPort(status);
								if (S_OK == hr && 0 != (status & 128))
								{
									hr = S_FALSE;
								}
								break;
							}
							hr = GetStatusPort(status);
						}
						if (S_OK == hr && lBit < 0)
						{
							unsigned char value;
							hr = GetDataPort(value);
							if (S_OK == hr)
							{
								rlValue = static_cast<long>(value);
							}
						}
						else
						{
							unsigned char value;
							hr = GetDataPort(value);
							if (S_OK == hr)
							{
								rlValue = (static_cast<long>(value) & (1 << lBit)) ? 1 : 0;
							}
						}
						if (S_OK == hr)
						{
							hr = SetControlPort(static_cast<unsigned char>(nVal | SVControlBidirectional | prevControl));
							if (S_OK == hr)
							{
								hr = SetControlPort(static_cast<unsigned char>(nVal | SVControlBidirectional));
							}
						}
					}
				}
				break;
			}
			default:
			{
				hr = S_FALSE;
				break;
			}
		}
		// Put control port back in to No Mode 
		HRESULT hrTmp = SetControlPort(nVal);
		if (S_OK == hr)
		{
			hr = hrTmp;
		}
	}

	return hr;
}

LPCTSTR SVLptIOImpl::GetControlText(long lControl)
{
	LPCTSTR pString = nullptr;
	{
		switch (lControl)
		{
			case SVLptIOImpl::SVControlCameraSetup:
			{
				pString = "Camera Setup";
				break;
			}
			case SVLptIOImpl::SVControlWriteDigital0:
			{
				pString = "Write Digital Bank 0";
				break;
			}
			case SVLptIOImpl::SVControlWriteDigital1:
			{
				pString = "Write Digital Bank 1";
				break;
			}
			case SVLptIOImpl::SVControlReadDigitalInputs:
			{
				pString = "Read Digital Inputs";
				break;
			}
			case SVLptIOImpl::SVControlReadCommand:
			{
				pString = "Command Read";
				break;
			}
			case SVLptIOImpl::SVControlCommand:
			{
				pString = "Command Write";
				break;
			}
		}
	}
	return pString;
}

void SVLptIOImpl::HandleIRQ()
{
	/// Only handle irq if the module ready is set this avoids problems with the PPQ Object not being ready
	if(false == m_moduleReady)
	{
		return;
	}
	double timeStamp = SvUl::GetTimeStamp();

#ifdef LogDebugData
	g_TDebugData[g_CallbackCount].m_dStatusReg0 = StatusReg;
	g_TDebugData[g_CallbackCount].m_dStatusReg1 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp1 = SvUl::GetTimeStamp();

	LARGE_INTEGER Frequency;
	LARGE_INTEGER Timestamp;

	::QueryPerformanceFrequency(&Frequency);

	Timestamp.LowPart = LoTime;
	Timestamp.HighPart = HiTime;

	double Seconds = static_cast< double >(Timestamp.QuadPart) / static_cast< double >(Frequency.QuadPart);

	g_TDebugData[g_CallbackCount].m_lParTimeStamp = SvUl::ConvertFrom(SvUl::Seconds, Seconds);
#endif

	unsigned char StatusReg;
	HRESULT hr = GetStatusPort(StatusReg);
	if (S_OK == hr)
	{
		for(unsigned long i = 1; i <= cMaxLptTriggers; ++i)
		{
			short nTriggerBit = SVTriggerNone;

			// Get the trigger bit 
			switch (i) //the 1 based channel.
			{
				case 1: // trigger 1 Bit 3 of Status port
				{
					nTriggerBit = SVTrigger1;
					break;
				}
				case 2:
				{
					nTriggerBit = SVTrigger2;
					break;
				}
				case 3:
				{
					nTriggerBit = SVTrigger3;
					break;
				}
				default:
				{
					break;
				}
			}

			if (((nTriggerBit != SVTriggerNone) &&
				((nTriggerBit & StatusReg) != (nTriggerBit & m_lLastTriggerState)) && 
				 ((nTriggerBit & StatusReg) == (nTriggerBit & m_lLptTriggerEdge))) || m_bUseSingleTrigger)
			{
				SvTrig::TriggerData triggerData;
				triggerData.m_triggerTimestamp = timeStamp;
				///Trigger channel 0 based
				triggerData.m_channel = static_cast<uint8_t> (i - 1);

				auto iter = m_triggerCallbackMap.find(i);
				if (m_triggerCallbackMap.end() != iter)
				{
					iter->second(std::move(triggerData));
				}
			}
		}
		m_lLastTriggerState = StatusReg;
	}

#ifdef LogDebugData
	// **** Debug logging ****
	g_TDebugData[g_CallbackCount].m_dStatusReg2 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp2 = SvUl::GetTimeStamp();
	if (g_CallbackCount < MaxDebugData)
		g_CallbackCount++; //*/
#endif
}

