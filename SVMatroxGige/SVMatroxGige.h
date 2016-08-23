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
#include "SVImageLibrary/SVLut.h"
#include "SVImageLibrary/SVLightReference.h"

#include "SVTriggerLibrary/SVInternalTrigger.h"
#include "TriggerHandling/AcquisitionTriggers.h"
#pragma endregion Includes

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
	virtual HRESULT RegisterInternalTriggerCallback( unsigned long p_Handle, const SvTh::TriggerDispatcher& rDispatcher );
	virtual HRESULT UnregisterInternalTriggerCallback( unsigned long p_Handle, const SvTh::TriggerDispatcher& rDispatcher );
	virtual HRESULT UnregisterAllInternalTriggerCallbacks( unsigned long p_Handle );

	virtual void ScanForCameras();

	// convert ordinal to handle
	unsigned long GetDigitizerHandle(unsigned long index) const;

	// Trigger stuff...
	virtual HRESULT TriggerGetCount(unsigned long& p_ulCount);
	
	// convert ordinal to handle
	unsigned long TriggerGetHandle(unsigned long p_ulIndex);

	virtual HRESULT TriggerGetName(unsigned long triggerchannel, BSTR& p_rbstrName);
	virtual HRESULT TriggerRegisterCallback(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher);
	virtual HRESULT TriggerUnregisterCallback(unsigned long triggerchannel, const SvTh::TriggerDispatcher &rDispatcher);
	virtual HRESULT TriggerUnregisterAllCallbacks(unsigned long triggerchannel);
	virtual HRESULT TriggerStart(unsigned long triggerchannel);
	virtual HRESULT TriggerStop(unsigned long triggerchannel);

	virtual HRESULT TriggerGetParameterCount( unsigned long triggerchannel, unsigned long *p_pulCount );
	virtual HRESULT TriggerGetParameterName( unsigned long triggerchannel, unsigned long p_ulIndex, BSTR *p_pbstrName );
	virtual HRESULT TriggerGetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );
	virtual HRESULT TriggerSetParameterValue( unsigned long triggerchannel, unsigned long p_ulIndex, VARIANT *p_pvarValue );

private:
	long m_lRefCount;

	SVMatroxGigeSystemList m_Systems;

	SVInternalTrigger m_triggerMgr;
	SvTh::AcquisitionTriggers m_acquisitionTriggers;

	SVMatroxGigeCameraParamTracker m_trackedCameraParameters;

	HRESULT CreateSystems();
	HRESULT DestroySystems();
	HRESULT DestroySystem(SVMatroxGigeSystem& rSystem);
	HRESULT AddSystem(const SVString& rName, long SystemNumber);

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

	HRESULT IsAcquisitionTriggered(unsigned long triggerchannel, bool& bAcquisitionTriggered) const;
	HRESULT SetAcquisitionTriggered(unsigned long triggerchannel, bool bAcquisitionTriggered);
	void DoAcquisitionTrigger( const SVMatroxGigeDigitizer& p_rCamera, SVMatroxIdentifier HookId );

	// Matrox Hooks
	static SVMatroxIdentifier __stdcall DigitizerStartFrameCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext );
	static SVMatroxIdentifier __stdcall DigitizerEndFrameCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext );
	static SVMatroxIdentifier __stdcall ProcessFrame( SVMatroxIdentifier HookType, SVMatroxIdentifier HookId, void* p_pvContext );
	static SVMatroxIdentifier __stdcall DigitizerCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext );
	static SVMatroxIdentifier __stdcall CameraPresentCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext );
	static SVMatroxIdentifier __stdcall LineEdgeEventCallback( SVMatroxIdentifier HookType, SVMatroxIdentifier EventId, void* p_pvContext );
};

