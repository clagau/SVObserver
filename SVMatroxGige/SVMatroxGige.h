//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGige
//* .File Name       : $Workfile:   SVMatroxGige.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   12 Jun 2013 15:13:28  $
//******************************************************************************

#ifndef SVMATROXGIGE_H
#define SVMATROXGIGE_H

#include "SVImageLibrary/SVLut.h"
#include "SVImageLibrary/SVLightReference.h"
#include "SVOMFCLibrary/SVCallbackStruct.h"
#include "SVTriggerLibrary/SVInternalTrigger.h"
#include "SVTriggerLibrary/SVTriggerCallbackMap.h"
#include "SVBufferQueueFreeListStrategy.h"

#include "SVMatroxGigeExports.h"
#include "SVMatroxGigeDigitizer.h"
#include "SVMatroxGigeSystem.h"
#include "SVMatroxGigeSystemList.h" 
#include "SVMatroxGigeCameraParamTracker.h"

class SVAcquisitionBufferInterface;

class SVMatroxGige
{
public:
	SVMatroxGige();
	virtual ~SVMatroxGige();

	virtual HRESULT Open();
	virtual HRESULT Close();

	virtual HRESULT Create();
	virtual HRESULT Destroy( bool p_bClose = false );
	
//	virtual HRESULT BufferIsLocked( unsigned long p_ulIndex, bool& p_rbIsLocked );
//	virtual HRESULT BufferLock( unsigned long p_ulIndex );
//	virtual HRESULT BufferUnlock( unsigned long p_ulIndex );

	virtual HRESULT CameraGetCount( unsigned long& p_rulCount );
	virtual HRESULT CameraGetName( unsigned long p_Handle, BSTR& p_rbstrName );
	virtual HRESULT CameraGetHeight( unsigned long p_Handle, unsigned long& p_rulHeight );
	virtual HRESULT CameraGetWidth( unsigned long p_Handle, unsigned long& p_rulWidth );
	virtual HRESULT CameraGetFormat( unsigned long p_Handle, int& p_riFormat );
	virtual HRESULT CameraGetParameterList( unsigned long p_Handle, VARIANT* p_pvarValue );
	virtual HRESULT CameraGetParameterName( unsigned long p_Handle, int p_iParameterID, BSTR* p_pBstrName );
	virtual HRESULT CameraGetParameter( unsigned long p_Handle, int p_iParameterID, int* p_piParameterIDType, VARIANT* p_pvarValue );
	virtual HRESULT CameraSetParameter( unsigned long p_Handle, int p_iParameterID, int p_iParameterIDType, VARIANT* p_pvarValue );
	virtual HRESULT CameraBufferCreateAll( unsigned long p_Handle, unsigned long p_ulCount );
	virtual HRESULT CameraBufferUnlockAll( unsigned long p_Handle );
	virtual HRESULT CameraBufferDestroyAll( unsigned long p_Handle );
	virtual HRESULT CameraRegisterBufferInterface( unsigned long p_Handle, SVAcquisitionBufferInterface* p_pInterface );
	virtual HRESULT CameraUnregisterBufferInterface( unsigned long p_Handle );
	virtual HRESULT CameraStart( unsigned long p_Handle );
	virtual HRESULT CameraStop( unsigned long p_Handle );

	virtual HRESULT CameraLoadFiles(unsigned long p_Handle, SAFEARRAY* p_psaFileNames);

