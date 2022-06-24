// ******************************************************************************
// * COPYRIGHT (c) 2002 by Körber Pharma Inspection GmbH. All Rights Reserved
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

void CALLBACK AcquisitionDevice::APCProc(ULONG_PTR pData)
{
	AcquisitionDevice* pAcquisitionDevice = reinterpret_cast<AcquisitionDevice*> (pData);
	if (nullptr != pAcquisitionDevice)
	{
		long queueSize {0L};
		pAcquisitionDevice->m_cameraQueue.GetSize(queueSize);
		bool done {1 > queueSize};

		while (false == done)
		{
			CameraInfo cameraInfo;
			pAcquisitionDevice->m_cameraQueue.RemoveHead(&cameraInfo);

			for (const auto& rCallback : pAcquisitionDevice->m_PpqCameraCallbackList)
			{
				rCallback.second(pAcquisitionDevice->m_pCaller, cameraInfo);
			}
			pAcquisitionDevice->m_cameraQueue.GetSize(queueSize);
			done = (1 > queueSize);
		}
	}
}

AcquisitionDevice::AcquisitionDevice(LPCTSTR Name)
: m_DeviceName(Name)
{
}

void AcquisitionDevice::ClearDevice()
{
	if (false == m_isStarted)
	{
		m_PpqCameraCallbackList.clear();
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
		m_isCreated = true;
	}

	return result;
}


HRESULT AcquisitionDevice::Destroy()
{
	m_isStarted = false;
	m_Thread.Destroy();
	m_PpqCameraCallbackList.clear();
	m_cameraQueue.clear();
	return S_OK;
}

/*
This method adds the callback data to the callback list.
*/
HRESULT AcquisitionDevice::RegisterCallback(ULONG_PTR pCaller, ULONG_PTR pPPQ, PpqCameraCallBack pPpqCameraCallback)
{
	if (false == m_isStarted)
	{
		if (0UL == m_pCaller)
		{
			m_pCaller = pCaller;
		}
		else if (m_pCaller != pCaller)
		{
			return E_FAIL;
		}

		m_PpqCameraCallbackList.push_back({ pPPQ, pPpqCameraCallback });
		return S_OK;
	}
	return E_FAIL;
}

HRESULT AcquisitionDevice::UnregisterCallback(ULONG_PTR pPPQ)
{
	if (false == m_isStarted)
	{
		if (0UL != pPPQ)
		{
			auto iter = std::find_if(m_PpqCameraCallbackList.begin(), m_PpqCameraCallbackList.end(), [&pPPQ](const auto& rCallback) { return rCallback.first == pPPQ; });
			if (m_PpqCameraCallbackList.end() != iter)
			{
				m_PpqCameraCallbackList.erase(iter);
			}
		}
		else
		{
			m_PpqCameraCallbackList.clear();
		}
		if (m_PpqCameraCallbackList.empty())
		{
			m_pCaller = 0UL;
		}
		return S_OK;
	}
	return E_FAIL;
}

HRESULT AcquisitionDevice::Start()
{
	HRESULT result = m_Thread.Create(&AcquisitionDevice::APCProc, m_DeviceName.c_str());

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
	Reset();

	return S_OK;
}

HRESULT AcquisitionDevice::Reset()
{
	m_cameraQueue.clear();
	return S_OK;
}

void AcquisitionDevice::Notify(CameraInfo&& cameraInfo)
{
	m_cameraQueue.emplace_back(std::move(cameraInfo));
	m_Thread.Signal(this);
}
