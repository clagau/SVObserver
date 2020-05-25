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
#include "TriggerInformation/IODeviceBase.h"
#pragma endregion Includes

constexpr unsigned long cMaxSoftwareTriggers = 4;

class SVSoftwareTriggerDevice : public SvTi::IODeviceBase
{
	struct TimerInfo
	{
		std::atomic_uint16_t m_timerID {0};
		std::atomic_ulong m_triggerIndex {0UL};
		std::atomic_uint16_t m_period {0};
		std::atomic_bool m_newPeriod {false};
	};

	enum ParameterEnum
	{
		SVTriggerPeriod	= 0,
		SVBoardVersion	= 1
	};

public:
	SVSoftwareTriggerDevice() = default;
	virtual ~SVSoftwareTriggerDevice() = default;

	HRESULT Initialize(bool bInit);

	unsigned long GetTriggerCount();
	unsigned long GetTriggerHandle(unsigned long index);
	BSTR GetTriggerName(unsigned long handle);

	HRESULT TriggerGetParameterCount( unsigned long triggerIndex, unsigned long *pCount );
	HRESULT TriggerGetParameterName( unsigned long triggerIndex, unsigned long Index, BSTR *pName );
	HRESULT TriggerGetParameterValue( unsigned long triggerIndex, unsigned long Index, VARIANT *pValue );
	HRESULT TriggerSetParameterValue( unsigned long triggerIndex, unsigned long Index, VARIANT *pValue );
	
	void dispatchTrigger(unsigned long triggerIndex);

	std::array<TimerInfo, cMaxSoftwareTriggers>& getTimerList() { return m_timerList; }
private:
	void beforeStartTrigger(unsigned long triggerIndex) override;
	void beforeStopTrigger(unsigned long triggerIndex) override;

	long GetTriggerPeriod(unsigned long triggerIndex) const;
	HRESULT SetTriggerPeriod(unsigned long triggerIndex, long period);

	std::array<TimerInfo, cMaxSoftwareTriggers> m_timerList;
};

