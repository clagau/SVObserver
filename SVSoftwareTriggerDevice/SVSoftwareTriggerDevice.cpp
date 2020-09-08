//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDevice
//* .File Name       : $Workfile:   SVSoftwareTriggerDevice.cpp  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:29:22  $
//******************************************************************************

#pragma region Includes
#include "stdafx.h"
#include "SVSoftwareTriggerDevice.h"
#include "SVIOLibrary/SVIOParameterEnum.h"
#include "SVTimerLibrary/SVClock.h"
#pragma endregion Includes

constexpr long SVDefaultTimerPeriod = 200;
constexpr unsigned int cTimerResolution = 1;
constexpr LPCTSTR cTriggerName = "SoftwareTrigger_1.Dig_";			///This name must match the name in the SVHardwareManifest

void CALLBACK TimerProc(UINT timerID, UINT , DWORD_PTR pData, DWORD_PTR , DWORD_PTR )
{
	SVSoftwareTriggerDevice* pDevice = reinterpret_cast<SVSoftwareTriggerDevice*> (pData);

	if(nullptr != pDevice)
	{
		for(const auto& rTimer : pDevice->getTimerList())
		{
			if(timerID == rTimer.m_timerID)
			{
				pDevice->dispatchTrigger(rTimer.m_triggerIndex);
			}
		}
	}
}

HRESULT SVSoftwareTriggerDevice::Initialize(bool init)
{
	HRESULT hr = S_OK;
	if (false == init)
	{
		for(auto& rTimer : m_timerList)
		{
			::timeKillEvent(rTimer.m_timerID);
			rTimer.m_timerID = 0;
		}
		::timeEndPeriod(cTimerResolution);

		m_triggerCallbackMap.clear();
	}
	return hr;
}

unsigned long SVSoftwareTriggerDevice::GetTriggerCount()
{
	return cMaxSoftwareTriggers;
}

unsigned long SVSoftwareTriggerDevice::GetTriggerHandle(unsigned long index)
{
	return index + 1;
}

