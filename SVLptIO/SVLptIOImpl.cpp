//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVIMPL
//* .File Name       : $Workfile:   SVLptIOImpl.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   17 Sep 2014 15:57:36  $
//******************************************************************************

//#define INITIALIZE_IO_SUBSYSTEM // for InitializeIOSubsystem Application

#include "stdafx.h"
#include <fstream>
#include <time.h>
#include <boost/bind.hpp>
#include <functional>
#include <algorithm>
#include <string>
#include "SVLptIOImpl.h"
#include "SVIOLibrary\SVIOParameterEnum.h"
#include "SVTimerLibrary\SVClock.h"

#ifndef INITIALIZE_IO_SUBSYSTEM
#include "SVStatusLibrary\MessageManager.h"
#include "SVMessage\SVMessage.h"
#endif

static const int RETRY = 2;

// ******* Timeout for I/O board Acknowledge microseconds.
static const int BOARD_SELECT_ACK_TIMEOUT  = 16000; // 16 millisecs

// ******* Debug Data **********
//#define LogDebugData 

#ifdef LogDebugData
static const int MaxDebugData = 500;

struct TriggerDebugData
{
	SVClock::SVTimeStamp m_lTimeStamp1;
	SVClock::SVTimeStamp m_lTimeStamp2;
	SVClock::SVTimeStamp m_lParTimeStamp;
	unsigned short m_dStatusReg0;
	unsigned short m_dStatusReg1;
	unsigned short m_dStatusReg2;
};

long g_lCriticalSecInits = 0;
TriggerDebugData g_TDebugData[MaxDebugData];
long g_CallbackCount = 0;
#endif

class SVLptTriggerFind : public std::binary_function<SVLptIOTriggerStruct, SVLptIOCallbackPtr, bool>
{
public:
	bool operator()(const SVLptIOTriggerStruct& triggerStruct, const SVLptIOCallbackPtr& pCallback) const
	{
		return (triggerStruct.pCallback == pCallback);
	}
};

SVLptIOImpl::SVLptIOImpl() 
: m_numPorts(2)
, m_numInputs(12)
, m_numOutputs(24)
, m_numTriggers(3)
, m_lBoardVersion(0)
{
	try
	{
		#if (_WIN32_WINNT >= 0x0500)
		  ::InitializeCriticalSectionAndSpinCount(&m_hCriticalSection, BOARD_SELECT_ACK_TIMEOUT + 100);
		#else
		  ::InitializeCriticalSection(&m_hCriticalSection);
		#endif
		m_bCriticalSectionCreated = TRUE;
	}
	catch (...)
	{
		m_bCriticalSectionCreated = FALSE;
	}
	m_bUseSingleTrigger = false;
	m_lParallelBoardInterfaceBehavior = Function00ForWrite1;

	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.Create();
	#endif
}

SVLptIOImpl::~SVLptIOImpl()
{
	DisableAckInterrupt();
	if (m_bCriticalSectionCreated)
	{
		try
		{
		  ::DeleteCriticalSection(&m_hCriticalSection);
		}
		catch (...)
		{
		}
	}
}

