//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDevice
//* .File Name       : $Workfile:   SVSoftwareTriggerDevice.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:29:50  $
//******************************************************************************

#ifndef SVSOFTWARETRIGGER_H
#define SVSOFTWARETRIGGER_H

#include <comdef.h>
#include <map>
#include <deque>
#include "SVTimerLibrary/SVTimerCallbackImpl.h"
#include "SVTriggerLibrary/SVTriggerCallbackStruct.h"
#include "SVContainerLibrary/SVBidirectionalMap.h"
#include "SVSystemLibrary\/SVCriticalSection.h"

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
struct SVSoftwareTriggerStruct
{
	SVTriggerCallbackStruct callbackStruct;
	bool bStarted;
};

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
typedef std::deque<SVSoftwareTriggerStruct> TriggerCallbackList;
typedef std::map<HANDLE, TriggerCallbackList> TriggerList;

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
class SVSoftwareTriggerDevice
{
	///////////////////////////////////////////////////////////////////////
	//
	///////////////////////////////////////////////////////////////////////
	struct SVSoftwareTimerStruct
	{
		long timerPeriod;
		SVTimerCallbackImpl<SVSoftwareTriggerDevice, void> timerCallback;
	};
	typedef std::map<HANDLE, SVSoftwareTimerStruct> TimerList;

	enum ParameterEnum
	{
		SVTriggerPeriod	= 0,
		SVBoardVersion	= 1
	};

protected:
	typedef SVBidirectionalMap<SVString, HANDLE>::type NameHandleList;
	NameHandleList m_nameHandleList;
	TimerList m_timerList;
	TriggerList m_triggerList;
	int m_numTriggers;

public:
	SVSoftwareTriggerDevice();
	virtual ~SVSoftwareTriggerDevice();

	HRESULT Initialize(bool bInit);

	unsigned long GetTriggerCount();
	HANDLE GetTriggerHandle(unsigned long index);
	BSTR GetTriggerName(HANDLE handle);
	HRESULT AddTriggerCallback(HANDLE handle, SVTriggerCallbackPtr pCallback, void* pOwner, void* pData);
	HRESULT RemoveTriggerCallback(HANDLE handle, SVTriggerCallbackPtr pCallback);
	HRESULT RemoveAllTriggerCallbacks(HANDLE handle);
	HRESULT StartTrigger(HANDLE handle);
	HRESULT StopTrigger(HANDLE handle);

	HRESULT TriggerGetParameterCount( HANDLE p_ulHandle, unsigned long *p_pulCount );
	HRESULT TriggerGetParameterName( HANDLE p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT TriggerGetParameterValue( HANDLE p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT TriggerSetParameterValue( HANDLE p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

private:
	HRESULT GetTriggerPeriod( HANDLE handle, long* p_lPeriod ) const;
	HRESULT SetTriggerPeriod( HANDLE handle, long p_lPeriod );

	HRESULT SetTimerCallback(HANDLE handle);
	HRESULT RemoveTimerCallback(HANDLE handle);
	void OnSoftwareTimer(const SVString& tag);
	static void DispatchTrigger(const SVSoftwareTriggerStruct& triggerListener);
	SVCriticalSection m_CritSec;
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSoftwareTriggerDevice\SVSoftwareTriggerDevice.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:29:50   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   23 Jun 2009 14:42:08   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Runtime trigger interval changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   02 Jun 2009 15:20:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised include for SVBidirectionalMap (moved to SVContainerLibrary)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jan 2009 14:20:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added the RemoveTimerCallback method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Oct 2008 17:26:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:25:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/

