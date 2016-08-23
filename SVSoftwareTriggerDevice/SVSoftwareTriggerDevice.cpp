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

#include "stdafx.h"
//Moved to precompiled header: #include <boost/config.hpp>
//Moved to precompiled header: #include <boost/assign/list_of.hpp>
//Moved to precompiled header: #include <boost/assign/list_inserter.hpp>
//Moved to precompiled header: #include <functional>
//Moved to precompiled header: #include <algorithm>
//Moved to precompiled header: #include <string>


#include "SVSoftwareTriggerDevice.h"
#include "SVTimerLibrary/SVMMTimer.h"

#pragma comment( lib, "Winmm" )

static const long SVDefaultTimerPeriod = 200;
static const int SVMaximumSoftwareTriggers = 4;

SVSoftwareTriggerDevice::SVSoftwareTriggerDevice()
: m_numTriggers(SVMaximumSoftwareTriggers)
{
	m_nameHandleList = (boost::assign::list_of<NameHandleList::value_type>
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_0")), 1ul))
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_1")), 2ul))
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_2")), 3ul))
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_3")), 4ul))
		).convert_to_container<NameHandleList>();
}

SVSoftwareTriggerDevice::~SVSoftwareTriggerDevice()
{
}

HRESULT SVSoftwareTriggerDevice::Initialize(bool bInit)
{
	HRESULT hr = S_OK;
	if (!bInit)
	{
		SVMMTimer::Stop();

		// Clear all callbacks
		m_triggerDispatcherMap.clear();
		m_timerList.clear();
	}
	else
	{
		m_timerList.clear();

		SVSoftwareTimerStruct timerStruct;
		timerStruct.timerPeriod = SVDefaultTimerPeriod;
		timerStruct.timerCallback.Bind(this, &SVSoftwareTriggerDevice::OnSoftwareTimer);
		
		m_timerList = (boost::assign::map_list_of<unsigned long, SVSoftwareTimerStruct>
			(1, timerStruct)
			(2, timerStruct)
			(3, timerStruct)
			(4, timerStruct)).convert_to_container<TimerList>();


		//SVMMTimer::Start();
	}
	return hr;
}

unsigned long SVSoftwareTriggerDevice::GetTriggerCount()
{
	return m_numTriggers;
}

unsigned long SVSoftwareTriggerDevice::GetTriggerHandle(unsigned long index)
{
	return (unsigned long)(index + 1);
}

