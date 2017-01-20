// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODeviceClass
// * .File Name       : $Workfile:   SVODeviceClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 08:47:30  $
// ******************************************************************************

#pragma once

#pragma region Includes
//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <string>

#include "SVContainerLibrary/SVRingBuffer.h"
#include "SVSystemLibrary/SVAsyncProcedure.h"
#include "SVUtilityLibrary/SVString.h"
#include "SVProcessFunctor.h"
#include "SVOCallbackClass.h"
#include "SVOResponseClass.h"
#include "SVQueueObject.h"
#pragma endregion Includes

/*
This class represents the base functionality required for a device.
*/
class SVODeviceClass  
{
public:
	typedef SVProcessFunctor< SVODeviceClass > SVProcessFunctorImpl;

	friend class SVProcessFunctor< SVODeviceClass >;

	static VOID CALLBACK APCProc( ULONG_PTR dwParam );

	SVODeviceClass( LPCTSTR deviceName );
	virtual ~SVODeviceClass();

	virtual bool IsValid() const;
	virtual bool IsStarted() const;

	virtual void ClearDevice();

	virtual HRESULT Create( unsigned long ulSize = 50 );

	virtual HRESULT Destroy();

	virtual HRESULT RegisterCallback( SVOCallbackPtr pCallback, 
	                                  void *pvOwner, void *pvCaller );
	virtual HRESULT UnregisterCallback( SVOCallbackPtr pCallback,
	                                    void *pvOwner, void *pvCaller );
	virtual HRESULT UnregisterAllCallbacks();

	virtual HRESULT Start();
	virtual HRESULT Stop();
	virtual HRESULT Reset();
	virtual HRESULT Pulse();

	LPCTSTR GetDeviceName() const;
	void SetDeviceName( LPCTSTR p_szName );

	virtual HRESULT Notify( SVOResponseClass& p_rResponse );

protected:
	typedef SVRingBuffer< SVOResponseClass > SVResponseQueue;

	/* SVODeviceClass Callback Functions */

	virtual HRESULT Process( bool& p_WaitForEvents );

	/* Device Callback Functions */

	virtual HRESULT GetUsedResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );
	virtual HRESULT AddUsedResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );

	virtual HRESULT ProcessResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );

	virtual HRESULT ExecuteCallback( SVCallbackClassPtrQueue &rCallbackList, SVOResponseClass& p_rResponse );

	virtual HRESULT GetIsValid( bool &rbIsValid )const;
	virtual HRESULT SetIsValid( bool bIsValid );

	virtual HRESULT TriggerDevice();

	//This attribute is a container that holds the list of registered recipients of the acquisition notifications.
	SVCallbackClassPtrQueue m_CallbackList;

	//This attribute holds the device name.
	SVString m_DeviceName;

private:
	SVODeviceClass();

	typedef VOID ( CALLBACK* SVAPCFunctionPtr )( ULONG_PTR );

	HRESULT DestroyLocal();

	//This attribute holds the creation state of this object.
	bool mbIsCreated;
	//This attribute holds the validity state of this object.
	bool mbIsValid;
	//This attribute holds the processing state of this object.
	bool mbIsStarted;

	//This attribute holds the handle to the application thread.
	SVAsyncProcedure< SVAPCFunctionPtr, SVProcessFunctorImpl > m_Thread;

	//This attribute is a container that holds the unused resposes in the callback system.
	SVResponseQueue mUsedQueue;
};
