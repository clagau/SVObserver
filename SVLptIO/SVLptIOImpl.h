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
#pragma region Includes
#include "SVLptIO.h"
#include "TriggerInformation/IODeviceBase.h"
#pragma endregion Includes

constexpr unsigned long cMaxLptTriggers = 3;


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

	enum SVtriggerIndex
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
	SVLptIOImpl() = default;
	virtual ~SVLptIOImpl();
#pragma endregion Constructor

#pragma region Public Methods
public:
	// Trigger Variables...
	HRESULT Initialize(bool bInit);
	
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
	BSTR GetTriggerName(unsigned long triggerIndex);

	void beforeStartTrigger(unsigned long) override;
	HRESULT afterStartTrigger() override;
	HRESULT afterStopTrigger() override;

	HRESULT TriggerGetParameterCount(unsigned long triggerIndex, unsigned long* pCount);
	HRESULT TriggerGetParameterName(unsigned long triggerIndex, unsigned long Index, BSTR* pName);
	HRESULT TriggerGetParameterValue(unsigned long triggerIndex, unsigned long Index, VARIANT* pValue);
	HRESULT TriggerSetParameterValue(unsigned long triggerIndex, unsigned long Index, VARIANT* pValue);

	// Non-Trigger Parameter Functions
	HRESULT GetParameterCount(unsigned long* pCount);
	HRESULT GetParameterName(unsigned long Index, BSTR* pName);
	HRESULT GetParameterValue(unsigned long Index, VARIANT* pValue);
	HRESULT SetParameterValue(unsigned long Index, VARIANT* pValue);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

	short GetPreviousOutputs(long lControl);
	void SetPreviousOutputs(long lControl, short sValue);

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
	long m_lLastTriggerState{0L};
	long m_lLptTriggerEdge{0L};
	long m_lIOBrdTriggerEdge{0L};

	bool m_bUseSingleTrigger{ false };
	short m_nPreviousOutputs[SVNumOutputPorts + 1]{0, 0, 0, 0};

	long m_lBoardVersion{ 0L };
	ParallelBoardInterfaceType m_lParallelBoardInterfaceBehavior{ ParallelBoardInterfaceType::Function00ForWrite1 };

	bool m_isFirstTimeToReadOrWrite = true; //this variable introduced in SVO-1692 to suppress spurious "invalid line state" warning
#pragma endregion Member Variables
};

