//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMMTimer
//* .File Name       : $Workfile:   SVMMTimer.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Dec 2014 14:12:44  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
//Moved to precompiled header: #include <mmsystem.h>
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVMMTimer.h"
#pragma endregion Includes

namespace SvTl
{
SVMMTimer::SVMMTimer()
	: m_timerID(0)
{
}

SVMMTimer::~SVMMTimer()
{
	Stop();
}

SVMMTimer& SVMMTimer::Instance()
{
	static SVMMTimer timer;
	return timer;
}

void SVMMTimer::Start()
{
	SVMMTimer& timer = SVMMTimer::Instance();

	// if have subscribers
	if (timer.m_eventListeners.size())
	{
		timer.m_asyncProcedure.Create(&SVMMTimer::TimerAPCProc, boost::bind(&SVMMTimer::Dispatch, &timer, _1), _T("SVMMTimer"), SVNone);

		timer.m_asyncProcedure.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);

		if (!timer.m_timerID)
		{
			timer.m_timerID = ::timeSetEvent(1, 0, SVMMTimer::TimerProc, (ULONG_PTR)&timer, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		}
	}
}

void SVMMTimer::Stop()
{
	SVMMTimer& timer = SVMMTimer::Instance();
	if (timer.m_timerID)
	{
		::timeKillEvent(timer.m_timerID);
		timer.m_timerID = 0;
	}

	if (timer.m_asyncProcedure.IsActive())
	{
		timer.m_asyncProcedure.SetPriority(THREAD_PRIORITY_NORMAL);

		// Stop and Cleanup Async procedure
		timer.m_asyncProcedure.Destroy();
	}
	
		std::lock_guard<std::recursive_mutex> AutoLog(timer.m_Mutex);
		timer.m_eventListeners.clear();
	
}

// Add Event Handlers
void SVMMTimer::Subscribe(const std::string& receiverTag, unsigned long interval, SVTimerCallback* pCallback)
{
	SVMMTimer& timer = SVMMTimer::Instance();

	// Subscribe to event
	SVMMTimerEventHandler eventHandler(interval, pCallback);
	
		std::lock_guard<std::recursive_mutex> AutoLog(timer.m_Mutex);
		
		timer.m_eventListeners[receiverTag] = eventHandler;
		// start it up if not started
		if (!timer.m_asyncProcedure.IsActive())
		{
			timer.Start();
		}
		
	
}

// Set Timer Interval
void SVMMTimer::SetInterval(const std::string& receiverTag, unsigned long interval)
{
	SVMMTimer& timer = SVMMTimer::Instance();
	typedef SVTimerEventListeners::iterator Iter;
	
	std::lock_guard<std::recursive_mutex> AutoLog(timer.m_Mutex);


	// Find event handler
	Iter it = timer.m_eventListeners.find(receiverTag);

	// set timer interval
	if (it != timer.m_eventListeners.end())
	{
		it->second.setInterval(interval);
	}
	
}

// Remove Event handlers
void SVMMTimer::UnSubscribe(const std::string& receiverTag)
{
	SVMMTimer& timer = SVMMTimer::Instance();

	std::lock_guard<std::recursive_mutex> AutoLog(timer.m_Mutex);
	// UnSubscribe to event
	SVTimerEventListeners::iterator it = timer.m_eventListeners.find(receiverTag);
	if (it != timer.m_eventListeners.end())
	{
		timer.m_eventListeners.erase(it);
	}
	// stop the timer if no Subscribers
	if (!timer.m_eventListeners.size())
	{
		timer.Stop();
	}
	
}

// Callback for the TimeSetEvent API call
void CALLBACK SVMMTimer::TimerProc(UINT uTimerID, UINT uRsvd, DWORD_PTR dwUser, DWORD_PTR dwRsvd1, DWORD_PTR dwRsvd2)
{
	SVMMTimer& timer = SVMMTimer::Instance();

	// Wake up Dispatch Thread
	timer.m_asyncProcedure.Signal(nullptr);
}

// Callback for the QueueUserAPC API call
// Do nothing (just wake up thread)
void CALLBACK SVMMTimer::TimerAPCProc(ULONG_PTR dwParam)
{
}

// Event Dispatcher
void SVMMTimer::Dispatch(bool& p_WaitForEvents)
{
	
	std::lock_guard<std::recursive_mutex> AutoLog(m_Mutex);
	SVTimerEventListeners::iterator it;
	for (it = m_eventListeners.begin(); it != m_eventListeners.end(); ++it)
	{
		it->second.tick(it->first);
		// check elapsed time
		//it->second.tickCount++;
		//
		//if (it->second.tickCount >= it->second.interval)
		//{
		//	it->second.tickCount = 0;
		//	if (it->second.getCallback())
		//	{
		//		it->second.getCallback()->Notify(it->first);
		//	}
		//}
	}
	
}
} //namespace SvTl
