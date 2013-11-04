// ******************************************************************************
// * COPYRIGHT (c) 2005 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVTVicLptImpl
// * .File Name       : $Workfile:   SVTVicLptImpl.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   25 Apr 2013 19:09:30  $
// ******************************************************************************

#ifndef SVTVICLPTIMPL_H
#define SVTVICLPTIMPL_H

#pragma warning (disable : 4786)

#include <comdef.h>
#include <map>
#include <vector>
#include "VicLpt.h"
#include "SVTVicLptCallback.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVUtilityLibrary/SVString.h"

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
struct SVTVicLptTriggerStruct
{
	SVTVicLptCallbackPtr pCallback;
	void* pOwner;
	void* pData;
	bool bStarted;
};

enum ParallelBoardInterfaceType
{
	LegacyFunctions = 0,
	Function00ForWrite1 = 1,
};
typedef std::vector<SVTVicLptTriggerStruct> TriggerCallbackList;
typedef std::map<HANDLE, TriggerCallbackList> TriggerList;

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
class SVTVicLptImpl : public TVicLPT
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

protected:
	TriggerList m_triggerList;

	int m_numPorts;
	int m_numInputs;
	int m_numOutputs;
	int m_numTriggers;
	long m_lBoardVersion;
	ParallelBoardInterfaceType m_lParallelBoardInterfaceBehavior;

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

	SVTVicLptImpl();
	virtual ~SVTVicLptImpl();

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
	HRESULT AddTriggerCallback(HANDLE handle, SVTVicLptCallbackPtr pCallback, void* pOwner, void* pData);
	HRESULT RemoveTriggerCallback(HANDLE handle, SVTVicLptCallbackPtr pCallback);
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
};

#endif

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTVicLpt\SVTVicLptImpl.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 19:09:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   28 May 2010 14:25:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  694
 * SCR Title:  Upgrade SVObserver version for Chromis release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to fix issues with includes and make them consistant.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Jan 2010 15:51:32   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.  Updated logical flow to handle better startup functionality and initialization.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   15 Dec 2009 13:42:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   08 Oct 2009 11:59:56   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  666
 * SCR Title:  Change command format for  X-Series IO
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Modified ReadWriteLpt to log when select line was in wrong state.  Added new functions to support logging in Rabbit. Also change write bank1 to use different function if the rabbit version is 110 or above.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   22 May 2009 14:30:22   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  663
 * SCR Title:  Add support to SVTVicLpt dll to lock and unlock outputs.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added Lock Rabbit IO before the dll closes.
 * Added ability to get lock state so the dll can retry if the lock / unlock was unsuccessful.
 * Added ability to read the fan frequency for testing.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   27 Jun 2006 09:58:10   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  564
 * SCR Title:  Add configurability to SVTVicLpt DLL to set the system type
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Moved functions from exports to Impl class.
 * Added Set and Get Parameter functions that are not associated with triggers.
 * Added conditional logic that is dependant on the firmware that is running on the parallel board for triggers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   18 Nov 2005 07:22:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  513
 * SCR Title:  Add Parallel Port I/O to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added GetBoardVersion to SVTVicLPTImpl
 * Added logic to unlock outputs if the board was reset or lost power after trying to send outputs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   03 Nov 2005 13:23:14   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  513
 * SCR Title:  Add Parallel Port I/O to SVObserver
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Initial Check-in
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
