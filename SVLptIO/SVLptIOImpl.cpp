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
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <string>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVLptIOImpl.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "SVTimerLibrary\SVClock.h"
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#include "SVUtilityLibrary/StringHelper.h"
#pragma endregion Includes

constexpr int cPortNumber = 2;
constexpr int cInputCount = 8;
constexpr int cOutputCount = 16;
constexpr int RETRY = 2;
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

				// The Rabbit board version is read here.
				for (int i = 0;i < RETRY; i++)
				{
					if (S_OK == GetBoardVersion(m_lBoardVersion))
					{
						break;
					}
				}
				//! Board Version 0 means invalid version
				if( 0 == m_lBoardVersion )
				{
					SvStl::MessageManager Exception(SvStl::MsgType::Log );
					Exception.setMessage( SVMSG_IO_BOARD_VERSION, SvUl::Format( _T("%d"), m_lBoardVersion).c_str() , SvStl::SourceFileParams(StdMessageParams) );
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
			INT64 Tic = static_cast<INT64>(SvTl::ConvertTo(SvTl::Microseconds, g_TDebugData[i].m_lParTimeStamp));
			INT64 Start = static_cast<INT64>(SvTl::ConvertTo(SvTl::Microseconds, g_TDebugData[i].m_lTimeStamp1));
			INT64 Last = static_cast<INT64>(SvTl::ConvertTo(SvTl::Microseconds, (g_TDebugData[i].m_lTimeStamp1 - LastTimeStamp)));
			INT64 End = static_cast<INT64>(SvTl::ConvertTo(SvTl::Microseconds, g_TDebugData[i].m_lTimeStamp2));

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

unsigned long SVLptIOImpl::GetInputCount()
{
	return cInputCount;
}

unsigned long SVLptIOImpl::GetOutputCount()
{
	return cOutputCount;
}

unsigned long SVLptIOImpl::GetPortCount()
{
	return cPortNumber;
}

HRESULT SVLptIOImpl::GetInputValue(unsigned long* pVal)
{
	SVReadWriteLpt(*pVal, SVControlReadDigitalInputs);
	*pVal = *pVal & 0xff;
	return S_OK;
}

HRESULT SVLptIOImpl::SetOutputValue(unsigned long val)
{
	// This function will only write the SVIMs 16 outputs
	HRESULT hr = S_OK;

	for (int i = 0; i < RETRY; i++)
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
HRESULT SVLptIOImpl::GetBoardVersion(long& rlVer)
{
	HRESULT hr = S_OK;
	long lVersion = 0;
	unsigned long lCommand = NO_CMD;
	SVReadWriteLpt(lCommand, SVControlCommand); // send a No Mode command to reset states.

	lCommand = GET_BOARD_VERSION;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);

	if (S_OK == hr)
	{
		for (int i = 0;i < 4;i++)
		{
			unsigned long lValue;
			HRESULT tmphr = SVReadWriteLpt(lValue, SVControlReadCommand);
			if (S_OK != tmphr)
			{
				hr = tmphr;
			}
			else
			{
				lVersion = (lVersion << 8) | lValue;
			}
		}
	}
	rlVer = lVersion;
	return hr;
}

HRESULT SVLptIOImpl::GetInputBit(unsigned long bitNum, bool& rBitVal)
{
	HRESULT hr = S_OK;
	unsigned long lValue=0;

	if (bitNum < 8)
	{
		SVReadWriteLpt(lValue, SVControlReadDigitalInputs, bitNum);
	}
	rBitVal = lValue ? true : false;
	return hr;
}

HRESULT SVLptIOImpl::SetOutputBit(unsigned long bitNum, bool bitVal)
{
	HRESULT hr = S_FALSE;

	unsigned long lData = bitVal ? 1 : 0;

	for (int i = 0; i < RETRY; i++)
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
HRESULT SVLptIOImpl::GetBoardType(unsigned long& rBoardType)
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
HRESULT SVLptIOImpl::GetFanState(unsigned long& rFanState)
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
HRESULT SVLptIOImpl::GetFanFreq(long* plFanFreq)
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
HRESULT SVLptIOImpl::GetLog(unsigned long* pulTS, unsigned long* pucLog, long& lSize)
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
HRESULT SVLptIOImpl::GetRTC(unsigned long& rValue)
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

unsigned long SVLptIOImpl::GetTriggerCount()
{
	return cMaxLptTriggers;
}

unsigned long SVLptIOImpl::GetTriggerHandle(unsigned long index)
{
	return (index + 1);
}

BSTR SVLptIOImpl::GetTriggerName(unsigned long triggerIndex)
{
	std::string triggerName {cTriggerName};

	triggerName += std::to_string(triggerIndex - 1);
	BSTR name = _bstr_t(triggerName.c_str()).Detach();
	return name;
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
			result = EnableAckInterrupt(boost::bind(&SVLptIOImpl::HandleIRQ, this));
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



HRESULT SVLptIOImpl::TriggerGetParameterCount(unsigned long, unsigned long* pCount)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pCount)
	{
		*pCount = 3;
		hr = S_OK;
	}
	return hr;
}

HRESULT SVLptIOImpl::TriggerGetParameterName(unsigned long, unsigned long Index, BSTR* pName)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pName)
	{
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
			*pName = nullptr;
		}

		// SVSignalEdge and SVBoardVersion enums are used here to make the code more clear.
		// however at some time in the future the Dll parameters may be implemented
		// as an array and therefore this enum may not apply.
		if (SVSignalEdge == Index)
		{
			*pName = ::SysAllocString(L"Edge");
		}
		else if (SVSingleTriggerEdge == Index)
		{
			*pName = ::SysAllocString(L"Single Trigger Edge");
		}
		else if (SVBoardVersion == Index)
		{
			*pName = ::SysAllocString(L"Board Version");
		}
		if (nullptr != *pName)
		{
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::TriggerGetParameterValue(unsigned long triggerIndex, unsigned long Index, VARIANT* pValue)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pValue && S_OK == ::VariantClear(pValue))
	{
		// SVSignalEdge enum is used here to make the code more clear.
		// however at some time in the future the Dll parameters may be implemented
		// as an array and therefore this enum may not apply.
		if (SVSignalEdge == Index)
		{
			long lMask{0};
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

			pValue->vt = VT_I4;

			if ((m_lLptTriggerEdge & lMask) == lMask)
			{
				pValue->lVal = 1;
			}
			else
			{
				pValue->lVal = -1;
			}
			hr = S_OK;
		}
		else if (SVBoardVersion == Index)
		{
			long lVer;
			WCHAR wbuf[256];
			pValue->vt = VT_BSTR;
			hr = GetBoardVersion(lVer);
			swprintf(wbuf, L"Dedicated I/O Processor board %ld.%02ld firmware %ld.%02ld",
					 lVer >> 24 & 0xff,
					 lVer >> 16 & 0xff,
					 lVer >> 8 & 0xff,
					 lVer & 0xff);
			pValue->bstrVal = ::SysAllocString(wbuf);
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::TriggerSetParameterValue(unsigned long triggerIndex, unsigned long Index, VARIANT* pValue)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pValue)
	{
		// SVSignalEdge enum is used here to make the code more clear.
		// however at some time in the future the Dll parameters may be implemented
		// as an array and therefore this enum may not apply.
		if (SVSignalEdge == Index)
		{
			if (VT_I4 == pValue->vt)
			{
				long lMask = 0; //1 << (triggerchannel - 1);

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

				switch (pValue->lVal)
				{
					case 1:
					{
						m_lLptTriggerEdge |= lMask;
						m_lIOBrdTriggerEdge |= 1 << (triggerIndex -1);

						hr = S_OK;
						break;
					}
					case -1:
					{
						m_lLptTriggerEdge &= ~lMask;
						m_lIOBrdTriggerEdge &= ~(1 << (triggerIndex -1));

						hr = S_OK;
						break;
					}
					default:
					{
						break;
					}
				}
				hr = SetTriggerEdge(m_lIOBrdTriggerEdge);
			}
		}
		else if (SVSingleTriggerEdge == Index) // Use Single Trigger Methodology
		{
			if (VT_I4 == pValue->vt)
			{
				if (0 == pValue->lVal)
				{
					m_bUseSingleTrigger = false;
				}
				else
				{
					m_bUseSingleTrigger = true;
				}
			}
			if (VT_BOOL == pValue->vt)
			{
				m_bUseSingleTrigger = pValue->bVal == TRUE;
			}
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::GetParameterCount(unsigned long* pCount)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pCount)
	{
		*pCount = 3;
		hr = S_OK;
	}
	return hr;
}

HRESULT SVLptIOImpl::GetParameterName(unsigned long Index, BSTR* pName)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pName)
	{
		if (nullptr != *pName)
		{
			::SysFreeString(*pName);
			*pName = nullptr;
		}

		if (SVBoardVersion == Index)
		{
			*pName = ::SysAllocString(L"Board Version(R)");
		}
		else if (SVFanState == Index)
		{
			*pName = ::SysAllocString(L"Fan State(R)");
		}
		else if (SVBoardType == Index)
		{
			*pName = ::SysAllocString(L"Board Type(R/W)");
		}
		else if (SVFanFreq == Index)
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

HRESULT SVLptIOImpl::GetParameterValue(unsigned long Index, VARIANT* pValue)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pValue)
	{
		if (S_OK == ::VariantClear(pValue))
		{
			// Board Version
			if (SVBoardVersion == Index)
			{
				long lVer;
				WCHAR wbuf[256];
				pValue->vt = VT_BSTR;
				hr = GetBoardVersion(lVer);
				swprintf(wbuf, L"Dedicated I/O Processor board %d.%02d firmware %d.%02d",
					lVer >> 24 & 0xff,
					lVer >> 16 & 0xff,
					lVer >> 8 & 0xff,
					lVer & 0xff);
				pValue->bstrVal = ::SysAllocString(wbuf);
			}
			else if (SVBoardType == Index)			// Board Type : X-Series, D-Series, or A-Series Boards.
			{
				unsigned long lType;
				hr = GetBoardType(lType);
				pValue->vt = VT_I4;
				if (S_OK == hr)
				{
					pValue->lVal = lType;
				}
			}
			else if (SVFanState == Index)			// Fan State
			{
				unsigned long lFanState;
				pValue->vt = VT_I4;
				hr = GetFanState(lFanState);
				pValue->lVal = lFanState;
			}
			else if (SVFanFreq == Index)
			{
				long lTmp[4];
				hr = GetFanFreq(lTmp);
				if (S_OK == hr)
				{	// The results are only 8 bits each so stuff all 4 into a 32bit long.
					// This is just for testing.
					// If this is something we want to support then it should be wrapped up in a safearray of longs.
					pValue->vt = VT_I4;
					pValue->lVal = (lTmp[0] & 0xff) |
						((lTmp[1] & 0xff) << 8) |
						((lTmp[2] & 0xff) << 16) | 
						((lTmp[3] & 0xff) << 24);  
				}
			}
			else if (SVLogDump == Index)
			{
				unsigned long LogCode[1024];
				unsigned long TimeStamp[1024];

				long Size = 2048;
				memset(LogCode, 0, Size);
				hr = GetLog(TimeStamp, LogCode, Size);

				// Instead of sending the data back through a SafeArray, put it directly into a file here.
				if (S_OK == hr)
				{
					FILE* fh = fopen("c:\\SVObserver\\IOBoardLog.txt", "a");
					if (nullptr != fh)
					{
						time_t timeVal;
						time(&timeVal);
						struct tm *newTime;
						newTime = localtime(&timeVal);
						char strTmp[50];
						strcpy_s(strTmp, 50, asctime(newTime));
						size_t size = strlen(strTmp);
						if (size >= 25) // get rid of the newline from asctime.
							strTmp[24] = 0;

						fprintf(fh, "\nNew Log Entry %s\n", strTmp);
						for (int i = 0;i < Size; i++)
						{
							timeVal = TimeStamp[i];
							newTime = localtime(&timeVal);
							strcpy_s(strTmp, 50, asctime(newTime));
							size = strlen(strTmp);
							if (size >= 25) // get rid of the newline from asctime.
							{
								strTmp[24] = 0;
							}
							fprintf(fh, "%s >function %02x\n", strTmp, LogCode[i]);
						}
						fclose(fh);
					}
				}
			}
			else if (SVRabbitRTC == Index)
			{
				unsigned long lRTC = 0;
				hr = GetRTC(lRTC);
				if (S_OK == hr)
				{
					// put data into variant safe array.
					pValue->lVal = lRTC;
					pValue->vt = VT_I4;
				}
			}
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::SetParameterValue(unsigned long Index, VARIANT* pValue)
{
	HRESULT hr = S_FALSE;

	if (nullptr != pValue)
	{
		switch(Index)
		{
			case SVBoardType:
			{
				if (VT_I4 == pValue->vt)
				{
					long lBoardType = pValue->lVal;
					if (S_OK == TranslateBoardType(lBoardType))
					{
						hr = SetBoardType(lBoardType);
					}
					else
					{
						hr = SetBoardType(pValue->lVal);
					}
				}
				break;
			}
			case SVRabbitRTC:
			{
				if (VT_I4 == pValue->vt)
				{
					hr = SetRTC(pValue->lVal);
				}
				break;
			}
			case SVRabbitWriteLog:
			{
				hr = SetLogValue(pValue->lVal);
			}
			case SVModuleReady:
			{
				if(VT_BOOL == pValue->vt)
				{
					m_moduleReady = pValue->boolVal ? true : false;
				}
				break;
			}
			default:
				break;
		}
	}
	return hr;
}

short SVLptIOImpl::GetPreviousOutputs(long lControl)
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

void SVLptIOImpl::SetPreviousOutputs(long lControl, short sValue)
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
	unsigned long lCommand = 0;
	LPCSTR pString = "Unlock";
	
	long Retry = RETRY;
	bool bLocked = true;

	while (Retry > 0)
	{
		// Resets parallel board sub command sequence.
		lCommand = NO_CMD;
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

	return hr;
}

// WriteLockString 
// Sends a command 0x55 to the Parallel board
// to disable the outputs.
HRESULT SVLptIOImpl::WriteLockString()
{
	HRESULT hr = S_OK;

	unsigned long lCommand = 0;
	LPCSTR pString = "Lock";

	long Retry = RETRY;
	bool bLocked = false;

	while (Retry > 0)
	{
		// Resets parallel board sub command sequence.
		lCommand = NO_CMD;
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

HRESULT SVLptIOImpl::SVReadWriteLpt(unsigned long& rlValue, long prevControl, long lBit)
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
		double Start = SvTl::GetTimeStamp();

		// **** Wait for Acknowledge off...
		unsigned char status;
		hr = GetStatusPort(status);
		while (S_OK == hr && 0 == (status & 128))
		{
			double Check = SvTl::GetTimeStamp();

			if (SvTl::ConvertTo(SvTl::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
			{
				hr = GetStatusPort(status);
				if (0 == (status & 128))
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
							Start = SvTl::GetTimeStamp();
							hr = GetStatusPort(status);
							while (S_OK == hr && 0 != (status & 128))
							{
								double Check = SvTl::GetTimeStamp();
								if (SvTl::ConvertTo(SvTl::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
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
							hr = SetControlPort(static_cast<unsigned char>(nVal | lControl));
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
						Start = SvTl::GetTimeStamp();
						hr = GetStatusPort(status);
						while (S_OK == hr && 0 != (status & 128))
						{
							double Check = SvTl::GetTimeStamp();
							if (SvTl::ConvertTo(SvTl::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
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
						if (lBit < 0)
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

#ifdef LogDebugData
	g_TDebugData[g_CallbackCount].m_dStatusReg0 = StatusReg;
	g_TDebugData[g_CallbackCount].m_dStatusReg1 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp1 = SvTl::GetTimeStamp();

	LARGE_INTEGER Frequency;
	LARGE_INTEGER Timestamp;

	::QueryPerformanceFrequency(&Frequency);

	Timestamp.LowPart = LoTime;
	Timestamp.HighPart = HiTime;

	double Seconds = static_cast< double >(Timestamp.QuadPart) / static_cast< double >(Frequency.QuadPart);

	g_TDebugData[g_CallbackCount].m_lParTimeStamp = SvTl::ConvertFrom(SvTl::Seconds, Seconds);
#endif

	unsigned char StatusReg;
	HRESULT hr = GetStatusPort(StatusReg);
	if (S_OK == hr)
	{
		double timeStamp = SvTl::GetTimeStamp();

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
				SvTi::IntVariantMap triggerData;
				triggerData[SvTi::TriggerDataEnum::TimeStamp] = _variant_t(timeStamp);
				///Trigger channel 0 based
				triggerData[SvTi::TriggerDataEnum::TriggerChannel] = _variant_t(i - 1);

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
	g_TDebugData[g_CallbackCount].m_lTimeStamp2 = SvTl::GetTimeStamp();
	if (g_CallbackCount < MaxDebugData)
		g_CallbackCount++; //*/
#endif
}

