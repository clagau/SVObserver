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

namespace SvTh
{
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
		if (S_OK == hr)
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

}//namespace SvTh