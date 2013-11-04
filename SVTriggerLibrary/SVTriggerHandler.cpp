//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerHandler
//* .File Name       : $Workfile:   SVTriggerHandler.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:59:20  $
//******************************************************************************
#include "stdafx.h"
#include "SVTriggerHandler.h"

SVTriggerHandler::SVTriggerHandler(unsigned long handle, SVTriggerActivator& activator, SVTriggerCallback& callback)
: m_bFired(false), m_handle(handle), m_activator(activator), m_callback(callback)
{
}

SVTriggerHandler::SVTriggerHandler(const SVTriggerHandler& rHandler)
: m_bFired(rHandler.m_bFired), m_handle(rHandler.m_handle), m_activator(rHandler.m_activator), m_callback(rHandler.m_callback)
{
}

SVTriggerHandler& SVTriggerHandler::operator=(const SVTriggerHandler& rHandler)
{
	m_bFired = rHandler.m_bFired;
	m_handle = rHandler.m_handle;
	m_activator = rHandler.m_activator;
	m_callback = rHandler.m_callback;
	return *this;
}

HRESULT SVTriggerHandler::Fire()
{
	HRESULT hr = m_activator.Exec(m_handle);
	if (hr == S_OK)
	{
		SetFired(true);
	}
	return hr;
}

void SVTriggerHandler::SetFired(bool bFireState)
{
	::InterlockedExchange(reinterpret_cast<long *>(&m_bFired), bFireState);
}

bool SVTriggerHandler::HasFired() const
{
	return m_bFired;
}

void SVTriggerHandler::Notify()
{
	m_callback.Notify(m_handle);
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerHandler.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:59:20   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:03:30   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
