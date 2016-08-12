//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   SVTriggerCallbackMap.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:04  $
//******************************************************************************
#include "stdafx.h"
#include "SVTriggerCallbackMap.h"

namespace Seidenader { namespace TriggerHandling {

	void SVTriggerCallbackMap::Add(unsigned long p_ulIndex, const TriggerCallbackInformation& rTriggerCallbackInfo)
	{
		TriggerCallbackList& callbacks = m_triggerCallbacks[p_ulIndex];
		// find in callbacks - if found replace, else add
		bool bFound = false;
		TriggerCallbackList::iterator listIt;
		for (listIt = callbacks.begin();listIt != callbacks.end();++listIt)
		{
			if (listIt->m_pCallback == rTriggerCallbackInfo.m_pCallback)
			{
				(*listIt) = rTriggerCallbackInfo;
				bFound = true;
				break;
			}
		}
		if (!bFound)
			callbacks.push_back(rTriggerCallbackInfo);
	}

	void SVTriggerCallbackMap::Remove(unsigned long p_ulIndex, const TriggerCallbackInformation& rTriggerCallbackInfo)
	{
		TriggerCallbackMap::iterator it = m_triggerCallbacks.find(p_ulIndex);
		if (it != m_triggerCallbacks.end())
		{
			TriggerCallbackList& callbacks = (*it).second;
			TriggerCallbackList::iterator listIt;
			for (listIt = callbacks.begin();listIt != callbacks.end();++listIt)
			{
				if (listIt->m_pCallback == rTriggerCallbackInfo.m_pCallback && 
					listIt->m_TriggerParameters.m_pOwner == rTriggerCallbackInfo.m_TriggerParameters.m_pOwner &&
					listIt->m_TriggerParameters.m_pData == rTriggerCallbackInfo.m_TriggerParameters.m_pData)
				{
					callbacks.erase(listIt);
					break;
				}
			}
		}
	}

	void SVTriggerCallbackMap::RemoveAll(unsigned long p_ulIndex)
	{
		TriggerCallbackMap::iterator it = m_triggerCallbacks.find(p_ulIndex);
		if (it != m_triggerCallbacks.end())
		{
			m_triggerCallbacks.erase(it);
		}
	}

	HRESULT SVTriggerCallbackMap::Dispatch(unsigned long p_ulIndex)
	{
		HRESULT hr = S_FALSE;
		TriggerCallbackMap::iterator it = m_triggerCallbacks.find(p_ulIndex);
		if (it != m_triggerCallbacks.end())
		{
			hr = S_OK;
			TriggerCallbackList& callbacks = (*it).second;
			TriggerCallbackList::iterator listIt;
			for (listIt = callbacks.begin();listIt != callbacks.end();++listIt)
			{
				listIt->m_pCallback(listIt->m_TriggerParameters);
			}
		}
		return hr;
	}

}}
