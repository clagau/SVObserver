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

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVTriggerLibrary\SVTriggerCallbackMap.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 18:58:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   11 Sep 2008 13:41:46   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added PVCS Header and Log block
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
