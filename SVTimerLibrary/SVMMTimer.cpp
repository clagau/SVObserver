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
#include <boost\thread\lock_guard.hpp>
#pragma endregion Includes

namespace SvTl
{
SVMMTimer::SVMMTimer()
	: m_timerID(0)
{
}

SVMMTimer::~SVMMTimer()
{
	_Stop();
}

SVMMTimer& SVMMTimer::Instance()
{
	static SVMMTimer timer;
	return timer;
}
void SVMMTimer::Start()
{
	SVMMTimer& timer = SVMMTimer::Instance();
	boost::lock_guard<boost::mutex> AutoLog(timer.m_Mutex);
	timer._Start();
}
void SVMMTimer::Stop()
{
	SVMMTimer& timer = SVMMTimer::Instance();
	boost::lock_guard<boost::mutex> AutoLog(timer.m_Mutex);
	timer._Stop();
}

// Add Event Handlers
void SVMMTimer::Subscribe(const std::string& receiverTag, unsigned long interval, SVTimerCallback* pCallback)
{
	SVMMTimer& timer = SVMMTimer::Instance();
	boost::lock_guard<boost::mutex> AutoLog(timer.m_Mutex);
	timer._Subscribe(receiverTag, interval, pCallback);
}

// Set Timer Interval
void SVMMTimer::SetInterval(const std::string& receiverTag, unsigned long interval)
{
	SVMMTimer& timer = SVMMTimer::Instance();
	boost::lock_guard<boost::mutex> AutoLog(timer.m_Mutex);
	timer._SetInterval(receiverTag, interval);
}
// Remove Event handlers
void SVMMTimer::UnSubscribe(const std::string& receiverTag)
{
	SVMMTimer& timer = SVMMTimer::Instance();
	boost::lock_guard<boost::mutex> AutoLog(timer.m_Mutex);
	timer._UnSubscribe(receiverTag);
}



void SVMMTimer::_Start()
{
	// if have subscribers
	if (m_eventListeners.size())
	{
		m_asyncProcedure.Create(&SVMMTimer::TimerAPCProc, boost::bind(&SVMMTimer::Dispatch, this, _1), _T("SVMMTimer"), SVNone);

		m_asyncProcedure.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);

		if (!m_timerID)
		{
			m_timerID = ::timeSetEvent(1, 0, SVMMTimer::TimerProc, (ULONG_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		}
	}
}


void SVMMTimer::_Stop()
{

	if (m_timerID)
	{
		::timeKillEvent(m_timerID);
		m_timerID = 0;
	}

	if (m_asyncProcedure.IsActive())
	{
		m_asyncProcedure.SetPriority(THREAD_PRIORITY_NORMAL);

		// Stop and Cleanup Async procedure
		m_asyncProcedure.Destroy();
	}
	m_eventListeners.clear();
}

void SVMMTimer::_Subscribe(const std::string& receiverTag, unsigned long interval, SVTimerCallback* pCallback)
{
	// Subscribe to event
	SVMMTimerEventHandler eventHandler(interval, pCallback);
	m_eventListeners[receiverTag] = eventHandler;
	// start it up if not started
	if (!m_asyncProcedure.IsActive())
	{
		_Start();
	}
}

void SVMMTimer::_SetInterval(const std::string& receiverTag, unsigned long interval)
{
	typedef SVTimerEventListeners::iterator Iter;
	// Find event handler
	Iter it = m_eventListeners.find(receiverTag);
	// set timer interval
	if (it != m_eventListeners.end())
	{
		it->second.setInterval(interval);
	}

}


void SVMMTimer::_UnSubscribe(const std::string& receiverTag)
{

	// UnSubscribe to event
	SVTimerEventListeners::iterator it = m_eventListeners.find(receiverTag);
	if (it != m_eventListeners.end())
	{
		m_eventListeners.erase(it);
	}
	// stop the timer if no Subscribers
	if (!m_eventListeners.size())
	{
		_Stop();
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
	boost::lock_guard<boost::mutex> AutoLog(m_Mutex);
	SVTimerEventListeners::iterator it;
	for (it = m_eventListeners.begin(); it != m_eventListeners.end(); ++it)
	{
		it->second.tick(it->first);
	}
}
} //namespace SvTl
