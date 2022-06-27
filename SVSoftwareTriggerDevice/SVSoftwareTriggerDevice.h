//******************************************************************************
//* COPYRIGHT (c) 2008 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "Triggering/IODeviceBase.h"
#pragma endregion Includes

constexpr unsigned long cMaxSoftwareTriggers = 4;

class SVSoftwareTriggerDevice : public SvTrig::IODeviceBase
{
public:
	struct TimerInfo
	{
		std::thread m_thread;
		HANDLE m_shutdown {nullptr};
		std::atomic_bool m_timerOn {false};
		std::atomic_ulong m_triggerIndex {0UL};
		std::atomic_uint16_t m_period {0};
		std::atomic_bool m_newSetting {false};
		std::atomic_bool m_pause {false};
	};

	SVSoftwareTriggerDevice() = default;
	virtual ~SVSoftwareTriggerDevice() = default;

	HRESULT Initialize();
	HRESULT Destroy();

	unsigned long GetTriggerCount() const;
	unsigned long GetTriggerHandle(unsigned long index) const;
	_variant_t GetTriggerName(unsigned long handle) const;

	unsigned long TriggerGetParameterCount( unsigned long triggerIndex) const;
	_variant_t TriggerGetParameterName( unsigned long triggerIndex, unsigned long index) const;
	_variant_t TriggerGetParameterValue( unsigned long triggerIndex, unsigned long index) const;
	HRESULT TriggerSetParameterValue( unsigned long triggerIndex, unsigned long index, const _variant_t& rValue);

	void dispatchTrigger(unsigned long triggerIndex, double timestamp);
private:
	void beforeStartTrigger(unsigned long triggerIndex) override;
	void beforeStopTrigger(unsigned long triggerIndex) override;

	long GetTriggerPeriod(unsigned long triggerIndex) const;
	HRESULT SetTriggerPeriod(unsigned long triggerIndex, long period);
	void SetTriggerPause(unsigned long triggerIndex, bool pause);

	std::array<TimerInfo, cMaxSoftwareTriggers> m_timerList;
};

