// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODeviceClass
// * .File Name       : $Workfile:   SVODeviceClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Dec 2014 13:26:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVODeviceClass.h"

VOID CALLBACK SVODeviceClass::APCProc( ULONG_PTR dwParam )
{
}

/*
This constructor initializes the attributes to the desired default values.
*/
SVODeviceClass::SVODeviceClass()
: mbIsCreated(false),  
	mbIsValid(false), 
	mbIsStarted(false),
	mUsedQueue( 10 )
{
}

SVODeviceClass::SVODeviceClass(LPCTSTR deviceName)
: m_DeviceName(deviceName), 
	mbIsCreated(false),  
	mbIsValid(false), 
	mbIsStarted(false),
	mUsedQueue( 10 )
{
}

/*
This destructor calls the DestroyLocal method before the object gets destroyed.
*/
SVODeviceClass::~SVODeviceClass()
{
	DestroyLocal();
}

/*
This method returns the validity state of the object.
*/
bool SVODeviceClass::IsValid() const
{
	return mbIsValid;
}

void SVODeviceClass::ClearDevice()
{
	if( m_CallbackList.IsCreated() )
	{
		m_CallbackList.clear();
	}

	mUsedQueue.clear();
}

/*
This method creates the processing threads, events, and initializes processing queues.
*/
HRESULT SVODeviceClass::Create( unsigned long ulSize )
{
	HRESULT hrOk = S_OK;

	if ( mbIsCreated )
	{
		hrOk = Destroy();
	}

	if ( S_OK == hrOk )
	{
		m_CallbackList.Create();

		if( m_CallbackList.IsCreated() && S_OK == hrOk )
		{
			mbIsValid = true;
			mbIsCreated = true;
		}
		else
		{
			if (S_OK == hrOk)
			{
				hrOk = -4353;
			}

			Destroy();
		}
	}

	return hrOk;
}

/*
This method calls the DestroyLocal.
*/
HRESULT SVODeviceClass::Destroy()
{
	HRESULT hrOk = S_OK;

	hrOk = DestroyLocal();

	return hrOk;
}

/*
This method adds the callback data to the callback list.
*/
HRESULT SVODeviceClass::RegisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller)
{
	HRESULT hrOk = S_FALSE;

	if ( m_CallbackList.IsCreated() )
	{
		if ( m_CallbackList.Lock() )
		{
			long lSize = 0;

			m_CallbackList.GetSize( lSize );
			long l = 0;
			for ( ; l < lSize; l++ )
			{
				SVOCallbackClassPtr pData;

				m_CallbackList.GetAt( l, &pData );
				if (nullptr != pData )
				{
					if ( pData->mpCallback == pCallback &&
					     pData->mpvOwner == pvOwner &&
							 pData->mpvCaller == pvCaller )
					{
						pData->m_lRefCount++;

						hrOk = S_OK;

						break;
					}
				}
				else
				{
					break;
				}
			}

			if (hrOk == S_FALSE)
			{
				hrOk = -4353;
			}

			if ( lSize <= l )
			{
				SVOCallbackClassPtr pData{ new SVOCallbackClass };
				if (nullptr != pData )
				{
					pData->mpCallback = pCallback;
					pData->mpvOwner = pvOwner;
					pData->mpvCaller = pvCaller;
					pData->m_lRefCount = 1;

					if ( m_CallbackList.AddTail( pData ) )
					{
						hrOk = S_OK;
					}
					else
					{
						hrOk = -4354;
					}
				}
				else
				{
					hrOk = -4355;
				}
			}
			else
			{
				hrOk = -4356;
			}

			m_CallbackList.Unlock();
		}
		else
		{
			hrOk = -4357;
		}
	}
	else
	{
		hrOk = -4358;
	}

	return hrOk;
}

