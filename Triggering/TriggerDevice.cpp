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

void __stdcall TriggerDevice::ProcessCallback(ULONG_PTR pCaller)
{
	TriggerDevice* pTriggerDevice = reinterpret_cast<TriggerDevice*> (pCaller);
	if (nullptr != pTriggerDevice)
	{
		bool done {0 == pTriggerDevice->m_triggerQueue.size()};

		while (false == done)
		{
			SvTrig::SVTriggerInfoStruct triggerInfo;
			pTriggerDevice->m_triggerQueue.PopHead(triggerInfo);

			if (triggerInfo.bValid && nullptr != pTriggerDevice->m_pPpqTriggerCallback)
			{
				triggerInfo.lTriggerCount = ++pTriggerDevice->m_triggerCount;
				DWORD sleepDuration {0};
				std::string acquisitionFile;
				//If in the input data it has a valid time stamp value then it is more accurate then use it
				if (0.0 < triggerInfo.m_Data.m_triggerTimestamp)
				{
					triggerInfo.m_triggerTimeStamp = triggerInfo.m_Data.m_triggerTimestamp;
					////@TODO[GRA][10.30][27.01.2023] Need to implement SVO-3902
					//When ObjectID is present then PLC connected so make sure the acquisition is done after the trigger timestamp
					if (triggerInfo.m_Data.m_objectData[0].m_objectID > 0)
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
					if (nullptr != triggerInfo.m_Data.m_pAcquisitionFile)
					{
						acquisitionFile = triggerInfo.m_Data.m_pAcquisitionFile;
					}
				}
				else
				{
					///This is the fallback trigger time stamp
					triggerInfo.m_triggerTimeStamp = SvUl::GetTimeStamp();
				}

				pTriggerDevice->preProcessTriggers(triggerInfo);
				pTriggerDevice->m_pPpqTriggerCallback(std::move(triggerInfo));
				pTriggerDevice->postProcessTriggers(sleepDuration, triggerInfo.m_softwareTrigger, acquisitionFile.c_str());
			}
			done = (0 == pTriggerDevice->m_triggerQueue.size());
		}
	}
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
	m_processThread.Destroy();
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
	HRESULT result = m_processThread.Create( &TriggerDevice::ProcessCallback, m_DeviceName.c_str());

	if( S_OK == result)
	{
		m_processThread.SetPriority( THREAD_PRIORITY_TIME_CRITICAL );

		if(m_processThread.IsActive() )
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
	m_processThread.SetPriority( THREAD_PRIORITY_NORMAL );
	m_processThread.Destroy();
	Reset();

	return S_OK;
}

HRESULT TriggerDevice::Reset()
{
	m_triggerQueue.clear();
	return S_OK;
}


/*
This method adds the new response to the process queue.
*/
void TriggerDevice::Notify(const SvTrig::SVTriggerInfoStruct& rTriggerInfo)
{
	m_triggerQueue.PushTail(rTriggerInfo);
	m_processThread.Signal(reinterpret_cast<ULONG_PTR> (this));
}
}//namespace SvTrig
