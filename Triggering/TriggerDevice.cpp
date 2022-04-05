// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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

namespace SvTrig
{

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
	auto threadProcess = [this](bool& rProcessed) {return Process(rProcessed); };
	HRESULT result = m_Thread.Create( &TriggerDevice::APCProc, threadProcess, m_DeviceName.c_str());

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
	Reset();

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
			std::string acquisitionFile;
			//If in the input data it has a valid time stamp value then it is more accurate then use it
			const _variant_t& rTimeStamp = triggerInfo.m_Data[SvTrig::TriggerDataEnum::TimeStamp];
			if (VT_R8 == rTimeStamp.vt && 0.0 < rTimeStamp.dblVal)
			{
				triggerInfo.m_triggerTimeStamp = rTimeStamp.dblVal;
				//When ObjectID is present then PLC connected so make sure the acquisition is done after the trigger timestamp
				const _variant_t& rObjectID = triggerInfo.m_Data[SvTrig::TriggerDataEnum::ObjectID];
				if (VT_UI4 == rObjectID.vt && rObjectID.ulVal > 0)
				{
					double timeDifference = triggerInfo.m_triggerTimeStamp - SvUl::GetTimeStamp();
					bool isTimeInTheFuture = timeDifference > 0.0;
					if (isTimeInTheFuture)
					{
						sleepDuration = static_cast<DWORD> (timeDifference);
						//Add 1ms as time difference has a rest and we need to make sure the sleep duration is longer
						++sleepDuration;
					}
				}
				const _variant_t& rAcquisitionFile = triggerInfo.m_Data[SvTrig::TriggerDataEnum::AcquisitionFile];
				if (VT_BSTR == rAcquisitionFile.vt)
				{
					acquisitionFile = _bstr_t(rAcquisitionFile.bstrVal);
				}
			}
			else
			{
				///This is the fallback trigger time stamp
				triggerInfo.m_triggerTimeStamp = SvUl::GetTimeStamp();
			}

			const _variant_t& rSoftwareTrigger = triggerInfo.m_Data[SvTrig::TriggerDataEnum::SoftwareTrigger];
			bool softwareTrigger = (VT_BOOL == rSoftwareTrigger.vt) ? (rSoftwareTrigger ? true : false) : false;

			preProcessTriggers(triggerInfo);
			m_pPpqTriggerCallback(std::move(triggerInfo));
			postProcessTriggers(sleepDuration, softwareTrigger, acquisitionFile.c_str());
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
}//namespace SvTrig