/*
This method removes the callback data from the callback list.
*/
HRESULT SVODeviceClass::UnregisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller)
{
	HRESULT hrOk = S_FALSE;

	if ( m_CallbackList.IsCreated() )
	{
		if ( m_CallbackList.Lock() )
		{
			long lSize = 0;

			m_CallbackList.GetSize( lSize );

			for ( long l = lSize - 1; -1 < l; l-- )
			{
				SVOCallbackClassPtr pData;
				
				m_CallbackList.GetAt( l, &pData );

				if (nullptr != pData )
				{
					if ( pData->mpCallback == pCallback &&
					     pData->mpvOwner == pvOwner &&
							 pData->mpvCaller == pvCaller )
					{
						pData->m_lRefCount--;

						if ( pData->m_lRefCount < 1 )
						{
							if ( m_CallbackList.RemoveAt( l ) )
							{
								hrOk = S_OK;
							}
						}
						else
						{
							hrOk = S_OK;
						}

						break;
					}
				}
				else
				{
					m_CallbackList.RemoveAt( l );
				}
			}

			if (hrOk == S_FALSE)
			{
				hrOk = -4359;
			}

			m_CallbackList.Unlock();
		}
		else
		{
			hrOk = -4360;
		}
	}
	else
	{
		hrOk = -4361;
	}

	return hrOk;
}

/*
This method removes all callback data from the callback list.
*/
HRESULT SVODeviceClass::UnregisterAllCallbacks()
{
	HRESULT hrOk = S_FALSE;

	if ( m_CallbackList.IsCreated() )
	{
		if ( m_CallbackList.Lock() )
		{
			long lSize = 0;

			m_CallbackList.GetSize( lSize );

			hrOk = S_OK;

			for ( long l = lSize - 1; -1 < l; l-- )
			{
				if ( ! m_CallbackList.RemoveAt( l ) )
				{
					hrOk = -4362;
				}
			}

			m_CallbackList.Unlock();
		}
		else
		{
			hrOk = -4363;
		}
	}
	else
	{
		hrOk = -4364;
	}

	return hrOk;
}

/*
This method starts the acquisition process.
*/
HRESULT SVODeviceClass::Start()
{
	HRESULT hrOk = S_FALSE;

	if ( mbIsValid )
	{
		hrOk = m_Thread.Create( &SVODeviceClass::APCProc, SVProcessFunctorImpl( this ), m_DeviceName.c_str(), SVAffinityAcq );

		if( S_OK == hrOk )
		{
			m_Thread.SetPriority( THREAD_PRIORITY_TIME_CRITICAL );

			if( m_Thread.IsActive() )
			{
				hrOk = Reset();

				if ( S_OK == hrOk )
				{
					mbIsStarted = true;
				}
				else
				{
					hrOk = -4369;
				}
			}
			else
			{
				hrOk = -4371;
			}
		}
	}
	else
	{
		hrOk = -4372;
	}

	return hrOk;
}

/*
This method stops the acquisition process.
*/
HRESULT SVODeviceClass::Stop()
{
	HRESULT hrOk = S_OK;

	mbIsStarted = false;

	m_Thread.SetPriority( THREAD_PRIORITY_NORMAL );

	m_Thread.Destroy();

	return hrOk;
}

/*
This method resets the internal attributes of this object.
*/
HRESULT SVODeviceClass::Reset()
{
	HRESULT hrOk = S_OK;

	mUsedQueue.clear();

	return hrOk;
}

/*
This method calls TriggerDevice if this object is in a valid state.
*/
HRESULT SVODeviceClass::Pulse()
{
	HRESULT hrOk = S_FALSE;

	if ( mbIsValid )
	{
		hrOk = TriggerDevice();
	}
	else
	{
		hrOk = -4378;
	}

	return hrOk;
}

/*
This method processes the data in the device process thread.
*/
HRESULT SVODeviceClass::Process( bool& p_WaitForEvents )
{
	HRESULT hrOk = S_OK;

	size_t lRes = 0;

	if ( mbIsValid )
	{
		bool bDone = true;

		lRes = mUsedQueue.size();

		bDone = lRes < 1;
		
		while( ! bDone && S_OK == hrOk )
		{
			SVOResponseClass l_Response;

			hrOk = ProcessResponse( mUsedQueue, l_Response );
			if ( S_OK == hrOk )
			{
				hrOk = ExecuteCallback( m_CallbackList, l_Response );
			}
			else
			{
				hrOk = -4332;
			}

			lRes = mUsedQueue.size();

			bDone = lRes < 1;
		} // while ((!bDone) ...;
	}
	else
	{
		hrOk = -4330;
	}

	return hrOk;
}

