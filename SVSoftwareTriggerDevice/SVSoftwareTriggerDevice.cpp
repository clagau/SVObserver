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
#include <boost/config.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>

#include <functional>
#include <algorithm>
#include <string>
#include "SVSoftwareTriggerDevice.h"
#include "SVTimerLibrary/SVMMTimer.h"

#pragma comment( lib, "Winmm" )

static const long SVDefaultTimerPeriod = 200;
static const int SVMaximumSoftwareTriggers = 4;

class SVSoftwareTriggerFind : public std::binary_function<SVSoftwareTriggerStruct, SVTriggerCallbackPtr, bool>
{
public:
	bool operator()(const SVSoftwareTriggerStruct& triggerStruct, const SVTriggerCallbackPtr& pCallback) const
	{
		return (triggerStruct.callbackStruct.pCallback == pCallback);
	}
};

SVSoftwareTriggerDevice::SVSoftwareTriggerDevice()
: m_numTriggers(SVMaximumSoftwareTriggers)
{
	m_nameHandleList = boost::assign::list_of<NameHandleList::value_type>
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_0")), reinterpret_cast<HANDLE>(1)))
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_1")), reinterpret_cast<HANDLE>(2)))
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_2")), reinterpret_cast<HANDLE>(3)))
		(std::make_pair(SVString(_T("SoftwareTrigger_1.Dig_3")), reinterpret_cast<HANDLE>(4)));
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
		m_triggerList.clear();
		m_timerList.clear();
	}
	else
	{
		m_timerList.clear();

		SVSoftwareTimerStruct timerStruct;
		timerStruct.timerPeriod = SVDefaultTimerPeriod;
		timerStruct.timerCallback.Bind(this, &SVSoftwareTriggerDevice::OnSoftwareTimer);
		
		m_timerList = boost::assign::map_list_of<HANDLE, SVSoftwareTimerStruct>
			(reinterpret_cast<HANDLE>(1), timerStruct)
			(reinterpret_cast<HANDLE>(2), timerStruct)
			(reinterpret_cast<HANDLE>(3), timerStruct)
			(reinterpret_cast<HANDLE>(4), timerStruct);

		//SVMMTimer::Start();
	}
	return hr;
}

unsigned long SVSoftwareTriggerDevice::GetTriggerCount()
{
	return m_numTriggers;
}

HANDLE SVSoftwareTriggerDevice::GetTriggerHandle(unsigned long index)
{
	return (HANDLE)(index + 1);
}

BSTR SVSoftwareTriggerDevice::GetTriggerName(HANDLE handle)
{
	BSTR name = NULL;

	typedef NameHandleList::index_const_iterator<to>::type Iterator;
	Iterator it = m_nameHandleList.get<to>().find(handle);
	if (it != m_nameHandleList.get<to>().end())
	{
		_bstr_t bstrName = it->first.ToBSTR();
		name = bstrName.Detach();
	}

	return name;
}

HRESULT SVSoftwareTriggerDevice::AddTriggerCallback(HANDLE handle, SVTriggerCallbackPtr pCallback, void* pOwner, void* pData)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;

		// check for dups
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVSoftwareTriggerFind(), pCallback));
		
		if (callbackIt != list.end())
		{
			// DUPLICATE Entry!!!
		}
		else
		{
			// add it
			SVSoftwareTriggerStruct triggerStruct;
			triggerStruct.bStarted = false;
			triggerStruct.callbackStruct.pCallback = pCallback;
			triggerStruct.callbackStruct.pOwner = pOwner;
			triggerStruct.callbackStruct.pData = pData;

			list.push_back(triggerStruct);
			hr = S_OK;
		}
	}
	else
	{
		// add it
		TriggerCallbackList list;
		SVSoftwareTriggerStruct triggerStruct;
		triggerStruct.bStarted = false;
		triggerStruct.callbackStruct.pCallback = pCallback;
		triggerStruct.callbackStruct.pOwner = pOwner;
		triggerStruct.callbackStruct.pData = pData;

		list.push_back(triggerStruct);
		m_triggerList.insert(std::make_pair(handle, list));
			
		hr = S_OK;
	}
	return hr;
}

HRESULT SVSoftwareTriggerDevice::RemoveTriggerCallback(HANDLE handle, SVTriggerCallbackPtr pCallback)
{
	HRESULT hr = S_FALSE;

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		// check if it is in the list
		TriggerCallbackList& list = it->second;
		
		TriggerCallbackList::iterator callbackIt = std::find_if(list.begin(), list.end(), std::bind2nd(SVSoftwareTriggerFind(), pCallback));
		if (callbackIt != list.end())
		{
			list.erase(callbackIt);
			hr = S_OK;
		}
	}
	return hr;
}

HRESULT SVSoftwareTriggerDevice::RemoveAllTriggerCallbacks(HANDLE handle)
{
	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		m_triggerList.erase(it);
	}
	return S_OK;
}

