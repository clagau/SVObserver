// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : TriggerDevice
// * .File Name       : $Workfile:   TriggerDevice.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 08:47:30  $
// ******************************************************************************

#pragma once

#pragma region Includes
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "TriggerInformation/SVTriggerInfoStruct.h"
#include "SVContainerLibrary/SVRingBuffer.h"
#pragma endregion Includes

typedef std::function<void(SvTi::SVTriggerInfoStruct&&)> PpqTriggerCallBack;

class TriggerDevice  
{
public:
	static void CALLBACK APCProc(ULONG_PTR pParam);

	explicit TriggerDevice(LPCTSTR deviceName);
	virtual ~TriggerDevice() = default;

	long getTriggerCount() { return m_triggerCount; }
	void resetTriggerCount() { m_triggerCount = 0L; }

	virtual bool IsStarted() const { return m_isStarted;  }

	virtual void ClearDevice();
	virtual HRESULT Destroy();

	virtual HRESULT RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback);
	virtual HRESULT UnregisterCallback();

	virtual HRESULT Start();
	virtual HRESULT Stop();
	virtual HRESULT Reset();

	LPCTSTR GetDeviceName() const {	return m_DeviceName.c_str(); }
	void SetDeviceName(LPCTSTR pName) { m_DeviceName = pName; }

	virtual void Notify(const SvTi::SVTriggerInfoStruct& rTriggerInfo);

protected:
	typedef SVRingBuffer<SvTi::SVTriggerInfoStruct> TriggerQueue;

	void Process(bool& rWaitForEvents);

	virtual void processAcquisitionTriggers(const SvTi::SVTriggerInfoStruct& ) const {}

private:
	TriggerDevice() = default;

	std::string m_DeviceName;
	bool m_isStarted {false};
	long m_triggerCount{0L};

	SVAsyncProcedure m_Thread;

	TriggerQueue m_triggerQueue{ 10 };

	PpqTriggerCallBack m_pPpqTriggerCallback{ nullptr };
};