/*
This method adds the new response to the process queue.
*/
HRESULT SVODeviceClass::Notify( SVOResponseClass& p_rResponse )
{
	HRESULT hrOk = S_FALSE;

	if ( mbIsStarted )
	{
		hrOk = AddUsedResponse( mUsedQueue, p_rResponse );
		if ( S_OK == hrOk )
		{
			hrOk = m_Thread.Signal( this );
		}
		else
		{
			hrOk = -4403;
		}
	}
	else
	{
		hrOk = -4348;
	}

	return hrOk;
}

LPCTSTR SVODeviceClass::GetDeviceName() const
{
	return m_DeviceName.c_str(); 
}

void SVODeviceClass::SetDeviceName( LPCTSTR p_szName )
{
	m_DeviceName = p_szName;
}

/*
This method gets the available next response from the used queue.
*/
HRESULT SVODeviceClass::GetUsedResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse )
{
	HRESULT hrOk = S_OK;

	hrOk = rUsedQueue.PopHead( p_rResponse );

	return hrOk;
}

/*
This method adds the new response from the used queue.
*/
HRESULT SVODeviceClass::AddUsedResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse )
{
	HRESULT hrOk = S_OK;

	hrOk = rUsedQueue.PushTail( p_rResponse );

	return hrOk;
}

/*
This method gets the next new response from the used queue.
*/
HRESULT SVODeviceClass::ProcessResponse( SVResponseQueue &rUsedQueue, SVOResponseClass& p_rResponse )
{
	HRESULT hrOk = S_FALSE;

	hrOk = GetUsedResponse( rUsedQueue, p_rResponse );

	return hrOk;
}

/*
This method walks the Callback list and executes the provided callback with the provided response.
*/
HRESULT SVODeviceClass::ExecuteCallback( SVCallbackClassPtrQueue&, SVOResponseClass& p_rResponse )
{
	HRESULT hrOk = S_FALSE;

	if ( m_CallbackList.IsCreated() )
	{
		long lSize = 0;

		m_CallbackList.GetSize( lSize );

		hrOk = S_OK;

		for ( long l = 0; l < lSize; l++ )
		{
			SVOCallbackClassPtr pData;

			m_CallbackList.GetAt( l, &pData );
			if (nullptr != pData )
			{
				if ( S_OK != (pData->mpCallback)( pData->mpvOwner, pData->mpvCaller, &p_rResponse ) )
				{
					hrOk = -4386;;
				}
			}
			else
			{
				hrOk = -4387;
			}
		}
	}
	else
	{
		hrOk = -4388;
	}

	return hrOk;
}

/*
This method provide the current validity state of this object.
*/
HRESULT SVODeviceClass::GetIsValid(bool &rbIsValid) const
{
	HRESULT hrOk = S_OK;

	rbIsValid = mbIsValid;

	return hrOk;
}

/*
This method sets the current validity state of this object.
*/
HRESULT SVODeviceClass::SetIsValid(bool bIsValid)
{
	HRESULT hrOk = S_OK;

	mbIsValid = bIsValid;

	return hrOk;
}

/*
This method will initiate a trigger to send a response through the process loop.
*/
HRESULT SVODeviceClass::TriggerDevice()
{
	HRESULT hrOk = S_FALSE;

	if ( mbIsValid )
	{
		SVOResponseClass l_Response;

		hrOk = AddUsedResponse( mUsedQueue, l_Response );
		if ( S_OK == hrOk )
		{
			hrOk = m_Thread.Signal( this );
		}
		else
		{
			hrOk = -4402;
		}
	}
	else
	{
		hrOk = -4391;
	}

	return hrOk;
}

/*
This method will clear and destroy the local attribute of this object.
*/
HRESULT SVODeviceClass::DestroyLocal()
{
	HRESULT hrOk = S_OK;

	mbIsValid = false;
	mbIsStarted = false;

	m_Thread.Destroy();

	if ( m_CallbackList.IsCreated() )
	{
		UnregisterAllCallbacks();	
		m_CallbackList.Destroy();
	}

	mUsedQueue.clear();

	mbIsCreated = false;

	return hrOk;
}

/*
This method returns the acquisition state of this object.
*/
bool SVODeviceClass::IsStarted() const
{
  return mbIsStarted;
}

