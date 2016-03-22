//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerHandler
//* .File Name       : $Workfile:   SVTriggerHandler.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:59:36  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
#include "SVTriggerActivator.h"
#include "SVTriggerCallback.h"

class SVTriggerHandler
{
private:
	unsigned long m_handle;
	bool m_bFired;
	SVTriggerActivator m_activator;
	SVTriggerCallback m_callback;

public:
	SVTriggerHandler(unsigned long handle, SVTriggerActivator& activator, SVTriggerCallback& callback);
	SVTriggerHandler(const SVTriggerHandler& rHandler);
	SVTriggerHandler& operator=(const SVTriggerHandler& rHandler);
	
	HRESULT Fire();
	void SetFired(bool bFireState);
	bool HasFired() const;

	void Notify();
};

typedef std::map<unsigned long, SVTriggerHandler> SVTriggerHandlerList;

