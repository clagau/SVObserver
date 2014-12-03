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

#include "stdafx.h"
#include <mmsystem.h>
#include <boost/bind.hpp>
#include "SVMMTimer.h"

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
		timer.m_asyncProcedure.Create(&SVMMTimer::TimerAPCProc, boost::bind(&SVMMTimer::Dispatch, &timer, _1), _T("SVMMTimer"), SVNone );

		timer.m_asyncProcedure.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);

		if (!timer.m_timerID)
		{
			timer.m_timerID = ::timeSetEvent( 1, 0, SVMMTimer::TimerProc, ( ULONG_PTR )&timer, TIME_PERIODIC | TIME_CALLBACK_FUNCTION );
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
	// Remove all Subscribed Event handlers
	timer.m_eventListeners.clear();
}

// Add Event Handlers
void SVMMTimer::Subscribe(const SVString& receiverTag, unsigned long interval, SVTimerCallback* pCallback)
{
	SVMMTimer& timer = SVMMTimer::Instance();
	
	// Subscribe to event
	SVMMTimerEventHandler eventHandler(interval, pCallback);
	timer.m_eventListeners[receiverTag] = eventHandler;

	// start it up if not started
	if (!timer.m_asyncProcedure.IsActive())
	{
		timer.Start();
	}
}

// Set Timer Interval
void SVMMTimer::SetInterval(const SVString& receiverTag, unsigned long interval)
{
	SVMMTimer& timer = SVMMTimer::Instance();
	typedef SVTimerEventListeners::iterator Iter;
	// Find event handler
	Iter it = timer.m_eventListeners.find(receiverTag);

	// set timer interval
	if (it != timer.m_eventListeners.end())
	{
		it->second.setInterval(interval);
	}
}

// Remove Event handlers
void SVMMTimer::UnSubscribe(const SVString& receiverTag)
{
	SVMMTimer& timer = SVMMTimer::Instance();

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
void CALLBACK SVMMTimer::TimerProc( UINT uTimerID, UINT uRsvd, DWORD_PTR dwUser, DWORD_PTR dwRsvd1, DWORD_PTR dwRsvd2 )
{
	SVMMTimer& timer = SVMMTimer::Instance();

	// Wake up Dispatch Thread
	timer.m_asyncProcedure.Signal(NULL);
}

// Callback for the QueueUserAPC API call
// Do nothing (just wake up thread)
void CALLBACK SVMMTimer::TimerAPCProc( ULONG_PTR dwParam )
{
}

// Event Dispatcher
void SVMMTimer::Dispatch( bool& p_WaitForEvents )
{
	SVTimerEventListeners::iterator it;
	for (it = m_eventListeners.begin();it != m_eventListeners.end();++it)
	{
		it->second.tick(it->first);
		// check elapsed time
		//it->second.tickCount++;
		//
		//if (it->second.tickCount >= it->second.interval)
		//{
		//	it->second.tickCount = 0;
		//	if (it->second.pCallback)
		//	{
		//		it->second.pCallback->Notify(it->first);
		//	}
		//}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTimerLibrary\SVMMTimer.cpp_v  $
 * 
 *    Rev 1.2   01 Dec 2014 14:12:44   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   02 Oct 2013 10:18:18   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 18:35:40   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   19 Jan 2010 15:33:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   23 Jun 2009 14:46:40   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added runtime timer setting and callback
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jan 2009 14:16:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the Start method to only start the thread if there are subscribers.
 * Revised the Stop method to check if the thread is running. 
 * Revised the Subscribe method to start the timer.
 * Revised the Unscribe method to stop the timer if there are no subscribers.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   03 Oct 2008 10:22:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to set the thread priority to Time Critical
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   01 Oct 2008 15:56:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to send tag to SVAsyncProcedure
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 2008 13:38:08   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added PVCS Log block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/