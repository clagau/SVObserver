//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : AcquisitionTriggers
//* .File Name       : $Workfile:   AcquisitionTriggers.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:22  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <map>
#include "TriggerHandling/TriggerBasics.h"
#include "TriggerHandling/CallbackStructContainers.h"
#include "IODeviceBase.h"

namespace SvTi
{
	class AcquisitionTriggers
	{
	private:
		SvTh::TriggerDispatcherMap m_dispatcherVectorCollection;

	public:
		void Add(unsigned long p_ulIndex, const SvTh::TriggerDispatcher& rDispatcher);
		void Remove(unsigned long p_ulIndex, const SvTh::TriggerDispatcher& rDispatcher);
		void RemoveAll(unsigned long p_ulIndex);

		HRESULT Dispatch(unsigned long p_ulIndex);
	};
} //namespace SvTi
