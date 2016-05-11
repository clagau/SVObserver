//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInternalTrigger
//* .File Name       : $Workfile:   SVInternalTrigger.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 14:15:24  $
//******************************************************************************
#include "stdafx.h"
//Moved to precompiled header: #include <boost/bind.hpp>
#include "SVInternalTrigger.h"
#include "SVSystemLibrary/SVThreadManager.h"

SVInternalTrigger::SVInternalTrigger()
{
}

SVInternalTrigger::~SVInternalTrigger()
{
	Destroy();
}

void SVInternalTrigger::Create(LPCTSTR p_strOwner, SVThreadAttribute eAttr)
{
	m_thread.Create(boost::bind(&SVInternalTrigger::Dispatch, this, _1), p_strOwner, eAttr);
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
		if (S_OK == hr)
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

