//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVInternalTrigger
//* .File Name       : $Workfile:   SVInternalTrigger.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 14:15:24  $
//******************************************************************************

#pragma once

//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/function.hpp>
#include "TriggerHandling/SVTriggerHandler.h"
#include "SVSystemLibrary/SVThread.h"

namespace SvTi
{
	class SVInternalTrigger
	{
		typedef boost::function<void(bool&)> ThreadSignalHandler;

	public:
		SVInternalTrigger();
		~SVInternalTrigger();

		void Create(LPCTSTR p_strOwner, SVThreadAttribute eAttr);
		void Destroy();

		HRESULT Subscribe(unsigned long p_ulIndex, SvTh::SVTriggerHandler& handler);
		HRESULT Unsubscribe(unsigned long p_ulIndex);
		bool HasSubscribers(unsigned long p_ulIndex) const;

		HRESULT Fire(unsigned long p_ulIndex);

	private:
		SVThread<ThreadSignalHandler> m_thread;
		SvTh::SVTriggerHandlerList m_triggerHandlerList;

		void Dispatch(bool& p_WaitForEvents);
	};

} //namespace SvTi

