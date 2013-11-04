//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMMTimerEventHandler
//* .File Name       : $Workfile:   SVMMTimerEventHandler.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:36:08  $
//******************************************************************************

#ifndef SVMMTIMEREVENTHANDLER_H
#define SVMMTIMEREVENTHANDLER_H

#include <map>
#include "SVTimerCallback.h"
#include "SVUtilityLibrary/SVString.h"

namespace dummy
{
	class Callback: public SVTimerCallback
	{
		void Notify(const SVString&) {}
	};
}
struct SVMMTimerEventHandler
{
	unsigned long interval;
	//unsigned long newInterval; 
	unsigned long tickCount;
	SVTimerCallback* pCallback;
	dummy::Callback dummy;

	SVMMTimerEventHandler(unsigned long interval_, SVTimerCallback* pCallback_)
	: interval(interval_), pCallback(pCallback_), tickCount(0)//, newInterval(0)
	{}

	SVMMTimerEventHandler()
	: interval(0), pCallback(0), tickCount(0)//, newInterval(0)
	{ pCallback = &dummy; }

	void tick(const SVString & str)
	{
		if (++tickCount >= interval)
		{
			tickCount = 0;
			pCallback->Notify(str);
		}
	}

	void setInterval(unsigned long intrvl)
	{
		interval = intrvl;
	}
};

typedef std::map<SVString, SVMMTimerEventHandler> SVTimerEventListeners;

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTimerLibrary\SVMMTimerEventHandler.h_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:36:08   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
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
