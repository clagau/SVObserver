// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : TriggerDevice
// * .File Name       : $Workfile:   TriggerDevice.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Dec 2014 13:26:40  $
// ******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "TriggerDevice.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

void CALLBACK TriggerDevice::APCProc(ULONG_PTR)
{
}

TriggerDevice::TriggerDevice(LPCTSTR deviceName)
: m_DeviceName(deviceName)
{
}

void TriggerDevice::ClearDevice()
{
	if (false == m_isStarted)
	{
		m_pPpqTriggerCallback = nullptr;
	}
	m_triggerQueue.clear();
}

HRESULT TriggerDevice::Destroy()
{
	m_isStarted = false;
	m_Thread.Destroy();
	m_pPpqTriggerCallback = nullptr;
	m_triggerQueue.clear();
	return S_OK;
}

HRESULT TriggerDevice::RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback)
{
	if (false == m_isStarted)
	{
		m_pPpqTriggerCallback = pPpqTriggerCallback;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT TriggerDevice::UnregisterCallback()
{
	if (false == m_isStarted)
	{
		m_pPpqTriggerCallback = nullptr;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT TriggerDevice::Start()
{
	HRESULT result = m_Thread.Create( &TriggerDevice::APCProc, std::bind(&TriggerDevice::Process, this, std::placeholders::_1), m_DeviceName.c_str(), SVAffinityAcq );

	if( S_OK == result)
	{
		m_Thread.SetPriority( THREAD_PRIORITY_TIME_CRITICAL );

		if( m_Thread.IsActive() )
		{
			Reset();
			m_isStarted = true;
		}
	}

	return result;
}

HRESULT TriggerDevice::Stop()
{
	m_isStarted = false;
	m_Thread.SetPriority( THREAD_PRIORITY_NORMAL );
	m_Thread.Destroy();

	return S_OK;
}

HRESULT TriggerDevice::Reset()
{
	m_triggerQueue.clear();
	return S_OK;
}

void TriggerDevice::Process(bool&)
{
	bool done{ 1 > m_triggerQueue.size() };

	while(false == done)
	{
		SvTi::SVTriggerInfoStruct triggerInfo;
		m_triggerQueue.PopHead(triggerInfo);

		if (triggerInfo.bValid && nullptr != m_pPpqTriggerCallback)
		{
			triggerInfo.lTriggerCount = ++m_triggerCount;
			//If in the input data it has a valid time stamp value then it is more accurate then use it
			SvTi::IntVariantMap::const_iterator iterData{ triggerInfo.m_Data.end() };
			iterData = triggerInfo.m_Data.find(SvTi::TriggerDataEnum::TimeStamp);
			if (triggerInfo.m_Data.end() != iterData && VT_R8 == iterData->second.vt && 0.0 < iterData->second.dblVal)
			{
				triggerInfo.m_triggerTimeStamp = iterData->second.dblVal;
			}
			else
			{
				///This is the fallback trigger time stamp
				triggerInfo.m_triggerTimeStamp = SvTl::GetTimeStamp();
			}

			processAcquisitionTriggers(triggerInfo);
			m_pPpqTriggerCallback(std::move(triggerInfo));
		}
		done = (1 > m_triggerQueue.size());
	}
}

/*
This method adds the new response to the process queue.
*/
void TriggerDevice::Notify(const SvTi::SVTriggerInfoStruct& rTriggerInfo)
{
	m_triggerQueue.PushTail(rTriggerInfo);
	m_Thread.Signal(this);
}

