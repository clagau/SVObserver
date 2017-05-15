//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : 
//* .File Name       : $Workfile:   AcquisitionTriggers.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 18:58:04  $
//******************************************************************************
#include "stdafx.h"
#include "AcquisitionTriggers.h"

namespace SvTh
{
	void AcquisitionTriggers::Add(unsigned long p_ulIndex, const TriggerDispatcher& rDispatcher)
	{
		DispatcherVector& callbacks = m_dispatcherVectorCollection[p_ulIndex];
		// find in callbacks - if found replace, else add
		bool bFound = false;
		DispatcherVector::iterator listIt;
		for (listIt = callbacks.begin();listIt != callbacks.end();++listIt)
		{
			if (listIt->getCallback() == rDispatcher.getCallback())
			{
				(*listIt) = rDispatcher;
				bFound = true;
				break;
			}
		}
		if (!bFound)
			callbacks.push_back(rDispatcher);
	}

	void AcquisitionTriggers::Remove(unsigned long p_ulIndex, const TriggerDispatcher& rDispatcher)
	{
		TriggerDispatcherMap::iterator it = m_dispatcherVectorCollection.find(p_ulIndex);
		if (it != m_dispatcherVectorCollection.end())
		{
			DispatcherVector& callbacks = (*it).second;
			DispatcherVector::const_iterator listIt;
			for (listIt = callbacks.begin();listIt != callbacks.end();++listIt)
			{
				if(*listIt == rDispatcher)
				{
					callbacks.erase(listIt);
					break;
				}
			}
		}
	}

	void AcquisitionTriggers::RemoveAll(unsigned long p_ulIndex)
	{
		TriggerDispatcherMap::iterator it = m_dispatcherVectorCollection.find(p_ulIndex);
		if (it != m_dispatcherVectorCollection.end())
		{
			m_dispatcherVectorCollection.erase(it);
		}
	}

	HRESULT AcquisitionTriggers::Dispatch(unsigned long p_ulIndex)
	{
		HRESULT hr = S_FALSE;
		TriggerDispatcherMap::iterator it = m_dispatcherVectorCollection.find(p_ulIndex);
		if (it != m_dispatcherVectorCollection.end())
		{
			hr = S_OK;
			DispatcherVector& callbacks = (*it).second;
			DispatcherVector::iterator listIt;
			for (listIt = callbacks.begin();listIt != callbacks.end();++listIt)
			{
				listIt->Dispatch();
			}
		}
		return hr;
	}
} //namespace SvTh