BSTR SVSoftwareTriggerDevice::GetTriggerName(unsigned long handle)
{
	BSTR name = nullptr;

	typedef NameHandleList::index_const_iterator<to>::type Iterator;
	Iterator it = m_nameHandleList.get<to>().find(handle);
	if (it != m_nameHandleList.get<to>().end())
	{
		_bstr_t bstrName = _bstr_t(it->first.c_str());
		name = bstrName.Detach();
	}

	return name;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pulCount )
	{
		if ( 0 < triggerchannel )
		{
			*p_pulCount = 2;

			l_hrOk = S_OK;
		}
		else
		{
			*p_pulCount = 0;
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pbstrName )
	{
		if ( nullptr != *p_pbstrName )
		{
			::SysFreeString( *p_pbstrName );
			*p_pbstrName = nullptr;
		}

		if ( 0 < triggerchannel )
		{
			// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
			// however at some time in the future the Dll parameters may be implemented
			// as an array and therefore this enum may not apply.
			switch ( p_ulIndex )
			{
				case SVTriggerPeriod:
				*p_pbstrName = ::SysAllocString( L"Trigger Timer Period" );
				break;
			
				case SVBoardVersion:
				*p_pbstrName = ::SysAllocString( L"Board Version");
				break;
			}
			if ( nullptr != *p_pbstrName )
			{
				l_hrOk = S_OK;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( nullptr != p_pvarValue )
	{
		if ( S_OK == ::VariantClear( p_pvarValue ) )
		{
			if ( 0 < triggerchannel )
			{
				// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				switch ( p_ulIndex)
				{
					case SVTriggerPeriod:
					{
						p_pvarValue->vt = VT_I4;
						l_hrOk = GetTriggerPeriod(triggerchannel, &p_pvarValue->lVal);
						break;
					}
				
					case SVBoardVersion:
					{
						WCHAR wbuf[256];
						p_pvarValue->vt = VT_BSTR;
						
						swprintf( wbuf, L"Software Trigger Module 1.0 firmware 0x0500");
						
						p_pvarValue->bstrVal = ::SysAllocString( wbuf );
						l_hrOk = S_OK;
						break;
					}

					default:
						break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::TriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < triggerchannel )
	{
		if ( nullptr != p_pvarValue )
		{
			switch (p_ulIndex)
			{
				case SVTriggerPeriod:
				{
					if( p_pvarValue->vt == VT_I4 )
					{
						l_hrOk = SetTriggerPeriod( triggerchannel, p_pvarValue->lVal );
					}
					break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::GetTriggerPeriod( unsigned long handle, long* p_lPeriod) const
{
	HRESULT hr = S_FALSE;

	TimerList::const_iterator it = m_timerList.find(handle);
	if (it != m_timerList.end())
	{
		const SVSoftwareTimerStruct& timerInfo = it->second;
		*p_lPeriod = timerInfo.timerPeriod;
		hr = S_OK;
	}
	return hr;
}

HRESULT SVSoftwareTriggerDevice::SetTriggerPeriod( unsigned long handle, long p_lPeriod )
{
	HRESULT hr = S_FALSE;
	m_CritSec.Lock();
	TimerList::iterator it = m_timerList.find(handle);
	if (it != m_timerList.end())
	{
		SVSoftwareTimerStruct& timerInfo = it->second;
		timerInfo.timerPeriod = p_lPeriod;
		typedef NameHandleList::index_const_iterator<to>::type Iterator;
		Iterator nameIt = m_nameHandleList.get<to>().find(handle);
		if (nameIt != m_nameHandleList.get<to>().end())
		{
			SVMMTimer::SetInterval(nameIt->first, p_lPeriod);
		}
		hr = S_OK;
	}
	m_CritSec.Unlock();
	return hr;
}

HRESULT SVSoftwareTriggerDevice::SetTimerCallback(unsigned long handle)
{
	HRESULT hr = S_FALSE;
	m_CritSec.Lock();
	TimerList::iterator it = m_timerList.find(handle);
	if (it != m_timerList.end())
	{
		SVSoftwareTimerStruct& timerInfo = it->second;

		// register callback for SVMMTimer
		timerInfo.timerCallback.Bind(this, &SVSoftwareTriggerDevice::OnSoftwareTimer);
		
		// register callback for MM Timer (need some sort of context)
		typedef NameHandleList::index_const_iterator<to>::type Iterator;
		Iterator nameIt = m_nameHandleList.get<to>().find(handle);
		if (nameIt != m_nameHandleList.get<to>().end())
		{
			SVMMTimer::Subscribe(nameIt->first, timerInfo.timerPeriod, &timerInfo.timerCallback);
			hr = S_OK;
		}
	}
	m_CritSec.Unlock();
	return hr;
}

HRESULT SVSoftwareTriggerDevice::RemoveTimerCallback(unsigned long handle)
{
	HRESULT hr = S_FALSE;
	m_CritSec.Lock();
	TimerList::iterator it = m_timerList.find(handle);
	if (it != m_timerList.end())
	{
		// register callback for MM Timer (need some sort of context)
		typedef NameHandleList::index_const_iterator<to>::type Iterator;
		Iterator nameIt = m_nameHandleList.get<to>().find(handle);
		if (nameIt != m_nameHandleList.get<to>().end())
		{
			SVMMTimer::UnSubscribe(nameIt->first);
			hr = S_OK;
		}
	}
	m_CritSec.Unlock();
	return hr;
}

void SVSoftwareTriggerDevice::DispatchTrigger(const SvTh::TriggerDispatcher& triggerListenerInfo)
{
	triggerListenerInfo.DispatchIfPossible();
}




void SVSoftwareTriggerDevice::OnSoftwareTimer(const SVString& tag)
{
	// find trigger callbacks for this trigger and dispatch
	NameHandleList::const_iterator it = m_nameHandleList.get<from>().find(tag);
	if (it != m_nameHandleList.end())
	{
		unsigned long handle = it->second;

		// get callback list
		m_CritSec.Lock();
		SvTh::TriggerDispatcherMap::iterator triggerIt = m_triggerDispatcherMap.find(handle);
		if (triggerIt != m_triggerDispatcherMap.end())
		{
			SvTh::DispatcherVector& list = triggerIt->second;
			std::for_each(list.begin(), list.end(), SVSoftwareTriggerDevice::DispatchTrigger);
		}
		m_CritSec.Unlock();
	}
}

