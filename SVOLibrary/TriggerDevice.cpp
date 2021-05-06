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
#include "SVUtilityLibrary/SVClock.h"
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
		SvTrig::SVTriggerInfoStruct triggerInfo;
		m_triggerQueue.PopHead(triggerInfo);

		if (triggerInfo.bValid && nullptr != m_pPpqTriggerCallback)
		{
			triggerInfo.lTriggerCount = ++m_triggerCount;
			DWORD sleepDuration{ 0 };
			bool softwareTrigger{ false };
			//If in the input data it has a valid time stamp value then it is more accurate then use it
			SvTrig::IntVariantMap::const_iterator iterData = triggerInfo.m_Data.find(SvTrig::TriggerDataEnum::TimeStamp);
			if (triggerInfo.m_Data.end() != iterData && VT_R8 == iterData->second.vt && 0.0 < iterData->second.dblVal)
			{
				triggerInfo.m_triggerTimeStamp = iterData->second.dblVal;
				//When ObjectID is present then PLC connected so make sure the acquisition is done after the trigger timestamp
				iterData = triggerInfo.m_Data.find(SvTrig::TriggerDataEnum::ObjectID);
				if (triggerInfo.m_Data.end() != iterData)
				{
					double timeDifference = triggerInfo.m_triggerTimeStamp - SvUl::GetTimeStamp();
					bool isTimeInTheFuture = timeDifference > 0.0;
					if (isTimeInTheFuture)
					{
						sleepDuration = static_cast<DWORD> (timeDifference);
						//Add 1ms as time difference has a rest and we need to make sure the sleep duartion is longer
						++sleepDuration;
					}
				}
			}
			else
			{
				///This is the fallback trigger time stamp
				triggerInfo.m_triggerTimeStamp = SvUl::GetTimeStamp();
			}

			iterData = triggerInfo.m_Data.find(SvTrig::TriggerDataEnum::SoftwareTrigger);
			if (triggerInfo.m_Data.end() != iterData && VT_BOOL == iterData->second.vt)
			{
				softwareTrigger = iterData->second ? true : false;
			}

			preProcessTriggers(triggerInfo);
			m_pPpqTriggerCallback(std::move(triggerInfo));
			postProcessTriggers(sleepDuration, softwareTrigger);
		}
		done = (1 > m_triggerQueue.size());
	}
}

/*
This method adds the new response to the process queue.
*/
void TriggerDevice::Notify(const SvTrig::SVTriggerInfoStruct& rTriggerInfo)
{
	m_triggerQueue.PushTail(rTriggerInfo);
	m_Thread.Signal(this);
}