HRESULT SVLptIOImpl::Initialize(bool bInit)
{
	HRESULT hr = S_OK;

	if (bInit && !IsActive())
	{
		SetActive(1);
	
		if (!IsActive())
		{
			hr = S_FALSE;
		}
		else
		{
			bool bBiDirectional;

			hr = IsBidirectionalSupported(bBiDirectional);
			if (S_OK == hr && !bBiDirectional)
			{
				hr = S_FALSE;
			}
		}
		if (S_OK == hr)
		{
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
				SetRTC(lTime);	// 5 command 

		//		SetLogValue(0xF3);
				// Unlock I/O Board
				if (S_OK != WriteUnlockString())
				{
					WriteUnlockString();
				}

				// The Rabbit board version is read here.
				for (int i = 0;i < RETRY; i++)
				{
					if (S_OK == GetBoardVersion(m_lBoardVersion)) // 5 commands
						break;
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
		SVClock::SVTimeStamp LastTimeStamp = g_TDebugData[0].m_lTimeStamp1;
		fprintf(pfh, "Critical Section inits %d", g_lCriticalSecInits);
		for (int i = 0 ;i < g_CallbackCount; i++)
		{
			INT64 Tic = static_cast<INT64>(SVClock::ConvertTo(SVClock::Microseconds, g_TDebugData[i].m_lParTimeStamp));
			INT64 Start = static_cast<INT64>(SVClock::ConvertTo(SVClock::Microseconds, g_TDebugData[i].m_lTimeStamp1));
			INT64 Last = static_cast<INT64>(SVClock::ConvertTo(SVClock::Microseconds, (g_TDebugData[i].m_lTimeStamp1 - LastTimeStamp)));
			INT64 End = static_cast<INT64>(SVClock::ConvertTo(SVClock::Microseconds, g_TDebugData[i].m_lTimeStamp2));

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
				WriteLockString();
			}

			// Block the IRQ
			DisableAckInterrupt();

			// Unload the driver
			if (IsActive())
			{
				SetActive(0);  
			}
			// Stop all triggers
			m_triggerList.clear();
		}
	}
	return hr;
}

unsigned long SVLptIOImpl::GetNumPorts()
{
	return m_numPorts;
}

unsigned long SVLptIOImpl::GetNumInputs()
{
	return m_numInputs;
}

unsigned long SVLptIOImpl::GetNumOutputs()
{
	return m_numOutputs;
}

unsigned long SVLptIOImpl::GetInputCount()
{
	return m_numInputs;
}

unsigned long SVLptIOImpl::GetOutputCount()
{
	return m_numOutputs;
}

unsigned long SVLptIOImpl::GetPortCount()
{
	return m_numPorts;
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
		if (SVReadWriteLpt(data, SVControlWriteDigital0) != S_OK)
		{
			hr = S_FALSE;
		}
		data = (val >> 8) & 0xff;
		if (SVReadWriteLpt(data, SVControlWriteDigital1) != S_OK)
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
	// there are only 16 output bits.

//	data = (val >> 16) & 0xff;
//	if (SVReadWriteLpt(data, SVControlCameraSetup) != S_OK)
//	{
//		hr = S_FALSE;
//	}

	return hr;
}

HRESULT SVLptIOImpl::GetPortInputValue(unsigned portNo, unsigned long* pVal)
{
	return GetInputValue(pVal);
}

HRESULT SVLptIOImpl::SetPortOutputValue(unsigned portNo, unsigned long val)
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
	unsigned long lValue=0;//Arvid 150108 added a default value to avoid Cppcheck warning
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
	unsigned long lTmp = 0;

	unsigned long lCommand = NO_CMD;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = READ_FAN_FREQ;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0; i < 4; i++)
		{
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
	unsigned long lTmp = 0;

	unsigned long lCommand = NO_CMD;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = READ_LOG;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0;i < lSize && S_OK  == hr;i++)
		{
			// Logged time stamp.
			unsigned long lTimeStamp = 0;
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
	unsigned long lTmp = 0;

	unsigned long lCommand = NO_CMD;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = READ_RTC;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0;i < 4; i++)
		{
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
	unsigned long lTmp = 0;

	unsigned long lCommand = NO_CMD;
	hr = SVReadWriteLpt(lCommand, SVControlCommand);

	lCommand = WRITE_RTC;	// Get the Rabbit ready to receive a string of bytes for RTC set
	hr = SVReadWriteLpt(lCommand, SVControlCommand);
	if (S_OK == hr)
	{
		for (int i = 0;i < 4;i++)
		{
			lTmp = lValue >> (i * 8);
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
	unsigned long lTmp = 0;

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
	return m_numTriggers;
}

HANDLE SVLptIOImpl::GetTriggerHandle(unsigned long index)
{
	return (HANDLE)(index + 1);
}

BSTR SVLptIOImpl::GetTriggerName(HANDLE handle)
{
	char szTmp[128];
	szTmp[0] = 0;

	int iIndex = (int) handle - 1;
	if (iIndex >= 0 && iIndex < m_numTriggers)
	{
		strcpy(&szTmp[0], "IO_Board_1.Dig_");
		size_t iStrLen = strlen(&szTmp[0]);
		char* sz = _itoa(iIndex, &szTmp[iStrLen], 10);
		szTmp[iStrLen + 1] = 0;
	}
	BSTR name = _com_util::ConvertStringToBSTR(szTmp);
	return name;
}

HRESULT SVLptIOImpl::AddTriggerCallback(HANDLE handle, SVLptIOCallbackPtr pCallback, void* pOwner, void* pData)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;

		// check for dups
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVLptTriggerFind(), pCallback));
		
		if (callbackIt != list.end())
		{
			// DUPLICATE Entry!!!
		}
		else
		{
			// add it
			SVLptIOTriggerStruct triggerStruct;
			triggerStruct.bStarted = false;
			triggerStruct.pCallback = pCallback;
			triggerStruct.pOwner = pOwner;
			triggerStruct.pData = pData;

			list.push_back(triggerStruct);
			hr = S_OK;
		}
	}
	else
	{
		// add it
		TriggerCallbackList list;
		SVLptIOTriggerStruct triggerStruct;
		triggerStruct.bStarted = false;
		triggerStruct.pCallback = pCallback;
		triggerStruct.pOwner = pOwner;
		triggerStruct.pData = pData;

		list.push_back(triggerStruct);
		m_triggerList.insert(std::make_pair(handle, list));
			
		hr = S_OK;
	}
	return hr;
}

