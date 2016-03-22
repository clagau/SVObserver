// ******************************************************************************
// * COPYRIGHT (c) 2004 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVIntekFireStackClass
// * .File Name       : $Workfile:   SVIntekFireStackClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.0  $
// * .Check In Date   : $Date:   26 Apr 2013 14:53:34  $
// ******************************************************************************

#pragma once

#include "SVIntekExports.h"
#include "SVIntekEnums.h"
#include "SVOLibrary/SVQueueObject.h"

#include "SVIntekBoardStruct.h"
#include "SVIntekCameraStruct.h"

#include "SVIntekCameraParamManager.h"
#include "SVTriggerLibrary/SVInternalTrigger.h"
#include "SVTriggerLibrary/SVTriggerCallbackMap.h"

class SVAcquisitionBufferInterface;
struct SVCallbackStruct;

class SVIntekFireStackClass  
{
public:
	static DWORD WINAPI SVIntekEndFrameThreadProc( LPVOID p_lpParam );

	SVIntekFireStackClass();
	virtual ~SVIntekFireStackClass();

	virtual HRESULT Create();
	virtual HRESULT Destroy( bool p_bClose = false );

	virtual HRESULT Open();
	virtual HRESULT ThreadOpen();
	virtual HRESULT Close();

	virtual HRESULT GetStatusHandles( unsigned long &p_rulSize, HANDLE **p_pphArray );

	virtual HRESULT BufferCreate( unsigned long p_ulWidth, unsigned long p_ulHeight, int p_iFormat, SVIntekBufferStruct &p_rBuffer );
	virtual HRESULT BufferDestroy( SVIntekBufferStruct &p_rBuffer );

	virtual HRESULT CameraCreate( SVIntekBoardStruct *p_pBoard, CCamera *p_pCamera, bool& bNewCamera );
	virtual HRESULT CameraDestroy( CCamera *p_pCamera );
	virtual HRESULT CameraGetCount( unsigned long &p_rulCount );
	virtual HRESULT CameraGetName( unsigned long p_ulIndex, BSTR &p_rbstrName );
	virtual HRESULT CameraGetHeight( unsigned long p_ulIndex, unsigned long &p_rulHeight );
	virtual HRESULT CameraGetWidth( unsigned long p_ulIndex, unsigned long &p_rulWidth );
	virtual HRESULT CameraGetFormat( unsigned long p_ulIndex, int &p_riFormat );
	virtual HRESULT CameraGetParameterList( unsigned long p_ulIndex, VARIANT *p_pvarValue );
	virtual HRESULT CameraGetParameterName( unsigned long p_ulIndex, int p_iParameterID, BSTR &p_rbstrName );
	virtual HRESULT CameraGetParameter( unsigned long p_ulIndex, int p_iParameterID, int *p_piParameterIDType, VARIANT *p_pvarValue );
	virtual HRESULT CameraSetParameter( unsigned long p_ulIndex, int p_iParameterID, int p_iParameterIDType, VARIANT *p_pvarValue );
	virtual HRESULT CameraBufferCreateAll( unsigned long p_ulIndex, unsigned long p_ulCount );
	virtual HRESULT CameraBufferDestroyAll( unsigned long p_ulIndex );
	virtual HRESULT CameraRegisterBufferInterface( unsigned long p_ulIndex, SVAcquisitionBufferInterface* p_pInterface );
	virtual HRESULT CameraUnregisterBufferInterface( unsigned long p_ulIndex );
	virtual HRESULT CameraStart( unsigned long p_ulIndex );
	virtual HRESULT CameraStop( unsigned long p_ulIndex );
	virtual HRESULT CameraProcessStartFrame( unsigned long p_ulIndex );
	virtual HRESULT CameraAPCProcessEndFrame( unsigned long p_ulIndex );
	virtual HRESULT CameraProcessEndFrame( unsigned long p_ulIndex );
	virtual HRESULT CameraLoadFiles(unsigned long p_ulIndex, SAFEARRAY* p_psaFileNames);

	virtual HRESULT InternalTriggerEnable( unsigned long p_ulIndex );
	virtual HRESULT InternalTrigger( unsigned long p_ulIndex );
	virtual HRESULT RegisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterInternalTriggerCallback( unsigned long p_ulIndex, SVCallbackStruct& callbackStruct );
	virtual HRESULT UnregisterAllInternalTriggerCallbacks( unsigned long p_ulIndex );

	HRESULT ReloadCameraParams(unsigned long p_ulIndex, CCamera* p_pCamera);

	// Trigger functions ...
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

protected:
	virtual HRESULT CameraGetFormatModeRate( unsigned long p_ulIndex, unsigned long &p_rulValue );
	virtual HRESULT CameraSetFormatModeRate( unsigned long p_ulIndex, unsigned long p_ulValue );
	virtual HRESULT CameraUpdateFormatModeRateColorFormat( unsigned long p_ulIndex );

	//This attribute holds the reference count for this object.  It can only be create and destroyed once.
	long m_lRefCount;

	//This attribute is a container that holds the available acquisition boards in the system.
	SVIntekBoardStructPtrCArray m_caBoards;
	//This attribute is a container that holds the available digitizers in the system.
	SVIntekCameraStructCArray m_caCameras;

	//This attribute holds the handle to the processing thread that process the acquisition buffer to the application.
	HANDLE m_hThread;
	//This attribute holds the handle to the shutdown event that stops the processing thread.
	HANDLE m_hShutdown;

private:
	HRESULT SetFeature(int p_ulIndex, unsigned char featureId, unsigned long ulVal, bool bSubValue=false);
	HRESULT WriteQuad(int p_ulIndex, unsigned long ulVal);
	HRESULT WriteBlock(int p_ulIndex, VARIANT* p_pvarValue);
	HRESULT SetIntekCameraParams(unsigned long p_ulIndex);
	void TrackCameraParams(unsigned long p_ulIndex);
	void LockMainCameraParamList(unsigned long p_ulIndex);
	void UnTrackCameraParams(unsigned long p_ulIndex);

	HRESULT FireOneShot( unsigned long p_ulIndex );
	HRESULT FireSoftwareTrigger( unsigned long p_ulIndex );
	HRESULT DispatchTriggerCallback( unsigned long p_ulIndex );

	bool IsValidDigitizer(unsigned long p_ulIndex) const;
	SVIntekCameraStruct& GetDigitizer(unsigned long p_ulHandle);
	const SVIntekCameraStruct& GetDigitizer(unsigned long p_ulHandle) const;

	HRESULT IsAcquisitionTriggered(unsigned long p_ulHandle, bool& bAcquisitionTriggered) const;
	HRESULT SetAcquisitionTriggered(unsigned long p_ulHandle, bool bAcquisitionTriggered);
	void DoAcquisitionTrigger(const SVIntekCameraStruct& p_rCamera);

	//This attribute is a container that manages the camera parameters.
	SVIntekCameraParamManager m_cameraParamsMgr;
	//This attribute holds all of the trigger connections for non-hardware based triggers.
	SVInternalTrigger m_triggerMgr;
	//This attribute holds all of the callback information for the non-hardware based triggers.
	SVTriggerCallbackMap m_triggerMap;
};

