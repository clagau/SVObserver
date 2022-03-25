// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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
#include "ObjectIDParameters.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVTriggerInfoStruct.h"
#include "SVContainerLibrary/SVRingBuffer.h"
#pragma endregion Includes

namespace SvTrig
{

typedef std::function<void(SvTrig::SVTriggerInfoStruct&&)> PpqTriggerCallBack;

class TriggerDevice
{
public:
	static void CALLBACK APCProc(ULONG_PTR pParam);

	explicit TriggerDevice(LPCTSTR deviceName);
	virtual ~TriggerDevice() = default;

	long getTriggerCount() { return m_triggerCount; }
	void setTriggerCount(long triggerCount = 0L) { m_triggerCount = triggerCount; }

	const ObjectIDParameters& getObjectIDParameters() const { return m_objectIDParams; }
	void setObjectIDParameters(const ObjectIDParameters& rObjectIDParams) { m_objectIDParams = rObjectIDParams; }

	virtual bool IsStarted() const { return m_isStarted; }

	virtual void ClearDevice();
	virtual HRESULT Destroy();

	virtual HRESULT RegisterCallback(PpqTriggerCallBack pPpqTriggerCallback);
	virtual HRESULT UnregisterCallback();

	virtual HRESULT Start();
	virtual HRESULT Stop();
	virtual HRESULT Reset();

	LPCTSTR GetDeviceName() const { return m_DeviceName.c_str(); }
	void SetDeviceName(LPCTSTR pName) { m_DeviceName = pName; }

	virtual void Notify(const SVTriggerInfoStruct& rTriggerInfo);

protected:
	typedef SVRingBuffer<SVTriggerInfoStruct, SVElementClear> TriggerQueue;

	void Process(bool& rWaitForEvents);

	virtual void preProcessTriggers(SVTriggerInfoStruct&) {}
	virtual void postProcessTriggers(DWORD, bool, LPCTSTR) {}

private:
	TriggerDevice() = default;

	std::string m_DeviceName;
	bool m_isStarted {false};
	long m_triggerCount {0L};
	ObjectIDParameters m_objectIDParams {};

	SvSyl::SVAsyncProcedure m_Thread;

	TriggerQueue m_triggerQueue {10};

	PpqTriggerCallBack m_pPpqTriggerCallback {nullptr};
};
}//namespace SvTrig