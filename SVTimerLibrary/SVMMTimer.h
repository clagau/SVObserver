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

#ifndef SVMMTIMER_H
#define SVMMTIMER_H

#include <boost/config.hpp>
#include <boost/function.hpp>
#include "SVMMTimerEventHandler.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"

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
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVTimerLibrary\SVMMTimer.h_v  $
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
 *    Rev 1.0   25 Apr 2013 18:35:54   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jan 2010 15:33:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Sep 2009 09:38:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   23 Jun 2009 14:46:40   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Added runtime timer setting and callback
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 09:55:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/