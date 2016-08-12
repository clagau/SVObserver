////*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file IODeviceBase.h
/// All Rights Reserved
//*****************************************************************************

/// contains method function bodies for IODeviceBase
//******************************************************************************

#include "stdafx.h"
#include "IODeviceBase.h"

namespace Seidenader { namespace TriggerHandling {

	HRESULT IODeviceBase::AddTriggerCallback(unsigned long handle, SvTh::TriggerCallbackInformation triggerCallbackInfo)
	{
		HRESULT hr = S_FALSE;

		TriggerCallbackMap::iterator it = m_triggerCallbackMap.find(handle);
		if (it != m_triggerCallbackMap.end())
		{
			TriggerCallbackList& list = it->second;

			// check for dups
			TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(TriggerFinder(), triggerCallbackInfo.m_pCallback));

			if (callbackIt != list.end())
			{
				// DUPLICATE Entry!!!
			}
			else
			{
				// add it
				list.push_back(triggerCallbackInfo);
				hr = S_OK;
			}
		}
		else
		{
			// add it
			TriggerCallbackList list;

			list.push_back(triggerCallbackInfo);
			lockIfRequired();
			m_triggerCallbackMap.insert(std::make_pair(handle, list));
			unlockIfRequired();
			hr = S_OK;
		}
		return hr;
	}


	HRESULT IODeviceBase::RemoveTriggerCallback(unsigned long handle, SVTriggerCallbackPtr pCallback)
	{
		lockIfRequired();
		HRESULT hr = S_FALSE;

		TriggerCallbackMap::iterator it = m_triggerCallbackMap.find(handle);
		if (it != m_triggerCallbackMap.end())
		{
			// check if it is in the list
			TriggerCallbackList& list = it->second;

			TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(TriggerFinder(), pCallback));
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
		TriggerCallbackMap::iterator it = m_triggerCallbackMap.find(handle);
		if (it != m_triggerCallbackMap.end())
		{
			TriggerCallbackList& list = it->second;
			for (size_t i = 0;i < list.size();i++)
			{
				list[i].m_IsStarted = false;
			}
			m_triggerCallbackMap.erase(it);
		}
		unlockIfRequired();
		return S_OK;
	}

	HRESULT IODeviceBase::StartTrigger(unsigned long handle)
	{
		HRESULT hr = S_FALSE;

		beforeStartTrigger(handle);

		TriggerCallbackMap::iterator it = m_triggerCallbackMap.find(handle);
		if (it != m_triggerCallbackMap.end())
		{
			TriggerCallbackList& list = it->second;

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

		TriggerCallbackMap::iterator it = m_triggerCallbackMap.find(handle);
		if (it != m_triggerCallbackMap.end())
		{
			TriggerCallbackList& list = it->second;

			for (size_t i = 0;i < list.size();i++)
			{
				list[i].m_IsStarted = false;
			}
			hr = S_OK;
		}

		hr = afterStopTrigger(hr);

		return hr;
	}



} /* namespace TriggerHandling */ } /* namespace Seidenader */
