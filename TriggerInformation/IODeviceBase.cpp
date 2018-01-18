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
		HRESULT hr = S_FALSE;

		lockIfRequired();

		if(m_TriggerDispatchers.AddDispatcher(handle, rDispatcher))
		{
			hr = S_OK;
		}

		unlockIfRequired();
		return hr;
	}


	HRESULT IODeviceBase::RemoveDispatcher(unsigned long handle, const SvTh::TriggerDispatcher &rDispatcher)
	{
		lockIfRequired();
		HRESULT hr = S_FALSE;

		if (m_TriggerDispatchers.RemoveDispatcher(handle, rDispatcher))
		{
			hr = S_OK;
		}
		unlockIfRequired();
		return hr;
	}


	HRESULT IODeviceBase::RemoveAllDispatchers(unsigned long handle)
	{
		lockIfRequired();

		m_TriggerDispatchers.RemoveAllDispatchers(handle);
		unlockIfRequired();
		return S_OK;
	}


	HRESULT IODeviceBase::StartTrigger(unsigned long handle)
	{
		HRESULT hr = S_FALSE;

		beforeStartTrigger(handle);

		if (m_TriggerDispatchers.StartTrigger(handle))
		{
			hr = S_OK;
		}

		hr = afterStartTrigger(hr);

		return hr;
	}


	HRESULT IODeviceBase::StopTrigger(unsigned long handle)
	{
		HRESULT hr = S_FALSE;

		beforeStopTrigger(handle);

		if (m_TriggerDispatchers.StopTrigger(handle))
		{
			hr = S_OK;
		}

		hr = afterStopTrigger(hr);

		return hr;
	}

} //namespace SvTi