	virtual HRESULT InternalTriggerEnable( unsigned long p_Handle );
	virtual HRESULT InternalTrigger( unsigned long p_Handle );
	virtual HRESULT RegisterInternalTriggerCallback( unsigned long p_Handle, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterInternalTriggerCallback( unsigned long p_Handle, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterAllInternalTriggerCallbacks( unsigned long p_Handle );

	virtual void ScanForCameras();

	// convert ordinal to handle
	unsigned long GetDigitizerHandle(unsigned long index) const;

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
	long m_lRefCount;

	SVMatroxGigeSystemList m_Systems;

	SVInternalTrigger m_triggerMgr;
	SVTriggerCallbackMap m_triggerMap;

	SVMatroxGigeCameraParamTracker m_trackedCameraParameters;

	HRESULT CreateSystems();
	HRESULT DestroySystems();
	HRESULT DestroySystem(SVMatroxGigeSystem& rSystem);
	HRESULT AddSystem(const SVMatroxString& rName, long SystemNumber);

	HRESULT CreateDigitizers(SVMatroxGigeSystem& rSystem);
	HRESULT CreateDigitizer(SVMatroxGigeSystem& rSystem, long digitizerIndex);
	HRESULT AllocDigitizer(SVMatroxGigeSystem& system, long digitizerIndex, SVMatroxGigeDigitizer& p_rCamera);

	HRESULT DestroyDigitizers(SVMatroxGigeSystem& rSystem);
	HRESULT DestroyDigitizer(SVMatroxGigeDigitizer& digitizer);

	HRESULT DestroyBuffers();
	
	HRESULT StartDigitizer(unsigned long p_Handle, SVMatroxGigeDigitizer& p_rCamera);
	HRESULT StopDigitizer(SVMatroxGigeDigitizer& p_rCamera);

	HRESULT ProcessStartFrame( SVMatroxGigeDigitizer& p_rCamera );
	HRESULT ProcessEndFrame( SVMatroxGigeDigitizer& p_rCamera, SVMatroxIdentifier p_SrcBufferID );
	HRESULT CameraStartFrame( SVMatroxGigeDigitizer& p_rCamera );
	HRESULT CameraEndFrame( SVMatroxGigeDigitizer& p_rCamera, SVMatroxIdentifier p_SrcBufferID );
	
	HRESULT FireOneShot( unsigned long p_Handle );
	HRESULT DispatchTriggerCallback( unsigned long p_Handle );

	bool IsValidDigitizerHandle(unsigned long p_Handle) const;
	bool IsValidDigitizer(unsigned long p_Handle) const;

	HRESULT ReadCameraSerialNumber(SVMatroxGigeDigitizer& p_rCamera);
	HRESULT ReadCameraFormat(SVMatroxGigeDigitizer& p_rCamera);
	HRESULT ReadCameraIPAddress(SVMatroxGigeDigitizer& p_rCamera);

	bool IsCameraActive(unsigned long p_Handle);

	HRESULT GetNextAvailableProcBuffer(SVMatroxGigeDigitizer& p_rCamera);

	HRESULT RegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera);
	HRESULT UnRegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera);

	HRESULT EnableGigeEvents(const SVMatroxGigeDigitizer& p_rCamera);
	HRESULT DisableGigeEvents(const SVMatroxGigeDigitizer& p_rCamera);

	HRESULT SetGrabMode(const SVMatroxGigeDigitizer& p_rCamera);
	HRESULT EnableTriggering(const SVMatroxGigeDigitizer& p_rCamera);

	const SVMatroxGigeDigitizer& GetDigitizer(unsigned long p_Handle) const;
	SVMatroxGigeDigitizer& GetDigitizer(unsigned long p_Handle);

	void HandleConnect(SVMatroxGigeSystem& p_rSystem, long deviceNumber);
	void HandleDisconnect(SVMatroxGigeSystem& p_rSystem, long deviceNumber);

	void TrackCameraParams(const SVMatroxGigeDigitizer& digitizer);
	void LockMainCameraParamList(const SVMatroxGigeDigitizer& digitizer, bool bLock=true);
	void SetCameraParam(const SVMatroxGigeDigitizer& digitizer, int p_iParameterID, int p_iParameterTypeID, VARIANT* p_pvarValue);

	HRESULT FindCamera(const SVMatroxGigeSystem& p_rSystem, long p_DeviceNumber, unsigned long& p_rHandle);

	HRESULT IsAcquisitionTriggered(unsigned long p_ulHandle, bool& bAcquisitionTriggered) const;
	HRESULT SetAcquisitionTriggered(unsigned long p_ulHandle, bool bAcquisitionTriggered);
	void DoAcquisitionTrigger(const SVMatroxGigeDigitizer& p_rCamera, long HookId);

	// Matrox Hooks
	static long __stdcall DigitizerStartFrameCallback(long HookType, SVMatroxIdentifier EventId, void* p_pvContext);
	static long __stdcall DigitizerEndFrameCallback(long HookType, SVMatroxIdentifier EventId, void* p_pvContext);
	static long __stdcall ProcessFrame(long HookType, long HookId, void* p_pvContext);
	static long __stdcall DigitizerCallback(long HookType, SVMatroxIdentifier EventId, void* p_pvContext);
	static long __stdcall CameraPresentCallback(long HookType, long EventId, void* p_pvContext);
	static long __stdcall LineEdgeEventCallback(long HookType, SVMatroxIdentifier EventId, void* p_pvContext);
};

#endif

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_src\SVMatroxGige\SVMatroxGige.h_v  $
 * 
 *    Rev 1.1   12 Jun 2013 15:13:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Merged with svo_src label SVO 6.10 Beta 018.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   22 May 2013 10:17:16   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  833
 * SCR Title:  Change GigE Camera Manager to be based on IP Address
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   added new method ReadCameraIPAddress
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   22 Apr 2013 14:38:30   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   21 Jan 2013 09:59:14   sjones
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
 *    Rev 1.2   29 Jun 2012 16:43:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  772
 * SCR Title:  Implement GigE Camera Reconnect Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to add the needed functionality to track parameter information and reset the camera to the desired parameters on reconnect.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   30 Jul 2009 07:50:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to use the new buffer management and buffer transfer methodologies.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   16 Jun 2009 17:17:22   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  650
 * SCR Title:  Integrate Gigabit ethernet cameras
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial Checkin
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
