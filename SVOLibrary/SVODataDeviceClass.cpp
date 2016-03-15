// ******************************************************************************
// * COPYRIGHT (c) 2002 by SVResearch, Harrisburg
// * All Rights Reserved
// ******************************************************************************
// * .Module Name     : SVODataDeviceClass
// * .File Name       : $Workfile:   SVODataDeviceClass.cpp  $
// * ----------------------------------------------------------------------------
// * .Current Version : $Revision:   1.3  $
// * .Check In Date   : $Date:   01 Dec 2014 13:26:40  $
// ******************************************************************************

#include "stdafx.h"
#include "SVODataDeviceClass.h"

VOID CALLBACK SVODataDeviceClass::APCProc( ULONG_PTR dwParam )
{
}

/*
This constructor initializes the attributes to the desired default values.
*/
SVODataDeviceClass::SVODataDeviceClass()
: mbIsCreated(false)
, mbIsValid(false)
, mbIsStarted(false)
{
}

SVODataDeviceClass::SVODataDeviceClass(LPCTSTR deviceName)
: mDeviceName(deviceName)
, mbIsCreated(false)
, mbIsValid(false)
, mbIsStarted(false)
{
}

/*
This destructor calls the DestroyLocal method before the object gets destroyed.
*/
SVODataDeviceClass::~SVODataDeviceClass()
{
	DestroyLocal();
}

/*
This method returns the validity state of the object.
*/
bool SVODataDeviceClass::IsValid() const
{
	return mbIsValid;
}

void SVODataDeviceClass::ClearDevice()
{
	if( m_CallbackList.IsCreated() )
	{
		m_CallbackList.clear();
	}

	if( mUsedQueue.IsCreated() )
	{
		mUsedQueue.clear();
	}
}

