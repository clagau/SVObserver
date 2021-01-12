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
#include "Triggering/IODeviceBase.h"
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
	unsigned long GetInputCount() const;
	unsigned long GetOutputCount() const;
	unsigned long GetPortCount() const;

	unsigned long GetInputValue() const;
	HRESULT SetOutputValue(unsigned long val);
	bool GetInputBit(unsigned long bitNum) const;
	HRESULT SetOutputBit(unsigned long bitNum, bool bitVal);
	
	HRESULT SetPortOutputValue(unsigned portNo, unsigned long val);
	long GetBoardVersion() const;

	HRESULT SetBoardType(unsigned long lBoardType);
	HRESULT GetBoardType(unsigned long& rlBoardType) const;
	HRESULT SetTriggerEdge(unsigned long lTriggerEdge);
	HRESULT GetFanState(unsigned long& rlFanState) const;
	HRESULT GetFanFreq(long* plFanFreq) const;
	HRESULT GetLog(unsigned long* pulTS, unsigned long* pucLog, long& lSize) const;
	HRESULT GetRTC(unsigned long& rlValue) const;
	HRESULT SetRTC(unsigned long lValue);
	HRESULT SetLogValue(unsigned long lValue);

	// Triggers
	unsigned long GetTriggerCount() const;
	unsigned long GetTriggerHandle(unsigned long index) const;
	_variant_t GetTriggerName(unsigned long triggerIndex) const;

	void beforeStartTrigger(unsigned long) override;
	HRESULT afterStartTrigger() override;
	HRESULT afterStopTrigger() override;

	unsigned long TriggerGetParameterCount(unsigned long triggerIndex) const;
	_variant_t TriggerGetParameterName(unsigned long triggerIndex, unsigned long index) const;
	_variant_t TriggerGetParameterValue(unsigned long triggerIndex, unsigned long index) const;
	HRESULT TriggerSetParameterValue(unsigned long triggerIndex, unsigned long index, const _variant_t& rValue);

	// Non-Trigger Parameter Functions
	unsigned long GetParameterCount() const;
	_variant_t GetParameterName(unsigned long index) const;
	_variant_t GetParameterValue(unsigned long index) const;
	HRESULT SetParameterValue(unsigned long index, const _variant_t& rValue);
#pragma endregion Public Methods

#pragma region Protected Methods
protected:

	short GetPreviousOutputs(long lControl) const;
	void SetPreviousOutputs(long lControl, short sValue) const;

	HRESULT GetLockState(bool& bLocked);
	HRESULT WriteUnlockString();
	HRESULT WriteLockString();
	HRESULT TranslateBoardType(long& ulBoardType);
	HRESULT SVReadWriteLpt(unsigned long& rlValue, long lControl, long lBit = -1) const;

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
	mutable short m_nPreviousOutputs[SVNumOutputPorts + 1]{0, 0, 0, 0};

	long m_lBoardVersion{ 0L };
	ParallelBoardInterfaceType m_lParallelBoardInterfaceBehavior{ ParallelBoardInterfaceType::Function00ForWrite1 };

	mutable bool m_isFirstTimeToReadOrWrite = true; //this variable introduced in SVO-1692 to suppress spurious "invalid line state" warning
#pragma endregion Member Variables
};

