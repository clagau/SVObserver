//*****************************************************************************
/// \copyright COPYRIGHT (c) 2016, 2016 by Seidenader Maschinenbau GmbH
/// \file TriggerDispatcherCollection.h
/// All Rights Reserved
//*****************************************************************************
  
/// Containers for trigger-related information
//******************************************************************************

#pragma once

#include "TriggerDispatcher.h"

namespace SvTh
{
	typedef std::vector<TriggerDispatcher> DispatcherVector;

	///\brief Contains the trigger dispatcher lists required by IODeviceBase and classes derived from it
	class TriggerDispatcherCollection
	{
	public:
		void Clear() { m_dispatchers.clear(); }
		const std::map<unsigned long, DispatcherVector>& GetDispatchers() { return m_dispatchers; }

		bool ContainsNoActiveTriggers();
		void DispatchIfPossible(unsigned long triggerIndex);
		bool Dispatch(unsigned long triggerIndex);
		bool AddDispatcher(unsigned long triggerIndex, const TriggerDispatcher &rDispatcher);
		bool RemoveDispatcher(unsigned long triggerIndex, const TriggerDispatcher &rDispatcher);
		void RemoveAllDispatchers(unsigned long triggerIndex);
		bool StartTrigger(unsigned long triggerIndex);
		bool StopTrigger(unsigned long triggerIndex);

	private:
		std::map<unsigned long, DispatcherVector> m_dispatchers;
	};
} //namespace SvTh
