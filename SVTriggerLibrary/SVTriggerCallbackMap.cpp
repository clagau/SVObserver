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

void SVTriggerCallbackMap::Add(unsigned long p_ulIndex, SVTriggerCallbackStruct& callback)
{
	SVTriggerCallbackList& list = m_triggerCallbacks[p_ulIndex];
	// find in list - if found replace, else add
	bool bFound = false;
	SVTriggerCallbackList::iterator listIt;
	for (listIt = list.begin();listIt != list.end();++listIt)
	{
		if (listIt->pCallback == callback.pCallback)
		{
			(*listIt) = callback;
			bFound = true;
			break;
		}
	}
	if (!bFound)
		list.push_back(callback);
}

void SVTriggerCallbackMap::Remove(unsigned long p_ulIndex, SVTriggerCallbackStruct& callback)
{
	TriggerCallbackMap::iterator it = m_triggerCallbacks.find(p_ulIndex);
	if (it != m_triggerCallbacks.end())
	{
		SVTriggerCallbackList& list = (*it).second;
		SVTriggerCallbackList::iterator listIt;
		for (listIt = list.begin();listIt != list.end();++listIt)
		{
			if (listIt->pCallback == callback.pCallback && 
				listIt->pOwner == callback.pOwner &&
				listIt->pData == callback.pData)
			{
				list.erase(listIt);
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
		SVTriggerCallbackList& list = (*it).second;
		SVTriggerCallbackList::iterator listIt;
		for (listIt = list.begin();listIt != list.end();++listIt)
		{
			listIt->pCallback(listIt->pOwner, listIt->pData);
		}
	}
	return hr;
}

