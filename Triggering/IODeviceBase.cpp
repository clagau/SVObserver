////*****************************************************************************
/// \copyright COPYRIGHT (c) 2016,2016 by Seidenader Maschinenbau GmbH
/// \file IODeviceBase.h
/// All Rights Reserved
//*****************************************************************************

/// contains method function bodies for IODeviceBase
//******************************************************************************

#include "stdafx.h"
#include "IODeviceBase.h"

namespace SvTrig
{
HRESULT IODeviceBase::RegisterCallback(unsigned long triggerIndex, TriggerCallBack pTriggerCallback)
{
	///When module ready is true we are in run mode no changes to callback functions!
	if(false == m_moduleReady)
	{
		m_triggerCallbackMap[triggerIndex] = pTriggerCallback;
		return S_OK;
	}

	return E_FAIL;
}


HRESULT IODeviceBase::UnRegisterCallback(unsigned long triggerIndex)
{
	///When module ready is true we are in run mode no changes to callback functions!
	if (false == m_moduleReady)
	{
		auto iter = m_triggerCallbackMap.find(triggerIndex);
		if (m_triggerCallbackMap.end() != iter)
		{
			m_triggerCallbackMap.erase(iter);
		}
		return S_OK;
	}

	return E_FAIL;
}


HRESULT IODeviceBase::StartTrigger(unsigned long triggerIndex)
{
	beforeStartTrigger(triggerIndex);
	return afterStartTrigger();
}


HRESULT IODeviceBase::StopTrigger(unsigned long triggerIndex)
{
	beforeStopTrigger(triggerIndex);
	return afterStopTrigger();
}

} //namespace SvTrig
