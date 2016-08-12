//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVTriggerCallbackMap
//* .File Name       : $Workfile:   SVTriggerCallbackMap.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:22  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
#include "TriggerBasics.h"
#include "CallbackStructContainers.h"

namespace Seidenader { namespace TriggerHandling {

	class SVTriggerCallbackMap
	{
	private:
		TriggerCallbackMap m_triggerCallbacks;

	public:
		void Add(unsigned long p_ulIndex, const TriggerCallbackInformation& rTriggerCallbackInfo);
		void Remove(unsigned long p_ulIndex, const TriggerCallbackInformation& rTriggerCallbackInfo);
		void RemoveAll(unsigned long p_ulIndex);

		HRESULT Dispatch(unsigned long p_ulIndex);
	};


}}

namespace SvTh = Seidenader::TriggerHandling;