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
#include "SVMatroxGigeSystemList.h" 
#include "SVMatroxGigeCameraParamTracker.h"
#pragma endregion Includes

class SVAcquisitionBufferInterface;

class SVMatroxGige
{
public:
	SVMatroxGige();
	~SVMatroxGige();


	HRESULT Create();
	HRESULT Destroy( bool p_bClose = false );
	
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

	HRESULT InternalTriggerEnable(unsigned long p_Handle);
	HRESULT InternalTrigger(unsigned long p_Handle);

	void ScanForCameras();

	// convert ordinal to handle
	unsigned long GetDigitizerHandle(unsigned long index) const;

	void ProcessStartFrame(SVMatroxGigeDigitizer* pCamera);
	void ProcessEndFrame(SVMatroxGigeDigitizer* pCamera, __int64 HookID);

private:
	long m_lRefCount = 0;

	SVMatroxGigeSystemList m_Systems;

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

	HRESULT FireOneShot( unsigned long p_Handle );

	bool IsValidDigitizerHandle(unsigned long p_Handle) const;
	bool IsValidDigitizer(unsigned long p_Handle) const;

	HRESULT ReadCameraSerialNumber(SVMatroxGigeDigitizer& p_rCamera);
	HRESULT ReadCameraFormat(SVMatroxGigeDigitizer& p_rCamera);
	HRESULT ReadCameraIPAddress(SVMatroxGigeDigitizer& p_rCamera);

	bool IsCameraActive(unsigned long p_Handle);

	HRESULT RegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera);
	HRESULT UnRegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera);

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

	// Matrox Hooks
	static __int64 __stdcall DigitizerStartFrameCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall DigitizerEndFrameCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall CameraPresentCallback( __int64 HookType, __int64 EventId, void* pContext );
};

extern SVMatroxGige g_matroxAcqDevice;
