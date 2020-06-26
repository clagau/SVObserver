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

VOID CALLBACK SVODeviceClass::APCProc( ULONG_PTR )
{
}

/*
This constructor initializes the attributes to the desired default values.
*/
SVODeviceClass::SVODeviceClass()
: m_UsedQueue( 10 )
{
}

SVODeviceClass::SVODeviceClass(LPCTSTR deviceName)
: m_DeviceName(deviceName), 
	m_UsedQueue( 10 )
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

	m_UsedQueue.clear();
}

/*
This method creates the processing threads, events, and initializes processing queues.
*/
HRESULT SVODeviceClass::Create( unsigned long  )
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
						m_IsRegistered = true;
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
								m_IsRegistered = false;
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

			m_IsRegistered = false;

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

	m_UsedQueue.clear();

	return hrOk;
}

/*
This method processes the data in the device process thread.
*/
HRESULT SVODeviceClass::Process(bool& )
{
	HRESULT hrOk = S_OK;

	size_t lRes = 0;

	if ( mbIsValid )
	{
		bool bDone = true;

		lRes = m_UsedQueue.size();

		bDone = lRes < 1;
		
		while( ! bDone && S_OK == hrOk )
		{
			SVOResponseClass response;

			hrOk = ProcessResponse( m_UsedQueue, response );
			if ( S_OK == hrOk )
			{
				hrOk = ExecuteCallback(response);
				if(S_OK == hrOk)
				{
					hrOk = processAcquisitionTriggers(response);
				}
			}
			else
			{
				hrOk = -4332;
			}

			lRes = m_UsedQueue.size();

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
HRESULT SVODeviceClass::Notify(const SVOResponseClass& rResponse) const
{
	HRESULT hrOk = S_FALSE;

	if ( mbIsStarted )
	{
		hrOk = AddUsedResponse( m_UsedQueue, rResponse );
		if ( S_OK == hrOk )
		{
			hrOk = m_Thread.Signal(const_cast<SVODeviceClass*> (this) );
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
HRESULT SVODeviceClass::AddUsedResponse( SVResponseQueue &rUsedQueue, const SVOResponseClass& rResponse) const
{
	HRESULT hrOk = S_OK;

	hrOk = rUsedQueue.PushTail( rResponse );

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
HRESULT SVODeviceClass::ExecuteCallback(SVOResponseClass& rResponse)
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
				if ( S_OK != (pData->mpCallback)( pData->mpvOwner, pData->mpvCaller, &rResponse))
				{
					hrOk = -4386;
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

	m_UsedQueue.clear();

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

