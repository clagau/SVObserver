//******************************************************************************
//* COPYRIGHT (c) 2014 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVLptIOImpl
//******************************************************************************

#pragma once

#pragma warning (disable : 4786)

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVLptIO.h"
#include "TriggerInformation/IODeviceBase.h"


enum ParallelBoardInterfaceType
{
	LegacyFunctions = 0,
	Function00ForWrite1 = 1,
};


///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
class SVLptIOImpl : public SVLptIO, public SvTi::IODeviceBase
//@TODO [developer] it might be better to use aggregation instead of multiple inheritance here
{
#pragma region Declarations
	enum SVHWControlEnum
	{
//		NoMode Not used anywhere.
//		SVControlNoMode               = 0x001B, // 0000 0000 0001 1011
		SVControlCameraSetup          = 0x001A, // 0000 0000 0001 1010
		SVControlWriteDigital0        = 0x0019, // 0000 0000 0001 1001
		SVControlWriteDigital1        = 0x0018, // 0000 0000 0001 1000
		SVControlWriteDigital1_110    = 0x001B, // 0000 0000 0001 1011
		SVControlReadDigitalInputs    = 0x0013, // 0000 0000 0001 0011
		SVControlReadCommand          = 0x0012, // 0000 0000 0001 0010
		SVControlCommand              = 0x0011, // 0000 0000 0001 0001
		SVControlSelectMode           = 0x0014, // 0000 0000 0001 0100
		SVControlEnableInterrupt      = 0x0010, // 0000 0000 0001 0000
		SVControlBidirectional        = 0x0020, // 0000 0000 0010 0000
		SVControlUseBitLevel          = 0x1000, // 0001 0000 0000 0000
	};

	enum 
	{
		SVNumOutputPorts = 3,
	};

	enum SVTriggerChannel
	{
		SVTriggerNone = 0,
		SVTrigger1    = 0x08,	// S3- Error
		SVTrigger2    = 0x10,	// S4+ SelectIn
		SVTrigger3    = 0x20,	// S5+ PaperEnd
		SVTrigger4    = 0x80,	// S7- Busy
	};
#pragma endregion Declarations

#pragma region Constructor
public:
	SVLptIOImpl();
	virtual ~SVLptIOImpl();
#pragma endregion Constructor

#pragma region Public Methods
public:
	// Trigger Variables...
	long m_lLastTriggerState;
	long m_lLptTriggerEdge;
	long m_lIOBrdTriggerEdge;

	bool m_bUseSingleTrigger;
	short m_nPreviousOutputs[SVNumOutputPorts + 1];

	HRESULT Initialize(bool bInit);
	
	unsigned long GetNumPorts();
	unsigned long GetNumInputs();
	unsigned long GetNumOutputs();

	// Digital I/O
	unsigned long GetInputCount();
	unsigned long GetOutputCount();
	unsigned long GetPortCount();

	HRESULT GetInputValue(unsigned long* pVal);
	HRESULT SetOutputValue(unsigned long val);
	HRESULT GetInputBit(unsigned long bitNum, bool& bitVal);
	HRESULT SetOutputBit(unsigned long bitNum, bool bitVal);
	
	HRESULT SetPortOutputValue(unsigned portNo, unsigned long val);
	HRESULT GetBoardVersion(long& p_rlVer);

//	HRESULT GetPortInputBit(unsigned portNo, unsigned long bitNo, bool& bitVal);
//	HRESULT SetPortOutputBit(unsigned portNo, unsigned long bitNo, bool bitVal);

	HRESULT SetBoardType(unsigned long lBoardType);
	HRESULT GetBoardType(unsigned long& rlBoardType);
	HRESULT SetTriggerEdge(unsigned long lTriggerEdge);
	HRESULT GetFanState(unsigned long& rlFanState);
	HRESULT GetFanFreq(long* plFanFreq);
	HRESULT GetLog(unsigned long* pulTS, unsigned long* pucLog, long& lSize);
	HRESULT GetRTC(unsigned long& rlValue);
	HRESULT SetRTC(unsigned long lValue);
	HRESULT SetLogValue(unsigned long lValue);

	// Triggers
	unsigned long GetTriggerCount();
	unsigned long GetTriggerHandle(unsigned long index);
	BSTR GetTriggerName(unsigned long handle);

	void beforeStartTrigger(unsigned long) override;
	HRESULT afterStartTrigger(HRESULT hr) override;
	HRESULT afterStopTrigger(HRESULT hr) override;

	HRESULT TriggerGetParameterCount(unsigned long triggerchannel, unsigned long *pulCount);
	HRESULT TriggerGetParameterName(unsigned long triggerchannel, unsigned long ulIndex, BSTR *pbstrName);
	HRESULT TriggerGetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT *pvarValue);
	HRESULT TriggerSetParameterValue(unsigned long triggerchannel, unsigned long ulIndex, VARIANT *pvarValue);

	// Non-Trigger Parameter Functions
	HRESULT GetParameterCount(unsigned long *pulCount);
	HRESULT GetParameterName(unsigned long ulIndex, BSTR *p_pbstrName);
	HRESULT GetParameterValue(unsigned long ulIndex, VARIANT *pvarValue);
	HRESULT SetParameterValue(unsigned long ulIndex, VARIANT *pvarValue);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:
	#ifdef SV_LOG_STATUS_INFO
		SVStatusDeque m_StatusLog;
	#endif

	int m_numPorts;
	int m_numInputs;
	int m_numOutputs;
	int m_numTriggers;
	long m_lBoardVersion;
	ParallelBoardInterfaceType m_lParallelBoardInterfaceBehavior;

	short GetPreviousOutputs(long lControl);
	void SetPreviousOutputs(long lControl, short sValue);

	HRESULT Lock();
	HRESULT UnLock();
	HRESULT GetLockState(bool& bLocked);
	HRESULT WriteUnlockString();
	HRESULT WriteLockString();
	HRESULT TranslateBoardType(long& ulBoardType);
	HRESULT SVReadWriteLpt(unsigned long& rlValue, long lControl, long lBit = -1);

#pragma endregion Protected Methods

#pragma region Private Methods
private:
	LPCTSTR GetControlText(long lControl);

	// IRQ Interrupt handler
	void HandleIRQ();
#pragma endregion Private Methods

#pragma region Member Variables
private:
	CRITICAL_SECTION m_hCriticalSection;
	BOOL m_bCriticalSectionCreated;
	//! Trigger active is used as a work around because the thread handle in SVLpt.dll is not being closed in when calling DisableInterrupt
	//! The thread is started only once and uses this active flag in the callback function to control going online and offline
	//! It should be solved in the dll however changes could cause the driver signature to become invalid
	//! The thread is closed on destruction causing the one thread handle to leak
	bool m_TriggerActive;
	bool m_isFirstTimeToReadOrWrite = true; //this variable introduced in SVO-1692 to suppress spurious "invalid line state" warning
#pragma endregion Member Variables
};