HRESULT SVLptIOImpl::RemoveTriggerCallback(HANDLE handle, SVLptIOCallbackPtr pCallback)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		// check if it is in the list
		TriggerCallbackList& list = it->second;
		
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVLptTriggerFind(), pCallback));
		if (callbackIt != list.end())
		{
			list.erase(callbackIt);
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::RemoveAllTriggerCallbacks(HANDLE handle)
{
	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		m_triggerList.erase(it);
	}
	return S_OK;
}

HRESULT SVLptIOImpl::StartTrigger(HANDLE handle)
{
	HRESULT hr = S_FALSE;

	#ifdef SV_LOG_STATUS_INFO
		m_StatusLog.clear();
	#endif

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = true;
		}
		hr = S_OK;
	}
	if (S_OK == hr)
	{
		// Initialize previous Trigger State
		unsigned char status;
		hr = GetStatusPort(status);
		if (S_OK == hr)
		{
			m_lLastTriggerState = status;
			hr = EnableAckInterrupt(boost::bind(&SVLptIOImpl::HandleIRQ, this));
		}
	}

	#ifdef SV_LOG_STATUS_INFO
		if(S_OK != hr)
		{
			SVString String;
			String.Format(_T("StartTrigger - Status = 0x%X"), hr);
			m_StatusLog.push_back(String);
		}
	#endif

	return hr;
}

HRESULT SVLptIOImpl::StopTrigger(HANDLE handle)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		hr = S_OK;
	}
	if (S_OK == hr)
	{
		bool bDisableIrq = true;
		for (it = m_triggerList.begin() ; it != m_triggerList.end() ; ++it)
		{
			TriggerCallbackList& list = it->second;

			if(0 == list.size() || list[0].bStarted)
			{
				bDisableIrq = false;
			}
		}
		if (bDisableIrq) // Make sure all triggers are stopped before Masking the IRQ
		{
			hr = DisableAckInterrupt();
		}
	}

	#ifdef SV_LOG_STATUS_INFO
		SVString FileName;

		FileName.Format(_T("C:\\SVObserver\\SVLpt.log"));
		std::fstream Stream(FileName.ToString(), std::ios_base::trunc | std::ios_base::out);
		if (Stream.is_open())
		{
			for (int i = 0;i < m_StatusLog.GetCount(); ++i)
			{
				SVString String;
				m_StatusLog.GetAt(i, &String);
				Stream << String.ToString() << std::endl;
			}
			Stream.close();
			m_StatusLog.clear();
		}
	#endif

	return hr;
}

