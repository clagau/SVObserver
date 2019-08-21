// ******************************************************************************
// * COPYRIGHT (c) 2018, 2018 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .File Name       TriggerDispatcherCollection.cpp
// ******************************************************************************

#include "stdafx.h"
#include "TriggerDispatcherCollection.h"

namespace SvTh
{
	bool TriggerDispatcherCollection::ContainsNoActiveTriggers()
	{
		bool bNoActiveTriggers = true;
		for (auto it = m_dispatchers.begin(); it != m_dispatchers.end(); ++it)
		{
			SvTh::DispatcherVector& list = it->second;

			if (0 == list.size() || list[0].m_IsStarted)
			{
				bNoActiveTriggers = false;
			}
		}
		return bNoActiveTriggers;
	}


	void TriggerDispatcherCollection::DispatchIfPossible(unsigned long channel)
	{
		try
		{
			for (auto &dispatcher : m_dispatchers.at(channel))
			{
				dispatcher.DispatchIfPossible();
			}
		}
		catch (std::out_of_range&)
		{
			//unknown channel: do nothing (this should not normally happen)
		}
	}


	bool TriggerDispatcherCollection::Dispatch(unsigned long channel)
	{
		bool found = false;
		auto it = m_dispatchers.find(channel);
		if (it != m_dispatchers.end())
		{
			found = true;
			SvTh::DispatcherVector& callbacks = (*it).second;
			SvTh::DispatcherVector::iterator listIt;
			for (listIt = callbacks.begin(); listIt != callbacks.end(); ++listIt)
			{
				listIt->Dispatch();
			}
		}

		return found;
	}


	bool TriggerDispatcherCollection::AddDispatcher(unsigned long channel, const SvTh::TriggerDispatcher &rDispatcher)
	{
		auto it = m_dispatchers.find(channel);
		if (it != m_dispatchers.end())
		{
			SvTh::DispatcherVector& list = it->second;

			// check for duplicates
			SvTh::DispatcherVector::iterator callbackIt = std::find_if(list.begin(), list.end(), [rDispatcher](const DispatcherVector::value_type rEntry)->bool
			{
				return (rEntry.getCallback() == rDispatcher.getCallback() && rEntry.GetTriggerParameters().m_pOwner == rDispatcher.GetTriggerParameters().m_pOwner);
			}
			);

			if (callbackIt != list.end())
			{
				// DUPLICATE Entry!!!
				return false;
			}
			else
			{
				// add it
				list.push_back(rDispatcher);
				return true;
			}
		}
		else
		{
			// add it
			SvTh::DispatcherVector list;

			list.push_back(rDispatcher);
			m_dispatchers.insert(std::make_pair(channel, list));
			return true;
		}
	}

	bool TriggerDispatcherCollection::RemoveDispatcher(unsigned long channel, const SvTh::TriggerDispatcher &rDispatcher)
	{
		auto it = m_dispatchers.find(channel);
		if (it != m_dispatchers.end())
		{
			// check if it is in the list
			SvTh::DispatcherVector& list = it->second;

			SvTh::DispatcherVector::iterator callbackIt = std::find_if(list.begin(), list.end(), [rDispatcher](const DispatcherVector::value_type rEntry)->bool
			{
				return (rEntry.getCallback() == rDispatcher.getCallback() && rEntry.GetTriggerParameters().m_pOwner == rDispatcher.GetTriggerParameters().m_pOwner);
			}
			);

			if (callbackIt != list.end())
			{
				list.erase(callbackIt);
				return true;
			}
		}

		return false;
	}


	void TriggerDispatcherCollection::RemoveAllDispatchers(unsigned long channel)
	{
		auto it = m_dispatchers.find(channel);
		if (it != m_dispatchers.end())
		{
			SvTh::DispatcherVector& list = it->second;
			for (size_t i = 0; i < list.size(); i++)
			{
				list[i].m_IsStarted = false;
			}
			m_dispatchers.erase(it);
		}
	}


	bool TriggerDispatcherCollection::StartTrigger(unsigned long channel)
	{
		auto it = m_dispatchers.find(channel);
		if (it != m_dispatchers.end())
		{
			SvTh::DispatcherVector& list = it->second;

			for (size_t i = 0; i < list.size(); i++)
			{
				list[i].m_IsStarted = true;
			}
			return true;
		}

		return false;
	}

	bool TriggerDispatcherCollection::StopTrigger(unsigned long channel)
	{
		auto it = m_dispatchers.find(channel);

		if (it != m_dispatchers.end())
		{
			SvTh::DispatcherVector& list = it->second;

			for (size_t i = 0; i < list.size(); i++)
			{
				list[i].m_IsStarted = false;
			}

			return true;
		}
		return false;
	}
} //namespace SvTh
