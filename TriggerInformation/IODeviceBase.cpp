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
	HRESULT IODeviceBase::AddDispatcher(unsigned long handle, const SvTh::TriggerDispatcher &rDispatcher)
	{
		///When module ready is true we are in run mode no changes to dispatcher!
		if(false == m_moduleReady)
		{
			return m_TriggerDispatchers.AddDispatcher(handle, rDispatcher);
		}

		return E_FAIL;
	}


	HRESULT IODeviceBase::RemoveDispatcher(unsigned long handle, const SvTh::TriggerDispatcher &rDispatcher)
	{
		///When module ready is true we are in run mode no changes to dispatcher!
		if (false == m_moduleReady)
		{
			return m_TriggerDispatchers.RemoveDispatcher(handle, rDispatcher);
		}

		return E_FAIL;
	}


	HRESULT IODeviceBase::RemoveAllDispatchers(unsigned long handle)
	{
		///When module ready is true we are in run mode no changes to dispatcher!
		if (false == m_moduleReady)
		{
			m_TriggerDispatchers.RemoveAllDispatchers(handle);
			return S_OK;
		}
		return E_FAIL;
	}


	HRESULT IODeviceBase::StartTrigger(unsigned long triggerIndex)
	{
		HRESULT hr = E_FAIL;

		beforeStartTrigger(triggerIndex);

		///Note here the bool value is atomic so no need to use a mutex
		if (m_TriggerDispatchers.StartTrigger(triggerIndex))
		{
			hr = S_OK;
		}

		hr = afterStartTrigger(hr);

		return hr;
	}


	HRESULT IODeviceBase::StopTrigger(unsigned long triggerIndex)
	{
		HRESULT hr = S_FALSE;

		beforeStopTrigger(triggerIndex);

		///Note here the bool value is atomic so no need to use a mutex
		if (m_TriggerDispatchers.StopTrigger(triggerIndex))
		{
			hr = S_OK;
		}

		hr = afterStopTrigger(hr);

		return hr;
	}

} //namespace SvTi
