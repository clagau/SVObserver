//******************************************************************************
//* COPYRIGHT (c) 2008 by Seidenader Vision Inc., Harrisburg
//* All Rights Reserved
//******************************************************************************
//* .Module Name     : SVThread
//* .File Name       : $Workfile:   SVThread.inl  $
//* ----------------------------------------------------------------------------
//* .Current Version : $Revision:   1.1  $
//* .Check In Date   : $Date:   01 Dec 2014 13:59:04  $
//******************************************************************************

#include <tchar.h>
#include "SVStatusLibrary\MessageManager.h"
#include "ObjectInterfaces\ErrorNumbers.h"
#include "SVMessage\SVMessage.h"
#include "SVThreadManager.h"

template<typename SVThreadSignalHandler>
SVThread<SVThreadSignalHandler>::SVThread()
: m_hShutdown( 0 )
, m_hThreadComplete( 0 )
, m_hThread( 0 )
, m_ulThreadID( 0 )
{
}

template<typename SVThreadSignalHandler>
SVThread<SVThreadSignalHandler>::~SVThread()
{
	Destroy();
}

template<typename SVThreadSignalHandler>
DWORD WINAPI SVThread<SVThreadSignalHandler>::ThreadProc( LPVOID lpParam )
{
	HRESULT Result = S_OK;

	if( lpParam )
	{
		#ifdef DEBUG
			SVString l_Message;
		#endif

		SVThread* pThread = static_cast< SVThread* >( lpParam );

		::ResetEvent( pThread->m_hThreadComplete );

		bool l_Continue = true;
		bool l_WaitForEvents = true;

		while( l_Continue )
		{
			if( l_WaitForEvents )
			{
				l_Continue = ::WaitForSingleObjectEx( pThread->m_hShutdown, INFINITE, TRUE ) == WAIT_IO_COMPLETION;
			}

			if( l_Continue )
			{
				l_Continue = ::WaitForSingleObjectEx( pThread->m_hShutdown, 0, FALSE ) == WAIT_TIMEOUT;

				if( l_Continue )
				{
					l_WaitForEvents = true;

					pThread->m_threadHandler( l_WaitForEvents );
				}
				#ifdef DEBUG
					else
					{
						l_Message.Format( _T( "SVThread(%d) - Shutdown Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
						::OutputDebugString( l_Message.ToString() );
					}
				#endif
			}
			#ifdef DEBUG
				else
				{
					l_Message.Format( _T( "SVThread(%d) - Exit Loop = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
					::OutputDebugString( l_Message.ToString() );
				}
			#endif
		}

		#ifdef DEBUG
			l_Message.Format( _T( "SVThread(%d) - Exit Thread = %s\n" ), pThread->m_ulThreadID, pThread->m_tag.c_str() );
			::OutputDebugString( l_Message.ToString() );
		#endif

		::SetEvent( pThread->m_hThreadComplete );
	}
	else
	{
		DWORD errorCode = GetLastError();

		Result = SVMSG_THREAD_EXIT_ERROR;

		SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
		Exception.setMessage( static_cast<DWORD> (Result), _T( "Unknown Thread" ), StdMessageParams, SvOi::Err_25030_Thread, errorCode );
	}

	return Result;
}

template<typename SVThreadSignalHandler>
HRESULT SVThread<SVThreadSignalHandler>::Create(const SVThreadSignalHandler& threadHandler, LPCTSTR tag, SVThreadAttribute eAttribute )
{
	HRESULT Result = S_OK;

	m_tag = tag;
	m_threadHandler = threadHandler;

	if (!m_hShutdown)
	{
		m_hShutdown = ::CreateEvent( NULL, TRUE, FALSE, NULL );
		if (m_hShutdown == NULL)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25031_Thread, errorCode );
		}
	}

	if( !m_hThreadComplete )
	{
		m_hThreadComplete = ::CreateEvent( NULL, TRUE, TRUE, NULL );
		if( m_hThreadComplete == NULL )
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25032_Thread, errorCode );
		}
	}

	if (!m_hThread)
	{
		m_hThread = ::CreateThread( NULL, 0, SVThread::ThreadProc, (LPVOID)this, 0, &m_ulThreadID );

		if (m_hThread == NULL)
		{
			Result = SVMSG_THREAD_CREATION_ERROR;

			DWORD errorCode = GetLastError();

			SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
			Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25033_Thread, errorCode );
		}
		else
		{
			SVThreadManager::Instance().Add(m_hThread, tag, eAttribute); // keep track of thread.

			if( Result == S_OK )
			{
				unsigned long l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );

				for( size_t i = 0; l_WaitStatus == WAIT_OBJECT_0 && i < 1000; ++i )
				{
					::Sleep( 1 );

					l_WaitStatus = ::WaitForSingleObject( m_hThreadComplete, 0 );
				}

				if( l_WaitStatus != WAIT_TIMEOUT )
				{
					Result = SVMSG_THREAD_CREATION_ERROR;

					DWORD errorCode = GetLastError();

					SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
					Exception.setMessage( static_cast<DWORD> (Result), m_tag.c_str(), StdMessageParams, SvOi::Err_25034_Thread, errorCode );
				}
			}
		}
	}

	return Result;
}