/*
This method creates the processing threads, events, and initializes processing queues.
*/
HRESULT SVODataDeviceClass::Create( unsigned long ulSize )
{
	HRESULT hrOk = S_OK;

	if ( mbIsCreated )
	{
		hrOk = Destroy();
	}

	if ( hrOk == S_OK )
	{
		mUsedQueue.Create();
		m_CallbackList.Create();

		if( mUsedQueue.IsCreated() && m_CallbackList.IsCreated() && hrOk == S_OK )
		{
			mbIsValid = TRUE;
			mbIsCreated = TRUE;
		}
		else
		{
			if (hrOk == S_OK)
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
HRESULT SVODataDeviceClass::Destroy()
{
	HRESULT hrOk = S_OK;

	hrOk = DestroyLocal();

	return hrOk;
}

/*
This method adds the callback data to the callback list.
*/
HRESULT SVODataDeviceClass::RegisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller)
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
				if ( !( pData.empty() ) )
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
				SVOCallbackClassPtr pData = new SVOCallbackClass;
				if ( !( pData.empty() ) )
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
HRESULT SVODataDeviceClass::UnregisterCallback(SVOCallbackPtr pCallback, void *pvOwner, void *pvCaller)
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

				if ( !( pData.empty() ) )
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
HRESULT SVODataDeviceClass::UnregisterAllCallbacks()
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
HRESULT SVODataDeviceClass::Start()
{
	HRESULT hrOk = S_FALSE;

	if ( mbIsValid )
	{
		hrOk = m_Thread.Create( &SVODataDeviceClass::APCProc, SVProcessFunctorImpl( this ), mDeviceName.c_str(), SVAffinityAcq  );

		if( hrOk == S_OK )
		{
			m_Thread.SetPriority( THREAD_PRIORITY_ABOVE_NORMAL );

			if( m_Thread.IsActive() )
			{
				hrOk = Reset();

				if ( hrOk == S_OK )
				{
					mbIsStarted = TRUE;
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
HRESULT SVODataDeviceClass::Stop()
{
	HRESULT hrOk = S_OK;

	mbIsStarted = FALSE;

	m_Thread.SetPriority( THREAD_PRIORITY_NORMAL );

	m_Thread.Destroy();

	return hrOk;
}

/*
This method resets the internal attributes of this object.
*/
HRESULT SVODataDeviceClass::Reset()
{
	HRESULT hrOk = S_OK;

	if( mUsedQueue.IsCreated() )
	{
		if( ! mUsedQueue.RemoveAll() )
		{
			hrOk = E_FAIL;
		}
	}
	else
	{
		hrOk = E_FAIL;
	}

	return hrOk;
}

/*
This method processes the data in the device process thread.
*/
HRESULT SVODataDeviceClass::Process( bool& p_WaitForEvents )
{
	HRESULT hrOk = S_OK;

	long lRes = 0;

	if ( mbIsValid )
	{
		BOOL bDone = TRUE;

		mUsedQueue.GetSize( lRes );

		bDone = lRes < 1;
		
		while( ! bDone && hrOk == S_OK )
		{
			SVODataResponseClass l_Response;

			hrOk = ProcessResponse( mUsedQueue, l_Response );
			if ( hrOk == S_OK )
			{
				hrOk = ExecuteCallback( m_CallbackList, l_Response );
			}
			else
			{
				hrOk = -4332;
			}

			mUsedQueue.GetSize( lRes );

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
HRESULT SVODataDeviceClass::Notify( SVODataResponseClass& p_rResponse )
{
	HRESULT hrOk = S_FALSE;

	if ( mbIsStarted )
	{
		hrOk = AddUsedResponse( mUsedQueue, p_rResponse );
		if ( hrOk == S_OK )
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

LPCTSTR SVODataDeviceClass::GetDeviceName() const
{
	return mDeviceName.c_str(); 
}

void SVODataDeviceClass::SetDeviceName( LPCTSTR p_szName )
{
	mDeviceName = p_szName;
}

/*
This method gets the available next response from the used queue.
*/
HRESULT SVODataDeviceClass::GetUsedResponse( SVDataResponseQueue &rUsedQueue, SVODataResponseClass& p_rResponse )
{
	HRESULT hrOk = S_FALSE;

	if ( rUsedQueue.IsCreated() )
	{
		rUsedQueue.RemoveHead( &p_rResponse );

		hrOk = S_OK;
	}
	else
	{
		hrOk = -4349;
	}

	return hrOk;
}

/*
This method adds the new response from the used queue.
*/
HRESULT SVODataDeviceClass::AddUsedResponse( SVDataResponseQueue &rUsedQueue, SVODataResponseClass& p_rResponse )
{
	HRESULT hrOk = S_FALSE;

	if ( rUsedQueue.IsCreated() )
	{
		if ( rUsedQueue.AddTail( p_rResponse ) )
		{
			hrOk = S_OK;
		}
		else 
		{
			hrOk = -4380;
		}
	}
	else
	{
		hrOk = -4381;
	}

	return hrOk;
}

/*
This method gets the next new response from the used queue.
*/
HRESULT SVODataDeviceClass::ProcessResponse( SVDataResponseQueue &rUsedQueue, SVODataResponseClass& p_rResponse )
{
	HRESULT hrOk = S_FALSE;

	hrOk = GetUsedResponse( rUsedQueue, p_rResponse );

	return hrOk;
}

/*
This method walks the Callback list and executes the provided callback with the provided response.
*/
HRESULT SVODataDeviceClass::ExecuteCallback( SVCallbackClassPtrQueue &rCallbackList, SVODataResponseClass& p_rResponse )
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
			if ( !( pData.empty() ) )
			{
				if ( (pData->mpCallback)( pData->mpvOwner,
																	pData->mpvCaller,
																	&p_rResponse ) != S_OK )
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
HRESULT SVODataDeviceClass::GetIsValid(bool &rbIsValid) const
{
	HRESULT hrOk = S_OK;

	rbIsValid = mbIsValid;

	return hrOk;
}

/*
This method sets the current validity state of this object.
*/
HRESULT SVODataDeviceClass::SetIsValid(bool bIsValid)
{
	HRESULT hrOk = S_OK;

	mbIsValid = bIsValid;

	return hrOk;
}

/*
This method will clear and destroy the local attribute of this object.
*/
HRESULT SVODataDeviceClass::DestroyLocal()
{
	HRESULT hrOk = S_OK;

	mbIsValid = FALSE;
	mbIsStarted = FALSE;

	m_Thread.Destroy();

	if ( m_CallbackList.IsCreated() )
	{
		UnregisterAllCallbacks();	
		m_CallbackList.Destroy();
	}

	if( mUsedQueue.IsCreated() )
	{
		mUsedQueue.Destroy();
	}

	mbIsCreated = FALSE;

	return hrOk;
}

/*
This method returns the acquisition state of this object.
*/
bool SVODataDeviceClass::IsStarted() const
{
  return mbIsStarted;
}

