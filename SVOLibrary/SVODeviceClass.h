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
	virtual bool IsRegistered() const { return m_IsRegistered; }

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
	void SetDeviceName( LPCTSTR p_szName );

	virtual HRESULT Notify(const SVOResponseClass& rResponse) const;

protected:
	typedef SVRingBuffer< SVOResponseClass > SVResponseQueue;

	/* SVODeviceClass Callback Functions */

	virtual HRESULT Process( bool& p_WaitForEvents );

	/* Device Callback Functions */

	virtual HRESULT GetUsedResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );
	virtual HRESULT AddUsedResponse( SVResponseQueue &rUsedQueue, const SVOResponseClass& rResponse ) const;

	virtual HRESULT ProcessResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse );

	virtual HRESULT ExecuteCallback(SVOResponseClass& rResponse);
	virtual HRESULT processAcquisitionTriggers(const SVOResponseClass& ) const { return S_OK; } 

	virtual HRESULT GetIsValid( bool &rbIsValid )const;
	virtual HRESULT SetIsValid( bool bIsValid );

	//This attribute is a container that holds the list of registered recipients of the acquisition notifications.
	SVCallbackClassPtrQueue m_CallbackList;

	//This attribute holds the device name.
	std::string m_DeviceName;

private:
	SVODeviceClass();

	typedef VOID ( CALLBACK* SVAPCFunctionPtr )( ULONG_PTR );

	HRESULT DestroyLocal();

	//This attribute holds the creation state of this object.
	bool mbIsCreated {false};
	//This attribute holds the validity state of this object.
	bool mbIsValid {false};
	//This attribute holds the processing state of this object.
	bool mbIsStarted {false};
	//This attribute holds the registered state of this object.
	bool m_IsRegistered {false};

	//This attribute holds the handle to the application thread.
	mutable SVAsyncProcedure< SVAPCFunctionPtr, SVProcessFunctorImpl > m_Thread;

	//This attribute is a container that holds the unused resposes in the callback system.
	mutable SVResponseQueue m_UsedQueue;
};
