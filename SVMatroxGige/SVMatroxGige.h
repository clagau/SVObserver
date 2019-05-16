//******************************************************************************
//* COPYRIGHT (c) 2009 by Seidenader Vision, Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVMatroxGige
//* .File Name       : $Workfile:   SVMatroxGige.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.2  $
//* .Check In Date   : $Date:   01 Oct 2013 10:49:00  $
//******************************************************************************
#pragma once

#pragma region Includes
#include "TriggerInformation/SVInternalTrigger.h"
#include "TriggerHandling/TriggerDispatcherCollection.h"
#include "SVMatroxGigeSystemList.h" 
#include "SVMatroxGigeCameraParamTracker.h"
#pragma endregion Includes

class SVAcquisitionBufferInterface;

class SVMatroxGige
{
public:
	SVMatroxGige();
	~SVMatroxGige();

	HRESULT Open();
	HRESULT Close();

	HRESULT Create();
	HRESULT Destroy( bool p_bClose = false );
	
//	HRESULT BufferIsLocked( unsigned long p_ulIndex, bool& p_rbIsLocked );
//	HRESULT BufferLock( unsigned long p_ulIndex );
//	HRESULT BufferUnlock( unsigned long p_ulIndex );

	HRESULT CameraGetCount( unsigned long& p_rulCount );
	HRESULT CameraGetName( unsigned long p_Handle, BSTR& p_rbstrName );
	HRESULT CameraGetHeight( unsigned long p_Handle, unsigned long& p_rulHeight );
	HRESULT CameraGetWidth( unsigned long p_Handle, unsigned long& p_rulWidth );
	HRESULT CameraGetFormat( unsigned long p_Handle, int& p_riFormat );
	HRESULT CameraGetParameterList( unsigned long p_Handle, VARIANT* p_pvarValue );
	HRESULT CameraGetParameterName( unsigned long p_Handle, int p_iParameterID, BSTR* p_pBstrName );
	HRESULT CameraGetParameter( unsigned long p_Handle, int p_iParameterID, int* p_piParameterIDType, VARIANT* p_pvarValue );
	HRESULT CameraSetParameter( unsigned long p_Handle, int p_iParameterID, int p_iParameterIDType, VARIANT* p_pvarValue );
	HRESULT CameraBufferCreateAll( unsigned long p_Handle, unsigned long p_ulCount );
	HRESULT CameraBufferUnlockAll( unsigned long p_Handle );
	HRESULT CameraBufferDestroyAll( unsigned long p_Handle );
	HRESULT CameraRegisterBufferInterface( unsigned long p_Handle, SVAcquisitionBufferInterface* p_pInterface );
	HRESULT CameraUnregisterBufferInterface( unsigned long p_Handle );
	HRESULT CameraStart( unsigned long p_Handle );
	HRESULT CameraStop( unsigned long p_Handle );

	HRESULT CameraLoadFiles(unsigned long p_Handle, SAFEARRAY* p_psaFileNames);

	HRESULT InternalTriggerEnable( unsigned long p_Handle );
	HRESULT InternalTrigger( unsigned long p_Handle );
	HRESULT RegisterInternalTriggerCallback( unsigned long p_Handle, const SvTh::TriggerDispatcher& rDispatcher );
	HRESULT UnregisterInternalTriggerCallback( unsigned long p_Handle, const SvTh::TriggerDispatcher& rDispatcher );
	HRESULT UnregisterAllInternalTriggerCallbacks( unsigned long p_Handle );

	void ScanForCameras();

	// convert ordinal to handle
	unsigned long GetDigitizerHandle(unsigned long index) const;

	// Trigger stuff...
	HRESULT TriggerGetCount(unsigned long& p_ulCount);
	
	// convert ordinal to handle
	unsigned long TriggerGetHandle(unsigned long p_ulIndex);

	HRESULT TriggerGetName(unsigned long triggerchannel, BSTR& p_rbstrName);
	HRESULT TriggerRegisterCallback(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher);
	HRESULT TriggerUnregisterCallback(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher);
	HRESULT TriggerUnregisterAllCallbacks(unsigned long triggerchannel);
	HRESULT TriggerStart(unsigned long triggerchannel);
	HRESULT TriggerStop(unsigned long triggerchannel);

	HRESULT TriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount );
	HRESULT TriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName );
	HRESULT TriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT TriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );

private:
	long m_lRefCount;

	SVMatroxGigeSystemList m_Systems;

	SvTi::SVInternalTrigger m_triggerMgr;
	SvTh::TriggerDispatcherCollection m_AcquisitionDispatchers;

	SVMatroxGigeCameraParamTracker m_trackedCameraParameters;

	HRESULT CreateSystems();
	HRESULT DestroySystems();
	HRESULT DestroySystem(SVMatroxGigeSystem& rSystem);
	HRESULT AddSystem(const std::string& rName, long SystemNumber);

	HRESULT CreateDigitizers(SVMatroxGigeSystem& rSystem);
	HRESULT CreateDigitizer(SVMatroxGigeSystem& rSystem, long digitizerIndex);
	HRESULT AllocDigitizer(SVMatroxGigeSystem& system, long digitizerIndex, SVMatroxGigeDigitizer& p_rCamera);

	HRESULT DestroyDigitizers(SVMatroxGigeSystem& rSystem);
	HRESULT DestroyDigitizer(SVMatroxGigeDigitizer& digitizer);

	HRESULT DestroyBuffers();
	
	HRESULT StartDigitizer(unsigned long p_Handle, SVMatroxGigeDigitizer& p_rCamera);
	HRESULT StopDigitizer(SVMatroxGigeDigitizer& p_rCamera);

	HRESULT ProcessStartFrame( SVMatroxGigeDigitizer& p_rCamera );
	HRESULT ProcessEndFrame( SVMatroxGigeDigitizer& p_rCamera, __int64 p_SrcBufferID );
	HRESULT CameraStartFrame( SVMatroxGigeDigitizer& p_rCamera );
	HRESULT CameraEndFrame( SVMatroxGigeDigitizer& p_rCamera, __int64 p_SrcBufferID );
	
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

	HRESULT IsAcquisitionTriggered(unsigned long triggerchannel, bool& bAcquisitionTriggered) const;
	HRESULT SetAcquisitionTriggered(unsigned long triggerchannel, bool bAcquisitionTriggered);
	void DoAcquisitionTrigger( const SVMatroxGigeDigitizer& p_rCamera, __int64 HookId );

	// Matrox Hooks
	static __int64 __stdcall DigitizerStartFrameCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall DigitizerEndFrameCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall ProcessFrame( __int64 HookType, __int64 HookId, void* pContext );
	static __int64 __stdcall DigitizerCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall CameraPresentCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall LineEdgeEventCallback( __int64 HookType, __int64 EventId, void* pContext );
};

