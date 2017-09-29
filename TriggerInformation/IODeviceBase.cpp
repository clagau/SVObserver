////*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file IODeviceBase.h
/// All Rights Reserved
//*****************************************************************************

/// contains method function bodies for IODeviceBase
//******************************************************************************

#include "stdafx.h"
#include "IODeviceBase.h"

namespace SvTi
{
	HRESULT IODeviceBase::AddTriggerCallback(unsigned long handle, const SvTh::TriggerDispatcher &rDispatcher)
	{
		HRESULT hr = S_FALSE;

		SvTh::TriggerDispatcherMap::iterator it = m_triggerDispatcherMap.find(handle);
		if (it != m_triggerDispatcherMap.end())
		{
			SvTh::DispatcherVector& list = it->second;

			// check for dups
			SvTh::DispatcherVector::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(TriggerFinder(), rDispatcher.getCallback()));

			if (callbackIt != list.end())
			{
				// DUPLICATE Entry!!!
			}
			else
			{
				// add it
				list.push_back(rDispatcher);
				hr = S_OK;
			}
		}
		else
		{
			// add it
			SvTh::DispatcherVector list;

			list.push_back(rDispatcher);
			lockIfRequired();
			m_triggerDispatcherMap.insert(std::make_pair(handle, list));
			unlockIfRequired();
			hr = S_OK;
		}
		return hr;
	}


	HRESULT IODeviceBase::RemoveTriggerCallback(unsigned long handle, SvTh::SVTriggerCallbackPtr pCallback)
	{
		lockIfRequired();
		HRESULT hr = S_FALSE;

		SvTh::TriggerDispatcherMap::iterator it = m_triggerDispatcherMap.find(handle);
		if (it != m_triggerDispatcherMap.end())
		{
			// check if it is in the list
			SvTh::DispatcherVector& list = it->second;

			SvTh::DispatcherVector::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(TriggerFinder(), pCallback));
			if (callbackIt != list.end())
			{
				list.erase(callbackIt);
				hr = S_OK;
			}
		}
		unlockIfRequired();
		return hr;
	}


	HRESULT IODeviceBase::RemoveAllTriggerCallbacks(unsigned long handle)
	{
		lockIfRequired();
		SvTh::TriggerDispatcherMap::iterator it = m_triggerDispatcherMap.find(handle);
		if (it != m_triggerDispatcherMap.end())
		{
			SvTh::DispatcherVector& list = it->second;
			for (size_t i = 0;i < list.size();i++)
			{
				list[i].m_IsStarted = false;
			}
			m_triggerDispatcherMap.erase(it);
		}
		unlockIfRequired();
		return S_OK;
	}

	HRESULT IODeviceBase::StartTrigger(unsigned long handle)
	{
		HRESULT hr = S_FALSE;

		beforeStartTrigger(handle);

		SvTh::TriggerDispatcherMap::iterator it = m_triggerDispatcherMap.find(handle);
		if (it != m_triggerDispatcherMap.end())
		{
			SvTh::DispatcherVector& list = it->second;

			for (size_t i = 0;i < list.size();i++)
			{
				list[i].m_IsStarted = true;
			}
			hr = S_OK;
		}
		hr = afterStartTrigger(hr);

		return hr;
	}


	HRESULT IODeviceBase::StopTrigger(unsigned long handle)
	{
		HRESULT hr = S_FALSE;

		beforeStopTrigger(handle);

		SvTh::TriggerDispatcherMap::iterator it = m_triggerDispatcherMap.find(handle);
		if (it != m_triggerDispatcherMap.end())
		{
			SvTh::DispatcherVector& list = it->second;

			for (size_t i = 0;i < list.size();i++)
			{
				list[i].m_IsStarted = false;
			}
			hr = S_OK;
		}

		hr = afterStopTrigger(hr);

		return hr;
	}

} //namespace SvTi
