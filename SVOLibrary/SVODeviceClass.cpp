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
: mDeviceName(deviceName), 
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

	if ( hrOk == S_OK )
	{
		m_CallbackList.Create();

		if( m_CallbackList.IsCreated() && hrOk == S_OK )
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
		hrOk = m_Thread.Create( &SVODeviceClass::APCProc, SVProcessFunctorImpl( this ), mDeviceName.c_str(), SVAffinityAcq );

		if( hrOk == S_OK )
		{
			m_Thread.SetPriority( THREAD_PRIORITY_TIME_CRITICAL );

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
HRESULT SVODeviceClass::Stop()
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
		BOOL bDone = TRUE;

		lRes = mUsedQueue.size();

		bDone = lRes < 1;
		
		while( ! bDone && hrOk == S_OK )
		{
			SVOResponseClass l_Response;

			hrOk = ProcessResponse( mUsedQueue, l_Response );
			if ( hrOk == S_OK )
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

LPCTSTR SVODeviceClass::GetDeviceName() const
{
	return mDeviceName.c_str(); 
}

void SVODeviceClass::SetDeviceName( LPCTSTR p_szName )
{
	mDeviceName = p_szName;
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
HRESULT SVODeviceClass::ExecuteCallback( SVCallbackClassPtrQueue &rCallbackList, SVOResponseClass& p_rResponse )
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
		if ( hrOk == S_OK )
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

	mbIsValid = FALSE;
	mbIsStarted = FALSE;

	m_Thread.Destroy();

	if ( m_CallbackList.IsCreated() )
	{
		UnregisterAllCallbacks();	
		m_CallbackList.Destroy();
	}

	mUsedQueue.clear();

	mbIsCreated = FALSE;

	return hrOk;
}

/*
This method returns the acquisition state of this object.
*/
bool SVODeviceClass::IsStarted() const
{
  return mbIsStarted;
}

// ******************************************************************************
// * LOG HISTORY:
// ******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVOLibrary\SVODeviceClass.cpp_v  $
 * 
 *    Rev 1.3   01 Dec 2014 13:26:40   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread attribute.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   02 Oct 2013 08:47:30   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  852
 * SCR Title:  Add Multiple Platform Support to SVObserver's Visual Studio Solution
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Add x64 platform.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   08 May 2013 15:28:10   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 012.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.15   29 Apr 2013 13:10:16   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  830
 * SCR Title:  Consolidate all Time Stamp and Clock Functionality
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to use the new SVClock::SVTimeStamp type for all time stamp variables.  Update to use new SVClock::GetTimeStamp() function to get the time stamp value in milliseconds.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 11:55:28   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.14   24 May 2011 15:37:30   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  702
 * SCR Title:  Updated Coreco Acquisition Functionality to Include Xcelera-AN Board
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated device class to use the new Ring Buffer to commuicate to the thread using lockless mentality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.13   30 Mar 2011 15:42:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  707
 * SCR Title:  Change Inspection Display Functionality to Force Display of Last Inspected
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated source code to clairify construction functionality and add clean-up functionality to remove memory leaks.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.12   24 Feb 2010 10:11:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Moved thread create to start and stop functions to allow for thread clean-up is a thread problem occurs.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.11   15 Feb 2010 10:58:54   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  682
 * SCR Title:  Upgrade SVObserver version for 5.31 release
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated old and added missing source code comments to existing code.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.10   19 Jan 2010 14:55:42   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.9   03 Sep 2009 09:22:58   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated files to fix moved file includes.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.8   25 Aug 2009 16:43:36   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated new threading methodology to remove crash when closing a configuration.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   30 Jul 2009 08:56:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   10 Sep 2008 12:56:56   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added constructor that takes the name string
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   30 Oct 2007 14:49:16   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Re-aranged catch to insure that memory is freed
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   20 Jun 2007 14:38:32   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  598
 * SCR Title:  Upgrade SVObserver to compile using vc++ in VS2005
 * Checked in by:  jSpila;  Joseph Spila
 * Change Description:  
 *   These changes include modification based on fixing compiler-based and project-based differences between VC6 and VC8.  These changes mainly include casting issues, but some include type conversion and assignment of new compiler controlling defines.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.1   14 Jul 2009 10:40:22   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated the code to handle buffer transfer and signal handling.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3.1.0   30 Oct 2007 08:49:32   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  609
 * SCR Title:  Fix GDI and Handle Leaks that limit the number of configuration loads.
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Re-aranged catch so resource will be cleaned up even after an exception in the thread proc.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   24 Mar 2003 12:21:34   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  eBeyeler;  Eric Beyeler
 * Change Description:  
 *   Updated RegisterCallback and UnregisterCallback methods to use a reference count to determine when to destroy a registered callback.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   11 Feb 2003 16:42:42   Joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added IsStarted method to the class.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   28 Jan 2003 16:06:36   joe
 * Project:  SVObserver
 * Change Request (SCR) nbr:  301
 * SCR Title:  Implement Result Image Circle Buffer
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated Process method to handle the Data Manager and new image buffer handling methodology.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   18 Nov 2002 10:15:36   ryoho
 * Project:  SVObserver
 * Change Request (SCR) nbr:  226
 * SCR Title:  Monochrome SVIM configuration compatibility between ViperQUAD and ViperDUAL
 * Checked in by:  rYoho;  Rob Yoho
 * Change Description:  
 *   first version of file
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/
