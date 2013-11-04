//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInternalTrigger
//* .File Name       : $Workfile:   SVInternalTrigger.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:55:36  $
//******************************************************************************
#include "stdafx.h"
#include "SVInternalTrigger.h"
#include <boost/bind.hpp>

SVInternalTrigger::SVInternalTrigger()
{
}

SVInternalTrigger::~SVInternalTrigger()
{
	Destroy();
}

void SVInternalTrigger::Create()
{
	m_thread.Create(boost::bind(&SVInternalTrigger::Dispatch, this, _1), _T("SVInternalTrigger"));
	m_thread.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);
}

void SVInternalTrigger::Destroy()
{
	m_thread.SetPriority(THREAD_PRIORITY_NORMAL);
	m_thread.Destroy();

	// Remove all Subscribed Event handlers
	m_triggerHandlerList.clear();
}

bool SVInternalTrigger::HasSubscribers(unsigned long p_ulIndex) const
{
	bool bRetVal = false;
	SVTriggerHandlerList::const_iterator it = m_triggerHandlerList.find(p_ulIndex);
	if (it != m_triggerHandlerList.end())
		bRetVal = true;
	return bRetVal;
}

HRESULT SVInternalTrigger::Subscribe(unsigned long p_ulIndex, SVTriggerHandler& handler)
{
	HRESULT hr = S_OK;
	m_triggerHandlerList.insert(std::make_pair(p_ulIndex, handler));
	return hr;
}

HRESULT SVInternalTrigger::Unsubscribe(unsigned long p_ulIndex)
{
	HRESULT hr = S_FALSE;
	SVTriggerHandlerList::iterator it = m_triggerHandlerList.find(p_ulIndex);
	if (it != m_triggerHandlerList.end())
	{
		m_triggerHandlerList.erase(it);
		hr = S_OK;
	}
	return hr;
}

HRESULT SVInternalTrigger::Fire(unsigned long p_ulIndex)
{
	HRESULT hr = S_FALSE;
	// Find Actuator
	SVTriggerHandlerList::iterator it = m_triggerHandlerList.find(p_ulIndex);
	if (it != m_triggerHandlerList.end())
	{
		hr = (*it).second.Fire();
		if (hr == S_OK)
		{
			(*it).second.Notify();
		}
	}
	return hr;
}

void SVInternalTrigger::Dispatch( bool& p_WaitForEvents )
{
	SVTriggerHandlerList::iterator it;
	for (it = m_triggerHandlerList.begin();it != m_triggerHandlerList.end();++it)
	{
		if (it->second.HasFired())
		{
			it->second.SetFired(false);
			it->second.Notify();
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVInternalTrigger.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:55:36   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Feb 2010 10:03:04   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change thread creation location.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   19 Jan 2010 15:47:46   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Oct 2008 10:23:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to set the thread priority to Time Critical
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2008 15:57:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to send tag to SVAsyncProcedure
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 09:59:42   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