HRESULT SVSoftwareTriggerDevice::StartTrigger(HANDLE handle)
{
	HRESULT hr = S_FALSE;

	SetTimerCallback(handle);

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			if (!list[i].bStarted)
			{
				list[i].bStarted = true;	
			}
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT SVSoftwareTriggerDevice::StopTrigger(HANDLE handle)
{
	HRESULT hr = S_FALSE;

	RemoveTimerCallback(handle);

	TriggerList::iterator it = m_triggerList.find(handle);
	if (it != m_triggerList.end())
	{
		TriggerCallbackList& list = it->second;
		
		for (size_t i = 0;i < list.size();i++)
		{
			list[i].bStarted = false;
		}
		hr = S_OK;
	}
	return hr;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterCount( HANDLE p_ulHandle, unsigned long *p_pulCount )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pulCount != NULL )
	{
		if ( 0 < p_ulHandle )
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

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterName( HANDLE p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pbstrName != NULL )
	{
		if ( *p_pbstrName != NULL )
		{
			::SysFreeString( *p_pbstrName );
			*p_pbstrName = NULL;
		}

		if ( 0 < p_ulHandle )
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
			if ( *p_pbstrName != NULL )
			{
				l_hrOk = S_OK;
			}
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::TriggerGetParameterValue( HANDLE p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( p_pvarValue != NULL )
	{
		if ( ::VariantClear( p_pvarValue ) == S_OK )
		{
			if ( 0 < p_ulHandle )
			{
				// SVTriggerPeriod and SVBoardVersion enums are used here to make the code more clear.
				// however at some time in the future the Dll parameters may be implemented
				// as an array and therefore this enum may not apply.
				switch ( p_ulIndex)
				{
					case SVTriggerPeriod:
					{
						p_pvarValue->vt = VT_I4;
						l_hrOk = GetTriggerPeriod(p_ulHandle, &p_pvarValue->lVal);
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

HRESULT SVSoftwareTriggerDevice::TriggerSetParameterValue( HANDLE p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue )
{
	HRESULT l_hrOk = S_FALSE;

	if ( 0 < p_ulHandle )
	{
		if ( p_pvarValue != NULL )
		{
			switch (p_ulIndex)
			{
				case SVTriggerPeriod:
				{
					if( p_pvarValue->vt == VT_I4 )
					{
						l_hrOk = SetTriggerPeriod( p_ulHandle, p_pvarValue->lVal );
					}
					break;
				}
			}
		}
	}
	return l_hrOk;
}

HRESULT SVSoftwareTriggerDevice::GetTriggerPeriod( HANDLE handle, long* p_lPeriod) const
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

HRESULT SVSoftwareTriggerDevice::SetTriggerPeriod( HANDLE handle, long p_lPeriod )
{
	HRESULT hr = S_FALSE;
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
	return hr;
}

HRESULT SVSoftwareTriggerDevice::SetTimerCallback(HANDLE handle)
{
	HRESULT hr = S_FALSE;
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
	return hr;
}

HRESULT SVSoftwareTriggerDevice::RemoveTimerCallback(HANDLE handle)
{
	HRESULT hr = S_FALSE;
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
	return hr;
}

void SVSoftwareTriggerDevice::DispatchTrigger(const SVSoftwareTriggerStruct& triggerListener)
{
	if (triggerListener.bStarted && triggerListener.callbackStruct.pCallback)
	{
		triggerListener.callbackStruct.pCallback(triggerListener.callbackStruct.pOwner, triggerListener.callbackStruct.pData);
	}
}

void SVSoftwareTriggerDevice::OnSoftwareTimer(const SVString& tag)
{
	// find trigger callbacks for this trigger and dispatch
	NameHandleList::const_iterator it = m_nameHandleList.get<from>().find(tag);
	if (it != m_nameHandleList.end())
	{
		HANDLE handle = it->second;

		// get callback list
		TriggerList::iterator triggerIt = m_triggerList.find(handle);
		if (triggerIt != m_triggerList.end())
		{
			TriggerCallbackList& list = triggerIt->second;
			std::for_each(list.begin(), list.end(), SVSoftwareTriggerDevice::DispatchTrigger);
		}
	}
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVSoftwareTriggerDevice\SVSoftwareTriggerDevice.cpp_v  $
 * 
 *    Rev 1.0   25 Apr 2013 17:29:22   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   23 Jun 2009 14:42:08   jhanebach
 * Project:  SVObserver
 * Change Request (SCR) nbr:  654
 * SCR Title:  Software Trigger Runtime Adjustments
 * Checked in by:  jHanebach;  Jack Hanebach
 * Change Description:  
 *   Runtime trigger interval changes
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   29 Jan 2009 14:20:00   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised the Initialize method to not start the multimedia timer.
 * Revised the StopTrigger method to call the RemoveTimerCallback method.
 * Added the RemoveTimerCallback method.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   03 Oct 2008 17:26:18   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Changed Frequency to Period
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:25:16   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/


