//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMMTimer
//* .File Name       : $Workfile:   SVMMTimer.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   02 Oct 2013 10:18:18  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "SVMMTimerEventHandler.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVSystemLibrary/SVCriticalSection.h"

class SVMMTimer
{
	typedef void (CALLBACK *APCSignalHandler)( ULONG_PTR );
	typedef boost::function<void ( bool& )> ThreadSignalHandler;

public:
	static SVMMTimer& Instance();
	
	static void Start();
	static void Stop();

	static void Subscribe(const SVString& receiverTag, unsigned long interval, SVTimerCallback* pCallback);
	static void SetInterval(const SVString& receiverTag, unsigned long interval);
	static void UnSubscribe(const SVString& receiverTag);

private:
	SVAsyncProcedure<APCSignalHandler, ThreadSignalHandler> m_asyncProcedure;
	SVTimerEventListeners m_eventListeners;
	unsigned int m_timerID;

	SVMMTimer();
	~SVMMTimer();

	static void CALLBACK TimerProc( UINT uTimerID, UINT uRsvd, DWORD_PTR dwUser, DWORD_PTR dwRsvd1, DWORD_PTR dwRsvd2 );
	static void CALLBACK TimerAPCProc( ULONG_PTR dwParam );

	void Dispatch( bool& p_WaitForEvents );
	SVCriticalSection m_CritSec;
};