BSTR SVSoftwareTriggerDevice::GetTriggerName(unsigned long triggerIndex)
{
	std::string triggerName {cTriggerName};

	triggerName += std::to_string(triggerIndex - 1);
	BSTR name = _bstr_t(triggerName.c_str()).Detach();
	return name;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterCount(unsigned long , unsigned long* pCount)
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pCount )
	{
		*pCount = 2;
		l_hrOk = S_OK;
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterName( unsigned long, unsigned long index, BSTR* pName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pName)
	{
		if ( nullptr != *pName)
		{
			::SysFreeString(*pName);
			*pName = nullptr;
		}

		// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
		// however at some time in the future the Dll parameters may be implemented
		// as an array and therefore this enum may not apply.
		switch ( index )
		{
			case SVTriggerPeriod:
			*pName = ::SysAllocString( L"Trigger Timer Period" );
			break;
			
			case SVBoardVersion:
			*pName = ::SysAllocString( L"Board Version");
			break;
		}
		if ( nullptr != *pName)
		{
			l_hrOk = S_OK;
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterValue( unsigned long triggerIndex, unsigned long Index, VARIANT* pValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pValue && S_OK == ::VariantClear( pValue ))
	{
		// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
		// however at some time in the future the Dll parameters may be implemented
		// as an array and therefore this enum may not apply.
		switch ( Index)
		{
			case SVTriggerPeriod:
			{
				pValue->vt = VT_I4;
				pValue->lVal = GetTriggerPeriod(triggerIndex);
				l_hrOk = S_OK;
				break;
			}
				
			case SVBoardVersion:
			{
				pValue->vt = VT_BSTR;
				pValue->bstrVal = ::SysAllocString(L"Software Trigger Module 1.0 firmware 0x0500");;
				l_hrOk = S_OK;
				break;
			}

			default:
				break;
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::TriggerSetParameterValue( unsigned long triggerIndex, unsigned long Index, VARIANT* pValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != pValue )
	{
		switch (Index)
		{
			case SVTriggerPeriod:
			{
				if( pValue->vt == VT_I4 )
				{
					l_hrOk = SetTriggerPeriod( triggerIndex, pValue->lVal );
				}
				break;
			}
		}
	}

	return l_hrOk;
}

void SVSoftwareTriggerDevice::dispatchTrigger(unsigned long triggerIndex)
{
	if (m_moduleReady)
	{
		int triggerChannel = triggerIndex - 1;

		SvTi::IntVariantMap triggerData;
		triggerData[SvTi::TriggerDataEnum::TimeStamp] = _variant_t(SvTl::GetTimeStamp());
		triggerData[SvTi::TriggerDataEnum::TriggerChannel] = _variant_t(triggerChannel);

		auto iter = m_triggerCallbackMap.find(triggerIndex);
		if (m_triggerCallbackMap.end() != iter)
		{
			iter->second(triggerData);
		}

		///Check if a new trigger period
		if(triggerChannel >= 0 && triggerChannel < m_timerList.size() && m_timerList[triggerChannel].m_newPeriod)
		{
			::timeKillEvent(m_timerList[triggerChannel].m_timerID);
			m_timerList[triggerChannel].m_newPeriod = false;
			if(0 != m_timerList[triggerChannel].m_period)
			{
				m_timerList[triggerChannel].m_timerID = ::timeSetEvent(m_timerList[triggerChannel].m_period, 0, TimerProc, reinterpret_cast<DWORD_PTR> (this), TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
			}
		}
	}
}

void SVSoftwareTriggerDevice::beforeStartTrigger(unsigned long triggerIndex)
{
	if(false == m_moduleReady)
	{
		::timeBeginPeriod(cTimerResolution);
		m_moduleReady = true;
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	}
	int triggerChannel = triggerIndex - 1;
	TimerInfo& rTimer = m_timerList[triggerChannel];
	assert(0 == rTimer.m_timerID);
	rTimer.m_newPeriod = false;
	rTimer.m_timerID = ::timeSetEvent(rTimer.m_period, 0, TimerProc, reinterpret_cast<DWORD_PTR> (this), TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
}

void SVSoftwareTriggerDevice::beforeStopTrigger(unsigned long triggerIndex)
{
	int triggerChannel = triggerIndex - 1;
	TimerInfo& rTimer = m_timerList[triggerChannel];
	if(TIMERR_NOERROR != ::timeKillEvent(rTimer.m_timerID))
	{
		assert(false);
	}
	rTimer.m_timerID = 0;
	m_moduleReady = false;

	if (std::none_of(m_timerList.begin(), m_timerList.end(), [](const auto& rEntry) { return rEntry.m_timerID != 0; }))
	{
		::timeEndPeriod(cTimerResolution);
		::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	}
}
long SVSoftwareTriggerDevice::GetTriggerPeriod( unsigned long triggerIndex) const
{
	int triggerChannel = triggerIndex - 1;
	return static_cast<long> (m_timerList[triggerChannel].m_period);
}

HRESULT SVSoftwareTriggerDevice::SetTriggerPeriod(unsigned long triggerIndex, long period)
{
	if(0 <= period)
	{
		int triggerChannel = triggerIndex - 1;
		TimerInfo& rTimer = m_timerList[triggerChannel];
		bool restartTimer = (0 == rTimer.m_period);
		rTimer.m_period = static_cast<uint16_t> (period);
		rTimer.m_triggerIndex = triggerIndex;
		if (true == m_moduleReady)
		{
			if(restartTimer && 0 != rTimer.m_period)
			{
				rTimer.m_timerID = ::timeSetEvent(rTimer.m_period, 0, TimerProc, reinterpret_cast<DWORD_PTR> (this), TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
			}
			else
			{
				rTimer.m_newPeriod = true;
			}
		}
	}
	return S_OK;
}

