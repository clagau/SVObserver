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

