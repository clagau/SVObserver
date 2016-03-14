//******************************************************************************
//* COPYRIGHT (c) 2012 by Seidenader Vision Inc.
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVCameraTriggerClass
//* .File Name       : $Workfile:   SVCameraTriggerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 09:50:26  $
//******************************************************************************

#pragma once

#include "SVSoftwareTriggerClass.h"

class SVCameraTriggerClass : public SVTriggerClass
{
private:
	SVSoftwareTriggerClass* m_pSoftwareTrigger;

public:
	SVCameraTriggerClass(LPCTSTR deviceName);
	virtual ~SVCameraTriggerClass();

	void SetSoftwareTriggerDevice(SVSoftwareTriggerClass* pSoftwareTrigger);

	virtual HRESULT Destroy();

	virtual HRESULT Start();
	virtual HRESULT Stop();

	// for Software Trigger Emulation
	void RegisterAcquistionInitiator(SVAcquisitionInitiator& rFunc);
	virtual HRESULT EnableInternalTrigger();
	HRESULT FireAcquisitionTrigger();

	virtual HRESULT RegisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller);
	virtual HRESULT UnregisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller);
	
	static HRESULT CALLBACK SoftwareTriggerCompleteCallback(void *p_pvOwner, void *p_pvData, void * p_pvResponse);
	static HRESULT CALLBACK TriggerCallback(void *p_pvOwner, void *p_pvData);
};

