//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDevice
//* .File Name       : $Workfile:   SVFileAcquisitionDevice.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:26:04  $
//******************************************************************************

#ifndef SVFILEACQUISITIONDEVICE_H
#define SVFILEACQUISITIONDEVICE_H

#include "SVTriggerLibrary/SVInternalTrigger.h"
#include "SVTriggerLibrary/SVTriggerCallbackMap.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVOLibrary/SVQueueObject.h"
#include "SVFileAcquisitionDeviceExports.h"
#include "SVFileCamera.h"

class SVAcquisitionBufferInterface;
struct SVCallbackStruct;

class SVFileAcquisitionDevice
{
private:
	long m_lRefCount;

	SVFileCameraList m_cameras;
	
	SVInternalTrigger m_triggerMgr;
	SVTriggerCallbackMap m_triggerMap;

public:
	SVFileAcquisitionDevice();
	virtual ~SVFileAcquisitionDevice();

	virtual HRESULT Create();
	virtual HRESULT Destroy( bool p_bClose = false );

	virtual HRESULT Open();
	virtual HRESULT Close();

	virtual HRESULT CameraCreate( SVFileCameraStruct *p_pCameraStruct, SVFileCamera *p_pCamera );
	virtual HRESULT CameraDestroy( SVFileCamera *p_pCamera );

	virtual HRESULT CameraGetCount( unsigned long &p_rulCount );
	virtual HRESULT CameraGetName( unsigned long p_ulIndex, BSTR &p_rbstrName );
	virtual HRESULT CameraGetHeight( unsigned long p_ulIndex, unsigned long &p_rulHeight );
	virtual HRESULT CameraGetWidth( unsigned long p_ulIndex, unsigned long &p_rulWidth );
	virtual HRESULT CameraGetFormat( unsigned long p_ulIndex, int &p_riFormat );
	virtual HRESULT CameraGetParameterList( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	virtual HRESULT CameraGetParameterName( unsigned long p_ulIndex, int p_iParameterID, BSTR &p_rbstrName );
	virtual HRESULT CameraGetParameter( unsigned long p_ulIndex, int p_iParameterID, int *p_piParameterIDType, VARIANT *p_pvarValue );
	virtual HRESULT CameraSetParameter( unsigned long p_ulIndex, int p_iParameterID, int p_iParameterIDType, VARIANT *p_pvarValue );
	
	virtual HRESULT CameraBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount );
	virtual HRESULT CameraBufferDestroyAll( unsigned long p_ulIndex );
	virtual HRESULT CameraRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface );
	virtual HRESULT CameraUnregisterBufferInterface( unsigned long p_ulIndex );
	virtual HRESULT CameraStart( unsigned long p_ulIndex );
	virtual HRESULT CameraStop( unsigned long p_ulIndex );
	virtual HRESULT CameraLoadFiles(unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames);

	virtual HRESULT InternalTriggerEnable( unsigned long p_ulIndex );
	virtual HRESULT InternalTrigger( unsigned long p_ulIndex );
	virtual HRESULT RegisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterAllInternalTriggerCallbacks( unsigned long p_ulIndex );

	// Trigger stuff...
	virtual HRESULT TriggerGetCount(unsigned long& p_ulCount);
	
	// convert ordinal to handle
	unsigned long TriggerGetHandle(unsigned long p_ulIndex);

	virtual HRESULT TriggerGetName(unsigned long p_ulHandle, BSTR& p_rbstrName);
	virtual HRESULT TriggerRegisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback);
	virtual HRESULT TriggerUnregisterCallback(unsigned long p_ulHandle, SVCallbackStruct p_Callback);
	virtual HRESULT TriggerUnregisterAllCallbacks(unsigned long p_ulHandle);
	virtual HRESULT TriggerStart(unsigned long p_ulHandle);
	virtual HRESULT TriggerStop(unsigned long p_ulHandle);

	virtual HRESULT TriggerGetParameterCount( unsigned long p_ulHandle, unsigned long *p_pulCount );
	virtual HRESULT TriggerGetParameterName( unsigned long p_ulHandle, unsigned long p_ulIndex, BSTR *p_pbstrName );
	virtual HRESULT TriggerGetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	virtual HRESULT TriggerSetParameterValue( unsigned long p_ulHandle, unsigned long p_ulIndex, VARIANT *p_pvarValue );

private:
	bool IsValidDigitizer(unsigned long p_ulHandle) const;
	SVFileCamera& GetDigitizer(unsigned long p_ulHandle);
	const SVFileCamera& GetDigitizer(unsigned long p_ulHandle) const;

	HRESULT CameraProcessStartFrame( unsigned long p_ulIndex );
	HRESULT CameraProcessEndFrame( unsigned long p_ulIndex );

	HRESULT FireOneShot( unsigned long p_ulIndex );
	HRESULT DispatchTriggerCallback( unsigned long p_ulIndex );

	void DoAcquisitionTrigger(SVFileCamera& rCamera);
	HRESULT IsAcquisitionTriggered(unsigned long p_ulHandle, bool& bAcquisitionTriggered) const;
	HRESULT SetAcquisitionTriggered(unsigned long p_ulHandle, bool bAcquisitionTriggered);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVFileAcquisitionDevice\SVFileAcquisitionDevice.h_v  $
 * 
 *    Rev 1.0   22 Apr 2013 09:26:04   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   21 Jan 2013 09:47:26   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  809
 * SCR Title:  Non I/O SVIM
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added methods for trigger interface
 * Revised to support Acquisition Triggering
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   29 Jul 2009 15:00:02   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Sep 2008 07:58:06   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  634
 * SCR Title:  Implement a File Acquistion Device
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