template<typename SVThreadSignalHandler>
unsigned long SVThread<SVThreadSignalHandler>::GetThreadID() const
{
	return m_ulThreadID;
}

template<typename SVThreadSignalHandler>
HRESULT SVThread<SVThreadSignalHandler>::Restart()
{
	Destroy();

	return Create( m_threadHandler, m_tag.c_str(), SVNone );
}

template< typename SVThreadSignalHandler >
void SVThread< SVThreadSignalHandler >::Destroy()
{
	if( m_hThread )
	{
		if( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT )
		{
			::SetEvent( m_hShutdown );

			if( ::WaitForSingleObject( m_hThreadComplete, m_timeoutShutdownThread ) != WAIT_OBJECT_0 )
			{
				DWORD errorCode = GetLastError();

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_THREAD_EXIT_ERROR, m_tag.c_str(), StdMessageParams, SvOi::Err_25035_Thread, errorCode );

				::TerminateThread( m_hThread, E_FAIL );

				::SetEvent( m_hThreadComplete );
			}

			::ResetEvent( m_hShutdown );
		}
		else
		{
			if( ::WaitForSingleObject( m_hThread, 0 ) == WAIT_TIMEOUT )
			{
				DWORD errorCode = GetLastError();

				SvStl::MessageMgrNoDisplay Exception( SvStl::LogOnly );
				Exception.setMessage( SVMSG_THREAD_EXIT_ERROR, m_tag.c_str(), StdMessageParams, SvOi::Err_25036_Thread, errorCode );

				::TerminateThread( m_hThread, E_FAIL );
			}
		}

		::CloseHandle( m_hThread );
		SVThreadManager::Instance().Remove( m_hThread );
		m_hThread = NULL;
	}

	if( m_hThreadComplete )
	{
		::CloseHandle( m_hThreadComplete );
		m_hThreadComplete = NULL;
	}

	if( m_hShutdown )
	{
		::CloseHandle( m_hShutdown );
		m_hShutdown = NULL;
	}

	m_ulThreadID = 0;
}

template<typename SVThreadSignalHandler>
HANDLE SVThread<SVThreadSignalHandler>::GetThreadHandle() const
{
	return m_hThread;
}

template<typename SVThreadSignalHandler>
int SVThread<SVThreadSignalHandler>::GetPriority() const
{
	return ::GetThreadPriority(m_hThread);
}

template<typename SVThreadSignalHandler>
void SVThread<SVThreadSignalHandler>::SetPriority(int priority)
{
	::SetThreadPriority(m_hThread, priority);
}

