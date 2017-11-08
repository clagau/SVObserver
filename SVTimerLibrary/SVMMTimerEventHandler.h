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

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <map>
#include "SVTimerCallback.h"

#pragma endregion Includes

namespace SvTl
{
	namespace dummy
	{
		class Callback: public SVTimerCallback
		{
			void Notify(const std::string&) {}
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

		void tick(const std::string & str)
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
	typedef std::map<std::string, SVMMTimerEventHandler> SVTimerEventListeners;

} //namespace SvTl
