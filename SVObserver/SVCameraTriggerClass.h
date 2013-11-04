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

#ifndef SVCAMERATRIGGERCLASS_H
#define SVCAMERATRIGGERCLASS_H

#include "SVMaterialsLibrary/SVMaterials.h"
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

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVObserver\SVCameraTriggerClass.h_v  $
 * 
 *    Rev 1.0   23 Apr 2013 09:50:26   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   21 Jan 2013 10:59:40   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
