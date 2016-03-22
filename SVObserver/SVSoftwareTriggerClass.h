//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVSoftwareTriggerClass
//* .File Name       : $Workfile:   SVSoftwareTriggerClass.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   23 Apr 2013 15:08:46  $
//******************************************************************************

#pragma once

#include "SVTriggerClass.h"
#include "SVAcquisitionInitiator.h"

class SVSoftwareTriggerClass : public SVTriggerClass
{
private:
	SVAcquisitionInitiator m_acquisitionInitiator;

public:
	SVSoftwareTriggerClass(LPCTSTR deviceName);
	virtual ~SVSoftwareTriggerClass();

	virtual HRESULT RegisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller );
	virtual HRESULT UnregisterCallback( SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller );
	
	static HRESULT CALLBACK TriggerCallback( void *p_pvOwner, void *p_pvData );
	static HRESULT CALLBACK TriggerCompleteCallback( void *p_pvOwner, void *p_pvData );

	virtual void RegisterAcquistionInitiator( SVAcquisitionInitiator& rFunc); 

	virtual HRESULT EnableInternalTrigger();
	virtual HRESULT FireAcquisitionTrigger();
};