template<typename SVThreadSignalHandler>
bool SVThread<SVThreadSignalHandler>::IsActive() const
{
	bool bRetVal = m_hThread != NULL;

	if( bRetVal )
	{
		DWORD exitCode = STILL_ACTIVE;

		bRetVal = bRetVal && ( ::WaitForSingleObject( m_hThreadComplete, 0 ) == WAIT_TIMEOUT );
		bRetVal = bRetVal && ( ::GetExitCodeThread(m_hThread, &exitCode) != FALSE );
		bRetVal = bRetVal && ( exitCode == STILL_ACTIVE );
	}

	return bRetVal;
}

//******************************************************************************
//* LOG HISTORY:
//******************************************************************************
/*
$Log:   N:\PVCSarch65\ProjectFiles\archives\SVObserver_SRC\SVSystemLibrary\SVThread.inl_v  $
 * 
 *    Rev 1.1   01 Dec 2014 13:59:04   tbair
 * Project:  SVObserver
 * Change Request (SCR) nbr:  960
 * SCR Title:  Pipe/core management
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Added thread manager.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   25 Apr 2013 18:03:02   bWalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  814
 * SCR Title:  Upgrade SVObserver to Compile Using Visual Studio 2010
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Initial check in to SVObserver_src.  (Merged with svo_src label SVO 6.10 Beta 008.)
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   16 Sep 2011 15:14:48   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  730
 * SCR Title:  Adjust SVObserver to fix issues with Inspection resource handshaking
 * Checked in by:  tBair;  Tom Bair
 * Change Description:  
 *   Updated object to allow access to the thread identifier.  Added a new Event notified thread object.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   24 Feb 2010 10:09:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated code to change thread creation methodology to fix hang-ups in the thread start.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   18 Feb 2010 14:10:14   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  686
 * SCR Title:  Update Laptop version to load all valid configurations
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated thread shutdown logic to fix hang-up issues and log errors.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   19 Jan 2010 15:05:08   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  681
 * SCR Title:  Update PPQ to automatically threshold inspections when continuously NAKing
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added a parameter to the Thread process method to control the wait functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   15 Dec 2009 13:34:12   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  677
 * SCR Title:  Fix problem in camera notify thread
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Updated include information and comments.  Updated Thread creation functionality and re-create functionality.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   02 Sep 2009 15:45:28   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   File moved to this project to allow for multple project inclusion.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.7   25 Aug 2009 16:48:44   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Added new files for the new threading model.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.6   30 Jul 2009 09:41:00   jspila
 * Project:  SVObserver
 * Change Request (SCR) nbr:  665
 * SCR Title:  Fix unrecoverable failure when processing acquisitions at high speed
 * Checked in by:  Joe;  Joe Spila
 * Change Description:  
 *   Merged branced changes into current code branch with appropriate updates.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.5   24 Mar 2009 17:08:28   bwalter
 * Project:  SVObserver
 * Change Request (SCR) nbr:  656
 * SCR Title:  SVObserver may hang during shutdown when running software triggers
 * Checked in by:  bWalter;  Ben Walter
 * Change Description:  
 *   Changed Stop and ThreadProc functions to use the event m_hThreadComplete
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.4   29 Jan 2009 14:28:34   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to close the thread handle in the threadproc when it exits, to alleivate infinite loops when checking for the thread to exit.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.3   11 Nov 2008 13:51:20   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to create the thread suspended to correct a problem where the thread runs before the thread handle is assigned to the member variable.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.2   03 Oct 2008 10:21:48   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Added Get/Set Priority methods to get/set the thread priority
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.1   01 Oct 2008 15:52:28   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Revised to write to event log if creation failure.
 * Added tag to help identify usage.
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
 * 
 *    Rev 1.0   10 Sep 2008 10:33:32   sjones
 * Project:  SVObserver
 * Change Request (SCR) nbr:  612
 * SCR Title:  Implement a timer trigger object
 * Checked in by:  sJones;  Steve Jones
 * Change Description:  
 *   Initial CheckIn
 * 
 * /////////////////////////////////////////////////////////////////////////////////////
*/