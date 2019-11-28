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

#pragma once

#pragma region Includes
#include "SVOLibrary/SVQueueObject.h"
#include "SVFileCamera.h"
#pragma endregion Includes

class SVAcquisitionBufferInterface;

constexpr long MaxFileCameras = 12;

class SVFileAcquisitionDevice
{
private:
	long m_lRefCount;

	std::array<SVFileCamera, MaxFileCameras> m_cameras;
	
public:
	SVFileAcquisitionDevice();
	~SVFileAcquisitionDevice();

	HRESULT Create();
	HRESULT Destroy( bool p_bClose = false );

	HRESULT CameraGetCount( unsigned long &p_rulCount );
	HRESULT CameraGetName( unsigned long p_ulIndex, BSTR &p_rbstrName );
	HRESULT CameraGetHeight( unsigned long p_ulIndex, unsigned long &p_rulHeight );
	HRESULT CameraGetWidth( unsigned long p_ulIndex, unsigned long &p_rulWidth );
	HRESULT CameraGetFormat( unsigned long p_ulIndex, int &p_riFormat );
	HRESULT CameraGetParameterList( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	HRESULT CameraGetParameterName( unsigned long p_ulIndex, int p_iParameterID, BSTR &p_rbstrName );
	HRESULT CameraGetParameter( unsigned long p_ulIndex, int p_iParameterID, int *p_piParameterIDType, VARIANT *p_pvarValue );
	HRESULT CameraSetParameter( unsigned long p_ulIndex, int p_iParameterID, int p_iParameterIDType, VARIANT *p_pvarValue );
	
	HRESULT CameraBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount );
	HRESULT CameraBufferDestroyAll( unsigned long p_ulIndex );
	HRESULT CameraRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface );
	HRESULT CameraUnregisterBufferInterface( unsigned long p_ulIndex );
	HRESULT CameraStart( unsigned long p_ulIndex );
	HRESULT CameraStop( unsigned long p_ulIndex );
	HRESULT CameraLoadFiles(unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames);

	HRESULT InternalTriggerEnable( unsigned long p_ulIndex );
	HRESULT InternalTrigger( unsigned long p_ulIndex );

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
	bool IsValidDigitizer(unsigned long triggerchannel) const;
	SVFileCamera& GetDigitizer(unsigned long triggerchannel);
	const SVFileCamera& GetDigitizer(unsigned long triggerchannel) const;

	HRESULT CameraProcessStartFrame( unsigned long p_ulIndex );
	HRESULT CameraProcessEndFrame( unsigned long p_ulIndex );

	HRESULT FireOneShot( unsigned long p_ulIndex );
};

