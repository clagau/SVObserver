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
	SVMatroxGige() = default;
	~SVMatroxGige();

	HRESULT Create();
	HRESULT Destroy( bool close = false );
	
	unsigned long GetDigitizerHandle(long index) const;
	unsigned long CameraGetCount() const;
	_variant_t CameraGetName(unsigned long digitizerHandle) const;
	unsigned long CameraGetWidth(unsigned long digitizerHandle) const;
	unsigned long CameraGetHeight(unsigned long digitizerHandle) const;
	int CameraGetFormat(unsigned long digitizerHandle) const;
	HRESULT CameraBufferCreateAll(unsigned long digitizerHandle);
	HRESULT CameraBufferDestroyAll(unsigned long digitizerHandle);
	HRESULT CameraRegisterBufferInterface(unsigned long digitizerHandle, SVAcquisitionBufferInterface* pInterface);
	HRESULT CameraUnregisterBufferInterface(unsigned long digitizerHandle);
	HRESULT CameraStart(unsigned long digitizerHandle);
	HRESULT CameraStop(unsigned long digitizerHandle);
	_variant_t CameraGetParameterName(unsigned long digitizerHandle, int parameterID) const;
	_variant_t CameraGetParameter(unsigned long digitizerHandle, int parameterID) const;
	HRESULT CameraSetParameter(unsigned long digitizerHandle, int parameterID, const _variant_t& rValue);
	_variant_t CameraGetParameterList(unsigned long digitizerHandle) const;

	HRESULT InternalTrigger(unsigned long digitizerHandle);

	void ScanForCameras();

	void ProcessStartFrame(SVMatroxGigeDigitizer* pCamera);
	void ProcessEndFrame(SVMatroxGigeDigitizer* pCamera, __int64 HookID);

private:
	long m_lRefCount{ 0L };
	bool m_DllMatroxStartup {false};

	SVMatroxGigeSystemList m_Systems;

	SVMatroxGigeCameraParamTracker m_trackedCameraParameters;

	HRESULT CreateSystems();
	HRESULT DestroySystems();
	HRESULT DestroySystem(SVMatroxGigeSystem& rSystem);
	HRESULT AddSystem(const std::string& rName, long SystemNumber);

	HRESULT CreateDigitizers(SVMatroxGigeSystem& rSystem);
	HRESULT CreateDigitizer(SVMatroxGigeSystem& rSystem, long digitizerIndex);
	HRESULT AllocDigitizer(SVMatroxGigeSystem& system, long digitizerIndex, SVMatroxGigeDigitizer& rCamera);

	HRESULT DestroyDigitizers(SVMatroxGigeSystem& rSystem);
	HRESULT DestroyDigitizer(SVMatroxGigeDigitizer& digitizer);

	HRESULT StartDigitizer(SVMatroxGigeDigitizer& rCamera, const _variant_t& rTriggerSource);
	HRESULT StopDigitizer(SVMatroxGigeDigitizer& rCamera);

	bool IsValidDigitizerHandle(unsigned long digitizerHandle) const;
	bool IsValidDigitizer(unsigned long digitizerHandle) const;

	HRESULT ReadCameraSerialNumber(SVMatroxGigeDigitizer& rCamera);
	HRESULT ReadCameraIPAddress(SVMatroxGigeDigitizer& rCamera);
	HRESULT ReadCameraFormat(SVMatroxGigeDigitizer& rCamera);

	bool IsCameraActive(unsigned long digitizerHandle);

	HRESULT RegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera);
	HRESULT UnRegisterMatroxDigitizerHooks(const SVMatroxGigeDigitizer& p_rCamera);

	HRESULT SetGrabMode(const SVMatroxGigeDigitizer& p_rCamera);
	HRESULT EnableTriggering(const SVMatroxGigeDigitizer& p_rCamera, const _variant_t& rTriggerSource);

	const SVMatroxGigeDigitizer& GetDigitizer(unsigned long digitizerHandle) const;
	SVMatroxGigeDigitizer& GetDigitizer(unsigned long digitizerHandle);

	void HandleConnect(SVMatroxGigeSystem& p_rSystem, long deviceNumber);
	void HandleDisconnect(SVMatroxGigeSystem& p_rSystem, long deviceNumber);

	void TrackCameraParams(const SVMatroxGigeDigitizer& digitizer);
	void LockMainCameraParamList(const SVMatroxGigeDigitizer& digitizer, bool bLock=true);
	void SetCameraParam(const SVMatroxGigeDigitizer& digitizer, int parameterID, _variant_t rValue);

	HRESULT FindCamera(const SVMatroxGigeSystem& rSystem, long deviceNumber, unsigned long& rHandle);

	// Matrox Hooks
	static __int64 __stdcall DigitizerStartFrameCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall DigitizerEndFrameCallback( __int64 HookType, __int64 EventId, void* pContext );
	static __int64 __stdcall CameraPresentCallback( __int64 HookType, __int64 EventId, void* pContext );
};

extern SVMatroxGige g_matroxAcqDevice;
