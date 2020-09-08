// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : AcquisitionDevice
// * .File Name       : $Workfile:   AcquisitionDevice.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 08:47:28  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>

#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "CameraInfo.h"
#include "SVQueueObject.h"
#pragma endregion Includes

typedef std::function<void(ULONG_PTR, CameraInfo&&)> PpqCameraCallBack;

class AcquisitionDevice
{
public:
	static void CALLBACK APCProc( ULONG_PTR dwParam );

	explicit AcquisitionDevice(LPCTSTR deviceName);
	virtual ~AcquisitionDevice() = default;

	virtual bool IsValid() const { return true; }
	virtual bool IsStarted() const { return m_isStarted; }
	virtual bool IsCreated() const { return m_isCreated; }

	virtual void ClearDevice();
	virtual HRESULT Create();
	virtual HRESULT Destroy();

	virtual HRESULT RegisterCallback(ULONG_PTR pCaller, PpqCameraCallBack pPpqCameraCallback);
	virtual HRESULT UnregisterCallback();

	virtual HRESULT Start();
	virtual HRESULT Stop();
	virtual HRESULT Reset();


	LPCTSTR GetDeviceName() const { return m_DeviceName.c_str(); }
	void SetDeviceName( LPCTSTR pName ) { m_DeviceName = pName; }

protected:
	typedef SVTQueueObject<CameraInfo> CameraQueue;

	/* AcquisitionDevice Callback Functions */

	void Process(bool& rWaitForEvents);

	/* Device Callback Functions */

	virtual void Notify(const CameraInfo&);

private:
	AcquisitionDevice() = default;

	//This attribute holds the device name.
	std::string m_DeviceName;

	bool m_isStarted{false};
	bool m_isCreated{ false };

	ULONG_PTR m_pCaller{ 0UL }; 

	SVAsyncProcedure m_Thread;

	CameraQueue m_cameraQueue;

	PpqCameraCallBack m_pPpqCameraCallback;
};

