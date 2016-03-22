// ******************************************************************************
// * COPYRIGHT (c) 2011 by Seidenader Vision, Inc.
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODataDeviceClass
// * .File Name       : $Workfile:   SVODataDeviceClass.h  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.1  $
// * .Check In Date   : $Date:   02 Oct 2013 08:47:28  $
// ******************************************************************************

#pragma once

//Moved to precompiled header: #include <comdef.h>
//Moved to precompiled header: #include <string>

#include "SVSystemLibrary/SVAsyncProcedure.h"

#include "SVOCallbackClass.h"
#include "SVODataResponseClass.h"
#include "SVProcessFunctor.h"
#include "SVQueueObject.h"

//typedef HRESULT ( CALLBACK *SVODataDeviceClassCallbackPtr )( void *, /* Owner */
//                                                         void *, /* Caller */
//                                                         SVODataResponseClass * );

/*
This class represents the base functionality required for a device.
*/
class SVODataDeviceClass  
{
public:
	typedef std::string SVDeviceName;
	typedef SVProcessFunctor< SVODataDeviceClass > SVProcessFunctorImpl;

	friend class SVProcessFunctor< SVODataDeviceClass >;

	static VOID CALLBACK APCProc( ULONG_PTR dwParam );

	SVODataDeviceClass( LPCTSTR deviceName );
	virtual ~SVODataDeviceClass();

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

	LPCTSTR GetDeviceName() const;
	virtual void SetDeviceName( LPCTSTR p_szName );

protected:
	typedef SVTQueueObject< SVODataResponseClass > SVDataResponseQueue;

	/* SVODataDeviceClass Callback Functions */

	virtual HRESULT Process( bool& p_WaitForEvents );

	/* Device Callback Functions */

	virtual HRESULT Notify( SVODataResponseClass& p_rResponse );

	virtual HRESULT GetUsedResponse( SVDataResponseQueue &rUsedQueue, SVODataResponseClass& p_rResponse );
	virtual HRESULT AddUsedResponse( SVDataResponseQueue &rUsedQueue, SVODataResponseClass& p_rResponse );

	virtual HRESULT ProcessResponse( SVDataResponseQueue &rUsedQueue, SVODataResponseClass& p_rResponse );

	virtual HRESULT ExecuteCallback( SVCallbackClassPtrQueue &rCallbackList, SVODataResponseClass& p_rResponse );

	virtual HRESULT GetIsValid( bool &rbIsValid )const;
	virtual HRESULT SetIsValid( bool bIsValid );

	//This attribute is a container that holds the list of registered recipients of the acquisition notifications.
	SVCallbackClassPtrQueue m_CallbackList;

	//This attribute holds the device name.
	SVDeviceName mDeviceName;

	//This attribute holds the creation state of this object.
	bool mbIsCreated;

private:
	SVODataDeviceClass();

	typedef VOID ( CALLBACK* SVAPCFunctionPtr )( ULONG_PTR );

	HRESULT DestroyLocal();

	//This attribute holds the validity state of this object.
	bool mbIsValid;
	//This attribute holds the processing state of this object.
	bool mbIsStarted;

	//This attribute holds the handle to the application thread.
	SVAsyncProcedure< SVAPCFunctionPtr, SVProcessFunctorImpl > m_Thread;

	//This attribute is a container that holds the unused resposes in the callback system.
	SVDataResponseQueue mUsedQueue;

};

