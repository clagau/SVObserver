// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : AcquisitionDevice
// * .File Name       : $Workfile:   AcquisitionDevice.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Dec 2014 13:26:40  $
// ******************************************************************************

#include "stdafx.h"
#include "AcquisitionDevice.h"

void CALLBACK AcquisitionDevice::APCProc(ULONG_PTR )
{
}

AcquisitionDevice::AcquisitionDevice(LPCTSTR Name)
: m_DeviceName(Name)
{
}

void AcquisitionDevice::ClearDevice()
{
	if (false == m_isStarted)
	{
		m_pPpqCameraCallback = nullptr;
	}
	m_cameraQueue.clear();
}

HRESULT AcquisitionDevice::Create()
{
	HRESULT result{ S_OK };

	if (m_isCreated)
	{
		result = Destroy();
	}

	if (S_OK == result)
	{
		m_cameraQueue.Create();
		if (m_cameraQueue.IsCreated())
		{
			m_isCreated = true;
		}
		else
		{
			result = E_FAIL;
			Destroy();
		}
	}

	return result;
}


HRESULT AcquisitionDevice::Destroy()
{
	m_isStarted = false;
	m_Thread.Destroy();
	m_pPpqCameraCallback = nullptr;
	m_cameraQueue.clear();
	return S_OK;
}

/*
This method adds the callback data to the callback list.
*/
HRESULT AcquisitionDevice::RegisterCallback(ULONG_PTR pCaller, PpqCameraCallBack pPpqCameraCallback)
{
	if (false == m_isStarted)
	{
		m_pCaller = pCaller;
		m_pPpqCameraCallback = pPpqCameraCallback;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT AcquisitionDevice::UnregisterCallback()
{
	if (false == m_isStarted)
	{
		m_pPpqCameraCallback = nullptr;
		return S_OK;
	}
	return E_FAIL;
}

HRESULT AcquisitionDevice::Start()
{
	HRESULT result = m_Thread.Create(&AcquisitionDevice::APCProc, std::bind(&AcquisitionDevice::Process, this, std::placeholders::_1), m_DeviceName.c_str(), SVAffinityAcq);

	if (S_OK == result)
	{
		m_Thread.SetPriority(THREAD_PRIORITY_TIME_CRITICAL);

		if (m_Thread.IsActive())
		{
			Reset();
			m_isStarted = true;
		}
	}

	return result;
}

HRESULT AcquisitionDevice::Stop()
{
	m_isStarted = false;
	m_Thread.SetPriority(THREAD_PRIORITY_NORMAL);
	m_Thread.Destroy();

	return S_OK;
}

HRESULT AcquisitionDevice::Reset()
{
	m_cameraQueue.clear();
	return S_OK;
}

void AcquisitionDevice::Process( bool& )
{
	long queueSize{ 0L };
	m_cameraQueue.GetSize(queueSize);
	bool done{ 1 >  queueSize};

	while (false == done)
	{
		CameraInfo cameraInfo;
		m_cameraQueue.RemoveHead(&cameraInfo);

		if (nullptr != m_pPpqCameraCallback)
		{
			m_pPpqCameraCallback(m_pCaller, std::move(cameraInfo));
		}
		m_cameraQueue.GetSize(queueSize);
		done = (1 > queueSize);
	}
}

void AcquisitionDevice::Notify(const CameraInfo& rCameraInfo)
{
	m_cameraQueue.AddTail(rCameraInfo);
	m_Thread.Signal(this);
}
