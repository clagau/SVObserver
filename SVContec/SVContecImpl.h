// ******************************************************************************
// * COPYRIGHT (c) 2013 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVContecImpl
// * .File Name       : $Workfile:   SVContecImpl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.2  $
// * .Check In Date   : $Date:   16 Jan 2014 10:20:04  $
// ******************************************************************************

#ifndef SVCONTECIMPL_H
#define SVCONTECIMPL_H

#pragma warning (disable : 4786)

#include <comdef.h>
#include <map>
#include <vector>
#include "CDio.h"
#include "SVContecCallback.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVUtilityLibrary/SVString.h"

#pragma comment( lib, "CDIO.lib" )
						
struct SVContecTriggerStruct
{
	SVContecCallbackPtr pCallback;
	void* pOwner;
	void* pData;
	bool bStarted;
};

enum ParallelBoardInterfaceType
{
	LegacyFunctions = 0,
	Function00ForWrite1 = 1,
};
typedef std::vector< SVContecTriggerStruct > TriggerCallbackList;
typedef std::map<HANDLE, TriggerCallbackList> TriggerList;

class SVContecImpl //: public TVicLPT
{
public:
	enum SVLptControlEnum
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

	// Trigger Variables...
	long m_lLastTriggerState;
	long m_lLptTriggerEdge;
	long m_lIOBrdTriggerEdge;

private:
	CRITICAL_SECTION m_hCriticalSection;
	BOOL m_bCriticalSectionCreated;
	LPCTSTR GetControlText( long p_lControl );
	bool m_ControlPortOutbound;
	bool m_DataPortOutbound;
	bool m_Active;
	bool m_Acquired;

protected:
	TriggerList m_triggerList;

	int m_numPorts;
	int m_numInputs;
	int m_numOutputs;
	int m_numTriggers;
	long m_lBoardVersion;
	ParallelBoardInterfaceType m_lParallelBoardInterfaceBehavior;
	short m_DeviceId;

	short GetPreviousOutputs( long p_lControl );
	void SetPreviousOutputs( long p_lControl, short p_sValue );
	HRESULT SVReadWriteLpt( unsigned long & p_plValue, long p_lControl, long p_lBit = -1);
	HRESULT Lock();
	HRESULT UnLock();
	HRESULT WriteUnlockString( void );
	HRESULT WriteLockString( void );
	HRESULT GetLockState( bool& p_bLocked );
	HRESULT TranslateBoardType( long& p_ulBoardType );

	#ifdef SV_LOG_STATUS_INFO
		SVStatusDeque m_StatusLog;
	#endif

public:
	bool m_bUseSingleTrigger;
	short m_nPreviousOutputs[ SVNumOutputPorts + 1 ];

	SVContecImpl();
	virtual ~SVContecImpl();

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
	
	HRESULT GetPortInputValue(unsigned portNo, unsigned long* pVal);
	HRESULT SetPortOutputValue(unsigned portNo, unsigned long val);
	HRESULT GetBoardVersion( long& p_rlVer );

	HRESULT GetPortInputBit(unsigned portNo, unsigned long bitNo, bool& bitVal);
	HRESULT SetPortOutputBit(unsigned portNo, unsigned long bitNo, bool bitVal);

	HRESULT SetBoardType( unsigned long p_lBoardType );
	HRESULT GetBoardType( unsigned long& p_lBoardType );
	HRESULT SetTriggerEdge( unsigned long p_lTriggerEdge );
	HRESULT GetFanState( unsigned long& p_lFanState );
	HRESULT GetFanFreq( long* p_plFanFreq );
	HRESULT GetLog( unsigned long* p_pulTS, unsigned long* p_pucLog, long& p_lSize );
	HRESULT GetRTC( unsigned long& p_lValue );
	HRESULT SetRTC( unsigned long p_lValue );
	HRESULT SetLogValue( unsigned long p_lValue );

	// Triggers
	unsigned long GetTriggerCount();
	HANDLE GetTriggerHandle(unsigned long index);
	BSTR GetTriggerName(HANDLE handle);
	HRESULT AddTriggerCallback( HANDLE handle, SVContecCallbackPtr pCallback, void* pOwner, void* pData );
	HRESULT RemoveTriggerCallback( HANDLE handle, SVContecCallbackPtr pCallback );
	HRESULT RemoveAllTriggerCallbacks(HANDLE handle);
	HRESULT StartTrigger(HANDLE handle);
	HRESULT StopTrigger(HANDLE handle);

	HRESULT TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
	HRESULT TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );


	// Non-Trigger Parameter Functions
	HRESULT GetParameterCount( unsigned long *p_pulCount );
	HRESULT GetParameterName( unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT GetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT SetParameterValue( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	
	// IRQ Interrupt handler
	void FireLptInterrupt(unsigned short DataReg, unsigned short StatusReg, unsigned long LoTime, unsigned long HiTime);
	void NewFireLptInterrupt( short Id, WPARAM wParam, LPARAM lParam, void *Param );
	static void WINAPI CallBackProc( short Id, WPARAM wParam, LPARAM lParam, void *Param );

	// Methods to emulate TVicLPT
	HRESULT MaskIrq();
	HRESULT UnmaskIrq();
	BYTE GetControlPort();
	HRESULT SetControlPort( BYTE p_Value );
	BYTE GetDataPort();
	HRESULT SetDataPort( BYTE p_Value );
	BYTE GetStatusPort();
	HRESULT SetStatusPort( BYTE p_Value );
	bool GetActive();
	HRESULT SetActive( bool p_Active );
	bool GetAcquired();
	HRESULT SetAcquired( bool p_Acquired );
	HRESULT SetControlPortInputMode();
	HRESULT SetControlPortOutputMode();
	HRESULT SetDataPortInputMode();
	HRESULT SetDataPortOutputMode();
	HRESULT SetIOPortsModes();
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVContec\SVContecImpl.h_v  $
 * 
 *    Rev 1.2   16 Jan 2014 10:20:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Multi Platform Path Changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   06 Nov 2013 08:07:50   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added different library path for 32 bit compile.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   30 Oct 2013 11:31:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in of Contec files.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/