HRESULT SVLptIOImpl::TriggerGetParameterCount(unsigned long ulHandle, unsigned long* pulCount)
{
	HRESULT hr = S_FALSE;

	if (NULL != pulCount)
	{
		if (0 < ulHandle)
		{
			*pulCount = 3;
			hr = S_OK;
		}
		else
		{
			*pulCount = 0;
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::TriggerGetParameterName(unsigned long ulHandle, unsigned long ulIndex, BSTR* pbstrName)
{
	HRESULT hr = S_FALSE;

	if (NULL != pbstrName)
	{
		if (NULL != *pbstrName)
		{
			::SysFreeString(*pbstrName);
			*pbstrName = NULL;
		}

		if (0 < ulHandle)
		{
			// SVSignalEdge and SVBoardVersion enums are used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			if (SVSignalEdge == ulIndex)
			{
				*pbstrName = ::SysAllocString(L"Edge");
			}
			else if (SVSingleTriggerEdge == ulIndex)
			{
				*pbstrName = ::SysAllocString(L"Single Trigger Edge");
			}
			else if (SVBoardVersion == ulIndex)
			{
				*pbstrName = ::SysAllocString(L"Board Version");
			}
			if (*pbstrName != NULL)
			{
				hr = S_OK;
			}
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::TriggerGetParameterValue(unsigned long ulHandle, unsigned long ulIndex, VARIANT* pvarValue)
{
	HRESULT hr = S_FALSE;

	if (NULL != pvarValue)
	{
		if (S_OK == ::VariantClear(pvarValue))
		{
			if (0 < ulHandle)
			{
				// SVSignalEdge enum is used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				if (SVSignalEdge == ulIndex)
				{
					long lMask;
					switch (ulHandle)
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

					pvarValue->vt = VT_I4;

					if ((m_lLptTriggerEdge & lMask) == lMask)
					{
						pvarValue->lVal = 1;
					}
					else
					{
						pvarValue->lVal = -1;
					}

					hr = S_OK;
				}
				// SVBoardVersion enum is used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				if (SVBoardVersion == ulIndex)
				{
					long lVer;
					WCHAR wbuf[256];
					pvarValue->vt = VT_BSTR;
					hr = GetBoardVersion(lVer);
					swprintf(wbuf, L"Dedicated I/O Processor board %d.%02d firmware %d.%02d",
						lVer >> 24 & 0xff,
						lVer >> 16 & 0xff,
						lVer >> 8 & 0xff,
						lVer & 0xff);
					pvarValue->bstrVal = ::SysAllocString(wbuf);
				}
			}
		}
	}
	return hr;
}

HRESULT SVLptIOImpl::TriggerSetParameterValue(unsigned long ulHandle, unsigned long ulIndex, VARIANT* pvarValue)
{
	HRESULT hr = S_FALSE;

	if (0 < ulHandle)
	{
		if (NULL != pvarValue)
		{
			// SVSignalEdge enum is used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			if (SVSignalEdge == ulIndex)
			{
				if (VT_I4 == pvarValue->vt)
				{
					long lMask = 0; //1 << (ulHandle - 1);

					switch (ulHandle)
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

					switch (pvarValue->lVal)
					{
						case 1:
						{
							m_lLptTriggerEdge |= lMask;
							m_lIOBrdTriggerEdge |= 1 << (ulHandle -1);

							hr = S_OK;
							break;
						}
						case -1:
						{
							m_lLptTriggerEdge &= ~lMask;
							m_lIOBrdTriggerEdge &= ~(1 << (ulHandle -1));

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
			if (SVSingleTriggerEdge == ulIndex) // Use Single Trigger Methodology
			{
				if (VT_I4 == pvarValue->vt)
				{
					if (0 == pvarValue->lVal)
					{
						m_bUseSingleTrigger = false;
					}
					else
					{
						m_bUseSingleTrigger = true;
					}
				}
				if (VT_BOOL == pvarValue->vt)
				{
					m_bUseSingleTrigger = pvarValue->bVal == TRUE;
				}
			}
		}
	}
	return hr;
}

// Non-Trigger Parameter Functions

// GetParameterCount
// This function returns the number of available parameters.
HRESULT SVLptIOImpl::GetParameterCount(unsigned long* pulCount)
{
	HRESULT hr = S_FALSE;

	if (NULL != pulCount)
	{
		*pulCount = 3;
		hr = S_OK;
	}
	return hr;
}

// GetParameterName
// This function creates a BSTR with the name of the parameter
// It is the responsibility of the caller to free the BSTR.
HRESULT SVLptIOImpl::GetParameterName(unsigned long ulIndex, BSTR* pbstrName)
{
	HRESULT hr = S_FALSE;

	if (NULL != pbstrName)
	{
		if (NULL != *pbstrName)
		{
			::SysFreeString(*pbstrName);
			*pbstrName = NULL;
		}

		if (SVBoardVersion == ulIndex)
		{
			*pbstrName = ::SysAllocString(L"Board Version(R)");
		}
		else if (SVFanState == ulIndex)
		{
			*pbstrName = ::SysAllocString(L"Fan State(R)");
		}
		else if (SVBoardType == ulIndex)
		{
			*pbstrName = ::SysAllocString(L"Board Type(R/W)");
		}
		else if (SVFanFreq == ulIndex)
		{
			*pbstrName = ::SysAllocString(L"Fan Frequency");
		}
			
		if (NULL != *pbstrName)
		{
			hr = S_OK;
		}
	}
	return hr;
}

// GetParameterValue
// This function Gets the parameter value specified by ulIndex.
HRESULT SVLptIOImpl::GetParameterValue(unsigned long ulIndex, VARIANT* pvarValue)
{
	HRESULT hr = S_FALSE;

	if (NULL !=  pvarValue)
	{
		if (S_OK == ::VariantClear(pvarValue))
		{
			// Board Version
			if (SVBoardVersion == ulIndex)
			{
				long lVer;
				WCHAR wbuf[256];
				pvarValue->vt = VT_BSTR;
				hr = GetBoardVersion(lVer);
				swprintf(wbuf, L"Dedicated I/O Processor board %d.%02d firmware %d.%02d",
					lVer >> 24 & 0xff,
					lVer >> 16 & 0xff,
					lVer >> 8 & 0xff,
					lVer & 0xff);
				pvarValue->bstrVal = ::SysAllocString(wbuf);
			}
			else if (SVBoardType == ulIndex)			// Board Type : X-Series, D-Series, or A-Series Boards.
			{
				unsigned long lType;
				hr = GetBoardType(lType);
				pvarValue->vt = VT_I4;
				if (S_OK == hr)
				{
					pvarValue->lVal = lType;
				}
			}
			else if (SVFanState == ulIndex)			// Fan State
			{
				unsigned long lFanState;
				pvarValue->vt = VT_I4;
				hr = GetFanState(lFanState);
				pvarValue->lVal = lFanState;
			}
			else if (SVFanFreq == ulIndex)
			{
				long lTmp[4];
				hr = GetFanFreq(lTmp);
				if (S_OK == hr)
				{	// The results are only 8 bits each so stuff all 4 into a 32bit long.
					// This is just for testing.
					// If this is something we want to support then it should be wrapped up in a safearray of longs.
					pvarValue->vt = VT_I4;
					pvarValue->lVal = (lTmp[0] & 0xff) |
						((lTmp[1] & 0xff) << 8) |
						((lTmp[2] & 0xff) << 16) | 
						((lTmp[3] & 0xff) << 24);  
				}
			}
			else if (SVLogDump == ulIndex)
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
					if (fh != NULL)
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
			else if (SVRabbitRTC == ulIndex)
			{
				unsigned long lRTC = 0;
				hr = GetRTC(lRTC);
				if (S_OK == hr)
				{
					// put data into variant safe array.
					pvarValue->lVal = lRTC;
					pvarValue->vt = VT_I4;
				}
			}
		}
	}
	return hr;
}

// SetParameterValue
// This function sets the value specified by ulIndex.
// if the index is not supported for setting, then an error is returned.
HRESULT SVLptIOImpl::SetParameterValue(unsigned long ulIndex, VARIANT* pvarValue)
{
	HRESULT hr = S_FALSE;

	if (NULL != pvarValue)
	{
		// Set Board Type
		if (SVBoardType == ulIndex) 
		{
			if (VT_I4 == pvarValue->vt)
			{
				long lBoardType = pvarValue->lVal;
				if (S_OK == TranslateBoardType(lBoardType))
				{
					hr = SetBoardType(lBoardType);
				}
				else
				{
					hr = SetBoardType(pvarValue->lVal);
				}
			}
		}
		else if (SVRabbitRTC == ulIndex)
		{
			long lRTC = pvarValue->lVal;
			if (VT_I4 == pvarValue->vt)
			{
				hr = SetRTC(lRTC);
			}
		}
		else if (SVRabbitWriteLog == ulIndex)
		{
			long lValue = pvarValue->lVal;
			hr = SetLogValue(lValue);
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

HRESULT SVLptIOImpl::Lock()
{
	HRESULT hr = S_OK;
	try
	{
	  ::EnterCriticalSection(&m_hCriticalSection);
	}
	catch (...)
	{
		hr = S_FALSE;
//		::MessageBox(NULL, "SVReadWrite Locked", NULL, MB_OK);
	}
	return hr;
}

HRESULT SVLptIOImpl::UnLock()
{
	HRESULT hr = S_OK;
	try
	{
		::LeaveCriticalSection(&m_hCriticalSection);
	}
	catch (...)
	{
		hr = S_FALSE;
	}
	return hr;
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
#ifndef INITIALIZE_IO_SUBSYSTEM
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
			if (false == bLocked)
			{
				break;
			}
		}
		Retry--;
	}
#endif
	return hr;
}

// WriteLockString 
// Sends a command 0x55 to the Parallel board
// to enable the outputs.
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
			if (true == bLocked)
			{
				break;
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

	if (S_OK == Lock())
	{
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
#ifndef INITIALIZE_IO_SUBSYSTEM
				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_INVALID_LINE_STATE, _T("Select Line In wrong State"), StdMessageParams );
#endif
			}
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
			SVClock::SVTimeStamp Start = SVClock::GetTimeStamp();

			// **** Wait for Acknowledge off...
			unsigned char status;
			hr = GetStatusPort(status);
			while (S_OK == hr && 0 == (status & 128))
			{
				SVClock::SVTimeStamp Check = SVClock::GetTimeStamp();

				if (SVClock::ConvertTo(SVClock::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
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
						unsigned char nNewOutput;
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
						hr = SetDataPort(nNewOutput);					// Write Data to Data Port
						if (S_OK == hr)
						{
							SetPreviousOutputs(prevControl, nNewOutput); //
							hr = SetControlPort(static_cast<unsigned char>(nVal | lControl | SVControlSelectMode));
							if (S_OK == hr)
							{
								// **** Wait for Acknowledge...
								Start = SVClock::GetTimeStamp();
								unsigned char status;
								hr = GetStatusPort(status);
								while (S_OK == hr && 0 != (status & 128))
								{
									SVClock::SVTimeStamp Check = SVClock::GetTimeStamp();
									if (SVClock::ConvertTo(SVClock::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
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
							Start = SVClock::GetTimeStamp();
							unsigned char status;
							hr = GetStatusPort(status);
							while (S_OK == hr && 0 != (status & 128))
							{
								SVClock::SVTimeStamp Check = SVClock::GetTimeStamp();
								if (SVClock::ConvertTo(SVClock::Microseconds, (Check - Start)) > BOARD_SELECT_ACK_TIMEOUT)
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
		UnLock();
	}
	else
	{
		hr = S_FALSE;
	}
	return hr;
}

LPCTSTR SVLptIOImpl::GetControlText(long lControl)
{
	LPCTSTR pString = NULL;
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
	::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

#ifdef LogDebugData
	g_TDebugData[g_CallbackCount].m_dStatusReg0 = StatusReg;
	g_TDebugData[g_CallbackCount].m_dStatusReg1 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp1 = SVClock::GetTimeStamp();

	LARGE_INTEGER Frequency;
	LARGE_INTEGER Timestamp;

	::QueryPerformanceFrequency(&Frequency);

	Timestamp.LowPart = LoTime;
	Timestamp.HighPart = HiTime;

	double Seconds = static_cast< double >(Timestamp.QuadPart) / static_cast< double >(Frequency.QuadPart);

	g_TDebugData[g_CallbackCount].m_lParTimeStamp = SVClock::ConvertFrom(SVClock::Seconds, Seconds);
#endif

	unsigned char StatusReg;
	HRESULT hr = GetStatusPort(StatusReg);
	if (S_OK == hr)
	{
		#ifdef SV_LOG_STATUS_INFO
			SVString String;

			String.Format(_T("FireLptInterrupt - StatusReg = 0x%X"), StatusReg);

			m_StatusLog.push_back(String);
		#endif

		// call trigger callbacks
		TriggerList::iterator it;

		for (it = m_triggerList.begin();it != m_triggerList.end() ;it++)
		{
			long lTrigger = reinterpret_cast<long>(it->first); // Trigger = the 1 based handle.

			short nTriggerBit = SVTriggerNone;

			// Get the trigger bit 
			switch (lTrigger)
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
				#ifdef SV_LOG_STATUS_INFO
					String.Format(_T("FireLptInterrupt - TriggerBit = 0x%X - TriggerEdge = 0x%X"), nTriggerBit, m_lLptTriggerEdge);

					m_StatusLog.push_back(String);
				#endif

				TriggerCallbackList& list = it->second;

				for (size_t i = 0;i < list.size();i++)
				{
					if (list[i].bStarted)
					{
						(list[i].pCallback)(list[i].pOwner, list[i].pData);
					}
				}
			}
		}
		m_lLastTriggerState = StatusReg;
	}
/* SEJ - don't think we should be doing this...
	unsigned short TmpStatusReg = GetStatusPort();
	if (TmpStatusReg != StatusReg)
	{
		OnLptInterrupt(irqNo);
	}
*/

#ifdef LogDebugData
	// **** Debug logging ****
	g_TDebugData[g_CallbackCount].m_dStatusReg2 = GetStatusPort();
	g_TDebugData[g_CallbackCount].m_lTimeStamp2 = SVClock::GetTimeStamp();
	if (g_CallbackCount < MaxDebugData)
		g_CallbackCount++; //*/
#endif
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVLptIO\SVLptIOImpl.cpp_v  $
 * 
 *    Rev 1.0   17 Sep 2014 15:57:36   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  943
 * SCR Title:  Parallel Port I/O for 64 Bit and Windows 7
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
