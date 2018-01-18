//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerDevice
//* .File Name       : $Workfile:   SVSoftwareTriggerDevice.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   25 Apr 2013 17:29:50  $
//******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <map>
//Moved to precompiled header: #include <vector>
#include "SVTimerLibrary/SVTimerCallbackImpl.h"
#include "TriggerInformation/IODeviceBase.h"
#include "SVContainerLibrary/SVBidirectionalMap.h"
#include "SVSystemLibrary/SVCriticalSection.h"

#pragma endregion Includes

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
class SVSoftwareTriggerDevice : public SvTi::IODeviceBase
{
	///////////////////////////////////////////////////////////////////////
	//
	///////////////////////////////////////////////////////////////////////
	struct SVSoftwareTimerStruct
	{
		long timerPeriod;
		SvTl::SVTimerCallbackImpl<SVSoftwareTriggerDevice, void> timerCallback;
	};
	typedef std::map<unsigned long, SVSoftwareTimerStruct> TimerList;

	enum ParameterEnum
	{
		SVTriggerPeriod	= 0,
		SVBoardVersion	= 1
	};

protected:
	typedef SVBidirectionalMap<std::string, unsigned long>::type NameHandleList;
	NameHandleList m_nameHandleList;
	TimerList m_timerList;
	int m_numTriggers;

public:
	SVSoftwareTriggerDevice();
	virtual ~SVSoftwareTriggerDevice();

	HRESULT Initialize(bool bInit);

	unsigned long GetTriggerCount();
	unsigned long GetTriggerHandle(unsigned long index);
	BSTR GetTriggerName(unsigned long handle);

	HRESULT TriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount );
	HRESULT TriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT TriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT TriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );

protected:
	void lockIfRequired() override {m_CritSec.Lock();}
	void unlockIfRequired() override {m_CritSec.Unlock();}
	void beforeStartTrigger(unsigned long handle) override {SetTimerCallback(handle);	m_CritSec.Lock();}
	HRESULT afterStartTrigger(HRESULT hr) override {m_CritSec.Unlock();return hr;}
	virtual void beforeStopTrigger(unsigned long handle) override {RemoveTimerCallback(handle);m_CritSec.Lock();}
	HRESULT afterStopTrigger(HRESULT hr) override {m_CritSec.Unlock();return hr;}


private:
	HRESULT GetTriggerPeriod( unsigned long handle, long* p_lPeriod ) const;
	HRESULT SetTriggerPeriod( unsigned long handle, long p_lPeriod );

	HRESULT SetTimerCallback(unsigned long handle);
	HRESULT RemoveTimerCallback(unsigned long handle);
	void OnSoftwareTimer(const std::string& tag);
	SVCriticalSection m_CritSec;
};

