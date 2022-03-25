//******************************************************************************
//* COPYRIGHT (c) 2008 by K�rber Pharma Inspection GmbH. All Rights Reserved
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVFileAcquisitionDevice
//* .File Name       : $Workfile:   SVFileAcquisitionDevice.h  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.0  $
//* .Check In Date   : $Date:   22 Apr 2013 09:26:04  $
//******************************************************************************

#pragma once

#pragma region Includes
#include "SVFileCamera.h"
#pragma endregion Includes

class SVAcquisitionBufferInterface;

constexpr long MaxFileCameras = 12;

class SVFileAcquisitionDevice
{
public:
	SVFileAcquisitionDevice() = default;
	~SVFileAcquisitionDevice();

	HRESULT Create();
	HRESULT Destroy(bool close = false);

	unsigned long CameraGetCount() const;
	_variant_t CameraGetName(unsigned long cameraIndex) const;
	unsigned long CameraGetWidth(unsigned long cameraIndex) const;
	unsigned long CameraGetHeight(unsigned long cameraIndex) const;
	int CameraGetFormat(unsigned long cameraIndex) const;
	HRESULT CameraBufferCreateAll(unsigned long cameraIndex);
	HRESULT CameraBufferDestroyAll(unsigned long cameraIndex);
	HRESULT CameraRegisterBufferInterface(unsigned long cameraIndex, SVAcquisitionBufferInterface* pInterface);
	HRESULT CameraUnregisterBufferInterface(unsigned long cameraIndex);
	HRESULT CameraStart(unsigned long cameraIndex);
	HRESULT CameraStop(unsigned long cameraIndex);
	_variant_t CameraGetParameterName(unsigned long cameraIndex, int parameterID) const;
	_variant_t CameraGetParameter(unsigned long cameraIndex, int parameterID) const;
	HRESULT CameraSetParameter(unsigned long cameraIndex, int parameterID, const _variant_t& rValue);
	_variant_t CameraGetParameterList(unsigned long cameraIndex) const;

	HRESULT InternalTrigger(unsigned long cameraIndex, LPCTSTR pAcquisitionFile);

private:
	HRESULT CameraProcessStartFrame( unsigned long cameraIndex);
	HRESULT CameraProcessEndFrame( unsigned long cameraIndex);

	HRESULT FireOneShot(unsigned long cameraIndex, LPCTSTR pAcquisitionFile);

private:
	long m_lRefCount{ 0L };

	std::array<SVFileCamera, MaxFileCameras> m_cameras;
};

